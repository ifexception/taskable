// Productivity tool to help you track the time you spend on tasks
// Copyright (C) 2023  Szymon Welgus
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

#include <algorithm>

namespace app
{
DateTraverser::DateTraverser()
    : mDaysToGoBackToMonday(-1)
    , mCurrentDate(wxDateTime::Now())
{
    std::fill(mDates.begin(), mDates.end(), wxDateTime::Now());

    CalculateMondayFromCurrentDate();
    CalculateTuesdayDate();
    CalculateWednesdayDate();
    CalculateThursdayDate();
    CalculateFridayDate();
    CalculateSaturdayDate();
    CalculateSundayDate();
}

void DateTraverser::Recalculate(wxDateTime newDate)
{
    std::fill(mDates.begin(), mDates.end(), wxDefaultDateTime);
    std::fill(mDateStrings.begin(), mDateStrings.end(), wxGetEmptyString());

    mCurrentDate = newDate;

    std::fill(mDates.begin(), mDates.end(), newDate);

    CalculateMondayFromCurrentDate();
    CalculateTuesdayDate();
    CalculateWednesdayDate();
    CalculateThursdayDate();
    CalculateFridayDate();
    CalculateSaturdayDate();
    CalculateSundayDate();
}

const wxDateTime DateTraverser::GetMondayDate()
{
    return mDates[constants::Days::Monday];
}

const wxDateTime DateTraverser::GetTuesdayDate()
{
    return mDates[constants::Days::Tuesday];
}

const wxDateTime DateTraverser::GetWednesdayDate()
{
    return mDates[constants::Days::Wednesday];
}

const wxDateTime DateTraverser::GetThursdayDate()
{
    return mDates[constants::Days::Thursday];
}

const wxDateTime DateTraverser::GetFridayDate()
{
    return mDates[constants::Days::Friday];
}

const wxDateTime DateTraverser::GetSaturdayDate()
{
    return mDates[constants::Days::Saturday];
}

const wxDateTime DateTraverser::GetSundayDate()
{
    return mDates[constants::Days::Sunday];
}

const std::array<wxString, 7> DateTraverser::GetISODates()
{
    return mDateStrings;
}

const wxDateTime DateTraverser::GetDayDate(constants::Days index)
{
    return mDates[index];
}

const wxString DateTraverser::GetDayISODate(constants::Days index)
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
    mDates[constants::Days::Monday] = mDates[constants::Days::Monday].Subtract(toGoBackToMonday);
    mDateStrings[constants::Days::Monday] = mDates[constants::Days::Monday].FormatISODate();
}

void DateTraverser::CalculateTuesdayDate()
{
    wxDateSpan dateSpan(0, 0, 0, 1);
    mDates[constants::Days::Tuesday] = mDates[constants::Days::Monday].Add(dateSpan);
    mDateStrings[constants::Days::Tuesday] = mDates[constants::Days::Tuesday].FormatISODate();
}

void DateTraverser::CalculateWednesdayDate()
{
    wxDateSpan dateSpan(0, 0, 0, 1);
    mDates[constants::Days::Wednesday] = mDates[constants::Days::Tuesday].Add(dateSpan);
    mDateStrings[constants::Days::Wednesday] = mDates[constants::Days::Wednesday].FormatISODate();
}

void DateTraverser::CalculateThursdayDate()
{
    wxDateSpan dateSpan(0, 0, 0, 1);
    mDates[constants::Days::Thursday] = mDates[constants::Days::Wednesday].Add(dateSpan);
    mDateStrings[constants::Days::Thursday] = mDates[constants::Days::Thursday].FormatISODate();
}

void DateTraverser::CalculateFridayDate()
{
    wxDateSpan dateSpan(0, 0, 0, 1);
    mDates[constants::Days::Friday] = mDates[constants::Days::Thursday].Add(dateSpan);
    mDateStrings[constants::Days::Friday] = mDates[constants::Days::Friday].FormatISODate();
}

void DateTraverser::CalculateSaturdayDate()
{
    wxDateSpan dateSpan(0, 0, 0, 1);
    mDates[constants::Days::Saturday] = mDates[constants::Days::Friday].Add(dateSpan);
    mDateStrings[constants::Days::Saturday] = mDates[constants::Days::Saturday].FormatISODate();
}

void DateTraverser::CalculateSundayDate()
{
    wxDateSpan dateSpan(0, 0, 0, 1);
    mDates[constants::Days::Sunday] = mDates[constants::Days::Saturday].Add(dateSpan);
    mDateStrings[constants::Days::Sunday] = mDates[constants::Days::Sunday].FormatISODate();
}
} // namespace app
