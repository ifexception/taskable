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

#include "../common/common.h"
#include "../config/configuration.h"

namespace app::dlg
{
DatabasePage::DatabasePage(wxWindow* parent, std::shared_ptr<cfg::Configuration> config)
    : wxPanel(parent, wxID_ANY)
    , pConfig(config)
    , pParent(parent)
    , pBackupDatabaseCtrl(nullptr)
    , pBackupPathTextCtrl(nullptr)
    , pBrowseBackupPathButton(nullptr)
{
    CreateControls();
    ConfigureEventBindings();
    FillControls();
}

void DatabasePage::Apply()
{
    pConfig->SetBackupEnabled(pBackupDatabaseCtrl->GetValue());
    pConfig->SetBackupPath(pBackupPathTextCtrl->GetValue());
}

void DatabasePage::CreateControls()
{
    auto sizer = new wxBoxSizer(wxVERTICAL);

    /* Database Settings Panel */
    auto databaseSettingsBox = new wxStaticBox(this, wxID_ANY, wxT("Database"));
    auto databaseSettingsSizer = new wxStaticBoxSizer(databaseSettingsBox, wxHORIZONTAL);

    auto databaseSizer = new wxBoxSizer(wxVERTICAL);
    databaseSettingsSizer->Add(databaseSizer, 1, wxALL | wxEXPAND, 5);

    pBackupDatabaseCtrl = new wxCheckBox(databaseSettingsBox, IDC_BACKUP_DATABASE, wxT("Backup Database"));
    databaseSizer->Add(pBackupDatabaseCtrl, common::sizers::ControlDefault);

    auto gridSizer = new wxBoxSizer(wxHORIZONTAL);
    databaseSizer->Add(gridSizer, common::sizers::ControlDefault);

    auto backUpLocationText = new wxStaticText(databaseSettingsBox, wxID_ANY, wxT("Path"));
    gridSizer->Add(backUpLocationText, common::sizers::ControlCenter);

    pBackupPathTextCtrl =
        new wxTextCtrl(databaseSettingsBox, IDC_BACKUP_PATH, wxGetEmptyString(), wxDefaultPosition, wxSize(256, -1), 0);
    pBackupPathTextCtrl->SetEditable(false);
    gridSizer->Add(pBackupPathTextCtrl, common::sizers::ControlDefault);

    pBrowseBackupPathButton = new wxButton(databaseSettingsBox, IDC_BACKUP_PATH_BUTTON, wxT("Browse..."));
    gridSizer->Add(pBrowseBackupPathButton, common::sizers::ControlDefault);

    sizer->Add(databaseSettingsSizer, 0, wxLEFT | wxRIGHT | wxEXPAND, 5);

    SetSizerAndFit(sizer);
}

void DatabasePage::ConfigureEventBindings()
{
    pBackupDatabaseCtrl->Bind(wxEVT_CHECKBOX, &DatabasePage::OnBackupDatabaseCheck, this);

    pBrowseBackupPathButton->Bind(wxEVT_BUTTON, &DatabasePage::OnOpenDirectory, this, IDC_BACKUP_PATH_BUTTON);
}

void DatabasePage::FillControls()
{
    pBackupDatabaseCtrl->SetValue(pConfig->IsBackupEnabled());
    pBackupPathTextCtrl->SetValue(pConfig->GetBackupPath());

    if (!pBackupDatabaseCtrl->GetValue()) {
        pBackupPathTextCtrl->Disable();
        pBrowseBackupPathButton->Disable();
    }
}

void DatabasePage::OnBackupDatabaseCheck(wxCommandEvent& event)
{
    if (event.IsChecked()) {
        pBackupPathTextCtrl->Enable();
        pBrowseBackupPathButton->Enable();
    } else {
        pBackupPathTextCtrl->Disable();
        pBrowseBackupPathButton->Disable();
    }
}

void DatabasePage::OnOpenDirectory(wxCommandEvent& event)
{
    wxString pathDirectory;
    if (pConfig->GetBackupPath().length() == 0) {
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
