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

#include "taskitemdlg.h"

#include <algorithm>

#include <sqlite_modern_cpp/errors.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/timectrl.h>
#include <wx/statline.h>

#include "../common/constants.h"
#include "../common/common.h"
#include "../common/ids.h"
#include "../common/resources.h"
#include "../common/util.h"

#include "../config/configurationprovider.h"

#include "../data/taskdata.h"

wxDEFINE_EVENT(EVT_TASK_ITEM_INSERTED, wxCommandEvent);
wxDEFINE_EVENT(EVT_TASK_ITEM_UPDATED, wxCommandEvent);
wxDEFINE_EVENT(EVT_TASK_ITEM_DELETED, wxCommandEvent);

namespace app::dlg
{
static const wxString TaskContextWithoutClient = wxT("Employer %s");
static const wxString TaskContextWithClient = wxT("Employer %s | Client %s");

TaskItemDialog::TaskItemDialog(wxWindow* parent,
    std::shared_ptr<spdlog::logger> logger,
    constants::TaskItemTypes taskItemType,
    wxDateTime dateTimeContext,
    const wxString& name)
    : pParent(parent)
    , pLogger(logger)
    , mType(taskItemType)
    , bIsEdit(false)
    , mTaskItemId(-1)
    , mDateContext(dateTimeContext)
    , pTaskContextTextCtrl(nullptr)
    , pDateContextCtrl(nullptr)
    , pProjectChoiceCtrl(nullptr)
    , pStartTimeCtrl(nullptr)
    , pEndTimeCtrl(nullptr)
    , pDurationCtrl(nullptr)
    , pDurationHoursCtrl(nullptr)
    , pDurationMinutesCtrl(nullptr)
    , pDescriptionCtrl(nullptr)
    , pCategoryChoiceCtrl(nullptr)
    , pDateCreatedTextCtrl(nullptr)
    , pDateModifiedTextCtrl(nullptr)
    , pIsActiveCtrl(nullptr)
    , pOkButton(nullptr)
    , pCancelButton(nullptr)
    , mCalculatedRate(0.0)
    , pTaskItem(std::make_unique<model::TaskItemModel>())
    , pProject(nullptr)
    , mProjectData()
    , mTaskItemData()
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
    constants::TaskItemTypes taskItemType,
    bool edit,
    int taskId,
    wxDateTime dateTimeContext,
    const wxString& name)
    : pParent(parent)
    , pLogger(logger)
    , mType(taskItemType)
    , bIsEdit(edit)
    , mTaskItemId(taskId)
    , mDateContext(dateTimeContext)
    , pTaskContextTextCtrl(nullptr)
    , pDateContextCtrl(nullptr)
    , pProjectChoiceCtrl(nullptr)
    , pStartTimeCtrl(nullptr)
    , pEndTimeCtrl(nullptr)
    , pDurationCtrl(nullptr)
    , pDurationHoursCtrl(nullptr)
    , pDurationMinutesCtrl(nullptr)
    , pDescriptionCtrl(nullptr)
    , pCategoryChoiceCtrl(nullptr)
    , pDateCreatedTextCtrl(nullptr)
    , pDateModifiedTextCtrl(nullptr)
    , pIsActiveCtrl(nullptr)
    , pOkButton(nullptr)
    , pCancelButton(nullptr)
    , mCalculatedRate(0.0)
    , pTaskItem(std::make_unique<model::TaskItemModel>(mTaskItemId))
    , pProject(nullptr)
    , mProjectData()
    , mTaskItemData()
{
    Create(parent,
        wxID_ANY,
        wxT("Edit Task"),
        wxDefaultPosition,
        wxSize(395, 620),
        wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU,
        name);
}

const int64_t TaskItemDialog::GetTaskItemId() const
{
    return static_cast<int64_t>(mTaskItemId);
}

void TaskItemDialog::SetDurationFromStopwatchTask(wxTimeSpan duration)
{
    wxDateTime durationTime;
    durationTime.ParseISOTime(duration.Format());
    auto hours = durationTime.GetHour();
    auto minutes = durationTime.GetMinute();
    pDurationHoursCtrl->SetValue(hours);
    pDurationMinutesCtrl->SetValue(minutes);
}

void TaskItemDialog::SetTimesFromStopwatchTask(wxDateTime startTime, wxDateTime endTime)
{
    pStartTimeCtrl->SetValue(startTime);
    pEndTimeCtrl->SetValue(endTime);

    CalculateTimeDiff(startTime, endTime);
}

