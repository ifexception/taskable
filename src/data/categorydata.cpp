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

#include "categorydata.h"

#include <spdlog/spdlog.h>

#include "projectdata.h"

#include "../common/util.h"

namespace app::data
{
CategoryData::CategoryData()
{
    spdlog::get("msvc")->debug("ACQUIRE connection in CategoryData");
    pConnection = db::ConnectionProvider::Get().Handle()->Acquire();
}

CategoryData::~CategoryData()
{
    spdlog::get("msvc")->debug("RELEASE connection in CategoryData");
    db::ConnectionProvider::Get().Handle()->Release(pConnection);
}

int64_t CategoryData::Create(std::unique_ptr<model::CategoryModel> category)
{
    unsigned int color = static_cast<unsigned int>(category->GetColor().GetRGB());

    *pConnection->DatabaseExecutableHandle()
        << CategoryData::createCategory << category->GetName().ToStdString() << color << category->GetProjectId();
    return pConnection->DatabaseExecutableHandle()->last_insert_rowid();
}

std::unique_ptr<model::CategoryModel> CategoryData::GetById(const int id)
{
    std::unique_ptr<model::CategoryModel> category = nullptr;

    *pConnection->DatabaseExecutableHandle() << CategoryData::getCategoryById << id >> [&](int categoryId,
                                                                                           std::string categoryName,
                                                                                           unsigned int color,
                                                                                           int dateCreated,
                                                                                           int dateModified,
                                                                                           int isActive,
                                                                                           int projectId) {
        category = std::make_unique<model::CategoryModel>(
            categoryId, categoryName, color, dateCreated, dateModified, isActive);

        data::ProjectData mProjectData;
        category->SetProject(std::move(mProjectData.GetById(projectId)));
    };

    return category;
}

void CategoryData::Update(std::unique_ptr<model::CategoryModel> category)
{
    unsigned int color = static_cast<unsigned int>(category->GetColor().GetRGB());

    *pConnection->DatabaseExecutableHandle()
        << CategoryData::updateCategory << category->GetName().ToStdString() << color << category->GetProjectId()
        << util::UnixTimestamp() << category->GetCategoryId();
}

void CategoryData::Delete(int categoryId)
{
    *pConnection->DatabaseExecutableHandle() << CategoryData::deleteCategory << util::UnixTimestamp() << categoryId;
}

std::vector<std::unique_ptr<model::CategoryModel>> CategoryData::GetByProjectId(const int projectId)
{
    std::vector<std::unique_ptr<model::CategoryModel>> categories;

    *pConnection->DatabaseExecutableHandle() << CategoryData::getCategoriesByProjectId << projectId >>
        [&](int categoryId,
            std::string categoryName,
            unsigned int color,
            int dateCreated,
            int dateModified,
            int isActive,
            int projectId) {
            auto category = std::make_unique<model::CategoryModel>(
                categoryId, categoryName, color, dateCreated, dateModified, isActive);

            data::ProjectData mProjectData;
            category->SetProject(std::move(mProjectData.GetById(projectId)));
            categories.push_back(std::move(category));
        };

    return categories;
}

std::vector<std::unique_ptr<model::CategoryModel>> CategoryData::GetAll()
{
    std::vector<std::unique_ptr<model::CategoryModel>> categories;

    *pConnection->DatabaseExecutableHandle() << CategoryData::getCategories >> [&](int categoryId,
                                                                                   std::string categoryName,
                                                                                   unsigned int color,
                                                                                   int dateCreated,
                                                                                   int dateModified,
                                                                                   int isActive,
                                                                                   int projectId) {
        auto category = std::make_unique<model::CategoryModel>(
            categoryId, categoryName, color, dateCreated, dateModified, isActive);

        data::ProjectData mProjectData;
        category->SetProject(std::move(mProjectData.GetById(projectId)));
        categories.push_back(std::move(category));
    };

    return categories;
}

const std::string CategoryData::createCategory = "INSERT INTO categories (name, color, is_active, project_id) "
                                                 "VALUES (?, ?, 1, ?)";

const std::string CategoryData::getCategoryById = "SELECT categories.category_id, "
                                                  "categories.name AS category_name, "
                                                  "categories.color, "
                                                  "categories.date_created, "
                                                  "categories.date_modified, "
                                                  "categories.is_active, "
                                                  "projects.project_id "
                                                  "FROM categories "
                                                  "INNER JOIN projects ON categories.project_id = projects.project_id "
                                                  "WHERE categories.category_id = ? "
                                                  "AND categories.is_active = 1";

const std::string CategoryData::updateCategory = "UPDATE categories SET name = ?, color = ?, project_id = "
                                                 "?, date_modified = ? WHERE category_id = ?";

const std::string CategoryData::deleteCategory = "UPDATE categories SET is_active = 0, date_modified = ? "
                                                 "WHERE category_id = ?";

const std::string CategoryData::getCategoriesByProjectId = "SELECT categories.category_id, "
                                                           "categories.name, "
                                                           "categories.color, "
                                                           "categories.date_created, "
                                                           "categories.date_modified, "
                                                           "categories.is_active, "
                                                           "categories.project_id "
                                                           "FROM categories "
                                                           "WHERE categories.project_id = ?";

const std::string CategoryData::getCategories = "SELECT categories.category_id, "
                                                "categories.name, "
                                                "categories.color, "
                                                "categories.date_created, "
                                                "categories.date_modified, "
                                                "categories.is_active, "
                                                "categories.project_id "
                                                "FROM categories "
                                                "WHERE categories.is_active = 1";
} // namespace app::data
