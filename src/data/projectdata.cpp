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

#include "projectdata.h"

#include <spdlog/spdlog.h>

#include "../common/util.h"
#include "employerdata.h"
#include "clientdata.h"
#include "ratetypedata.h"
#include "currencydata.h"

namespace app::data
{
ProjectData::ProjectData()
{
    pConnection = db::ConnectionProvider::Get().Handle()->Acquire();
    spdlog::get("msvc")->debug("ACQUIRE connection in ProjectData|ConnectionTally: {0:d}",
        db::ConnectionProvider::Get().Handle()->ConnectionsInUse());
}

ProjectData::~ProjectData()
{
    db::ConnectionProvider::Get().Handle()->Release(pConnection);
    spdlog::get("msvc")->debug("RELEASE connection in ProjectData|ConnectionTally: {0:d}",
        db::ConnectionProvider::Get().Handle()->ConnectionsInUse());
}

void ProjectData::Create(std::unique_ptr<model::ProjectModel> project)
{
    auto ps = *pConnection->DatabaseExecutableHandle() << ProjectData::createProject;
    ps << project->GetName() << project->GetDisplayName() << project->IsBillable() << project->IsDefault()
       << project->GetEmployerId();

    if (project->HasClientLinked())
        ps << project->GetClientId();
    else
        ps << nullptr;

    if (project->IsNonBillableScenario())
        ps << nullptr << nullptr << nullptr;

    if (project->IsBillableWithUnknownRateScenario())
        ps << nullptr << project->GetRateTypeId() << nullptr;

    if (project->IsBillableScenarioWithHourlyRate())
        ps << *project->GetRate() << project->GetRateTypeId() << project->GetCurrencyId();

    ps.execute();
}

std::unique_ptr<model::ProjectModel> ProjectData::GetById(const int projectId)
{
    std::unique_ptr<model::ProjectModel> project = nullptr;
    data::EmployerData employerData;
    data::ClientData clientData;
    data::RateTypeData rateTypeData;
    data::CurrencyData currencyData;

    *pConnection->DatabaseExecutableHandle() << ProjectData::getProject << projectId >>
        [&](int projectId,
            std::string name,
            std::string displayName,
            int billable,
            int isDefault,
            std::unique_ptr<double> rate,
            int dateCreated,
            int dateModified,
            int isActive,
            int employerId,
            std::unique_ptr<int> clientId,
            std::unique_ptr<int> rateTypeId,
            std::unique_ptr<int> currencyId) {
            project = std::make_unique<model::ProjectModel>(projectId,
                wxString(name),
                wxString(displayName),
                billable,
                isDefault,
                dateCreated,
                dateModified,
                isActive);

            if (rate != nullptr) {
                project->SetRate(std::move(rate));
            }

            project->SetEmployerId(employerId);
            auto employer = employerData.GetById(employerId);
            project->SetEmployer(std::move(employer));

            if (clientId != nullptr) {
                project->SetClientId(*clientId);
                auto client = clientData.GetById(*clientId);
                project->SetClient(std::move(client));
            }

            if (rateTypeId != nullptr) {
                project->SetRateTypeId(*rateTypeId);
                auto rateType = rateTypeData.GetById(*rateTypeId);
                project->SetRateType(std::move(rateType));
            }

            if (currencyId != nullptr) {
                project->SetCurrencyId(*currencyId);
                auto currency = currencyData.GetById(*currencyId);
                project->SetCurrency(std::move(currency));
            }
        };

    return project;
}

void ProjectData::Update(std::unique_ptr<model::ProjectModel> project)
{
    auto ps = *pConnection->DatabaseExecutableHandle()
              << ProjectData::updateProject << project->GetName() << project->GetDisplayName() << project->IsBillable()
              << project->IsDefault() << util::UnixTimestamp() << project->GetEmployerId();

    if (project->HasClientLinked())
        ps << project->GetClientId();
    else
        ps << nullptr;

    if (project->IsNonBillableScenario())
        ps << nullptr << nullptr << nullptr;

    if (project->IsBillableWithUnknownRateScenario())
        ps << nullptr << project->GetRateTypeId() << nullptr;

    if (project->IsBillableScenarioWithHourlyRate())
        ps << *project->GetRate() << project->GetRateTypeId() << project->GetCurrencyId();

    ps << project->GetProjectId();

    ps.execute();
}

void ProjectData::Delete(const int projectId)
{
    *pConnection->DatabaseExecutableHandle() << ProjectData::deleteProject << util::UnixTimestamp() << projectId;
}

std::vector<std::unique_ptr<model::ProjectModel>> ProjectData::GetAll()
{
    std::vector<std::unique_ptr<model::ProjectModel>> projects;
    data::EmployerData employerData;
    data::ClientData clientData;
    data::RateTypeData rateTypeData;
    data::CurrencyData currencyData;

    *pConnection->DatabaseExecutableHandle() << ProjectData::getProjects >> [&](int projectId,
                                                                                std::string name,
                                                                                std::string displayName,
                                                                                int billable,
                                                                                int isDefault,
                                                                                std::unique_ptr<double> rate,
                                                                                int dateCreated,
                                                                                int dateModified,
                                                                                int isActive,
                                                                                int employerId,
                                                                                std::unique_ptr<int> clientId,
                                                                                std::unique_ptr<int> rateTypeId,
                                                                                std::unique_ptr<int> currencyId) {
        auto project = std::make_unique<model::ProjectModel>(
            projectId, wxString(name), wxString(displayName), billable, isDefault, dateCreated, dateModified, isActive);

        if (rate != nullptr) {
            project->SetRate(std::move(rate));
        }

        project->SetEmployerId(employerId);
        auto employer = employerData.GetById(employerId);
        project->SetEmployer(std::move(employer));
        if (clientId != nullptr) {
            project->SetClientId(*clientId);
            auto client = clientData.GetById(*clientId);
            project->SetClient(std::move(client));
        }

        if (rateTypeId != nullptr) {
            project->SetRateTypeId(*rateTypeId);
            auto rateType = rateTypeData.GetById(*rateTypeId);
            project->SetRateType(std::move(rateType));
        }

        if (currencyId != nullptr) {
            project->SetCurrencyId(*currencyId);
            auto currency = currencyData.GetById(*currencyId);
            project->SetCurrency(std::move(currency));
        }

        projects.push_back(std::move(project));
    };

    return projects;
}

void ProjectData::UnmarkDefaultProjects()
{
    *pConnection->DatabaseExecutableHandle() << ProjectData::unmarkDefaultProjects << util::UnixTimestamp();
}

int ProjectData::GetLastInsertId() const
{
    return (int) pConnection->DatabaseExecutableHandle()->last_insert_rowid();
}

const std::string ProjectData::createProject = "INSERT INTO "
                                               "projects(name, display_name, billable, is_default, is_active, "
                                               "employer_id, client_id, rate, rate_type_id, currency_id) "
                                               "VALUES(?, ?, ?, ?, 1, ?, ?, ?, ?, ?)";

const std::string ProjectData::getProject = "SELECT projects.project_id, "
                                            "projects.name AS project_name, "
                                            "projects.display_name, "
                                            "projects.billable, "
                                            "projects.is_default,"
                                            "projects.rate, "
                                            "projects.date_created, "
                                            "projects.date_modified, "
                                            "projects.is_active, "
                                            "employers.employer_id, "
                                            "clients.client_id AS client_id, "
                                            "rate_types.rate_type_id AS rate_type_id, "
                                            "currencies.currency_id AS currency_id "
                                            "FROM projects "
                                            "INNER JOIN employers ON projects.employer_id = employers.employer_id "
                                            "LEFT JOIN clients ON projects.client_id = clients.client_id "
                                            "LEFT JOIN rate_types ON projects.rate_type_id = rate_types.rate_type_id "
                                            "LEFT JOIN currencies ON projects.currency_id = currencies.currency_id "
                                            "WHERE projects.project_id = ?";

const std::string ProjectData::updateProject =
    "UPDATE projects "
    "SET name = ?, display_name = ?, billable = ?, is_default = ?, date_modified = ?, "
    "employer_id = ?, client_id = ?, rate = ?, rate_type_id = ?, currency_id = ? "
    "WHERE project_id = ?";

const std::string ProjectData::deleteProject = "UPDATE projects"
                                               "SET is_active = 0, date_modified = ?"
                                               "WHERE project_id = ?";

const std::string ProjectData::getProjects = "SELECT projects.project_id, "
                                             "projects.name AS project_name, "
                                             "projects.display_name, "
                                             "projects.billable, "
                                             "projects.is_default,"
                                             "projects.rate, "
                                             "projects.date_created, "
                                             "projects.date_modified, "
                                             "projects.is_active, "
                                             "employers.employer_id, "
                                             "clients.client_id AS client_id, "
                                             "rate_types.rate_type_id AS rate_type_id, "
                                             "currencies.currency_id AS currency_id "
                                             "FROM projects "
                                             "INNER JOIN employers ON projects.employer_id = employers.employer_id "
                                             "LEFT JOIN clients ON projects.client_id = clients.client_id "
                                             "LEFT JOIN rate_types ON projects.rate_type_id = rate_types.rate_type_id "
                                             "LEFT JOIN currencies ON projects.currency_id = currencies.currency_id "
                                             "WHERE projects.is_active = 1";

const std::string ProjectData::unmarkDefaultProjects = "UPDATE projects "
                                                       "SET is_default = 0, "
                                                       "date_modified = ?";

} // namespace app::data
