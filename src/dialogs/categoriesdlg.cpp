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

#include "categoriesdlg.h"

#include <sqlite_modern_cpp/errors.h>
#include <wx/richtooltip.h>
#include <wx/statline.h>

#include "../common/constants.h"
#include "../common/common.h"
#include "../common/ids.h"
#include "../common/util.h"

namespace app::dlg
{
CategoriesDialog::CategoriesDialog(wxWindow* parent, std::shared_ptr<spdlog::logger> logger, const wxString& name)
    : pLogger(logger)
    , pParent(parent)
    , pProjectChoiceCtrl(nullptr)
    , pNameTextCtrl(nullptr)
    , pColorPickerCtrl(nullptr)
    , pIsActiveCtrl(nullptr)
    , pDateCreatedTextCtrl(nullptr)
    , pDateUpdatedTextCtrl(nullptr)
    , pCategoryListCtrl(nullptr)
    , pAddButton(nullptr)
    , pRemoveButton(nullptr)
    , pRemoveAllButton(nullptr)
    , pClearButton(nullptr)
    , pOkButton(nullptr)
    , pCancelButton(nullptr)
    , mItemIndex(-1)
    , mItemIndexes()
    , pCategory(std::make_unique<model::CategoryModel>())
    , mCategories()
    , bEditFromListCtrl(false)
{
    Create(pParent,
        wxID_ANY,
        wxT("Add Categories"),
        wxDefaultPosition,
        wxSize(660, 330),
        wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU,
        name);
}

bool CategoriesDialog::Create(wxWindow* parent,
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
        PostInitializeProcedure();

        GetSizer()->Fit(this);
        GetSizer()->SetSizeHints(this);
        SetIcon(common::GetProgramIcon());
        Center();
    }

    return created;
}

void CategoriesDialog::PostInitializeProcedure()
{
    if (mCategories.size() == 0) {
        pRemoveButton->Disable();
        pRemoveAllButton->Disable();
    }
}

// clang-format off
void CategoriesDialog::ConfigureEventBindings()
{
    pClearButton->Bind(
        wxEVT_BUTTON,
        &CategoriesDialog::OnClear,
        this,
        wxID_CLEAR
    );

    pAddButton->Bind(
        wxEVT_BUTTON,
        &CategoriesDialog::OnAdd,
        this,
        wxID_ADD
    );

    pRemoveButton->Bind(
        wxEVT_BUTTON,
        &CategoriesDialog::OnRemove,
        this,
        wxID_REMOVE
    );

    pRemoveAllButton->Bind(
        wxEVT_BUTTON,
        &CategoriesDialog::OnRemoveAll,
        this,
        wxID_DELETE
    );

    pCategoryListCtrl->Bind(
        wxEVT_LIST_ITEM_CHECKED,
        &CategoriesDialog::OnItemChecked,
        this
    );

    pCategoryListCtrl->Bind(
        wxEVT_LIST_ITEM_UNCHECKED,
        &CategoriesDialog::OnItemUnchecked,
        this
    );

    pCategoryListCtrl->Bind(
        wxEVT_LIST_ITEM_RIGHT_CLICK,
        &CategoriesDialog::OnItemRightClick,
        this
    );

    Bind(
        wxEVT_MENU,
        &CategoriesDialog::OnEdit,
        this,
        wxID_EDIT
    );

    pCancelButton->Bind(
        wxEVT_BUTTON,
        &CategoriesDialog::OnCancel,
        this,
        wxID_CANCEL
    );

    pOkButton->Bind(
        wxEVT_BUTTON,
        &CategoriesDialog::OnOK,
        this,
        wxID_OK
    );
}
// clang-format on

