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

#include <wx/datetime.h>
#include <wx/string.h>

#include "employermodel.h"
#include "clientmodel.h"

namespace app::model
{
class ProjectModel final
{
public:
    ProjectModel();
    ProjectModel(int projectId);
    ProjectModel(int projectId, wxString name, wxString displayName);
    ProjectModel(int projectId,
        wxString name,
        wxString displayName,
        bool isDefault,
        int dateCreated,
        int dateModified,
        bool isActive);
    bool HasClientLinked();

    const int GetProjectId() const;
    const wxString GetName() const;
    const wxString GetDisplayName() const;
    const bool IsDefault() const;
    const wxDateTime GetDateCreated();
    const wxDateTime GetDateModified();
    const bool IsActive() const;
    const int GetEmployerId() const;
    const int GetClientId() const;

    EmployerModel* GetEmployer();
    ClientModel* GetClient();

    void SetProjectId(const int projectId);
    void SetName(const wxString& name);
    void SetDisplayName(const wxString& displayName);
    void IsDefault(const bool isDefault);
    void SetDateCreated(const wxDateTime& dateCreated);
    void SetDateUpdated(const wxDateTime& dateUpdated);
    void IsActive(const bool isActive);
    void SetEmployerId(const int employerId);
    void SetClientId(const int clientId);

    void SetEmployer(std::unique_ptr<EmployerModel> employer);
    void SetClient(std::unique_ptr<ClientModel> client);

private:
    int mProjectId;
    wxString mName;
    wxString mDisplayName;
    bool bIsDefault;
    wxDateTime mDateCreated;
    wxDateTime mDateModified;
    bool bIsActive;
    int mEmployerId;
    int mClientId;

    std::unique_ptr<EmployerModel> pEmployer;
    std::unique_ptr<ClientModel> pClient;
};
} // namespace app::model
