//  TimesheetsTracker is a desktop that aids you in tracking your timesheets
//  and seeing what work you have done.
//
//  Copyright(C) <2018> <Szymon Welgus>
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

class wxTimePickerCtrl;
class wxDateEvent;

wxDECLARE_EVENT(TASK_INSERTED, wxCommandEvent);

namespace app::dialog
{
class TaskItemDialog : public wxDialog
{
public:
    TaskItemDialog() = delete;
    explicit TaskItemDialog(wxWindow* parent,
        std::shared_ptr<spdlog::logger> logger,
        bool isEdit = false,
        int taskDetailId = 0,
        const wxString& name = wxT("TaskItemDialog"));
    explicit TaskItemDialog(wxWindow* parent,
        std::shared_ptr<spdlog::logger> logger,
        wxTimeSpan duration,
        const wxString& name = wxT("TaskItemDialog"));
    explicit TaskItemDialog(wxWindow* parent,
        std::shared_ptr<spdlog::logger> logger,
        wxDateTime startTime,
        wxDateTime endTime,
        const wxString& name = wxT("TaskItemDialog"));
    virtual ~TaskItemDialog() = default;

private:
    wxDECLARE_EVENT_TABLE();

    bool Create(wxWindow* parent,
        wxWindowID windowId,
        const wxString& title,
        const wxPoint& point,
        const wxSize& size,
        long style,
        const wxString& name);

    bool CreateWithParam(wxWindow* parent,
        wxWindowID windowId,
        const wxString& title,
        const wxPoint& point,
        const wxSize& size,
        long style,
        const wxString& name);
    bool CreateWithParams(wxWindow* parent,
        wxWindowID windowId,
        const wxString& title,
        const wxPoint& point,
        const wxSize& size,
        long style,
        const wxString& name);

    void CreateControls();
    void FillControls();
    void DataToControls();
    void SetValuesToControls();
    void SetValueToControl();

    int GetTaskId();

    bool Validate();
    bool AreControlsEmpty();

    void OnProjectChoice(wxCommandEvent& event);
    void OnStartTimeChange(wxDateEvent& event);
    void OnEndTimeChange(wxDateEvent& event);
    void OnIsActiveCheck(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnTaskSaved();

    void CalculateTimeDiff(wxDateTime start, wxDateTime end);
    void FillCategoryControl(int projectId);

    wxString mTaskDate;
    bool bIsEdit;
    int mTaskItemId;

    std::shared_ptr<spdlog::logger> pLogger;
    wxWindow* pParent;

    wxChoice* pProjectChoiceCtrl;
    wxTimePickerCtrl* pStartTimeCtrl;
    wxTimePickerCtrl* pEndTimeCtrl;
    wxStaticText* pDurationCtrl;
    wxTextCtrl* pDescriptionCtrl;
    wxChoice* pCategoryChoiceCtrl;
    wxStaticText* pDateCreatedTextCtrl;
    wxStaticText* pDateUpdatedTextCtrl;
    wxCheckBox* pIsActiveCtrl;

    wxTimeSpan mDuration;

    int mProjectId;
    wxDateTime mStartTime;
    wxDateTime mEndTime;
    wxString mDurationText;
    int mCategoryId;
    wxString mDescriptionText;

    bool bIsPausableTask;

    enum {
        IDC_PROJECTCHOICE,
        IDC_STARTTIME,
        IDC_ENDTIME,
        IDC_DURATION,
        IDC_CATEGORYCHOICE,
        IDC_DESCRIPTION,
        IDC_DATECREATED,
        IDC_DATEUPDATED,
        IDC_ISACTIVE
    };
};
} // namespace app::dialog
