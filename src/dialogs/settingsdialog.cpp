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

#include "../common/common.h"

namespace app::dialog
{
wxIMPLEMENT_DYNAMIC_CLASS(SettingsDialog, wxDialog);

wxBEGIN_EVENT_TABLE(SettingsDialog, wxDialog)
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

    auto windowsSettingsBox = new wxStaticBox(this, wxID_ANY, wxT("Windows"));
    auto windowsSettingsSizer = new wxStaticBoxSizer(windowsSettingsBox, wxHORIZONTAL);
    mainSizer->Add(windowsSettingsSizer, common::sizers::ControlExpandProp);

    auto settingsPanel = new wxPanel(this, wxID_STATIC);
}

void SettingsDialog::FillControls()
{ }

void SettingsDialog::OnSave(wxCommandEvent& event)
{ }

void SettingsDialog::OnCancel(wxCommandEvent& event)
{ }
}
