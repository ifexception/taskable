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

#pragma once

#include <string>

#include <toml.hpp>

#include <wx/gdicmn.h>

namespace app::cfg
{
class Configuration
{
public:
    Configuration();
    ~Configuration() = default;

    void Save();

    /* Getters */
    bool IsStartOnBoot() const;
    bool IsShowInTray() const;
    bool IsMinimizeToTray() const;
    bool IsCloseToTray() const;

    std::string GetDatabasePath() const;
    bool IsBackupEnabled() const;
    std::string GetBackupPath() const;
    int GetDeleteBackupsAfter() const;

    bool IsMinimizeStopwatchWindow() const;
    int GetHideWindowTimerInterval() const;
    int GetNotificationTimerInterval() const;
    int GetPausedTaskReminderInterval() const;
    bool IsStartStopwatchOnLaunch() const;
    bool IsStartStopwatchOnResume() const;

    bool IsTimeRoundingEnabled() const;
    int GetTimeToRoundTo() const;

    std::string GetFrameSize() const;

    std::string GetDelimiter() const;
    std::string GetExportPath() const;

    /* Setters */
    void SetStartOnBoot(bool value);
    void SetShowInTray(bool value);
    void SetMinimizeToTray(bool value);
    void SetCloseToTray(bool value);

    void SetDatabasePath(const std::string& value);
    void SetBackupEnabled(bool value);
    void SetBackupPath(const std::string& value);
    void SetDeleteBackupsAfter(int value);

    void SetMinimizeStopwatchWindow(bool value);
    void SetHideWindowTimerInterval(int value);
    void SetNotificationTimerInterval(int value);
    void SetPausedTaskReminderInterval(int value);
    void SetStartStopwatchOnLaunch(bool value);
    void SetStartStopwatchOnResume(bool value);

    void SetTimeRounding(bool value);
    void SetTimeToRoundTo(int value);

    void SetFrameSize(const std::string& value);

    void SetDelimiter(const std::string& value);
    void SetExportPath(const std::string& value);

private:
    void LoadConfigFile();

    void GetGeneralConfig(const toml::value& config);
    void GetDatabaseConfig(const toml::value& config);
    void GetStopwatchConfig(const toml::value& config);
    void GetTaskItemConfig(const toml::value& config);
    void GetPersistenceConfig(const toml::value& config);
    void GetExportConfig(const toml::value& config);

    struct Sections {
        static const std::string GeneralSection;
        static const std::string DatabaseSection;
        static const std::string StopwatchSection;
        static const std::string TaskItemSection;
        static const std::string PersistenceSection;
        static const std::string ExportSection;
    };

    struct Settings {
        bool StartOnBoot;
        bool ShowInTray;
        bool MinimizeToTray;
        bool CloseToTray;

        std::string DatabasePath;
        bool BackupEnabled;
        std::string BackupPath;
        int DeleteBackupsAfter;

        bool MinimizeStopwatchWindow;
        int HideWindowTimerInterval;
        int NotificationTimerInterval;
        int PausedTaskReminderInterval;
        bool StartStopwatchOnLaunch;
        bool StartStopwatchOnResume;

        bool TimeRounding;
        int TimeToRoundTo;

        std::string Dimension;

        std::string Delimiter;
        std::string ExportPath;

        Settings() = default;
        ~Settings() = default;
    };

    Settings mSettings;
};
} // namespace app::cfg