void CategoriesDialog::CreateControls()
{
    /* Window Sizers */
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    auto mainPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(mainPanelSizer, common::sizers::ControlDefault);

    /* Sizer for left hand size controls */
    auto leftSizer = new wxBoxSizer(wxVERTICAL);
    mainPanelSizer->Add(leftSizer, 0);

    /* Sizer for center size controls */
    auto centerSizer = new wxBoxSizer(wxVERTICAL);
    mainPanelSizer->Add(centerSizer, 0);

    /* Sizer for right hand side controls */
    auto rightSizer = new wxBoxSizer(wxVERTICAL);
    mainPanelSizer->Add(rightSizer, 0);

    /* Sizer for bottom controls */
    auto mainBottomSizer = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(mainBottomSizer, wxSizerFlags().Border(wxALL, 5).Expand());

    auto bottomSizer = new wxBoxSizer(wxVERTICAL);
    mainBottomSizer->Add(bottomSizer, 1);

    /* Left Side */
    /* Category Details Box */
    auto detailsBox = new wxStaticBox(this, wxID_ANY, wxT("Category Details"));
    auto detailsBoxSizer = new wxStaticBoxSizer(detailsBox, wxVERTICAL);
    leftSizer->Add(detailsBoxSizer, common::sizers::ControlExpandProp);

    auto categoryDetailsPanel = new wxPanel(this, wxID_STATIC);
    detailsBoxSizer->Add(categoryDetailsPanel, common::sizers::ControlExpand);

    auto flexGridSizer = new wxFlexGridSizer(0, 2, 0, 0);
    categoryDetailsPanel->SetSizer(flexGridSizer);

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

    auto colorPickerFiller = new wxStaticText(categoryDetailsPanel, wxID_ANY, wxT(""));
    flexGridSizer->Add(colorPickerFiller, common::sizers::ControlDefault);

    pColorPickerCtrl = new wxColourPickerCtrl(categoryDetailsPanel, IDC_COLOR);
    pColorPickerCtrl->SetToolTip(wxT("Select a color to associate this category with"));
    flexGridSizer->Add(pColorPickerCtrl, common::sizers::ControlDefault);

    auto categoryDetailsButtonPanel = new wxPanel(this, wxID_STATIC);
    auto categoryDetailsButtonPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    categoryDetailsButtonPanel->SetSizer(categoryDetailsButtonPanelSizer);
    detailsBoxSizer->Add(categoryDetailsButtonPanel, common::sizers::ControlCenter);

    pClearButton = new wxButton(categoryDetailsButtonPanel, wxID_CLEAR, wxT("&Clear"));

    categoryDetailsButtonPanelSizer->Add(pClearButton, common::sizers::ControlDefault);

    /* Center */
    /* Button Controls */
    auto centerButtonPanel = new wxPanel(this, wxID_ANY);
    auto centerButtonPanelSizer = new wxBoxSizer(wxVERTICAL);
    centerButtonPanel->SetSizer(centerButtonPanelSizer);
    centerSizer->Add(centerButtonPanel, wxSizerFlags().Border(wxALL, 4));

    pAddButton = new wxButton(centerButtonPanel, wxID_ADD, wxT("Add >>"));
    centerButtonPanelSizer->Add(pAddButton, common::sizers::ControlDefault);

    pRemoveButton = new wxButton(centerButtonPanel, wxID_REMOVE, wxT("Remove"));
    centerButtonPanelSizer->Add(pRemoveButton, common::sizers::ControlDefault);

    pRemoveAllButton = new wxButton(centerButtonPanel, wxID_DELETE, wxT("Remove All"));
    centerButtonPanelSizer->Add(pRemoveAllButton, common::sizers::ControlDefault);

    /* Right Side */
    /* Category List Static Box Control */
    auto categoryListStaticBox = new wxStaticBox(this, wxID_ANY, wxT("Categories to Add"));
    auto categoryListStaticBoxSizer = new wxStaticBoxSizer(categoryListStaticBox, wxVERTICAL);
    rightSizer->Add(categoryListStaticBoxSizer, common::sizers::ControlExpandProp);

    /* Category List Panel */
    auto categoryListPanel = new wxPanel(this, wxID_ANY);
    categoryListStaticBoxSizer->Add(categoryListPanel, common::sizers::ControlExpand);

    /* Category List Panel Sizer */
    auto categoryListPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    categoryListPanel->SetSizer(categoryListPanelSizer);

    /* Category List Control */
    pCategoryListCtrl =
        new wxListCtrl(categoryListPanel, IDC_LIST, wxDefaultPosition, wxSize(250, 180), wxLC_REPORT | wxLC_HRULES);
    pCategoryListCtrl->EnableCheckBoxes();

    wxListItem projectColumn;
    projectColumn.SetId(0);
    projectColumn.SetText(wxT("Project"));
    projectColumn.SetWidth(100);
    pCategoryListCtrl->InsertColumn(0, projectColumn);

    wxListItem nameColumn;
    nameColumn.SetId(1);
    nameColumn.SetText(wxT("Name"));
    nameColumn.SetWidth(146);
    pCategoryListCtrl->InsertColumn(1, nameColumn);

    categoryListPanelSizer->Add(pCategoryListCtrl, 1, wxEXPAND | wxALL, 5);

    /* Bottom */
    /* Horizontal Line*/
    auto bottomSeparationLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(3, 3), wxLI_HORIZONTAL);
    bottomSizer->Add(bottomSeparationLine, wxSizerFlags().Expand());

    /* Button Panel */
    auto buttonPanelSizer = new wxBoxSizer(wxHORIZONTAL);

    auto buttonPanel = new wxPanel(this, wxID_STATIC);
    buttonPanel->SetSizer(buttonPanelSizer);

    bottomSizer->Add(buttonPanel, wxSizerFlags().Border(wxALL, 5).Center());

    pOkButton = new wxButton(buttonPanel, wxID_OK, wxT("OK"));
    buttonPanelSizer->Add(pOkButton, wxSizerFlags().Border(wxALL, 5));

    pCancelButton = new wxButton(buttonPanel, wxID_CANCEL, wxT("Cancel"));
    buttonPanelSizer->Add(pCancelButton, wxSizerFlags().Border(wxALL, 5));
}

