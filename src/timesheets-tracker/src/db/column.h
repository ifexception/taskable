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


#pragma once

#include <string>
#include <tuple>

#include <sqlite3.h>

#include "column_type.hh"

namespace app::db
{

class column
{
public:
    class column_stream
    {
    public:
        explicit column_stream(column& column, int index);

        template <class T>
        column_stream& operator >> (T& value);

    private:
        column& rColumn;
        int mIndex;
    };
    column() = delete;
    explicit column(const column& other);
    explicit column(sqlite3_stmt* handle);
    ~column() = default;

    column& operator=(const column& other);

    template <class T>
    T get(int index) const noexcept;

    int data_count() const noexcept;
    int column_bytes(int index) const noexcept;
    const std::string get_name(int index) const noexcept;
    column_type get_type(int index) const noexcept;

    column_stream getter(int index = 0);

private:
    int get(int index, int) const noexcept;
    long long get(int index, long long) const noexcept;
    double get(int index, double) const noexcept;
    std::string get(int index, std::string) const noexcept;
    const void* get(int index, const void*) const noexcept;

    sqlite3_stmt* pHandle;
};

template <class T>
column::column_stream& column::column_stream::operator>>(T& value)
{
    value = rColumn.get(mIndex);
    ++mIndex;
    return *this;
}

template <class T>
T column::get(int index) const noexcept
{
    return get(index, T());
}

// template <class... Types>
// std::tuple<Types...> column::get(int maxIndex)
// {
//     return get_columns(std::make_integer_sequence<int, maxIndex>{});
// }

// template <class... Types, const int... Integers>
// std::tuple<Types...> column::get_columns(
//     const std::integer_sequence<int, Integers...>) const
// {
//     return std::make_tuple(get(Integers, Types())...);
// }

} // namespace app::db
