//  Tasks Tracker is a desktop that aids you in tracking your timesheets
//  and seeing what work you have done.
//
//  Copyright(C) <2019> <Szymon Welgus>
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

#include "configuration.h"

#include "../common/util.h"

namespace app::cfg
{
wxString Configuration::CFG_FILE = "tasks-tracker.ini";

Configuration::Configuration()
{
    wxString configPath = wxPathOnly(wxStandardPaths::Get().GetExecutablePath()) + "\\" + CFG_FILE;
    pConfig = new wxFileConfig(wxEmptyString, wxEmptyString, configPath);
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

bool Configuration::IsMinimizeTimedTaskWindow() const
{
    return Get<bool>(wxT("settings"), wxT("minimizeTimedTaskWindow"));
}

void Configuration::SetMinimizeTimedTaskWindow(bool value)
{
    Set<bool>(wxT("settings"), wxT("minimizeTimedTaskWindow"), value);
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