void TaskItemDialog::SetDescriptionFromStopwatchTask(const wxString& value)
{
    pDescriptionCtrl->ChangeValue(value);
}

void TaskItemDialog::SetMeetingData(svc::Meeting* meeting)
{
    wxString value = wxString::Format(wxT("%s\n%s"), meeting->Subject, meeting->Body);
    pDescriptionCtrl->ChangeValue(value);

    pStartTimeCtrl->SetValue(meeting->Start);
    pEndTimeCtrl->SetValue(meeting->End);

    CalculateTimeDiff(meeting->Start, meeting->End);
}

/* --PRIVATE METHODS-- */
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
        ConfigureEventBindings();
        FillControls();

        if (bIsEdit) {
            DataToControls();
        }

        GetSizer()->Fit(this);
        GetSizer()->SetSizeHints(this);

        wxIconBundle iconBundle("AppIcon", 0);
        SetIcons(iconBundle);

        Center();
    }

    return created;
}

void TaskItemDialog::CreateControls()
{
    /* Sizers */
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    auto mainPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(mainPanelSizer, common::sizers::ControlDefault);

    /* Sizer for left hand size controls */
    auto leftSizer = new wxBoxSizer(wxVERTICAL);
    mainPanelSizer->Add(leftSizer, 0);

    auto vertLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
    mainPanelSizer->Add(vertLine, common::sizers::ControlExpand);

    /* Sizer for right hand side controls */
    auto rightSizer = new wxBoxSizer(wxVERTICAL);
    mainPanelSizer->Add(rightSizer, 0);

    /* Sizer for bottom controls */
    auto mainBottomSizer = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(mainBottomSizer, common::sizers::ControlExpand);

    auto bottomSizer = new wxBoxSizer(wxVERTICAL);
    mainBottomSizer->Add(bottomSizer, 1);

    /* Task Context Box */
    auto contextBox = new wxStaticBox(this, wxID_ANY, wxT("Context"));
    auto contextBoxSizer = new wxStaticBoxSizer(contextBox, wxVERTICAL);
    leftSizer->Add(contextBoxSizer, common::sizers::ControlExpand);

    auto taskContextPanel = new wxPanel(this, wxID_STATIC);
    contextBoxSizer->Add(taskContextPanel, common::sizers::ControlExpand);

    auto contextSizer = new wxBoxSizer(wxVERTICAL);
    taskContextPanel->SetSizer(contextSizer);

    /* -- Controls -- */
    /* Task Context Info Text Control */
    pTaskContextTextCtrl = new wxStaticText(taskContextPanel, IDC_TASKCONTEXTINFO, wxT("Select a project..."));
    pTaskContextTextCtrl->SetToolTip(
        wxT("Additional information under which employer (and client) the task is being captured under"));
    auto taskContextTextFont = pTaskContextTextCtrl->GetFont();
    taskContextTextFont.SetPointSize(9);
    taskContextTextFont.MakeItalic();
    pTaskContextTextCtrl->SetFont(taskContextTextFont);
    contextSizer->Add(pTaskContextTextCtrl, common::sizers::ControlDefault);

    auto dateContextSizer = new wxBoxSizer(wxHORIZONTAL);
    contextSizer->Add(dateContextSizer, common::sizers::ControlDefault);

    /* Date Context DatePicker Control */
    auto dateContextLabel = new wxStaticText(taskContextPanel, wxID_STATIC, wxT("Date"));
    dateContextSizer->Add(dateContextLabel, common::sizers::ControlCenter);

    dateContextSizer->AddStretchSpacer(8);

    pDateContextCtrl = new wxDatePickerCtrl(
        taskContextPanel, IDC_DATECONTEXT, wxDefaultDateTime, wxDefaultPosition, wxSize(150, -1), wxDP_DROPDOWN);
    pDateContextCtrl->SetToolTip(wxT("The date under which this task will be saved under"));
    dateContextSizer->Add(pDateContextCtrl, common::sizers::ControlDefault);

    /* Task Details Box */
    auto detailsBox = new wxStaticBox(this, wxID_ANY, wxT("Task Details"));
    auto detailsBoxSizer = new wxStaticBoxSizer(detailsBox, wxVERTICAL);
    leftSizer->Add(detailsBoxSizer, common::sizers::ControlExpandProp);

    auto taskDetailsPanel = new wxPanel(this, wxID_STATIC);
    detailsBoxSizer->Add(taskDetailsPanel, common::sizers::ControlExpand);

    auto flexGridSizer = new wxFlexGridSizer(0, 2, 0, 0);
    taskDetailsPanel->SetSizer(flexGridSizer);

    /* -- Controls -- */
    /* Project Choice Control */
    auto projectLabel = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT("Project"));
    flexGridSizer->Add(projectLabel, common::sizers::ControlCenterVertical);

    pProjectChoiceCtrl = new wxChoice(taskDetailsPanel, IDC_PROJECTCHOICE, wxDefaultPosition, wxSize(150, -1));
    pProjectChoiceCtrl->AppendString(wxT("Select a project"));
    pProjectChoiceCtrl->SetSelection(0);
    pProjectChoiceCtrl->SetToolTip(wxT("Select a project to associate this task with"));
    flexGridSizer->Add(pProjectChoiceCtrl, common::sizers::ControlDefault);

    /* Category Choice Control */
    auto taskCategory = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT("Category"));
    flexGridSizer->Add(taskCategory, common::sizers::ControlCenterVertical);

    pCategoryChoiceCtrl = new wxChoice(taskDetailsPanel, IDC_CATEGORYCHOICE, wxDefaultPosition, wxSize(150, -1));
    pCategoryChoiceCtrl->AppendString(wxT("Select a category"));
    pCategoryChoiceCtrl->SetSelection(0);
    pCategoryChoiceCtrl->Disable();
    flexGridSizer->Add(pCategoryChoiceCtrl, common::sizers::ControlDefault);

    if (mType == constants::TaskItemTypes::TimedTask) {
        /* Start Time Control */
        auto startTimeLabel = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT("Start Time"));
        flexGridSizer->Add(startTimeLabel, common::sizers::ControlCenterVertical);

        pStartTimeCtrl = new wxTimePickerCtrl(
            taskDetailsPanel, IDC_STARTTIME, wxDefaultDateTime, wxDefaultPosition, wxSize(150, -1));
        pStartTimeCtrl->SetToolTip(wxT("Enter the time the task started"));
        flexGridSizer->Add(pStartTimeCtrl, common::sizers::ControlDefault);

        /* End Time Control */
        auto endTimeLabel = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT("End Time"));
        flexGridSizer->Add(endTimeLabel, common::sizers::ControlCenterVertical);

        pEndTimeCtrl =
            new wxTimePickerCtrl(taskDetailsPanel, IDC_ENDTIME, wxDefaultDateTime, wxDefaultPosition, wxSize(150, -1));
        pEndTimeCtrl->SetToolTip(wxT("Enter the time the task ended"));
        flexGridSizer->Add(pEndTimeCtrl, common::sizers::ControlDefault);

        /* Task Duration Text Control */
        auto durationLabel = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT("Duration"));
        flexGridSizer->Add(durationLabel, common::sizers::ControlCenterVertical);

        pDurationCtrl = new wxStaticText(taskDetailsPanel, IDC_DURATION, wxT("00:00:00"));
        pDurationCtrl->SetToolTip(wxT("Elasped time for the task"));
        flexGridSizer->Add(pDurationCtrl, common::sizers::ControlDefault);
    }

    if (mType == constants::TaskItemTypes::EntryTask) {
        auto durationBox = new wxStaticBox(this, wxID_ANY, wxEmptyString);
        auto durationBoxSizer = new wxStaticBoxSizer(durationBox, wxHORIZONTAL);
        leftSizer->Add(durationBoxSizer, common::sizers::ControlExpand);

        auto durationLabel = new wxStaticText(durationBox, wxID_STATIC, "Duration");
        durationBoxSizer->Add(durationLabel, common::sizers::ControlCenterVertical);

        durationBoxSizer->AddStretchSpacer(4);

        pDurationHoursCtrl = new wxSpinCtrl(durationBox,
            IDC_DURATIONHOURS,
            wxEmptyString,
            wxDefaultPosition,
            wxDefaultSize,
            wxSP_ARROW_KEYS | wxSP_WRAP | wxALIGN_CENTRE_HORIZONTAL,
            0,
            16);
        pDurationHoursCtrl->SetToolTip("Number of hours the task took");
        durationBoxSizer->Add(pDurationHoursCtrl, common::sizers::ControlDefault);

        pDurationMinutesCtrl = new wxSpinCtrl(durationBox,
            IDC_DURATIONMINUTES,
            wxEmptyString,
            wxDefaultPosition,
            wxDefaultSize,
            wxSP_ARROW_KEYS | wxSP_WRAP | wxALIGN_CENTRE_HORIZONTAL,
            0,
            59);
        pDurationMinutesCtrl->SetToolTip("Number of minutes the task took");
        pDurationMinutesCtrl->SetValue(15);
        durationBoxSizer->Add(pDurationMinutesCtrl, common::sizers::ControlDefault);
    }

    /* Task Description Box */
    auto descriptionBox = new wxStaticBox(this, wxID_ANY, wxGetEmptyString());
    auto descriptionBoxSizer = new wxStaticBoxSizer(descriptionBox, wxVERTICAL);
    rightSizer->Add(descriptionBoxSizer, common::sizers::ControlExpand);

    auto taskDescriptionPanel = new wxPanel(this, wxID_STATIC);
    descriptionBoxSizer->Add(taskDescriptionPanel, common::sizers::ControlExpand);

    auto descriptionSizer = new wxBoxSizer(wxVERTICAL);
    taskDescriptionPanel->SetSizer(descriptionSizer);

    /* Task Description Text Control */
    wxTextValidator descriptionValidator(wxFILTER_ALPHANUMERIC | wxFILTER_INCLUDE_CHAR_LIST);
    wxArrayString allowedChars;
    allowedChars.Add(wxT(" "));
    allowedChars.Add(wxT(","));
    allowedChars.Add(wxT("."));
    allowedChars.Add(wxT("'"));
    allowedChars.Add(wxT("("));
    allowedChars.Add(wxT(")"));
    allowedChars.Add(wxT("-"));
    allowedChars.Add(wxT("&"));
    descriptionValidator.SetIncludes(allowedChars);

    wxSize descriptionSize;
    if (mType == constants::TaskItemTypes::EntryTask) {
        descriptionSize.Set(275, 239);
    } else {
        descriptionSize.Set(275, 270);
    }

    pDescriptionCtrl = new wxTextCtrl(taskDescriptionPanel,
        IDC_DESCRIPTION,
        wxGetEmptyString(),
        wxDefaultPosition,
        descriptionSize,
        wxTE_MULTILINE,
        descriptionValidator);
    pDescriptionCtrl->SetHint(wxT("Description of task"));
    pDescriptionCtrl->SetToolTip(wxT("Enter a description for the task done"));
    pDescriptionCtrl->SetMaxLength(1024);
    descriptionSizer->Add(pDescriptionCtrl, wxSizerFlags().Border(wxALL, 5).Expand().Proportion(1));

    /* Bottom */
    if (bIsEdit) {
        /* Horizontal Line*/
        auto bottomSeparationLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(3, 3), wxLI_HORIZONTAL);
        bottomSizer->Add(bottomSeparationLine, wxSizerFlags().Expand());

        /* Metadata Box */
        auto metadataBox = new wxStaticBox(this, wxID_ANY, wxT("Metadata"));
        auto metadataBoxSizer = new wxStaticBoxSizer(metadataBox, wxVERTICAL);
        bottomSizer->Add(metadataBoxSizer, common::sizers::ControlExpand);

        auto metadataPanel = new wxPanel(this, wxID_STATIC);
        metadataBoxSizer->Add(metadataPanel, common::sizers::ControlExpand);

        auto metadataSizer = new wxBoxSizer(wxHORIZONTAL);
        metadataPanel->SetSizer(metadataSizer);

        /* Is Active Checkbox Control */
        pIsActiveCtrl = new wxCheckBox(metadataPanel, IDC_ISACTIVE, wxT("Is Active"));
        metadataSizer->Add(pIsActiveCtrl, common::sizers::ControlDefault);

        /* Date Created Static Text Control */
        pDateCreatedTextCtrl = new wxStaticText(metadataPanel, wxID_STATIC, wxGetEmptyString());
        metadataSizer->Add(pDateCreatedTextCtrl, common::sizers::ControlCenterVertical);

        /* Date Updated Static Text Control */
        pDateModifiedTextCtrl = new wxStaticText(metadataPanel, wxID_STATIC, wxGetEmptyString());
        metadataSizer->Add(pDateModifiedTextCtrl, common::sizers::ControlCenterVertical);
    }

    /* Horizontal Line*/
    auto bottomSeparationLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(3, 3), wxLI_HORIZONTAL);
    bottomSizer->Add(bottomSeparationLine, wxSizerFlags().Expand());

    /* Button Panel */
    auto buttonPanelSizer = new wxBoxSizer(wxHORIZONTAL);

    auto buttonPanel = new wxPanel(this, wxID_STATIC);
    buttonPanel->SetSizer(buttonPanelSizer);

    bottomSizer->Add(buttonPanel, wxSizerFlags().Border(wxALL, 5).Center());

    pOkButton = new wxButton(buttonPanel, wxID_OK, wxT("OK"));
    pOkButton->SetDefault();
    buttonPanelSizer->Add(pOkButton, wxSizerFlags().Border(wxALL, 5));

    pCancelButton = new wxButton(buttonPanel, wxID_CANCEL, wxT("Cancel"));
    buttonPanelSizer->Add(pCancelButton, wxSizerFlags().Border(wxALL, 5));

    pProjectChoiceCtrl->SetFocus();
}

