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

#include <sqlite3.h>

#include "../common/constants.hh"
#include "database.hh"
#include "database_exception.hh"

namespace app::db
{
statement::statement(const database& db, const std::string& query)
    : pStatement(nullptr)
    , mQuery(query)
    , bHasRow(false)
    , bIsDone(false)
{
    pLogger = spdlog::get(Constants::LoggerName);

    const int ret = sqlite3_prepare_v2(db.get_handle(),
        mQuery.c_str(),
        mQuery.size(),
        &pStatement,
        nullptr);

    if (ret != SQLITE_OK) {
        pLogger->error(Constants::Error::StatementInitialization);
        pLogger->error(Constants::Error::SqliteError.c_str(), ret);

        throw database_exception(db.get_handle(), ret);
    }
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
    copy_options copy)
{
    int ret;
    if (copy == copy_options::Copy) {
        ret = sqlite3_bind_text(pStatement, index, value.c_str(), value.size(),
            SQLITE_TRANSIENT);
    } else {
        ret = sqlite3_bind_text(pStatement, index, value.c_str(), value.size(),
            SQLITE_STATIC);
    }

    check(ret);
}

void statement::bind(const int index, const void* value, int size,
    copy_options copy)
{
    int ret;
    if (copy == copy_options::Copy) {
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
    int index = sqlite3_bind_parameter_index(pStatement, name.c_str());
    bind(index, value);
}

void statement::bind(const std::string& name, const unsigned int value)
{
    int index = sqlite3_bind_parameter_index(pStatement, name.c_str());
    bind(index, value);
}

void statement::bind(const std::string& name, const long value)
{
    int index = sqlite3_bind_parameter_index(pStatement, name.c_str());
    bind(index, value);
}

void statement::bind(const std::string& name, const long long value)
{
    int index = sqlite3_bind_parameter_index(pStatement, name.c_str());
    bind(index, value);
}

void statement::bind(const std::string& name, const double value)
{
    int index = sqlite3_bind_parameter_index(pStatement, name.c_str());
    bind(index, value);
}

void statement::bind(const std::string& name, const std::string& value,
    copy_options copy)
{
    int index = sqlite3_bind_parameter_index(pStatement, name.c_str());
    bind(index, value, copy);
}

void statement::bind(const std::string& name, const void* value, int size,
    copy_options copy)
{
    int index = sqlite3_bind_parameter_index(pStatement, name.c_str());
    bind(index, value, size, copy);
}

void statement::bind(const std::string& name, const void* null = nullptr)
{
    int index = sqlite3_bind_parameter_index(pStatement, name.c_str());
    bind(index);
}

bool statement::run()
{
    const int ret = step();

    if ((ret != SQLITE_ROW) && (ret != SQLITE_DONE))
    {
        pLogger->error(Constants::Error::StatementExecution.c_str(), mQuery);
        pLogger->error(Constants::Error::SqliteError.c_str(), ret);

        throw database_exception(ret);
    }

    return has_row();
}

bool statement::has_row() const
{
    return bHasRow;
}

bool statement::is_done() const
{
    return bIsDone;
}

void statement::check(const int returnCode)
{
    if (returnCode != SQLITE_OK) {
        pLogger->error(Constants::Error::SqliteError.c_str(), returnCode);
        throw database_exception(returnCode);
    }
}

void statement::check_row() const
{
    if (bHasRow == false)
    {
        pLogger->error("Now to get a column from. run() was not called, or returned false");
        throw database_exception("No row to get a column from. run() was not called, or returned false.");
    }
}

int statement::step()
{
    if (bIsDone == false)
    {
        const int ret = sqlite3_step(pStatement);
        if (ret == SQLITE_ROW)
        {
            bHasRow = true;
        }
        else if (ret == SQLITE_DONE)
        {
            bHasRow = false;
            bIsDone = true;
        }
        else
        {
            bHasRow = false;
            bIsDone = false;
        }

        return ret;
    }
    else
    {
        return SQLITE_MISUSE;
    }
}

} // namespace app::db
