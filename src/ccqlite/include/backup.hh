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
#include <memory>

#include <sqlite3.h>

#define FMT_HEADER_ONLY
#include <spdlog/spdlog.h>

#include "ccqliteapi.hh"

namespace ccqlite
{
class database;

class CCQLITE_API backup
{
  public:
    backup() = delete;
    explicit backup(const backup&) = delete;
    explicit backup(database& destination, const std::string& destinationName,
                    database& source, const std::string& sourceName);
    ~backup();

    backup& operator=(const backup&) = delete;

    void execute_step(const int numberPages);

    int get_remaining_page_count();
    int get_total_page_count();

  private:
    sqlite3_backup* pBackupHandle;

#pragma warning(suppress : 4251) // pLogger is not exportable
    std::shared_ptr<spdlog::logger> pLogger;
};
} // namespace ccqlite
