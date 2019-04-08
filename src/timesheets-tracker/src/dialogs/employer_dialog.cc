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

#include "employer_dialog.hh"

#include <wx/statline.h>

#include "../common/common.hh"
#include "../common/ids.hh"
#include "../services/db_service.hh"

namespace app::dialog
{
wxIMPLEMENT_DYNAMIC_CLASS(employer_dialog, wxDialog);

wxBEGIN_EVENT_TABLE(employer_dialog, wxDialog)
    EVT_BUTTON(ids::ID_SAVE, employer_dialog::on_save)
    EVT_BUTTON(wxID_CANCEL, employer_dialog::on_cancel)
wxEND_EVENT_TABLE()

employer_dialog::employer_dialog(wxWindow* parent, bool isEdit, const wxString& name)
    : mEmployerText(wxT(""))
{
    long style = wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU;
    wxSize employerSize(WIDTH, HEIGHT);
    wxString title;
    if (isEdit) {
        title = wxT("Edit Employer");
    } else {
        title = wxT("Add Employer");
    }
    bool success = create(parent, wxID_ANY, title, wxDefaultPosition, employerSize, style, name);

    SetMinClientSize(wxSize(MIN_WIDTH, MIN_HEIGHT));
}

employer_dialog::~employer_dialog()
{
    Destroy();
}

void employer_dialog::launch_employer_dialog()
{
    ShowModal();
}

bool employer_dialog::create(wxWindow* parent,
    wxWindowID windowId,
    const wxString& title,
    const wxPoint& point,
    const wxSize& size,
    long style,
    const wxString& name)
{
    bool created = wxDialog::Create(parent, windowId, title, point, size, style, name);
    if (created) {
        create_controls();

        GetSizer()->Fit(this);
        //SetIcon
        Centre();
    }

    return created;
}

void employer_dialog::create_controls()
{
    /* Window Sizing */
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    auto mainPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(mainPanelSizer, wxSizerFlags().Border(wxALL, 5));

    auto sizer = new wxBoxSizer(wxVERTICAL);
    mainPanelSizer->Add(sizer, 0);

    /* Employer Details Box */
    auto detailsBox = new wxStaticBox(this, wxID_ANY, wxT("Employer Details"));
    auto detailsBoxSizer = new wxStaticBoxSizer(detailsBox, wxVERTICAL);
    sizer->Add(detailsBoxSizer, common::sizers::ControlExpandProp);

    auto employerDetailsPanel = new wxPanel(this, wxID_STATIC);
    detailsBoxSizer->Add(employerDetailsPanel, common::sizers::ControlExpand);

    auto taskFlexGridSizer = new wxFlexGridSizer(0, 2, 0, 0);
    employerDetailsPanel->SetSizer(taskFlexGridSizer);

    /* --- Controls --- */
    /* Employer Name Control */
    auto employerName = new wxStaticText(employerDetailsPanel, wxID_STATIC, wxT("Name"));
    taskFlexGridSizer->Add(employerName, common::sizers::ControlCenterVertical);

    pEmployerCtrl = new wxTextCtrl(employerDetailsPanel, wxID_STATIC, wxGetEmptyString(), wxDefaultPosition, wxSize(150, -1), wxTE_LEFT, wxDefaultValidator, wxT("employer_name_ctrl"));
    taskFlexGridSizer->Add(pEmployerCtrl, common::sizers::ControlDefault);

    /* Employer Is Active warning */
    wxString activeEmployerString(wxT("Adding a employer will automatically make it the active one"));
    auto employerActiveText = new wxStaticText(this, wxID_ANY, activeEmployerString, wxDefaultPosition, wxSize(316, -1));
    auto font = employerActiveText->GetFont();
    font.MakeItalic();
    employerActiveText->SetFont(font);
    detailsBoxSizer->Add(employerActiveText, 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 10);

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

bool employer_dialog::validate()
{
    bool isInvalid = mEmployerText.length() > 255 || mEmployerText.length() < 2 ||
                   mEmployerText.empty();
    if (isInvalid) {
        wxMessageBox(wxT("Employer name is invalid"), wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
        return false;
    }
    return true;
}

bool employer_dialog::are_controls_empty()
{
    bool isEmpty = mEmployerText.empty();
    if (!isEmpty) {
        return false;
    }
    return true;
}

void employer_dialog::on_save(wxCommandEvent& event)
{
    mEmployerText = pEmployerCtrl->GetValue();

    bool validationSuccess = validate();
    if (!validationSuccess) {
        return;
    }

    services::db_service employerService;
    try {
        employerService.create_new_employer(std::string(mEmployerText));
    } catch (const std::exception& e) {
        wxMessageBox(wxT("An error occured\n"), wxT("Error"), wxOK_DEFAULT | wxICON_ERROR);
        return;
    }

    Destroy();
}

void employer_dialog::on_cancel(wxCommandEvent& event)
{
    bool areControlsEmpty = are_controls_empty();
    if (!areControlsEmpty) {

        int answer = wxMessageBox(wxT("Are you sure you want to cancel?"), wxT("Confirm"),
                                  wxYES_NO | wxICON_QUESTION);
        if (answer == wxYES) {
            Destroy();
        }
    } else {
        Destroy();
    }
}
}
