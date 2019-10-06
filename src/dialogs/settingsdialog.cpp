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

#include "settingsdialog.h"

#include <wx/statline.h>

#include "../common/common.h"

namespace app::dialog
{
// clang-format off
wxBEGIN_EVENT_TABLE(SettingsDialog, wxDialog)
    EVT_CHECKBOX(IDC_BACKUP_DATABASE, SettingsDialog::OnBackupDatabaseCheck)
    EVT_CHECKBOX(IDC_SHOW_IN_TRAY, SettingsDialog::OnShowInTrayCheck)
    EVT_CHECKBOX(IDC_CLOSE_TIMED_TASK_WINDOW, SettingsDialog::OnMinimizeTimedTaskWindowCheck)
    EVT_CHECKBOX(IDC_ENABLE_TIME_ROUNDING, SettingsDialog::OnTimeRoundingCheck)
    EVT_BUTTON(IDC_BACKUP_PATH_BUTTON, SettingsDialog::OnOpenDirectory)
    EVT_BUTTON(wxID_OK, SettingsDialog::OnOk)
    EVT_BUTTON(wxID_CANCEL, SettingsDialog::OnCancel)
wxEND_EVENT_TABLE()

SettingsDialog::SettingsDialog(wxWindow* parent, std::shared_ptr<cfg::Configuration> config, const wxString& name)
: pConfig(config)
// clang-format on
{
    bool success = Create(parent,
        wxID_ANY,
        wxT("Settings"),
        wxDefaultPosition,
        wxSize(500, 320),
        wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU,
        name);
    SetMinClientSize(wxSize(480, 320));
}

bool SettingsDialog::Create(wxWindow* parent,
    wxWindowID windowId,
    const wxString& title,
    const wxPoint& position,
    const wxSize& size,
    long style,
    const wxString& name)
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

    pMinimizeToTray = new wxCheckBox(panel, IDC_MINIMIZE_TO_TRAY, wxT("Minimize application to Windows tray"));
    panelSizer->Add(pMinimizeToTray, 0, wxLEFT | wxBOTTOM, 10);

    pCloseToTray = new wxCheckBox(panel, IDC_CLOSE_TO_TRAY, wxT("Close application to Windows tray"));
    panelSizer->Add(pCloseToTray, 0, wxLEFT, 10);
    trayGridSizer->Add(panel, common::sizers::ControlDefault);

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

    pBackupPath =
        new wxTextCtrl(databasePanel, IDC_BACKUP_PATH, wxGetEmptyString(), wxDefaultPosition, wxSize(256, -1), 0);
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

    pMinimizeTimedTaskWindow =
        new wxCheckBox(timedTaskPanel, IDC_CLOSE_TIMED_TASK_WINDOW, wxT("Minimize Timed Task Window"));
    timedTaskGridSizer->Add(pMinimizeTimedTaskWindow, common::sizers::ControlDefault);

    auto minimizeTimedTaskGridFiller = new wxStaticText(timedTaskPanel, wxID_ANY, wxT(""));
    timedTaskGridSizer->Add(minimizeTimedTaskGridFiller, common::sizers::ControlDefault);

    auto hideWindowTimeText = new wxStaticText(timedTaskPanel, wxID_ANY, wxT("Hide Window (s)"));
    timedTaskGridSizer->Add(hideWindowTimeText, common::sizers::ControlDefault);

    wxArrayString hideWindowTimeChoices;
    hideWindowTimeChoices.Add("1");
    hideWindowTimeChoices.Add("2");
    hideWindowTimeChoices.Add("3");
    hideWindowTimeChoices.Add("4");
    hideWindowTimeChoices.Add("5");
    pHideWindowTimeChoice = new wxChoice(
        timedTaskPanel, IDC_HIDE_WINDOW_TIME_CHOICE, wxDefaultPosition, wxSize(150, -1), hideWindowTimeChoices);
    pHideWindowTimeChoice->SetToolTip(wxT("Hide the timed task after X amount of seconds"));
    timedTaskGridSizer->Add(pHideWindowTimeChoice, common::sizers::ControlDefault);

    auto notificationTimeText = new wxStaticText(timedTaskPanel, wxID_ANY, wxT("Notifications Interval (m)"));
    timedTaskGridSizer->Add(notificationTimeText, common::sizers::ControlDefault);

    wxArrayString notificationTimeChoices;
    notificationTimeChoices.Add("5");
    notificationTimeChoices.Add("15");
    notificationTimeChoices.Add("30");
    notificationTimeChoices.Add("45");
    notificationTimeChoices.Add("60");
    pNotificationTimeChoice = new wxChoice(
        timedTaskPanel, IDC_NOTIFICATION_TIME_CHOICE, wxDefaultPosition, wxSize(150, -1), notificationTimeChoices);
    pNotificationTimeChoice->SetToolTip(wxT("Select a interval in minutes for timed task notification update"));
    timedTaskGridSizer->Add(pNotificationTimeChoice, common::sizers::ControlDefault);

