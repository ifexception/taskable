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

#include "projectmodel.h"

#include "../common/util.h"
#include "../services/databaseconnection.h"

namespace app::model
{
ProjectModel::ProjectModel()
    : mProjectId(-1)
    , mName(wxGetEmptyString())
    , mDisplayName(wxGetEmptyString())
    , bIsBillable(false)
    , pRate(nullptr)
    , pHours(nullptr)
    , bIsDefault(nullptr)
    , mDateCreated(wxDefaultDateTime)
    , mDateModified(wxDefaultDateTime)
    , bIsActive(false)
    , mEmployerId(-1)
    , mClientId(-1)
    , mRateTypeId(-1)
    , mCurrencyId(-1)
    , pEmployer(std::make_unique<EmployerModel>())
    , pClient(nullptr)
    , pRateType(std::make_unique<RateTypeModel>())
    , pCurrency(nullptr)
{
}

ProjectModel::ProjectModel(int projectId)
    : ProjectModel()
{
    mProjectId = projectId;
}

ProjectModel::ProjectModel(int projectId, wxString name, wxString displayName)
    : ProjectModel()
{
    mProjectId = projectId;
    mName = name;
    mDisplayName = displayName;
}

ProjectModel::ProjectModel(int projectId,
    wxString name,
    wxString displayName,
    bool billable,
    bool isDefault,
    double* rate,
    int* hours,
    int employerId,
    int* clientId,
    int* rateTypeId,
    int* currencyId)
    : ProjectModel()
{
}

ProjectModel::ProjectModel(int projectId,
    wxString name,
    wxString displayName,
    bool billable,
    bool isDefault,
    int dateCreated,
    int dateModified,
    bool isActive)
    : ProjectModel()
{
    mProjectId = projectId;
    mName = name;
    mDisplayName = displayName;
    bIsBillable = billable;
    bIsDefault = isDefault;
    mDateCreated = util::ToDateTime(dateCreated);
    mDateModified = util::ToDateTime(dateModified);
    bIsActive = isActive;
}

bool ProjectModel::IsNonBillableScenario()
{
    return bIsBillable == false && pRate == nullptr && mRateTypeId == -1 && mCurrencyId == -1;
}

bool ProjectModel::IsBillableWithUnknownRateScenario()
{
    return bIsBillable == true && mRateTypeId == 1 && pRate == nullptr && mCurrencyId == -1;
}

bool ProjectModel::IsBillableScenarioWithHourlyRate()
{
    return bIsBillable == true && mRateTypeId == static_cast<int>(constants::RateTypes::Hourly) && pRate != nullptr &&
           mCurrencyId > 0;
}

bool ProjectModel::IsBillableScenarioWithDailyRate()
{
    return bIsBillable == true && mRateTypeId == static_cast<int>(constants::RateTypes::Daily) && pRate != nullptr &&
           mCurrencyId > 0;
}

bool ProjectModel::HasClientLinked()
{
    return pClient != nullptr || mClientId > 0;
}

void ProjectModel::SwitchOutOfBillableScenario()
{
    pRate.reset();
    mRateTypeId = -1;
    mCurrencyId = -1;
    pHours.reset();
}

void ProjectModel::SwitchInToUnknownRateBillableScenario()
{
    pRate.reset();
    mCurrencyId = -1;
    pHours.reset();
}

void ProjectModel::SwitchInToHourlyRateBillableScenario()
{
    pHours.reset();
}

const int ProjectModel::GetProjectId() const
{
    return mProjectId;
}

const wxString ProjectModel::GetName() const
{
    return mName;
}

const wxString ProjectModel::GetDisplayName() const
{
    return mDisplayName;
}

const bool ProjectModel::IsBillable() const
{
    return bIsBillable;
}

const double* ProjectModel::GetRate() const
{
    return pRate.get();
}

const int* ProjectModel::GetHours() const
{
    return pHours.get();
}

const bool ProjectModel::IsDefault() const
{
    return bIsDefault;
}

const wxDateTime ProjectModel::GetDateCreated()
{
    return mDateCreated;
}

const wxDateTime ProjectModel::GetDateModified()
{
    return mDateModified;
}

const bool ProjectModel::IsActive() const
{
    return bIsActive;
}

const int ProjectModel::GetEmployerId() const
{
    return mEmployerId;
}

const int ProjectModel::GetClientId() const
{
    return mClientId;
}

const int ProjectModel::GetRateTypeId() const
{
    return mRateTypeId;
}

const int ProjectModel::GetCurrencyId() const
{
    return mCurrencyId;
}

EmployerModel* ProjectModel::GetEmployer()
{
    return pEmployer.get();
}

ClientModel* ProjectModel::GetClient()
{
    return pClient.get();
}

RateTypeModel* ProjectModel::GetRateType()
{
    return pRateType.get();
}

CurrencyModel* ProjectModel::GetCurrency()
{
    return pCurrency.get();
}

void ProjectModel::SetProjectId(const int projectId)
{
    mProjectId = projectId;
}

void ProjectModel::SetName(const wxString& name)
{
    mName = name;
}

void ProjectModel::SetDisplayName(const wxString& displayName)
{
    mDisplayName = displayName;
}

void ProjectModel::IsBillable(const bool billable)
{
    bIsBillable = billable;
}

void ProjectModel::SetRate(std::unique_ptr<double> rate)
{
    pRate = std::move(rate);
}

void ProjectModel::SetHours(std::unique_ptr<int> hours)
{
    pHours = std::move(hours);
}

void ProjectModel::IsDefault(const bool isDefault)
{
    bIsDefault = isDefault;
}

void ProjectModel::SetDateCreated(const wxDateTime& dateCreated)
{
    mDateCreated = dateCreated;
}

void ProjectModel::SetDateUpdated(const wxDateTime& dateUpdated)
{
    mDateModified = dateUpdated;
}

void ProjectModel::IsActive(const bool isActive)
{
    bIsActive = isActive;
}

void ProjectModel::SetEmployerId(const int employerId)
{
    mEmployerId = employerId;
}

void ProjectModel::SetClientId(const int clientId)
{
    mClientId = clientId;
}

void ProjectModel::SetRateTypeId(const int rateTypeId)
{
    mRateTypeId = rateTypeId;
}

void ProjectModel::SetCurrencyId(const int currencyId)
{
    mCurrencyId = currencyId;
}

void ProjectModel::SetEmployer(std::unique_ptr<EmployerModel> employer)
{
    pEmployer = std::move(employer);
}

void ProjectModel::SetClient(std::unique_ptr<ClientModel> client)
{
    pClient = std::move(client);
}

void ProjectModel::SetRateType(std::unique_ptr<RateTypeModel> rateType)
{
    pRateType = std::move(rateType);
}

void ProjectModel::SetCurrency(std::unique_ptr<CurrencyModel> currency)
{
    pCurrency = std::move(currency);
}
} // namespace app::model
