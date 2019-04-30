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

#include <string>
#include <vector>

#include "models/employer.h"
#include "models/client.h"
#include "models/project.h"
#include "models/category.h"
#include "models/detailed_task.h"
#include "models/task_detail.h"

namespace app::services
{
struct db_service
{
    db_service() = default;
    ~db_service() = default;

    void create_new_employer(const std::string& employerName);

    std::vector<models::employer> get_employers();

    models::employer get_employer(const int employerId);

    void update_employer(models::employer employer);

    void delete_employer(const int employerId);

    void create_new_client(const std::string& name, const int employerId);

    std::vector<models::client> get_clients_by_employer_id(const int employerId);

    void create_new_project(const std::string& name, const std::string& displayName, const int employerId, const int* clientId);

    std::vector<models::project> get_projects();

    void create_new_category(const int projectId, const std::string& name, const std::string& description);

    std::vector<models::category> get_categories_by_project_id(const int projectId);

    int create_or_get_task_id(const std::string& date, const int projectId);

    void create_new_task(const int projectId, const int taskId, const std::string& startTime, const std::string& endTime, const std::string& duration, const int categoryId, const std::string& description);

    std::vector<models::detailed_task> get_all_tasks_by_date(const std::string& date);

    models::task_detail get_task_by_id(const int taskDetailId);

    void update_task_detail(models::task_detail taskDetail);

    void delete_task_detail(const int taskDetailId);
};
}
