//  Tasks Tracker is a desktop that aids you in tracking your timesheets
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

namespace app::models
{
struct category
{
    category() = default;
    explicit category(int categoryId,
        std::string categoryName,
        int color,
        std::string description,
        int dateCreatedUtc,
        int dateModifiedUtc,
        int isActive,
        int projectId);
    explicit category(int categoryId,
        std::string categoryName,
        std::string description,
        int dateCreatedUtc,
        int dateModifiedUtc,
        int isActive,
        std::string projectName);
    explicit category(int categoryId,
        std::string categoryName,
        int color,
        std::string description,
        int dateCreatedUtc,
        int dateModifiedUtc,
        int isActive,
        std::string projectName);
    ~category() = default;

    int category_id;
    std::string category_name;
    int color;
    std::string description;
    int date_created_utc;
    int date_modified_utc;
    int is_active;
    int project_id;
    std::string project_name;

    static const std::string createNewCategory;
    static const std::string getCategoriesByProjectId;
    static const std::string getCategoryById;
    static const std::string getCategories;
    static const std::string updateCategory;
    static const std::string deleteCategory;
};
}
