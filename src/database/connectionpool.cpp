// Productivity tool to help you track the time you spend on tasks
// Copyright (C) 2020  Szymon Welgus
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
//  Contact:
//    szymonwelgus at gmail dot com

#include "connectionpool.h"

namespace app::db
{
const int ConnectionPool::PoolSize = 4;

ConnectionPool& ConnectionPool::Get()
{
    static ConnectionPool instance;
    return instance;
}

sqlite::database* ConnectionPool::Acquire(std::string databasePath)
{
    if (mConnectionPool.empty()) {
        auto config = sqlite::sqlite_config{ sqlite::OpenFlags::READWRITE, nullptr, sqlite::Encoding::UTF8 };
        return new sqlite::database(databasePath, config);
    } else {
        if (mAquiredConnections > 4) {
            return nullptr; // max pool size reached
        }
        auto connection = mConnectionPool.front();
        mConnectionPool.pop_front();
        return connection;
    }
    mAquiredConnections++;
}

void ConnectionPool::Release(sqlite::database* connection)
{
    mConnectionPool.push_back(connection);
    mAquiredConnections--;
}

ConnectionPool::ConnectionPool()
    : mAquiredConnections(0)
{
}
} // namespace app::svc
