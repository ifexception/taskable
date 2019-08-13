//  TimesheetsTracker is a desktop that aids you in tracking your timesheets
//  and seeing what work you have done.
//
//  Copyright(C) <2018> <Szymon Welgus>
//
//  This program is free software : you can redistribute it and /
//  or modify it under the terms of the GNU General Public License as published
//  by the Free Software Foundation
//  , either version 3 of the License
//  , or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful
//  , but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <string>

namespace app::models
{
struct employer
{
    employer() = default;
    explicit employer(int employerId, std::string employerName, int dateCreatedUtc, int dateModifiedUtc, int isActive);
    ~employer() = default;

    int employer_id;
    std::string employer_name;
    int date_created_utc;
    int date_modified_utc;
    int is_active;

    static const std::string createNewEmployer;
    static const std::string getEmployers;
    static const std::string getEmployer;
    static const std::string updateEmployer;
    static const std::string deleteEmployer;
};
} // namespace app::models
