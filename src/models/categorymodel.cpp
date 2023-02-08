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

#include "categorymodel.h"

#include "../common/util.h"

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
} // namespace app::model
