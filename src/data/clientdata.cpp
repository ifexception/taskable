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

#include "clientdata.h"

#include <wx/string.h>

#include "../common/util.h"
#include "employerdata.h"

namespace app::data
{
ClientData::ClientData()
{
    pConnection = db::ConnectionProvider::Get().Handle()->Acquire();
}

ClientData::~ClientData()
{
    db::ConnectionProvider::Get().Handle()->Release(pConnection);
}

int64_t ClientData::Create(std::unique_ptr<model::ClientModel> client)
{
    *pConnection->DatabaseExecutableHandle()
        << ClientData::createClient << std::string(client->GetName().ToUTF8()) << client->GetEmployerId();

    return pConnection->DatabaseExecutableHandle()->last_insert_rowid();
}

std::unique_ptr<model::ClientModel> ClientData::GetById(const int clientId)
{
    std::unique_ptr<model::ClientModel> client = nullptr;

    *pConnection->DatabaseExecutableHandle() << ClientData::getClientById << clientId >> [&](int clientsClientId,
                                                                                             std::string clientsName,
                                                                                             int clientsDateCreated,
                                                                                             int clientsDateModified,
                                                                                             int clientIsActive,
                                                                                             int clientsEmployerIdDb,
                                                                                             int employersEmployerId,
                                                                                             std::string employersName,
                                                                                             int employersDateCreated,
                                                                                             int employersDateModified,
                                                                                             int employersIsActive) {
        client = std::make_unique<model::ClientModel>(
            clientsClientId, wxString(clientsName), clientsDateCreated, clientsDateModified, clientIsActive);
        client->SetEmployerId(clientsEmployerIdDb);
        auto employer = std::make_unique<model::EmployerModel>(employersEmployerId,
            wxString(employersName),
            employersDateCreated,
            employersDateModified,
            employersIsActive);
        client->SetEmployer(std::move(employer));
    };
    return client;
}

void ClientData::Update(std::unique_ptr<model::ClientModel> client)
{
    *pConnection->DatabaseExecutableHandle()
        << ClientData::updateClient << std::string(client->GetName().ToUTF8()) << util::UnixTimestamp()
        << client->GetEmployerId() << client->GetClientId();
}

void ClientData::Delete(const int clientId)
{
    *pConnection->DatabaseExecutableHandle() << ClientData::deleteClient << util::UnixTimestamp() << clientId;
}

std::vector<std::unique_ptr<model::ClientModel>> ClientData::GetByEmployerId(const int employerId)
{
    std::vector<std::unique_ptr<model::ClientModel>> clients;

    *pConnection->DatabaseExecutableHandle() << ClientData::getClientsByEmployerId << employerId >>
        [&](int clientsClientId,
            std::string clientsName,
            int clientsDateCreated,
            int clientsDateModified,
            int clientIsActive,
            int clientsEmployerIdDb,
            int employersEmployerId,
            std::string employersName,
            int employersDateCreated,
            int employersDateModified,
            int employersIsActive) {
            auto client = std::make_unique<model::ClientModel>(
                clientsClientId, wxString(clientsName), clientsDateCreated, clientsDateModified, clientIsActive);
            client->SetEmployerId(clientsEmployerIdDb);
            auto employer = std::make_unique<model::EmployerModel>(employersEmployerId,
                wxString(employersName),
                employersDateCreated,
                employersDateModified,
                employersIsActive);
            client->SetEmployer(std::move(employer));
            clients.push_back(std::move(client));
        };

    return clients;
}

std::vector<std::unique_ptr<model::ClientModel>> ClientData::GetAll()
{
    std::vector<std::unique_ptr<model::ClientModel>> clients;

    *pConnection->DatabaseExecutableHandle() << ClientData::getClients >> [&](int clientsClientId,
                                                                              std::string clientsName,
                                                                              int clientsDateCreated,
                                                                              int clientsDateModified,
                                                                              int clientIsActive,
                                                                              int clientsEmployerId,
                                                                              int employersEmployerId,
                                                                              std::string employersName,
                                                                              int employersDateCreated,
                                                                              int employersDateModified,
                                                                              int employersIsActive) {
        auto client = std::make_unique<model::ClientModel>(
            clientsClientId, wxString(clientsName), clientsDateCreated, clientsDateModified, clientIsActive);
        client->SetEmployerId(clientsEmployerId);
        auto employer = std::make_unique<model::EmployerModel>(employersEmployerId,
            wxString(employersName),
            employersDateCreated,
            employersDateModified,
            employersIsActive);
        client->SetEmployer(std::move(employer));
        clients.push_back(std::move(client));
    };

    return clients;
}

const std::string ClientData::createClient = "INSERT INTO "
                                             "clients (name, is_active, employer_id) "
                                             "VALUES (?, 1, ?)";
;
const std::string ClientData::getClientsByEmployerId = "SELECT clients.client_id, "
                                                       "clients.name AS client_name, "
                                                       "clients.date_created, "
                                                       "clients.date_modified, "
                                                       "clients.is_active, "
                                                       "clients.employer_id, "
                                                       "employers.employer_id, "
                                                       "employers.name AS employer_name, "
                                                       "employers.date_created, "
                                                       "employers.date_modified, "
                                                       "employers.is_active "
                                                       "FROM clients "
                                                       "INNER JOIN employers "
                                                       "ON clients.employer_id = employers.employer_id "
                                                       "WHERE employers.employer_id = ?";
;
const std::string ClientData::getClients = "SELECT clients.client_id, "
                                           "clients.name AS client_name, "
                                           "clients.date_created, "
                                           "clients.date_modified, "
                                           "clients.is_active, "
                                           "clients.employer_id, "
                                           "employers.employer_id, "
                                           "employers.name AS employer_name, "
                                           "employers.date_created, "
                                           "employers.date_modified, "
                                           "employers.is_active "
                                           "FROM clients "
                                           "INNER JOIN employers "
                                           "ON clients.employer_id = employers.employer_id "
                                           "WHERE clients.is_active = 1";
;
const std::string ClientData::getClientById = "SELECT clients.client_id, "
                                              "clients.name AS client_name, "
                                              "clients.date_created, "
                                              "clients.date_modified, "
                                              "clients.is_active, "
                                              "clients.employer_id, "
                                              "employers.employer_id, "
                                              "employers.name AS employer_name, "
                                              "employers.date_created, "
                                              "employers.date_modified, "
                                              "employers.is_active "
                                              "FROM clients "
                                              "INNER JOIN employers "
                                              "ON clients.employer_id = employers.employer_id "
                                              "WHERE clients.client_id = ?";
;
const std::string ClientData::updateClient = "UPDATE clients "
                                             "SET name = ?, date_modified = ?, employer_id = ? "
                                             "WHERE client_id = ?";

const std::string ClientData::deleteClient = "UPDATE clients "
                                             "SET is_active = 0, date_modified = ? "
                                             "WHERE client_id = ?";

} // namespace app::data
