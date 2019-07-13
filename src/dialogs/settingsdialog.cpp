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
    EVT_CHECKBOX(IDC_BACKUP_DATABASE, SettingsDialog::OnBackupDatabaseCheck)
    EVT_CHECKBOX(IDC_SHOW_IN_TRAY, SettingsDialog::OnShowInTrayCheck)
    EVT_BUTTON(IDC_BACKUP_PATH_BUTTON, SettingsDialog::OnOpenDirectory)
    EVT_BUTTON(wxID_OK, SettingsDialog::OnOk)
    EVT_BUTTON(wxID_CANCEL, SettingsDialog::OnCancel)
wxEND_EVENT_TABLE()

SettingsDialog::SettingsDialog(wxWindow* parent, std::shared_ptr<cfg::Configuration> config, const wxString& name)
    : pConfig(config)
{
    long style = wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU;
    bool success = Create(parent, wxID_ANY, wxT("Settings"), wxDefaultPosition, wxSize(500, 320), style, name);
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

    auto panel = new wxPanel(traySettingsPanel, wxID_STATIC);
    auto panelSizer = new wxBoxSizer(wxVERTICAL);
    panel->SetSizer(panelSizer);

    pShowInTray = new wxCheckBox(panel, IDC_SHOW_IN_TRAY, wxT("Show in tray"));
    panelSizer->Add(pShowInTray, common::sizers::ControlDefault);

    // TODO Indent the below two controls with an offset to the right
    pMinimizeToTray = new wxCheckBox(panel, IDC_MINIMIZE_TO_TRAY, wxT("Minimize application to Windows tray"));
    panelSizer->Add(pMinimizeToTray, common::sizers::ControlDefault);

    pCloseToTray = new wxCheckBox(panel, IDC_CLOSE_TO_TRAY, wxT("Close application to Windows tray"));
    panelSizer->Add(pCloseToTray, common::sizers::ControlDefault);
    trayGridSizer->Add(panel, common::sizers::ControlDefault);

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

    auto backUpLocationText = new wxStaticText(databasePanel, wxID_ANY, wxT("Path"));
    gridSizer->Add(backUpLocationText, common::sizers::ControlCenter);

    pBackupPath = new wxTextCtrl(databasePanel, IDC_BACKUP_PATH, wxGetEmptyString(), wxDefaultPosition, wxSize(256, -1), 0);
    pBackupPath->SetEditable(false);
    gridSizer->Add(pBackupPath, common::sizers::ControlDefault);

    pBrowseBackupPath = new wxButton(databasePanel, IDC_BACKUP_PATH_BUTTON, wxT("Browse"));
    gridSizer->Add(pBrowseBackupPath, common::sizers::ControlDefault);

    /* Timed Task Settings */
    auto timedTaskSettingsBox = new wxStaticBox(this, wxID_ANY, wxT("Timed Task"));
    auto timedTaskSettingsSizer = new wxStaticBoxSizer(timedTaskSettingsBox, wxHORIZONTAL);
    mainSizer->Add(timedTaskSettingsSizer, common::sizers::ControlExpand);

    auto timedTaskPanel = new wxPanel(this, wxID_STATIC);
    timedTaskSettingsSizer->Add(timedTaskPanel, common::sizers::ControlDefault);

    auto timedTaskGridSizer = new wxFlexGridSizer(2, 10, 10);
    timedTaskPanel->SetSizer(timedTaskGridSizer);

    auto hideWindowTimeText = new wxStaticText(timedTaskPanel, wxID_ANY, wxT("Hide Window"));
    timedTaskGridSizer->Add(hideWindowTimeText, common::sizers::ControlDefault);

    wxArrayString hideWindowTimeChoices;
    hideWindowTimeChoices.Add("1");
    hideWindowTimeChoices.Add("2");
    hideWindowTimeChoices.Add("3");
    hideWindowTimeChoices.Add("4");
    hideWindowTimeChoices.Add("5");
    pHideWindowTimeChoice = new wxChoice(timedTaskPanel, IDC_HIDE_WINDOW_TIME_CHOICE, wxDefaultPosition, wxSize(150, -1), hideWindowTimeChoices);
    pHideWindowTimeChoice->SetToolTip(wxT("Hide the timed task after X amount of seconds"));
    timedTaskGridSizer->Add(pHideWindowTimeChoice, common::sizers::ControlDefault);

    auto notificationTimeText = new wxStaticText(timedTaskPanel, wxID_ANY, wxT("Notifications Interval"));
    timedTaskGridSizer->Add(notificationTimeText, common::sizers::ControlDefault);

    wxArrayString notificationTimeChoices;
    notificationTimeChoices.Add("5");
    notificationTimeChoices.Add("15");
    notificationTimeChoices.Add("30");
    notificationTimeChoices.Add("45");
    notificationTimeChoices.Add("60");
    pNotificationTimeChoice = new wxChoice(timedTaskPanel, IDC_NOTIFICATION_TIME_CHOICE, wxDefaultPosition, wxSize(150, -1), notificationTimeChoices);
    pNotificationTimeChoice->SetToolTip(wxT("Select a interval in minutes for timed task notification update"));
    timedTaskGridSizer->Add(pNotificationTimeChoice, common::sizers::ControlDefault);

    /* Horizontal Line*/
    auto separationLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(150, -1), wxLI_HORIZONTAL, wxT("settings_static_line"));
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
{
    pDialogOnExit->SetValue(pConfig->IsConfirmOnExit());
    pStartWithWindows->SetValue(pConfig->IsStartOnBoot());

    pShowInTray->SetValue(pConfig->IsShowInTray());
    bool enabled = pShowInTray->GetValue();
    if (enabled) {
        pMinimizeToTray->Enable();
        pCloseToTray->Enable();
    } else {
        pMinimizeToTray->Disable();
        pCloseToTray->Disable();
    }

    pMinimizeToTray->SetValue(pConfig->IsMinimizeToTray());
    pCloseToTray->SetValue(pConfig->IsCloseToTray());
    pShowBalloonNotifications->SetValue(pConfig->IsShowBalloonNotifications());

    pBackupDatabase->SetValue(pConfig->IsBackupEnabled());
    pBackupPath->SetValue(pConfig->GetBackupPath());

    enabled = pBackupDatabase->GetValue();
    if (!enabled) {
        pBackupPath->Disable();
        pBrowseBackupPath->Disable();
    }

    pHideWindowTimeChoice->SetStringSelection(std::to_string(pConfig->GetHideWindowTimerInterval()));
    pNotificationTimeChoice->SetStringSelection(std::to_string(pConfig->GetNotificationTimerInterval()));
}

