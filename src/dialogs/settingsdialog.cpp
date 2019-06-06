//  TimesheetsTracker is a desktop that aids you in tracking your timesheets
//  and seeing what work you have done.
//
//  Copyright(C) <2018> <Szymon Welgus>
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

#include "settingsdialog.h"

#include <wx/statline.h>

#include "../common/common.h"

namespace app::dialog
{
wxIMPLEMENT_DYNAMIC_CLASS(SettingsDialog, wxDialog);

wxBEGIN_EVENT_TABLE(SettingsDialog, wxDialog)
    EVT_BUTTON(IDC_BACKUP_LOCATION_BUTTON, SettingsDialog::OnOpenDirectory)
wxEND_EVENT_TABLE()

SettingsDialog::SettingsDialog(wxWindow* parent, const wxString& name)
{
    long style = wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU;
    bool success = Create(parent, wxID_ANY, wxT("Settings"), wxDefaultPosition, wxSize(480, 320), style, name);
    SetMinClientSize(wxSize(480, 320));
}

bool SettingsDialog::Create(wxWindow* parent, wxWindowID windowId, const wxString& title, const wxPoint& position, const wxSize& size, long style, const wxString& name)
{
    bool created = wxDialog::Create(parent, windowId, title, position, size, style, name);
    if (created) {
        CreateControls();
        FillControls();

        GetSizer()->Fit(this);
        SetIcon(common::GetProgramIcon());
        Center();
    }

    return created;
}

void SettingsDialog::CreateControls()
{
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    /* Display Settings */
    auto windowsSettingsBox = new wxStaticBox(this, wxID_ANY, wxT("Display Options"));
    auto windowsSettingsSizer = new wxStaticBoxSizer(windowsSettingsBox, wxHORIZONTAL);
    mainSizer->Add(windowsSettingsSizer, common::sizers::ControlExpand);

    auto settingsPanel = new wxPanel(this, wxID_STATIC);
    windowsSettingsSizer->Add(settingsPanel, common::sizers::ControlDefault);

    auto windowsGridSizer = new wxFlexGridSizer(2, 10, 10);
    settingsPanel->SetSizer(windowsGridSizer);

    pDialogOnExit = new wxCheckBox(settingsPanel, IDC_DIALOG_EXIT, wxT("Show confirmation dialog on exit"));
    windowsGridSizer->Add(pDialogOnExit, common::sizers::ControlDefault);

    pStartWithWindows = new wxCheckBox(settingsPanel, IDC_START_WITH_WINDOWS, wxT("Start application with Windows"));
    windowsGridSizer->Add(pStartWithWindows, common::sizers::ControlDefault);

    /* System Tray Settings */
    auto traySettingsBox = new wxStaticBox(this, wxID_ANY, wxT("System Tray"));
    auto traySettingsSizer = new wxStaticBoxSizer(traySettingsBox, wxHORIZONTAL);
    mainSizer->Add(traySettingsSizer, common::sizers::ControlExpand);

    auto traySettingsPanel = new wxPanel(this, wxID_STATIC);
    traySettingsSizer->Add(traySettingsPanel, common::sizers::ControlDefault);

    auto trayGridSizer = new wxFlexGridSizer(2, 10, 10);
    traySettingsPanel->SetSizer(trayGridSizer);

    pMinimizeToTray = new wxCheckBox(traySettingsPanel, IDC_MINIMIZE_TO_TRAY, wxT("Minimize application to Windows tray"));
    trayGridSizer->Add(pMinimizeToTray, common::sizers::ControlDefault);

    pCloseToTray = new wxCheckBox(traySettingsPanel, IDC_CLOSE_TO_TRAY, wxT("Close application to Windows tray"));
    trayGridSizer->Add(pCloseToTray, common::sizers::ControlDefault);

    pShowBalloonNotifications = new wxCheckBox(traySettingsPanel, IDC_SHOW_BALLOON_NOTIFICATIONS, wxT("Show balloon notifications"));
    trayGridSizer->Add(pShowBalloonNotifications, common::sizers::ControlDefault);

    /* Database Settings Panel */
    auto databaseSettingsBox = new wxStaticBox(this, wxID_ANY, wxT("Database"));
    auto databaseSettingsSizer = new wxStaticBoxSizer(databaseSettingsBox, wxHORIZONTAL);
    mainSizer->Add(databaseSettingsSizer, common::sizers::ControlExpand);

    auto databasePanel = new wxPanel(this, wxID_STATIC);
    databaseSettingsSizer->Add(databasePanel, common::sizers::ControlDefault);

    auto databaseSizer = new wxBoxSizer(wxVERTICAL);
    databasePanel->SetSizer(databaseSizer);

    pBackupDatabase = new wxCheckBox(databasePanel, IDC_BACKUP_DATABASE, wxT("Backup Database"));
    databaseSizer->Add(pBackupDatabase, common::sizers::ControlDefault);

    auto gridSizer = new wxBoxSizer(wxHORIZONTAL);
    databaseSizer->Add(gridSizer, common::sizers::ControlDefault);

    auto backUpLocationText = new wxStaticText(databasePanel, wxID_ANY, wxT("Location"));
    gridSizer->Add(backUpLocationText, common::sizers::ControlCenter);

    pBackupLocation = new wxTextCtrl(databasePanel, IDC_BACKUP_LOCATION, wxGetEmptyString(), wxDefaultPosition, wxSize(256, -1), 0);
    pBackupLocation->SetEditable(false);
    gridSizer->Add(pBackupLocation, common::sizers::ControlDefault);

    auto backupLocationButton = new wxButton(databasePanel, IDC_BACKUP_LOCATION_BUTTON, wxT("Browse"));
    gridSizer->Add(backupLocationButton, common::sizers::ControlDefault);

    /* Horizontal Line*/
    auto separationLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(150, -1), wxLI_HORIZONTAL, wxT("new_task_static_line"));
    mainSizer->Add(separationLine, 0, wxEXPAND | wxALL, 1);

    /* Button Panel */
    auto buttonPanel = new wxPanel(this, wxID_STATIC);
    auto buttonPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonPanel->SetSizer(buttonPanelSizer);
    mainSizer->Add(buttonPanel, wxSizerFlags(wxSizerFlags().Border(wxALL, 5)).Center());

    auto okButton = new wxButton(buttonPanel, wxID_OK, wxT("&Ok"));
    auto cancelButton = new wxButton(buttonPanel, wxID_CANCEL, wxT("&Cancel"));

    buttonPanelSizer->Add(okButton, wxSizerFlags().Border(wxALL, 5));
    buttonPanelSizer->Add(cancelButton, wxSizerFlags().Border(wxALL, 5));
}

void SettingsDialog::FillControls()
{ }

void SettingsDialog::OnOk(wxCommandEvent& event)
{ }

void SettingsDialog::OnCancel(wxCommandEvent& event)
{ }

void SettingsDialog::OnOpenDirectory(wxCommandEvent& WXUNUSED(event))
{
    auto openDirDialog = new wxDirDialog(this, wxT("Select a Backup directory"), wxEmptyString, wxDD_DEFAULT_STYLE, wxDefaultPosition);
    auto res = openDirDialog->ShowModal();
    if (res == wxID_OK) {
        auto fileLocation = openDirDialog->GetPath(); // TODO: append file name with bak extension
        pBackupLocation->SetValue(fileLocation);
    }
    openDirDialog->Destroy();
}
}
