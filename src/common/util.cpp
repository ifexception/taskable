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

#include "util.h"

#include <chrono>
#include <ctime>
#include <sstream>

#include <wx/datetime.h>
#include <wx/string.h>

namespace app::util
{
wxString ConvertUnixTimestampToString(int timestamp)
{
    std::time_t time = static_cast<std::time_t>(timestamp);
    wxDateTime asDate(time);
    wxString dateString = asDate.FormatISOCombined();
    return dateString;
}

wxDateTime ToDateTime(int timestamp)
{
    std::time_t time = static_cast<std::time_t>(timestamp);
    return wxDateTime(time);
}

wxDateTime RoundToNearestInterval(wxDateTime value, int interval)
{
    std::time_t seconds = value.GetTicks();
    double timeRoundToInterval = std::round((double)seconds / (interval * 60.0)) * (interval * 60.0);
    std::time_t time = timeRoundToInterval;
    wxDateTime roundedTime(time);
    roundedTime.SetSecond(0);
    return roundedTime;
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

int VoidPointerToInt(void* value)
{
    intptr_t p = reinterpret_cast<intptr_t>(value);
    return static_cast<int>(p);
}

void* IntToVoidPointer(int value)
{
    intptr_t p = static_cast<intptr_t>(value);
    return reinterpret_cast<void*>(p);
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
