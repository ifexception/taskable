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
#include <vector>

#include <wx/datetime.h>
#include <wx/string.h>

#include "../common/constants.h"
#include "employermodel.h"
#include "clientmodel.h"

namespace app::model
{
class RateTypeModel final
{
public:
    RateTypeModel();
    RateTypeModel(const int rateTypeId);
    RateTypeModel(const int rateTypeId, bool initializeFromDatabase);
    RateTypeModel(const int rateTypeId, wxString name);
    ~RateTypeModel() = default;

    const int GetRateTypeId() const;
    const wxString GetName() const;
    const constants::RateTypes GetType() const;

    void SetRateTypeId(const int rateTypeId);
    void SetName(const wxString& name);

    static std::unique_ptr<RateTypeModel> GetById(const int rateTypeId);
    static std::vector<std::unique_ptr<RateTypeModel>> GetAll();

private:
    int mRateTypeId;
    wxString mName;
    constants::RateTypes mRateType;

    static const std::string getRateTypeById;
    static const std::string getRateTypes;
};

class CurrencyModel final
{
public:
    CurrencyModel();
    CurrencyModel(const int currencyId);
    CurrencyModel(const int currencyId, bool initialize);
    CurrencyModel(int currencyId, wxString name, wxString code, wxString symbol);
    ~CurrencyModel() = default;

    const int GetCurrencyId() const;
    const wxString GetName() const;
    const wxString GetCode() const;
    const wxString GetSymbol() const;

    void SetCurrencyId(const int currencyId);
    void SetName(const wxString& name);
    void SetCode(const wxString& code);
    void SetSymbol(const wxString& symbol);

    static std::unique_ptr<CurrencyModel> GetById(const int id);
    static std::vector<std::unique_ptr<CurrencyModel>> GetAll();

private:
    int mCurrencyId;
    wxString mName;
    wxString mCode;
    wxString mSymbol;

    static const std::string getCurrencies;
    static const std::string getCurrencyById;
};

class ProjectModel final
{
public:
    ProjectModel();
    ProjectModel(int projectId);
    ProjectModel(int projectId, bool initializeFromDatabase);
    ProjectModel(int projectId, wxString name, wxString displayName);
    ProjectModel(int projectId,
        wxString name,
        wxString displayName,
        bool billable,
        int dateCreated,
        int dateModified,
        bool isActive);

    bool IsNonBillableScenario();
    bool IsBillableWithUnknownRateScenario();
    bool IsBillableScenarioWithHourlyRate();
    bool IsBillableScenarioWithDailyRate();
    bool HasClientLinked();

    void SwitchOutOfBillableScenario();
    void SwitchInToUnknownRateBillableScenario();
    void SwitchInToHourlyRateBillableScenario();

    const int GetProjectId() const;
    const wxString GetName() const;
    const wxString GetDisplayName() const;
    const bool IsBillable() const;
    const double* GetRate() const;
    const int* GetHours() const;
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
    void SetHours(std::unique_ptr<int> hours);
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

    static void Create(std::unique_ptr<ProjectModel> project);
    static std::unique_ptr<ProjectModel> GetById(const int projectId);
    static void Update(std::unique_ptr<ProjectModel> project);
    static void Delete(std::unique_ptr<ProjectModel> project);
    static std::vector<std::unique_ptr<ProjectModel>> GetAll();

private:
    int mProjectId;
    wxString mName;
    wxString mDisplayName;
    bool bIsBillable;
    std::unique_ptr<double> pRate;
    std::unique_ptr<int> pHours;
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

    static const std::string createProject;
    static const std::string getProject;
    static const std::string updateProject;
    static const std::string deleteProject;
    static const std::string getProjects;
};
} // namespace app::model
