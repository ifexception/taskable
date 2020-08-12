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

#include <spdlog/spdlog.h>

#include "../common/util.h"
#include "employerdata.h"

namespace app::data
{
ClientData::ClientData()
    : bBorrowedConnection(false)
{
    pConnection = db::ConnectionProvider::Get().Handle()->Acquire();
    spdlog::get("msvc")->debug("ACQUIRE connection in ClientData|ConnectionTally: {0:d}",
        db::ConnectionProvider::Get().Handle()->ConnectionsInUse());
}

ClientData::ClientData(std::shared_ptr<db::SqliteConnection> connection)
    : bBorrowedConnection(true)
{
    pConnection = connection;
    spdlog::get("msvc")->debug("BORROW connection in ClientData|ConnectionTally: {0:d}",
        db::ConnectionProvider::Get().Handle()->ConnectionsInUse());
}

ClientData::~ClientData()
{
    if (!bBorrowedConnection) {
        db::ConnectionProvider::Get().Handle()->Release(pConnection);
        spdlog::get("msvc")->debug("RELEASE connection in ClientData|ConnectionTally: {0:d}",
            db::ConnectionProvider::Get().Handle()->ConnectionsInUse());
    }
}

int64_t ClientData::Create(std::unique_ptr<model::ClientModel> client)
{
    *pConnection->DatabaseExecutableHandle()
        << ClientData::createClient << std::string(client->GetName().ToUTF8()) << client->GetEmployerId();

    return pConnection->DatabaseExecutableHandle()->last_insert_rowid();
}

std::unique_ptr<model::ClientModel> ClientData::GetById(const int clientId)
{
    data::EmployerData data(pConnection);
    std::unique_ptr<model::ClientModel> client = nullptr;

    *pConnection->DatabaseExecutableHandle() << ClientData::getClientById << clientId >>
        [&](int clientId, std::string clientName, int dateCreated, int dateModified, int isActive, int employerId) {
            client = std::make_unique<model::ClientModel>(clientId, clientName, dateCreated, dateModified, isActive);
            client->SetEmployer(data.GetById(employerId));
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
    data::EmployerData data(pConnection);
    std::vector<std::unique_ptr<model::ClientModel>> clients;

    *pConnection->DatabaseExecutableHandle() << ClientData::getClientsByEmployerId << employerId >>
        [&](int clientId, std::string name, int dateCreated, int dateModified, int isActive, int employerIdDb) {
            auto client =
                std::make_unique<model::ClientModel>(clientId, wxString(name), dateCreated, dateModified, isActive);
            client->SetEmployer(data.GetById(employerId));
            clients.push_back(std::move(client));
        };

    return clients;
}

std::vector<std::unique_ptr<model::ClientModel>> ClientData::GetAll()
{
    data::EmployerData data(pConnection);
    std::vector<std::unique_ptr<model::ClientModel>> clients;

    *pConnection->DatabaseExecutableHandle() << ClientData::getClients >>
        [&](int clientId, std::string name, int dateCreated, int dateModified, bool isActive, int employerId) {
            auto client =
                std::make_unique<model::ClientModel>(clientId, wxString(name), dateCreated, dateModified, isActive);
            client->SetEmployer(data.GetById(employerId));
            clients.push_back(std::move(client));
        };

    return clients;
}

int64_t ClientData::GetLastInsertId() const
{
    return pConnection->DatabaseExecutableHandle()->last_insert_rowid();
}

const std::string ClientData::createClient = "INSERT INTO "
                                             "clients (name, is_active, employer_id) "
                                             "VALUES (?, 1, ?)";
;
const std::string ClientData::getClientsByEmployerId = "SELECT clients.client_id, "
                                                       "clients.name, "
                                                       "clients.date_created, "
                                                       "clients.date_modified, "
                                                       "clients.is_active, "
                                                       "clients.employer_id "
                                                       "FROM clients "
                                                       "WHERE employer_id = ?";
;
const std::string ClientData::getClients = "SELECT clients.client_id, "
                                           "clients.name AS client_name, "
                                           "clients.date_created, "
                                           "clients.date_modified, "
                                           "clients.is_active, "
                                           "clients.employer_id "
                                           "FROM clients "
                                           "WHERE clients.is_active = 1";
;
const std::string ClientData::getClientById = "SELECT clients.client_id, "
                                              "clients.name AS client_name, "
                                              "clients.date_created, "
                                              "clients.date_modified, "
                                              "clients.is_active, "
                                              "clients.employer_id "
                                              "FROM clients "
                                              "WHERE clients.client_id = ?";
;
const std::string ClientData::updateClient = "UPDATE clients "
                                             "SET name = ?, date_modified = ?, employer_id = ? "
                                             "WHERE client_id = ?";

const std::string ClientData::deleteClient = "UPDATE clients "
                                             "SET is_active = 0, date_modified = ? "
                                             "WHERE client_id = ?";

} // namespace app::data
