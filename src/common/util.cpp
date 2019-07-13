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

#include "util.h"

#include <chrono>
#include <fstream>
#include <sstream>
#include <wx/datetime.h>
#include <wx/string.h>

namespace app::util
{
std::vector<char> ReadFile(const std::string& file)
{
    std::ifstream cfgFile(file.c_str());
    std::vector<char> buffer(std::istreambuf_iterator<char>{cfgFile}, std::istreambuf_iterator<char>());
    buffer.push_back('\0');

    return buffer;
}

wxString ConvertUnixTimestampToString(int timestamp)
{
    time_t time = static_cast<time_t>(timestamp);
    wxDateTime asDate(time);
    wxString dateString = asDate.FormatISOCombined();
    return dateString;
}

int UnixTimestamp()
{
    auto tp = std::chrono::system_clock::now();
    auto dur = tp.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(dur).count();
    return seconds;
}

int SecondsToMilliseconds(int value)
{
    return value * 1000;
}

int MinutesToMilliseconds(int value)
{
    return value * 60000;
}
} // namespace app::util

std::vector<std::string> app::util::lib::split(const std::string& in, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(in);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}
