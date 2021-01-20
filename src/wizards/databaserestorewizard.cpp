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

#include "databaserestorewizard.h"

#include <string>

#include <wx/file.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/regex.h>
#include <wx/stdpaths.h>

#include "../config/configurationprovider.h"
#include "../database/sqliteconnectionfactory.h"
#include "../database/sqliteconnection.h"
#include "../database/connectionprovider.h"

namespace app::wizard
{
DatabaseRestoreWizard::DatabaseRestoreWizard(frm::MainFrame* frame,
    std::shared_ptr<spdlog::logger> logger,
    bool restoreWithNoPreviousFileExisting)
    : wxWizard(frame,
          wxID_ANY,
          wxT("Database Restore Wizard"),
          wxBitmap(database_restore_wizard),
          wxDefaultPosition,
          wxDEFAULT_DIALOG_STYLE)
    , pFrame(frame)
    , pLogger(logger)
    , pPage1(nullptr)
    , mDatabaseFileToRestore(wxGetEmptyString())
    , bRestoreWithNoPreviousFileExisting(restoreWithNoPreviousFileExisting)
{
    pPage1 = new DatabaseRestoreWelcomePage(this);
    auto page2 = new SelectDatabaseVersionPage(this);
    auto page3 = new DatabaseRestoredPage(this, pLogger);

    wxWizardPageSimple::Chain(pPage1, page2);
    wxWizardPageSimple::Chain(page2, page3);
}

bool DatabaseRestoreWizard::Run()
{
    auto success = wxWizard::RunWizard(pPage1);
    Destroy();

    return success;
}

const wxString DatabaseRestoreWizard::GetDatabaseFileVersionToRestore() const
{
    return mDatabaseFileToRestore;
}

const bool DatabaseRestoreWizard::IsRestoreWithNoPreviousFileExisting() const
{
    return bRestoreWithNoPreviousFileExisting;
}

void DatabaseRestoreWizard::SetDatabaseVersionToRestore(const wxString& value)
{
    mDatabaseFileToRestore = value;
}

DatabaseRestoreWelcomePage::DatabaseRestoreWelcomePage(DatabaseRestoreWizard* parent)
    : wxWizardPageSimple(parent)
{
    CreateControls();
}

void DatabaseRestoreWelcomePage::CreateControls()
{
    auto mainSizer = new wxBoxSizer(wxVERTICAL);

    /* Welcome message Static Text Control */
    auto welcomeMessage = wxT("Welcome to the Taskable\n"
                              "Database Restore Wizard");
    auto welcomeTextLabel = new wxStaticText(this, wxID_ANY, welcomeMessage);
    auto welcomeTextFont = welcomeTextLabel->GetFont();
    welcomeTextFont.MakeBold();
    welcomeTextFont.SetPointSize(16);
    welcomeTextLabel->SetFont(welcomeTextFont);
    mainSizer->Add(welcomeTextLabel, wxSizerFlags().Border(wxALL, 5));

    /* Wizard introduction Static Text Control */
    wxString introWizardMessage = wxT("This wizard will help you restore\n"
                                      "a previous version of the database.");
    auto introText = new wxStaticText(this, wxID_ANY, introWizardMessage);
    mainSizer->Add(introText, wxSizerFlags().Border(wxALL, 5));

    /* Continue next Static Text Control */
    wxString continueNextMessage = wxT("\n\n\nTo continue, click Next.");
    auto continueNextText = new wxStaticText(this, wxID_ANY, continueNextMessage);
    mainSizer->Add(continueNextText, wxSizerFlags().Border(wxALL, 5));

    SetSizerAndFit(mainSizer);
}

SelectDatabaseVersionPage::SelectDatabaseVersionPage(DatabaseRestoreWizard* parent)
    : wxWizardPageSimple(parent)
    , pParent(parent)
    , pListCtrl(nullptr)
    , mSelectedIndex(-1)
{
    CreateControls();
    ConfigureEventBindings();
    FillControls();
}

bool SelectDatabaseVersionPage::TransferDataFromWindow()
{
    if (mSelectedIndex < 0) {
        wxMessageBox(wxT("A selection is required"), common::GetProgramName(), wxOK_DEFAULT | wxICON_ERROR, this);
        return false;
    }

    wxListItem item;
    item.m_itemId = mSelectedIndex;
    item.m_col = 0;
    item.m_mask = wxLIST_MASK_TEXT;
    pListCtrl->GetItem(item);

    wxString databaseSelection = item.GetText();
    pParent->SetDatabaseVersionToRestore(databaseSelection);

    return true;
}

void SelectDatabaseVersionPage::CreateControls()
{
    auto mainSizer = new wxBoxSizer(wxVERTICAL);

    auto databaseVersionSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(databaseVersionSizer, wxSizerFlags().Border(wxALL, 5).Expand());

    wxString descriptiveText = wxT("Please select a version to restore the database to\n"
                                   "from the below list");
    auto descriptiveTextLabel = new wxStaticText(this, wxID_ANY, descriptiveText);
    databaseVersionSizer->Add(descriptiveTextLabel, wxSizerFlags().Border(wxALL, 5));

    long style = wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_HRULES;
    pListCtrl = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
    pListCtrl->EnableCheckBoxes();
    databaseVersionSizer->Add(pListCtrl, wxSizerFlags().Border(wxALL, 5).Expand());

    pListCtrl->SetSize(250, 200);

    wxListItem databaseColumn;
    databaseColumn.SetId(0);
    databaseColumn.SetText(wxT("Database"));
    databaseColumn.SetWidth(150);
    pListCtrl->InsertColumn(0, databaseColumn);

    wxListItem dateColumn;
    dateColumn.SetAlign(wxLIST_FORMAT_CENTER);
    dateColumn.SetId(1);
    dateColumn.SetText(wxT("Modified"));
    dateColumn.SetWidth(98);
    pListCtrl->InsertColumn(1, dateColumn);

    SetSizerAndFit(mainSizer);
}

// clang-format off
void SelectDatabaseVersionPage::ConfigureEventBindings()
{
    pListCtrl->Bind(
        wxEVT_LIST_ITEM_CHECKED,
        &SelectDatabaseVersionPage::OnItemChecked,
        this
    );

    pListCtrl->Bind(
        wxEVT_LIST_ITEM_UNCHECKED,
        &SelectDatabaseVersionPage::OnItemUnchecked,
        this
    );

    Bind(
        wxEVT_WIZARD_CANCEL,
        &SelectDatabaseVersionPage::OnWizardCancel,
        this
    );
}
// clang-format on

void SelectDatabaseVersionPage::FillControls()
{
    const wxString backupPath = cfg::ConfigurationProvider::Get().Configuration->GetBackupPath();
    wxArrayString files;
    wxDir::GetAllFiles(backupPath, &files, wxEmptyString, wxDIR_FILES);

    int listIndex = 0;
    int columnIndex = 0;
    for (const auto& file : files) {
        wxRegEx dateRegex(wxT("([0-9]{4}-[0-9]{2}-[0-9]{2})"));
        if (dateRegex.IsValid()) {
            if (dateRegex.Matches(file)) {
                wxFileName filename(file);
                auto dateComponent = dateRegex.GetMatch(file, 0);
                listIndex = pListCtrl->InsertItem(columnIndex++, filename.GetFullName());
                pListCtrl->SetItem(listIndex, columnIndex++, dateComponent);
            }
        }
        columnIndex = 0;
    }
}

void SelectDatabaseVersionPage::OnItemChecked(wxListEvent& event)
{
    mSelectedIndex = event.GetIndex();
}

void SelectDatabaseVersionPage::OnItemUnchecked(wxListEvent& event)
{
    mSelectedIndex = -1;
}

void SelectDatabaseVersionPage::OnWizardCancel(wxWizardEvent& event)
{
    auto userResponse = wxMessageBox(
        wxT("Are you sure want to cancel the wizard and exit?"), common::GetProgramName(), wxICON_QUESTION | wxYES_NO);
    if (userResponse == wxNO) {
        event.Veto();
    }
}

DatabaseRestoredPage::DatabaseRestoredPage(DatabaseRestoreWizard* parent, std::shared_ptr<spdlog::logger> logger)
    : wxWizardPageSimple(parent)
    , pParent(parent)
    , pLogger(logger)
    , pStatusInOperationLabel(nullptr)
    , pGaugeCtrl(nullptr)
    , pStatusCompleteLabel(nullptr)
{
    CreateControls();
    ConfigureEventBindings();
}

void DatabaseRestoredPage::CreateControls()
{
    auto mainSizer = new wxBoxSizer(wxVERTICAL);

    auto sizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(sizer, wxSizerFlags().Border(wxALL, 5).Expand());

    wxString databaseRestoreText = wxT("Performing restoration... Please wait.");
    pStatusInOperationLabel = new wxStaticText(this, wxID_ANY, databaseRestoreText);
    sizer->Add(pStatusInOperationLabel, wxSizerFlags().Border(wxALL, 5).Left());

    pGaugeCtrl = new wxGauge(this, wxID_ANY, 100);
    sizer->Add(pGaugeCtrl, wxSizerFlags().Border(wxALL, 5).CenterHorizontal());

    sizer->AddSpacer(8);

    pStatusCompleteLabel = new wxStaticText(this, wxID_ANY, wxGetEmptyString());
    sizer->Add(pStatusCompleteLabel, wxSizerFlags().Border(wxALL, 5).Left());

    SetSizerAndFit(mainSizer);
}

// clang-format off
void DatabaseRestoredPage::ConfigureEventBindings()
{
    Bind(
        wxEVT_WIZARD_PAGE_SHOWN,
        &DatabaseRestoredPage::OnWizardPageShown,
        this
    );

    Bind(
        wxEVT_WIZARD_CANCEL,
        &DatabaseRestoredPage::OnWizardCancel,
        this
    );
}
// clang-format on

void DatabaseRestoredPage::OnWizardPageShown(wxWizardEvent& event)
{
    pGaugeCtrl->Pulse();

    const wxString fileToRestore = pParent->GetDatabaseFileVersionToRestore();

    const wxString backupPath = cfg::ConfigurationProvider::Get().Configuration->GetBackupPath();
    const wxString dataPath = cfg::ConfigurationProvider::Get().Configuration->GetDatabasePath();

    auto fullBackupDatabaseFilePath = wxString::Format(wxT("%s\\%s"), backupPath, fileToRestore);
    auto toCopyDatabaseFilePath = wxString::Format(wxT("%s\\%s"), dataPath, fileToRestore);

    /* Check if the backups are in the same place as main database */
    if (backupPath != dataPath) {
        /* Copy selected database file to correct path */
        bool copySuccessful = wxCopyFile(fullBackupDatabaseFilePath, toCopyDatabaseFilePath);
        if (!copySuccessful) {
            FileOperationErrorFeedback();
            pLogger->error("Failed to copy {0} to destination {1}",
                fullBackupDatabaseFilePath.ToStdString(),
                toCopyDatabaseFilePath.ToStdString());
            return;
        }
    }

    auto existingDatabaseFile =
        common::GetDatabaseFilePath(cfg::ConfigurationProvider::Get().Configuration->GetDatabasePath());

    /* If there is a existing 'db' file */
    if (!pParent->IsRestoreWithNoPreviousFileExisting()) {
        /* Terminate connection to database */
        db::ConnectionProvider::Get().PurgeConnectionPool();

        /* Rename existing file temporarily (in case any of the next steps fail) */
        bool tmpRenameOfCurrentDatabaseFileSuccessful =
            wxRenameFile(existingDatabaseFile, wxString::Format(wxT("%s.tmp"), existingDatabaseFile));
        if (!tmpRenameOfCurrentDatabaseFileSuccessful) {
            FileOperationErrorFeedback();
            pLogger->error("Failed to rename file {0} to {1}",
                existingDatabaseFile.ToStdString(),
                wxString::Format(wxT("%s.tmp"), existingDatabaseFile).ToStdString());
            return;
        }
    }

    /* Rename the selected database file to the plain name */
    bool renameToNewNameSuccessful = wxRenameFile(toCopyDatabaseFilePath, existingDatabaseFile);
    if (!renameToNewNameSuccessful) {
        FileOperationErrorFeedback();
        pLogger->error("Failed to rename file {0} to {1}",
            toCopyDatabaseFilePath.ToStdString(),
            existingDatabaseFile.ToStdString());
        return;
    }

    /* If there is a existing 'db' file */
    if (!pParent->IsRestoreWithNoPreviousFileExisting()) {
        /* Remove the temporary file */
        bool removeTmpDatabaseFile = wxRemoveFile(wxString::Format(wxT("%s.tmp"), existingDatabaseFile));
        if (!removeTmpDatabaseFile) {
            FileOperationErrorFeedback();
            pLogger->error(
                "Failed to remove file {0}", wxString::Format(wxT("%s.tmp"), existingDatabaseFile).ToStdString());
            return;
        }

        /* Restore connection to database */
        if (!InitializeDatabaseConnectionProvider()) {
            FileOperationErrorFeedback();
            pLogger->error("Failed to re-initialize database connection provider");
            return;
        }
    }

    /* Complete operation */
    pStatusInOperationLabel->SetLabel(wxT("Complete."));
    auto statusComplete = wxT("The wizard has successfully restored the\ndatabase!"
                              "\n\n\nClick 'Finish' to exit the wizard.");
    pStatusCompleteLabel->SetLabel(statusComplete);
    pGaugeCtrl->SetValue(100);
}

void DatabaseRestoredPage::OnWizardCancel(wxWizardEvent& event)
{
    auto userResponse = wxMessageBox(
        wxT("Are you sure want to cancel and exit?"), common::GetProgramName(), wxICON_QUESTION | wxYES_NO);
    if (userResponse == wxNO) {
        event.Veto();
    }
}

void DatabaseRestoredPage::FileOperationErrorFeedback()
{
    pStatusInOperationLabel->SetLabel(wxT("The operation encountered an error."));
    auto statusError = wxT("The wizard has encountered an error.\n"
                           "Any operations executed have been rolled back."
                           "\n\n\nClick 'Finish' to exit the wizard.");
    pStatusCompleteLabel->SetLabel(statusError);
    pGaugeCtrl->SetValue(100);
}

bool DatabaseRestoredPage::InitializeDatabaseConnectionProvider()
{
    auto sqliteConnectionFactory = std::make_shared<db::SqliteConnectionFactory>(
        common::GetDatabaseFilePath(cfg::ConfigurationProvider::Get().Configuration->GetDatabasePath()).ToStdString());
    auto connectionPool = std::make_unique<db::ConnectionPool<db::SqliteConnection>>(sqliteConnectionFactory, 14);
    db::ConnectionProvider::Get().ReInitializeConnectionPool(std::move(connectionPool));

    return true;
}
} // namespace app::wizard
