// Productivity tool to help you track the time you spend on tasks
// Copyright (C) 2020  Szymon Welgus
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

#include "employerdlg.h"

#include <sqlite_modern_cpp/errors.h>
#include <wx/richtooltip.h>
#include <wx/statline.h>

#include "../common/constants.h"
#include "../common/common.h"
#include "../common/ids.h"
#include "../common/util.h"

namespace app::dlg
{
EmployerDialog::EmployerDialog(wxWindow* parent, std::shared_ptr<spdlog::logger> logger, const wxString& name)
    : pLogger(logger)
    , bIsEdit(false)
    , mEmployerId(-1)
    , pEmployer(std::make_unique<model::EmployerModel>())
    , mData()
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
    , mData()
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

    wxTextValidator nameValidator(wxFILTER_ALPHANUMERIC | wxFILTER_INCLUDE_CHAR_LIST);
    wxArrayString allowedChars;
    allowedChars.Add(wxT(" "));
    nameValidator.SetIncludes(allowedChars);

    pNameTextCtrl = new wxTextCtrl(employerDetailsPanel,
        IDC_EMPLOYERTEXT,
        wxGetEmptyString(),
        wxDefaultPosition,
        wxSize(150, -1),
        wxTE_LEFT,
        nameValidator);
    pNameTextCtrl->SetHint(wxT("Employer name"));
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
    pOkButton->SetDefault();
    pCancelButton = new wxButton(buttonPanel, wxID_CANCEL, wxT("&Cancel"));

    buttonPanelSizer->Add(pOkButton, common::sizers::ControlDefault);
    buttonPanelSizer->Add(pCancelButton, common::sizers::ControlDefault);
}

// clang-format off
void EmployerDialog::ConfigureEventBindings()
{
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
    std::unique_ptr<model::EmployerModel> employer;
    try {
        employer = mData.GetById(mEmployerId);
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in EmployerModel::GetById() - {0:d} : {1}", e.get_code(), e.what());
        return;
    }

    pNameTextCtrl->ChangeValue(employer->GetName());

    pDateTextCtrl->SetLabel(wxString::Format(constants::DateLabel,
        util::ToFriendlyDateTimeString(employer->GetDateCreated()),
        util::ToFriendlyDateTimeString(employer->GetDateModified())));

    pIsActiveCtrl->SetValue(employer->IsActive());
}

void EmployerDialog::OnOk(wxCommandEvent& event)
{
    if (TransferDataAndValidate()) {
        if (!bIsEdit) {
            try {
                mData.Create(std::move(pEmployer));
            } catch (const sqlite::sqlite_exception& e) {
                pLogger->error(
                    "Error occured in category EmployerData::Create() - {0:d} : {1}", e.get_code(), e.what());
                EndModal(ids::ID_ERROR_OCCURED);
            }
        }
        if (bIsEdit && pIsActiveCtrl->IsChecked()) {
            try {
                mData.Update(std::move(pEmployer));
            } catch (const sqlite::sqlite_exception& e) {
                pLogger->error(
                    "Error occured in category EmployerData::Update() - {0:d} : {1}", e.get_code(), e.what());
                EndModal(ids::ID_ERROR_OCCURED);
            }
        }
        if (bIsEdit && !pIsActiveCtrl->IsChecked()) {
            try {
                mData.Delete(mEmployerId);
            } catch (const sqlite::sqlite_exception& e) {
                pLogger->error(
                    "Error occured in category EmployerData::Delete() - {0:d} : {1}", e.get_code(), e.what());
                EndModal(ids::ID_ERROR_OCCURED);
            }
        }

        EndModal(wxID_OK);
    }
}

void EmployerDialog::OnCancel(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
}

void EmployerDialog::OnIsActiveCheck(wxCommandEvent& event)
{
    if (event.IsChecked()) {
        pNameTextCtrl->Enable();
    } else {
        pNameTextCtrl->Disable();
    }
}

bool EmployerDialog::TransferDataAndValidate()
{
    wxString name = pNameTextCtrl->GetValue();
    if (name.empty() || name.length() < 2 || name.length() > 255) {
        common::validations::ForRequiredText(pNameTextCtrl, wxT("employer name"));
        return false;
    }
    pEmployer->SetName(name);

    return true;
}
} // namespace app::dlg
