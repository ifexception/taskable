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

#include <wx/wx.h>

#include <spdlog/spdlog.h>

#include "../config/configuration.h"

#include "../models/projectmodel.h"
#include "../models/taskmodel.h"
#include "../models/taskitemmodel.h"

class wxDateEvent;
class wxDatePickerCtrl;
class wxTimePickerCtrl;

wxDECLARE_EVENT(EVT_TASK_ITEM_INSERTED, wxCommandEvent);

namespace app::dlg
{
class TaskItemDialog : public wxDialog
{
public:
    TaskItemDialog() = delete;
    explicit TaskItemDialog(wxWindow* parent,
        std::shared_ptr<spdlog::logger> logger,
        std::shared_ptr<cfg::Configuration> config,
        constants::TaskItemTypes taskItemType,
        wxDateTime dateTimeContext = wxDateTime::Now(),
        const wxString& name = wxT("taskitemdlg"));

    explicit TaskItemDialog(wxWindow* parent,
        std::shared_ptr<spdlog::logger> logger,
        std::shared_ptr<cfg::Configuration> config,
        constants::TaskItemTypes taskItemType,
        bool edit,
        int taskId,
        wxDateTime dateTimeContext = wxDateTime::Now(),
        const wxString& name = wxT("taskitemdlg"));

    virtual ~TaskItemDialog() = default;

    void SetDurationFromStopwatchTask(wxTimeSpan duration);
    void SetTimesFromStopwatchTask(wxDateTime startTime, wxDateTime endTime);
    void SetDescriptionFromStopwatchTask(const wxString& value);

private:
    bool Create(wxWindow* parent,
        wxWindowID windowId,
        const wxString& title,
        const wxPoint& point,
        const wxSize& size,
        long style,
        const wxString& name);

    void CreateControls();
    void ConfigureEventBindings();
    void FillControls();
    void DataToControls();

    void OnDateContextChange(wxDateEvent& event);
    void OnProjectChoice(wxCommandEvent& event);
    void OnStartTimeChange(wxDateEvent& event);
    void OnEndTimeChange(wxDateEvent& event);
    void OnDurationTimeChange(wxDateEvent& event);
    void OnIsActiveCheck(wxCommandEvent& event);
    void OnStartTimeFocusLost(wxFocusEvent& event);
    void OnEndTimeFocusLost(wxFocusEvent& event);
    void OnDurationTimeFocusLost(wxFocusEvent& event);
    void OnOk(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);

    void FillCategoryControl(int projectId);
    void SetRateLabel(model::ProjectModel* project);

    void CalculateTimeDiff(wxDateTime start, wxDateTime end);
    void CalculateRate();
    void CalculateRate(wxDateTime time);
    void CalculateRate(wxDateTime start, wxDateTime end);
    void CalculateRate(wxTimeSpan timeSpan);

    void GenerateTaskSavedEvent();

    bool TransferDataAndValidate();

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
    wxStaticText* pCalculatedRateTextCtrl;
    wxTextCtrl* pDescriptionCtrl;
    wxChoice* pCategoryChoiceCtrl;
    wxStaticText* pDateCreatedTextCtrl;
    wxStaticText* pDateModifiedTextCtrl;
    wxCheckBox* pIsActiveCtrl;
    wxButton* pOkButton;
    wxButton* pCancelButton;

    constants::TaskItemTypes mType;
    int mTaskItemId;
    bool bIsEdit;
    wxDateTime mDateContext;
    double mCalculatedRate;

    std::unique_ptr<model::TaskItemModel> pTaskItem;
    std::unique_ptr<model::ProjectModel> pProject;

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
        IDC_CALCULATEDRATE,
        IDC_DATECREATED,
        IDC_DATEUPDATED,
        IDC_ISACTIVE
    };

    static const wxString DateCreatedLabel;
    static const wxString DateModifiedLabel;
    static const wxString CalculatedRateLabelNonBillable;
    static const wxString CalculatedRateLabelBillableUnknownRate;
    static const wxString CalculatedRateLabelBillableHourlyRate;
};
} // namespace app::dlg
