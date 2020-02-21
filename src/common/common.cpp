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

#include "common.h"

#include <wx/stdpaths.h>
#include <wx/richtooltip.h>

#include "constants.h"
#include "../../res/taskable.xpm"
#include "../../res/about.xpm"
#include "../../res/checkforupdate.xpm"
#include "../../res/entry-task.xpm"
#include "../../res/timed-task.xpm"
#include "../../res/stopwatch.xpm"
#include "../../res/database-restore.xpm"
#include "../../res/database-backup.xpm"
#include "../../res/taskable-64.xpm"
#include "../../res/settings.xpm"
#include "../../res/quit.xpm"
#include "../../res/feedback.xpm"

const char** app::common::GetProgramIcon()
{
    return taskablexpm;
}

const char** app::common::GetAboutIcon()
{
    return aboutxpm;
}

const char** app::common::GetCheckForUpdateIcon()
{
    return checkforupdatexpm;
}

const char** app::common::GetEntryTaskIcon()
{
    return entrytaskxpm;
}

const char** app::common::GetTimedTaskIcon()
{
    return timedtaskxpm;
}

const char** app::common::GetStopwatchIcon()
{
    return stopwatchxpm;
}

const char** app::common::GetDatabaseRestoreIcon()
{
    return database_restore;
}

const char** app::common::GetDatabaseBackupIcon()
{
    return database_backup_xpm;
}

const char** app::common::GetProgramIcon64()
{
    return taskable_64_xpm;
}

const char** app::common::GetSettingsIcon()
{
    return settings_xpm;
}

const char** app::common::GetQuitIcon()
{
    return quit_xpm;
}

const char** app::common::GetFeedbackIcon()
{
    return feedback_xpm;
}

std::string app::common::GetLicense()
{
    return "Taskable is a desktop that aids you in tracking your timesheets\n"
           "and seeing what work you have done.\n"
           "Copyright(C) 2019 Szymon Welgus\n"
           "This program is free software : you can redistribute it and /\n"
           "or modify it under the terms of the GNU General Public License as published\n"
           "by the Free Software Foundation\n"
           ", either version 3 of the License\n"
           ", or (at your option) any later version.\n"
           "This program is distributed in the hope that it will be useful\n"
           ", but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
           "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the\n"
           "GNU General Public License for more details.\n"
           "You should have received a copy of the GNU General Public License\n"
           "along with this program.If not, see <https://www.gnu.org/licenses/>.";
}

wxString app::common::GetVersion()
{
    return wxString::Format(wxT("%d.%d.%d"), TASKABLE_MAJOR, TASKABLE_MINOR, TASKABLE_PATCH);
}

wxString app::common::GetProgramName()
{
    return wxT("Taskable");
}

wxString app::common::GetDatabasePath()
{
    return wxString::Format(wxT("%s\\data"), wxStandardPaths::Get().GetUserDataDir());
}

wxString app::common::GetDatabaseFileName()
{
    return wxT("taskable.db");
}

wxString app::common::GetDatabaseFilePath()
{
    return wxString::Format(
        wxT("%s\\data\\%s"), wxStandardPaths::Get().GetUserDataDir(), common::GetDatabaseFileName());
}

wxString app::common::GetConfigFilePath()
{
    return wxString::Format(wxT("%s\\%s"), wxStandardPaths::Get().GetUserDataDir(), common::GetConfigFileName());
}

wxString app::common::GetConfigFileName()
{
    return wxT("taskable.ini");
}

wxString app::common::GetAppId()
{
    return wxT("6BE5E5E6-68BF-4AF7-A9E5-FF919709E86C");
}

void app::common::validations::ForRequiredChoiceSelection(wxWindow* window, wxString label)
{
    const wxString errorHeader = wxT("Invalid selection");
    const wxString errorMessage = wxString::Format(wxT("A %s selection is required"), label);

    wxRichToolTip tooltip(errorHeader, errorMessage);
    tooltip.SetIcon(wxICON_WARNING);
    tooltip.ShowFor(window);
}

void app::common::validations::ForRequiredText(wxWindow* window, wxString label)
{
    const wxString errorHeader = wxT("Invalid input");
    const wxString errorMessage =
        wxString::Format(wxT("A %s is required \nand must be within %d to %d characters long"),
            label,
            constants::MinLength,
            constants::MaxLength);

    wxRichToolTip tooltip(errorHeader, errorMessage);
    tooltip.SetIcon(wxICON_WARNING);
    tooltip.ShowFor(window);
}

void app::common::validations::ForRequiredLongText(wxWindow* window, wxString label)
{
    const wxString errorHeader = wxT("Invalid input");
    const wxString errorMessage =
        wxString::Format(wxT("A %s is required \nand must be within %d to %d characters long"),
            label,
            constants::MinLength2,
            constants::MaxLength2);

    wxRichToolTip tooltip(errorHeader, errorMessage);
    tooltip.SetIcon(wxICON_WARNING);
    tooltip.ShowFor(window);
}

void app::common::validations::ForRequiredNumber(wxWindow* window, wxString label)
{
    const wxString errorHeader = wxT("Invalid amount");
    const wxString errorMessage =
        wxString::Format(wxT("%s is required \nand must be greater than %d"), label,
            constants::MinLength,
            constants::MaxLength);

    wxRichToolTip tooltip(errorHeader, errorMessage);
    tooltip.SetIcon(wxICON_WARNING);
    tooltip.ShowFor(window);
}

void app::common::validations::ForInvalidTime(wxWindow* window, wxString message)
{
    const wxString errorHeader = wxT("Invalid time");
    const wxString errorMessage = message;

    wxRichToolTip tooltip(errorHeader, errorMessage);
    tooltip.SetIcon(wxICON_WARNING);
    tooltip.ShowFor(window);
}
