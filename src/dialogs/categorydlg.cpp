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

#include "categorydlg.h"

#include <vector>

#include <sqlite_modern_cpp/errors.h>
#include <wx/richtooltip.h>
#include <wx/statline.h>

#include "../common/constants.h"
#include "../common/common.h"
#include "../common/ids.h"
#include "../common/util.h"

#include "../data/projectdata.h"

namespace app::dlg
{
const wxString& CategoryDialog::DateLabel = wxT("Created %s | Updated %s");

CategoryDialog::CategoryDialog(wxWindow* parent,
    std::shared_ptr<spdlog::logger> logger,
    int categoryId,
    const wxString& name)
    : pParent(parent)
    , pProjectChoiceCtrl(nullptr)
    , pNameTextCtrl(nullptr)
    , pColorPickerCtrl(nullptr)
    , pIsActiveCtrl(nullptr)
    , pDateTextCtrl(nullptr)
    , pOkButton(nullptr)
    , pCancelButton(nullptr)
    , mCategoryId(categoryId)
    , pCategory(std::make_unique<model::CategoryModel>(categoryId))
    , pLogger(logger)
{
    Create(parent,
        wxID_ANY,
        wxT("Edit Category"),
        wxDefaultPosition,
        wxSize(320, 320),
        wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU,
        name);
}

bool CategoryDialog::Create(wxWindow* parent,
    wxWindowID windowId,
    const wxString& title,
    const wxPoint& position,
    const wxSize& size,
    long style,
    const wxString& name)
{
    SetExtraStyle(GetExtraStyle() | wxWS_EX_BLOCK_EVENTS);
    bool created = wxDialog::Create(parent, windowId, title, position, size, style, name);
    if (created) {
        CreateControls();
        ConfigureEventBindings();
        FillControls();
        DataToControls();

        GetSizer()->Fit(this);
        GetSizer()->SetSizeHints(this);
        SetIcon(common::GetProgramIcon());
        Center();
    }

    return created;
}

// clang-format off
void CategoryDialog::ConfigureEventBindings()
{
    pIsActiveCtrl->Bind(
        wxEVT_CHECKBOX,
        &CategoryDialog::OnIsActiveCheck,
        this
    );

    pOkButton->Bind(
        wxEVT_BUTTON,
        &CategoryDialog::OnOk,
        this,
        wxID_OK
    );

    pCancelButton->Bind(
        wxEVT_BUTTON,
        &CategoryDialog::OnCancel,
        this,
        wxID_CANCEL
    );
}
// clang-format on

void CategoryDialog::CreateControls()
{
    /* Window Sizing */
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    /* Category Details Box */
    auto detailsBox = new wxStaticBox(this, wxID_ANY, wxT("Category Details"));
    auto detailsBoxSizer = new wxStaticBoxSizer(detailsBox, wxVERTICAL);
    mainSizer->Add(detailsBoxSizer, common::sizers::ControlExpandProp);

    auto categoryDetailsPanel = new wxPanel(this, wxID_STATIC);
    detailsBoxSizer->Add(categoryDetailsPanel, common::sizers::ControlExpand);

    auto flexGridSizer = new wxFlexGridSizer(0, 2, 0, 0);
    categoryDetailsPanel->SetSizer(flexGridSizer);

    /* ---Controls--- */
    /* Project Choice Control */
    auto projectChoiceText = new wxStaticText(categoryDetailsPanel, wxID_STATIC, wxT("Project"));
    flexGridSizer->Add(projectChoiceText, common::sizers::ControlCenterVertical);

    pProjectChoiceCtrl = new wxChoice(categoryDetailsPanel, IDC_PROJECTCHOICE, wxDefaultPosition, wxSize(150, -1));
    pProjectChoiceCtrl->AppendString(wxT("Select a project"));
    pProjectChoiceCtrl->SetSelection(0);
    pProjectChoiceCtrl->SetToolTip(wxT("Select a project to associate this category with"));
    flexGridSizer->Add(pProjectChoiceCtrl, common::sizers::ControlDefault);

    /* Category Name Text Control */
    auto nameText = new wxStaticText(categoryDetailsPanel, wxID_STATIC, wxT("Name"));
    flexGridSizer->Add(nameText, common::sizers::ControlCenterVertical);

    pNameTextCtrl = new wxTextCtrl(
        categoryDetailsPanel, IDC_NAME, wxGetEmptyString(), wxDefaultPosition, wxSize(150, -1), wxTE_LEFT);
    pNameTextCtrl->SetHint(wxT("Name for category"));
    pNameTextCtrl->SetToolTip(wxT("Enter a name for this category"));
    flexGridSizer->Add(pNameTextCtrl, common::sizers::ControlDefault);

    /* Color Picker Control */
    auto colorPickerFiller = new wxStaticText(categoryDetailsPanel, wxID_ANY, wxGetEmptyString());
    flexGridSizer->Add(colorPickerFiller, common::sizers::ControlDefault);

    pColorPickerCtrl = new wxColourPickerCtrl(categoryDetailsPanel, IDC_COLOR);
    pColorPickerCtrl->SetToolTip(wxT("Select a color to associate this category with"));
    flexGridSizer->Add(pColorPickerCtrl, common::sizers::ControlDefault);

    /* Is Active Checkbox Control */
    auto isActiveFiller = new wxStaticText(categoryDetailsPanel, wxID_STATIC, wxGetEmptyString());
    flexGridSizer->Add(isActiveFiller, common::sizers::ControlDefault);

    pIsActiveCtrl = new wxCheckBox(categoryDetailsPanel, IDC_ISACTIVE, wxT("Is Active"));
    flexGridSizer->Add(pIsActiveCtrl, common::sizers::ControlDefault);

    /* Date Created Text Control */
    pDateTextCtrl = new wxStaticText(this, wxID_STATIC, wxGetEmptyString());
    auto font = pDateTextCtrl->GetFont();
    font.SetPointSize(7);
    pDateTextCtrl->SetFont(font);
    detailsBoxSizer->Add(pDateTextCtrl, 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    /* Horizontal Line*/
    auto separationLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
    mainSizer->Add(separationLine, 0, wxEXPAND | wxALL, 1);

    /* Button Panel */
    auto buttonPanel = new wxPanel(this, wxID_STATIC);
    auto buttonPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonPanel->SetSizer(buttonPanelSizer);
    mainSizer->Add(buttonPanel, common::sizers::ControlCenter);

    pOkButton = new wxButton(buttonPanel, wxID_OK, wxT("OK"));
    pOkButton->SetDefault();
    pCancelButton = new wxButton(buttonPanel, wxID_CANCEL, wxT("Cancel"));

    buttonPanelSizer->Add(pOkButton, common::sizers::ControlDefault);
    buttonPanelSizer->Add(pCancelButton, common::sizers::ControlDefault);
}

void CategoryDialog::FillControls()
{
    std::vector<std::unique_ptr<model::ProjectModel>> projects;
    data::ProjectData mData;

    try {
        projects = mData.GetAll();
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in ProjectModel::GetAll() - {0:d} : {1}", e.get_code(), e.what());
    }

    for (const auto& project : projects) {
        pProjectChoiceCtrl->Append(project->GetDisplayName(), util::IntToVoidPointer(project->GetProjectId()));
    }
}

void CategoryDialog::DataToControls()
{
    std::unique_ptr<model::CategoryModel> category = nullptr;

    try {
        category = model::CategoryModel::GetById(mCategoryId);
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in get_category_by_id() - {0:d} : {1}", e.get_code(), e.what());
    }

    pProjectChoiceCtrl->SetStringSelection(category->GetProject()->GetDisplayName());

    pNameTextCtrl->SetValue(category->GetName());

    pColorPickerCtrl->SetColour(category->GetColor());

    pDateTextCtrl->SetLabel(wxString::Format(CategoryDialog::DateLabel,
        category->GetDateCreated().FormatISOCombined(),
        category->GetDateModified().FormatISOCombined()));

    pIsActiveCtrl->SetValue(category->IsActive());
}

void CategoryDialog::PostInitializeProcedure()
{
    pOkButton->Disable();
}

bool CategoryDialog::TransferDataAndValidate()
{
    wxString name = pNameTextCtrl->GetValue();
    if (name.empty() || name.length() < 2 || name.length() > 255) {
        common::validations::ForRequiredText(pNameTextCtrl, wxT("category name"));
        return false;
    }
    pCategory->SetName(name);

    int projectId = util::VoidPointerToInt(pProjectChoiceCtrl->GetClientData(pProjectChoiceCtrl->GetSelection()));
    if (projectId < 1) {
        common::validations::ForRequiredChoiceSelection(pProjectChoiceCtrl, wxT("project"));
        return false;
    }
    pCategory->SetProjectId(projectId);

    wxColor color = pColorPickerCtrl->GetColour();
    pCategory->SetColor(color);

    return true;
}

void CategoryDialog::OnOk(wxCommandEvent& event)
{
    if (TransferDataAndValidate()) {
        if (pIsActiveCtrl->IsChecked()) {
            try {
                model::CategoryModel::Update(std::move(pCategory));
            } catch (const sqlite::sqlite_exception& e) {
                pLogger->error("Error occured in category CategoryModel::Update - {0:d} : {1}", e.get_code(), e.what());
                EndModal(ids::ID_ERROR_OCCURED);
            }
        } else {
            try {
                model::CategoryModel::Delete(std::move(pCategory));
            } catch (const sqlite::sqlite_exception& e) {
                pLogger->error("Error occured in category CategoryModel::Delete - {0:d} : {1}", e.get_code(), e.what());
                EndModal(ids::ID_ERROR_OCCURED);
            }
        }

        EndModal(wxID_OK);
    }
}

void CategoryDialog::OnCancel(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
}

void CategoryDialog::OnIsActiveCheck(wxCommandEvent& event)
{
    if (event.IsChecked()) {
        pProjectChoiceCtrl->Enable();
        pNameTextCtrl->Enable();
        pColorPickerCtrl->Enable();
    } else {
        pProjectChoiceCtrl->Disable();
        pNameTextCtrl->Disable();
        pColorPickerCtrl->Disable();
    }
}

} // namespace app::dlg
