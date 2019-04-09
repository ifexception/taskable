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


#include "database.h"

#include "../common/constants.h"
#include "database_exception.h"

namespace app::db
{
database::database(database&& other)
{
    pHandle = other.pHandle;
    pLogger = other.pLogger;

    other.close_handle();
    other.pLogger = nullptr;
}

database::database(const std::string& filePath)
    : pHandle(nullptr)
    , pLogger(nullptr)
{
    pLogger = spdlog::get(Constants::LoggerName);
    const permission defaultPermission = permission::ReadOnly;
    init_sqlite_connection(filePath, defaultPermission);
}

database::database(const std::string& filePath, const permission permission)
    : pHandle(nullptr)
    , pLogger(nullptr)
{
    pLogger = spdlog::get(Constants::LoggerName);
    init_sqlite_connection(filePath, permission);
}

database::~database()
{
    close_handle();
}

database& database::operator=(database&& other)
{
    if (this != &other) {
        close_handle();
        pLogger = nullptr;

        pHandle = other.pHandle;
        pLogger = other.pLogger;

        other.close_handle();
        other.pLogger = nullptr;
    }

    return *this;
}

sqlite3* database::get_handle() const
{
    return pHandle;
}

const std::string database::get_lib_version()
{
    const char* libVersion = sqlite3_libversion();
    return std::string(libVersion);
}

const int database::get_lib_version_number()
{
    const int versionNumber = sqlite3_libversion_number();
    return versionNumber;
}

void database::init_sqlite_connection(const std::string& filePath,
    const permission permission)
{
    const char* filename = filePath.c_str();
    const int flags = static_cast<int>(permission);
    const int ret = sqlite3_open_v2(filename, &pHandle, flags, nullptr);

    if (ret != SQLITE_OK) {
        const database_exception exception(pHandle, ret);
        close_handle();

        pLogger->error(Constants::Error::SqliteConnection, filePath);
        pLogger->error(Constants::Error::SqliteError, exception.get_error_code());

        throw exception;
    }
}

void database::close_handle()
{
    sqlite3_close(pHandle);
    pHandle = nullptr;
}
} // namespace app::db
