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

#include <memory>
#include <string>
#include <vector>

#include <wx/string.h>
#include <wx/datetime.h>

namespace app::model
{
class EmployerModel final
{
public:
    EmployerModel();
    EmployerModel(const int employerId);
    EmployerModel(const int employerId, bool loadFromDb);
    EmployerModel(int employerId, wxString name, int dateCreated, int dateModified, bool isActive);
    ~EmployerModel() = default;


    bool IsNameValid();

    const int GetEmployerId() const;
    const wxString GetName() const;
    const wxDateTime GetDateCreated() const;
    const wxDateTime GetDateModified() const;
    const bool IsActive() const;

    void SetEmployerId(const int employerId);
    void SetName(const wxString& name);
    void SetDateCreated(const wxDateTime& dateCreated);
    void SetDateModified(const wxDateTime& dateModified);
    void IsActive(const bool isActive);

    static void Create(std::unique_ptr<EmployerModel> employer);
    static std::unique_ptr<EmployerModel> GetById(const int id);
    static std::vector<std::unique_ptr<EmployerModel>> GetAll();
    static void Update(std::unique_ptr<EmployerModel> employer);
    static void Delete(std::unique_ptr<EmployerModel> employer);

private:
    int mEmployerId;
    wxString mName;
    wxDateTime mDateCreated;
    wxDateTime mDateModified;
    bool bIsActive;

    static const std::string createEmployer;
    static const std::string getEmployers;
    static const std::string getEmployer;
    static const std::string updateEmployer;
    static const std::string deleteEmployer;
};
} // namespace app::model
