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

#include <spdlog/spdlog.h>
#include <wx/wx.h>
#include <wx/listctrl.h>

namespace app::dlg
{
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

    std::shared_ptr<spdlog::logger> pLogger;

    wxChoice* pAddColumnsChoice;
    wxButton* pAddColumnButton;
    wxListCtrl* pColumnListCtrl;
    wxChoice* pExportFormatChoice;
    wxChoice* pExportFileFormat;
    wxTextCtrl* pExportFilePathCtrl;
    wxButton* pBrowseExportPathButton;
    wxTextCtrl* pExportFileNameCtrl;
    wxButton* pExportButton;
    wxButton* pOkButton;

    enum {
        IDC_ADDCOLUMNS = wxID_HIGHEST + 1,
        IDC_ADDCOLUMNBUTTON,
        IDC_COLUMNLIST,
        IDC_EXPORTFORMATCHOICE,
        IDC_EXPORTFILEFORMAT,
        IDC_EXPORTPATH,
        IDC_EXPORTPATHBUTTON,
        IDC_EXPORTFILE,
        IDC_EXPORTBUTTON,
    };
};
} // namespace app::dlg
