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

#include "database_exception.hh"

namespace ccqlite
{
database_exception::database_exception(const char* errorMessage)
    : std::runtime_error(errorMessage)
    , mErrorCode(-1)
{
}
database_exception::database_exception(int ret)
    : std::runtime_error(sqlite3_errstr(ret))
    , mErrorCode(ret)
{
}

database_exception::database_exception(const char* errorMessage, int ret)
    : std::runtime_error(errorMessage)
    , mErrorCode(ret)
{
}

database_exception::database_exception(const std::string& errorMessage)
    : std::runtime_error(errorMessage)
    , mErrorCode(-1)
{
}

database_exception::database_exception(const std::string& errorMessage, int ret)
    : std::runtime_error(errorMessage)
    , mErrorCode(ret)
{
}

database_exception::database_exception(sqlite3* handle)
    : std::runtime_error(sqlite3_errmsg(handle)),
      mErrorCode(sqlite3_errcode(handle))
{
}

database_exception::database_exception(sqlite3* handle, int ret)
    : std::runtime_error(sqlite3_errmsg(handle)), mErrorCode(ret)
{
}

int database_exception::get_error_code() const noexcept
{
    return mErrorCode;
}
}

