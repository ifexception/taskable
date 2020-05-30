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

#include "clientmodel.h"

#include "../common/util.h"

namespace app::model
{
ClientModel::ClientModel()
    : mClientId(-1)
    , mName(wxGetEmptyString())
    , mDateCreated(wxDefaultDateTime)
    , mDateModified(wxDefaultDateTime)
    , bIsActive(false)
    , mEmployerId(-1)
    , pEmployer(std::make_unique<EmployerModel>())
{
}

ClientModel::ClientModel(const int clientId)
    : ClientModel()
{
    mClientId = clientId;
}

ClientModel::ClientModel(wxString name, int employerId)
    : ClientModel()
{
    mName = name;
    mEmployerId = employerId;
    bIsActive = true;
}

ClientModel::ClientModel(int clientId, wxString name, int dateCreated, int dateModified, bool isActive)
    : ClientModel()
{
    mClientId = clientId;
    mName = name;
    mDateCreated = util::ToDateTime(dateCreated);
    mDateModified = util::ToDateTime(dateModified);
    bIsActive = isActive;
}

const int ClientModel::GetClientId() const
{
    return mClientId;
}

const wxString ClientModel::GetName() const
{
    return mName;
}

const wxDateTime ClientModel::GetDateCreated() const
{
    return mDateCreated;
}

const wxDateTime ClientModel::GetDateModified() const
{
    return mDateModified;
}

const bool ClientModel::IsActive() const
{
    return bIsActive;
}

const int ClientModel::GetEmployerId() const
{
    return mEmployerId;
}

EmployerModel* ClientModel::GetEmployer()
{
    return pEmployer.get();
}

void ClientModel::SetClientId(const int clientId)
{
    mClientId = clientId;
}

void ClientModel::SetName(const wxString& name)
{
    mName = name;
}

void ClientModel::SetDateCreated(const wxDateTime& dateCreated)
{
    mDateCreated = dateCreated;
}

void ClientModel::SetDateModified(const wxDateTime& dateModified)
{
    mDateModified = dateModified;
}

void ClientModel::IsActive(const bool isActive)
{
    bIsActive = isActive;
}

void ClientModel::SetEmployerId(const int employerId)
{
    mEmployerId = employerId;
}

void ClientModel::SetEmployer(std::unique_ptr<EmployerModel> employer)
{
    pEmployer = std::move(employer);
}
} // namespace app::model
