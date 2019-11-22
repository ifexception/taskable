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

#include "employermodel.h"
#include "clientmodel.h"

namespace app::model
{
enum class RateTypes : int { Minute = 1, Hour = 2, Day = 3 };

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

    void SetRateTypeId(const int rateTypeId);
    void SetName(const wxString& name);

    static std::unique_ptr<RateTypeModel> GetById(const int rateTypeId);
    static std::vector<std::unique_ptr<RateTypeModel>> GetAll();

private:
    int mRateTypeId;
    wxString mName;

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
    ProjectModel(int projectId, wxString name, wxString displayName);

    void Reset();

    const int GetProjectId() const;
    const wxString GetName() const;
    const wxString GetDisplayName() const;
    const bool IsBillable() const;
    const wxDateTime GetDateCreated();
    const wxDateTime GetDateUpdated();
    const bool IsActive() const;

    EmployerModel* GetEmployer();
    ClientModel* GetClient();
    RateTypeModel* GetRateType();
    CurrencyModel* GetCurrency();

    void SetProjectId(const int projectId);
    void SetName(const wxString& name);
    void SetDisplayName(const wxString& displayName);
    void IsBillable(const bool billable);
    void SetDateCreated(const wxDateTime& dateCreated);
    void SetDateUpdated(const wxDateTime& dateUpdated);
    void IsActive(const bool isActive);

    void SetEmployer(std::unique_ptr<EmployerModel> employer);
    void SetClient(std::unique_ptr<ClientModel> client);
    void SetRateType(std::unique_ptr<RateTypeModel> rateType);
    void SetCurrency(std::unique_ptr<CurrencyModel> currency);

    static void Create(std::unique_ptr<ProjectModel> project);
    static std::unique_ptr<ProjectModel> GetById(const int projectId);
    static void Update(std::unique_ptr<ProjectModel> project);
    static void Delete(std::unique_ptr<ProjectModel> project);
    static std::vector<model::ProjectModel> GetAll();

private:
    int mProjectId;
    wxString mName;
    wxString mDisplayName;
    bool bIsBillable;
    wxDateTime mDateCreated;
    wxDateTime mDateUpdated;
    bool bIsActive;

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
