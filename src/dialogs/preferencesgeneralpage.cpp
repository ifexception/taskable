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

#include "preferencesgeneralpage.h"

#include "../common/common.h"
#include "../config/configuration.h"

namespace app::dialog
{
GeneralPage::GeneralPage(wxWindow* parent, std::shared_ptr<cfg::Configuration> config)
    : wxPanel(parent, wxID_ANY)
    , pConfig(config)
    , pParent(parent)
    , pDialogOnExitCtrl(nullptr)
    , pStartWithWindowsCtrl(nullptr)
    , pShowInTrayCtrl(nullptr)
    , pMinimizeToTrayCtrl(nullptr)
    , pCloseToTrayCtrl(nullptr)
{
    CreateControls();
    ConfigureEventBindings();
    FillControls();
}

void GeneralPage::Apply()
{
    pConfig->SetConfirmOnExit(pDialogOnExitCtrl->GetValue());
    pConfig->SetStartOnBoot(pStartWithWindowsCtrl->GetValue());

    pConfig->SetShowInTray(pShowInTrayCtrl->GetValue());
    pConfig->SetMinimizeToTray(pMinimizeToTrayCtrl->GetValue());
    pConfig->SetCloseToTray(pCloseToTrayCtrl->GetValue());
}

void GeneralPage::CreateControls()
{
    auto sizer = new wxBoxSizer(wxVERTICAL);

    /* Display Settings */
    auto displaySettingsBox = new wxStaticBox(this, wxID_ANY, wxT("Display Options"));
    auto displaySettingsSizer = new wxStaticBoxSizer(displaySettingsBox, wxHORIZONTAL);
    auto displayGridSizer = new wxFlexGridSizer(2, 10, 10);

    pDialogOnExitCtrl = new wxCheckBox(displaySettingsBox, IDC_DIALOG_EXIT, wxT("Show confirmation dialog on exit"));
    displayGridSizer->Add(pDialogOnExitCtrl, common::sizers::ControlDefault);

    displayGridSizer->Add(0, 0);

    pStartWithWindowsCtrl =
        new wxCheckBox(displaySettingsBox, IDC_START_WITH_WINDOWS, wxT("Start application with Windows"));
    displayGridSizer->Add(pStartWithWindowsCtrl, common::sizers::ControlDefault);

    displayGridSizer->Add(0, 0);

    displaySettingsSizer->Add(displayGridSizer, 1, wxALL , 5);

    sizer->Add(displaySettingsSizer, 0, wxLEFT | wxRIGHT | wxEXPAND, 5);

    /* System Tray Settings */
    auto traySettingsBox = new wxStaticBox(this, wxID_ANY, wxT("System Tray"));
    auto traySettingsSizer = new wxStaticBoxSizer(traySettingsBox, wxHORIZONTAL);
    auto trayGridSizer = new wxFlexGridSizer(2, 10, 10);

    auto panel = new wxPanel(traySettingsBox, wxID_STATIC);
    auto panelSizer = new wxBoxSizer(wxVERTICAL);
    panel->SetSizer(panelSizer);

    pShowInTrayCtrl = new wxCheckBox(panel, IDC_SHOW_IN_TRAY, wxT("Show in tray"));
    panelSizer->Add(pShowInTrayCtrl, common::sizers::ControlDefault);

    pMinimizeToTrayCtrl = new wxCheckBox(panel, IDC_MINIMIZE_TO_TRAY, wxT("Minimize application to Windows tray"));
    panelSizer->Add(pMinimizeToTrayCtrl, 0, wxLEFT | wxBOTTOM, 10);

    trayGridSizer->Add(0, 0);

    pCloseToTrayCtrl = new wxCheckBox(panel, IDC_CLOSE_TO_TRAY, wxT("Close application to Windows tray"));
    panelSizer->Add(pCloseToTrayCtrl, 0, wxLEFT, 10);

    trayGridSizer->Add(panel, common::sizers::ControlDefault);

    traySettingsSizer->Add(trayGridSizer, 1, wxALL | wxEXPAND, 5);

    sizer->Add(traySettingsSizer, 0, wxLEFT | wxRIGHT | wxEXPAND, 5);

    SetSizerAndFit(sizer);
}

void GeneralPage::ConfigureEventBindings()
{
     pShowInTrayCtrl->Bind(wxEVT_CHECKBOX, &GeneralPage::OnShowInTrayCheck, this);
}

void GeneralPage::FillControls()
{
    pDialogOnExitCtrl->SetValue(pConfig->IsConfirmOnExit());
    pStartWithWindowsCtrl->SetValue(pConfig->IsStartOnBoot());

    pShowInTrayCtrl->SetValue(pConfig->IsShowInTray());
    if (pConfig->IsShowInTray()) {
        pMinimizeToTrayCtrl->Enable();
        pCloseToTrayCtrl->Enable();
    } else {
        pMinimizeToTrayCtrl->Disable();
        pCloseToTrayCtrl->Disable();
    }

    pMinimizeToTrayCtrl->SetValue(pConfig->IsMinimizeToTray());
    pCloseToTrayCtrl->SetValue(pConfig->IsCloseToTray());
}

void GeneralPage::OnShowInTrayCheck(wxCommandEvent& event)
{
    if (event.IsChecked()) {
        pMinimizeToTrayCtrl->Enable();
        pCloseToTrayCtrl->Enable();
    } else {
        pMinimizeToTrayCtrl->Disable();
        pCloseToTrayCtrl->Disable();
    }
}
} // namespace app::dialog
