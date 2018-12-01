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

#pragma once

#include <string>

#include <sqlite3.h>

#define FMT_HEADER_ONLY
#include <spdlog/spdlog.h>
#include <spdlog/sinks/daily_file_sink.h>

#include "ccqliteapi.hh"
#include "permission.hh"

namespace ccqlite
{
class CCQLITE_API database
{
  public:
    database() = delete;
    database(const database& other) = delete;
    database(database&& other);
    explicit database(const std::string filePath);
    explicit database(const std::string filePath, const permission permission);
    ~database();

    database& operator=(const database& other) = delete;
    database& operator=(database&& other);

    sqlite3* get_handle() const;
    const std::string get_lib_version();
    const int get_lib_version_number();

  private:
    void init_logging();
    void init_sqlite_connection(const std::string filePath,
        const permission permission);
    void close_handle();

    sqlite3* pHandle;

#pragma warning(suppress : 4251) // pLogger is not exportable
    std::shared_ptr<spdlog::logger> pLogger;
};
} // namespace ccqlite
