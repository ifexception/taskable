// Productivity tool to help you track the time you spend on tasks
// Copyright (C) 2023  Szymon Welgus
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

#include "preferencestaskitempage.h"

#include "../common/common.h"
#include "../config/configuration.h"

namespace app::dlg
{
TaskItemPage::TaskItemPage(wxWindow* parent, cfg::Configuration* config)
    : wxPanel(parent, wxID_ANY)
    , pConfig(config)
    , pParent(parent)
    , pTimeRoundingCtrl(nullptr)
    , pRoundOffToChoiceCtrl(nullptr)
{
    CreateControls();
    ConfigureEventBindings();
    FillControls();
}

void TaskItemPage::Apply()
{
    pConfig->SetTimeRounding(pTimeRoundingCtrl->GetValue());
    pConfig->SetTimeToRoundTo(std::stoi(pRoundOffToChoiceCtrl->GetStringSelection().ToStdString()));
}

void TaskItemPage::CreateControls()
{
    auto sizer = new wxBoxSizer(wxVERTICAL);

    /* Task Item Settings */
    auto taskItemSettingsBox = new wxStaticBox(this, wxID_ANY, wxT("Task Item"));
    auto taskItemSettingsSizer = new wxStaticBoxSizer(taskItemSettingsBox, wxHORIZONTAL);
    auto taskItemGridSizer = new wxFlexGridSizer(2, 10, 10);

    pTimeRoundingCtrl = new wxCheckBox(taskItemSettingsBox, IDC_ENABLE_TIME_ROUNDING, wxT("Enable Time Rounding"));
    taskItemGridSizer->Add(pTimeRoundingCtrl, common::sizers::ControlDefault);

    taskItemGridSizer->Add(0, 0);

    auto timeToRoundLabel = new wxStaticText(taskItemSettingsBox, wxID_ANY, wxT("Time to Round to (m)"));
    taskItemGridSizer->Add(timeToRoundLabel, common::sizers::ControlDefault);

    wxArrayString roundOffChoices;
    roundOffChoices.Add(wxT("5"));
    roundOffChoices.Add(wxT("10"));
    roundOffChoices.Add(wxT("15"));

    pRoundOffToChoiceCtrl =
        new wxChoice(taskItemSettingsBox, IDC_ROUND_OFF_TO, wxDefaultPosition, wxSize(150, -1), roundOffChoices);
    pRoundOffToChoiceCtrl->SetToolTip(wxT("Select a interval in minutes to which time the application will round to"));
    taskItemGridSizer->Add(pRoundOffToChoiceCtrl, common::sizers::ControlDefault);

    taskItemSettingsSizer->Add(taskItemGridSizer, 1, wxALL | wxEXPAND, 5);

    sizer->Add(taskItemSettingsSizer, 0, wxLEFT | wxRIGHT | wxEXPAND, 5);

    SetSizerAndFit(sizer);
}

void TaskItemPage::ConfigureEventBindings()
{
    pTimeRoundingCtrl->Bind(wxEVT_CHECKBOX, &TaskItemPage::OnTimeRoundingCheck, this);
}

void TaskItemPage::FillControls()
{
    pTimeRoundingCtrl->SetValue(pConfig->IsTimeRoundingEnabled());
    if (!pConfig->IsTimeRoundingEnabled()) {
        pRoundOffToChoiceCtrl->Disable();
    }

    pRoundOffToChoiceCtrl->SetStringSelection(wxString(std::to_string(pConfig->GetTimeToRoundTo())));
}

void TaskItemPage::OnTimeRoundingCheck(wxCommandEvent& event)
{
    if (event.IsChecked()) {
        pRoundOffToChoiceCtrl->Enable();
    } else {
        pRoundOffToChoiceCtrl->Disable();
    }
}
} // namespace app::dlg
