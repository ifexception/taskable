// Productivity tool to help you track the time you spend on tasks
// Copyright (C) 2020  Szymon Welgus
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
//  Contact:
//    szymonwelgus at gmail dot com

#include "outlookintegrator.h"

#ifdef __WINDOWS__
BOOL IsElevated()
{
    BOOL bRet = FALSE;
    HANDLE hToken = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION Elevation;
        DWORD cbSize = sizeof(TOKEN_ELEVATION);
        if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
            bRet = Elevation.TokenIsElevated;
        }
    }
    if (hToken) {
        CloseHandle(hToken);
    }
    return bRet;
}
#endif // __WINDOWS__

namespace app::svc
{
const wxString OutlookName = wxT("Outlook.Application");

OutlookIntegrator::OutlookIntegrator()
    : mOutlookApplication()
    , mMeetingsList()
{
}

bool OutlookIntegrator::TryGetOutlookInstance()
{
    mOutlookApplication.SetLCID(MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT));
    if (!mOutlookApplication.GetInstance(OutlookName)) {
        if (!mOutlookApplication.CreateInstance(OutlookName)) {
            // log error
            return false;
        }
    }
    return true;
}

bool OutlookIntegrator::Execute()
{
    return IterateAndGetCalendarMeetings();
}

std::vector<Meeting*> OutlookIntegrator::GetMeetings()
{
    return mMeetingsList;
}

