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

#include "employerdata.h"

#include <spdlog/spdlog.h>

#include "../common/util.h"

namespace app::data
{
EmployerData::EmployerData()
{
    pConnection = db::ConnectionProvider::Get().Handle()->Acquire();
    spdlog::get("msvc")->debug("ACQUIRE connection in EmployerData|ConnectionTally: {0:d}",
        db::ConnectionProvider::Get().Handle()->ConnectionsInUse());
}

EmployerData::~EmployerData()
{
    db::ConnectionProvider::Get().Handle()->Release(pConnection);
    spdlog::get("msvc")->debug("RELEASE connection in EmployerData|ConnectionTally: {0:d}",
        db::ConnectionProvider::Get().Handle()->ConnectionsInUse());
}

int64_t EmployerData::Create(std::unique_ptr<model::EmployerModel> employer)
{
    *pConnection->DatabaseExecutableHandle() << EmployerData::createEmployer << employer->GetName().ToStdString();
    return pConnection->DatabaseExecutableHandle()->last_insert_rowid();
}

std::unique_ptr<model::EmployerModel> EmployerData::GetById(const int employerId)
{
    std::unique_ptr<model::EmployerModel> employer;

    *pConnection->DatabaseExecutableHandle() << EmployerData::getEmployer << employerId >>
        [&](int employerId, std::string employerName, int dateCreated, int dateModified, int isActive) {
            employer = std::make_unique<model::EmployerModel>(
                employerId, wxString(employerName), dateCreated, dateModified, isActive);
        };

    return std::move(employer);
}

std::vector<std::unique_ptr<model::EmployerModel>> EmployerData::GetAll()
{
    std::vector<std::unique_ptr<model::EmployerModel>> employers;

    *pConnection->DatabaseExecutableHandle() << EmployerData::getEmployers >>
        [&](int employerId, std::string employerName, int dateCreated, int dateModified, int isActive) {
            auto employer = std::make_unique<model::EmployerModel>(
                employerId, wxString(employerName), dateCreated, dateModified, isActive);
            employers.push_back(std::move(employer));
        };

    return employers;
}

void EmployerData::Update(std::unique_ptr<model::EmployerModel> employer)
{
    *pConnection->DatabaseExecutableHandle() << EmployerData::updateEmployer << employer->GetName().ToStdString()
                                             << util::UnixTimestamp() << employer->GetEmployerId();
}

void EmployerData::Delete(const int employerId)
{
    *pConnection->DatabaseExecutableHandle() << EmployerData::deleteEmployer << util::UnixTimestamp() << employerId;
}

int64_t EmployerData::GetLastInsertId() const
{
    return pConnection->DatabaseExecutableHandle()->last_insert_rowid();
}

const std::string EmployerData::createEmployer = "INSERT INTO employers (name, is_active) VALUES (?, 1);";

const std::string EmployerData::getEmployers = "SELECT employer_id, "
                                               "name, "
                                               "date_created, "
                                               "date_modified, "
                                               "is_active "
                                               "FROM employers "
                                               "WHERE is_active = 1;";

const std::string EmployerData::getEmployer = "SELECT employer_id, "
                                              "name, "
                                              "date_created, "
                                              "date_modified, "
                                              "is_active "
                                              "FROM employers "
                                              "WHERE employer_id = ?";

const std::string EmployerData::updateEmployer =
    "UPDATE employers SET name = ?, date_modified = ? WHERE employer_id = ?";

const std::string EmployerData::deleteEmployer =
    "UPDATE employers SET is_active = 0, date_modified = ? WHERE employer_id = ?";
} // namespace app::data
