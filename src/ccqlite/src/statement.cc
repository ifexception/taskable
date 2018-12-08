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

#include "statement.hh"

#define FMT_HEADER_ONLY
#include <spdlog/spdlog.h>

#include <sqlite3.h>

#include "constants.hh"
#include "copy_semantic.hh"
#include "database.hh"
#include "database_exception.hh"

namespace ccqlite
{
statement::statement(const database& db, const std::string& query)
    : pStatement(nullptr), mColumnCount(0), mQuery(query), bHasRow(false),
      bIsDone(false)
{
    pLogger = spdlog::get(Constants::LoggerName);

    const int ret = sqlite3_prepare_v2(db.get_handle(), mQuery.c_str(),
                                       mQuery.size(), &pStatement, nullptr);

    if (ret != SQLITE_OK) {
        throw database_exception(db.get_handle(), ret);
    }

    mColumnCount = sqlite3_column_count(pStatement);
}

statement::~statement()
{
    sqlite3_finalize(pStatement);
}

void statement::reset()
{
    bHasRow = false;
    bIsDone = false;
    const int ret = sqlite3_reset(pStatement);
    check(ret);
}

void statement::clear()
{
    const int ret = sqlite3_clear_bindings(pStatement);
    check(ret);
}

void statement::bind(const int index, const int value)
{
    const int ret = sqlite3_bind_int(pStatement, index, value);
    check(ret);
}

void statement::bind(const int index, const unsigned int value)
{
    const int ret = sqlite3_bind_int64(pStatement, index, value);
    check(ret);
}

void statement::bind(const int index, const long value)
{
    const int ret = sqlite3_bind_int64(pStatement, index, value);
    check(ret);
}

void statement::bind(const int index, const long long value)
{
    const int ret = sqlite3_bind_int64(pStatement, index, value);
    check(ret);
}

void statement::bind(const int index, const double value)
{
    const int ret = sqlite3_bind_double(pStatement, index, value);
    check(ret);
}

void statement::bind(const int index, const std::string& value,
    copy_semantic copy)
{
    int ret;
    if (copy == copy_semantic::Copy) {
        ret = sqlite3_bind_text(pStatement, index, value.c_str(), value.size(),
            SQLITE_TRANSIENT);
    } else {
        ret = sqlite3_bind_text(pStatement, index, value.c_str(), value.size(),
            SQLITE_STATIC);
    }

    check(ret);
}

void statement::bind(const int index, const void* value, int size,
    copy_semantic copy)
{
    int ret;
    if (copy == copy_semantic::Copy) {
        ret = sqlite3_bind_blob(pStatement, index, value, size,
            SQLITE_TRANSIENT);
    } else {
        ret = sqlite3_bind_blob(pStatement, index, value, size,
            SQLITE_STATIC);
    }

    check(ret);
}

void statement::bind(const int index, const void* null = nullptr)
{
    const int ret = sqlite3_bind_null(pStatement, index);
    check(ret);
}

void statement::bind(const std::string& name, const int value)
{
}

void statement::bind(const std::string& name, const unsigned int value)
{
}

void statement::bind(const std::string& name, const long value)
{
}

void statement::bind(const std::string& name, const long long value)
{
}

void statement::bind(const std::string& name, const double value)
{
}

void statement::bind(const std::string& name, const std::string& value,
    copy_semantic copy)
{
}

void statement::bind(const std::string& name, const void* value, int size,
    copy_semantic copy)
{
}

void statement::bind(const std::string& name)
{
}

bool statement::has_row() const
{
    return false;
}

bool statement::is_done() const
{
    return false;
}

bool statement::is_value_null(const int index) const
{
    return false;
}

bool statement::is_value_null(const std::string& name) const
{
    return false;
}

void statement::check(const int returnCode)
{
    if (returnCode != SQLITE_OK) {
        throw database_exception(returnCode);
    }
}

void statement::check_row() const
{
}

void statement::check_index() const
{
}

} // namespace ccqlite
