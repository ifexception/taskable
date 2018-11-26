//  TimesheetsTracker is a desktop that aids you in tracking your timesheets
//  and seeing what work you have done.
//
//  Copyright(C)<2018><Szymon Welgus>
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
//
//  Contact:
//    szymonwelgus at gmail dot com

#include "database.hh"

namespace ccqlite
{
database::database(const std::string filePath)
    : pHandle(nullptr)
{
    const char* filename = filePath.c_str();
    const permission defaultPermission = permission::ReadOnly;
    const int flags = static_cast<int>(defaultPermission);
    const int ret = sqlite3_open_v2(filename, &pHandle, flags, nullptr);
}

database::database(const std::string filePath, const permission permission)
    : pHandle(nullptr)
{
    const char* filename = filePath.c_str();
    const int flags = static_cast<int>(permission);
    const int ret = sqlite3_open_v2(filename, &pHandle, flags, nullptr);
}

database::~database()
{
    int const ret = sqlite3_close(pHandle);
    pHandle = nullptr;
}

void database::init_logging()
{
    try {
        auto logger = spdlog::daily_logger_mt("ccqlite_logger",
            "logs/ccqlite.log.txt");
    } catch (const spdlog::spdlog_ex&) {
        exit(EXIT_FAILURE);
    }
}
} // namespace ccqlite
