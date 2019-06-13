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

#include "configuration.h"

namespace app::cfg
{
wxString Configuration::CFG_FILE = "tasks-tracker.ini";

Configuration::Configuration()
{
    wxString configPath = wxPathOnly(wxStandardPaths::Get().GetExecutablePath()) + "\\" + CFG_FILE;
    pConfig = new wxFileConfig(wxEmptyString, wxEmptyString, configPath);
    /*auto b = mConfig->GetNumberOfGroups();
    mConfig->SetPath("settings");
    bool def = false;
    auto value = mConfig->Read("confirmOnExit", &def);*/
}

Configuration::~Configuration()
{
    delete pConfig;
}

void Configuration::Save()
{
    pConfig->Flush();
}

wxString Configuration::GetConnectionString() const
{
    return Get<wxString>(wxT("connection"), wxT("connectionString"));
}

void Configuration::SetConnectionString(const wxString& value)
{
    Set<wxString>(wxT("connection"), wxT("connectionString"), value);
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

bool Configuration::IsShowBalloonNotifications() const
{
    return Get<bool>(wxT("settings"), wxT("showBalloonNotifications"));
}

void Configuration::SetShowBalloonNotifications(bool value)
{
    Set<bool>(wxT("settings"), wxT("showBalloonNotifications"), value);
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

template<class T>
T Configuration::Get(const wxString& group, const wxString& key) const
{
    pConfig->SetPath(group);
    T value;
    pConfig->Read(key, &value);
    pConfig->SetPath("/");
    return value;
}

template<class T>
void Configuration::Set(const wxString& group, const wxString& key, T value)
{
    pConfig->SetPath(group);
    pConfig->Write(key, value);
}
}