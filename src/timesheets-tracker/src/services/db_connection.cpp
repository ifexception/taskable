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

#include "db_connection.h"
#include "../common/config.h"

namespace app::services
{
db_connection& db_connection::get_instance()
{
    static db_connection instance;
    return instance;
}

db_connection::~db_connection()
{
    delete pDatabase;
}

db::database* db_connection::get_database()
{
    return pDatabase;
}

db_connection::db_connection()
    : mPermission(db::permission::ReadWrite)
{
    auto connectionString = cfg::config::get_instance().get_connection_string();
    pDatabase = new db::database(connectionString, mPermission);
}
}
