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

#include "employerdlg.h"

#include <sqlite_modern_cpp/errors.h>
#include <wx/richtooltip.h>
#include <wx/statline.h>

#include "../common/constants.h"
#include "../common/common.h"
#include "../common/ids.h"
#include "../common/util.h"
#include "../services/db_service.h"

namespace app::dialog
{
const wxString& EmployerDialog::DateLabel = wxT("Created %s | Updated %s");

EmployerDialog::EmployerDialog(wxWindow* parent, std::shared_ptr<spdlog::logger> logger, const wxString& name)
    : pLogger(logger)
    , bIsEdit(false)
    , mEmployerId(-1)
    , mEmployer()
{
    Create(parent,
        wxID_ANY,
        wxT("Add Employer"),
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
    , bIsEdit(isEdit)
    , mEmployerId(employerId)
    , mEmployer(employerId)
{
    Create(parent,
        wxID_ANY,
        wxT("Edit Employer"),
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
        ConfigureEventBindings();

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

    pNameTextCtrl = new wxTextCtrl(
        employerDetailsPanel, IDC_EMPLOYERTEXT, wxGetEmptyString(), wxDefaultPosition, wxSize(150, -1), wxTE_LEFT);
    pNameTextCtrl->SetHint(wxT("Enter employer name"));
    pNameTextCtrl->SetToolTip(wxT("Enter a name for the employer"));
    taskFlexGridSizer->Add(pNameTextCtrl, common::sizers::ControlDefault);

    if (bIsEdit) {
        auto isActiveFiller = new wxStaticText(employerDetailsPanel, wxID_STATIC, wxT(""));
        taskFlexGridSizer->Add(isActiveFiller, common::sizers::ControlDefault);

        /* Is Active Checkbox Control */
        pIsActiveCtrl = new wxCheckBox(employerDetailsPanel, IDC_ISACTIVE, wxT("Is Active"));
        taskFlexGridSizer->Add(pIsActiveCtrl, common::sizers::ControlDefault);

        /* Date Created Text Control */
        pDateTextCtrl = new wxStaticText(this, wxID_STATIC, wxT("Created on: %s"));
        auto font = pDateTextCtrl->GetFont();
        font.SetPointSize(7);
        pDateTextCtrl->SetFont(font);
        detailsBoxSizer->Add(pDateTextCtrl, 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    }

    /* Horizontal Line*/
    auto separation_line = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
    mainSizer->Add(separation_line, 0, wxEXPAND | wxALL, 1);

    /* Button Panel */
    auto buttonPanel = new wxPanel(this, wxID_STATIC);
    auto buttonPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonPanel->SetSizer(buttonPanelSizer);
    mainSizer->Add(buttonPanel, common::sizers::ControlCenter);

    pOkButton = new wxButton(buttonPanel, wxID_OK, wxT("&OK"));
    pCancelButton = new wxButton(buttonPanel, wxID_CANCEL, wxT("&Cancel"));

    buttonPanelSizer->Add(pOkButton, common::sizers::ControlDefault);
    buttonPanelSizer->Add(pCancelButton, common::sizers::ControlDefault);
}

// clang-format off
void EmployerDialog::ConfigureEventBindings()
{
    pNameTextCtrl->Bind(
        wxEVT_TEXT,
        &EmployerDialog::OnNameChange,
        this
    );

    pOkButton->Bind(
        wxEVT_BUTTON,
        &EmployerDialog::OnOk,
        this,
        wxID_OK
    );

    pCancelButton->Bind(
        wxEVT_BUTTON,
        &EmployerDialog::OnCancel,
        this,
        wxID_CANCEL
    );

    if (bIsEdit) {
        pIsActiveCtrl->Bind(
            wxEVT_CHECKBOX,
            &EmployerDialog::OnIsActiveCheck,
            this
        );
    }
}
// clang-format on

void EmployerDialog::DataToControls()
{
    model::EmployerModel employer;
    try {
        employer = model::EmployerModel::GetById(mEmployerId);
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in GetById() - {0:d} : {1}", e.get_code(), e.what());
    }

    pNameTextCtrl->SetValue(employer.GetName());

    pDateTextCtrl->SetLabel(wxString::Format(EmployerDialog::DateLabel,
        employer.GetDateCreated().FormatISOCombined(),
        employer.GetDateModified().FormatISOCombined()));

    pIsActiveCtrl->SetValue(employer.IsActive());
}

void EmployerDialog::AttachRichTooltipToNameTextControl()
{
    const wxString errorHeader = wxT("Invalid input");
    const wxString errorMessage = wxT("A name is required \nand must be within %d to %d characters long");
    const wxString errorMessageFormat = wxString::Format(errorMessage, Constants::MinLength, Constants::MaxLength);

    wxRichToolTip tooltip(errorHeader, errorMessageFormat);
    tooltip.SetIcon(wxICON_WARNING);
    tooltip.ShowFor(pNameTextCtrl);
}

void EmployerDialog::OnNameChange(wxCommandEvent& event)
{
    wxString name = pNameTextCtrl->GetValue();
    mEmployer.SetName(name);
}

bool EmployerDialog::Validate()
{
    if (!mEmployer.IsNameValid()) {
        AttachRichTooltipToNameTextControl();
        return false;
    }

    return true;
}

bool EmployerDialog::AreControlsEmpty()
{
    return pNameTextCtrl->GetValue().empty();
}

void EmployerDialog::OnOk(wxCommandEvent& event)
{
    if (Validate()) {
        if (!bIsEdit) {
            model::EmployerModel::Create(mEmployer);
        }
        if (bIsEdit && pIsActiveCtrl->IsChecked()) {
            mEmployer.SetDateModified(wxDateTime::Now());
            model::EmployerModel::Update(mEmployer);
        }
        if (bIsEdit && !pIsActiveCtrl->IsChecked()) {
            mEmployer.SetDateModified(wxDateTime::Now());
            model::EmployerModel::Delete(mEmployer);
        }

        EndModal(wxID_OK);
    }
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
        pNameTextCtrl->Enable();
    } else {
        pNameTextCtrl->Disable();
    }
}
} // namespace app::dialog