    auto pausedTaskReminerText = new wxStaticText(timedTaskPanel, wxID_ANY, wxT("Paused Task Reminder Interval (m)"));
    timedTaskGridSizer->Add(pausedTaskReminerText, common::sizers::ControlDefault);

    wxArrayString pausedTaskReminderChoices;
    pausedTaskReminderChoices.Add(wxT("1"));
    pausedTaskReminderChoices.Add(wxT("3"));
    pausedTaskReminderChoices.Add(wxT("5"));
    pausedTaskReminderChoices.Add(wxT("10"));
    pausedTaskReminderChoices.Add(wxT("15"));
    pPausedTaskReminderChoice = new wxChoice(
        timedTaskPanel, IDC_PAUSED_TASK_REMINDER_CHOICE, wxDefaultPosition, wxSize(150, -1), pausedTaskReminderChoices);
    pPausedTaskReminderChoice->SetToolTip(wxT("Select a interval in minutes for a reminder when a task is paused"));
    timedTaskGridSizer->Add(pPausedTaskReminderChoice, common::sizers::ControlDefault);

    /* Task Item Settings */
    auto taskItemSettingsBox = new wxStaticBox(this, wxID_ANY, wxT("Task Item"));
    auto taskItemSettingsSizer = new wxStaticBoxSizer(taskItemSettingsBox, wxHORIZONTAL);
    mainSizer->Add(taskItemSettingsSizer, common::sizers::ControlExpand);

    auto taskItemPanel = new wxPanel(this, wxID_STATIC);
    taskItemSettingsSizer->Add(taskItemPanel, common::sizers::ControlDefault);

    auto taskItemGridSizer = new wxFlexGridSizer(2, 10, 10);
    taskItemPanel->SetSizer(taskItemGridSizer);

    pTimeRounding = new wxCheckBox(taskItemPanel, IDC_ENABLE_TIME_ROUNDING, wxT("Enable Time Rounding"));
    taskItemGridSizer->Add(pTimeRounding, common::sizers::ControlDefault);

    auto timeRoundingFiller = new wxStaticText(taskItemPanel, wxID_STATIC, wxT(""));
    taskItemGridSizer->Add(timeRoundingFiller, common::sizers::ControlDefault);

    auto timeToRoundText = new wxStaticText(taskItemPanel, wxID_ANY, wxT("Time to Round to (m)"));
    taskItemGridSizer->Add(timeToRoundText, common::sizers::ControlDefault);

    wxArrayString roundOffChoices;
    roundOffChoices.Add(wxT("5"));
    roundOffChoices.Add(wxT("10"));
    roundOffChoices.Add(wxT("15"));
    pRoundOffToChoice =
        new wxChoice(taskItemPanel, IDC_ROUND_OFF_TO, wxDefaultPosition, wxSize(150, -1), roundOffChoices);
    pRoundOffToChoice->SetToolTip(wxT("Select a interval in minutes to which time the application will round to"));
    taskItemGridSizer->Add(pRoundOffToChoice, common::sizers::ControlDefault);

    /* Horizontal Line*/
    auto separationLine = new wxStaticLine(
        this, wxID_ANY, wxDefaultPosition, wxSize(150, -1), wxLI_HORIZONTAL);
    mainSizer->Add(separationLine, 0, wxEXPAND | wxALL, 1);

    /* Button Panel */
    auto buttonPanel = new wxPanel(this, wxID_STATIC);
    auto buttonPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonPanel->SetSizer(buttonPanelSizer);
    mainSizer->Add(buttonPanel, common::sizers::ControlCenter);

    auto okButton = new wxButton(buttonPanel, wxID_OK, wxT("&OK"));
    auto cancelButton = new wxButton(buttonPanel, wxID_CANCEL, wxT("&Cancel"));
    cancelButton->SetFocus();

    buttonPanelSizer->Add(okButton, common::sizers::ControlDefault);
    buttonPanelSizer->Add(cancelButton, common::sizers::ControlDefault);
}

