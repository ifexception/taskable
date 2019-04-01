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

#define FMT_HEADER_ONLY
#include <spdlog/spdlog.h>

#include "copy_options.hh"

struct sqlite3_stmt;

namespace app::db
{
class database;

class statement
{
  public:
    statement() = delete;
    statement(const statement&) = delete;
    explicit statement(const database& db, const std::string& query);
    virtual ~statement();

    statement& operator=(const statement&) = delete;

    void reset();
    void clear();

    void bind(int index, const int value);
    void bind(int index, const unsigned int value);
    void bind(int index, const long value);
    void bind(int index, const long long value);
    void bind(int index, const double value);
    void bind(int index, const std::string& value, copy_options copy);
    void bind(int index, const void* value, int size, copy_options copy);
    void bind(int index, const void* null);

    void bind(const std::string& name, const int value);
    void bind(const std::string& name, const unsigned int value);
    void bind(const std::string& name, const long value);
    void bind(const std::string& name, const long long value);
    void bind(const std::string& name, const double value);
    void bind(const std::string& name, const std::string& value, copy_options copy);
    void bind(const std::string& name, const void* value, int size, copy_options copy);
    void bind(const std::string& name, const void* null);

    bool run();

    bool has_row() const;
    bool is_done() const;


    sqlite3_stmt* pStatement;

  private:
    void check(const int returnCode);
    void check_row() const;
    int step();

    std::shared_ptr<spdlog::logger> pLogger;

    std::string mQuery;
    bool bHasRow;
    bool bIsDone;
};
} // namespace app::db
