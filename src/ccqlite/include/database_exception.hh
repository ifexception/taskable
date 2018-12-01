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

#include <sqlite3.h>
#include <stdexcept>

namespace ccqlite
{
class database_exception : std::runtime_error
{
  public:
    database_exception() = delete;
    explicit database_exception(const char* errorMessage);
    explicit database_exception(int ret);
    explicit database_exception(const char* errorMessage, int ret);
    explicit database_exception(const std::string& errorMessage);
    explicit database_exception(const std::string& errorMessage, int ret);
    explicit database_exception(sqlite3* handle);
    explicit database_exception(sqlite3* handle, int ret);
    ~database_exception() = default;

    int get_error_code() const noexcept;

  private:
    int mErrorCode;
};
} // namespace ccqlite
