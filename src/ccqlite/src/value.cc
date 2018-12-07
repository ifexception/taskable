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


#include "value.hh"

namespace ccqlite
{
value::value(const value& other)
{
    pHandle = other.pHandle;
    mIndex = other.mIndex;
}

value::value(sqlite3_stmt* handle, const int index)
    : pHandle(handle)
    , mIndex(index)
{}

value& value::operator=(const value& other)
{
    if (this != &other) {
        pHandle = nullptr;
        mIndex = 0;

        pHandle = other.pHandle;
        mIndex = other.mIndex;
    }

    return *this;
}
const std::string value::get_name() const noexcept
{
    std::string name(sqlite3_column_name(pHandle, mIndex));
    return name;
}

#ifdef SQLITE_ENABLE_COLUMN_METADATA
const std::string value::get_origin_name() const noexcept
{
    std::string name(sqlite3_column_origin_name(pHandle, mIndex));
    return name;
}
#endif // SQLITE_ENABLE_COLUMN_METADATA


int value::get_int() const noexcept
{
    return sqlite3_column_int(pHandle, mIndex);
}

unsigned int value::get_uint() const noexcept
{
    return static_cast<unsigned>(get_int64());
}

long long value::get_int64() const noexcept
{
    return sqlite3_column_int64(pHandle, mIndex);
}

double value::get_double() const noexcept
{
    return sqlite3_column_double(pHandle, mIndex);
}

std::string value::get_text() const noexcept
{
    const unsigned char* text = sqlite3_column_text(pHandle, mIndex);
    const char* ctext = reinterpret_cast<const char*>(text);
    return std::string(ctext);
}

const void* value::get_blob() const noexcept
{
    return sqlite3_column_blob(pHandle, mIndex);
}

std::string value::get_string() const noexcept
{
    const void* blob = sqlite3_column_blob(pHandle, mIndex);
    const char* data = static_cast<const char*>(blob);
    return std::string(data, sqlite3_column_bytes(pHandle, mIndex));
}

value_type value::get_type() const noexcept
{
    int valueType = sqlite3_column_type(pHandle, mIndex);
    switch (valueType) {
    case SQLITE_INTEGER:
        return value_type::Integer;
    case SQLITE_FLOAT:
        return value_type::Float;
    case SQLITE_TEXT:
        return value_type::Text;
    case SQLITE_BLOB:
        return value_type::Blob;
    case SQLITE_NULL:
        return value_type::Null;
    default:
        return value_type::Unknown;
    }
}
}
