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

#pragma once

#include <wx/sizer.h>

namespace app::common
{
namespace sizers
{
const wxSizerFlags ControlDefault = wxSizerFlags().Border(wxALL, 5);
const wxSizerFlags ControlCenter = wxSizerFlags(ControlDefault).Center();
const wxSizerFlags ControlCenterVertical = wxSizerFlags(ControlDefault).CenterVertical();
const wxSizerFlags ControlRight = wxSizerFlags(ControlDefault).Align(wxALIGN_RIGHT);
const wxSizerFlags ControlExpand = wxSizerFlags(ControlDefault).Expand();
const wxSizerFlags ControlExpandProp = wxSizerFlags(ControlDefault).Align(wxEXPAND).Proportion(1);
} // namespace sizers

// TODO Check licensing https://icons8.com/icon/pack/business/plasticine
extern const char** GetProgramIcon();
} // namespace app::common
