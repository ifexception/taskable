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
#include <vector>

#include <wx/datetime.h>
#include <wx/string.h>

#include "../common/constants.h"
#include "employermodel.h"
#include "clientmodel.h"
#include "ratetypemodel.h"
#include "currencymodel.h"

namespace app::model
{
class ProjectModel final
{
public:
    ProjectModel();
    ProjectModel(int projectId);
    ProjectModel(int projectId, wxString name, wxString displayName);
    ProjectModel(wxString name,
        wxString displayName,
        bool billable,
        std::unique_ptr<double> rate,
        int rateTypeId,
        int currencyId);
    ProjectModel(int projectId,
        wxString name,
        wxString displayName,
        bool billable,
        bool isDefault,
        int dateCreated,
        int dateModified,
        bool isActive);

    bool IsNonBillableScenario();
    bool IsBillableWithUnknownRateScenario();
    bool IsBillableScenarioWithHourlyRate();
    bool HasClientLinked();

    void SwitchOutOfBillableScenario();
    void SwitchInToUnknownRateBillableScenario();

    const int GetProjectId() const;
    const wxString GetName() const;
    const wxString GetDisplayName() const;
    const bool IsBillable() const;
    const double* GetRate() const;
    const bool IsDefault() const;
    const wxDateTime GetDateCreated();
    const wxDateTime GetDateModified();
    const bool IsActive() const;
    const int GetEmployerId() const;
    const int GetClientId() const;
    const int GetRateTypeId() const;
    const int GetCurrencyId() const;

    EmployerModel* GetEmployer();
    ClientModel* GetClient();
    RateTypeModel* GetRateType();
    CurrencyModel* GetCurrency();

    void SetProjectId(const int projectId);
    void SetName(const wxString& name);
    void SetDisplayName(const wxString& displayName);
    void IsBillable(const bool billable);
    void SetRate(std::unique_ptr<double> rate);
    void IsDefault(const bool isDefault);
    void SetDateCreated(const wxDateTime& dateCreated);
    void SetDateUpdated(const wxDateTime& dateUpdated);
    void IsActive(const bool isActive);
    void SetEmployerId(const int employerId);
    void SetClientId(const int clientId);
    void SetRateTypeId(const int rateTypeId);
    void SetCurrencyId(const int currencyId);

    void SetEmployer(std::unique_ptr<EmployerModel> employer);
    void SetClient(std::unique_ptr<ClientModel> client);
    void SetRateType(std::unique_ptr<RateTypeModel> rateType);
    void SetCurrency(std::unique_ptr<CurrencyModel> currency);

private:
    int mProjectId;
    wxString mName;
    wxString mDisplayName;
    bool bIsBillable;
    std::unique_ptr<double> pRate;
    bool bIsDefault;
    wxDateTime mDateCreated;
    wxDateTime mDateModified;
    bool bIsActive;
    int mEmployerId;
    int mClientId;
    int mRateTypeId;
    int mCurrencyId;

    std::unique_ptr<EmployerModel> pEmployer;
    std::unique_ptr<ClientModel> pClient;
    std::unique_ptr<RateTypeModel> pRateType;
    std::unique_ptr<CurrencyModel> pCurrency;
};
} // namespace app::model
