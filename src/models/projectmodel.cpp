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

#include "../common/util.h"
#include "../services/db_connection.h"

namespace app::model
{
RateTypeModel::RateTypeModel()
    : mRateTypeId(-1)
    , mName(wxGetEmptyString())
    , mRateType()
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
    mRateType = static_cast<RateTypes>(mRateTypeId);
}

RateTypeModel::RateTypeModel(const int rateTypeId, wxString name)
    : RateTypeModel()
{
    mRateTypeId = rateTypeId;
    mName = name;
    mRateType = static_cast<RateTypes>(mRateTypeId);
}

const int RateTypeModel::GetRateTypeId() const
{
    return mRateTypeId;
}

const wxString RateTypeModel::GetName() const
{
    return mName;
}

const RateTypes RateTypeModel::GetType() const
{
    return mRateType;
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
                                                   "FROM rate_types "
                                                   "WHERE rate_type_id = ?";

const std::string RateTypeModel::getRateTypes = "SELECT rate_type_id, "
                                                "name "
                                                "FROM rate_types";

CurrencyModel::CurrencyModel()
    : mCurrencyId(-1)
    , mName(wxGetEmptyString())
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
    , bIsBillable(false)
    , pRate(nullptr)
    , mDateCreated(wxDefaultDateTime)
    , mDateModified(wxDefaultDateTime)
    , bIsActive(false)
    , mEmployerId(-1)
    , mClientId(-1)
    , mRateTypeId(-1)
    , mCurrencyId(-1)
    , pEmployer(std::make_unique<EmployerModel>())
    , pClient(nullptr)
    , pRateType(nullptr)
    , pCurrency(nullptr)
{
}

ProjectModel::ProjectModel(int projectId)
    : ProjectModel()
{
    mProjectId = projectId;
}

ProjectModel::ProjectModel(int projectId, bool initializeFromDatabase)
    : ProjectModel()
{
    assert(initializeFromDatabase == true);
    auto project = ProjectModel::GetById(projectId);
    mProjectId = project->GetProjectId();
    mName = project->GetName();
    mDisplayName = project->GetDisplayName();
    mDateCreated = project->GetDateCreated();
    mDateModified = project->GetDateModified();
    bIsActive = project->IsActive();
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
    int dateCreated,
    int dateModified,
    bool isActive)
    : ProjectModel()
{
    mProjectId = projectId;
    mName = name;
    mDisplayName = displayName;
    bIsBillable = billable;
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
    return bIsBillable == true && mRateTypeId == static_cast<int>(RateTypes::Hourly) && pRate != nullptr &&
           mCurrencyId > 0;
}

bool ProjectModel::IsBillableScenarioWithDailyRate()
{
    return bIsBillable == true && mRateTypeId == static_cast<int>(RateTypes::Daily) && pRate != nullptr &&
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

void ProjectModel::Create(std::unique_ptr<ProjectModel> project)
{
    auto db = services::db_connection::get_instance().get_handle();
    auto ps = db << ProjectModel::createProject;
    ps << project->GetName() << project->GetDisplayName() << project->IsBillable() << project->GetEmployerId();

    if (project->HasClientLinked())
        ps << project->GetClientId();
    else
        ps << nullptr;

    if (project->IsNonBillableScenario())
        ps << nullptr << nullptr << nullptr << nullptr;

    if (project->IsBillableWithUnknownRateScenario())
        ps << nullptr << project->GetRateTypeId() << nullptr << nullptr;

    if (project->IsBillableScenarioWithHourlyRate())
        ps << *project->GetRate() << project->GetRateTypeId() << project->GetCurrencyId() << nullptr;

    if (project->IsBillableScenarioWithDailyRate())
        ps << *project->GetRate() << project->GetRateTypeId() << project->GetCurrencyId() << *project->GetHours();

    ps.execute();
}

std::unique_ptr<ProjectModel> ProjectModel::GetById(const int projectId)
{
    std::unique_ptr<ProjectModel> project = nullptr;
    auto db = services::db_connection::get_instance().get_handle();
    db << ProjectModel::getProject << projectId >> [&](int projectId,
                                                       std::string name,
                                                       std::string displayName,
                                                       int billable,
                                                       std::unique_ptr<double> rate,
                                                       std::unique_ptr<int> hours,
                                                       int dateCreated,
                                                       int dateModified,
                                                       int isActive,
                                                       int employerId,
                                                       std::unique_ptr<int> clientId,
                                                       std::unique_ptr<int> rateTypeId,
                                                       std::unique_ptr<int> currencyId) {
        project = std::make_unique<ProjectModel>(
            projectId, wxString(name), wxString(displayName), billable, dateCreated, dateModified, isActive);

        if (rate != nullptr) {
            project->SetRate(std::move(rate));
        }

        if (hours != nullptr) {
            project->SetHours(std::move(hours));
        }

        project->SetEmployerId(employerId);
        auto employer = std::make_unique<EmployerModel>(employerId, true);
        project->SetEmployer(std::move(employer));
        if (clientId != nullptr) {
            project->SetClientId(*clientId);
            auto client = std::make_unique<ClientModel>(*clientId, true);
            project->SetClient(std::move(client));
        }

        if (rateTypeId != nullptr) {
            project->SetRateTypeId(*rateTypeId);
            auto rateType = std::make_unique<RateTypeModel>(*rateTypeId, true);
            project->SetRateType(std::move(rateType));
        }

        if (currencyId != nullptr) {
            project->SetCurrencyId(*currencyId);
            auto currency = std::make_unique<CurrencyModel>(*currencyId, true);
            project->SetCurrency(std::move(currency));
        }
    };

    return project;
}

void ProjectModel::Update(std::unique_ptr<ProjectModel> project)
{
    auto db = services::db_connection::get_instance().get_handle();

    auto ps = db << ProjectModel::updateProject << project->GetName() << project->GetDisplayName()
                 << project->IsBillable() << util::UnixTimestamp() << project->GetEmployerId();

    if (project->HasClientLinked())
        ps << project->GetClientId();
    else
        ps << nullptr;

    if (project->IsNonBillableScenario())
        ps << nullptr << nullptr << nullptr << nullptr;

    if (project->IsBillableWithUnknownRateScenario())
        ps << nullptr << project->GetRateTypeId() << nullptr << nullptr;

    if (project->IsBillableScenarioWithHourlyRate())
        ps << *project->GetRate() << project->GetRateTypeId() << project->GetCurrencyId() << nullptr;

    if (project->IsBillableScenarioWithDailyRate())
        ps << *project->GetRate() << project->GetRateTypeId() << project->GetCurrencyId() << *project->GetHours();

    ps << project->GetProjectId();

    ps.execute();
}

void ProjectModel::Delete(std::unique_ptr<ProjectModel> project)
{
    auto db = services::db_connection::get_instance().get_handle();
    db << ProjectModel::deleteProject << util::UnixTimestamp() << project->GetProjectId();
}

std::vector<std::unique_ptr<ProjectModel>> ProjectModel::GetAll()
{
    std::vector<std::unique_ptr<ProjectModel>> projects;
    auto db = services::db_connection::get_instance().get_handle();
    db << ProjectModel::getProjects >> [&](int projectId,
                                           std::string name,
                                           std::string displayName,
                                           int billable,
                                           std::unique_ptr<double> rate,
                                           std::unique_ptr<int> hours,
                                           int dateCreated,
                                           int dateModified,
                                           int isActive,
                                           int employerId,
                                           std::unique_ptr<int> clientId,
                                           std::unique_ptr<int> rateTypeId,
                                           std::unique_ptr<int> currencyId) {
        auto project = std::make_unique<ProjectModel>(
            projectId, wxString(name), wxString(displayName), billable, dateCreated, dateModified, isActive);

        if (rate != nullptr) {
            project->SetRate(std::move(rate));
        }

        if (hours != nullptr) {
            project->SetHours(std::move(hours));
        }

        project->SetEmployerId(employerId);
        auto employer = std::make_unique<EmployerModel>(employerId, true);
        project->SetEmployer(std::move(employer));
        if (clientId != nullptr) {
            project->SetClientId(*clientId);
            auto client = std::make_unique<ClientModel>(*clientId, true);
            project->SetClient(std::move(client));
        }

        if (rateTypeId != nullptr) {
            project->SetRateTypeId(*rateTypeId);
            auto rateType = std::make_unique<RateTypeModel>(*rateTypeId, true);
            project->SetRateType(std::move(rateType));
        }

        if (currencyId != nullptr) {
            project->SetCurrencyId(*currencyId);
            auto currency = std::make_unique<CurrencyModel>(*currencyId, true);
            project->SetCurrency(std::move(currency));
        }

        projects.push_back(std::move(project));
    };

    return projects;
}

const std::string ProjectModel::createProject = "INSERT INTO "
                                                "projects(name, display_name, billable, is_active, "
                                                "employer_id, client_id, rate, rate_type_id, currency_id, hours) "
                                                "VALUES(?, ?, ?, 1, ?, ?, ?, ?, ?, ?)";

const std::string ProjectModel::getProject = "SELECT projects.project_id, "
                                             "projects.name AS project_name, "
                                             "projects.display_name, "
                                             "projects.billable, "
                                             "projects.rate, "
                                             "projects.hours,"
                                             "projects.date_created, "
                                             "projects.date_modified, "
                                             "projects.is_active, "
                                             "employers.employer_id, "
                                             "clients.client_id AS client_id, "
                                             "rate_types.rate_type_id AS rate_type_id, "
                                             "currencies.currency_id AS currency_id "
                                             "FROM projects "
                                             "INNER JOIN employers ON projects.employer_id = employers.employer_id "
                                             "LEFT JOIN clients ON projects.client_id = clients.client_id "
                                             "LEFT JOIN rate_types ON projects.rate_type_id = rate_types.rate_type_id "
                                             "LEFT JOIN currencies ON projects.currency_id = currencies.currency_id "
                                             "WHERE projects.project_id = ?";

const std::string ProjectModel::updateProject =
    "UPDATE projects "
    "SET name = ?, display_name = ?, billable = ?, date_modified = ?, "
    "employer_id = ?, client_id = ?, rate = ?, rate_type_id = ?, currency_id = ?, "
    "hours = ? "
    "WHERE project_id = ?";

const std::string ProjectModel::deleteProject = "UPDATE projects"
                                                "SET is_active = 0, date_modified = ?"
                                                "WHERE project_id = ?";

const std::string ProjectModel::getProjects = "SELECT projects.project_id, "
                                              "projects.name AS project_name, "
                                              "projects.display_name, "
                                              "projects.billable, "
                                              "projects.rate, "
                                              "projects.hours,"
                                              "projects.date_created, "
                                              "projects.date_modified, "
                                              "projects.is_active, "
                                              "employers.employer_id, "
                                              "clients.client_id AS client_id, "
                                              "rate_types.rate_type_id AS rate_type_id, "
                                              "currencies.currency_id AS currency_id "
                                              "FROM projects "
                                              "INNER JOIN employers ON projects.employer_id = employers.employer_id "
                                              "LEFT JOIN clients ON projects.client_id = clients.client_id "
                                              "LEFT JOIN rate_types ON projects.rate_type_id = rate_types.rate_type_id "
                                              "LEFT JOIN currencies ON projects.currency_id = currencies.currency_id";
} // namespace app::model
