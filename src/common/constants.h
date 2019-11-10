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

namespace app::Constants
{
static const std::string LoggerName = "Taskable_Daily";
static const char* LogsDirectory = "logs";
static const char* LogsFilename = "Taskable.log.txt";

static const char* TotalHours = "Total Hours %H:%M:%S";

static const int MinLength = 2;
static const int MaxLength = 255;

namespace Messages
{
static const char* SelectionRequired = "A %s selection is required!";
static const char* IsEmpty = "%s is required. Please provide a value";
static const char* TooShort = "%s provided is too short. Minimum length is %d";
static const char* TooLong = "%s provided is too long. Maximum length is %d";
} // namespace Messages

namespace Info
{
static std::string LoggerInitialized = "Logger initialized.";
} // namespace Info
} // namespace app::Constants
