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

#pragma once

#include <string>

#include <wx/string.h>
#include <wx/datetime.h>

#include "employermodel.h"

namespace app::model
{
class ClientModel final
{
public:
    ClientModel();
    ClientModel(const int clientId);
    ClientModel(int clientId, wxString name, int dateCreated, int dateModified, bool isActive);
    ~ClientModel() = default;

    bool IsNameValid();
    bool IsEmployerSelected();

    const int GetClientId() const;
    const wxString GetName() const;
    const wxDateTime GetDateCreated() const;
    const wxDateTime GetDateModified() const;
    const bool IsActive() const;
    const int GetEmployerId() const;

    EmployerModel* GetEmployer();

    void SetClientId(const int clientId);
    void SetName(const wxString& name);
    void SetDateCreated(const wxDateTime& dateCreated);
    void SetDateModified(const wxDateTime& dateModified);
    void IsActive(const bool isActive);
    void SetEmployerId(const int employerId);

    void SetEmployer(std::unique_ptr<EmployerModel> employer);

    static void Create(std::unique_ptr<model::ClientModel> client);
    static std::unique_ptr<ClientModel > GetById(const int clientId);
    static void Update(std::unique_ptr<model::ClientModel> client);
    static void Delete(std::unique_ptr<model::ClientModel> client);

private:
    int mClientId;
    wxString mName;
    wxDateTime mDateCreated;
    wxDateTime mDateModified;
    bool bIsActive;
    int mEmployerId;

    std::unique_ptr<EmployerModel> pEmployer;

    static const std::string createClient;
    static const std::string getClientsByEmployerId;
    static const std::string getClients;
    static const std::string getClientById;
    static const std::string updateClient;
    static const std::string deleteClient;
};
} // namespace app::model
