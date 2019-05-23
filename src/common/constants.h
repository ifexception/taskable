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

namespace app::Constants
{
static const std::string LoggerName = "TimesheetsTracker_Daily";
static const char* LogsDirectory = "logs";
static const char* LogsFilename = "TimesheetsTracker.log.txt";

namespace Info
{
static std::string LoggerInitialized = "Logger initialized.";
} // Info

namespace Error
{
static const char* SqliteError = "Error sqlite code: {0}";

static const char* SqliteConnection = "Failed to connect to sqlite instance: {0}";

static const char* StatementExecution = "Error while executing query: {0}";

// std::string messages
static std::string BackupInitialization = "Error initializing sqlite backup handle";

static std::string UnableToBackupPage = "Error while backing up";

static std::string StatementInitialization = "Error initializing sqlite statement handle";

static std::string NoRowToGet = "No row to get a column from. Run() was not called, or returned false.";

} // namespace Error
} // namespace app::db::Constants
