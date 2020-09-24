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

#include <memory>
#include <string>

#include <spdlog/spdlog.h>

#include "../database/connectionprovider.h"
#include "../database/sqliteconnection.h"

namespace app::svc
{
class DatabaseStructureUpdater final
{
public:
    DatabaseStructureUpdater(std::shared_ptr<spdlog::logger> logger);
    ~DatabaseStructureUpdater() = default;

    void ExecuteScripts();

private:
    void CreateMeetingsTableScript();
    void AddMeetingForeignKeyToTaskItemsTable();

    std::shared_ptr<spdlog::logger> pLogger;
    std::shared_ptr<db::SqliteConnection> pConnection;
};

struct Scripts {
    std::wstring CreateMeetingsTable = L"CREATE TABLE IF NOT EXISTS meetings "
                                       "(meeting_id INTEGER PRIMARY KEY NOT NULL,"
                                       "attended INTEGER NULL,"
                                       "duration INTEGER NOT NULL,"
                                       "starting TEXT NOT NULL,"
                                       "ending TEXT NOT NULL,"
                                       "location TEXT NOT NULL,"
                                       "subject TEXT NOT NULL,"
                                       "body TEXT NOT NULL,"
                                       "date_created INTEGER NOT NULL DEFAULT(strftime('%s', 'now', 'localtime')),"
                                       "date_modified INTEGER NOT NULL DEFAULT(strftime('%s', 'now', 'localtime')),"
                                       "is_active INTEGER NOT NULL,"
                                       "task_id INTEGER NOT NULL,"
                                       "FOREIGN KEY(task_id) REFERENCES tasks(task_id));";

    std::wstring AddForeignKeyToTaskItemsTable;
};
}
