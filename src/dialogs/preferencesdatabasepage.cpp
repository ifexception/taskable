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

#include "preferencesdatabasepage.h"

#include <wx/valnum.h>

#include "../common/common.h"
#include "../config/configuration.h"

namespace app::dlg
{
DatabasePage::DatabasePage(wxWindow* parent, std::shared_ptr<cfg::Configuration> config)
    : wxPanel(parent, wxID_ANY)
    , pConfig(config)
    , pParent(parent)
    , pDatabasePathTextCtrl(nullptr)
    , pBrowseDatabasePathButton(nullptr)
    , pBackupDatabaseCtrl(nullptr)
    , pBackupPathTextCtrl(nullptr)
    , pBrowseBackupPathButton(nullptr)
    , pDeleteBackupsAfterCtrl(nullptr)
{
    CreateControls();
    ConfigureEventBindings();
    FillControls();
}

void DatabasePage::Apply()
{
    pConfig->SetDatabasePath(pDatabasePathTextCtrl->GetValue());
    pConfig->SetBackupEnabled(pBackupDatabaseCtrl->GetValue());
    pConfig->SetBackupPath(pBackupPathTextCtrl->GetValue());
    pConfig->SetDeleteBackupsAfter(std::stoi(pDeleteBackupsAfterCtrl->GetValue().ToStdString()));
}

void DatabasePage::CreateControls()
{
    auto sizer = new wxBoxSizer(wxVERTICAL);

    /* Database Settings Panel */
    auto databaseSettingsBox = new wxStaticBox(this, wxID_ANY, wxT("Database"));
    auto databaseSettingsSizer = new wxStaticBoxSizer(databaseSettingsBox, wxHORIZONTAL);

    auto databaseSizer = new wxBoxSizer(wxVERTICAL);
    databaseSettingsSizer->Add(databaseSizer, 1, wxALL | wxEXPAND, 5);

    auto databaseHorizontalSizer = new wxBoxSizer(wxHORIZONTAL);
    databaseSizer->Add(databaseHorizontalSizer, common::sizers::ControlDefault);

    auto databasePathLabel = new wxStaticText(databaseSettingsBox, wxID_ANY, wxT("Path"));
    databaseHorizontalSizer->Add(databasePathLabel, common::sizers::ControlCenter);

    pDatabasePathTextCtrl = new wxTextCtrl(
        databaseSettingsBox, IDC_DATABASE_PATH, wxGetEmptyString(), wxDefaultPosition, wxSize(256, -1), 0);
    pDatabasePathTextCtrl->SetEditable(false);
    databaseHorizontalSizer->Add(pDatabasePathTextCtrl, common::sizers::ControlDefault);

    pBrowseDatabasePathButton = new wxButton(databaseSettingsBox, IDC_DATABASE_PATH_BUTTON, wxT("Browse..."));
    databaseHorizontalSizer->Add(pBrowseDatabasePathButton, common::sizers::ControlDefault);

    sizer->Add(databaseSettingsSizer, 0, wxLEFT | wxRIGHT | wxEXPAND, 5);

    /* Backups Settings Panel */
    auto backupsSettingsBox = new wxStaticBox(this, wxID_ANY, wxT("Backups"));
    auto backupsSettingsSizer = new wxStaticBoxSizer(backupsSettingsBox, wxHORIZONTAL);

    auto backupsSizer = new wxBoxSizer(wxVERTICAL);
    backupsSettingsSizer->Add(backupsSizer, 1, wxALL | wxEXPAND, 5);

    pBackupDatabaseCtrl = new wxCheckBox(backupsSettingsBox, IDC_BACKUP_DATABASE, wxT("Backup Database"));
    backupsSizer->Add(pBackupDatabaseCtrl, common::sizers::ControlDefault);

    auto gridSizer = new wxBoxSizer(wxHORIZONTAL);
    backupsSizer->Add(gridSizer, common::sizers::ControlDefault);

    auto backUpLocationText = new wxStaticText(backupsSettingsBox, wxID_ANY, wxT("Path"));
    gridSizer->Add(backUpLocationText, common::sizers::ControlCenter);

    pBackupPathTextCtrl =
        new wxTextCtrl(backupsSettingsBox, IDC_BACKUP_PATH, wxGetEmptyString(), wxDefaultPosition, wxSize(256, -1), 0);
    pBackupPathTextCtrl->SetEditable(false);
    gridSizer->Add(pBackupPathTextCtrl, common::sizers::ControlDefault);

    pBrowseBackupPathButton = new wxButton(backupsSettingsBox, IDC_BACKUP_PATH_BUTTON, wxT("Browse..."));
    gridSizer->Add(pBrowseBackupPathButton, common::sizers::ControlDefault);

    sizer->Add(backupsSettingsSizer, 0, wxLEFT | wxRIGHT | wxEXPAND, 5);

    /* Database Backups Options Panel */
    auto databaseBackupsBox = new wxStaticBox(this, wxID_ANY, wxT("Backup Options"));
    auto databaseBackupsSizer = new wxStaticBoxSizer(databaseBackupsBox, wxHORIZONTAL);

    auto backupOptionsSizer = new wxBoxSizer(wxHORIZONTAL);
    databaseBackupsSizer->Add(backupOptionsSizer, 1, wxALL | wxEXPAND, 5);

    auto deleteBackupsAfterLabel = new wxStaticText(databaseBackupsBox, wxID_ANY, wxT("Delete Backups After (days)"));
    backupOptionsSizer->Add(deleteBackupsAfterLabel, common::sizers::ControlCenter);

    wxIntegerValidator<int> integerValidator;
    integerValidator.SetMin(1);
    integerValidator.SetMax(30);

    pDeleteBackupsAfterCtrl = new wxTextCtrl(databaseBackupsBox,
        IDC_DELETE_BACKUPS_AFTER,
        wxT("30"),
        wxDefaultPosition,
        wxSize(42, -1),
        wxTE_CENTRE,
        integerValidator);
    pDeleteBackupsAfterCtrl->SetToolTip(wxT("Number of days to keep a backup"));
    backupOptionsSizer->Add(pDeleteBackupsAfterCtrl, common::sizers::ControlDefault);

    sizer->Add(databaseBackupsSizer, 0, wxLEFT | wxRIGHT | wxEXPAND, 5);

    SetSizerAndFit(sizer);
}

void DatabasePage::ConfigureEventBindings()
{
    pBrowseDatabasePathButton->Bind(
        wxEVT_BUTTON, &DatabasePage::OnOpenDirectoryForDatabaseLocation, this, IDC_DATABASE_PATH_BUTTON);

    pBackupDatabaseCtrl->Bind(wxEVT_CHECKBOX, &DatabasePage::OnBackupDatabaseCheck, this);

    pBrowseBackupPathButton->Bind(
        wxEVT_BUTTON, &DatabasePage::OnOpenDirectoryForBackupLocation, this, IDC_BACKUP_PATH_BUTTON);
}

void DatabasePage::FillControls()
{
    pDatabasePathTextCtrl->SetValue(pConfig->GetDatabasePath());
    pBackupDatabaseCtrl->SetValue(pConfig->IsBackupEnabled());
    pBackupPathTextCtrl->SetValue(pConfig->GetBackupPath());
    pDeleteBackupsAfterCtrl->SetValue(wxString(std::to_string(pConfig->GetDeleteBackupsAfter())));

    if (!pBackupDatabaseCtrl->GetValue()) {
        pBackupPathTextCtrl->Disable();
        pBrowseBackupPathButton->Disable();
        pDeleteBackupsAfterCtrl->Disable();
    }
}

void DatabasePage::OnOpenDirectoryForDatabaseLocation(wxCommandEvent& event)
{
    wxString pathDirectory = wxGetEmptyString();
    if (pConfig->GetDatabasePath().length() == 0 && !wxDirExists(pConfig->GetDatabasePath())) {
        pathDirectory = wxStandardPaths::Get().GetAppDocumentsDir();
    } else {
        pathDirectory = pConfig->GetDatabasePath();
    }

    auto openDirDialog =
        new wxDirDialog(this, wxT("Select a Database Directory"), pathDirectory, wxDD_DEFAULT_STYLE, wxDefaultPosition);
    int res = openDirDialog->ShowModal();

    if (res == wxID_OK) {
        auto selectedDatavasePath = openDirDialog->GetPath();
        pDatabasePathTextCtrl->SetValue(selectedDatavasePath);
        pDatabasePathTextCtrl->SetToolTip(selectedDatavasePath);
    }

    openDirDialog->Destroy();
}

void DatabasePage::OnBackupDatabaseCheck(wxCommandEvent& event)
{
    if (event.IsChecked()) {
        pBackupPathTextCtrl->Enable();
        pBrowseBackupPathButton->Enable();
        pDeleteBackupsAfterCtrl->Enable();
    } else {
        pBackupPathTextCtrl->Disable();
        pBrowseBackupPathButton->Disable();
        pDeleteBackupsAfterCtrl->Disable();
    }
}

void DatabasePage::OnOpenDirectoryForBackupLocation(wxCommandEvent& event)
{
    wxString pathDirectory = wxGetEmptyString();
    if (pConfig->GetBackupPath().length() == 0 && !wxDirExists(pConfig->GetBackupPath())) {
        pathDirectory = wxStandardPaths::Get().GetAppDocumentsDir();
    } else {
        pathDirectory = pConfig->GetBackupPath();
    }
    auto openDirDialog =
        new wxDirDialog(this, wxT("Select a Backup Directory"), pathDirectory, wxDD_DEFAULT_STYLE, wxDefaultPosition);
    int res = openDirDialog->ShowModal();

    if (res == wxID_OK) {
        auto selectedBackupPath = openDirDialog->GetPath();
        pBackupPathTextCtrl->SetValue(selectedBackupPath);
        pBackupPathTextCtrl->SetToolTip(selectedBackupPath);
    }

    openDirDialog->Destroy();
}
} // namespace app::dlg