void SettingsDialog::OnOk(wxCommandEvent& WXUNUSED(event))
{
    pConfig->SetConfirmOnExit(pDialogOnExit->GetValue());
    pConfig->SetStartOnBoot(pStartWithWindows->GetValue());

    pConfig->SetShowInTray(pShowInTray->GetValue());
    pConfig->SetMinimizeToTray(pMinimizeToTray->GetValue());
    pConfig->SetCloseToTray(pCloseToTray->GetValue());
    pConfig->SetShowBalloonNotifications(pShowBalloonNotifications->GetValue());

    pConfig->SetBackupEnabled(pBackupDatabase->GetValue());
    pConfig->SetBackupPath(pBackupPath->GetValue());

    pConfig->SetHideWindowTimerInterval(std::stoi(pHideWindowTimeChoice->GetStringSelection().ToStdString()));
    pConfig->SetNotificationTimerInterval(std::stoi(pNotificationTimeChoice->GetStringSelection().ToStdString()));

    pConfig->Save();

    EndModal(wxID_OK);
}

void SettingsDialog::OnCancel(wxCommandEvent& WXUNUSED(event))
{
    int ret = wxMessageBox(wxT("Are you sure you want to exit?\nAny changes you've made will be lost."), wxT("Tasks Tracker"), wxYES_NO | wxICON_QUESTION);
    if (ret == wxNO) {
        return;
    }

    EndModal(wxID_CANCEL);
}

void SettingsDialog::OnBackupDatabaseCheck(wxCommandEvent& event)
{
    if (event.IsChecked()) {
        pBackupPath->Enable();
        pBrowseBackupPath->Enable();
    } else {
        pBackupPath->Disable();
        pBrowseBackupPath->Disable();
    }
}

void SettingsDialog::OnShowInTrayCheck(wxCommandEvent & event)
{
    if (event.IsChecked()) {
        pMinimizeToTray->Enable();
        pCloseToTray->Enable();
    } else {
        pMinimizeToTray->Disable();
        pCloseToTray->Disable();
    }
}

void SettingsDialog::OnOpenDirectory(wxCommandEvent& WXUNUSED(event))
{
    auto openDirDialog = new wxDirDialog(this, wxT("Select a Backup directory"), wxEmptyString, wxDD_DEFAULT_STYLE, wxDefaultPosition);
    auto res = openDirDialog->ShowModal();
    if (res == wxID_OK) {
        auto fileLocation = openDirDialog->GetPath(); // TODO: append file name with bak extension
        pBackupPath->SetValue(fileLocation);
        pBackupPath->SetToolTip(fileLocation);
    }
    openDirDialog->Destroy();
}
}
