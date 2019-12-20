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

namespace app::cfg
{
class Configuration;
}

namespace app::dialog
{
class DatabasePage final : public wxPanel
{
public:
    DatabasePage() = delete;
    DatabasePage(wxWindow* parent, std::shared_ptr<cfg::Configuration> config);
    virtual ~DatabasePage() = default;

    void Apply();

private:
    void CreateControls();
    void ConfigureEventBindings();
    void FillControls();

    void OnBackupDatabaseCheck(wxCommandEvent& event);
    void OnOpenDirectory(wxCommandEvent& event);

    std::shared_ptr<cfg::Configuration> pConfig;

    wxWindow* pParent;

    wxCheckBox* pBackupDatabaseCtrl;
    wxTextCtrl* pBackupPathTextCtrl;
    wxButton* pBrowseBackupPathButton;

    enum {
        IDC_BACKUP_DATABASE = wxID_HIGHEST + 1,
        IDC_BACKUP_PATH,
        IDC_BACKUP_PATH_BUTTON,
    };
};
} // namespace app::dialog
