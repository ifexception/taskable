// Productivity tool to help you track the time you spend on tasks
// Copyright (C) 2023  Szymon Welgus
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

#pragma once

#include <memory>
#include <tuple>

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/clrpicker.h>

#include <spdlog/spdlog.h>

#include "../models/categorymodel.h"
#include "../data/categorydata.h"

namespace app::dlg
{
class CategoriesDialog final : public wxDialog
{
public:
    CategoriesDialog() = delete;
    CategoriesDialog(wxWindow* parent,
        std::shared_ptr<spdlog::logger> logger,
        const wxString& name = wxT("categoriesdlg"));
    virtual ~CategoriesDialog() = default;

private:
    bool Create(wxWindow* parent,
        wxWindowID windowId,
        const wxString& title,
        const wxPoint& position,
        const wxSize& size,
        long style,
        const wxString& name);

    void ConfigureEventBindings();
    void CreateControls();
    void FillControls();
    void FillControls(std::unique_ptr<model::CategoryModel> category);
    void PostInitializeProcedure();

    void AppendListControlEntry(model::CategoryModel* category);
    void UpdateListControlEntry(model::CategoryModel* category);

    void OnClear(wxCommandEvent& event);
    void OnAdd(wxCommandEvent& event);
    void OnEdit(wxCommandEvent& event);
    void OnRemove(wxCommandEvent& event);
    void OnRemoveAll(wxCommandEvent& event);
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);

    void OnItemChecked(wxListEvent& event);
    void OnItemUnchecked(wxListEvent& event);
    void OnItemRightClick(wxListEvent& event);

    std::unique_ptr<model::CategoryModel> ExtractCategoryDataFromListIndex();
    std::unique_ptr<model::CategoryModel> ExtractCategoryDataFromListIndex(long index);

    bool HasPendingChanges();
    bool TransferDataAndValidate();

    void ResetControlValues();

    std::shared_ptr<spdlog::logger> pLogger;

    wxWindow* pParent;
    wxChoice* pProjectChoiceCtrl;
    wxTextCtrl* pNameTextCtrl;
    wxColourPickerCtrl* pColorPickerCtrl;
    wxCheckBox* pIsActiveCtrl;
    wxStaticText* pDateCreatedTextCtrl;
    wxStaticText* pDateUpdatedTextCtrl;
    wxListCtrl* pCategoryListCtrl;
    wxButton* pAddButton;
    wxButton* pRemoveButton;
    wxButton* pRemoveAllButton;
    wxButton* pClearButton;
    wxButton* pOkButton;
    wxButton* pCancelButton;

    long mItemIndex;
    std::vector<long> mItemIndexes;
    std::unique_ptr<model::CategoryModel> pCategory;
    std::vector<std::unique_ptr<model::CategoryModel>> mCategories;
    bool bEditFromListCtrl;

    data::CategoryData mCategoryData;

    enum { IDC_PROJECTCHOICE = wxID_HIGHEST + 1, IDC_NAME, IDC_COLOR, IDC_ISACTIVE, IDC_LIST };
};
} // namespace app::dlg
