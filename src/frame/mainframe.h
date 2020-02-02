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

#include <sqlite_modern_cpp.h>

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>

#include <spdlog/spdlog.h>

#include "../config/configuration.h"
#include "../services/taskstateservice.h"
#include "../services/taskstorageservice.h"

namespace app::frm
{
class TaskBarIcon;

class MainFrame : public wxFrame
{
public:
    MainFrame();
    MainFrame(std::shared_ptr<cfg::Configuration> config,
        std::shared_ptr<spdlog::logger> logger,
        sqlite::database* database,
        const wxString& name = wxT("mainframe"));
    MainFrame(const MainFrame&) = delete;
    virtual ~MainFrame();

    MainFrame& operator=(const MainFrame&) = delete;

    bool CreateFrame();

    void ResetDatabaseHandleOnDatabaseRestore(sqlite::database* database);

private:
    wxDECLARE_EVENT_TABLE();

    bool Create();

    void CreateControls();
    void DataToControls();

    void OnAbout(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnNewEntryTask(wxCommandEvent& event);
    void OnNewTimedTask(wxCommandEvent& event);
    void OnNewEmployer(wxCommandEvent& event);
    void OnNewClient(wxCommandEvent& event);
    void OnNewProject(wxCommandEvent& event);
    void OnNewCategory(wxCommandEvent& event);
    void OnEditEmployer(wxCommandEvent& event);
    void OnEditClient(wxCommandEvent& event);
    void OnEditProject(wxCommandEvent& event);
    void OnEditCategory(wxCommandEvent& event);
    void OnTaskInserted(wxCommandEvent& event);
    void OnItemDoubleClick(wxListEvent& event);
    void OnItemRightClick(wxListEvent& event);
    void OnIconize(wxIconizeEvent& event);
    void OnPreferences(wxCommandEvent& event);
    void OnTaskStopwatch(wxCommandEvent& event);
    void OnDateChanged(wxDateEvent& event);
    void OnNewStopwatchTaskFromPausedStopwatchTask(wxCommandEvent& event);
    void OnCheckForUpdate(wxCommandEvent& event);
    void OnResize(wxSizeEvent& event);
    void OnRestoreDatabase(wxCommandEvent& event);
    void OnBackupDatabase(wxCommandEvent& event);

    void CalculateTotalTime(wxDateTime date = wxDateTime::Now());
    void RefreshItems(wxDateTime date = wxDateTime::Now());

    bool RunDatabaseBackup();

    std::shared_ptr<spdlog::logger> pLogger;
    std::shared_ptr<cfg::Configuration> pConfig;
    std::shared_ptr<services::TaskStateService> pTaskState;
    std::unique_ptr<services::TaskStorage> pTaskStorage;
    wxDatePickerCtrl* pDatePickerCtrl;
    wxStaticText* pTotalHoursText;
    wxListCtrl* pListCtrl;
    wxStatusBar* pStatusBar;
    TaskBarIcon* pTaskBarIcon;

    sqlite::database* pDatabase;

    bool bHasPendingTaskToResume;
    bool bHasInitialized;

    enum { IDC_GO_TO_DATE = wxID_HIGHEST + 1, IDC_HOURS_TEXT, IDC_LIST };
};
} // namespace app::frm