void CategoriesDialog::FillControls()
{
    std::vector<std::unique_ptr<model::ProjectModel>> projects;

    try {
        projects = model::ProjectModel::GetAll();
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in ProjectModel::GetAll()() - {0:d} : {1}", e.get_code(), e.what());
    }

    for (const auto& project : projects) {
        pProjectChoiceCtrl->Append(project->GetDisplayName(), util::IntToVoidPointer(project->GetProjectId()));
    }
}

void CategoriesDialog::FillControls(std::unique_ptr<model::CategoryModel> category)
{
    pProjectChoiceCtrl->SetStringSelection(category->GetProject()->GetDisplayName());

    pNameTextCtrl->ChangeValue(category->GetName());

    pColorPickerCtrl->SetColour(category->GetColor());
}

void CategoriesDialog::AppendListControlEntry(model::CategoryModel* category)
{
    int listIndex = 0;
    int columnIndex = 0;

    listIndex = pCategoryListCtrl->InsertItem(columnIndex++, category->GetProject()->GetDisplayName());
    pCategoryListCtrl->SetItem(listIndex, columnIndex++, category->GetName());
    pCategoryListCtrl->SetItemBackgroundColour(listIndex, category->GetColor());
    pCategoryListCtrl->SetItemPtrData(listIndex, category->GetProjectId());
}

void CategoriesDialog::UpdateListControlEntry(model::CategoryModel* category)
{
    int columnIndex = 0;

    pCategoryListCtrl->SetItem(mItemIndex, columnIndex++, category->GetProject()->GetDisplayName());
    pCategoryListCtrl->SetItem(mItemIndex, columnIndex++, category->GetName());
    pCategoryListCtrl->SetItemBackgroundColour(mItemIndex, category->GetColor());
    pCategoryListCtrl->SetItemPtrData(mItemIndex, category->GetProjectId());

    mItemIndex = -1;
}

void CategoriesDialog::OnClear(wxCommandEvent& event)
{
    ResetControlValues();
}

void CategoriesDialog::OnAdd(wxCommandEvent& event)
{
    if (TransferDataAndValidate()) {
        if (!bEditFromListCtrl) {
            AppendListControlEntry(pCategory.get());
            mCategories.push_back(std::move(pCategory));
            pRemoveAllButton->Enable();

        } else {
            auto iterator = std::find_if(
                mCategories.begin(), mCategories.end(), [&](std::unique_ptr<model::CategoryModel>& category) {
                    return category->GetName() == pCategory->GetName();
                });
            if (iterator != mCategories.end()) {
                int index = std::distance(mCategories.begin(), iterator);
                UpdateListControlEntry(pCategory.get());
                mCategories.at(index) = std::move(pCategory);
            }
        }

        pCategory = std::make_unique<model::CategoryModel>();
        ResetControlValues();
    }
}

void CategoriesDialog::OnEdit(wxCommandEvent& event)
{
    bEditFromListCtrl = true;

    auto category = ExtractCategoryDataFromListIndex();

    FillControls(std::move(category));
}

void CategoriesDialog::OnRemove(wxCommandEvent& event)
{
    for (long index : mItemIndexes) {
        auto categoryAtIndex = ExtractCategoryDataFromListIndex(index);

        mCategories.erase(
            std::remove_if(mCategories.begin(), mCategories.end(), [&](std::unique_ptr<model::CategoryModel>& category) {
                return category->GetName() == categoryAtIndex->GetName();
            }));

        pCategoryListCtrl->DeleteItem(index);
    }

    pAddButton->Enable();
    pRemoveButton->Disable();
}

void CategoriesDialog::OnRemoveAll(wxCommandEvent& event)
{
    mCategories.clear();
    pCategoryListCtrl->DeleteAllItems();
    pRemoveAllButton->Disable();
}