// clang-format off
void TaskItemDialog::ConfigureEventBindings()
{
    pDateContextCtrl->Bind(
        wxEVT_DATE_CHANGED,
        &TaskItemDialog::OnDateContextChange,
        this
    );

    pProjectChoiceCtrl->Bind(
        wxEVT_CHOICE,
        &TaskItemDialog::OnProjectChoice,
        this
    );

    if (mType == constants::TaskItemTypes::TimedTask) {
        pStartTimeCtrl->Bind(
            wxEVT_TIME_CHANGED,
            &TaskItemDialog::OnStartTimeChange,
            this
        );

        pEndTimeCtrl->Bind(
            wxEVT_TIME_CHANGED,
            &TaskItemDialog::OnEndTimeChange,
            this
        );
    }

    if (bIsEdit) {
        pIsActiveCtrl->Bind(
            wxEVT_CHECKBOX,
            &TaskItemDialog::OnIsActiveCheck,
            this
        );
    }

    pOkButton->Bind(
        wxEVT_BUTTON,
        &TaskItemDialog::OnOk,
        this,
        wxID_OK
    );

    pCancelButton->Bind(
        wxEVT_BUTTON,
        &TaskItemDialog::OnCancel,
        this,
        wxID_CANCEL
    );
}
// clang-format on

