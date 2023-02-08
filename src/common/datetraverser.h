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

#pragma once

#include <array>

#include <wx/datetime.h>
#include <wx/string.h>

#include "constants.h"

namespace app
{
class DateTraverser final
{
public:
    DateTraverser();
    ~DateTraverser() = default;

    void Recalculate(wxDateTime newDate);

    const wxDateTime GetMondayDate();
    const wxDateTime GetTuesdayDate();
    const wxDateTime GetWednesdayDate();
    const wxDateTime GetThursdayDate();
    const wxDateTime GetFridayDate();
    const wxDateTime GetSaturdayDate();
    const wxDateTime GetSundayDate();

    const std::array<wxString, 7> GetISODates();

    const wxDateTime GetDayDate(constants::Days index);
    const wxString GetDayISODate(constants::Days index);

private:
    void CalculateDayPosition();
    void CalculateMondayFromCurrentDate();
    void CalculateTuesdayDate();
    void CalculateWednesdayDate();
    void CalculateThursdayDate();
    void CalculateFridayDate();
    void CalculateSaturdayDate();
    void CalculateSundayDate();

    int mDaysToGoBackToMonday;
    wxDateTime mCurrentDate;

    std::array<wxDateTime, 7> mDates;
    std::array<wxString, 7> mDateStrings;
};
} // namespace app
