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

#include <wx/string.h>

namespace app::constants
{
static const char* TotalHours = "Total Hours %H:%M:%S";

static const int MinLength = 2;
static const int MinLength2 = 4;
static const int MaxLength = 255;
static const int MaxLength2 = 1024;

enum class RateTypes : int {
    Unknown = 1,
    Hourly = 2,
};

enum class TaskItemTypes : int {
    EntryTask = 1,
    TimedTask = 2,
};

enum Days { Monday = 0, Tuesday, Wednesday, Thursday, Friday, Saturday, Sunday };

enum class ExportFormats : int { OfficeXml = 1, Excel };

Days MapIndexToEnum(int index);

static const wxString DateCreatedLabel = wxT("Created: %s");
static const wxString DateModifiedLabel = wxT("Updated: %s");
static const wxString DateLabel = wxT("Created %s | Updated %s");

static const wxString OperationCompletedSuccessfully = wxT("Operation completed successfully");
static const wxString OperationEncounteredErrors = wxT("Operation encountered error(s)");
} // namespace app::constants
