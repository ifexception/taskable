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

#include <wx/wx.h>

class wxTimePickerCtrl;
class wxDateEvent;

namespace app::dialog
{
class TaskDetailsDialog : public wxDialog
{
public:
    TaskDetailsDialog() = default;
    explicit TaskDetailsDialog(wxWindow* parent, bool isEdit = false, int taskDetailId = 0, const wxString& name = wxT("TaskDetailsDialog"));
    virtual ~TaskDetailsDialog() = default;

private:
    wxDECLARE_DYNAMIC_CLASS(TaskDetailsDialog);
    wxDECLARE_EVENT_TABLE();

    bool Create(wxWindow* parent, wxWindowID windowId, const wxString& title, const wxPoint& point, const wxSize& size, long style, const wxString& name);

    void CreateControls();
    void FillControls();
    void DataToControls();

    int GetTaskId();

    bool Validate();
    bool AreControlsEmpty();

    void OnProjectChoice(wxCommandEvent& event);
    void OnStartTimeChange(wxDateEvent& event);
    void OnEndTimeChange(wxDateEvent& event);
    void OnIsActiveCheck(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnTaskSaved(wxCommandEvent& event);

    void CaclulateTimeDiff(wxDateTime start, wxDateTime end);
    void FillCategoryControl(int projectId);

    wxString mTaskDate;
    bool bIsEdit;
    int mTaskDetailId;

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

    int mProjectId;
    wxDateTime mStartTime;
    wxDateTime mEndTime;
    wxString mDurationText;
    int mCategoryId;
    wxString mDescriptionText;

    enum
    {
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
