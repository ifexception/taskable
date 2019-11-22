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

#include "clientmodel.h"

#include "../common/constants.h"
#include "../common/util.h"
#include "../services/db_connection.h"

namespace app::model
{
ClientModel::ClientModel()
    : mClientId(-1)
    , mName(wxGetEmptyString())
    , mDateCreated(wxDefaultDateTime)
    , mDateModified(wxDefaultDateTime)
    , bIsActive(false)
    , mEmployerId(-1)
    , pEmployer(new EmployerModel())
{
}

ClientModel::ClientModel(const int clientId)
    : ClientModel()
{
    mClientId = clientId;
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

bool ClientModel::IsNameValid()
{
    return !mName.empty() && mName.length() >= 2 && mName.length() <= 255;
}

bool ClientModel::IsEmployerSelected()
{
    return mEmployerId > 0;
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

void ClientModel::Create(std::unique_ptr<model::ClientModel> client)
{
    auto db = services::db_connection::get_instance().get_handle();

    db << model::ClientModel::createClient << std::string(client->GetName().ToUTF8()) << client->GetEmployerId();
}

std::unique_ptr<ClientModel> ClientModel::GetById(const int clientId)
{
    std::unique_ptr<ClientModel> client = nullptr;

    auto db = services::db_connection::get_instance().get_handle();
    db << model::ClientModel::getClientById << clientId >> [&](int clientId,
                                                               std::string clientName,
                                                               int dateCreatedUtc,
                                                               int dateModifiedUtc,
                                                               int isActive,
                                                               int employerId) {
        client = std::make_unique<ClientModel>(clientId, clientName, dateCreatedUtc, dateModifiedUtc, isActive);
        client->SetEmployer(std::make_unique<EmployerModel>(employerId, true));
    };
    return client;
}

void ClientModel::Update(std::unique_ptr<model::ClientModel> client)
{
    auto db = services::db_connection::get_instance().get_handle();

    db << model::ClientModel::updateClient << std::string(client->GetName().ToUTF8()) << util::UnixTimestamp()
       << client->GetEmployerId() << client->GetClientId();
}

void ClientModel::Delete(std::unique_ptr<model::ClientModel> client)
{
    auto db = services::db_connection::get_instance().get_handle();

    db << model::ClientModel::deleteClient << util::UnixTimestamp() << client->GetClientId();
}

const std::string ClientModel::createClient = "INSERT INTO clients (name, is_active, employer_id) VALUES (?, 1, ?)";
;
const std::string ClientModel::getClientsByEmployerId = "SELECT clients.client_id, "
                                                        "clients.name AS client_name, "
                                                        "clients.date_created, "
                                                        "clients.date_modified, "
                                                        "clients.is_active, "
                                                        "FROM clients "
                                                        "WHERE employer_id = ?";
;
const std::string ClientModel::getClients = "SELECT clients.client_id, "
                                            "clients.name AS client_name, "
                                            "clients.date_created, "
                                            "clients.date_modified, "
                                            "clients.is_active, "
                                            "employers.name AS employer_name "
                                            "FROM clients "
                                            "INNER JOIN employers ON clients.employer_id = employers.employer_id "
                                            "WHERE clients.is_active = 1";
;
const std::string ClientModel::getClientById = "SELECT clients.client_id, "
                                               "clients.name AS client_name, "
                                               "clients.date_created, "
                                               "clients.date_modified, "
                                               "clients.is_active, "
                                               "employers.employer_id AS employer_id "
                                               "FROM clients "
                                               "INNER JOIN employers ON clients.employer_id = employers.employer_id "
                                               "WHERE clients.client_id = ?";
;
const std::string ClientModel::updateClient =
    "UPDATE clients SET name = ?, date_modified = ?, employer_id = ? WHERE client_id = ?";
const std::string ClientModel::deleteClient = "UPDATE clients SET is_active = 0, date_modified = ? WHERE client_id = ?";

} // namespace app::model
