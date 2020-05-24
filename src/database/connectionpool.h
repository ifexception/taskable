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

#pragma once

#include <deque>
#include <memory>
#include <string>

#include <sqlite_modern_cpp.h>
#include "connection.h"
#include "connectionfactory.h"

namespace app::db
{
template<class T>
class ConnectionPool final
{
public:
    ConnectionPool() = delete;
    ConnectionPool(std::shared_ptr<IConnectionFactory> factory, std::size_t poolSize);
    ~ConnectionPool();

    std::shared_ptr<T> Acquire();
    void Release(std::shared_ptr<T> connection);

private:
    std::size_t mPoolSize;
    std::shared_ptr<IConnectionFactory> pFactory;
    std::deque<std::shared_ptr<IConnection>> mPool;
};

template<class T>
inline ConnectionPool<T>::ConnectionPool(std::shared_ptr<IConnectionFactory> factory, std::size_t poolSize)
    : pFactory(factory)
    , mPoolSize(poolSize)
    , mPool()
{
    while (mPool.size() < mPoolSize) {
        mPool.push_back(pFactory->Create());
    }
}

template<class T>
inline ConnectionPool<T>::~ConnectionPool()
{
    mPool.clear();
}

template<class T>
inline std::shared_ptr<T> ConnectionPool<T>::Acquire()
{
    if (mPool.size() == 0) {
        auto connection = pFactory->Create();
        return std::dynamic_pointer_cast<T>(connection);
    }

    auto connection = mPool.front();
    mPool.pop_front();
    wxLogDebug(wxT("Acquire()"));

    return std::dynamic_pointer_cast<T>(connection);
}

template<class T>
inline void ConnectionPool<T>::Release(std::shared_ptr<T> connection)
{
    mPool.push_back(std::dynamic_pointer_cast<IConnection>(connection));
    wxLogDebug(wxT("Release(std::shared_ptr<T> connection)"));
}
} // namespace app::db
