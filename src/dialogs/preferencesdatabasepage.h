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

namespace app::cfg
{
class Configuration;
}

namespace app::dlg
{
class DatabasePage final : public wxPanel
{
public:
    DatabasePage() = delete;
    DatabasePage(wxWindow* parent, cfg::Configuration* config);
    virtual ~DatabasePage() = default;

    void Apply();

private:
    void CreateControls();
    void ConfigureEventBindings();
    void FillControls();

    void OnOpenDirectoryForDatabaseLocation(wxCommandEvent& event);
    void OnBackupDatabaseCheck(wxCommandEvent& event);
    void OnOpenDirectoryForBackupLocation(wxCommandEvent& event);

    cfg::Configuration* pConfig;

    wxWindow* pParent;

    wxTextCtrl* pDatabasePathTextCtrl;
    wxButton* pBrowseDatabasePathButton;
    wxCheckBox* pBackupDatabaseCtrl;
    wxTextCtrl* pBackupPathTextCtrl;
    wxButton* pBrowseBackupPathButton;
    wxTextCtrl* pDeleteBackupsAfterCtrl;

    enum {
        IDC_DATABASE_PATH = wxID_HIGHEST + 1,
        IDC_DATABASE_PATH_BUTTON,
        IDC_BACKUP_DATABASE,
        IDC_BACKUP_PATH,
        IDC_BACKUP_PATH_BUTTON,
        IDC_DELETE_BACKUPS_AFTER
    };
};
} // namespace app::dlg
