// Productivity tool to help you track the time you spend on tasks
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

#include <sqlite_modern_cpp.h>

#include <wx/wx.h>
#include <wx/bmpbuttn.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/infobar.h>
#include <wx/listctrl.h>
#include <wx/timer.h>

#include <spdlog/spdlog.h>

#include "../config/configurationprovider.h"
#include "../services/taskstateservice.h"
#include "../services/taskstorageservice.h"
#include "feedbackpopup.h"

namespace app::frm
{
class TaskBarIcon;

class MainFrame : public wxFrame
{
public:
    MainFrame(std::shared_ptr<spdlog::logger> logger,
        const wxString& name = wxT("mainfrm"));
    MainFrame(const MainFrame&) = delete;
    virtual ~MainFrame();

    MainFrame& operator=(const MainFrame&) = delete;

    bool CreateFrame();

private:
    wxDECLARE_EVENT_TABLE();

    bool Create();

    void CreateControls();
    void DataToControls();

    /* General Event Handlers */
    void OnClose(wxCloseEvent& event);
    void OnIconize(wxIconizeEvent& event);
    void OnResize(wxSizeEvent& event);
    void OnDismissInfoBar(wxTimerEvent& event);

    /* Main Menu Event Handlers */
    void OnAbout(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnNewEntryTask(wxCommandEvent& event);
    void OnNewTimedTask(wxCommandEvent& event);
    void OnNewEmployer(wxCommandEvent& event);
    void OnNewClient(wxCommandEvent& event);
    void OnNewProject(wxCommandEvent& event);
    void OnNewCategory(wxCommandEvent& event);
    void OnWeeklyView(wxCommandEvent& event);
    //void OnMeetingsView(wxCommandEvent& event);
    void OnEditEmployer(wxCommandEvent& event);
    void OnEditClient(wxCommandEvent& event);
    void OnEditProject(wxCommandEvent& event);
    void OnEditCategory(wxCommandEvent& event);
    void OnPreferences(wxCommandEvent& event);
    void OnTaskStopwatch(wxCommandEvent& event);
    void OnCheckForUpdate(wxCommandEvent& event);
    void OnRestoreDatabase(wxCommandEvent& event);
    void OnBackupDatabase(wxCommandEvent& event);
    void OnReturnToCurrentDate(wxCommandEvent& event);
    void OnExportToCsv(wxCommandEvent& event);

    /* Frame Controls Event Handlers */
    void OnPrevDay(wxCommandEvent& event);
    void OnDateChanged(wxDateEvent& event);
    void OnNextDay(wxCommandEvent& event);
    void OnFeedback(wxCommandEvent& event);
    void OnKeyDown(wxKeyEvent& event);

    /* ListCtrl Control Event Handlers */
    void OnItemDoubleClick(wxListEvent& event);
    void OnItemRightClick(wxListEvent& event);
    void OnPopupMenuCopyToClipboard(wxCommandEvent& event);
    void OnPopupMenuEdit(wxCommandEvent& event);
    void OnPopupMenuDelete(wxCommandEvent& event);
    void OnColumnBeginDrag(wxListEvent& event);

    /* Uncategorized Event Handlers */
    void OnTaskInserted(wxCommandEvent& event);
    void OnTaskUpdated(wxCommandEvent& event);
    void OnTaskDeleted(wxCommandEvent& event);
    void OnNewStopwatchTaskFromPausedStopwatchTask(wxCommandEvent& event);

    void CalculateTotalTime(wxDateTime date = wxDateTime::Now());
    void FillListControl(wxDateTime date = wxDateTime::Now());

    bool RunDatabaseBackup();

    void ShowInfoBarMessage(int modalRetCode);

    void DateChangedProcedure(wxDateTime dateTime);
    void CopyToClipboardProcedure(long itemIndex);

    std::shared_ptr<spdlog::logger> pLogger;
    std::shared_ptr<services::TaskStateService> pTaskState;
    std::unique_ptr<services::TaskStorage> pTaskStorage;

    std::unique_ptr<wxTimer> pDismissInfoBarTimer;

    wxButton* pPrevDayBtn;
    wxDatePickerCtrl* pDatePickerCtrl;
    wxButton* pNextDayBtn;
    wxStaticText* pTotalHoursText;
    wxListCtrl* pListCtrl;
    wxStatusBar* pStatusBar;
    wxInfoBar* pInfoBar;
    TaskBarIcon* pTaskBarIcon;
    wxBitmapButton* pFeedbackButton;
    FeedbackPopupWindow* pFeedbackPopupWindow;

    bool bHasPendingTaskToResume;
    long mItemIndex;
    int mSelectedTaskItemId;

    enum {
        IDC_PREV_DAY = wxID_HIGHEST + 1,
        IDC_GO_TO_DATE,
        IDC_NEXT_DAY,
        IDC_HOURS_TEXT,
        IDC_LIST,
        IDC_FEEDBACK,
        IDC_DISMISS_INFOBAR_TIMER
    };
};
} // namespace app::frm
