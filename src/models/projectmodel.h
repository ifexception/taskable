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

#include <vector>

#include <wx/string.h>

namespace app::model
{
class ProjectModel final
{
public:
    ProjectModel();
    ProjectModel(int projectId, wxString name, wxString displayName);

    void Reset();

    const int GetProjectId() const;
    const wxString GetName() const;
    const wxString GetDisplayName() const;
    const int GetDateCreated();
    const int GetDateUpdated();
    const bool IsActive() const;

    void SetProjectId(const int projectId);
    void SetName(const wxString& name);
    void SetDisplayName(const wxString& displayName);
    void SetDateCreated(const int dateCreated);
    void SetDateUpdated(const int dateUpdated);
    void IsActive(const bool isActive);

    static std::vector<model::ProjectModel> GetAllProjects();

private:
    int mProjectId;
    wxString mName;
    wxString mDisplayName;
    int mDateCreated;
    int mDateUpdated;
    bool bIsActive;

    static const std::string getProjects;
};
} // namespace app::model
