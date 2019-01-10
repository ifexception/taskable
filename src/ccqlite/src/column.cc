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


#include "column.hh"

#include "constants.hh"

namespace ccqlite
{
column::column(const column& other)
{
    pHandle = other.pHandle;
}

column::column(sqlite3_stmt* handle)
    : pHandle(handle)
{
}

column& column::operator=(const column& other)
{
    if (this != &other) {
        pHandle = nullptr;

        pHandle = other.pHandle;
    }

    return *this;
}
const std::string column::get_name(int index) const noexcept
{
    std::string name(sqlite3_column_name(pHandle, index));
    return name;
}

column_type column::get_type(int index) const noexcept
{
    int valueType = sqlite3_column_type(pHandle, index);
    switch (valueType) {
    case SQLITE_INTEGER:
        return column_type::Integer;
    case SQLITE_FLOAT:
        return column_type::Float;
    case SQLITE_TEXT:
        return column_type::Text;
    case SQLITE_BLOB:
        return column_type::Blob;
    case SQLITE_NULL:
        return column_type::Null;
    default:
        return column_type::Unknown;
    }
}

int column::get(int index, int) const noexcept
{
    return sqlite3_column_int(pHandle, index);
}

long long column::get(int index, long long) const noexcept
{
    return sqlite3_column_int64(pHandle, index);
}

double column::get(int index, double) const noexcept
{
    return sqlite3_column_double(pHandle, index);
}

std::string column::get(int index, std::string) const noexcept
{
    const char* text =
        reinterpret_cast<const char*>(sqlite3_column_text(pHandle, index));
    return std::string(text);
}

const void* column::get(int index, const void*) const noexcept
{
    return sqlite3_column_blob(pHandle, index);
}

}