void TaskItemDialog::FillControls()
{
    std::vector<std::unique_ptr<model::ProjectModel>> projects;

    try {
        projects = mProjectData.GetAll();
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in ProjectModel::GetAll() - {0:d} : {1}", e.get_code(), e.what());
        wxLogDebug(wxString(e.get_sql()));
    }

    for (const auto& project : projects) {
        pProjectChoiceCtrl->Append(project->GetDisplayName(), util::IntToVoidPointer(project->GetProjectId()));
    }

    if (!bIsEdit) {
        auto iterator = std::find_if(projects.begin(),
            projects.end(),
            [&](std::unique_ptr<model::ProjectModel>& project) { return project->IsDefault() == true; });

        if (iterator != projects.end()) {
            pProjectChoiceCtrl->SetStringSelection(iterator->get()->GetDisplayName());
            FillCategoryControl(iterator->get()->GetProjectId());

            pProject = mProjectData.GetById(iterator->get()->GetProjectId());

            if (iterator->get()->HasClientLinked()) {
                pTaskContextTextCtrl->SetLabel(wxString::Format(TaskContextWithClient,
                    wxString(iterator->get()->GetEmployer()->GetName()),
                    wxString(iterator->get()->GetClient()->GetName())));
            } else {
                pTaskContextTextCtrl->SetLabel(
                    wxString::Format(TaskContextWithoutClient, wxString(iterator->get()->GetEmployer()->GetName())));
            }
        }
    }

    wxDateTime timeInitializedToZero = wxDateTime::Now();
    if (mType == constants::TaskItemTypes::TimedTask) {
        timeInitializedToZero.SetSecond(0);

        pStartTimeCtrl->SetValue(timeInitializedToZero);
        pEndTimeCtrl->SetValue(timeInitializedToZero);
    }

    pDateContextCtrl->SetValue(mDateContext);
    auto bottomRangeDate = wxDateTime::GetCurrentYear() - 1;
    auto bottomDateContext = wxDateTime::Now().SetYear(bottomRangeDate);
    pDateContextCtrl->SetRange(bottomDateContext, mDateContext);
}

