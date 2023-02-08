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

#include "resources.h"

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

namespace app::rc
{
const char** GetProgramIcon()
{
    return taskablexpm;
}

const char** GetAboutIcon()
{
    return aboutxpm;
}

const char** GetCheckForUpdateIcon()
{
    return checkforupdatexpm;
}

const char** GetEntryTaskIcon()
{
    return entrytaskxpm;
}

const char** GetTimedTaskIcon()
{
    return timedtaskxpm;
}

const char** GetStopwatchIcon()
{
    return stopwatchxpm;
}

const char** GetDatabaseRestoreIcon()
{
    return database_restore;
}

const char** GetDatabaseBackupIcon()
{
    return database_backup_xpm;
}

const char** GetProgramIcon64()
{
    return taskable_64_xpm;
}

const char** GetSettingsIcon()
{
    return settings_xpm;
}

const char** GetQuitIcon()
{
    return quit_xpm;
}

const char** GetFeedbackIcon()
{
    return feedback_xpm;
}
}