bool OutlookIntegrator::IterateAndGetCalendarMeetings()
{
    wxVariant mAPI("MAPI");
    const wxVariant namespaceDispatchPtr = mOutlookApplication.CallMethod("GetNamespace", mAPI);

    if (namespaceDispatchPtr.IsNull()) {
        wxLogError("GetNamespace call failed.");
        return false;
    }

    wxAutomationObject namespaceObject;
    namespaceObject.SetDispatchPtr(namespaceDispatchPtr.GetVoidPtr());

    const wxVariant missingArgument(new wxVariantDataErrorCode(DISP_E_PARAMNOTFOUND));
    namespaceObject.CallMethod("Logon", missingArgument, missingArgument, true, true);

    const wxVariant accountsDispatchPtr = namespaceObject.GetProperty("Accounts");
    if (accountsDispatchPtr.IsNull()) {
        wxLogError("Failed to get \"Accounts\" property");
        return false;
    }

    wxAutomationObject accountsObject;
    accountsObject.SetDispatchPtr(accountsDispatchPtr.GetVoidPtr());

    const wxVariant accountCountProperty = accountsObject.GetProperty("Count");
    if (accountCountProperty.IsNull()) {
        wxLogError("Failed to get \"Count\" property");
        return false;
    }

    if (!(accountCountProperty.IsType("long") && accountCountProperty.GetLong() > 0)) {
        return false;
    }

    const long accountCount = accountCountProperty.GetLong();
    for (long i = 1; i <= accountCount; ++i) {
        wxVariant indexParam = i;
        const wxVariant accountDispatchPtr = accountsObject.CallMethod("Item", indexParam);
        if (accountDispatchPtr.IsNull()) {
            wxLogError("Failed to call \"Item\" with index %ld", i);
            return false;
        }

        wxAutomationObject accountObject;
        accountObject.SetDispatchPtr(accountDispatchPtr.GetVoidPtr());

        const wxVariant displayName = accountObject.GetProperty("DisplayName");
        if (displayName.IsNull() || !displayName.IsType("string")) {
            // log error
            return false;
        }

        const wxVariant deliveryStoreDispatchPtr = accountObject.GetProperty("DeliveryStore");
        if (deliveryStoreDispatchPtr.IsNull() || !deliveryStoreDispatchPtr.IsType("void*")) {
            wxLogError("Failed to get \"DeliveryStore\" property");
            return false;
        }

        wxAutomationObject deliveryStoreObject;
        deliveryStoreObject.SetDispatchPtr(deliveryStoreDispatchPtr.GetVoidPtr());

        wxVariant calendarFolderParam = 9; // olFolderCalendar
        wxVariant calendarFolderDispatchPtr = deliveryStoreObject.CallMethod("GetDefaultFolder", calendarFolderParam);
        if (calendarFolderDispatchPtr.IsNull() || !calendarFolderDispatchPtr.IsType("void*")) {
            wxLogError("Failed to call \"GetDefaultFolder\" with %d parameter", calendarFolderParam.GetInteger());
            return false;
        }

        wxAutomationObject calendarFolderObject;
        calendarFolderObject.SetDispatchPtr(calendarFolderDispatchPtr);

        wxAutomationObject calendarFolderItemsObject;
        if (!calendarFolderObject.GetObject(calendarFolderItemsObject, "Items")) {
            wxLogError("Failed to get \"Items\" object");
            return false;
        }

        wxVariant includeRecurrencesParam(true);
        if (!calendarFolderItemsObject.PutProperty("IncludeRecurrences", includeRecurrencesParam)) {
            wxLogError("Failed to set \"IncludeRecurrences\" property");
            return false;
        }

        wxVariant sortByStartParam("[Start]");
        calendarFolderItemsObject.CallMethod("Sort", sortByStartParam);

        wxDateTime today = wxDateTime::Now();
        wxString todayStr = today.Format("%Y/%m/%d");
        wxString strRestriction = wxString::Format("[Start] <= '%s 23:59' AND [End] >= '%s 00:00'", todayStr, todayStr);

        wxVariant restrictionParam(strRestriction);
        const wxVariant filteredItemsDispatchPtr = calendarFolderItemsObject.CallMethod("Restrict", restrictionParam);
        if (filteredItemsDispatchPtr.IsNull() || !filteredItemsDispatchPtr.IsType("void*")) {
            wxLogError("Failed to call \"Restrict\" method with %s", strRestriction);
            return false;
        }

        wxAutomationObject filteredItemsObject;
        filteredItemsObject.SetDispatchPtr(filteredItemsDispatchPtr);

        wxVariant itemObjectDispatchPtr = filteredItemsObject.CallMethod("GetFirst");
        if (itemObjectDispatchPtr.IsNull() || !itemObjectDispatchPtr.IsType("void*")) {
            wxLogError("Error calling \"GetFirst\" method");
            return false;
        }

        wxAutomationObject itemObject;
        itemObject.SetDispatchPtr(itemObjectDispatchPtr);

        do {
            if (!itemObject.IsOk()) {
                break;
            }

            Meeting* meeting = new Meeting;

            wxVariant subjectProperty = itemObject.GetProperty("Subject");
            if (!subjectProperty.IsNull()) {
                meeting->Subject = subjectProperty.GetString();
            }
            wxVariant bodyProperty = itemObject.GetProperty("Body");
            if (!bodyProperty.IsNull()) {
                meeting->Body = bodyProperty.GetString();
            }
            wxVariant startProperty = itemObject.GetProperty("Start");
            if (!startProperty.IsNull()) {
                meeting->Start = startProperty.GetString();
            }
            wxVariant endProperty = itemObject.GetProperty("End");
            if (!endProperty.IsNull()) {
                meeting->End = endProperty.GetString();
            }
            wxVariant durationProperty = itemObject.GetProperty("Duration");
            if (!durationProperty.IsNull()) {
                meeting->Duration = durationProperty.GetInteger();
            }
            wxVariant locationProperty = itemObject.GetProperty("Location");
            if (!locationProperty.IsNull()) {
                meeting->Location = locationProperty.GetString();
            }

            mMeetingsList.push_back(meeting);

            itemObjectDispatchPtr = filteredItemsObject.CallMethod("GetNext");
            if (itemObjectDispatchPtr.IsNull() || !itemObjectDispatchPtr.IsType("void*")) {
                // log error
                break;
            }

            itemObject.SetDispatchPtr(itemObjectDispatchPtr);

        } while (itemObject.IsOk());
    }

    return true;
}
} // namespace app::svc