void TaskItemDialog::DataToControls()
{
    std::unique_ptr<model::TaskItemModel> taskItem = nullptr;

    try {
        taskItem = mTaskItemData.GetById(mTaskItemId);
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in TaskItemData::GetById() - {0:d} : {1}", e.get_code(), e.what());
        wxLogDebug(wxString(e.get_sql()));
    }

    try {
        pProject = mProjectData.GetById(taskItem->GetProjectId());
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in ProjectData::GetById() - {0:d} : {1}", e.get_code(), e.what());
        wxLogDebug(wxString(e.get_sql()));
    }

    pDateContextCtrl->SetValue(mDateContext);
    auto bottomRangeDate = wxDateTime::GetCurrentYear() - 1;
    auto bottomDateContext = wxDateTime::Now().SetYear(bottomRangeDate);
    pDateContextCtrl->SetRange(bottomDateContext, mDateContext);

    if (pProject->HasClientLinked()) {
        pTaskContextTextCtrl->SetLabel(wxString::Format(TaskContextWithClient,
            wxString(pProject->GetEmployer()->GetName()),
            wxString(pProject->GetClient()->GetName())));
    } else {
        pTaskContextTextCtrl->SetLabel(
            wxString::Format(TaskContextWithoutClient, wxString(pProject->GetEmployer()->GetName())));
    }

    pProjectChoiceCtrl->SetStringSelection(pProject->GetDisplayName());

    if (mType == constants::TaskItemTypes::TimedTask) {
        pStartTimeCtrl->SetValue(*taskItem->GetStartTime());
        pEndTimeCtrl->SetValue(*taskItem->GetEndTime());

        pDurationCtrl->SetLabel(taskItem->GetDuration());
    }
    if (mType == constants::TaskItemTypes::EntryTask) {
        auto durationTime = *taskItem->GetDurationTime();

        auto hours = durationTime.GetHour();
        auto minutes = durationTime.GetMinute();
        pDurationHoursCtrl->SetValue(hours);
        pDurationMinutesCtrl->SetValue(minutes);
    }

    FillCategoryControl(taskItem->GetProjectId());
    pCategoryChoiceCtrl->SetStringSelection(taskItem->GetCategory()->GetName());

    pDescriptionCtrl->SetValue(taskItem->GetDescription());

    pDateCreatedTextCtrl->SetLabel(
        wxString::Format(constants::DateCreatedLabel, util::ToFriendlyDateTimeString(taskItem->GetDateCreated())));

    pDateModifiedTextCtrl->SetLabel(
        wxString::Format(constants::DateModifiedLabel, util::ToFriendlyDateTimeString(taskItem->GetDateModified())));

    pIsActiveCtrl->SetValue(taskItem->IsActive());
}

