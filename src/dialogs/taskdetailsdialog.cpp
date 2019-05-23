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

#include "taskdetailsdialog.h"

#include <wx/timectrl.h>
#include <wx/dateevt.h>
#include <wx/statline.h>

#include "../common/common.h"
#include "../common/ids.h"
#include "../common/util.h"
#include "../services/db_service.h"
#include "../db/database_exception.h"

namespace app::dialog
{
wxIMPLEMENT_DYNAMIC_CLASS(TaskDetailsDialog, wxDialog);

wxBEGIN_EVENT_TABLE(TaskDetailsDialog, wxDialog)
    EVT_BUTTON(ids::ID_SAVE, TaskDetailsDialog::OnSave)
    EVT_BUTTON(wxID_CANCEL, TaskDetailsDialog::OnCancel)
    EVT_CHOICE(TaskDetailsDialog::IDC_PROJECTCHOICE, TaskDetailsDialog::OnProjectChoice)
    EVT_TIME_CHANGED(TaskDetailsDialog::IDC_STARTTIME, TaskDetailsDialog::OnStartTimeChange)
    EVT_TIME_CHANGED(TaskDetailsDialog::IDC_ENDTIME, TaskDetailsDialog::OnEndTimeChange)
    EVT_CHECKBOX(TaskDetailsDialog::IDC_ISACTIVE, TaskDetailsDialog::OnIsActiveCheck)
wxEND_EVENT_TABLE()

TaskDetailsDialog::TaskDetailsDialog(wxWindow* parent, bool isEdit, int taskDetailId, const wxString& name)
    : mTaskDate(wxGetEmptyString())
    , bIsEdit(isEdit)
    , mTaskDetailId(taskDetailId)
    , mProjectId(-1)
    , mStartTime()
    , mEndTime()
    , mCategoryId(-1)
    , mDescriptionText(wxGetEmptyString())
    , pParent(parent)
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
    bool success = Create(parent, wxID_ANY, title, wxDefaultPosition, size, wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU, name);
}

TaskDetailsDialog::TaskDetailsDialog()
{}

TaskDetailsDialog::~TaskDetailsDialog()
{
    Destroy();
}

void TaskDetailsDialog::Launch()
{
    ShowModal();
}

bool TaskDetailsDialog::Create(wxWindow* parent, wxWindowID windowId, const wxString& title, const wxPoint& point, const wxSize& size, long style, const wxString& name)
{
    bool created = wxDialog::Create(parent, windowId, title, point, size, style, name);

    if (created) {
        CreateControls();
        FillControls();

        if (bIsEdit) {
            DataToControls();
        }

        GetSizer()->Fit(this);
        //SetIcon();
        GetSizer()->SetSizeHints(this);
        Center();
    }

    return created;
}

void TaskDetailsDialog::CreateControls()
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

    pStartTimeCtrl = new wxTimePickerCtrl(taskDetailsPanel, IDC_STARTTIME, wxDefaultDateTime, wxDefaultPosition, wxSize(150, -1));
    pStartTimeCtrl->SetToolTip(wxT("Enter the time the task started"));
    taskFlexGridSizer->Add(pStartTimeCtrl, common::sizers::ControlDefault);

    /* Task End Time Control */
    auto taskEndTime = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT("End Time"));
    taskFlexGridSizer->Add(taskEndTime, common::sizers::ControlCenterVertical);

    pEndTimeCtrl = new wxTimePickerCtrl(taskDetailsPanel, IDC_ENDTIME, wxDefaultDateTime, wxDefaultPosition, wxSize(150, -1));
    pEndTimeCtrl->SetToolTip(wxT("Enter the time the task ended"));
    taskFlexGridSizer->Add(pEndTimeCtrl, common::sizers::ControlDefault);

    /* Task Duration Text Control */
    auto durationText = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT("Duration"));
    taskFlexGridSizer->Add(durationText, common::sizers::ControlCenterVertical);

    pDurationCtrl = new wxStaticText(taskDetailsPanel, IDC_DURATION, wxT("00:00:00"));
    pDurationCtrl->SetToolTip(wxT("Elasped time for the task"));
    taskFlexGridSizer->Add(pDurationCtrl, common::sizers::ControlDefault);

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

    pDescriptionCtrl = new wxTextCtrl(this, IDC_DESCRIPTION, wxGetEmptyString(), wxDefaultPosition, wxSize(320, 180), wxTE_MULTILINE);
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

void TaskDetailsDialog::FillControls()
{
    std::vector<models::project> projects;

    services::db_service dbService;
    try {
        projects = dbService.get_projects();
    } catch (const db::database_exception& e) {
        // TODO Log exception
    }

    for (auto project : projects) {
        pProjectChoiceCtrl->Append(project.display_name, (void*)project.project_id);
    }
}

