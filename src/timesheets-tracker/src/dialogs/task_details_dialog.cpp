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

#include "task_details_dialog.h"
#include <wx/timectrl.h>
#include <wx/statline.h>

#include "../common/common.h"
#include "../common/ids.h"

#include "../services/db_service.h"

namespace app::dialog
{
wxIMPLEMENT_DYNAMIC_CLASS(task_details_dialog, wxDialog);

wxBEGIN_EVENT_TABLE(task_details_dialog, wxDialog)
EVT_BUTTON(ids::ID_SAVE, task_details_dialog::on_save)
EVT_BUTTON(wxID_CANCEL, task_details_dialog::on_cancel)
wxEND_EVENT_TABLE()

task_details_dialog::task_details_dialog(wxWindow* parent, bool isEdit, const wxString& name)
    : mProjectId(-1)
    , mStartTime()
    , mEndTime()
    , mCategoryId(-1)
    , mDescriptionText(wxT(""))
{
    wxString title;
    if (isEdit) {
        title = wxT("Edit Task");
    } else {
        title = wxT("Add Task");
    }
    bool success = create(parent, wxID_ANY, wxT("Add New Task"), wxDefaultPosition, wxSize(385, 488), wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU, name);

    SetMinClientSize(wxSize(400, 480));
}

task_details_dialog::~task_details_dialog()
{
    Destroy();
}

void task_details_dialog::launch_task_details_dialog()
{
    ShowModal();
}

bool task_details_dialog::create(wxWindow* parent, wxWindowID windowId, const wxString& title, const wxPoint& point, const wxSize& size, long style, const wxString& name)
{
    bool created = wxDialog::Create(parent, windowId, title, point, size, style, name);

    if (created) {
        create_controls();

        GetSizer()->Fit(this);
        //SetIcon();
        Centre();
    }

    return created;
}

// TODO Refactor
void task_details_dialog::create_controls()
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

    /* Task Category Dropdown Control */
    auto taskCategory = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT("Category"));
    taskFlexGridSizer->Add(taskCategory, common::sizers::ControlCenterVertical);

    pCategoryChoiceCtrl = new wxChoice(taskDetailsPanel, IDC_CATEGORYCHOICE, wxDefaultPosition, wxSize(150, -1));
    pCategoryChoiceCtrl->AppendString(wxT("Select a category"));
    pCategoryChoiceCtrl->SetSelection(0);
    taskFlexGridSizer->Add(pCategoryChoiceCtrl, common::sizers::ControlDefault);

    /* Task Description Text Control */
    auto taskDescription = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT("Description"));
    taskFlexGridSizer->Add(taskDescription, common::sizers::ControlDefault);

    pDescriptionCtrl = new wxTextCtrl(this, IDC_DESCRIPTION, wxGetEmptyString(), wxDefaultPosition, wxSize(320, 180), wxTE_MULTILINE, wxDefaultValidator, wxT("description_text_ctrl"));
    pDescriptionCtrl->SetToolTip(wxT("Enter a description for the task"));
    detailsBoxSizer->Add(pDescriptionCtrl, 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    /* Horizontal Line*/
    auto separation_line = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL, wxT("task_static_line"));
    mainSizer->Add(separation_line, 0, wxEXPAND | wxALL, 1);

    /* Button Panel */
    auto buttonPanel = new wxPanel(this, wxID_STATIC);
    auto buttonPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonPanel->SetSizer(buttonPanelSizer);
    mainSizer->Add(buttonPanel, wxSizerFlags(wxSizerFlags().Border(wxALL, 5)).Center());

    auto okButton = new wxButton(buttonPanel, ids::ID_SAVE, wxT("&Save"));
    auto cancelButton = new wxButton(buttonPanel, wxID_CANCEL, wxT("&Cancel"));

    buttonPanelSizer->Add(okButton, common::sizers::ControlDefault);
    buttonPanelSizer->Add(cancelButton, common::sizers::ControlDefault);
}

void task_details_dialog::fill_controls()
{}

bool task_details_dialog::validate()
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

    auto taskTimeSpan = mEndTime - mStartTime;
    auto fiveMinuteTimeSpan = wxTimeSpan::Minutes(5);
    auto isTaskLessThan5Minutes = taskTimeSpan.IsShorterThan(fiveMinuteTimeSpan);
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

    return true;
}

bool task_details_dialog::are_controls_empty()
{
    bool isEmpty = mProjectId == -1 &&
        mStartTime == wxDefaultDateTime &&
        mEndTime == wxDefaultDateTime &&
        mCategoryId == -1 &&
        mDescriptionText.empty();
    return isEmpty;
}

void task_details_dialog::on_save(wxCommandEvent& event)
{
    mProjectId = pProjectChoiceCtrl->GetCurrentSelection();
    mStartTime = pStartTimeCtrl->GetValue();
    mEndTime = pEndTimeCtrl->GetValue();
    mCategoryId = pCategoryChoiceCtrl->GetCurrentSelection();
    mDescriptionText = pDescriptionCtrl->GetValue();

    auto validationSuccess = validate();
    if (!validationSuccess) {
        return;
    }
}

void task_details_dialog::on_cancel(wxCommandEvent& event)
{
    bool areControlsEmpty = are_controls_empty();
    if (!areControlsEmpty) {

        int answer = wxMessageBox(wxT("Are you sure you want to cancel?"), wxT("Confirm"), wxYES_NO | wxICON_QUESTION);
        if (answer == wxYES) {
            EndModal(wxID_CANCEL);
        }
    } else {
        EndModal(wxID_CANCEL);
    }
}

} // namespace app::dialog
