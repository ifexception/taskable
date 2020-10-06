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

#include "databasestructureupdater.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace app::svc
{
DatabaseStructureUpdater::DatabaseStructureUpdater(std::shared_ptr<spdlog::logger> logger)
    : pLogger(logger)
{
    pConnection = db::ConnectionProvider::Get().Handle()->Acquire();
}

DatabaseStructureUpdater::~DatabaseStructureUpdater()
{
    db::ConnectionProvider::Get().Handle()->Release(pConnection);
}

bool DatabaseStructureUpdater::ExecuteScripts()
{
    bool projectsHoursColumnDropped = DropProjectsHoursColumn();
    bool meetingsTableCreated = CreateMeetingsTableScript();
    bool meetingForeignKeyAdded = AddMeetingForeignKeyToTaskItemsTable();

    return projectsHoursColumnDropped && meetingsTableCreated && meetingForeignKeyAdded;
}

bool DatabaseStructureUpdater::DropProjectsHoursColumn()
{
    const std::string DropProjectsHourColumnOperationName = "DropProjectsHourColumn";

    const std::string ProjectsHourColumnName = "hours";

    const std::string PragmaInfoTable = "pragma table_info(projects);";

    std::vector<std::string> columnNames;
    try {
        *pConnection->DatabaseExecutableHandle() << PragmaInfoTable >> [&](int64_t cid,
                                                                           std::string name,
                                                                           std::string type,
                                                                           int notnull,
                                                                           std::unique_ptr<std::string> dlft_value,
                                                                           int pk) { columnNames.push_back(name); };
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error in database structure update operation {0} | {1:d} : {2}",
            DropProjectsHourColumnOperationName,
            e.get_code(),
            e.what());
        return false;
    }

    auto hoursColumnExistsIterator = std::find_if_not(columnNames.begin(),
        columnNames.end(),
        [=](const std::string& columnName) { return columnName != ProjectsHourColumnName; });

    if (hoursColumnExistsIterator == columnNames.end()) {
        return true;
    }

    const std::string CreateTempTable = "CREATE TABLE temp_projects_table "
                                        "( "
                                        "project_id INTEGER PRIMARY KEY NOT NULL, "
                                        "name TEXT NOT NULL UNIQUE, "
                                        "display_name TEXT NOT NULL, "
                                        "billable INTEGER NOT NULL, "
                                        "rate REAL NULL, "
                                        "is_default INTEGER NOT NULL DEFAULT(0), "
                                        "date_created INTEGER NOT NULL DEFAULT(strftime('%s', 'now', 'localtime')), "
                                        "date_modified INTEGER NOT NULL DEFAULT(strftime('%s', 'now', 'localtime')), "
                                        "is_active INTEGER NOT NULL, "
                                        "employer_id INTEGER NOT NULL, "
                                        "client_id INTEGER NULL, "
                                        "rate_type_id INTEGER NULL, "
                                        "currency_id INTEGER NULL, "
                                        "FOREIGN KEY(employer_id) REFERENCES employers(employer_id), "
                                        "FOREIGN KEY(client_id) REFERENCES clients(client_id), "
                                        "FOREIGN KEY(rate_type_id) REFERENCES rate_types(rate_type_id), "
                                        "FOREIGN KEY(currency_id) REFERENCES currencies(currency_id)) "
                                        ")";

    const std::string CopyOldDataToTempTable = "INSERT INTO temp_projects_table "
                                               "SELECT project_id,"
                                               "name, "
                                               "display_name, "
                                               "billable, "
                                               "rate, "
                                               "is_default, "
                                               "date_created, "
                                               "date_modified, "
                                               "is_active, "
                                               "employer_id, "
                                               "client_id, "
                                               "rate_type_id, "
                                               "currency_id "
                                               "FROM projects";
    const std::string DropOldTable = "DROP TABLE projects;";

    const std::string RenameTempTableToTaskItemsTable = "ALTER TABLE temp_projects_table RENAME TO projects";

    try {
        *pConnection->DatabaseExecutableHandle() << "BEGIN";

        *pConnection->DatabaseExecutableHandle() << CreateTempTable;
        *pConnection->DatabaseExecutableHandle() << CopyOldDataToTempTable;
        *pConnection->DatabaseExecutableHandle() << DropOldTable;
        *pConnection->DatabaseExecutableHandle() << RenameTempTableToTaskItemsTable;

        *pConnection->DatabaseExecutableHandle() << "COMMIT";
    } catch (const sqlite::sqlite_exception& e) {
        *pConnection->DatabaseExecutableHandle() << "ROLLBACK";
        pLogger->error("Error in database structure update operation {0} | {1:d} : {2}",
            DropProjectsHourColumnOperationName,
            e.get_code(),
            e.what());
        return false;
    }

    return true;
}

