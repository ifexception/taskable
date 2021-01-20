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

#include <spdlog/spdlog.h>
#include <wx/wx.h>
#include <wx/datectrl.h>

namespace app::dlg
{
class ExportToCsvDialog final : public wxDialog
{
public:
    ExportToCsvDialog(wxWindow* parent,
        std::shared_ptr<spdlog::logger> logger,
        const wxString& name = "exporttocsvdlg");
    virtual ~ExportToCsvDialog() = default;

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
    void OnOpenDirectoryForExportLocation(wxCommandEvent& event);
    void OnExport(wxCommandEvent& event);
    void OnDelimiterChange(wxCommandEvent& event);

    void DateValidationProcedure();

    std::shared_ptr<spdlog::logger> pLogger;

    wxDatePickerCtrl* pStartDateCtrl;
    wxDatePickerCtrl* pEndDateCtrl;
    wxTextCtrl* pDelimiterTextCtrl;
    wxTextCtrl* pExportFilePathCtrl;
    wxButton* pBrowseExportPathButton;
    wxTextCtrl* pExportFileNameCtrl;
    wxStaticText* pFeedbackLabel;
    wxButton* pExportButton;
    wxButton* pOkButton;

    enum {
        IDC_STARTDATE = wxID_HIGHEST + 1,
        IDC_ENDDATE,
        IDC_DELIMITER,
        IDC_EXPORTPATH,
        IDC_EXPORTPATHBUTTON,
        IDC_EXPORTFILE,
        IDC_FEEDBACK,
        IDC_EXPORTBUTTON
    };
};
} // namespace app::dlg
