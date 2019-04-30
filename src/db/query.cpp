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

#include "query.h"

#include <sqlite3.h>

#include "database.h"

namespace app::db
{
query::query(database& db, const std::string& query)
    : statement(db, query)
{}

int query::column_count() const
{
    const int columnCount = sqlite3_column_count(pStatement);
    return columnCount;
}

std::string query::column_name(int index) const
{
    const char* name = sqlite3_column_name(pStatement, index);
    return std::string(name);
}

std::string query::column_decltype(int index) const
{
    const char* type = sqlite3_column_decltype(pStatement, index);
    return std::string(type);
}
}
