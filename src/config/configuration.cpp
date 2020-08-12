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

#include "configuration.h"

#include <wx/file.h>

#include "../common/common.h"
#include "../common/util.h"

namespace app::cfg
{
Configuration::Configuration()
{
    wxString configFile =
        wxString::Format(wxT("%s\\%s"), wxStandardPaths::Get().GetUserDataDir(), common::GetConfigFileName());

    pConfig = new wxFileConfig(wxEmptyString, wxEmptyString, configFile);
    pConfig->SetPath("/");
}

Configuration::~Configuration()
{
    delete pConfig;
}

void Configuration::Save()
{
    pConfig->Flush();
}

bool Configuration::IsConfirmOnExit() const
{
    return Get<bool>(wxT("settings"), wxT("confirmOnExit"));
}

void Configuration::SetConfirmOnExit(bool value)
{
    Set<bool>(wxT("settings"), wxT("confirmOnExit"), value);
}

bool Configuration::IsStartOnBoot() const
{
    return Get<bool>(wxT("settings"), wxT("startOnBoot"));
}

void Configuration::SetStartOnBoot(bool value)
{
    Set<bool>(wxT("settings"), wxT("startOnBoot"), value);
}

bool Configuration::IsShowInTray() const
{
    return Get<bool>(wxT("settings"), wxT("showInTray"));
}

void Configuration::SetShowInTray(bool value)
{
    Set<bool>(wxT("settings"), wxT("showInTray"), value);
}

bool Configuration::IsMinimizeToTray() const
{
    return Get<bool>(wxT("settings"), wxT("minimizeToTray"));
}

void Configuration::SetMinimizeToTray(bool value)
{
    Set<bool>(wxT("settings"), wxT("minimizeToTray"), value);
}

bool Configuration::IsCloseToTray() const
{
    return Get<bool>(wxT("settings"), wxT("closeToTray"));
}

void Configuration::SetCloseToTray(bool value)
{
    Set<bool>(wxT("settings"), wxT("closeToTray"), value);
}

wxString Configuration::GetDatabasePath() const
{
    return Get<wxString>(wxT("settings"), wxT("databasePath"));
}

void Configuration::SetDatabasePath(const wxString& value)
{
    Set<wxString>(wxT("settings"), wxT("databasePath"), value);
}

bool Configuration::IsBackupEnabled() const
{
    return Get<bool>(wxT("settings"), wxT("backupEnabled"));
}

void Configuration::SetBackupEnabled(bool value)
{
    Set<bool>(wxT("settings"), wxT("backupEnabled"), value);
}

wxString Configuration::GetBackupPath() const
{
    return Get<wxString>(wxT("settings"), wxT("backupPath"));
}

void Configuration::SetBackupPath(const wxString& value)
{
    Set<wxString>(wxT("settings"), wxT("backupPath"), value);
}

int Configuration::GetDeleteBackupsAfter() const
{
    return Get<int>(wxT("settings"), wxT("deleteBackupsAfter"));
}

void Configuration::SetDeleteBackupsAfter(int value)
{
    Set<int>(wxT("settings"), wxT("deleteBackupsAfter"), value);
}

bool Configuration::IsMinimizeStopwatchWindow() const
{
    return Get<bool>(wxT("settings"), wxT("minimizeStopwatchWindow"));
}

void Configuration::SetMinimizeStopwatchWindow(bool value)
{
    Set<bool>(wxT("settings"), wxT("minimizeStopwatchWindow"), value);
}

int Configuration::GetHideWindowTimerInterval() const
{
    return Get<int>(wxT("settings"), wxT("hideWindowTimer"));
}

void Configuration::SetHideWindowTimerInterval(int value)
{
    Set<int>(wxT("settings"), wxT("hideWindowTimer"), value);
}

int Configuration::GetNotificationTimerInterval() const
{
    return Get<int>(wxT("settings"), wxT("notificationTimer"));
}

void Configuration::SetNotificationTimerInterval(int value)
{
    Set<int>(wxT("settings"), wxT("notificationTimer"), value);
}

int Configuration::GetPausedTaskReminderInterval() const
{
    return Get<int>(wxT("settings"), wxT("pausedTaskReminder"));
}

void Configuration::SetPausedTaskReminderInterval(int value)
{
    Set<int>(wxT("settings"), wxT("pausedTaskReminder"), value);
}

bool Configuration::IsStartStopwatchOnLaunch() const
{
    return Get<bool>(wxT("settings"), wxT("startStopwatchOnLaunch"));
}

void Configuration::SetStartStopwatchOnLaunch(bool value)
{
    Set<bool>(wxT("settings"), wxT("startStopwatchOnLaunch"), value);
}

bool Configuration::IsStartStopwatchOnResume() const
{
    return Get<bool>(wxT("settings"), wxT("startStopwatchOnResume"));
}

void Configuration::SetStartStopwatchOnResume(bool value)
{
    Set<bool>(wxT("settings"), wxT("startStopwatchOnResume"), value);
}

wxSize Configuration::GetFrameSize() const
{
    auto dimensions = Get<wxString>(wxT("persistence"), wxT("dimensions"));
    auto dimensionsSplit = util::lib::split(dimensions.ToStdString(), ',');
    int w = std::stoi(dimensionsSplit[0]);
    int h = std::stoi(dimensionsSplit[1]);
    return wxSize(w, h);
}

void Configuration::SetFrameSize(const wxSize value)
{
    int w = value.GetWidth();
    int h = value.GetHeight();
    auto newSize = wxString::Format(wxT("%s,%s"), std::to_string(w), std::to_string(h));
    Set<wxString>(wxT("persistence"), wxT("dimensions"), newSize);
}

bool Configuration::IsTimeRoundingEnabled() const
{
    return Get<bool>(wxT("settings"), wxT("timeRounding"));
}

void Configuration::SetTimeRounding(const bool value)
{
    Set<bool>(wxT("settings"), wxT("timeRounding"), value);
}

int Configuration::GetTimeToRoundTo() const
{
    return Get<int>(wxT("settings"), wxT("timeToRoundTo"));
}

void Configuration::SetTimeToRoundTo(const int value)
{
    Set<int>(wxT("settings"), wxT("timeToRoundTo"), value);
}

} // namespace app::cfg
