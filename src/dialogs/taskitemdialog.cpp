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

#include "taskitemdialog.h"

#include <sqlite_modern_cpp/errors.h>
#include <wx/timectrl.h>
#include <wx/dateevt.h>
#include <wx/statline.h>

#include "../common/constants.h"
#include "../common/common.h"
#include "../common/ids.h"
#include "../common/util.h"
#include "../services/db_service.h"

wxDEFINE_EVENT(TASK_INSERTED, wxCommandEvent);

namespace app::dialog
{
// clang-format off
wxBEGIN_EVENT_TABLE(TaskItemDialog, wxDialog)
EVT_BUTTON(ids::ID_SAVE, TaskItemDialog::OnSave)
EVT_BUTTON(wxID_CANCEL, TaskItemDialog::OnCancel)
EVT_CHOICE(TaskItemDialog::IDC_PROJECTCHOICE, TaskItemDialog::OnProjectChoice)
EVT_TIME_CHANGED(TaskItemDialog::IDC_STARTTIME, TaskItemDialog::OnStartTimeChange)
EVT_TIME_CHANGED(TaskItemDialog::IDC_ENDTIME, TaskItemDialog::OnEndTimeChange)
EVT_CHECKBOX(TaskItemDialog::IDC_ISACTIVE, TaskItemDialog::OnIsActiveCheck)
wxEND_EVENT_TABLE()

TaskItemDialog::TaskItemDialog(wxWindow* parent,
    std::shared_ptr<spdlog::logger> logger,
    bool isEdit,
    int taskDetailId,
    const wxString& name)
    : pLogger(logger)
    , mTaskDate(wxGetEmptyString())
    , bIsEdit(isEdit)
    , mTaskItemId(taskDetailId)
    , mProjectId(-1)
    , mStartTime()
    , mEndTime()
    , pDurationCtrl()
    , mCategoryId(-1)
    , mDescriptionText(wxGetEmptyString())
    , pParent(parent)
    , bIsPausableTask(false)
// clang-format on
{
    wxString title;
    wxSize size;
    if (bIsEdit) {
        title = wxT("Edit Task");
        size.Set(395, 620);
    } else {
        title = wxT("Add Task");
        size.Set(395, 488);
    }
    bool success =
        Create(parent, wxID_ANY, title, wxDefaultPosition, size, wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU, name);
}

TaskItemDialog::TaskItemDialog(wxWindow* parent,
    std::shared_ptr<spdlog::logger> logger,
    wxTimeSpan duration,
    const wxString& name)
    : pLogger(logger)
    , mTaskDate(wxGetEmptyString())
    , bIsEdit(false)
    , mTaskItemId(0)
    , mProjectId(-1)
    , mStartTime()
    , mEndTime()
    , pDurationCtrl()
    , mCategoryId(-1)
    , mDescriptionText(wxGetEmptyString())
    , pParent(parent)
    , mDuration(duration)
    , bIsPausableTask(true)
{
    CreateWithParam(pParent,
        wxID_ANY,
        wxT("Add Task"),
        wxDefaultPosition,
        wxSize(395, 488),
        wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU,
        name);
}

TaskItemDialog::TaskItemDialog(wxWindow* parent,
    std::shared_ptr<spdlog::logger> logger,
    wxDateTime startTime,
    wxDateTime endTime,
    const wxString& name)
    : pLogger(logger)
    , mTaskDate(wxGetEmptyString())
    , bIsEdit(false)
    , mTaskItemId(0)
    , mProjectId(-1)
    , mStartTime(startTime)
    , mEndTime(endTime)
    , pDurationCtrl()
    , mCategoryId(-1)
    , mDescriptionText(wxGetEmptyString())
    , pParent(parent)
    , bIsPausableTask(false)
{
    CreateWithParams(pParent,
        wxID_ANY,
        wxT("Add Task"),
        wxDefaultPosition,
        wxSize(395, 488),
        wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU,
        name);
}

bool TaskItemDialog::Create(wxWindow* parent,
    wxWindowID windowId,
    const wxString& title,
    const wxPoint& point,
    const wxSize& size,
    long style,
    const wxString& name)
{
    bool created = wxDialog::Create(parent, windowId, title, point, size, style, name);

    if (created) {
        CreateControls();
        FillControls();

        if (bIsEdit) {
            DataToControls();
        }

        GetSizer()->Fit(this);
        SetIcon(common::GetProgramIcon());
        GetSizer()->SetSizeHints(this);
        Center();
    }

    return created;
}

bool TaskItemDialog::CreateWithParam(wxWindow* parent,
    wxWindowID windowId,
    const wxString& title,
    const wxPoint& point,
    const wxSize& size,
    long style,
    const wxString& name)
{
    bool created = wxDialog::Create(parent, windowId, title, point, size, style, name);

    if (created) {
        CreateControls();
        FillControls();

        SetValueToControl();

        GetSizer()->Fit(this);
        SetIcon(common::GetProgramIcon());
        GetSizer()->SetSizeHints(this);
        Center();
    }

    return created;
}

bool TaskItemDialog::CreateWithParams(wxWindow* parent,
    wxWindowID windowId,
    const wxString& title,
    const wxPoint& point,
    const wxSize& size,
    long style,
    const wxString& name)
{
    bool created = wxDialog::Create(parent, windowId, title, point, size, style, name);

    if (created) {
        CreateControls();
        FillControls();

        SetValuesToControls();

        GetSizer()->Fit(this);
        SetIcon(common::GetProgramIcon());
        GetSizer()->SetSizeHints(this);
        Center();
    }

    return created;
}

void TaskItemDialog::CreateControls()
{
    /* Window Sizing */
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    auto mainPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(mainPanelSizer, common::sizers::ControlDefault);

    auto sizer = new wxBoxSizer(wxVERTICAL);
    mainPanelSizer->Add(sizer, 0);

    /* Task Details Box */
    auto detailsBox = new wxStaticBox(this, wxID_ANY, wxT("Task Details"));
    auto detailsBoxSizer = new wxStaticBoxSizer(detailsBox, wxVERTICAL);
    sizer->Add(detailsBoxSizer, common::sizers::ControlExpandProp);

    auto taskDetailsPanel = new wxPanel(this, wxID_STATIC);
    detailsBoxSizer->Add(taskDetailsPanel, common::sizers::ControlExpand);

    auto taskFlexGridSizer = new wxFlexGridSizer(0, 2, 0, 0);
    taskDetailsPanel->SetSizer(taskFlexGridSizer);

    /* ---Controls--- */
    /* Active Project Dropdown Control */
    auto activeProject = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT("Project"));
    taskFlexGridSizer->Add(activeProject, common::sizers::ControlCenterVertical);

