//  Taskable is a desktop that aids you in tracking your timesheets
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

#pragma once

#include <string>
#include <vector>

#include "models/client.h"
#include "models/project.h"
#include "models/category.h"
#include "models/task_item.h"

namespace app::services
{
struct db_service
{
    db_service() = default;
    ~db_service() = default;

    void create_employers_table();
    void create_clients_table();
    void create_projects_table();
    void create_categories_table();
    void create_tasks_table();
    void create_task_item_types_table();
    void create_task_items_table();

    void seed_task_item_types_table();

    int get_last_insert_rowid();

    void create_new_client(const std::string& name, const int employerId);

    std::vector<models::client> get_clients_by_employer_id(const int employerId);

    std::vector<models::client> get_clients();

    models::client get_client_by_id(const int clientId);

    void update_client(models::client client);

    void delete_client(const int clientId, const int dateModified);

    void create_new_project(const std::string& name,
        const std::string& displayName,
        const int employerId,
        const int* clientId);

    std::vector<models::project> get_projects();

    models::project get_project_by_id(const int projectId);

    void update_project(models::project project);

    void delete_project(const int projectId, const int dateModified);

    void create_new_category(const int projectId, const std::string& name, int color, const std::string& description);

    void create_new_category(const models::category category);

    std::vector<models::category> get_categories_by_project_id(const int projectId);

    models::category get_category_by_id(const int categoryId);

    std::vector<models::category> get_categories();

    void update_category(const models::category category);

    void delete_category(const int categoryId, const int dateModified);

    models::task_context get_employer_and_client_by_project_id(const int projectId);

    int create_or_get_task_id(const std::string& date);

    void create_new_task_item(const models::task_item& taskItem);

    std::vector<models::task_item> get_all_task_items_by_date(const std::string& date);

    models::task_item get_task_item_by_id(const int taskId);

    void update_task_item(models::task_item task);

    void delete_task_item(const int taskId, const int dateModified);

    std::vector<std::string> get_task_hours_by_id(const std::string& date);
};
} // namespace app::services
