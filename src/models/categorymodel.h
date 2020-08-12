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

#include <wx/colour.h>
#include <wx/string.h>

#include "projectmodel.h"

namespace app::model
{
class CategoryModel final
{
public:
    CategoryModel();
    CategoryModel(int categoryId);
    CategoryModel(wxString name, wxColor color, int projectId);
    CategoryModel(int id, wxString name, wxColor color, int dateCreated, int dateModified, bool isActive);

    bool IsNameValid();
    bool IsProjectSelected();

    const int GetCategoryId() const;
    const wxString GetName() const;
    const wxColor GetColor() const;
    const wxDateTime GetDateCreated() const;
    const wxDateTime GetDateModified() const;
    const bool IsActive() const;
    const int GetProjectId() const;

    ProjectModel* GetProject();

    void SetCategoryId(const int categoryId);
    void SetName(const wxString& name);
    void SetColor(const wxColor& color);
    void SetDateCreated(const wxDateTime& dateCreated);
    void SetDateModified(const wxDateTime& dateModified);
    void IsActive(const bool isActive);
    void SetProjectId(const int projectId);

    void SetProject(std::unique_ptr<ProjectModel> project);

private:
    int mCategoryId;
    wxString mName;
    wxColor mColor;
    wxDateTime mDateCreated;
    wxDateTime mDateModified;
    bool bIsActive;
    int mProjectId;

    std::unique_ptr<ProjectModel> pProject;
};
} // namespace app::model
