//  TimesheetsTracker is a desktop that aids you in tracking your timesheets
//  and seeing what work you have done.
//
//  Copyright(C) <2018> <Szymon Welgus>
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

#include "client.h"

namespace app::models
{
const std::string client::createNewClient = "INSERT INTO clients (name, is_active, employer_id) VALUES (?, 1, ?)";
const std::string client::getClientsByEmployerId = "SELECT * FROM clients WHERE employer_id = ?";
const std::string client::getClients = "SELECT clients.client_id, "
                                       "clients.name AS client_name, "
                                       "clients.date_created_utc, "
                                       "clients.date_modified_utc, "
                                       "clients.is_active, "
                                       "employers.name AS employer_name "
                                       "FROM clients "
                                       "INNER JOIN employers ON clients.employer_id = employers.employer_id "
                                       "WHERE clients.is_active = 1";
const std::string client::getClientById = "SELECT clients.client_id, "
                                          "clients.name AS client_name, "
                                          "clients.date_created_utc, "
                                          "clients.date_modified_utc, "
                                          "clients.is_active, "
                                          "employers.name AS employer_name "
                                          "FROM clients "
                                          "INNER JOIN employers ON clients.employer_id = employers.employer_id "
                                          "WHERE clients.client_id = ?";
const std::string client::updateClient =
    "UPDATE clients SET name = ?, date_modified_utc = ?, employer_id = ? WHERE client_id = ?";
const std::string client::deleteClient = "UPDATE clients SET is_active = 0, date_modified_utc = ? WHERE client_id = ?";

client::client(int clientId,
    std::string clientName,
    int dateCreatedUtc,
    int dateModifiedUtc,
    int isActive,
    int employerId)
    : client_id(clientId)
    , client_name(clientName)
    , date_created_utc(dateCreatedUtc)
    , date_modified_utc(dateModifiedUtc)
    , is_active(isActive)
    , employer_id(employerId)
{
}

client::client(int clientId,
    std::string clientName,
    int dateCreatedUtc,
    int dateModifiedUtc,
    int isActive,
    std::string employerName)
    : client_id(clientId)
    , client_name(clientName)
    , date_created_utc(dateCreatedUtc)
    , date_modified_utc(dateModifiedUtc)
    , is_active(isActive)
    , employer_name(employerName)
{
}
} // namespace app::models
