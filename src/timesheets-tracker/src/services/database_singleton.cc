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

#include "database_singleton.hh"
#include "../common/config.hh"

namespace app::services
{
database_singleton& database_singleton::get_instance()
{
    static database_singleton instance;
    return instance;
}

const ccqlite::database* database_singleton::get_database()
{
    return pDatabase;
}

database_singleton::database_singleton()
    : mPermission(ccqlite::permission::CreateReadWrite)
{
    pDatabase =
        new ccqlite::database(cfg::config::get_instance().get_connection_string(), mPermission);
}
}
