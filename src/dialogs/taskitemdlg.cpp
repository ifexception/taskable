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

#include "taskitemdlg.h"

#include <sqlite_modern_cpp/errors.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/timectrl.h>
#include <wx/statline.h>

#include "../common/constants.h"
#include "../common/common.h"
#include "../common/ids.h"
#include "../common/util.h"
#include "../services/db_service.h"

wxDEFINE_EVENT(TASK_INSERTED, wxCommandEvent);

namespace app::dialog
{
static const wxString TaskContextWithoutClient = wxT("Capturing task for employer %s");
static const wxString TaskContextWithClient = wxT("Capturing task for employer %s and client %s");

// clang-format off
wxBEGIN_EVENT_TABLE(TaskItemDialog, wxDialog)
EVT_BUTTON(wxID_OK, TaskItemDialog::OnOk)
EVT_BUTTON(wxID_CANCEL, TaskItemDialog::OnCancel)
EVT_DATE_CHANGED(TaskItemDialog::IDC_DATECONTEXT, TaskItemDialog::OnDateContextChange)
EVT_CHOICE(TaskItemDialog::IDC_PROJECTCHOICE, TaskItemDialog::OnProjectChoice)
EVT_TIME_CHANGED(TaskItemDialog::IDC_STARTTIME, TaskItemDialog::OnStartTimeChange)
EVT_TIME_CHANGED(TaskItemDialog::IDC_ENDTIME, TaskItemDialog::OnEndTimeChange)
EVT_TIME_CHANGED(TaskItemDialog::IDC_DURATIONTIME, TaskItemDialog::OnDurationTimeChange)
EVT_CHECKBOX(TaskItemDialog::IDC_ISACTIVE, TaskItemDialog::OnIsActiveCheck)
wxEND_EVENT_TABLE()

TaskItemDialog::TaskItemDialog(wxWindow* parent,
    std::shared_ptr<spdlog::logger> logger,
    std::shared_ptr<cfg::Configuration> config,
    TaskItemType taskItemType,
    wxDateTime dateTimeContext,
    const wxString& name)
    : pParent(parent)
    , pLogger(logger)
    , pConfig(config)
    , mType(taskItemType)
    , bIsEdit(false)
    , mTaskItemId(0)
    , mDateContext(dateTimeContext)
    , mDurationSpan()
    , mProjectId(-1)
    , mStartTime()
    , mEndTime()
    , mDurationText(wxGetEmptyString())
    , bBillable(false)
    , mDescriptionText(wxGetEmptyString())
    , mCategoryId(-1)
// clang-format on
{
    Create(parent,
        wxID_ANY,
        wxT("Add Task"),
        wxDefaultPosition,
        wxSize(395, 488),
        wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU,
        name);
}

TaskItemDialog::TaskItemDialog(wxWindow* parent,
    std::shared_ptr<spdlog::logger> logger,
    std::shared_ptr<cfg::Configuration> config,
    TaskItemType editTaskItemType,
    bool edit,
    int taskId,
    wxDateTime dateTimeContext,
    const wxString& name)
    : pParent(parent)
    , pLogger(logger)
    , pConfig(config)
    , mType(editTaskItemType)
    , bIsEdit(edit)
    , mTaskItemId(taskId)
    , mDateContext(dateTimeContext)
    , mDurationSpan()
    , mProjectId(-1)
    , mStartTime()
    , mEndTime()
    , mDurationText(wxGetEmptyString())
    , bBillable(false)
    , mDescriptionText(wxGetEmptyString())
    , mCategoryId(-1)
{
    Create(parent,
        wxID_ANY,
        wxT("Edit Task"),
        wxDefaultPosition,
        wxSize(395, 620),
        wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU,
        name);
}

void TaskItemDialog::SetDurationFromStopwatchTask(wxTimeSpan duration)
{
    wxDateTime durationTime;
    durationTime.ParseISOTime(duration.Format());
    pDurationTimeCtrl->SetValue(durationTime);
}

void TaskItemDialog::SetTimesFromStopwatchTask(wxDateTime startTime, wxDateTime endTime)
{
    pStartTimeCtrl->SetValue(startTime);
    pEndTimeCtrl->SetValue(endTime);

    CalculateTimeDiff(startTime, endTime);
}

void TaskItemDialog::SetDescriptionFromStopwatchTask(const wxString& value)
{
    pDescriptionCtrl->SetValue(value);
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
        GetSizer()->SetSizeHints(this);
        SetIcon(common::GetProgramIcon());
        Center();
    }

