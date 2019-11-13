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

#include <wx/wx.h>
#include <wx/clrpicker.h>

#define FMT_HEADER_ONLY
#include <spdlog/spdlog.h>

#include "../models/categorymodel.h"

namespace app::dialog
{
class CategoryDialog final : public wxDialog
{
public:
    CategoryDialog() = default;
    explicit CategoryDialog(wxWindow* parent,
        std::shared_ptr<spdlog::logger> logger,
        int categoryId,
        const wxString& name = "categorydialogdlg");
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

    void AttachRichTooltipToNameTextControl();
    void AttachRichTooltipToProjectChoiceControl();

    void OnProjectChoiceSelection(wxCommandEvent& event);
    void OnNameChange(wxCommandEvent& event);
    void OnColorChange(wxColourPickerEvent& event);

    void OnOk(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnIsActiveCheck(wxCommandEvent& event);

    bool Validate();

    std::shared_ptr<spdlog::logger> pLogger;

    wxWindow* pParent;
    wxChoice* pProjectChoiceCtrl;
    wxTextCtrl* pNameTextCtrl;
    wxColourPickerCtrl* pColorPickerCtrl;
    wxCheckBox* pIsActiveCtrl;
    wxStaticText* pDateTextCtrl;
    wxButton* pOkButton;
    wxButton* pCancelButton;

    model::CategoryModel mCategory;
    int mCategoryId;
    bool bTouched;

    enum { IDC_PROJECTCHOICE = wxID_HIGHEST + 1, IDC_NAME, IDC_COLOR, IDC_ISACTIVE };

    static const wxString& DateLabel;
};
} // namespace app::dialog
