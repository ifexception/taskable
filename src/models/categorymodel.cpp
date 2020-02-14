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

#include "categorymodel.h"

#include "../common/constants.h"
#include "../common/util.h"
#include "../services/databaseconnection.h"

namespace app::model
{
CategoryModel::CategoryModel()
    : mCategoryId(0)
    , mName(wxGetEmptyString())
    , mColor(0, 0, 0)
    , mDateCreated(wxDefaultDateTime)
    , mDateModified(wxDefaultDateTime)
    , bIsActive(false)
    , mProjectId(0)
    , pProject(std::make_unique<ProjectModel>())
{
}

CategoryModel::CategoryModel(int id)
    : CategoryModel()
{
    mCategoryId = id;
}

CategoryModel::CategoryModel(int categoryId, bool initializeFromDatabase)
    : CategoryModel()
{
    auto category = CategoryModel::GetById(categoryId);
    mCategoryId = category->GetCategoryId();
    mName = category->GetName();
    mColor = category->GetColor();
    mDateCreated = category->GetDateCreated();
    mDateModified = category->GetDateModified();
    bIsActive = category->IsActive();
}

CategoryModel::CategoryModel(wxString name, wxColor color, int projectId)
    : CategoryModel()
{
    mName = name;
    mColor = color;
    mProjectId = projectId;
}

CategoryModel::CategoryModel(int id, wxString name, wxColor color, int dateCreated, int dateModified, bool isActive)
    : CategoryModel()
{
    mCategoryId = id;
    mName = name;
    mColor = color;
    mDateCreated = util::ToDateTime(dateCreated);
    mDateModified = util::ToDateTime(dateModified);
    bIsActive = isActive;
}

bool CategoryModel::IsNameValid()
{
    return !mName.empty() && mName.length() >= 2 && mName.length() <= 255;
}

bool CategoryModel::IsProjectSelected()
{
    return mProjectId > 0;
}

const int CategoryModel::GetCategoryId() const
{
    return mCategoryId;
}

const wxString CategoryModel::GetName() const
{
    return mName;
}

const wxColor CategoryModel::GetColor() const
{
    return mColor;
}

const wxDateTime CategoryModel::GetDateCreated() const
{
    return mDateCreated;
}

const wxDateTime CategoryModel::GetDateModified() const
{
    return mDateModified;
}

const bool CategoryModel::IsActive() const
{
    return bIsActive;
}

const int CategoryModel::GetProjectId() const
{
    return mProjectId;
}

ProjectModel* CategoryModel::GetProject()
{
    return pProject.get();
}

void CategoryModel::SetCategoryId(const int categoryId)
{
    mCategoryId = categoryId;
}

void CategoryModel::SetName(const wxString& name)
{
    mName = name;
}

void CategoryModel::SetColor(const wxColor& color)
{
    mColor = color;
}

void CategoryModel::SetDateCreated(const wxDateTime& dateCreated)
{
    mDateCreated = dateCreated;
}

void CategoryModel::SetDateModified(const wxDateTime& dateModified)
{
    mDateModified = dateModified;
}

void CategoryModel::IsActive(const bool isActive)
{
    bIsActive = isActive;
}

void CategoryModel::SetProjectId(const int projectId)
{
    mProjectId = projectId;
}

void CategoryModel::SetProject(std::unique_ptr<ProjectModel> project)
{
    pProject = std::move(project);
}

void CategoryModel::Create(std::unique_ptr<CategoryModel> category)
{
    unsigned int color = static_cast<unsigned int>(category->GetColor().GetRGB());

    auto db = svc::DatabaseConnection::Get().GetHandle();
    *db << CategoryModel::createCategory << category->GetName().ToStdString() << color << category->GetProjectId();
}

std::unique_ptr<CategoryModel> CategoryModel::GetById(const int id)
{
    std::unique_ptr<CategoryModel> category = nullptr;

    auto db = svc::DatabaseConnection::Get().GetHandle();
    *db << CategoryModel::getCategoryById << id >> [&](int categoryId,
                                                      std::string categoryName,
                                                      unsigned int color,
                                                      int dateCreated,
                                                      int dateModified,
                                                      int isActive,
                                                      int projectId) {
        category =
            std::make_unique<CategoryModel>(categoryId, categoryName, color, dateCreated, dateModified, isActive);
        auto project = std::make_unique<ProjectModel>(projectId, true);
        category->SetProject(std::move(project));
    };

    return category;
}

void CategoryModel::Update(std::unique_ptr<CategoryModel> category)
{
    unsigned int color = static_cast<unsigned int>(category->GetColor().GetRGB());

    auto db = svc::DatabaseConnection::Get().GetHandle();
    *db << CategoryModel::updateCategory << category->GetName().ToStdString() << color << category->GetProjectId()
       << util::UnixTimestamp() << category->GetCategoryId();
}

void CategoryModel::Delete(std::unique_ptr<CategoryModel> category)
{
    auto db = svc::DatabaseConnection::Get().GetHandle();
    *db << CategoryModel::deleteCategory << util::UnixTimestamp() << category->GetCategoryId();
}

std::vector<std::unique_ptr<CategoryModel>> CategoryModel::GetByProjectId(const int projectId)
{
    std::vector<std::unique_ptr<CategoryModel>> categories;
    auto db = svc::DatabaseConnection::Get().GetHandle();
    *db << CategoryModel::getCategoriesByProjectId << projectId >> [&](int categoryId,
                                                                      std::string categoryName,
                                                                      unsigned int color,
                                                                      int dateCreated,
                                                                      int dateModified,
                                                                      int isActive,
                                                                      int projectId) {
        auto category =
            std::make_unique<CategoryModel>(categoryId, categoryName, color, dateCreated, dateModified, isActive);
        auto project = std::make_unique<ProjectModel>(projectId, true);
        category->SetProject(std::move(project));
        categories.push_back(std::move(category));
    };

    return categories;
}

std::vector<std::unique_ptr<CategoryModel>> CategoryModel::GetAll()
{
    std::vector<std::unique_ptr<CategoryModel>> categories;

    auto db = svc::DatabaseConnection::Get().GetHandle();
    *db << CategoryModel::getCategoryById >> [&](int categoryId,
                                                std::string categoryName,
                                                unsigned int color,
                                                int dateCreated,
                                                int dateModified,
                                                int isActive,
                                                int projectId) {
        auto category =
            std::make_unique<CategoryModel>(categoryId, categoryName, color, dateCreated, dateModified, isActive);
        auto project = std::make_unique<ProjectModel>(projectId, true);
        category->SetProject(std::move(project));
        categories.push_back(std::move(category));
    };

    return categories;
}

const std::string CategoryModel::createCategory = "INSERT INTO categories (name, color, is_active, project_id) "
                                                  "VALUES (?, ?, 1, ?)";

const std::string CategoryModel::getCategoryById = "SELECT categories.category_id, "
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

const std::string CategoryModel::updateCategory = "UPDATE categories SET name = ?, color = ?, project_id = "
                                                  "?, date_modified = ? WHERE category_id = ?";

const std::string CategoryModel::deleteCategory = "UPDATE categories SET is_active = 0, date_modified = ? "
                                                  "WHERE category_id = ?";

const std::string CategoryModel::getCategoriesByProjectId = "SELECT categories.category_id, "
                                                            "categories.name, "
                                                            "categories.color, "
                                                            "categories.date_created, "
                                                            "categories.date_modified, "
                                                            "categories.is_active, "
                                                            "categories.project_id "
                                                            "FROM categories "
                                                            "WHERE categories.project_id = ?";

const std::string CategoryModel::getCategories = "SELECT categories.category_id, "
                                                 "categories.name, "
                                                 "categories.date_created, "
                                                 "categories.date_modified, "
                                                 "categories.is_active, "
                                                 "categories.project_id "
                                                 "FROM categories "
                                                 "WHERE categories.is_active = 1";
} // namespace app::model
