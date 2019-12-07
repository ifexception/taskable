//  Taskable is a desktop that aids you in tracking your timesheets
//  and seeing what work you have done.
//
//  Copyright(C) <2019> <Szymon Welgus>
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
//

#pragma once

#include <string>

namespace app::constants
{
static const std::string LoggerName = "Taskable_Daily";
static const char* LogsDirectory = "logs";
static const char* LogsFilename = "Taskable.log.txt";

static const char* TotalHours = "Total Hours %H:%M:%S";

static const int MinLength = 2;
static const int MinLength2 = 4;
static const int MaxLength = 255;
static const int MaxLength2 = 1024;

enum class RateTypes : int {
    Unknown = 1,
    Hourly = 2,
    Daily = 3,
    None = 9,
};

enum class TaskItemTypes : int {
    EntryTask = 1,
    TimedTask = 2,
};
} // namespace app::Constants
