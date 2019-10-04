//  Tasks Tracker is a desktop that aids you in tracking your timesheets
//  and seeing what work you have done.
//
//  Copyright(C)<2019><Szymon Welgus>
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
//
//
//  Contact:
//    szymonwelgus at gmail dot com

#include "setupwizard.h"

#include <sqlite_modern_cpp/errors.h>
#include <wx/wx.h>
#include <wx/file.h>

#include "../services/db_service.h"
#include "../../res/icons8-service-64.xpm"

namespace app::wizard
{
SetupWizard::SetupWizard(wxFrame* frame, std::shared_ptr<spdlog::logger> logger)
    : wxWizard(frame,
          wxID_ANY,
          wxT("Setup Wizard"),
          wxBitmap(tasks_tracker_service),
          wxDefaultPosition,
          wxDEFAULT_DIALOG_STYLE)
    , pLogger(logger)
    , pPage1(nullptr)
    , mEmployer(wxGetEmptyString())
    , mClient(wxGetEmptyString())
    , mProject(wxGetEmptyString())
{
    pPage1 = new wxWizardPageSimple(this);

    wxString introWizardMessage =
        wxT("This wizard will help you get started with Tasks Tracker.\n"
            "The next few pages will setup a employer, a client (which is optional), a project and a category.\n "
            "Please press \"Next\" to begin the process.");

    new wxStaticText(pPage1, wxID_ANY, introWizardMessage);

    auto page2 = new AddEmployerAndClientPage(this);
    auto page3 = new AddProjectPage(this);
    auto page4 = new AddCategoriesPage(this);

    wxWizardPageSimple::Chain(pPage1, page2);
    wxWizardPageSimple::Chain(page2, page3);
    wxWizardPageSimple::Chain(page3, page4);

    GetPageAreaSizer()->Add(pPage1);
}

bool SetupWizard::Run()
{
    auto wizardSuccess = wxWizard::RunWizard(pPage1);
    if (wizardSuccess) {
        wxStopWatch stopWatch;
        stopWatch.Start();
        CreateDatabaseFile();
        bool success = SetUpDatabase();
        if (!success) {
            DeleteDatabaseFile();
            return false;
        }

        success = SetUpEntities();
        if (!success) {
            DeleteDatabaseFile();
            return false;
        }

        stopWatch.Pause();
        wxLogDebug(wxString::Format("DbService calls elapsed time: %ldms", stopWatch.Time()));
    }
    Destroy();
    return wizardSuccess;
}

wxString SetupWizard::GetEmployer() const
{
    return mEmployer;
}

void SetupWizard::SetEmployer(const wxString& employer)
{
    mEmployer = employer;
}

wxString SetupWizard::GetClient() const
{
    return mClient;
}

void SetupWizard::SetClient(const wxString& client)
{
    mClient = client;
}

wxString SetupWizard::GetProject() const
{
    return mProject;
}

void SetupWizard::SetProject(const wxString& project)
{
    mProject = project;
}

void SetupWizard::SetProjectDisplayName(const wxString& displayName)
{
    mDisplayName = displayName;
}

void SetupWizard::SetCategory(const wxString& category)
{
    mCategory = category;
}

void SetupWizard::SetCategoryColor(const wxColour& color)
{
    mCategoryColor = color;
}

void SetupWizard::SetDescription(const wxString& description)
{
    mDescription = description;
}

void SetupWizard::CreateDatabaseFile()
{
    const wxString& databaseFilename = wxT("tasks-tracker.db");
    wxFile file;
    file.Create(databaseFilename);
}

void SetupWizard::DeleteDatabaseFile()
{
    const wxString& databaseFilename = wxT("tasks-tracker.db");
    wxFile file;
    if (wxFileExists(databaseFilename)) {
        wxRemoveFile(databaseFilename);
    }
}

bool SetupWizard::SetUpDatabase()
{
    services::db_service dbService;

    try {
        dbService.create_employers_table();
        dbService.create_clients_table();
        dbService.create_projects_table();
        dbService.create_categories_table();
        dbService.create_tasks_table();
        dbService.create_task_item_types_table();
        dbService.create_task_items_table();

        dbService.seed_task_item_types_table();
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured on create_X_table() - {0:d} : {1}", e.get_code(), e.what());
        return false;
    }

    return true;
}

bool SetupWizard::SetUpEntities()
{
    services::db_service dbService;
    int employerId = 0;
    try {
        dbService.create_new_employer(std::string(mEmployer.ToUTF8()));
        employerId = dbService.get_last_insert_rowid();
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured on create_new_employer() - {0:d} : {1}", e.get_code(), e.what());
        return false;
    }

    int clientId = 0;
    if (!mClient.empty()) {
        try {
            dbService.create_new_client(std::string(mClient.ToUTF8()), employerId);
            clientId = dbService.get_last_insert_rowid();
        } catch (const sqlite::sqlite_exception& e) {
            pLogger->error("Error occured on create_new_client() - {0:d} : {1}", e.get_code(), e.what());
            return false;
        }
    }

    int projectId = 0;
    try {
        bool isAssociatedWithClient = clientId != 0;
        if (isAssociatedWithClient) {
            dbService.create_new_project(
                std::string(mProject.ToUTF8()), std::string(mDisplayName.ToUTF8()), employerId, &clientId);
        } else {
            dbService.create_new_project(
                std::string(mProject.ToUTF8()), std::string(mDisplayName.ToUTF8()), employerId, nullptr);
        }
        projectId = dbService.get_last_insert_rowid();
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured on create_new_project() - {0:d} : {1}", e.get_code(), e.what());
        return false;
    }

    try {
        dbService.create_new_category(
            projectId, std::string(mCategory.ToUTF8()), mCategoryColor.GetRGB(), std::string(mDescription.ToUTF8()));
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured on create_new_category() - {0:d} : {1}", e.get_code(), e.what());
        return false;
    }
    return true;
}

// clang-format off
wxBEGIN_EVENT_TABLE(AddEmployerAndClientPage, wxWizardPageSimple)
    EVT_WIZARD_CANCEL(wxID_ANY, AddEmployerAndClientPage::OnWizardCancel)
wxEND_EVENT_TABLE()

AddEmployerAndClientPage::AddEmployerAndClientPage(SetupWizard* parent)
    : wxWizardPageSimple(parent)
    , pParent(parent)
// clang-format on
{
    auto sizer = new wxBoxSizer(wxVERTICAL);

    auto employerText = new wxStaticText(this, wxID_ANY, wxT("Employer:"));
    sizer->Add(employerText, 0, wxALL, 5);

    pEmployerCtrl = new wxTextCtrl(this, wxID_ANY, wxGetEmptyString(), wxDefaultPosition, wxSize(150, -1), 0);
    sizer->Add(pEmployerCtrl, 0, wxALL, 5);

    wxString employerHelpMessage =
        wxT("Specify a descriptive employer name.\n"
            "An employer is whoever you work for and under who all data will be grouped under");
    auto employerHelpText = new wxStaticText(this, wxID_ANY, employerHelpMessage);
    sizer->Add(employerHelpText, 0, wxALL, 5);

    auto clientText = new wxStaticText(this, wxID_ANY, wxT("Client:*"));
    sizer->Add(clientText, 0, wxALL, 5);

    pClientCtrl = new wxTextCtrl(this, wxID_ANY, wxGetEmptyString(), wxDefaultPosition, wxSize(150, -1), 0);
    sizer->Add(pClientCtrl, 0, wxALL, 5);

    wxString clientHelpMessage =
        wxT("Specify a descriptive name for a client.\n"
            "If your employer has multiple clients and you work with them then you can add a client\n"
            "A client is, however, optional and can be safely skipped if you do not deal with clients");
    auto clientHelpText = new wxStaticText(this, wxID_ANY, clientHelpMessage);
    sizer->Add(clientHelpText, 0, wxALL, 5);

    SetSizer(sizer);
    sizer->Fit(this);
}

bool AddEmployerAndClientPage::TransferDataFromWindow()
{
    const wxString employer = pEmployerCtrl->GetValue().Trim();
    if (employer.empty()) {
        wxMessageBox(wxT("An employer is required"), wxT("TasksTracker"), wxOK | wxICON_ERROR, this);
        return false;
    }

    const wxString client = pClientCtrl->GetValue().Trim();

    pParent->SetEmployer(employer);
    pParent->SetClient(client);

    return true;
}

void AddEmployerAndClientPage::OnWizardCancel(wxWizardEvent& event)
{
    auto userResponse = wxMessageBox(wxT("Are you sure want to cancel the setup and exit the wizard?"),
        wxT("Tasks Tracker Wizard"),
        wxICON_QUESTION | wxYES_NO);
    if (userResponse == wxNO) {
        event.Veto();
    }
}

// clang-format off
wxBEGIN_EVENT_TABLE(AddProjectPage, wxWizardPageSimple)
    EVT_WIZARD_CANCEL(wxID_ANY, AddProjectPage::OnWizardCancel)
wxEND_EVENT_TABLE()

AddProjectPage::AddProjectPage(SetupWizard* parent)
    : wxWizardPageSimple(parent)
    , pParent(parent)
// clang-format on
{
    auto sizer = new wxBoxSizer(wxVERTICAL);

    wxString infoMessage = wxGetEmptyString();
    if (!pParent->GetClient().empty()) {
        infoMessage = wxString::Format(
            "Add a project for employer: %s and client: %s", pParent->GetEmployer(), pParent->GetClient());
    } else {
        infoMessage = wxString::Format("Add a project for employer: %s", pParent->GetEmployer());
    }

    auto infoText = new wxStaticText(this, wxID_ANY, infoMessage);
    sizer->Add(infoText, 0, wxALL, 5);

    auto projectText = new wxStaticText(this, wxID_ANY, wxT("Project:"));
    sizer->Add(projectText, 0, wxALL, 5);

    pNameCtrl = new wxTextCtrl(this, wxID_ANY, wxGetEmptyString(), wxDefaultPosition, wxSize(150, -1), 0);
    sizer->Add(pNameCtrl, 0, wxALL, 5);

    wxString projectNameHelpMessage = wxT("Specify a descriptive project name.\n"
                                          "A project is a undertaking of a business for a client or for itself carried "
                                          "out individually or in a group to achieve a business goal");
    auto projectNameHelpText = new wxStaticText(this, wxID_ANY, projectNameHelpMessage);
    sizer->Add(projectNameHelpText, 0, wxALL, 5);

    auto displayNameText = new wxStaticText(this, wxID_ANY, wxT("Display Name:"));
    sizer->Add(displayNameText, 0, wxALL, 5);

    pDisplayNameCtrl = new wxTextCtrl(this, wxID_ANY, wxGetEmptyString(), wxDefaultPosition, wxSize(150, -1), 0);
    sizer->Add(pDisplayNameCtrl, 0, wxALL, 5);

    wxString displayNameHelpMessage = wxT("Specify a shortened version of the project name.\n"
                                          "Similar to a project name, a display name is merely a shortened version of "
                                          "the project name to aid in readability, identification and display");
    auto displayNameHelpText = new wxStaticText(this, wxID_ANY, displayNameHelpMessage);
    sizer->Add(displayNameHelpText, 0, wxALL, 5);

    SetSizer(sizer);
    sizer->Fit(this);
}

bool AddProjectPage::TransferDataFromWindow()
{
    const wxString projectName = pNameCtrl->GetValue().Trim();
    if (projectName.empty()) {
        wxMessageBox(wxT("An project name is required"), wxT("TasksTracker"), wxOK | wxICON_ERROR, this);
        return false;
    }

    const wxString displayName = pDisplayNameCtrl->GetValue().Trim();
    if (displayName.empty()) {
        wxMessageBox(wxT("An display name is required"), wxT("TasksTracker"), wxOK | wxICON_ERROR, this);
        return false;
    }

    pParent->SetProject(projectName);
    pParent->SetProjectDisplayName(displayName);

    return true;
}

void AddProjectPage::OnWizardCancel(wxWizardEvent& event)
{
    auto userResponse = wxMessageBox(wxT("Are you sure want to cancel the setup and exit the wizard?"),
        wxT("Tasks Tracker Wizard"),
        wxICON_QUESTION | wxYES_NO);
    if (userResponse == wxNO) {
        event.Veto();
    }
}

// clang-format off
wxBEGIN_EVENT_TABLE(AddCategoriesPage, wxWizardPageSimple)
EVT_WIZARD_CANCEL(wxID_ANY, AddCategoriesPage::OnWizardCancel)
wxEND_EVENT_TABLE()

AddCategoriesPage::AddCategoriesPage(SetupWizard* parent)
    : wxWizardPageSimple(parent)
    , pParent(parent)
// clang-format on
{
    auto sizer = new wxBoxSizer(wxVERTICAL);

    auto infoMessage = wxString::Format("Add a category to the project: %s", pParent->GetProject());
    auto infoText = new wxStaticText(this, wxID_ANY, infoMessage);
    sizer->Add(infoText, 0, wxALL, 5);

    auto categoryText = new wxStaticText(this, wxID_ANY, wxT("Category:"));
    sizer->Add(categoryText, 0, wxALL, 5);

    pNameCtrl = new wxTextCtrl(this, wxID_ANY, wxGetEmptyString(), wxDefaultPosition, wxSize(150, -1), 0);
    sizer->Add(pNameCtrl, 0, wxALL, 5);

    wxString categoryNameHelpMessage =
        wxT("Specify a category for the project.\n"
            "A category is the specific type of task you worked on or did for said project, e.g. \"meetings\"");
    auto categoryNameHelpText = new wxStaticText(this, wxID_ANY, categoryNameHelpMessage);
    sizer->Add(categoryNameHelpText, 0, wxALL, 5);

    auto colorText = new wxStaticText(this, wxID_ANY, wxT("Color"));
    sizer->Add(colorText, 0, wxALL, 5);

    pColorPickerCtrl = new wxColourPickerCtrl(this, wxID_ANY);
    sizer->Add(pColorPickerCtrl, 0, wxALL, 5);

    wxString categoryColorHelpMessage =
        wxT("Select a color for this category.\n"
            "Specifying a color helps you with easily identifying tasks on the main list view");
    auto categoryColorHelpText = new wxStaticText(this, wxID_ANY, categoryColorHelpMessage);
    sizer->Add(categoryColorHelpText, 0, wxALL, 5);

    auto descriptionText = new wxStaticText(this, wxID_ANY, wxT("Description:"));
    sizer->Add(descriptionText, 0, wxALL, 5);

    pDescriptionCtrl = new wxTextCtrl(this, wxID_ANY, wxGetEmptyString(), wxDefaultPosition, wxSize(150, -1), 0);
    sizer->Add(pDescriptionCtrl, 0, wxALL, 5);

    wxString descriptionHelpMessage = wxT("Specify a description for the above category.\n"
                                          "A description for the category helps you create a distinction between "
                                          "similar categories for different projects");
    auto descriptionHelpText = new wxStaticText(this, wxID_ANY, descriptionHelpMessage);
    sizer->Add(descriptionHelpText, 0, wxALL, 5);

    SetSizer(sizer);
    sizer->Fit(this);
}

bool AddCategoriesPage::TransferDataFromWindow()
{
    const wxString category = pNameCtrl->GetValue().Trim();
    if (category.empty()) {
        wxMessageBox(wxT("An category name is required"), wxT("TasksTracker"), wxOK | wxICON_ERROR, this);
        return false;
    }

    const wxColour color = pColorPickerCtrl->GetColour();

    const wxString description = pDescriptionCtrl->GetValue().Trim();

    pParent->SetCategory(category);
    pParent->SetCategoryColor(color);
    pParent->SetDescription(description);

    return true;
}

void AddCategoriesPage::OnWizardCancel(wxWizardEvent& event)
{
    auto userResponse = wxMessageBox(wxT("Are you sure want to cancel the setup and exit the wizard?"),
        wxT("Tasks Tracker Wizard"),
        wxICON_QUESTION | wxYES_NO);
    if (userResponse == wxNO) {
        event.Veto();
    }
}
} // namespace app::wizard
