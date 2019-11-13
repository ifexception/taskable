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

#include "categorymodel.h"

#include "../common/constants.h"
#include "../common/util.h"
#include "../services/db_connection.h"

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
    , mProject()
{
    pLogger = spdlog::get(Constants::LoggerName);
}

CategoryModel::CategoryModel(int id)
    : CategoryModel()
{
    mCategoryId = id;
}

CategoryModel::CategoryModel(wxString name, wxColor color, int projectId, wxString projectDisplayName)
    : CategoryModel()
{
    mName = name;
    mColor = color;
    mProjectId = projectId;
    mProject.SetDisplayName(projectDisplayName);
}

CategoryModel::CategoryModel(int id,
    wxString name,
    wxColor color,
    int dateCreated,
    int dateModified,
    bool isActive,
    wxString projectDisplayName)
    : CategoryModel()
{
    mCategoryId = id;
    mName = name;
    mColor = color;
    mDateCreated = util::ToDateTime(dateCreated);
    mDateModified = util::ToDateTime(dateModified);
    bIsActive = isActive;
    mProject.SetDisplayName(projectDisplayName);
}

void CategoryModel::Reset()
{
    mCategoryId = 0;
    mName = wxGetEmptyString();
    mColor = wxColor(0, 0, 0);
    mDateCreated = wxDefaultDateTime;
    mDateModified = wxDefaultDateTime;
    bIsActive = false;
    mProjectId = 0;

    mProject.Reset();
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

ProjectModel& CategoryModel::GetProject()
{
    return mProject;
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

void CategoryModel::SetProject(const ProjectModel& project)
{
    mProject = project;
}

void CategoryModel::Create(const CategoryModel& category)
{
    std::string name = std::string(category.GetName().ToUTF8());
    unsigned int color = static_cast<unsigned int>(category.GetColor().GetRGB());

    auto db = services::db_connection::get_instance().get_handle();
    db << CategoryModel::createCategory << name << color << category.GetProjectId();
}

CategoryModel CategoryModel::GetCategoryById(const int id)
{
    CategoryModel category;

    auto db = services::db_connection::get_instance().get_handle();
    db << CategoryModel::getCategoryById << id >> [&](int categoryId,
                                                      std::string categoryName,
                                                      unsigned int color,
                                                      int dateCreated,
                                                      int dateModified,
                                                      int isActive,
                                                      std::string projectName) {
        category = CategoryModel(categoryId, categoryName, color, dateCreated, dateModified, isActive, projectName);
    };

    return category;
}

void CategoryModel::Update(const CategoryModel& category)
{
    std::string name = std::string(category.GetName().ToUTF8());
    unsigned int color = static_cast<unsigned int>(category.GetColor().GetRGB());

    auto db = services::db_connection::get_instance().get_handle();
    db << CategoryModel::updateCategory << name << color
       << category.GetProjectId() << util::UnixTimestamp() << category.GetCategoryId();
}

void CategoryModel::Delete(const CategoryModel& category)
{
    auto db = services::db_connection::get_instance().get_handle();
    db << CategoryModel::deleteCategory << util::UnixTimestamp() << category.GetCategoryId();
}

const std::string CategoryModel::createCategory = "INSERT INTO categories (name, color, is_active, project_id) "
                                                  "VALUES (?, ?, 1, ?)";

const std::string CategoryModel::getCategoryById = "SELECT categories.category_id, "
                                                   "categories.name AS category_name, "
                                                   "categories.color, "
                                                   "categories.date_created, "
                                                   "categories.date_modified, "
                                                   "categories.is_active, "
                                                   "projects.display_name "
                                                   "FROM categories "
                                                   "INNER JOIN projects ON categories.project_id = projects.project_id "
                                                   "WHERE categories.category_id = ?"
                                                   "AND categories.is_active = 1";

const std::string CategoryModel::updateCategory = "UPDATE categories SET name = ?, color = ?, project_id = "
                                                  "?, date_modified = ? WHERE category_id = ?";

const std::string CategoryModel::deleteCategory = "UPDATE categories SET is_active = 0, date_modified = ? "
                                                  "WHERE category_id = ?";
} // namespace app::model
