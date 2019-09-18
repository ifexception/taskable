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

#include "category.h"

namespace app::models
{
const std::string category::createNewCategory =
    "INSERT INTO categories (name, color, description, is_active, project_id) VALUES (?, ?, ?, 1, ?)";
const std::string category::getCategoriesByProjectId = "SELECT * FROM categories WHERE project_id = ?";
const std::string category::getCategoryById = "SELECT categories.category_id, "
                                              "categories.name AS category_name, "
                                              "categories.color, "
                                              "categories.description, "
                                              "categories.date_created_utc, "
                                              "categories.date_modified_utc, "
                                              "categories.is_active, "
                                              "projects.display_name AS project_name "
                                              "FROM categories "
                                              "INNER JOIN projects ON categories.project_id = projects.project_id "
                                              "WHERE categories.category_id = ?";
const std::string category::getCategories = "SELECT categories.category_id, "
                                            "categories.name AS category_name, "
                                            "categories.description, "
                                            "categories.date_created_utc, "
                                            "categories.date_modified_utc, "
                                            "categories.is_active, "
                                            "projects.display_name AS project_name "
                                            "FROM categories "
                                            "INNER JOIN projects ON categories.project_id = projects.project_id "
                                            "WHERE categories.is_active = 1";
const std::string category::updateCategory = "UPDATE categories SET name = ?, color = ?, description = ?, project_id = "
                                             "?, date_modified_utc = ? WHERE category_id = ?";
const std::string category::deleteCategory =
    "UPDATE categories SET is_active = 0, date_modified_utc = ? WHERE category_id = ?";

category::category(int categoryId,
    std::string categoryName,
    int color,
    std::string description,
    int dateCreatedUtc,
    int dateModifiedUtc,
    int isActive,
    int projectId)
    : category_id(categoryId)
    , category_name(categoryName)
    , color(color)
    , description(description)
    , date_created_utc(dateCreatedUtc)
    , date_modified_utc(dateModifiedUtc)
    , is_active(isActive)
    , project_id(projectId)
{
}

category::category(int categoryId,
    std::string categoryName,
    std::string description,
    int dateCreatedUtc,
    int dateModifiedUtc,
    int isActive,
    std::string projectName)
    : category_id(categoryId)
    , category_name(categoryName)
    , description(description)
    , date_created_utc(dateCreatedUtc)
    , date_modified_utc(dateModifiedUtc)
    , is_active(isActive)
    , project_name(projectName)
{
}

category::category(int categoryId,
    std::string categoryName,
    int color,
    std::string description,
    int dateCreatedUtc,
    int dateModifiedUtc,
    int isActive,
    std::string projectName)
    : category_id(categoryId)
    , category_name(categoryName)
    , color(color)
    , description(description)
    , date_created_utc(dateCreatedUtc)
    , date_modified_utc(dateModifiedUtc)
    , is_active(isActive)
    , project_name(projectName)
{
}
} // namespace app::models
