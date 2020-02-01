//  Taskable is a desktop that aids you in tracking your timesheets
//  and seeing what work you have done.
//
//  Copyright(C)<2019><Szymon Welgus>
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

#include "databaseconnection.h"

namespace app::svc
{
DatabaseConnection& DatabaseConnection::Get()
{
    static DatabaseConnection instance;
    return instance;
}

sqlite::database* DatabaseConnection::GetHandle()
{
    return pDatabase;
}

void DatabaseConnection::SetHandle(sqlite::database* database)
{
    pDatabase = database;
}

void DatabaseConnection::ResetHandle(sqlite::database* database)
{
    pDatabase = nullptr;
    pDatabase = database;
}

void DatabaseConnection::UnsetHandle()
{
    pDatabase = nullptr;
}

DatabaseConnection::DatabaseConnection()
    : pDatabase(nullptr)
{}
} // namespace app::svc
