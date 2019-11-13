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

#pragma once

#include <memory>
#include <tuple>

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/clrpicker.h>
#define FMT_HEADER_ONLY
#include <spdlog/spdlog.h>

#include "../models/categorymodel.h"

namespace app::dialog
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
    void FillControls(model::CategoryModel category);
    void PostInitializeProcedure();

    void AppendListControlEntry(model::CategoryModel category);
    void UpdateListControlEntry(model::CategoryModel category);

    void AttachRichTooltipToNameControl();
    void AttachRichTooltipToProjectChoiceControl();

    void OnProjectChoiceSelection(wxCommandEvent& event);
    void OnNameChange(wxCommandEvent& event);
    void OnColorChange(wxColourPickerEvent& event);

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

    model::CategoryModel ExtractCategoryDataFromListIndex();
    model::CategoryModel ExtractCategoryDataFromListIndex(long index);

    bool HasPendingChanges();
    bool Validate();

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
    model::CategoryModel mModel;
    std::vector<model::CategoryModel> mCategories;
    bool bEditFromListCtrl;

    enum { IDC_PROJECTCHOICE = wxID_HIGHEST + 1, IDC_NAME, IDC_COLOR, IDC_ISACTIVE, IDC_LIST };
};
} // namespace app::dialog