void TaskItemDialog::CalculateTimeDiff(wxDateTime start, wxDateTime end)
{
    auto diff = end.Subtract(start);
    auto formated = diff.Format(wxT("%H:%M:%S"));
    pDurationCtrl->SetLabelText(formated);
}

void TaskItemDialog::GenerateTaskInsertedEvent(int taskItemId)
{
    wxCommandEvent taskInsertedEvent(EVT_TASK_ITEM_INSERTED);
    taskInsertedEvent.SetId(taskItemId);
    wxPostEvent(pParent, taskInsertedEvent);
}

void TaskItemDialog::GenerateTaskUpdatedEvent(int taskItemId)
{
    wxCommandEvent taskInsertedEvent(EVT_TASK_ITEM_UPDATED);
    taskInsertedEvent.SetId(taskItemId);
    wxPostEvent(pParent, taskInsertedEvent);
}

void TaskItemDialog::GenerateTaskDeletedEvent(int taskItemId)
{
    wxCommandEvent taskInsertedEvent(EVT_TASK_ITEM_DELETED);
    taskInsertedEvent.SetId(taskItemId);
    wxPostEvent(pParent, taskInsertedEvent);
}

void TaskItemDialog::OnDateContextChange(wxDateEvent& event)
{
    mDateContext = pDateContextCtrl->GetValue();
}

void TaskItemDialog::OnProjectChoice(wxCommandEvent& event)
{
    int selectedId = event.GetSelection();
    if (selectedId < 1) {
        return;
    }

    pCategoryChoiceCtrl->Clear();
    pCategoryChoiceCtrl->AppendString(wxT("Select a category"));
    pCategoryChoiceCtrl->SetSelection(0);
    int projectId = util::VoidPointerToInt(event.GetClientData());

    FillCategoryControl(projectId);

    try {
        pProject = mProjectData.GetById(projectId);
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in ProjectModel::GetById() - {0:d} : {1}", e.get_code(), e.what());
        wxLogDebug(wxString(e.get_sql()));
    }

    if (pProject->HasClientLinked()) {
        pTaskContextTextCtrl->SetLabel(wxString::Format(TaskContextWithClient,
            wxString(pProject->GetEmployer()->GetName()),
            wxString(pProject->GetClient()->GetName())));
    } else {
        pTaskContextTextCtrl->SetLabel(
            wxString::Format(TaskContextWithoutClient, wxString(pProject->GetEmployer()->GetName())));
    }
}