    pProjectChoiceCtrl = new wxChoice(taskDetailsPanel, IDC_PROJECTCHOICE, wxDefaultPosition, wxSize(150, -1));
    pProjectChoiceCtrl->AppendString(wxT("Select a project"));
    pProjectChoiceCtrl->SetSelection(0);
    pProjectChoiceCtrl->SetToolTip(wxT("Select a project to associate this task with"));
    taskFlexGridSizer->Add(pProjectChoiceCtrl, common::sizers::ControlDefault);

    /* Task Start Time Control */
    auto taskStartTime = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT("Start Time"));
    taskFlexGridSizer->Add(taskStartTime, common::sizers::ControlCenterVertical);

    pStartTimeCtrl =
        new wxTimePickerCtrl(taskDetailsPanel, IDC_STARTTIME, wxDefaultDateTime, wxDefaultPosition, wxSize(150, -1));
    pStartTimeCtrl->SetToolTip(wxT("Enter the time the task started"));
    taskFlexGridSizer->Add(pStartTimeCtrl, common::sizers::ControlDefault);

    /* Task End Time Control */
    auto taskEndTime = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT("End Time"));
    taskFlexGridSizer->Add(taskEndTime, common::sizers::ControlCenterVertical);

    pEndTimeCtrl =
        new wxTimePickerCtrl(taskDetailsPanel, IDC_ENDTIME, wxDefaultDateTime, wxDefaultPosition, wxSize(150, -1));
    pEndTimeCtrl->SetToolTip(wxT("Enter the time the task ended"));
    taskFlexGridSizer->Add(pEndTimeCtrl, common::sizers::ControlDefault);

    /* Task Duration Text Control */
    auto durationText = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT("Duration"));
    taskFlexGridSizer->Add(durationText, common::sizers::ControlCenterVertical);

    pDurationCtrl = new wxStaticText(taskDetailsPanel, IDC_DURATION, wxT("00:00:00"));
    pDurationCtrl->SetToolTip(wxT("Elasped time for the task"));
    taskFlexGridSizer->Add(pDurationCtrl, common::sizers::ControlDefault);

    /* Billable Checkbox Control */
    auto billableFillerText = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT(""));
    taskFlexGridSizer->Add(billableFillerText, common::sizers::ControlDefault);

    pBillableCtrl = new wxCheckBox(taskDetailsPanel, IDC_BILLABLE, wxT("Billable"));
    pBillableCtrl->SetToolTip(wxT("Set whether this task is billable or not"));
    taskFlexGridSizer->Add(pBillableCtrl, common::sizers::ControlDefault);

    /* Task Category Dropdown Control */
    auto taskCategory = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT("Category"));
    taskFlexGridSizer->Add(taskCategory, common::sizers::ControlCenterVertical);

    pCategoryChoiceCtrl = new wxChoice(taskDetailsPanel, IDC_CATEGORYCHOICE, wxDefaultPosition, wxSize(150, -1));
    pCategoryChoiceCtrl->AppendString(wxT("Select a category"));
    pCategoryChoiceCtrl->SetSelection(0);
    pCategoryChoiceCtrl->Disable();
    taskFlexGridSizer->Add(pCategoryChoiceCtrl, common::sizers::ControlDefault);

    if (bIsEdit) {
        auto isActiveFiller = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT(""));
        taskFlexGridSizer->Add(isActiveFiller, common::sizers::ControlDefault);

        /* Is Active Checkbox Control */
        pIsActiveCtrl = new wxCheckBox(taskDetailsPanel, IDC_ISACTIVE, wxT("Is Active"));
        taskFlexGridSizer->Add(pIsActiveCtrl, common::sizers::ControlDefault);
    }

    /* Task Description Text Control */
    auto taskDescription = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT("Description"));
    taskFlexGridSizer->Add(taskDescription, common::sizers::ControlDefault);

    auto descriptionFiller = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT(""));
    taskFlexGridSizer->Add(descriptionFiller, common::sizers::ControlDefault);

    pDescriptionCtrl =
        new wxTextCtrl(this, IDC_DESCRIPTION, wxGetEmptyString(), wxDefaultPosition, wxSize(320, 180), wxTE_MULTILINE);
    pDescriptionCtrl->SetToolTip(wxT("Enter a description for the task"));
    detailsBoxSizer->Add(pDescriptionCtrl, 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    if (bIsEdit) {
        /* Date Created Text Control */
        pDateCreatedTextCtrl = new wxStaticText(this, wxID_STATIC, wxT("Created on: %s"));
        auto font = pDateCreatedTextCtrl->GetFont();
        font.MakeItalic();
        font.SetPointSize(10);
        pDateCreatedTextCtrl->SetFont(font);
        detailsBoxSizer->Add(pDateCreatedTextCtrl, 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 10);

        /* Date Updated Text Control */
        pDateUpdatedTextCtrl = new wxStaticText(this, wxID_STATIC, wxT("Updated on: %s"));
        detailsBoxSizer->Add(pDateUpdatedTextCtrl, 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    }

    /* Horizontal Line*/
    auto separationLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
    mainSizer->Add(separationLine, 0, wxEXPAND | wxALL, 1);

    /* Button Panel */
    auto buttonPanel = new wxPanel(this, wxID_STATIC);
    auto buttonPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonPanel->SetSizer(buttonPanelSizer);
    mainSizer->Add(buttonPanel, common::sizers::ControlCenter);

    auto okButton = new wxButton(buttonPanel, ids::ID_SAVE, wxT("&Save"));
    auto cancelButton = new wxButton(buttonPanel, wxID_CANCEL, wxT("&Cancel"));

    buttonPanelSizer->Add(okButton, common::sizers::ControlDefault);
    buttonPanelSizer->Add(cancelButton, common::sizers::ControlDefault);
}

void TaskItemDialog::FillControls()
{
    std::vector<models::project> projects;

    services::db_service dbService;
    try {
        projects = dbService.get_projects();
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in get_projects() - {0:d} : {1}", e.get_code(), e.what());
    }

    for (auto project : projects) {
        pProjectChoiceCtrl->Append(project.display_name, util::IntToVoidPointer(project.project_id));
    }
}

void TaskItemDialog::DataToControls()
{
    services::db_service dbService;
    models::task_item taskItem;
    try {
        taskItem = dbService.get_task_item_by_id(mTaskItemId);
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in get_task_item_by_id() - {0:d} : {1}", e.get_code(), e.what());
    }

    pProjectChoiceCtrl->SetStringSelection(taskItem.project_name);

    if (!bIsPausableTask) {
        wxDateTime startTime;
        startTime.ParseISOTime(taskItem.start_time);
        pStartTimeCtrl->SetValue(startTime);

        wxDateTime endTime;
        endTime.ParseISOTime(taskItem.end_time);
        pEndTimeCtrl->SetValue(endTime);
    }

    pDurationCtrl->SetLabel(taskItem.duration);

    FillCategoryControl(taskItem.project_id);
    pCategoryChoiceCtrl->SetStringSelection(taskItem.category_name);

    pBillableCtrl->SetValue(taskItem.billable);

    pDescriptionCtrl->SetValue(taskItem.description);

    wxString dateCreatedString = util::ConvertUnixTimestampToString(taskItem.date_created_utc);
    wxString dateCreatedLabel = pDateCreatedTextCtrl->GetLabelText();
    pDateCreatedTextCtrl->SetLabel(wxString::Format(dateCreatedLabel, dateCreatedString));

    wxString dateUpdatedString = util::ConvertUnixTimestampToString(taskItem.date_modified_utc);
    wxString dateUpdatedLabel = pDateUpdatedTextCtrl->GetLabelText();
    pDateUpdatedTextCtrl->SetLabel(wxString::Format(dateUpdatedLabel, dateUpdatedString));

    pIsActiveCtrl->SetValue(taskItem.is_active);
}

void TaskItemDialog::SetValuesToControls()
{
    pStartTimeCtrl->SetValue(mStartTime);
    pEndTimeCtrl->SetValue(mEndTime);

    CalculateTimeDiff(mStartTime, mEndTime);
}

void TaskItemDialog::SetValueToControl()
{
    pDurationCtrl->SetLabel(mDuration.Format());
    pStartTimeCtrl->Disable();
    pEndTimeCtrl->Disable();
}

int TaskItemDialog::GetTaskId()
{
    wxDateTime date = wxDateTime::Now();
    mTaskDate = date.FormatISODate();
    services::db_service dbService;
    int taskId = 0;
    try {
        taskId = dbService.create_or_get_task_id(mTaskDate);
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in create_or_get_task_id() - {0:d} : {1}", e.get_code(), e.what());
    }

    return taskId;
}

bool TaskItemDialog::Validate()
{
    if (!bIsPausableTask) {
        auto isStartAheadOfEnd = mStartTime.IsLaterThan(mEndTime);
        if (isStartAheadOfEnd) {
            wxMessageBox(wxT("A task cannot be started after the time it has ended"),
                wxT("Validation failure"),
                wxOK | wxICON_EXCLAMATION);
            return false;
        }

        auto isEndBeforeStart = mEndTime.IsEarlierThan(mStartTime);
        if (isEndBeforeStart) {
            wxMessageBox(wxT("A task cannot end before the time it has started"),
                wxT("Validation failure"),
                wxOK | wxICON_EXCLAMATION);
            return false;
        }

        if (!bIsEdit) {
            auto isEndTimeInTheFuture = mEndTime.IsLaterThan(wxDateTime::Now());
            if (isEndTimeInTheFuture) {
                wxMessageBox(
                    wxT("A task cannot end in the future"), wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
                return false;
            }
        }

        auto taskTimeSpan = mEndTime - mStartTime;
        auto isTaskLessThan5Minutes = taskTimeSpan.IsShorterThan(wxTimeSpan::Minutes(5));
        if (isTaskLessThan5Minutes) {
            wxMessageBox(
                wxT("A task cannot be less than 5 minutes long"), wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
            return false;
        }
    }

    if (mDescriptionText.empty()) {
        auto message = wxString::Format(Constants::Messages::IsEmpty, wxT("Task description"));
        wxMessageBox(message, wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
        return false;
    }

    if (mDescriptionText.length() < 2) {
        auto message = wxString::Format(Constants::Messages::TooShort, wxT("Task description"));
        wxMessageBox(message, wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
        return false;
    }

    if (mDescriptionText.length() > 255) {
        auto message = wxString::Format(Constants::Messages::TooLong, wxT("Task description"));
        wxMessageBox(message, wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
        return false;
    }

    if (mProjectId == 0 || mProjectId == -1) {
        auto message = wxString::Format(Constants::Messages::SelectionRequired, wxT("Project"));
        wxMessageBox(message, wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
        return false;
    }

    if (mCategoryId == 0 || mCategoryId == -1) {
        auto message = wxString::Format(Constants::Messages::SelectionRequired, wxT("Category"));
        wxMessageBox(message, wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
        return false;
    }

    return true;
}

bool TaskItemDialog::AreControlsEmpty()
{
    if (bIsPausableTask) {
        bool isEmpty = (mProjectId == 0 || mProjectId == -1) && (mCategoryId == 0 || mCategoryId == -1) &&
                       mDescriptionText.empty();
        return isEmpty;
    } else {
        bool isEmpty = (mProjectId == 0 || mProjectId == -1) && mStartTime == wxDefaultDateTime &&
                       mEndTime == wxDefaultDateTime && (mCategoryId == 0 || mCategoryId == -1) &&
                       mDescriptionText.empty();
        return isEmpty;
    }
}

void TaskItemDialog::OnProjectChoice(wxCommandEvent& event)
{
    pCategoryChoiceCtrl->Clear();
    pCategoryChoiceCtrl->AppendString(wxT("Select a category"));
    pCategoryChoiceCtrl->SetSelection(0);
    int projectId = util::VoidPointerToInt(event.GetClientData());

    FillCategoryControl(projectId);
}

void TaskItemDialog::OnStartTimeChange(wxDateEvent& event)
{
    auto start = event.GetDate();
    auto end = pEndTimeCtrl->GetValue();
    if (end != wxDefaultDateTime) {
        CalculateTimeDiff(start, end);
    }
}

void TaskItemDialog::OnEndTimeChange(wxDateEvent& event)
{
    auto end = event.GetDate();
    auto start = pStartTimeCtrl->GetValue();

    if (start != wxDefaultDateTime) {
        CalculateTimeDiff(start, end);
    }
}

void TaskItemDialog::OnIsActiveCheck(wxCommandEvent& event)
{
    if (event.IsChecked()) {
        pProjectChoiceCtrl->Enable();
        if (!bIsPausableTask) {
            pStartTimeCtrl->Enable();
            pEndTimeCtrl->Enable();
        }
        pDurationCtrl->Enable();
        pBillableCtrl->Enable();
        pDescriptionCtrl->Enable();
        pCategoryChoiceCtrl->Enable();
    } else {
        pProjectChoiceCtrl->Disable();
        pStartTimeCtrl->Disable();
        pEndTimeCtrl->Disable();
        pDurationCtrl->Disable();
        pBillableCtrl->Disable();
        pDescriptionCtrl->Disable();
        pCategoryChoiceCtrl->Disable();
    }
}

void TaskItemDialog::OnSave(wxCommandEvent& event)
{
    mProjectId = util::VoidPointerToInt(pProjectChoiceCtrl->GetClientData(pProjectChoiceCtrl->GetSelection()));
    mStartTime = pStartTimeCtrl->GetValue();
    mEndTime = pEndTimeCtrl->GetValue();
    mDurationText = pDurationCtrl->GetLabelText();
    mCategoryId = util::VoidPointerToInt(pCategoryChoiceCtrl->GetClientData(pCategoryChoiceCtrl->GetSelection()));
    mDescriptionText = pDescriptionCtrl->GetValue();
    bBillable = pBillableCtrl->GetValue();

    auto validationSuccess = Validate();
    if (!validationSuccess) {
        return;
    }

    int taskId = GetTaskId();

    services::db_service dbService;
    try {
        wxString startTime;
        wxString endTime;

        if (bIsPausableTask) {
            startTime = wxT("");
            endTime = wxT("");
        } else {
            startTime = mStartTime.FormatISOTime();
            endTime = mEndTime.FormatISOTime();
        }

        if (bIsEdit && pIsActiveCtrl->IsChecked()) {
            models::task_item taskItem;
            taskItem.task_item_id = mTaskItemId;
            taskItem.start_time = startTime;
            taskItem.end_time = endTime;
            taskItem.duration = mDurationText;
            taskItem.description = std::string(mDescriptionText.ToUTF8());
            taskItem.billable = bBillable;
            taskItem.date_modified_utc = util::UnixTimestamp();
            taskItem.project_id = mProjectId;
            taskItem.category_id = mCategoryId;
            dbService.update_task_item(taskItem);
        } else if (bIsEdit && !pIsActiveCtrl->IsChecked()) {
            dbService.delete_task_item(mTaskItemId, util::UnixTimestamp());
        } else {
            dbService.create_new_task_item(mProjectId,
                taskId,
                std::string(startTime.ToUTF8()),
                std::string(endTime.ToUTF8()),
                std::string(mDurationText.ToUTF8()),
                mCategoryId,
                std::string(mDescriptionText.ToUTF8()),
                bBillable);
        }
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in task_item OnSave() - {0:d} : {1}", e.get_code(), e.what());
    }

    OnTaskSaved();

    EndModal(ids::ID_SAVE);
}

void TaskItemDialog::OnCancel(wxCommandEvent& event)
{
    bool areControlsEmpty = AreControlsEmpty();
    if (!areControlsEmpty) {
        int answer = wxMessageBox(wxT("Are you sure you want to cancel?"), wxT("Confirm"), wxYES_NO | wxICON_QUESTION);
        if (answer == wxYES) {
            EndModal(wxID_CANCEL);
        }
    } else {
        EndModal(wxID_CANCEL);
    }
}

void TaskItemDialog::OnTaskSaved()
{
    wxCommandEvent taskInsertedEvent(TASK_INSERTED);
    wxPostEvent(pParent, taskInsertedEvent);
}

void TaskItemDialog::CalculateTimeDiff(wxDateTime start, wxDateTime end)
{
    auto diff = end.Subtract(start);
    auto formated = diff.Format(wxT("%H:%M:%S"));
    pDurationCtrl->SetLabelText(formated);
}

void TaskItemDialog::FillCategoryControl(int projectId)
{
    std::vector<models::category> categories;
    try {
        services::db_service dbService;
        categories = dbService.get_categories_by_project_id(projectId);
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in get_categories_by_project_id() - {0:d} : {1}", e.get_code(), e.what());
    }

    for (auto category : categories) {
        pCategoryChoiceCtrl->Append(category.category_name, util::IntToVoidPointer(category.category_id));
    }

    if (!pCategoryChoiceCtrl->IsEnabled()) {
        pCategoryChoiceCtrl->Enable();
    }
}

} // namespace app::dialog
