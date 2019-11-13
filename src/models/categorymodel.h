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

#include <memory>

#include <wx/colour.h>
#include <wx/string.h>

#define FMT_HEADER_ONLY
#include <spdlog/spdlog.h>

#include "projectmodel.h"

namespace app::model
{
class CategoryModel final
{
public:
    CategoryModel();
    CategoryModel(int id);
    CategoryModel(wxString name, wxColor color, int projectId, wxString projectDisplayName);
    CategoryModel(int id,
        wxString name,
        wxColor color,
        int dateCreated,
        int dateModified,
        bool isActive,
        wxString projectDisplayName);

    void Reset();

    bool IsNameValid();
    bool IsProjectSelected();

    const int GetCategoryId() const;
    const wxString GetName() const;
    const wxColor GetColor() const;
    const wxDateTime GetDateCreated() const;
    const wxDateTime GetDateModified() const;
    const bool IsActive() const;
    const int GetProjectId() const;

    ProjectModel& GetProject();

    void SetCategoryId(const int categoryId);
    void SetName(const wxString& name);
    void SetColor(const wxColor& color);
    void SetDateCreated(const wxDateTime& dateCreated);
    void SetDateModified(const wxDateTime& dateModified);
    void IsActive(const bool isActive);
    void SetProjectId(const int projectId);

    void SetProject(const ProjectModel& project);

    static void Create(const CategoryModel& category);
    static CategoryModel GetCategoryById(const int id);
    static void Update(const CategoryModel& category);
    static void Delete(const CategoryModel& category);

private:
    std::shared_ptr<spdlog::logger> pLogger;

    int mCategoryId;
    wxString mName;
    wxColor mColor;
    wxDateTime mDateCreated;
    wxDateTime mDateModified;
    bool bIsActive;
    int mProjectId;

    ProjectModel mProject;

    static const std::string createCategory;
    static const std::string getCategoryById;
    static const std::string updateCategory;
    static const std::string deleteCategory;
};
} // namespace app::model
