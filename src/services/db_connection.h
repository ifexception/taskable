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

#pragma once

#include <sqlite_modern_cpp.h>
namespace app::services
{
class db_connection final
{
public:
    static db_connection& get_instance();

    ~db_connection() = default;

    db_connection(const db_connection&) = delete;
    db_connection& operator=(const db_connection&) = delete;

    sqlite::database get_handle();

private:
    db_connection();

    sqlite::database mDatabase;
};
} // namespace app::services
