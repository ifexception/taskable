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

#include "preferencesstopwatchpage.h"

#include "../common/common.h"
#include "../config/configuration.h"

namespace app::dlg
{
StopwatchPage::StopwatchPage(wxWindow* parent, std::shared_ptr<cfg::Configuration> config)
    : wxPanel(parent, wxID_ANY)
    , pConfig(config)
    , pParent(parent)
    , pMinimizeStopwatchWindowCtrl(nullptr)
    , pHideWindowTimeChoiceCtrl(nullptr)
    , pNotificationTimeChoiceCtrl(nullptr)
    , pPausedTaskReminderChoiceCtrl(nullptr)
    , pStartStopwatchOnLaunchCtrl(nullptr)
{
    CreateControls();
    ConfigureEventBindings();
    FillControls();
}

void StopwatchPage::Apply()
{
    pConfig->SetMinimizeStopwatchWindow(pMinimizeStopwatchWindowCtrl->GetValue());
    pConfig->SetHideWindowTimerInterval(std::stoi(pHideWindowTimeChoiceCtrl->GetStringSelection().ToStdString()));
    pConfig->SetNotificationTimerInterval(std::stoi(pNotificationTimeChoiceCtrl->GetStringSelection().ToStdString()));
    pConfig->SetPausedTaskReminderInterval(
        std::stoi(pPausedTaskReminderChoiceCtrl->GetStringSelection().ToStdString()));
    pConfig->SetStartStopwatchOnLaunch(pStartStopwatchOnLaunchCtrl->GetValue());
}

void StopwatchPage::CreateControls()
{
    auto sizer = new wxBoxSizer(wxVERTICAL);

    /* Stopwatch Settings */
    auto stopwatchSettingsBox = new wxStaticBox(this, wxID_ANY, wxT("Stopwatch"));
    auto stopwatchSettingsSizer = new wxStaticBoxSizer(stopwatchSettingsBox, wxHORIZONTAL);
    auto stopwatchGridSizer = new wxFlexGridSizer(2, 10, 10);

    pMinimizeStopwatchWindowCtrl =
        new wxCheckBox(stopwatchSettingsBox, IDC_MINIMIZE_STOPWATCH_WINDOW, wxT("Minimize Stopwatch Window"));
    stopwatchGridSizer->Add(pMinimizeStopwatchWindowCtrl, common::sizers::ControlDefault);

    stopwatchGridSizer->Add(0, 0);

    auto hideWindowTimeLabel = new wxStaticText(stopwatchSettingsBox, wxID_ANY, wxT("Hide Window (s)"));
    stopwatchGridSizer->Add(hideWindowTimeLabel, common::sizers::ControlDefault);

    wxArrayString hideWindowTimeChoices;
    hideWindowTimeChoices.Add("1");
    hideWindowTimeChoices.Add("2");
    hideWindowTimeChoices.Add("3");
    hideWindowTimeChoices.Add("4");
    hideWindowTimeChoices.Add("5");

    pHideWindowTimeChoiceCtrl = new wxChoice(
        stopwatchSettingsBox, IDC_HIDE_WINDOW_TIME_CHOICE, wxDefaultPosition, wxSize(150, -1), hideWindowTimeChoices);
    pHideWindowTimeChoiceCtrl->SetToolTip(wxT("Hide the timed task after selected amount of seconds"));
    stopwatchGridSizer->Add(pHideWindowTimeChoiceCtrl, common::sizers::ControlDefault);

    auto notificationTimeLabel = new wxStaticText(stopwatchSettingsBox, wxID_ANY, wxT("Notifications Interval (m)"));
    stopwatchGridSizer->Add(notificationTimeLabel, common::sizers::ControlDefault);

    wxArrayString notificationTimeChoices;
    notificationTimeChoices.Add("5");
    notificationTimeChoices.Add("15");
    notificationTimeChoices.Add("30");
    notificationTimeChoices.Add("45");
    notificationTimeChoices.Add("60");

    pNotificationTimeChoiceCtrl = new wxChoice(stopwatchSettingsBox,
        IDC_NOTIFICATION_TIME_CHOICE,
        wxDefaultPosition,
        wxSize(150, -1),
        notificationTimeChoices);
    pNotificationTimeChoiceCtrl->SetToolTip(wxT("Select a interval in minutes for timed task notification update"));
    stopwatchGridSizer->Add(pNotificationTimeChoiceCtrl, common::sizers::ControlDefault);

    auto pausedTaskReminerText =
        new wxStaticText(stopwatchSettingsBox, wxID_ANY, wxT("Paused Task Reminder Interval (m)"));
    stopwatchGridSizer->Add(pausedTaskReminerText, common::sizers::ControlDefault);

    wxArrayString pausedTaskReminderChoices;
    pausedTaskReminderChoices.Add(wxT("1"));
    pausedTaskReminderChoices.Add(wxT("3"));
    pausedTaskReminderChoices.Add(wxT("5"));
    pausedTaskReminderChoices.Add(wxT("10"));
    pausedTaskReminderChoices.Add(wxT("15"));

    pPausedTaskReminderChoiceCtrl = new wxChoice(stopwatchSettingsBox,
        IDC_PAUSED_TASK_REMINDER_CHOICE,
        wxDefaultPosition,
        wxSize(150, -1),
        pausedTaskReminderChoices);
    pPausedTaskReminderChoiceCtrl->SetToolTip(wxT("Select a interval in minutes for a reminder when a task is paused"));
    stopwatchGridSizer->Add(pPausedTaskReminderChoiceCtrl, common::sizers::ControlDefault);

    pStartStopwatchOnLaunchCtrl =
        new wxCheckBox(stopwatchSettingsBox, IDC_START_STOPWATCH_ON_LAUNCH, wxT("Start Stopwatch on Launch"));
    pStartStopwatchOnLaunchCtrl->SetToolTip(
        wxT("Set whether to start the stopwatch immediately or on \"Start\" button click"));
    stopwatchGridSizer->Add(pStartStopwatchOnLaunchCtrl, common::sizers::ControlDefault);

    stopwatchSettingsSizer->Add(stopwatchGridSizer, 1, wxALL | wxEXPAND, 5);

    sizer->Add(stopwatchSettingsSizer, 0, wxLEFT | wxRIGHT | wxEXPAND, 5);

    SetSizerAndFit(sizer);
}

void StopwatchPage::ConfigureEventBindings()
{
    pMinimizeStopwatchWindowCtrl->Bind(wxEVT_CHECKBOX, &StopwatchPage::OnMinimizeTimedTaskWindowCheck, this);
}

void StopwatchPage::FillControls()
{
    pMinimizeStopwatchWindowCtrl->SetValue(pConfig->IsMinimizeStopwatchWindow());
    if (!pConfig->IsMinimizeStopwatchWindow()) {
        pHideWindowTimeChoiceCtrl->Disable();
    }

    pHideWindowTimeChoiceCtrl->SetStringSelection(std::to_string(pConfig->GetHideWindowTimerInterval()));
    pNotificationTimeChoiceCtrl->SetStringSelection(std::to_string(pConfig->GetNotificationTimerInterval()));
    pPausedTaskReminderChoiceCtrl->SetStringSelection(std::to_string(pConfig->GetPausedTaskReminderInterval()));
    pStartStopwatchOnLaunchCtrl->SetValue(pConfig->IsStartStopwatchOnLaunch());
}

void StopwatchPage::OnMinimizeTimedTaskWindowCheck(wxCommandEvent& event)
{
    if (event.IsChecked()) {
        pHideWindowTimeChoiceCtrl->Enable();
    } else {
        pHideWindowTimeChoiceCtrl->Disable();
    }
}
} // namespace app::dlg