void TaskDetailsDialog::DataToControls()
{
    services::db_service dbService;
    models::task_detail taskDetail;
    try {
        taskDetail = dbService.get_task_by_id(mTaskDetailId);
    } catch (const db::database_exception& e) {
        // TODO Log exception
    }

    pProjectChoiceCtrl->SetStringSelection(taskDetail.project_name);
    wxDateTime startTime;
    startTime.ParseISOTime(taskDetail.start_time);
    pStartTimeCtrl->SetValue(startTime);

    wxDateTime endTime;
    endTime.ParseISOTime(taskDetail.end_time);
    pEndTimeCtrl->SetValue(endTime);

    pDurationCtrl->SetLabel(taskDetail.duration);

    FillCategoryControl(taskDetail.project_id);
    pCategoryChoiceCtrl->SetStringSelection(taskDetail.category_name);

    pDescriptionCtrl->SetValue(taskDetail.description);

    wxString dateCreatedString = util::ConvertUnixTimestampToString(taskDetail.date_created_utc);
    wxString dateCreatedLabel = pDateCreatedTextCtrl->GetLabelText();
    pDateCreatedTextCtrl->SetLabel(wxString::Format(dateCreatedLabel, dateCreatedString));

    wxString dateUpdatedString = util::ConvertUnixTimestampToString(taskDetail.date_modified_utc);
    wxString dateUpdatedLabel = pDateUpdatedTextCtrl->GetLabelText();
    pDateUpdatedTextCtrl->SetLabel(wxString::Format(dateUpdatedLabel, dateUpdatedString));

    pIsActiveCtrl->SetValue(taskDetail.is_active);
}

int TaskDetailsDialog::GetTaskId()
{
    wxDateTime date = wxDateTime::Now();
    mTaskDate = date.FormatISODate();
    services::db_service dbService;
    int taskId = 0;
    try {
        taskId = dbService.create_or_get_task_id(mTaskDate, mProjectId);
    } catch (const db::database_exception& e) {
        // TODO Log exception
    }

    return taskId;
}