bool DatabaseStructureUpdater::CreateMeetingsTableScript()
{
    const std::string CreateMeetingsTableOperationName = "CreateMeetingsTableScript";

    const std::string CreateMeetingsTable =
        "CREATE TABLE IF NOT EXISTS meetings "
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

    try {
        *pConnection->DatabaseExecutableHandle() << CreateMeetingsTable;
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error in database structure update operation {0} | {1:d} : {2}",
            CreateMeetingsTableOperationName,
            e.get_code(),
            e.what());
        return false;
    }

    return true;
}

bool DatabaseStructureUpdater::AddMeetingForeignKeyToTaskItemsTable()
{
    const std::string AddMeetingForeignKeyToTaskItemsTableOperationName = "AddMeetingForeignKeyToTaskItemsTable";
    const std::string MeetingForeignKeyColumnName = "meeting_id";

    const std::string PragmaInfoTable = "pragma table_info(task_items);";

    std::vector<std::string> columnNames;
    try {
        *pConnection->DatabaseExecutableHandle() << PragmaInfoTable >> [&](int64_t cid,
                                                                           std::string name,
                                                                           std::string type,
                                                                           int notnull,
                                                                           std::unique_ptr<std::string> dlft_value,
                                                                           int pk) { columnNames.push_back(name); };
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error in database structure update operation {0} | {1:d} : {2}",
            AddMeetingForeignKeyToTaskItemsTableOperationName,
            e.get_code(),
            e.what());
        return false;
    }

    auto meetingForeignKeyExistsIterator = std::find_if_not(columnNames.begin(),
        columnNames.end(),
        [=](const std::string& columnName) { return columnName == MeetingForeignKeyColumnName; });

    if (meetingForeignKeyExistsIterator == columnNames.end()) {
        return true;
    }

    const std::string CreateTempTable =
        "CREATE TABLE sqlb_temp_table_1 "
        "( "
        "task_item_id integer NOT NULL, "
        "start_time text, "
        "end_time text, "
        "duration text NOT NULL, "
        "description text NOT NULL, "
        "billable integer NOT NULL, "
        "calculated_rate real, "
        "date_created integer NOT NULL DEFAULT(strftime('%s', 'now')), "
        "date_modified integer NOT NULL DEFAULT(strftime('%s', 'now')), "
        "is_active integer NOT NULL, "
        "task_item_type_id integer NOT NULL, "
        "project_id integer NOT NULL, "
        "task_id integer NOT NULL, "
        "category_id integer NOT NULL, "
        "meeting_id INTEGER, "
        "PRIMARY KEY( task_item_id ), "
        "FOREIGN KEY( task_id ) REFERENCES tasks( task_id ), "
        "FOREIGN KEY( task_item_type_id ) REFERENCES task_item_types( task_item_type_id ), "
        "FOREIGN KEY( project_id ) REFERENCES projects( project_id ), "
        "FOREIGN KEY( category_id ) REFERENCES categories( category_id ), "
        "FOREIGN KEY( meeting_id ) REFERENCES meetings( meeting_id ) "
        ");";

    const std::string CopyOldDataToTempTable = "INSERT INTO sqlb_temp_table_1 "
                                               "SELECT task_item_id, "
                                               "start_time, end_time, "
                                               "duration, "
                                               "description, "
                                               "billable, "
                                               "calculated_rate, "
                                               "date_created, "
                                               "date_modified, "
                                               "is_active, "
                                               "task_item_type_id, "
                                               "project_id, "
                                               "task_id, "
                                               "category_id, NULL "
                                               "FROM task_items";

    const std::string DropOldTable = "DROP TABLE task_items;";

    const std::string RenameTempTableToTaskItemsTable = "ALTER TABLE sqlb_temp_table_1 RENAME TO task_items";

    try {
        *pConnection->DatabaseExecutableHandle() << "BEGIN";

        *pConnection->DatabaseExecutableHandle() << CreateTempTable;
        *pConnection->DatabaseExecutableHandle() << CopyOldDataToTempTable;
        *pConnection->DatabaseExecutableHandle() << DropOldTable;
        *pConnection->DatabaseExecutableHandle() << RenameTempTableToTaskItemsTable;

        *pConnection->DatabaseExecutableHandle() << "COMMIT";
    } catch (const sqlite::sqlite_exception& e) {
        *pConnection->DatabaseExecutableHandle() << "ROLLBACK";
        pLogger->error("Error in database structure update operation {0} | {1:d} : {2}",
            AddMeetingForeignKeyToTaskItemsTableOperationName,
            e.get_code(),
            e.what());
        return false;
    }
    return true;
}
} // namespace app::svc