void SettingsDialog::FillControls()
{
    pDialogOnExit->SetValue(pConfig->IsConfirmOnExit());
    pStartWithWindows->SetValue(pConfig->IsStartOnBoot());

    pShowInTray->SetValue(pConfig->IsShowInTray());
    if (pConfig->IsShowInTray()) {
        pMinimizeToTray->Enable();
        pCloseToTray->Enable();
    } else {
        pMinimizeToTray->Disable();
        pCloseToTray->Disable();
    }

    pMinimizeToTray->SetValue(pConfig->IsMinimizeToTray());
    pCloseToTray->SetValue(pConfig->IsCloseToTray());

    pBackupDatabase->SetValue(pConfig->IsBackupEnabled());
    pBackupPath->SetValue(pConfig->GetBackupPath());

    if (!pBackupDatabase->GetValue()) {
        pBackupPath->Disable();
        pBrowseBackupPath->Disable();
    }

    pMinimizeTimedTaskWindow->SetValue(pConfig->IsMinimizeTimedTaskWindow());
    if (!pConfig->IsMinimizeTimedTaskWindow()) {
        pHideWindowTimeChoice->Disable();
    }

    pHideWindowTimeChoice->SetStringSelection(std::to_string(pConfig->GetHideWindowTimerInterval()));
    pNotificationTimeChoice->SetStringSelection(std::to_string(pConfig->GetNotificationTimerInterval()));
    pPausedTaskReminderChoice->SetStringSelection(std::to_string(pConfig->GetPausedTaskReminderInterval()));

    pTimeRounding->SetValue(pConfig->IsTimeRoundingEnabled());
    if (!pConfig->IsTimeRoundingEnabled()) {
        pRoundOffToChoice->Disable();
    }

    pRoundOffToChoice->SetStringSelection(std::to_string(pConfig->GetTimeToRoundTo()));
}

void SettingsDialog::OnOk(wxCommandEvent& WXUNUSED(event))
{
    pConfig->SetConfirmOnExit(pDialogOnExit->GetValue());
    pConfig->SetStartOnBoot(pStartWithWindows->GetValue());

    pConfig->SetShowInTray(pShowInTray->GetValue());
    pConfig->SetMinimizeToTray(pMinimizeToTray->GetValue());
    pConfig->SetCloseToTray(pCloseToTray->GetValue());

    pConfig->SetBackupEnabled(pBackupDatabase->GetValue());
    pConfig->SetBackupPath(pBackupPath->GetValue());

    pConfig->SetMinimizeTimedTaskWindow(pMinimizeTimedTaskWindow->GetValue());
    pConfig->SetHideWindowTimerInterval(std::stoi(pHideWindowTimeChoice->GetStringSelection().ToStdString()));
    pConfig->SetNotificationTimerInterval(std::stoi(pNotificationTimeChoice->GetStringSelection().ToStdString()));
    pConfig->SetPausedTaskReminderInterval(std::stoi(pPausedTaskReminderChoice->GetStringSelection().ToStdString()));

    pConfig->SetTimeRounding(pTimeRounding->GetValue());
    pConfig->SetTimeToRoundTo(std::stoi(pRoundOffToChoice->GetStringSelection().ToStdString()));

    pConfig->Save();

    EndModal(wxID_OK);
}

void SettingsDialog::OnCancel(wxCommandEvent& WXUNUSED(event))
{
    int ret = wxMessageBox(wxT("Are you sure you want to exit?\nAny changes made will be lost."),
        wxT("Taskable"),
        wxYES_NO | wxICON_QUESTION);
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

void SettingsDialog::OnShowInTrayCheck(wxCommandEvent& event)
{
    if (event.IsChecked()) {
        pMinimizeToTray->Enable();
        pCloseToTray->Enable();
    } else {
        pMinimizeToTray->Disable();
        pCloseToTray->Disable();
    }
}

void SettingsDialog::OnMinimizeTimedTaskWindowCheck(wxCommandEvent& event)
{
    if (event.IsChecked()) {
        pHideWindowTimeChoice->Enable();
    } else {
        pHideWindowTimeChoice->Disable();
    }
}

void SettingsDialog::OnOpenDirectory(wxCommandEvent& WXUNUSED(event))
{
    auto openDirDialog =
        new wxDirDialog(this, wxT("Select a Backup directory"), wxEmptyString, wxDD_DEFAULT_STYLE, wxDefaultPosition);
    auto res = openDirDialog->ShowModal();
    if (res == wxID_OK) {
        auto fileLocation = openDirDialog->GetPath(); // TODO: append file name with bak extension
        pBackupPath->SetValue(fileLocation);
        pBackupPath->SetToolTip(fileLocation);
    }
    openDirDialog->Destroy();
}

void SettingsDialog::OnTimeRoundingCheck(wxCommandEvent& event)
{
    if (event.IsChecked()) {
        pRoundOffToChoice->Enable();
    } else {
        pRoundOffToChoice->Disable();
    }
}
} // namespace app::dialog