    return created;
}

void TaskItemDialog::CreateControls()
{
    /* Sizers */
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    /* Task Context Box */
    auto contextBox = new wxStaticBox(this, wxID_ANY, wxT("Context"));
    auto contextBoxSizer = new wxStaticBoxSizer(contextBox, wxVERTICAL);
    mainSizer->Add(contextBoxSizer, common::sizers::ControlExpand);

    auto taskContextPanel = new wxPanel(this, wxID_STATIC);
    contextBoxSizer->Add(taskContextPanel, common::sizers::ControlExpand);

    auto contextSizer = new wxBoxSizer(wxVERTICAL);
    taskContextPanel->SetSizer(contextSizer);

    /* -- Controls -- */
    /* Task Context Info Text Control */
    pTaskContextTextCtrl = new wxStaticText(taskContextPanel, IDC_TASKCONTEXTINFO, wxT("Select a project..."));
    pTaskContextTextCtrl->SetToolTip(
        wxT("Additional information under which employer and client the task is being added"));
    auto taskContextTextFont = pTaskContextTextCtrl->GetFont();
    taskContextTextFont.MakeItalic();
    taskContextTextFont.SetPointSize(9);
    pTaskContextTextCtrl->SetFont(taskContextTextFont);
    contextSizer->Add(pTaskContextTextCtrl, common::sizers::ControlDefault);

    auto dateContextSizer = new wxBoxSizer(wxHORIZONTAL);
    contextSizer->Add(dateContextSizer, common::sizers::ControlDefault);

    /* Date Context Control */
    auto dateContextText = new wxStaticText(taskContextPanel, wxID_STATIC, wxT("Date          "));
    dateContextSizer->Add(dateContextText, common::sizers::ControlCenter);

    pDateContextCtrl = new wxDatePickerCtrl(
        taskContextPanel, IDC_DATECONTEXT, wxDefaultDateTime, wxDefaultPosition, wxSize(150, -1), wxDP_DROPDOWN);
    pDateContextCtrl->SetToolTip(wxT("The date under which this task will be saved under"));
    dateContextSizer->Add(pDateContextCtrl, common::sizers::ControlDefault);

    /* Task Details Box */
    auto detailsBox = new wxStaticBox(this, wxID_ANY, wxT("Task Details"));
    auto detailsBoxSizer = new wxStaticBoxSizer(detailsBox, wxVERTICAL);
    mainSizer->Add(detailsBoxSizer, common::sizers::ControlExpandProp);

    auto taskDetailsPanel = new wxPanel(this, wxID_STATIC);
    detailsBoxSizer->Add(taskDetailsPanel, common::sizers::ControlExpand);

    auto flexGridSizer = new wxFlexGridSizer(0, 2, 0, 0);
    taskDetailsPanel->SetSizer(flexGridSizer);

    /* -- Controls -- */
    /* Project Choice Control */
    auto activeProject = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT("Project"));
    flexGridSizer->Add(activeProject, common::sizers::ControlCenterVertical);

    pProjectChoiceCtrl = new wxChoice(taskDetailsPanel, IDC_PROJECTCHOICE, wxDefaultPosition, wxSize(150, -1));
    pProjectChoiceCtrl->AppendString(wxT("Select a project"));
    pProjectChoiceCtrl->SetSelection(0);
    pProjectChoiceCtrl->SetToolTip(wxT("Select a project to associate this task with"));
    flexGridSizer->Add(pProjectChoiceCtrl, common::sizers::ControlDefault);

    if (mType == TaskItemType::TimedTask) {
        /* Task Start Time Control */
        auto taskStartTime = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT("Start Time"));
        flexGridSizer->Add(taskStartTime, common::sizers::ControlCenterVertical);

        pStartTimeCtrl = new wxTimePickerCtrl(
            taskDetailsPanel, IDC_STARTTIME, wxDefaultDateTime, wxDefaultPosition, wxSize(150, -1));
        pStartTimeCtrl->SetToolTip(wxT("Enter the time the task started"));
        flexGridSizer->Add(pStartTimeCtrl, common::sizers::ControlDefault);

        /* Task End Time Control */
        auto taskEndTime = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT("End Time"));
        flexGridSizer->Add(taskEndTime, common::sizers::ControlCenterVertical);

        pEndTimeCtrl =
            new wxTimePickerCtrl(taskDetailsPanel, IDC_ENDTIME, wxDefaultDateTime, wxDefaultPosition, wxSize(150, -1));
        pEndTimeCtrl->SetToolTip(wxT("Enter the time the task ended"));
        flexGridSizer->Add(pEndTimeCtrl, common::sizers::ControlDefault);

        /* Task Duration Text Control */
        auto durationText = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT("Duration"));
        flexGridSizer->Add(durationText, common::sizers::ControlCenterVertical);

        pDurationCtrl = new wxStaticText(taskDetailsPanel, IDC_DURATION, wxT("00:00:00"));
        pDurationCtrl->SetToolTip(wxT("Elasped time for the task"));
        flexGridSizer->Add(pDurationCtrl, common::sizers::ControlDefault);
    }

    if (mType == TaskItemType::EntryTask) {
        /* Task Duration Time Control */
        auto durationText = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT("Duration"));
        flexGridSizer->Add(durationText, common::sizers::ControlCenterVertical);

        pDurationTimeCtrl = new wxTimePickerCtrl(
            taskDetailsPanel, IDC_DURATIONTIME, wxDefaultDateTime, wxDefaultPosition, wxSize(150, -1));
        pDurationTimeCtrl->SetToolTip(wxT("Enter the elasped time for the task"));
        flexGridSizer->Add(pDurationTimeCtrl, common::sizers::ControlDefault);
    }

    /* Billable Checkbox Control */
    auto billableFillerText = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxGetEmptyString());
    flexGridSizer->Add(billableFillerText, common::sizers::ControlDefault);

    pBillableCtrl = new wxCheckBox(taskDetailsPanel, IDC_BILLABLE, wxT("Billable"));
    pBillableCtrl->SetToolTip(wxT("Set whether this task is billable or not"));
    flexGridSizer->Add(pBillableCtrl, common::sizers::ControlDefault);

    /* Task Category Dropdown Control */
    auto taskCategory = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT("Category"));
    flexGridSizer->Add(taskCategory, common::sizers::ControlCenterVertical);

    pCategoryChoiceCtrl = new wxChoice(taskDetailsPanel, IDC_CATEGORYCHOICE, wxDefaultPosition, wxSize(150, -1));
    pCategoryChoiceCtrl->AppendString(wxT("Select a category"));
    pCategoryChoiceCtrl->SetSelection(0);
    pCategoryChoiceCtrl->Disable();
    flexGridSizer->Add(pCategoryChoiceCtrl, common::sizers::ControlDefault);

    if (bIsEdit) {
        /* Is Active Checkbox Control */
        auto isActiveFiller = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT(""));
        flexGridSizer->Add(isActiveFiller, common::sizers::ControlDefault);

        pIsActiveCtrl = new wxCheckBox(taskDetailsPanel, IDC_ISACTIVE, wxT("Is Active"));
        flexGridSizer->Add(pIsActiveCtrl, common::sizers::ControlDefault);
    }

    /* Task Description Text Control */
    auto taskDescription = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT("Description"));
    flexGridSizer->Add(taskDescription, common::sizers::ControlDefault);

    auto descriptionFiller = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT(""));
    flexGridSizer->Add(descriptionFiller, common::sizers::ControlDefault);

    pDescriptionCtrl =
        new wxTextCtrl(this, IDC_DESCRIPTION, wxGetEmptyString(), wxDefaultPosition, wxSize(320, 180), wxTE_MULTILINE);
    pDescriptionCtrl->Bind(wxEVT_KILL_FOCUS, &TaskItemDialog::OnDescriptionControlFocusLost, this);
    pDescriptionCtrl->SetToolTip(wxT("Enter a description for the task"));
    pDescriptionCtrl->SetMaxLength(1024);
    detailsBoxSizer->Add(pDescriptionCtrl, 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    if (bIsEdit) {
        /* Date Created Static Text Control */
        pDateCreatedTextCtrl = new wxStaticText(this, wxID_STATIC, wxT("Created on: %s"));
        auto font = pDateCreatedTextCtrl->GetFont();
        font.MakeItalic();
        font.SetPointSize(8);
        pDateCreatedTextCtrl->SetFont(font);
        detailsBoxSizer->Add(pDateCreatedTextCtrl, 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 10);

        /* Date Updated Static Text Control */
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

    auto okButton = new wxButton(buttonPanel, wxID_OK, wxT("&OK"));
    auto cancelButton = new wxButton(buttonPanel, wxID_CANCEL, wxT("&Cancel"));

    buttonPanelSizer->Add(okButton, common::sizers::ControlDefault);
    buttonPanelSizer->Add(cancelButton, common::sizers::ControlDefault);
}

void TaskItemDialog::FillControls()
{
    services::db_service dbService;

    std::vector<models::project> projects;
    try {
        projects = dbService.get_projects();
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in get_projects() - {0:d} : {1}", e.get_code(), e.what());
    }

    for (auto project : projects) {
        pProjectChoiceCtrl->Append(project.display_name, util::IntToVoidPointer(project.project_id));
    }

    if (mType == TaskItemType::TimedTask) {
        wxDateTime timeWithZeroSeconds = wxDateTime::Now();
        timeWithZeroSeconds.SetSecond(0);

        pStartTimeCtrl->SetValue(timeWithZeroSeconds);
        pEndTimeCtrl->SetValue(timeWithZeroSeconds);
    }

    if (mType == TaskItemType::EntryTask) {
        wxDateTime timeInitializedToZero = wxDateTime::Now();
        timeInitializedToZero.SetSecond(0);
        timeInitializedToZero.SetMinute(0);
        timeInitializedToZero.SetHour(0);

        pDurationTimeCtrl->SetValue(timeInitializedToZero);
    }

    pDateContextCtrl->SetValue(mDateContext);
    auto bottomRangeDate = wxDateTime::GetCurrentYear() - 1;
    auto bottomDateContext = wxDateTime::Now().SetYear(bottomRangeDate);
    pDateContextCtrl->SetRange(bottomDateContext, mDateContext);
}

void TaskItemDialog::DataToControls()
{
    services::db_service dbService;
    models::task_item taskItem;

    try {
        assert(mTaskItemId != 0);
        taskItem = dbService.get_task_item_by_id(mTaskItemId);
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in get_task_item_by_id() - {0:d} : {1}", e.get_code(), e.what());
    }

    pDateContextCtrl->SetValue(mDateContext);
    auto bottomRangeDate = wxDateTime::GetCurrentYear() - 1;
    auto bottomDateContext = wxDateTime::Now().SetYear(bottomRangeDate);
    pDateContextCtrl->SetRange(bottomDateContext, mDateContext);

    pProjectChoiceCtrl->SetStringSelection(taskItem.project_name);

    if (mType == TaskItemType::TimedTask) {
        wxDateTime startTime;
        startTime.ParseISOTime(*taskItem.start_time);
        pStartTimeCtrl->SetValue(startTime);

        wxDateTime endTime;
        endTime.ParseISOTime(*taskItem.end_time);
        pEndTimeCtrl->SetValue(endTime);

        pDurationCtrl->SetLabel(taskItem.duration);
    }
    if (mType == TaskItemType::EntryTask) {
        wxDateTime durationTime;
        durationTime.ParseISOTime(taskItem.duration);
        pDurationTimeCtrl->SetValue(durationTime);
    }

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

int TaskItemDialog::GetTaskId()
{
    auto dateNowString = mDateContext.FormatISODate();
    services::db_service dbService;
    int taskId = 0;
    try {
        taskId = dbService.create_or_get_task_id(dateNowString);
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in create_or_get_task_id() - {0:d} : {1}", e.get_code(), e.what());
    }

    return taskId;
}

void TaskItemDialog::CalculateTimeDiff(wxDateTime start, wxDateTime end)
{
    auto diff = end.Subtract(start);
    auto formated = diff.Format(wxT("%H:%M:%S"));
    pDurationCtrl->SetLabelText(formated);
}

bool TaskItemDialog::PostValidate()
{
    if (mDescriptionText.empty()) {
        auto message = wxString::Format(Constants::Messages::IsEmpty, wxT("Task description"));
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
    if (mType == TaskItemType::EntryTask) {
        bool isEmpty = (mProjectId == 0 || mProjectId == -1) && (mCategoryId == 0 || mCategoryId == -1) &&
                       mDescriptionText.empty();
        return isEmpty;
    }
    if (mType == TaskItemType::TimedTask) {
        bool isEmpty = (mProjectId == 0 || mProjectId == -1) && mStartTime == wxDefaultDateTime &&
                       mEndTime == wxDefaultDateTime && (mCategoryId == 0 || mCategoryId == -1) &&
                       mDescriptionText.empty();
        return isEmpty;
    }
}

void TaskItemDialog::OnDateContextChange(wxDateEvent& event)
{
    mDateContext = pDateContextCtrl->GetValue();
}

void TaskItemDialog::OnProjectChoice(wxCommandEvent& event)
{
    pCategoryChoiceCtrl->Clear();
    pCategoryChoiceCtrl->AppendString(wxT("Select a category"));
    pCategoryChoiceCtrl->SetSelection(0);
    int projectId = util::VoidPointerToInt(event.GetClientData());

    FillCategoryControl(projectId);

    services::db_service dbService;
    models::task_context taskContext;
    try {
        taskContext = dbService.get_employer_and_client_by_project_id(projectId);
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error(
            "Error occured in get_employer_and_client_by_project_id() - {0:d} : {1}", e.get_code(), e.what());
    }

    if (taskContext.client_name) {
        pTaskContextTextCtrl->SetLabel(
            wxString::Format(TaskContextWithClient, taskContext.employer_name, taskContext.client_name));
        taskContext.cleanup();
    } else {
        pTaskContextTextCtrl->SetLabel(wxString::Format(TaskContextWithoutClient, taskContext.employer_name));
    }
}

void TaskItemDialog::OnStartTimeChange(wxDateEvent& event)
{
    auto start = event.GetDate();

    if (pConfig->IsTimeRoundingEnabled()) {
        auto date = start;
        auto roundedDate = util::RoundToNearestInterval(date, pConfig->GetTimeToRoundTo());
        pStartTimeCtrl->SetValue(roundedDate);
    }

    auto end = pEndTimeCtrl->GetValue();
    CalculateTimeDiff(start, end);

    mValidator.Validate(start, end);
}

void TaskItemDialog::OnEndTimeChange(wxDateEvent& event)
{
    auto end = event.GetDate();

    if (pConfig->IsTimeRoundingEnabled()) {
        auto date = end;
        auto roundedDate = util::RoundToNearestInterval(date, pConfig->GetTimeToRoundTo());
        pEndTimeCtrl->SetValue(roundedDate);
    }

    auto start = pStartTimeCtrl->GetValue();
    CalculateTimeDiff(start, end);

    mValidator.Validate(start, end);
}

void TaskItemDialog::OnIsActiveCheck(wxCommandEvent& event)
{
    if (event.IsChecked()) {
        pProjectChoiceCtrl->Enable();
        if (mType == TaskItemType::TimedTask) {
            pStartTimeCtrl->Enable();
            pEndTimeCtrl->Enable();
            pDurationCtrl->Enable();
        }
        if (mType == TaskItemType::EntryTask) {
            pDurationTimeCtrl->Enable();
        }
        pBillableCtrl->Enable();
        pDescriptionCtrl->Enable();
        pCategoryChoiceCtrl->Enable();
    } else {
        pProjectChoiceCtrl->Disable();
        if (mType == TaskItemType::TimedTask) {
            pStartTimeCtrl->Disable();
            pEndTimeCtrl->Disable();
            pDurationCtrl->Disable();
        }
        if (mType == TaskItemType::EntryTask) {
            pDurationTimeCtrl->Disable();
        }
        pDurationTimeCtrl->Disable();
        pBillableCtrl->Disable();
        pDescriptionCtrl->Disable();
        pCategoryChoiceCtrl->Disable();
    }
}

void TaskItemDialog::OnDurationTimeChange(wxDateEvent& event)
{
    if (pConfig->IsTimeRoundingEnabled()) {
        auto date = event.GetDate();
        auto roundedDate = util::RoundToNearestInterval(date, pConfig->GetTimeToRoundTo());
        pDurationTimeCtrl->SetValue(roundedDate);
    }
}

void TaskItemDialog::OnDescriptionControlFocusLost(wxFocusEvent& event)
{
    if (pDescriptionCtrl->GetValue().length() < 2) {
        auto message = wxString::Format(Constants::Messages::TooShort, wxT("Task description"), 2);
        wxMessageBox(message, wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
    }

    if (pDescriptionCtrl->GetValue().length() > 255) {
        auto message = wxString::Format(Constants::Messages::TooLong, wxT("Task description"), 255);
        wxMessageBox(message, wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
    }

    event.Skip();
}

void TaskItemDialog::OnOk(wxCommandEvent& event)
{
    mProjectId = util::VoidPointerToInt(pProjectChoiceCtrl->GetClientData(pProjectChoiceCtrl->GetSelection()));
    mCategoryId = util::VoidPointerToInt(pCategoryChoiceCtrl->GetClientData(pCategoryChoiceCtrl->GetSelection()));
    mDescriptionText = pDescriptionCtrl->GetValue();
    bBillable = pBillableCtrl->GetValue();

    bool isValid = PostValidate();
    if (!isValid) {
        return;
    }

    if (mType == TaskItemType::TimedTask) {
        mStartTime = pStartTimeCtrl->GetValue();
        mEndTime = pEndTimeCtrl->GetValue();
        mDurationText = pDurationCtrl->GetLabelText();
    }
    if (mType == TaskItemType::EntryTask) {
        mDurationTime = pDurationTimeCtrl->GetValue();
    }

    services::db_service dbService;

    const int taskId = GetTaskId();
    models::task_item taskItem;
    if (mType == TaskItemType::TimedTask) {
        taskItem.task_id = taskId;
        taskItem.start_time = new std::string(mStartTime.FormatISOTime().ToUTF8());
        taskItem.end_time = new std::string(mEndTime.FormatISOTime().ToUTF8());
        taskItem.duration = mDurationText;
        taskItem.description = std::string(mDescriptionText.ToUTF8());
        taskItem.billable = bBillable;
        taskItem.project_id = mProjectId;
        taskItem.category_id = mCategoryId;
        taskItem.task_item_type_id = static_cast<int>(mType);
    }
    if (mType == TaskItemType::EntryTask) {
        taskItem.task_id = taskId;
        taskItem.start_time = nullptr;
        taskItem.end_time = nullptr;
        taskItem.duration = std::string(mDurationTime.FormatISOTime().ToUTF8());
        taskItem.description = std::string(mDescriptionText.ToUTF8());
        taskItem.billable = bBillable;
        taskItem.project_id = mProjectId;
        taskItem.category_id = mCategoryId;
        taskItem.task_item_type_id = static_cast<int>(mType);
    }

    if (bIsEdit) {
        taskItem.task_item_id = mTaskItemId;
    }

    ProcessTaskItem(taskItem);

    taskItem.cleanup();
    GenerateTaskSavedEvent();
    EndModal(wxID_OK);
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

void TaskItemDialog::GenerateTaskSavedEvent()
{
    wxCommandEvent taskInsertedEvent(TASK_INSERTED);
    wxPostEvent(pParent, taskInsertedEvent);
}

void TaskItemDialog::ProcessTaskItem(const models::task_item& taskItem)
{
    services::db_service dbService;
    if (bIsEdit && pIsActiveCtrl->IsChecked()) {
        try {
            dbService.update_task_item(taskItem);
        } catch (const sqlite::sqlite_exception& e) {
            pLogger->error("Error occured in task_item update_task_item() - {0:d} : {1}", e.get_code(), e.what());
        }
    } else if (bIsEdit && !pIsActiveCtrl->IsChecked()) {
        try {
            dbService.delete_task_item(mTaskItemId, util::UnixTimestamp());
        } catch (const sqlite::sqlite_exception& e) {
            pLogger->error("Error occured in task_item delete_task_item() - {0:d} : {1}", e.get_code(), e.what());
        }
    } else {
        try {
            dbService.create_new_task_item(taskItem);
        } catch (const sqlite::sqlite_exception& e) {
            pLogger->error("Error occured in task_item create_new_task_item() - {0:d} : {1}", e.get_code(), e.what());
        }
    }
}

bool TimeValidators::Validate(wxDateTime startTime, wxDateTime endTime)
{
    auto isStartAheadOfEnd = startTime.IsLaterThan(endTime);
    if (isStartAheadOfEnd) {
        wxMessageBox(wxT("A task cannot be started after the time it has ended"),
            wxT("Validation failure"),
            wxOK | wxICON_EXCLAMATION);
        return false;
    }

    auto isEndBeforeStart = endTime.IsEarlierThan(startTime);
    if (isEndBeforeStart) {
        wxMessageBox(wxT("A task cannot end before the time it has started"),
            wxT("Validation failure"),
            wxOK | wxICON_EXCLAMATION);
        return false;
    }

    auto taskTimeSpan = endTime - startTime;
    auto isTaskLessThan5Minutes = taskTimeSpan.IsShorterThan(wxTimeSpan::Minutes(5));
    if (isTaskLessThan5Minutes) {
        wxMessageBox(
            wxT("A task cannot be less than 5 minutes long"), wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
        return false;
    }
}
} // namespace app::dialog
