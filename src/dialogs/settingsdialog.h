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

#include "../config/configuration.h"

namespace app::dialog
{
class SettingsDialog : public wxDialog
{
public:
    SettingsDialog() = default;
    explicit SettingsDialog(wxWindow* parent,
        std::shared_ptr<cfg::Configuration> config,
        const wxString& name = wxT("SettingsDialog"));
    virtual ~SettingsDialog() = default;

private:
    wxDECLARE_EVENT_TABLE();

    bool Create(wxWindow* parent,
        wxWindowID windowId,
        const wxString& title,
        const wxPoint& position,
        const wxSize& size,
        long style,
        const wxString& name);
    void CreateControls();
    void FillControls();

    void OnOk(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnBackupDatabaseCheck(wxCommandEvent& event);
    void OnShowInTrayCheck(wxCommandEvent& event);
    void OnMinimizeTimedTaskWindowCheck(wxCommandEvent& event);
    void OnOpenDirectory(wxCommandEvent& event);
    void OnTimeRoundingCheck(wxCommandEvent& event);

    wxCheckBox* pDialogOnExit;
    wxCheckBox* pStartWithWindows;

    wxCheckBox* pShowInTray;
    wxCheckBox* pMinimizeToTray;
    wxCheckBox* pCloseToTray;

    wxCheckBox* pBackupDatabase;
    wxTextCtrl* pBackupPath;
    wxButton* pBrowseBackupPath;

    wxCheckBox* pMinimizeTimedTaskWindow;
    wxChoice* pHideWindowTimeChoice;
    wxChoice* pNotificationTimeChoice;
    wxChoice* pPausedTaskReminderChoice;

    wxCheckBox* pTimeRounding;
    wxChoice* pRoundOffToChoice;

    std::shared_ptr<cfg::Configuration> pConfig;

    enum {
        IDC_DIALOG_EXIT = wxID_HIGHEST + 1,
        IDC_START_WITH_WINDOWS,
        IDC_SHOW_IN_TRAY,
        IDC_MINIMIZE_TO_TRAY,
        IDC_CLOSE_TO_TRAY,
        IDC_SHOW_BALLOON_NOTIFICATIONS,
        IDC_BACKUP_DATABASE,
        IDC_BACKUP_PATH,
        IDC_BACKUP_PATH_BUTTON,
        IDC_CLOSE_TIMED_TASK_WINDOW,
        IDC_HIDE_WINDOW_TIME_CHOICE,
        IDC_NOTIFICATION_TIME_CHOICE,
        IDC_PAUSED_TASK_REMINDER_CHOICE,
        IDC_ENABLE_TIME_ROUNDING,
        IDC_ROUND_OFF_TO
    };
};
} // namespace app::dialog
