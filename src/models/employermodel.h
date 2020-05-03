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
    EmployerModel(const wxString& employerName);
    EmployerModel(int employerId, wxString name, int dateCreated, int dateModified, bool isActive);
    ~EmployerModel() = default;

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

private:
    int mEmployerId;
    wxString mName;
    wxDateTime mDateCreated;
    wxDateTime mDateModified;
    bool bIsActive;
};
} // namespace app::model
