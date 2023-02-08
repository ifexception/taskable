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

#include "configuration.h"

#include <fstream>

#include <wx/stdpaths.h>

#include "../common/common.h"
#include "../common/util.h"

namespace app::cfg
{
const std::string Configuration::Sections::GeneralSection = "general";
const std::string Configuration::Sections::DatabaseSection = "database";
const std::string Configuration::Sections::StopwatchSection = "stopwatch";
const std::string Configuration::Sections::TaskItemSection = "task_item";
const std::string Configuration::Sections::PersistenceSection = "persistence";
const std::string Configuration::Sections::ExportSection = "export";

Configuration::Configuration()
    : mSettings()
{
    LoadConfigFile();
}

// clang-format off
void Configuration::Save()
{
    // TODO, refactor this to be more maintainable
    const toml::value data{
        {
            Sections::GeneralSection,
            {
                { "startOnBoot", mSettings.StartOnBoot },
                { "showInTray", mSettings.ShowInTray },
                { "minimizeToTray", mSettings.MinimizeToTray },
                { "closeToTray", mSettings.CloseToTray },
            }
        },
        {
            Sections::DatabaseSection,
            {
                { "databasePath", mSettings.DatabasePath },
                { "backupEnabled", mSettings.BackupEnabled },
                { "backupPath", mSettings.BackupPath },
                { "deleteBackupsAfter", mSettings.DeleteBackupsAfter }
            }
        },
        {
            Sections::StopwatchSection,
            {
                { "minimizeStopwatchWindow", mSettings.MinimizeStopwatchWindow },
                { "hideWindowTimer", mSettings.HideWindowTimerInterval },
                { "notificationTimer", mSettings.NotificationTimerInterval },
                { "pausedTaskReminder", mSettings.PausedTaskReminderInterval },
                { "startStopwatchOnLaunch", mSettings.StartStopwatchOnLaunch },
                { "startStopwatchOnResume", mSettings.StartStopwatchOnResume },
            }
        },
        {
            Sections::TaskItemSection,
            {
                { "timeRounding", mSettings.TimeRounding },
                { "timeToRoundTo", mSettings.TimeToRoundTo }
            }
        },
        {
            Sections::PersistenceSection,
            {
                { "dimensions", mSettings.Dimension }
            }
        },
        {
            Sections::ExportSection,
            {
                { "delimiter", mSettings.Delimiter },
                { "exportPath", mSettings.ExportPath }
            }
        }
    };

    const std::string configString = toml::format(data);

    const std::string configFilePath = common::GetConfigFilePath().ToStdString();
    std::ofstream configFile;
    configFile.open(configFilePath, std::ios_base::out);
    if (!configFile)
    {
         // log error
        return;
    }

    configFile << configString;

    configFile.close();
}
// clang-format on

bool Configuration::IsStartOnBoot() const
{
    return mSettings.StartOnBoot;
}

bool Configuration::IsShowInTray() const
{
    return mSettings.ShowInTray;
}

bool Configuration::IsMinimizeToTray() const
{
    return mSettings.MinimizeToTray;
}

bool Configuration::IsCloseToTray() const
{
    return mSettings.CloseToTray;
}

std::string Configuration::GetDatabasePath() const
{
    return mSettings.DatabasePath;
}

bool Configuration::IsBackupEnabled() const
{
    return mSettings.BackupEnabled;
}

std::string Configuration::GetBackupPath() const
{
    return mSettings.BackupPath;
}

int Configuration::GetDeleteBackupsAfter() const
{
    return mSettings.DeleteBackupsAfter;
}

bool Configuration::IsMinimizeStopwatchWindow() const
{
    return mSettings.MinimizeStopwatchWindow;
}

int Configuration::GetHideWindowTimerInterval() const
{
    return mSettings.HideWindowTimerInterval;
}

int Configuration::GetNotificationTimerInterval() const
{
    return mSettings.NotificationTimerInterval;
}

int Configuration::GetPausedTaskReminderInterval() const
{
    return mSettings.PausedTaskReminderInterval;
}

bool Configuration::IsStartStopwatchOnLaunch() const
{
    return mSettings.StartStopwatchOnLaunch;
}

bool Configuration::IsStartStopwatchOnResume() const
{
    return mSettings.StartStopwatchOnResume;
}

bool Configuration::IsTimeRoundingEnabled() const
{
    return mSettings.TimeRounding;
}

int Configuration::GetTimeToRoundTo() const
{
    return mSettings.TimeToRoundTo;
}

std::string Configuration::GetFrameSize() const
{
    return mSettings.Dimension;
}

std::string Configuration::GetDelimiter() const
{
    return mSettings.Delimiter;
}

std::string Configuration::GetExportPath() const
{
    return mSettings.ExportPath;
}

void Configuration::SetStartOnBoot(bool value)
{
    mSettings.StartOnBoot = value;
}

void Configuration::SetShowInTray(bool value)
{
    mSettings.ShowInTray = value;
}

void Configuration::SetMinimizeToTray(bool value)
{
    mSettings.MinimizeToTray = value;
}

void Configuration::SetCloseToTray(bool value)
{
    mSettings.CloseToTray = value;
}

void Configuration::SetDatabasePath(const std::string& value)
{
    mSettings.DatabasePath = value;
}

void Configuration::SetBackupEnabled(bool value)
{
    mSettings.BackupEnabled = value;
}

void Configuration::SetBackupPath(const std::string& value)
{
    mSettings.BackupPath = value;
}

void Configuration::SetDeleteBackupsAfter(int value)
{
    mSettings.DeleteBackupsAfter = value;
}

void Configuration::SetMinimizeStopwatchWindow(bool value)
{
    mSettings.MinimizeStopwatchWindow = value;
}

void Configuration::SetHideWindowTimerInterval(int value)
{
    mSettings.HideWindowTimerInterval = value;
}

void Configuration::SetNotificationTimerInterval(int value)
{
    mSettings.NotificationTimerInterval = value;
}

void Configuration::SetPausedTaskReminderInterval(int value)
{
    mSettings.PausedTaskReminderInterval = value;
}

void Configuration::SetStartStopwatchOnLaunch(bool value)
{
    mSettings.StartStopwatchOnLaunch = value;
}

void Configuration::SetStartStopwatchOnResume(bool value)
{
    mSettings.StartStopwatchOnResume;
}

void Configuration::SetTimeRounding(bool value)
{
    mSettings.TimeRounding = value;
}

void Configuration::SetTimeToRoundTo(int value)
{
    mSettings.TimeToRoundTo = value;
}

void Configuration::SetFrameSize(const std::string& value)
{
    mSettings.Dimension = value;
}

void Configuration::SetDelimiter(const std::string& value)
{
    mSettings.Delimiter = value;
}

void Configuration::SetExportPath(const std::string& value)
{
    mSettings.ExportPath = value;
}

void Configuration::LoadConfigFile()
{
    auto data = toml::parse(common::GetConfigFilePath());

    GetGeneralConfig(data);
    GetDatabaseConfig(data);
    GetStopwatchConfig(data);
    GetTaskItemConfig(data);
    GetPersistenceConfig(data);
    GetExportConfig(data);
}

void Configuration::GetGeneralConfig(const toml::value& config)
{
    const auto& generalSection = toml::find(config, Sections::GeneralSection);

    mSettings.StartOnBoot = toml::find<bool>(generalSection, "startOnBoot");
    mSettings.ShowInTray = toml::find<bool>(generalSection, "showInTray");
    mSettings.MinimizeToTray = toml::find<bool>(generalSection, "minimizeToTray");
    mSettings.CloseToTray = toml::find<bool>(generalSection, "closeToTray");
}

void Configuration::GetDatabaseConfig(const toml::value& config)
{
    const auto& databaseSection = toml::find(config, Sections::DatabaseSection);

    mSettings.DatabasePath = toml::find<std::string>(databaseSection, "databasePath");
    mSettings.BackupEnabled = toml::find<bool>(databaseSection, "backupEnabled");
    mSettings.BackupPath = toml::find<std::string>(databaseSection, "backupPath");
    mSettings.DeleteBackupsAfter = toml::find<int>(databaseSection, "deleteBackupsAfter");
}

void Configuration::GetStopwatchConfig(const toml::value& config)
{
    const auto& stopwatchSection = toml::find(config, Sections::StopwatchSection);

    mSettings.MinimizeStopwatchWindow = toml::find<bool>(stopwatchSection, "minimizeStopwatchWindow");
    mSettings.HideWindowTimerInterval = toml::find<int>(stopwatchSection, "hideWindowTimer");
    mSettings.NotificationTimerInterval = toml::find<int>(stopwatchSection, "notificationTimer");
    mSettings.PausedTaskReminderInterval = toml::find<int>(stopwatchSection, "pausedTaskReminder");
    mSettings.StartStopwatchOnLaunch = toml::find<bool>(stopwatchSection, "startStopwatchOnLaunch");
    mSettings.StartStopwatchOnResume = toml::find<bool>(stopwatchSection, "startStopwatchOnResume");
}

void Configuration::GetTaskItemConfig(const toml::value& config)
{
    const auto& taskItemSection = toml::find(config, Sections::TaskItemSection);

    mSettings.TimeRounding = toml::find<bool>(taskItemSection, "timeRounding");
    mSettings.TimeToRoundTo = toml::find<int>(taskItemSection, "timeToRoundTo");
}

void Configuration::GetPersistenceConfig(const toml::value& config)
{
    const auto& persistenceSection = toml::find(config, Sections::PersistenceSection);

    mSettings.Dimension = toml::find<std::string>(persistenceSection, "dimensions");
}
void Configuration::GetExportConfig(const toml::value& config)
{
    const auto exportSection = toml::find(config, Sections::ExportSection);

    mSettings.Delimiter = toml::find<std::string>(exportSection, "delimiter");
    mSettings.ExportPath = toml::find<std::string>(exportSection, "exportPath");
}
} // namespace app::cfg
