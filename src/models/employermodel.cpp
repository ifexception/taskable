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

#include "employermodel.h"

#include "../common/constants.h"
#include "../common/util.h"
#include "../services/databaseconnection.h"

namespace app::model
{
EmployerModel::EmployerModel()
    : mEmployerId(0)
    , mName(wxGetEmptyString())
    , mDateCreated(wxDefaultDateTime)
    , mDateModified(wxDefaultDateTime)
    , bIsActive(false)
{
}

EmployerModel::EmployerModel(const int employerId)
    : EmployerModel()
{
    mEmployerId = employerId;
}

EmployerModel::EmployerModel(const int employerId, bool initializeFromDatabase)
    : EmployerModel()
{
    assert(initializeFromDatabase == true);
    auto employer = EmployerModel::GetById(employerId);
    mEmployerId = employer->GetEmployerId();
    mName = employer->GetName();
    mDateCreated = employer->GetDateCreated();
    mDateModified = employer->GetDateModified();
    bIsActive = employer->IsActive();
}

EmployerModel::EmployerModel(int employerId, wxString name, int dateCreated, int dateModified, bool isActive)
    : EmployerModel()
{
    mEmployerId = employerId;
    mName = name;
    mDateCreated = util::ToDateTime(dateCreated);
    mDateModified = util::ToDateTime(dateModified);
    bIsActive = isActive;
}

const int EmployerModel::GetEmployerId() const
{
    return mEmployerId;
}

const wxString EmployerModel::GetName() const
{
    return mName;
}

const wxDateTime EmployerModel::GetDateCreated() const
{
    return mDateCreated;
}

const wxDateTime EmployerModel::GetDateModified() const
{
    return mDateModified;
}

const bool EmployerModel::IsActive() const
{
    return bIsActive;
}

void EmployerModel::SetEmployerId(const int employerId)
{
    mEmployerId = employerId;
}

void EmployerModel::SetName(const wxString& name)
{
    mName = name;
}

void EmployerModel::SetDateCreated(const wxDateTime& dateCreated)
{
    mDateCreated = dateCreated;
}

void EmployerModel::SetDateModified(const wxDateTime& dateModified)
{
    mDateModified = dateModified;
}

void EmployerModel::IsActive(const bool isActive)
{
    bIsActive = isActive;
}

void EmployerModel::Create(std::unique_ptr<EmployerModel> employer)
{
    auto db = svc::DatabaseConnection::Get().GetHandle();
    *db << EmployerModel::createEmployer << employer->GetName().ToStdString();
}

std::unique_ptr<EmployerModel> EmployerModel::GetById(const int id)
{
    std::unique_ptr<EmployerModel> employer;

    auto db = svc::DatabaseConnection::Get().GetHandle();
    *db << EmployerModel::getEmployer << id >>
        [&](int employerId, std::string employerName, int dateCreated, int dateModified, int isActive) {
            employer = std::make_unique<EmployerModel>(
                employerId, wxString(employerName), dateCreated, dateModified, isActive);
        };

    return std::move(employer);
}

std::vector<std::unique_ptr<EmployerModel>> EmployerModel::GetAll()
{
    std::vector<std::unique_ptr<EmployerModel>> employers;

    auto db = svc::DatabaseConnection::Get().GetHandle();
    *db << model::EmployerModel::getEmployers >>
        [&](int employerId, std::string employerName, int dateCreated, int dateModified, int isActive) {
            auto employer = std::make_unique<EmployerModel>(
                employerId, wxString(employerName), dateCreated, dateModified, isActive);
            employers.push_back(std::move(employer));
        };

    return employers;
}

void EmployerModel::Update(std::unique_ptr<EmployerModel> employer)
{
    auto db = svc::DatabaseConnection::Get().GetHandle();
    *db << EmployerModel::updateEmployer << employer->GetName().ToStdString() << util::UnixTimestamp()
        << employer->GetEmployerId();
}

void EmployerModel::Delete(std::unique_ptr<EmployerModel> employer)
{
    auto db = svc::DatabaseConnection::Get().GetHandle();

    *db << EmployerModel::deleteEmployer << util::UnixTimestamp() << employer->GetEmployerId();
}

const std::string EmployerModel::createEmployer = "INSERT INTO employers (name, is_active) VALUES (?, 1);";

const std::string EmployerModel::getEmployers = "SELECT employer_id, "
                                                "name, "
                                                "date_created, "
                                                "date_modified, "
                                                "is_active "
                                                "FROM employers "
                                                "WHERE is_active = 1;";

const std::string EmployerModel::getEmployer = "SELECT employer_id, "
                                               "name, "
                                               "date_created, "
                                               "date_modified, "
                                               "is_active "
                                               "FROM employers "
                                               "WHERE employer_id = ?";

const std::string EmployerModel::updateEmployer =
    "UPDATE employers SET name = ?, date_modified = ? WHERE employer_id = ?";

const std::string EmployerModel::deleteEmployer =
    "UPDATE employers SET is_active = 0, date_modified = ? WHERE employer_id = ?";
} // namespace app::model
