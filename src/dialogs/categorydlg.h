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

#include <wx/wx.h>
#include <wx/clrpicker.h>

#include <spdlog/spdlog.h>

#include "../models/categorymodel.h"
#include "../data/categorydata.h"

namespace app::dlg
{
class CategoryDialog final : public wxDialog
{
public:
    CategoryDialog() = default;
    explicit CategoryDialog(wxWindow* parent,
        std::shared_ptr<spdlog::logger> logger,
        int categoryId,
        const wxString& name = "categorydlg");
    virtual ~CategoryDialog() = default;

private:
    bool Create(wxWindow* parent,
        wxWindowID windowId,
        const wxString& title,
        const wxPoint& position,
        const wxSize& size,
        long style,
        const wxString& name);

    void CreateControls();
    void ConfigureEventBindings();
    void FillControls();
    void DataToControls();
    void PostInitializeProcedure();

    void OnOk(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnIsActiveCheck(wxCommandEvent& event);

    bool TransferDataAndValidate();

    std::shared_ptr<spdlog::logger> pLogger;

    wxWindow* pParent;
    wxChoice* pProjectChoiceCtrl;
    wxTextCtrl* pNameTextCtrl;
    wxColourPickerCtrl* pColorPickerCtrl;
    wxCheckBox* pIsActiveCtrl;
    wxStaticText* pDateTextCtrl;
    wxButton* pOkButton;
    wxButton* pCancelButton;

    std::unique_ptr<model::CategoryModel> pCategory;
    int mCategoryId;

    data::CategoryData mCategoryData;

    enum { IDC_PROJECTCHOICE = wxID_HIGHEST + 1, IDC_NAME, IDC_COLOR, IDC_ISACTIVE };
};
} // namespace app::dlg
