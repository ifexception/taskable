//  Tasks Tracker is a desktop that aids you in tracking your timesheets
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

#include "employerdlg.h"

#include <sqlite_modern_cpp/errors.h>
#include <wx/statline.h>

#include "../common/constants.h"
#include "../common/common.h"
#include "../common/ids.h"
#include "../common/util.h"
#include "../services/db_service.h"

namespace app::dialog
{
// clang-format off
wxBEGIN_EVENT_TABLE(EmployerDialog, wxDialog)
EVT_BUTTON(wxID_OK, EmployerDialog::OnOk)
EVT_BUTTON(wxID_CANCEL, EmployerDialog::OnCancel)
EVT_CHECKBOX(EmployerDialog::IDC_ISACTIVE, EmployerDialog::OnIsActiveCheck)
wxEND_EVENT_TABLE()

EmployerDialog::EmployerDialog(wxWindow* parent, std::shared_ptr<spdlog::logger> logger, const wxString& name)
    : pLogger(logger)
    , mNameText(wxGetEmptyString())
    , bIsEdit(false)
    , mEmployerId(0)
// clang-format on
{
    Create(parent,
        wxID_ANY,
        wxT("Add Task"),
        wxDefaultPosition,
        wxSize(330, 300),
        wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU,
        name);
    SetMinClientSize(wxSize(MIN_WIDTH, MIN_HEIGHT));
}

EmployerDialog::EmployerDialog(wxWindow* parent,
    std::shared_ptr<spdlog::logger> logger,
    bool isEdit,
    int employerId,
    const wxString& name)
    : pLogger(logger)
    , mNameText(wxGetEmptyString())
    , bIsEdit(isEdit)
    , mEmployerId(employerId)
{
    Create(parent,
        wxID_ANY,
        wxT("Edit Task"),
        wxDefaultPosition,
        wxSize(330, 400),
        wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU,
        name);
    SetMinClientSize(wxSize(MIN_WIDTH, MIN_HEIGHT));
}

bool EmployerDialog::Create(wxWindow* parent,
    wxWindowID windowId,
    const wxString& title,
    const wxPoint& point,
    const wxSize& size,
    long style,
    const wxString& name)
{
    bool created = wxDialog::Create(parent, windowId, title, point, size, style, name);
    if (created) {
        CreateControls();

        if (bIsEdit) {
            DataToControls();
        }

        GetSizer()->Fit(this);
        SetIcon(common::GetProgramIcon());
        Centre();
    }

    return created;
}

void EmployerDialog::CreateControls()
{
    /* Window Sizing */
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    /* Employer Details Box */
    auto detailsBox = new wxStaticBox(this, wxID_ANY, wxT("Employer Details"));
    auto detailsBoxSizer = new wxStaticBoxSizer(detailsBox, wxVERTICAL);
    mainSizer->Add(detailsBoxSizer, common::sizers::ControlExpandProp);

    auto employerDetailsPanel = new wxPanel(this, wxID_STATIC);
    detailsBoxSizer->Add(employerDetailsPanel, common::sizers::ControlExpand);

    auto taskFlexGridSizer = new wxFlexGridSizer(0, 2, 0, 0);
    employerDetailsPanel->SetSizer(taskFlexGridSizer);

    /* --- Controls --- */
    /* Employer Name Control */
    auto employerName = new wxStaticText(employerDetailsPanel, wxID_STATIC, wxT("Name"));
    taskFlexGridSizer->Add(employerName, common::sizers::ControlCenterVertical);

    pEmployerCtrl = new wxTextCtrl(
        employerDetailsPanel, IDC_EMPLOYERTEXT, wxGetEmptyString(), wxDefaultPosition, wxSize(150, -1), wxTE_LEFT);
    pEmployerCtrl->Bind(wxEVT_KILL_FOCUS, &EmployerDialog::OnEmployerTextControlLostFocus, this);
    taskFlexGridSizer->Add(pEmployerCtrl, common::sizers::ControlDefault);

    if (bIsEdit) {
        auto isActiveFiller = new wxStaticText(employerDetailsPanel, wxID_STATIC, wxT(""));
        taskFlexGridSizer->Add(isActiveFiller, common::sizers::ControlDefault);

        /* Is Active Checkbox Control */
        pIsActiveCtrl = new wxCheckBox(employerDetailsPanel, IDC_ISACTIVE, wxT("Is Active"));
        taskFlexGridSizer->Add(pIsActiveCtrl, common::sizers::ControlDefault);

        /* Date Created Text Control */
        pDateCreatedTextCtrl = new wxStaticText(this, wxID_STATIC, wxT("Created on: %s"));
        auto font = pDateCreatedTextCtrl->GetFont();
        font.MakeItalic();
        font.SetPointSize(8);
        pDateCreatedTextCtrl->SetFont(font);
        detailsBoxSizer->Add(pDateCreatedTextCtrl, 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 10);

        /* Date Updated Text Control */
        pDateUpdatedTextCtrl = new wxStaticText(this, wxID_STATIC, wxT("Updated on: %s"));
        detailsBoxSizer->Add(pDateUpdatedTextCtrl, 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    }

    /* Horizontal Line*/
    auto separation_line = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
    mainSizer->Add(separation_line, 0, wxEXPAND | wxALL, 1);

    /* Button Panel */
    auto buttonPanel = new wxPanel(this, wxID_STATIC);
    auto buttonPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonPanel->SetSizer(buttonPanelSizer);
    mainSizer->Add(buttonPanel, common::sizers::ControlCenter);

    pOkButton = new wxButton(buttonPanel, wxID_OK, wxT("&Save"));
    auto cancelButton = new wxButton(buttonPanel, wxID_CANCEL, wxT("&Cancel"));

    buttonPanelSizer->Add(pOkButton, common::sizers::ControlDefault);
    buttonPanelSizer->Add(cancelButton, common::sizers::ControlDefault);
}

void EmployerDialog::DataToControls()
{
    services::db_service dbService;
    models::employer employer;
    try {
        employer = dbService.get_employer(mEmployerId);
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in get_employer() - {0:d} : {1}", e.get_code(), e.what());
    }

    pEmployerCtrl->ChangeValue(employer.employer_name);

    wxString dateCreatedString = util::ConvertUnixTimestampToString(employer.date_created_utc);
    wxString dateCreatedLabel = pDateCreatedTextCtrl->GetLabelText();
    pDateCreatedTextCtrl->SetLabel(wxString::Format(dateCreatedLabel, dateCreatedString));

    wxString dateUpdatedString = util::ConvertUnixTimestampToString(employer.date_modified_utc);
    wxString dateUpdatedLabel = pDateUpdatedTextCtrl->GetLabelText();
    pDateUpdatedTextCtrl->SetLabel(wxString::Format(dateUpdatedLabel, dateUpdatedString));

    pIsActiveCtrl->SetValue(employer.is_active);
}

bool EmployerDialog::Validate()
{
    if (mNameText.empty()) {
        auto message = wxString::Format(Constants::Messages::IsEmpty, wxT("Employer name"));
        wxMessageBox(message, wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
        return false;
    }

    return true;
}

bool EmployerDialog::AreControlsEmpty()
{
    bool isEmpty = mNameText.empty();
    return isEmpty;
}

void EmployerDialog::OnEmployerTextControlLostFocus(wxFocusEvent& event)
{
    if (pEmployerCtrl->GetValue().length() < 2) {
        auto message = wxString::Format(Constants::Messages::TooShort, wxT("Employer name"), 2);
        wxMessageBox(message, wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
        pOkButton->Disable();
    } else if (pEmployerCtrl->GetValue().length() > 255) {
        auto message = wxString::Format(Constants::Messages::TooLong, wxT("Employer name"), 255);
        wxMessageBox(message, wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
        pOkButton->Disable();
    } else {
        pOkButton->Enable();
    }

    event.Skip();
}

void EmployerDialog::OnOk(wxCommandEvent& event)
{
    mNameText = pEmployerCtrl->GetValue();

    bool isValid = Validate();
    if (!isValid) {
        return;
    }

    services::db_service dbService;
    try {
        if (bIsEdit && pIsActiveCtrl->IsChecked()) {
            models::employer employer;
            employer.employer_name = std::string(mNameText.ToUTF8());
            employer.date_modified_utc = util::UnixTimestamp();
            dbService.update_employer(employer);
        }
        if (bIsEdit && !pIsActiveCtrl->IsChecked()) {
            dbService.delete_employer(mEmployerId, util::UnixTimestamp());
        }
        if (!bIsEdit) {
            dbService.create_new_employer(std::string(mNameText.ToUTF8()));
        }
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in employer OnSave() - {0:d} : {1}", e.get_code(), e.what());
    }

    EndModal(wxID_OK);
}

void EmployerDialog::OnCancel(wxCommandEvent& event)
{
    bool areControlsEmpty = AreControlsEmpty();
    if (!areControlsEmpty) {
        int answer = wxMessageBox(wxT("Are you sure you want to cancel?"), wxT("Confirm"), wxYES_NO | wxICON_QUESTION);
        if (answer == wxYES) {
            EndModal(wxID_CANCEL);
        }
    } else {
        EndModal(wxID_CANCEL);
    }
}

void EmployerDialog::OnIsActiveCheck(wxCommandEvent& event)
{
    if (event.IsChecked()) {
        pEmployerCtrl->Enable();
    } else {
        pEmployerCtrl->Disable();
    }
}
} // namespace app::dialog