void TaskItemDialog::OnStartTimeChange(wxDateEvent& event)
{
    auto& start = event.GetDate();

    if (cfg::ConfigurationProvider::Get().Configuration->IsTimeRoundingEnabled()) {
        auto roundedDate =
            util::RoundToNearestInterval(start, cfg::ConfigurationProvider::Get().Configuration->GetTimeToRoundTo());
        pStartTimeCtrl->SetValue(roundedDate);
    }

    auto end = pEndTimeCtrl->GetValue();
    CalculateTimeDiff(start, end);
}

void TaskItemDialog::OnEndTimeChange(wxDateEvent& event)
{
    auto& end = event.GetDate();

    if (cfg::ConfigurationProvider::Get().Configuration->IsTimeRoundingEnabled()) {
        auto roundedDate =
            util::RoundToNearestInterval(end, cfg::ConfigurationProvider::Get().Configuration->GetTimeToRoundTo());
        pEndTimeCtrl->SetValue(roundedDate);
    }

    auto start = pStartTimeCtrl->GetValue();
    CalculateTimeDiff(start, end);
}

void TaskItemDialog::OnIsActiveCheck(wxCommandEvent& event)
{
    if (event.IsChecked()) {
        pProjectChoiceCtrl->Enable();
        if (mType == constants::TaskItemTypes::TimedTask) {
            pStartTimeCtrl->Enable();
            pEndTimeCtrl->Enable();
            pDurationCtrl->Enable();
        }
        if (mType == constants::TaskItemTypes::EntryTask) {
            pDurationHoursCtrl->Enable();
            pDurationMinutesCtrl->Enable();
        }
        pDescriptionCtrl->Enable();
        pCategoryChoiceCtrl->Enable();
    } else {
        pProjectChoiceCtrl->Disable();
        if (mType == constants::TaskItemTypes::TimedTask) {
            pStartTimeCtrl->Disable();
            pEndTimeCtrl->Disable();
            pDurationCtrl->Disable();
        }
        if (mType == constants::TaskItemTypes::EntryTask) {
            pDurationHoursCtrl->Disable();
            pDurationMinutesCtrl->Disable();
        }
        pDescriptionCtrl->Disable();
        pCategoryChoiceCtrl->Disable();
    }
}

void TaskItemDialog::OnOk(wxCommandEvent& event)
{
    if (TransferDataAndValidate()) {
        if (!bIsEdit) {
            int64_t id = -1;
            try {
                id = mTaskItemData.Create(std::move(pTaskItem));
                mTaskItemId = id;
            } catch (const sqlite::sqlite_exception& e) {
                pLogger->error("Error occured in TaskItemModel::Create() - {0:d} : {1}", e.get_code(), e.what());
                wxLogDebug(wxString(e.get_sql()));
                EndModal(ids::ID_ERROR_OCCURED);
            }
            GenerateTaskInsertedEvent(id);
        }

        if (bIsEdit && pIsActiveCtrl->IsChecked()) {
            try {
                mTaskItemData.Update(std::move(pTaskItem));
            } catch (const sqlite::sqlite_exception& e) {
                pLogger->error("Error occured in TaskItemModel::Update() - {0:d} : {1}", e.get_code(), e.what());
                wxLogDebug(wxString(e.get_sql()));
                EndModal(ids::ID_ERROR_OCCURED);
            }
            GenerateTaskUpdatedEvent(mTaskItemId);
        }

        if (bIsEdit && !pIsActiveCtrl->IsChecked()) {
            try {
                mTaskItemData.Delete(mTaskItemId);
            } catch (const sqlite::sqlite_exception& e) {
                pLogger->error("Error occured in TaskItemModel::Delete() - {0:d} : {1}", e.get_code(), e.what());
                wxLogDebug(wxString(e.get_sql()));
                EndModal(ids::ID_ERROR_OCCURED);
            }
            GenerateTaskDeletedEvent(mTaskItemId);
        }

        EndModal(wxID_OK);
    }
}

void TaskItemDialog::OnCancel(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
}

