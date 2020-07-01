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

#include "datetraverser.h"

namespace app
{
DateTraverser::DateTraverser()
    : mDaysToGoBackToMonday(-1)
    , mCurrentDate(wxDateTime::Now())
    , mMondayDate(wxDateTime::Now())
    , mTuesdayDate(wxDateTime::Now())
    , mWednesdayDate(wxDateTime::Now())
    , mThursdayDate(wxDateTime::Now())
    , mFridayDate(wxDateTime::Now())
    , mSaturdayDate(wxDateTime::Now())
    , mSundayDate(wxDateTime::Now())
{
    CalculateMondayFromCurrentDate();
    CalculateTuesdayDate();
    CalculateWednesdayDate();
    CalculateThursdayDate();
    CalculateFridayDate();
    CalculateSaturdayDate();
    CalculateSundayDate();

    mDateStrings = { mMondayDate.FormatISODate(),
        mTuesdayDate.FormatISODate(),
        mWednesdayDate.FormatISODate(),
        mThursdayDate.FormatISODate(),
        mFridayDate.FormatISODate(),
        mSaturdayDate.FormatISODate(),
        mSundayDate.FormatISODate() };
}

const wxDateTime DateTraverser::GetMondayDate()
{
    return mMondayDate;
}

const wxDateTime DateTraverser::GetTuesdayDate()
{
    return mTuesdayDate;
}

const wxDateTime DateTraverser::GetWednesdayDate()
{
    return mWednesdayDate;
}

const wxDateTime DateTraverser::GetThursdayDate()
{
    return mThursdayDate;
}

const wxDateTime DateTraverser::GetFridayDate()
{
    return mFridayDate;
}

const wxDateTime DateTraverser::GetSaturdayDate()
{
    return mSaturdayDate;
}

const wxDateTime DateTraverser::GetSundayDate()
{
    return mSundayDate;
}

const std::array<wxString, 7> DateTraverser::GetISODates()
{
    return mDateStrings;
}

const wxString DateTraverser::GetDayISODate(Days index)
{
    return mDateStrings[index];
}

void DateTraverser::CalculateDayPosition()
{
    wxDateTime::WeekDay weekDay = mCurrentDate.GetWeekDay();
    switch (weekDay) {
    case wxDateTime::Mon:
        mDaysToGoBackToMonday = 0;
        break;
    case wxDateTime::Tue:
        mDaysToGoBackToMonday = 1;
        break;
    case wxDateTime::Wed:
        mDaysToGoBackToMonday = 2;
        break;
    case wxDateTime::Thu:
        mDaysToGoBackToMonday = 3;
        break;
    case wxDateTime::Fri:
        mDaysToGoBackToMonday = 4;
        break;
    case wxDateTime::Sat:
        mDaysToGoBackToMonday = 5;
        break;
    case wxDateTime::Sun:
        mDaysToGoBackToMonday = 6;
        break;
    case wxDateTime::Inv_WeekDay:
    default:
        break;
    }
}

void DateTraverser::CalculateMondayFromCurrentDate()
{
    CalculateDayPosition();

    wxDateSpan toGoBackToMonday(0, 0, 0, mDaysToGoBackToMonday);
    mMondayDate.Subtract(toGoBackToMonday);
}

void DateTraverser::CalculateTuesdayDate()
{
    wxDateSpan dateSpan(0, 0, 0, 1);
    mTuesdayDate = mMondayDate.Add(dateSpan);
}

void DateTraverser::CalculateWednesdayDate()
{
    wxDateSpan dateSpan(0, 0, 0, 1);
    mWednesdayDate = mTuesdayDate.Add(dateSpan);
}

void DateTraverser::CalculateThursdayDate()
{
    wxDateSpan dateSpan(0, 0, 0, 1);
    mThursdayDate = mWednesdayDate.Add(dateSpan);
}

void DateTraverser::CalculateFridayDate()
{
    wxDateSpan dateSpan(0, 0, 0, 1);
    mFridayDate = mThursdayDate.Add(dateSpan);
}

void DateTraverser::CalculateSaturdayDate()
{
    wxDateSpan dateSpan(0, 0, 0, 1);
    mSaturdayDate = mFridayDate.Add(dateSpan);
}

void DateTraverser::CalculateSundayDate()
{
    wxDateSpan dateSpan(0, 0, 0, 1);
    mSundayDate = mSaturdayDate.Add(dateSpan);
}
} // namespace app
