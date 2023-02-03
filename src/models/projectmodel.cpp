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

#include "projectmodel.h"

#include "../common/util.h"

namespace app::model
{
ProjectModel::ProjectModel()
    : mProjectId(-1)
    , mName(wxGetEmptyString())
    , mDisplayName(wxGetEmptyString())
    , bIsDefault(false)
    , mDateCreated(wxDefaultDateTime)
    , mDateModified(wxDefaultDateTime)
    , bIsActive(false)
    , mEmployerId(-1)
    , mClientId(-1)
    , pEmployer(std::make_unique<EmployerModel>())
    , pClient(nullptr)
{
}

ProjectModel::ProjectModel(int projectId)
    : ProjectModel()
{
    mProjectId = projectId;
}

ProjectModel::ProjectModel(int projectId, wxString name, wxString displayName)
    : ProjectModel()
{
    mProjectId = projectId;
    mName = name;
    mDisplayName = displayName;
}

ProjectModel::ProjectModel(int projectId,
    wxString name,
    wxString displayName,
    bool isDefault,
    int dateCreated,
    int dateModified,
    bool isActive)
    : ProjectModel()
{
    mProjectId = projectId;
    mName = name;
    mDisplayName = displayName;
    bIsDefault = isDefault;
    mDateCreated = util::ToDateTime(dateCreated);
    mDateModified = util::ToDateTime(dateModified);
    bIsActive = isActive;
}

bool ProjectModel::HasClientLinked()
{
    return pClient != nullptr || mClientId > 0;
}

const int ProjectModel::GetProjectId() const
{
    return mProjectId;
}

const wxString ProjectModel::GetName() const
{
    return mName;
}

const wxString ProjectModel::GetDisplayName() const
{
    return mDisplayName;
}

const bool ProjectModel::IsDefault() const
{
    return bIsDefault;
}

const wxDateTime ProjectModel::GetDateCreated()
{
    return mDateCreated;
}

const wxDateTime ProjectModel::GetDateModified()
{
    return mDateModified;
}

const bool ProjectModel::IsActive() const
{
    return bIsActive;
}

const int ProjectModel::GetEmployerId() const
{
    return mEmployerId;
}

const int ProjectModel::GetClientId() const
{
    return mClientId;
}

EmployerModel* ProjectModel::GetEmployer()
{
    return pEmployer.get();
}

ClientModel* ProjectModel::GetClient()
{
    return pClient.get();
}

void ProjectModel::SetProjectId(const int projectId)
{
    mProjectId = projectId;
}

void ProjectModel::SetName(const wxString& name)
{
    mName = name;
}

void ProjectModel::SetDisplayName(const wxString& displayName)
{
    mDisplayName = displayName;
}

void ProjectModel::IsDefault(const bool isDefault)
{
    bIsDefault = isDefault;
}

void ProjectModel::SetDateCreated(const wxDateTime& dateCreated)
{
    mDateCreated = dateCreated;
}

void ProjectModel::SetDateUpdated(const wxDateTime& dateUpdated)
{
    mDateModified = dateUpdated;
}

void ProjectModel::IsActive(const bool isActive)
{
    bIsActive = isActive;
}

void ProjectModel::SetEmployerId(const int employerId)
{
    mEmployerId = employerId;
}

void ProjectModel::SetClientId(const int clientId)
{
    mClientId = clientId;
}

void ProjectModel::SetEmployer(std::unique_ptr<EmployerModel> employer)
{
    pEmployer = std::move(employer);
}

void ProjectModel::SetClient(std::unique_ptr<ClientModel> client)
{
    pClient = std::move(client);
}
} // namespace app::model