void CategoriesDialog::OnOK(wxCommandEvent& event)
{
    for (auto& category : mCategories) {
        try {
            model::CategoryModel::Create(std::move(category));
        } catch (const sqlite::sqlite_exception& e) {
            pLogger->error("Error occured in category CategoryModel::Create() - {0:d} : {1}", e.get_code(), e.what());
            EndModal(ids::ID_ERROR_OCCURED);
        }
    }

    EndModal(wxID_OK);
}

void CategoriesDialog::OnCancel(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
}

void CategoriesDialog::OnItemChecked(wxListEvent& event)
{
    long index = event.GetIndex();
    mItemIndexes.push_back(index);

    if (mItemIndexes.size() >= 1) {
        pAddButton->Disable();

        pRemoveButton->Enable();
    }
}

void CategoriesDialog::OnItemUnchecked(wxListEvent& event)
{
    long index = event.GetIndex();
    mItemIndexes.erase(std::remove(mItemIndexes.begin(), mItemIndexes.end(), index), mItemIndexes.end());

    if (mItemIndexes.size() == 0) {
        pAddButton->Enable();

        pRemoveButton->Disable();
    }
}

void CategoriesDialog::OnItemRightClick(wxListEvent& event)
{
    mItemIndex = event.GetIndex();

    wxMenu menu;

    menu.Append(wxID_EDIT, wxT("&Edit"));

    PopupMenu(&menu);
}

std::unique_ptr<model::CategoryModel> CategoriesDialog::ExtractCategoryDataFromListIndex()
{
    assert(mItemIndex != -1);

    // declare variables
    unsigned int color;
    wxString projectName;
    int projectId;
    wxString categoryName;

    // get category color
    auto itemColor = pCategoryListCtrl->GetItemBackgroundColour(mItemIndex);
    color = itemColor.GetRGB();

    wxListItem item;
    item.m_itemId = mItemIndex;
    item.m_col = 0;
    item.m_mask = wxLIST_MASK_TEXT;
    pCategoryListCtrl->GetItem(item);

    // get associated category project ID
    projectName = item.GetText();
    projectId = item.GetData();

    item.m_col = 1;
    pCategoryListCtrl->GetItem(item);

    // get category name
    categoryName = item.GetText();

    auto category = std::make_unique<model::CategoryModel>(categoryName, color, projectId);
    category->GetProject()->SetProjectId(projectId);
    category->GetProject()->SetDisplayName(projectName);
    return std::move(category);
}

std::unique_ptr<model::CategoryModel> CategoriesDialog::ExtractCategoryDataFromListIndex(long index)
{
    // declare variables
    unsigned int color;
    wxString projectName;
    int projectId;
    wxString categoryName;

    // get category color
    auto itemColor = pCategoryListCtrl->GetItemBackgroundColour(index);
    color = itemColor.GetRGB();

    wxListItem item;
    item.m_itemId = index;
    item.m_col = 0;
    item.m_mask = wxLIST_MASK_TEXT;
    pCategoryListCtrl->GetItem(item);

    // get associated category project name and project ID
    projectName = item.GetText();
    projectId = item.GetData();

    item.m_col = 1;
    pCategoryListCtrl->GetItem(item);

    // get category name
    categoryName = item.GetText();

    auto category = std::make_unique<model::CategoryModel>(categoryName, color, projectId);
    category->GetProject()->SetProjectId(projectId);
    category->GetProject()->SetDisplayName(projectName);
    return std::move(category);
}

bool CategoriesDialog::HasPendingChanges()
{
    return mCategories.size() > 0;
}

bool CategoriesDialog::TransferDataAndValidate()
{
    wxString name = pNameTextCtrl->GetValue();
    if (name.empty() || name.length() < 2 || name.length() > 255) {
        common::validations::ForRequiredText(pNameTextCtrl, wxT("category name"));
        return false;
    }
    pCategory->SetName(name);

    int projectId = util::VoidPointerToInt(pProjectChoiceCtrl->GetClientData(pProjectChoiceCtrl->GetSelection()));
    wxString displayName = pProjectChoiceCtrl->GetStringSelection();
    if (projectId < 1) {
        common::validations::ForRequiredChoiceSelection(pProjectChoiceCtrl, wxT("project"));
        return false;
    }
    pCategory->SetProjectId(projectId);
    pCategory->GetProject()->SetProjectId(projectId);
    pCategory->GetProject()->SetDisplayName(displayName);

    wxColor color = pColorPickerCtrl->GetColour();
    pCategory->SetColor(color);

    return true;
}

void CategoriesDialog::ResetControlValues()
{
    pProjectChoiceCtrl->SetSelection(0);
    pNameTextCtrl->ChangeValue(wxGetEmptyString());
    pColorPickerCtrl->SetColour(*wxBLACK);
}
} // namespace app::dlg
