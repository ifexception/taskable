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

#include "connectionprovider.h"

namespace app::db
{
ConnectionProvider& ConnectionProvider::Get()
{
    static ConnectionProvider instance;
    return instance;
}

/*
 This function should only ever be called once as the connection pool cannot be re-initialized once the
 connection pool is set
 */
void ConnectionProvider::InitializeConnectionPool(std::unique_ptr<ConnectionPool<SqliteConnection>> connectionPool)
{
    if (!bInitialized) {
        pConnectionPool = std::move(connectionPool);
        bInitialized = true;
    }
}

void ConnectionProvider::ReInitializeConnectionPool(std::unique_ptr<ConnectionPool<SqliteConnection>> newConnectionPool)
{
    pConnectionPool.reset();
    pConnectionPool = std::move(newConnectionPool);
}

void ConnectionProvider::PurgeConnectionPool()
{
    if (pConnectionPool != nullptr) {
        pConnectionPool.reset();
    }
}

ConnectionPool<SqliteConnection>* ConnectionProvider::Handle()
{
    return pConnectionPool.get();
}

ConnectionProvider::ConnectionProvider()
    : pConnectionPool(nullptr)
    , bInitialized(false)
{
}
} // namespace app::db
