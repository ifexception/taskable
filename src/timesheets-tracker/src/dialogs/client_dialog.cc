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

#include "client_dialog.hh"

#include <wx/statline.h>

#include "../common/common.hh"
#include "../common/ids.hh"
#include "../services/db_service.hh"

namespace app::dialog
{
wxIMPLEMENT_DYNAMIC_CLASS(client_dialog, wxDialog);

wxBEGIN_EVENT_TABLE(client_dialog, wxDialog)
    EVT_BUTTON(ids::ID_SAVE, client_dialog::on_save)
    EVT_BUTTON(wxID_CANCEL, client_dialog::on_cancel)
wxEND_EVENT_TABLE()

client_dialog::client_dialog(wxWindow* parent, bool isEdit, const wxString& name)
    : mNameText(wxT(""))
    , mSelectedEmployerId(-1)
{
    wxString title;
    if (isEdit) {
        title = wxT("Edit Client");
    } else {
        title = wxT("Add Client");
    }

    bool success = create(parent, wxID_ANY, title, wxDefaultPosition, wxSize(320, 240), wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU, name);
}

client_dialog::~client_dialog()
{
    Destroy();
}

void client_dialog::launch_client_dialog()
{
    ShowModal();
}

bool client_dialog::create(wxWindow* parent, wxWindowID windowId, const wxString& title, const wxPoint& position, const wxSize& size, long style, const wxString& name)
{
    bool created = Create(parent, windowId, title, position, size, style, name);

    if (created) {
        create_controls();
        init_post_create();
        GetSizer()->Fit(this);
        //SetIcon();
        Centre();
    }

    return created;
}

void client_dialog::create_controls()
{
    /* Window Sizing */
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    auto mainPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(mainPanelSizer, common::sizers::ControlDefault);

    auto sizer = new wxBoxSizer(wxVERTICAL);
    mainPanelSizer->Add(sizer, 0);

    /* Task Details Box */
    auto detailsBox = new wxStaticBox(this, wxID_ANY, wxT("Task Details"));
    auto detailsBoxSizer = new wxStaticBoxSizer(detailsBox, wxVERTICAL);
    sizer->Add(detailsBoxSizer, common::sizers::ControlExpandProp);

    auto clientDetailsPanel = new wxPanel(this, wxID_STATIC);
    detailsBoxSizer->Add(clientDetailsPanel, common::sizers::ControlExpand);

    auto taskFlexGridSizer = new wxFlexGridSizer(0, 2, 0, 0);
    clientDetailsPanel->SetSizer(taskFlexGridSizer);

    /* ---Controls--- */
    /* Client Name Control */
    auto clientNameText = new wxStaticText(clientDetailsPanel, wxID_STATIC, wxT("Name"));
    taskFlexGridSizer->Add(clientNameText, common::sizers::ControlCenterVertical);

    pNameCtrl = new wxTextCtrl(clientDetailsPanel, IDC_NAME, wxGetEmptyString(), wxDefaultPosition, wxSize(150, -1), wxTE_LEFT, wxDefaultValidator, wxT("client_name_ctrl"));
    pNameCtrl->SetToolTip(wxT("Enter a name for the client"));
    taskFlexGridSizer->Add(pNameCtrl, common::sizers::ControlDefault);

    /* Employer Selection Control */
    auto employerNameText = new wxStaticText(clientDetailsPanel, wxID_STATIC, wxT("Employer"));
    taskFlexGridSizer->Add(employerNameText, common::sizers::ControlCenterVertical);

    pEmployerSelectCtrl = new wxComboBox(clientDetailsPanel, IDC_EMPLOYER, wxT("Select a employer"), wxDefaultPosition, wxDefaultSize);
    pEmployerSelectCtrl->SetToolTip(wxT("Select a employer to associate the client with"));
    taskFlexGridSizer->Add(pEmployerSelectCtrl, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL, 5);

    /* Horizontal Line*/
    auto separation_line = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(150, -1), wxLI_HORIZONTAL, wxT("new_task_static_line"));
    mainSizer->Add(separation_line, 0, wxEXPAND | wxALL, 1);

    /* Button Panel */
    auto buttonPanel = new wxPanel(this, wxID_STATIC);
    auto buttonPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonPanel->SetSizer(buttonPanelSizer);
    mainSizer->Add(buttonPanel, wxSizerFlags(wxSizerFlags().Border(wxALL, 5)).Center());

    auto okButton = new wxButton(buttonPanel, ids::ID_SAVE, wxT("&Save"));
    auto cancelButton = new wxButton(buttonPanel, wxID_CANCEL, wxT("&Cancel"));

    buttonPanelSizer->Add(okButton, wxSizerFlags().Border(wxALL, 5));
    buttonPanelSizer->Add(cancelButton, wxSizerFlags().Border(wxALL, 5));
}

void client_dialog::init_post_create()
{
    services::db_service employerService;
    std::vector<models::employer> employers;
    try {
        employers = employerService.get_active_employers();
    } catch (const std::exception& e) {
        wxLogDebug(wxT("Exception occured when getting employers"));
        wxLogDebug(e.what());
    }

    if (employers.size() == 1) {
        pEmployerSelectCtrl->AppendString(employers[0].employer_name);
        pEmployerSelectCtrl->SetSelection(0);
        pEmployerSelectCtrl->Disable();
    } else {
        for (auto employer : employers) {
            pEmployerSelectCtrl->AppendString(employer.employer_name);
        }
    }
}

bool client_dialog::validate()
{
    if (mNameText.length() > 255 || mNameText.length() < 2 || mNameText.empty()) {
        wxMessageBox(wxT("Client name is invalid"), wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
        return false;
    }

    if (mSelectedEmployerId == -1) {
        wxMessageBox(wxT("A employer is required"), wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
        return false;
    }
    return true;
}

bool client_dialog::are_controls_empty()
{
    if (mNameText.empty() && mSelectedEmployerId == -1) {
        return true;
    }
    return false;
}

void client_dialog::on_save(wxCommandEvent & event)
{
    mNameText = pNameCtrl->GetValue();
    mSelectedEmployerId = pEmployerSelectCtrl->GetSelection() + 1;

    bool isValid = validate();
    if (!isValid) {
        return;
    }

    services::db_service clientService;
    try {
        clientService.create_new_client(std::string(mNameText), mSelectedEmployerId);
    } catch (const std::exception&) {

    }
    EndModal(ids::ID_SAVE);
}

void client_dialog::on_cancel(wxCommandEvent & event)
{
    bool areControlsEmpty = are_controls_empty();
    if (!areControlsEmpty) {

        int answer = wxMessageBox(wxT("Are you sure you want to exit?"), wxT("Confirm"),
            wxYES_NO | wxICON_QUESTION);
        if (answer == wxYES) {
            EndModal(wxID_CANCEL);
        }
    } else {
        EndModal(wxID_CANCEL);
    }
}

}
