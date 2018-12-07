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
//


#pragma once

#include <string>

namespace ccqlite::Constants
{
static const std::string LoggerName = "ccqlite_logger";

namespace Info
{
static std::string LoggerInitialized = "Logger initialized...";
} // Info

namespace Error
{
static std::string SqliteError = "Error was: {0}";

static std::string SqliteConnection =
    "Failed to connect to sqlite instance: {0}";

static std::string BackupFailedToInit =
    "Error initializing sqlite backup object";

static std::string UnableToBackupPage = "Error while backing up";
} // namespace Error
} // namespace ccqlite::Constants
