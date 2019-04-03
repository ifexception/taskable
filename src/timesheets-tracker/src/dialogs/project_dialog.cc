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

#include "project_dialog.hh"

#include <wx/statline.h>

#include "../common/common.hh"
#include "../common/ids.hh"

namespace app::dialog
{
wxIMPLEMENT_DYNAMIC_CLASS(project_dialog, wxDialog)

wxBEGIN_EVENT_TABLE(project_dialog, wxDialog)
wxEND_EVENT_TABLE()

project_dialog::project_dialog(wxWindow* parent, bool isEdit, const wxString& name)
    : mNameText(wxT(""))
    , mDisplayNameText(wxT(""))
{
    wxString title;
    if (isEdit) {
        title = wxT("Edit Project");
    } else {
        title = wxT("Add Project");
    }
    bool success = create(parent, wxID_ANY, title, wxDefaultPosition, wxSize(420, 380), wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU, name);
}

project_dialog::~project_dialog()
{
    Destroy();
}

void project_dialog::launch_project_dialog()
{
    ShowModal();
}

bool project_dialog::create(wxWindow* parent, wxWindowID windowId, const wxString& title, const wxPoint& position, const wxSize& size, long style, const wxString& name)
{
    bool created = wxDialog::Create(parent, windowId, title, position, size, style, name);

    if (created) {
        create_controls();

        GetSizer()->Fit(this);
        //SetIcon();
        Centre();
    }
    return created;
}

void project_dialog::create_controls()
{
    /* Window Sizing */
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    auto mainPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(mainPanelSizer, common::sizers::ControlDefault);

    auto sizer = new wxBoxSizer(wxVERTICAL);
    mainPanelSizer->Add(sizer, 0);

    /* Project Details Box */
    auto detailsBox = new wxStaticBox(this, wxID_ANY, wxT("Task Details"));
    auto detailsBoxSizer = new wxStaticBoxSizer(detailsBox, wxVERTICAL);
    sizer->Add(detailsBoxSizer, common::sizers::ControlExpand);

    auto projectDetailsPanel = new wxPanel(this, wxID_STATIC);
    detailsBoxSizer->Add(projectDetailsPanel, common::sizers::ControlExpand);

    auto taskFlexGridSizer = new wxFlexGridSizer(0, 2, 0, 0);
    projectDetailsPanel->SetSizer(taskFlexGridSizer);

    /* ---Controls--- */
    /* Project Name Control */
    auto projectName = new wxStaticText(projectDetailsPanel, wxID_STATIC, wxT("Name"));
    taskFlexGridSizer->Add(projectName, common::sizers::ControlCenterVertical);

    pNameCtrl = new wxTextCtrl(projectDetailsPanel, wxID_STATIC, wxGetEmptyString(), wxDefaultPosition, wxSize(150, -1), wxTE_LEFT, wxDefaultValidator, wxT("project_name_ctrl"));
    pNameCtrl->SetToolTip(wxT("Enter a name for the project"));
    taskFlexGridSizer->Add(pNameCtrl, common::sizers::ControlDefault);

    /* Project Display Name Control */
    auto projectDisplayName = new wxStaticText(projectDetailsPanel, wxID_STATIC, wxT("Display Name"));
    taskFlexGridSizer->Add(projectDisplayName, common::sizers::ControlCenterVertical);

    pDisplayNameCtrl = new wxTextCtrl(projectDetailsPanel, wxID_STATIC, wxGetEmptyString(), wxDefaultPosition, wxSize(150, -1), wxTE_LEFT, wxDefaultValidator, wxT("project_display_name_ctrl"));
    pDisplayNameCtrl->SetToolTip(wxT("Enter a shortened, convenient display name for the project"));
    taskFlexGridSizer->Add(pDisplayNameCtrl, common::sizers::ControlDefault);

    /* Horizontal Line*/
    auto separation_line = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL, wxT("new_task_static_line"));
    mainSizer->Add(separation_line, 0, wxEXPAND | wxALL, 1);

    /* Button Panel */
    auto buttonPanel = new wxPanel(this, wxID_STATIC);
    auto buttonPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonPanel->SetSizer(buttonPanelSizer);
    mainSizer->Add(buttonPanel, common::sizers::ControlCenter);

    auto okButton = new wxButton(buttonPanel, ids::ID_SAVE, wxT("&Save"));
    auto cancelButton = new wxButton(buttonPanel, wxID_CANCEL, wxT("&Cancel"));

    buttonPanelSizer->Add(okButton, common::sizers::ControlDefault);
    buttonPanelSizer->Add(cancelButton, common::sizers::ControlDefault);
}

bool project_dialog::validate()
{
    if (mNameText.length() > 255 || mNameText.length() < 2 || mNameText.empty()) {
        wxMessageBox(wxT("Project name is invalid"), wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
        return false;
    }

    if (mDisplayNameText.length() > 255 || mDisplayNameText.length() < 2 || mDisplayNameText.empty()) {
        wxMessageBox(wxT("Display name is invalid"), wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
        return false;
    }

    return true;
}

bool project_dialog::are_controls_empty()
{
    bool isEmpty = mNameText.empty() && mDisplayNameText.empty();
    return isEmpty;
}

void project_dialog::on_save(wxCommandEvent& event)
{
    mNameText = pNameCtrl->GetValue();
    mDisplayNameText = pDisplayNameCtrl->GetValue();

    bool isValid = validate();
    if (!isValid) {
        return;
    }
}

void project_dialog::on_cancel(wxCommandEvent& event)
{
    bool areControlsEmpty = are_controls_empty();
    if (!areControlsEmpty) {

        int answer = wxMessageBox(wxT("Are you sure you want to cancel?"), wxT("Confirm"),
            wxYES_NO | wxICON_QUESTION);
        if (answer == wxYES) {
            Destroy();
        }
    }
    else {
        Destroy();
    }
}
}
