// Productivity tool to help you track the time you spend on tasks
// Copyright (C) 2023  Szymon Welgus
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

#include <string>
#include <vector>

class wxDateTime;
class wxString;

namespace app::util
{
wxString ConvertUnixTimestampToString(int timestamp);

wxDateTime ToDateTime(int timestamp);

wxDateTime RoundToNearestInterval(wxDateTime value, int interval);

int UnixTimestamp();

int SecondsToMilliseconds(int value);

int MinutesToMilliseconds(int value);

int VoidPointerToInt(void* value);

void* IntToVoidPointer(int value);

wxString ToFriendlyDateTimeString(const wxDateTime& value);

namespace lib
{
std::vector<std::string> split(const std::string& in, char delimiter);

std::string replace(std::string& input, const std::string& search, const std::string& replace);
} // namespace lib
} // namespace app::util
