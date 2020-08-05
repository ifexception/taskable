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

#pragma once

#include <map>

#include <wx/wx.h>
#include <wx/msw/ole/automtn.h>
#include <wx/msw/ole/oleutils.h>

#include "../models/meetingmodel.h"

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
class OutlookIntegrator
{
public:
    OutlookIntegrator();
    ~OutlookIntegrator() = default;

    bool TryGetOutlookInstance();

    bool Execute();

private:
    bool IterateAndGetCalendarMeetings();

    wxAutomationObject mOutlookApplication;

    std::map<wxString, model::MeetingModel> mMeetingsMap;
};
}
