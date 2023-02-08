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

#include <wx/wx.h>

#include "version.h"

namespace app::common
{
namespace sizers
{
const wxSizerFlags ControlDefault = wxSizerFlags().Border(wxALL, 5);
const wxSizerFlags ControlCenter = wxSizerFlags(ControlDefault).Center();
const wxSizerFlags ControlCenterHorizontal = wxSizerFlags(ControlDefault).CenterHorizontal();
const wxSizerFlags ControlCenterVertical = wxSizerFlags(ControlDefault).CenterVertical();
const wxSizerFlags ControlRight = wxSizerFlags(ControlDefault).Right();
const wxSizerFlags ControlLeft = wxSizerFlags(ControlDefault).Left();
const wxSizerFlags ControlExpand = wxSizerFlags(ControlDefault).Expand();
const wxSizerFlags ControlExpandProp = wxSizerFlags(ControlDefault).Align(wxEXPAND).Proportion(1);
} // namespace sizers

namespace validations
{
void ForRequiredChoiceSelection(wxWindow* window, wxString label);
void ForRequiredText(wxWindow* window, wxString label);
void ForRequiredLongText(wxWindow* window, wxString label);
void ForRequiredNumber(wxWindow* window, wxString label);
void ForInvalidTime(wxWindow* window, wxString message);
} // namespace validations

std::string GetLicense();

wxString GetVersion();

wxString GetProgramName();

wxString GetDatabaseFileName();

wxString GetDatabaseFilePath(const wxString& databasePath);

wxString GetConfigFilePath();

wxString GetConfigFileName();

wxString GetAppId();
} // namespace app::common