void TaskItemDialog::FillCategoryControl(int projectId)
{
    data::CategoryData categoryData;
    std::vector<std::unique_ptr<model::CategoryModel>> categories;
    try {
        categories = categoryData.GetByProjectId(projectId);
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in CategoryModel::GetByProjectId() - {0:d} : {1}", e.get_code(), e.what());
        wxLogDebug(wxString(e.get_sql()));
    }

    for (auto& category : categories) {
        pCategoryChoiceCtrl->Append(category->GetName(), util::IntToVoidPointer(category->GetCategoryId()));
    }

    if (!pCategoryChoiceCtrl->IsEnabled()) {
        pCategoryChoiceCtrl->Enable();
    }
}

bool TaskItemDialog::TransferDataAndValidate()
{
    pTaskItem->SetTaskItemTypeId(static_cast<int>(mType));

    if (pProjectChoiceCtrl->GetCount() <= 1) {
        common::validations::ForRequiredChoiceSelection(pProjectChoiceCtrl, wxT("project"));
        return false;
    }

    int projectId = util::VoidPointerToInt(pProjectChoiceCtrl->GetClientData(pProjectChoiceCtrl->GetSelection()));
    if (projectId < 1) {
        common::validations::ForRequiredChoiceSelection(pProjectChoiceCtrl, wxT("project"));
        return false;
    }
    pTaskItem->SetProjectId(projectId);

    // if (bIsEdit) {
    pTaskItem->SetProject(std::move(mProjectData.GetById(projectId)));
    // }

    if (mType == constants::TaskItemTypes::TimedTask) {
        auto startTime = pStartTimeCtrl->GetValue();
        auto endTime = pEndTimeCtrl->GetValue();
        auto isStartAheadOfEnd = startTime.IsLaterThan(endTime);
        if (isStartAheadOfEnd) {
            common::validations::ForInvalidTime(
                pStartTimeCtrl, wxT("A task cannot be started after\nthe time it has ended"));
            return false;
        }

        auto isEndBeforeStart = endTime.IsEarlierThan(startTime);
        if (isEndBeforeStart) {
            common::validations::ForInvalidTime(
                pEndTimeCtrl, wxT("A task cannot end before the \ntime it has started"));
            return false;
        }

        auto taskTimeSpan = endTime - startTime;
        auto isTaskLessThan15Minutes = taskTimeSpan.IsShorterThan(wxTimeSpan::Minutes(15));
        if (isTaskLessThan15Minutes) {
            common::validations::ForInvalidTime(pEndTimeCtrl, wxT("A task cannot be less than 15 minutes long"));
            return false;
        }

        pTaskItem->SetStartTime(std::move(std::make_unique<wxDateTime>(startTime)));
        pTaskItem->SetEndTime(std::move(std::make_unique<wxDateTime>(endTime)));
        pTaskItem->SetDuration(pDurationCtrl->GetLabel());
    }
    if (mType == constants::TaskItemTypes::EntryTask) {
        auto hours = pDurationHoursCtrl->GetValue();
        auto minutes = pDurationMinutesCtrl->GetValue();
        if (hours == 0) {
            if (minutes < 15) {
                common::validations::ForInvalidTime(pDurationMinutesCtrl, wxT("A task cannot be less than 15 minutes long"));
                return false;
            }
        }

        auto asFormattedTime = wxString::Format("%02d:%02d:00", hours, minutes);
        pTaskItem->SetDuration(asFormattedTime);
    }

    if (pCategoryChoiceCtrl->GetCount() <= 1) {
        common::validations::ForRequiredChoiceSelection(pCategoryChoiceCtrl, wxT("category"));
        return false;
    }

    int categoryId = util::VoidPointerToInt(pCategoryChoiceCtrl->GetClientData(pCategoryChoiceCtrl->GetSelection()));
    if (categoryId < 1) {
        common::validations::ForRequiredChoiceSelection(pCategoryChoiceCtrl, wxT("category"));
        return false;
    }
    pTaskItem->SetCategoryId(categoryId);

    wxString description = pDescriptionCtrl->GetValue().Trim();
    if (description.empty() || description.length() < 4 || description.length() > 1024) {
        common::validations::ForRequiredLongText(pDescriptionCtrl, wxT("description"));
        return false;
    }
    pTaskItem->SetDescription(description);

    data::TaskData taskData;
    int taskId = -1;
    try {
        taskId = taskData.GetByDate(pDateContextCtrl->GetValue())->GetTaskId();
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in TaskModel::GetByDate() - {0:d} : {1}", e.get_code(), e.what());
    }
    pTaskItem->SetTaskId(taskId);

    return true;
}
} // namespace app::dlg
