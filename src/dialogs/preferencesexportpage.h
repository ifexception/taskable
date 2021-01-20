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

#include <wx/wx.h>

namespace app::cfg
{
class Configuration;
}

namespace app::dlg
{
class ExportPage final : public wxPanel
{
public:
    ExportPage() = delete;
    ExportPage(wxWindow* parent, cfg::Configuration* config);
    virtual ~ExportPage() = default;

    void Apply();

private:
    void CreateControls();
    void ConfigureEventBindings();
    void FillControls();

    void OnOpenDirectoryForExportLocation(wxCommandEvent& event);

    cfg::Configuration* pConfig;

    wxWindow* pParent;

    wxTextCtrl* pDelimiterTextCtrl;
    wxTextCtrl* pExportFilePathCtrl;
    wxButton* pBrowseExportPathButton;

    enum {
        IDC_DELIMITER = wxID_HIGHEST + 1,
        IDC_EXPORTPATH,
        IDC_EXPORTPATHBUTTON,
    };
};
} // namespace app::dlg