bool TaskDetailsDialog::Validate()
{
    auto isStartAheadOfEnd = mStartTime.IsLaterThan(mEndTime);
    if (isStartAheadOfEnd) {
        wxMessageBox(wxT("A task cannot be started after the time it has ended"), wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
        return false;
    }

    auto isEndBeforeStart = mEndTime.IsEarlierThan(mStartTime);
    if (isEndBeforeStart) {
        wxMessageBox(wxT("A task cannot end before the time it has started"), wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
        return false;
    }

    auto isEndTimeInTheFuture = mEndTime.IsLaterThan(wxDateTime::Now());
    if (isEndTimeInTheFuture)
    {
        wxMessageBox(wxT("A task cannot end in the future"), wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
        return false;
    }

    auto taskTimeSpan = mEndTime - mStartTime;
    auto isTaskLessThan5Minutes = taskTimeSpan.IsShorterThan(wxTimeSpan::Minutes(5));
    if (isTaskLessThan5Minutes) {
        wxMessageBox(wxT("A task cannot be less than 5 minutes long"), wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
        return false;
    }

    auto isDescriptionInvalid = mDescriptionText.length() > 2048 || mDescriptionText.length() < 4 ||
        mDescriptionText.empty();
    if (isDescriptionInvalid) {
        wxMessageBox(wxT("Description is invalid"), wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
        return false;
    }

    auto isProjectChoiceValid = mProjectId != 0 || mProjectId != -1;
    if (!isProjectChoiceValid)
    {
        wxMessageBox(wxT("A project is required"), wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
        return false;
    }

    auto isCategoryChoiceValid = mCategoryId != 0 || mCategoryId != -1;
    if (!isCategoryChoiceValid)
    {
        wxMessageBox(wxT("A category is required"), wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
        return false;
    }

    return true;
}

bool TaskDetailsDialog::AreControlsEmpty()
{
    bool isEmpty = mProjectId == -1 &&
        mStartTime == wxDefaultDateTime &&
        mEndTime == wxDefaultDateTime &&
        mCategoryId == -1 &&
        mDescriptionText.empty();
    return isEmpty;
}

void TaskDetailsDialog::OnProjectChoice(wxCommandEvent& event)
{
    pCategoryChoiceCtrl->Clear();
    pCategoryChoiceCtrl->AppendString(wxT("Select a category"));
    pCategoryChoiceCtrl->SetSelection(0);
    int projectId = (int)event.GetClientData(); // FIXME: loss of precision -> convert to intptr_t and then to int

    FillCategoryControl(projectId);
}

void TaskDetailsDialog::OnStartTimeChange(wxDateEvent& event)
{
    auto start = event.GetDate();
    auto end = pEndTimeCtrl->GetValue();
    if (end != wxDefaultDateTime) {
        CaclulateTimeDiff(start, end);
    }
}

void TaskDetailsDialog::OnEndTimeChange(wxDateEvent& event)
{
    auto end = event.GetDate();
    auto start = pStartTimeCtrl->GetValue();

    if (start != wxDefaultDateTime) {
        CaclulateTimeDiff(start, end);
    }
}

void TaskDetailsDialog::OnIsActiveCheck(wxCommandEvent& event)
{
    if (event.IsChecked()) {
        pProjectChoiceCtrl->Enable();
        pStartTimeCtrl->Enable();
        pEndTimeCtrl->Enable();
        pDurationCtrl->Enable();
        pDescriptionCtrl->Enable();
        pCategoryChoiceCtrl->Enable();
    } else {
        pProjectChoiceCtrl->Disable();
        pStartTimeCtrl->Disable();
        pEndTimeCtrl->Disable();
        pDurationCtrl->Disable();
        pDescriptionCtrl->Disable();
        pCategoryChoiceCtrl->Disable();
    }
}

void TaskDetailsDialog::OnSave(wxCommandEvent& event)
{
    mProjectId = (int)pProjectChoiceCtrl->GetClientData(pProjectChoiceCtrl->GetSelection()); // FIXME: loss of precision -> convert to intptr_t and then to int
    mStartTime = pStartTimeCtrl->GetValue();
    mEndTime = pEndTimeCtrl->GetValue();
    mDurationText = pDurationCtrl->GetLabelText();
    mCategoryId = (int)pCategoryChoiceCtrl->GetClientData(pCategoryChoiceCtrl->GetSelection()); // FIXME: loss of precision -> convert to intptr_t and then to int
    mDescriptionText = pDescriptionCtrl->GetValue();

    auto validationSuccess = Validate();
    if (!validationSuccess) {
        return;
    }

    int taskId = GetTaskId();

    services::db_service dbService;
    try {
        wxString startTime = mStartTime.FormatISOTime();
        wxString endTime = mEndTime.FormatISOTime();
        if (bIsEdit && pIsActiveCtrl->IsChecked()) {
            models::task_detail taskDetail;
            taskDetail.task_detail_id = mTaskDetailId;
            taskDetail.start_time = startTime;
            taskDetail.end_time = endTime;
            taskDetail.duration = mDurationText;
            taskDetail.description = std::string(mDescriptionText.ToUTF8());
            taskDetail.date_modified_utc = util::UnixTimestamp();
            taskDetail.project_id = mProjectId;
            taskDetail.category_id = mCategoryId;
            dbService.update_task_detail(taskDetail);
        } else if (bIsEdit && !pIsActiveCtrl->IsChecked()) {
            dbService.delete_task_detail(mTaskDetailId);
        } else {
            dbService.create_new_task(mProjectId, taskId, std::string(startTime.ToUTF8()), std::string(endTime.ToUTF8()), std::string(mDurationText.ToUTF8()), mCategoryId, std::string(mDescriptionText.ToUTF8()));
        }
    } catch (const db::database_exception& e) {
        // TODO Log exception
    }

    OnTaskSaved(event);

    EndModal(ids::ID_SAVE);
}

void TaskDetailsDialog::OnCancel(wxCommandEvent& event)
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

void TaskDetailsDialog::OnTaskSaved(wxCommandEvent& event)
{
    wxCommandEvent eventForParent(ids::ID_TASK_INSERTED);
    eventForParent.SetEventObject(this);
    pParent->ProcessWindowEvent(eventForParent);
}

void TaskDetailsDialog::CaclulateTimeDiff(wxDateTime start, wxDateTime end)
{
    auto diff = end.Subtract(start);
    auto formated = diff.Format(wxT("%H:%M:%S"));
    pDurationCtrl->SetLabelText(formated);
}

void TaskDetailsDialog::FillCategoryControl(int projectId)
{
    std::vector<models::category> categories;
    try {
        services::db_service dbService;
        categories = dbService.get_categories_by_project_id(projectId);
    } catch (const db::database_exception& e) {
        // TODO Log exception
    }

    for (auto category : categories) {
        pCategoryChoiceCtrl->Append(category.category_name, (void*)category.category_id);
    }

    if (!pCategoryChoiceCtrl->IsEnabled()) {
        pCategoryChoiceCtrl->Enable();
    }
}

} // namespace app::dialog
