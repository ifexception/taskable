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


#include "backup.hh"

#include "../common/constants.hh"
#include "database.hh"
#include "database_exception.hh"

namespace app::db
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
        pLogger->error(Constants::Error::BackupInitialization);

        throw database_exception(destination.get_handle());
    }
}

backup::~backup()
{
    sqlite3_backup_finish(pBackupHandle);
    pBackupHandle = nullptr;
}

void backup::execute_step(const int numberPages = -1)
{
    const int ret = sqlite3_backup_step(pBackupHandle, numberPages);
    const bool success = ret == SQLITE_OK && ret == SQLITE_DONE &&
                         ret == SQLITE_BUSY && ret == SQLITE_LOCKED;
    if (!success) {
        pLogger->error(Constants::Error::UnableToBackupPage);
        pLogger->error(Constants::Error::SqliteError, ret);

        throw database_exception(ret);
    }
}

int backup::get_remaining_page_count()
{
    const int remainingPageCount = sqlite3_backup_remaining(pBackupHandle);
    return remainingPageCount;
}

int backup::get_total_page_count()
{
    return sqlite3_backup_pagecount(pBackupHandle);
}
} // namespace app::db
