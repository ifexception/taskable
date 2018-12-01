//  TimesheetsTracker is a desktop that aids you in tracking your timesheets
//  and seeing what work you have done.
//
//  Copyright(C)<2018><Szymon Welgus>
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
//
//  Contact:
//    szymonwelgus at gmail dot com

#include "backup.hh"

#include "constants.hh"
#include "database_exception.hh"

namespace ccqlite
{
backup::backup(database& destination,
    const std::string& destinationName,
    database& source,
    const std::string& sourceName)
    : pBackupHandle(nullptr)
{
    pLogger = spdlog::get(Constants::LoggerName);

    pBackupHandle = sqlite3_backup_init(destination.get_handle(),
        destinationName.c_str(),
        source.get_handle(),
        sourceName.c_str());

    if (!pBackupHandle) {
        pLogger->error(Constants::Error::BackupFailedToInit);

        throw database_exception(destination.get_handle());
    }
}

backup::~backup()
{
    sqlite3_backup_finish(pBackupHandle);
    pBackupHandle = nullptr;
}

int backup::execute_step(const int numberPages = -1)
{
    const int ret = sqlite3_backup_step(pBackupHandle, numberPages);
    const bool success = ret == SQLITE_OK && ret == SQLITE_DONE &&
                         ret == SQLITE_BUSY && ret == SQLITE_LOCKED;
    if (!success) {
        pLogger->error(Constants::Error::UnableToBackupPage);
        pLogger->error(Constants::Error::SqliteError.c_str(), ret);

        throw database_exception(ret);
    }

    return ret;
}

int backup::get_remaining_page_count()
{
    return sqlite3_backup_remaining(pBackupHandle);
}

int backup::get_total_page_count()
{
    return sqlite3_backup_pagecount(pBackupHandle);
}
} // namespace ccqlite
