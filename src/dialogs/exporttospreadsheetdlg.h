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

#pragma once

#include <memory>
#include <string>

#include <spdlog/spdlog.h>
#include <wx/wx.h>
#include <wx/datectrl.h>
#include <wx/listctrl.h>

#include "../common/constants.h"

namespace app::dlg
{
struct ExportFormatOptions final {
    std::string DisplayValue;
    constants::ExportFormats Value;
};

class ExportToSpreadsheetDialog final : public wxDialog
{
public:
    ExportToSpreadsheetDialog() = delete;
    ExportToSpreadsheetDialog(wxWindow* parent,
        std::shared_ptr<spdlog::logger> logger,
        const wxString name = "exporttospreadsheetdlg");
    virtual ~ExportToSpreadsheetDialog() = default;

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

    void OnStartDateFocusLost(wxFocusEvent& event);
    void OnEndDateFocusLost(wxFocusEvent& event);
    void OnAddColumn(wxCommandEvent& event);
    void OnOpenDirectoryForExportLocation(wxCommandEvent& event);
    void OnExport(wxCommandEvent& event);
    void OnRemove(wxCommandEvent& event);
    void OnItemRightClick(wxListEvent& event);

    void DateValidationProcedure();

    std::shared_ptr<spdlog::logger> pLogger;

    wxDatePickerCtrl* pStartDateCtrl;
    wxDatePickerCtrl* pEndDateCtrl;
    wxChoice* pAddColumnsChoice;
    wxButton* pAddColumnButton;
    wxListCtrl* pColumnListCtrl;
    wxChoice* pExportFormatChoice;
    wxTextCtrl* pExportFilePathCtrl;
    wxButton* pBrowseExportPathButton;
    wxTextCtrl* pExportFileNameCtrl;
    wxStaticText* pFeedbackLabel;
    wxButton* pExportButton;
    wxButton* pOkButton;

    ExportFormatOptions exportFormatOptions[2];
    std::vector<std::string> mSelectedColumns;

    long mListCtrlIndex;

    enum {
        IDC_STARTDATE = wxID_HIGHEST + 1,
        IDC_ENDDATE,
        IDC_ADDCOLUMNS,
        IDC_ADDCOLUMNBUTTON,
        IDC_COLUMNLIST,
        IDC_EXPORTFORMATCHOICE,
        IDC_EXPORTPATH,
        IDC_EXPORTPATHBUTTON,
        IDC_EXPORTFILE,
        IDC_FEEDBACK,
        IDC_EXPORTBUTTON,
    };
};
} // namespace app::dlg
