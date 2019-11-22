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

#include "projectmodel.h"

#include "../services/db_connection.h"

namespace app::model
{
RateTypeModel::RateTypeModel()
    : mRateTypeId(-1)
    , mName(wxGetEmptyString())
{
}

RateTypeModel::RateTypeModel(const int rateTypeId)
    : RateTypeModel()
{
    mRateTypeId = rateTypeId;
}

RateTypeModel::RateTypeModel(const int rateTypeId, bool initializeFromDatabase)
    : RateTypeModel()
{
    assert(initializeFromDatabase == true);
    auto rateType = RateTypeModel::GetById(rateTypeId);
    mRateTypeId = rateType->GetRateTypeId();
    mName = rateType->GetName();
}

RateTypeModel::RateTypeModel(const int rateTypeId, wxString name)
    : RateTypeModel()
{
    mRateTypeId = rateTypeId;
    mName = name;
}

const int RateTypeModel::GetRateTypeId() const
{
    return mRateTypeId;
}

const wxString RateTypeModel::GetName() const
{
    return mName;
}

void RateTypeModel::SetRateTypeId(const int rateTypeId)
{
    mRateTypeId = rateTypeId;
}

void RateTypeModel::SetName(const wxString& name)
{
    mName = name;
}

std::unique_ptr<RateTypeModel> RateTypeModel::GetById(const int rateTypeId)
{
    std::unique_ptr<RateTypeModel> rateType = nullptr;

    auto db = services::db_connection::get_instance().get_handle();
    db << RateTypeModel::getRateTypeById << rateTypeId >> [&](int rateTypeId, std::string name) {
        rateType = std::make_unique<RateTypeModel>(rateTypeId, wxString(name));
    };

    return std::move(rateType);
}

std::vector<std::unique_ptr<RateTypeModel>> RateTypeModel::GetAll()
{
    std::vector<std::unique_ptr<RateTypeModel>> rateTypes;

    auto db = services::db_connection::get_instance().get_handle();
    db << RateTypeModel::getRateTypes >> [&](int rateTypeId, std::string name) {
        auto rateType = std::make_unique<RateTypeModel>(rateTypeId, wxString(name));
        rateTypes.push_back(std::move(rateType));
    };

    return rateTypes;
}

const std::string RateTypeModel::getRateTypeById = "SELECT rate_type_id, "
                                                   "name "
                                                   "FROM rate_types"
                                                   "WHERE rate_type_id = ?";

const std::string RateTypeModel::getRateTypes = "SELECT rate_type_id, "
                                                "name "
                                                "FROM rate_types";

CurrencyModel::CurrencyModel()
    : mName(wxGetEmptyString())
    , mCode(wxGetEmptyString())
    , mSymbol(wxGetEmptyString())
{
}

CurrencyModel::CurrencyModel(const int currencyId)
    : CurrencyModel()
{
    mCurrencyId = currencyId;
}

CurrencyModel::CurrencyModel(const int currencyId, bool initialize)
    : CurrencyModel()
{
    assert(initialize == true);
    auto currency = CurrencyModel::GetById(currencyId);
    mCurrencyId = currency->GetCurrencyId();
    mName = currency->GetName();
    mCode = currency->GetCode();
    mSymbol = currency->GetSymbol();
}

CurrencyModel::CurrencyModel(int currencyId, wxString name, wxString code, wxString symbol)
    : CurrencyModel()
{
    mCurrencyId = currencyId;
    mName = name;
    mCode = code;
    mSymbol = symbol;
}

const int CurrencyModel::GetCurrencyId() const
{
    return mCurrencyId;
}

const wxString CurrencyModel::GetName() const
{
    return mName;
}

const wxString CurrencyModel::GetCode() const
{
    return mCode;
}

const wxString CurrencyModel::GetSymbol() const
{
    return mSymbol;
}

void CurrencyModel::SetCurrencyId(const int currencyId)
{
    mCurrencyId = currencyId;
}

void CurrencyModel::SetName(const wxString& name)
{
    mName = name;
}

void CurrencyModel::SetCode(const wxString& code)
{
    mCode = code;
}

void CurrencyModel::SetSymbol(const wxString& symbol)
{
    mSymbol = symbol;
}

std::unique_ptr<CurrencyModel> CurrencyModel::GetById(const int id)
{
    std::unique_ptr<CurrencyModel> currency = nullptr;
    auto db = services::db_connection::get_instance().get_handle();
    db << CurrencyModel::getCurrencyById << id >>
        [&](int currencyId, std::string name, std::string code, std::string symbol) {
            currency = std::make_unique<CurrencyModel>(currencyId, wxString(name), wxString(code), wxString(symbol));
        };

    return std::move(currency);
}

std::vector<std::unique_ptr<CurrencyModel>> CurrencyModel::GetAll()
{
    std::vector<std::unique_ptr<CurrencyModel>> currencies;
    auto db = services::db_connection::get_instance().get_handle();
    db << CurrencyModel::getCurrencies >> [&](int currencyId, std::string name, std::string code, std::string symbol) {
        auto currency = std::make_unique<CurrencyModel>(currencyId, wxString(name), wxString(code), wxString(symbol));
        currencies.push_back(std::move(currency));
    };

    return currencies;
}

const std::string CurrencyModel::getCurrencyById = "SELECT currency_id, "
                                                   "name, "
                                                   "code, "
                                                   "symbol "
                                                   "FROM currencies "
                                                   "WHERE currency_id = ?";

const std::string CurrencyModel::getCurrencies = "SELECT currency_id, "
                                                 "name, "
                                                 "code, "
                                                 "symbol "
                                                 "FROM currencies";

ProjectModel::ProjectModel()
    : mProjectId(-1)
    , mName(wxGetEmptyString())
    , mDisplayName(wxGetEmptyString())
    , mDateCreated(wxDefaultDateTime)
    , mDateUpdated(wxDefaultDateTime)
    , bIsActive(false)
    , pEmployer()
    , pClient(nullptr)
    , pRateType(nullptr)
    , pCurrency(nullptr)
{
}

ProjectModel::ProjectModel(int projectId, wxString name, wxString displayName)
    : ProjectModel()
{
    mProjectId = projectId;
    mName = name;
    mDisplayName = displayName;
}

void ProjectModel::Reset()
{
    mProjectId = 0;
    mName = wxGetEmptyString();
    mDisplayName = wxGetEmptyString();
    mDateCreated = wxDefaultDateTime;
    mDateUpdated = wxDefaultDateTime;
    bIsActive = false;
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

const wxDateTime ProjectModel::GetDateCreated()
{
    return mDateCreated;
}

const wxDateTime ProjectModel::GetDateUpdated()
{
    return mDateUpdated;
}

const bool ProjectModel::IsActive() const
{
    return bIsActive;
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

void ProjectModel::SetDateCreated(const wxDateTime& dateCreated)
{
    mDateCreated = dateCreated;
}

void ProjectModel::SetDateUpdated(const wxDateTime& dateUpdated)
{
    mDateUpdated = dateUpdated;
}

void ProjectModel::IsActive(const bool isActive)
{
    bIsActive = isActive;
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

void ProjectModel::Create(std::unique_ptr<ProjectModel> project)
{
    auto db = services::db_connection::get_instance().get_handle();
}

void ProjectModel::Update(std::unique_ptr<ProjectModel> project) {}

void ProjectModel::Delete(std::unique_ptr<ProjectModel> project) {}

std::vector<model::ProjectModel> ProjectModel::GetAll()
{
    std::vector<model::ProjectModel> projects;

    auto db = services::db_connection::get_instance().get_handle();
    db << ProjectModel::getProjects >> [&](int projectId, std::string name, std::string displayName) {
        model::ProjectModel project(projectId, wxString(name), wxString(displayName));
        projects.push_back(project);
    };

    return projects;
}

const std::string ProjectModel::createProject;

const std::string ProjectModel::getProject;

const std::string ProjectModel::updateProject;

const std::string ProjectModel::deleteProject;

const std::string ProjectModel::getProjects = "SELECT projects.project_id, "
                                              "projects.name, "
                                              "projects.display_name "
                                              "FROM projects "
                                              "WHERE projects.is_active = 1";
} // namespace app::model
