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
#define FMT_HEADER_ONLY
#include <spdlog/spdlog.h>

#include "../config/configuration.h"
#include "../services/models/task_item.h"

class wxDateEvent;
class wxDatePickerCtrl;
class wxTimePickerCtrl;

wxDECLARE_EVENT(TASK_INSERTED, wxCommandEvent);

namespace app::dialog
{
enum class TaskItemType : int {
    EntryTask = 1,
    TimedTask = 2,
};

struct TimeValidators {
    bool Validate(wxDateTime startTime, wxDateTime endTime);
};

class TaskItemDialog : public wxDialog
{
public:
    TaskItemDialog() = delete;
    explicit TaskItemDialog(wxWindow* parent,
        std::shared_ptr<spdlog::logger> logger,
        std::shared_ptr<cfg::Configuration> config,
        TaskItemType taskItemType,
        wxDateTime dateTimeContext = wxDateTime::Now(),
        const wxString& name = wxT("taskitemdlg"));
    explicit TaskItemDialog(wxWindow* parent,
        std::shared_ptr<spdlog::logger> logger,
        std::shared_ptr<cfg::Configuration> config,
        TaskItemType editTaskItemType,
        bool edit,
        int taskId,
        wxDateTime dateTimeContext = wxDateTime::Now(),
        const wxString& name = wxT("taskitemdlg"));
    virtual ~TaskItemDialog() = default;

    void SetDurationFromStopwatchTask(wxTimeSpan duration);
    void SetTimesFromStopwatchTask(wxDateTime startTime, wxDateTime endTime);

private:
    wxDECLARE_EVENT_TABLE();

    bool Create(wxWindow* parent,
        wxWindowID windowId,
        const wxString& title,
        const wxPoint& point,
        const wxSize& size,
        long style,
        const wxString& name);

    void CreateControls();
    void FillControls();
    void DataToControls();

    int GetTaskId();
    void CalculateTimeDiff(wxDateTime start, wxDateTime end);

    bool PostValidate();
    bool AreControlsEmpty();

    void OnDateContextChange(wxDateEvent& event);
    void OnProjectChoice(wxCommandEvent& event);
    void OnStartTimeChange(wxDateEvent& event);
    void OnEndTimeChange(wxDateEvent& event);
    void OnIsActiveCheck(wxCommandEvent& event);
    void OnDurationTimeChange(wxDateEvent& event);
    void OnDescriptionControlFocusLost(wxFocusEvent& event);
    void OnOk(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);

    void FillCategoryControl(int projectId);

    void GenerateTaskSavedEvent();
    void ProcessTaskItem(const models::task_item& taskItem);

    std::shared_ptr<spdlog::logger> pLogger;
    std::shared_ptr<cfg::Configuration> pConfig;

    wxWindow* pParent;
    wxStaticText* pTaskContextTextCtrl;
    wxDatePickerCtrl* pDateContextCtrl;
    wxChoice* pProjectChoiceCtrl;
    wxTimePickerCtrl* pStartTimeCtrl;
    wxTimePickerCtrl* pEndTimeCtrl;
    wxStaticText* pDurationCtrl;
    wxTimePickerCtrl* pDurationTimeCtrl;
    wxCheckBox* pBillableCtrl;
    wxTextCtrl* pDescriptionCtrl;
    wxChoice* pCategoryChoiceCtrl;
    wxStaticText* pDateCreatedTextCtrl;
    wxStaticText* pDateUpdatedTextCtrl;
    wxCheckBox* pIsActiveCtrl;

    TimeValidators mValidator;
    TaskItemType mType;
    int mTaskItemId;
    bool bIsEdit;
    bool isFormValid;
    wxDateTime mDateContext;
    wxTimeSpan mDurationSpan;

    int mProjectId;
    wxDateTime mStartTime;
    wxDateTime mEndTime;
    wxDateTime mDurationTime;
    wxString mDurationText;
    bool bBillable;
    wxString mDescriptionText;
    int mCategoryId;

    enum {
        IDC_TASKCONTEXTINFO = wxID_HIGHEST + 1,
        IDC_DATECONTEXT,
        IDC_PROJECTCHOICE,
        IDC_STARTTIME,
        IDC_ENDTIME,
        IDC_DURATION,
        IDC_DURATIONTIME,
        IDC_CATEGORYCHOICE,
        IDC_DESCRIPTION,
        IDC_BILLABLE,
        IDC_DATECREATED,
        IDC_DATEUPDATED,
        IDC_ISACTIVE
    };
};
} // namespace app::dialog
