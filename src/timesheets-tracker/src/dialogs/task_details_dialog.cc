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

#include "task_details_dialog.hh"
#include <wx/timectrl.h>
#include <wx/statline.h>

namespace app::dialog
{
wxIMPLEMENT_DYNAMIC_CLASS(task_details_dialog, wxDialog);

wxBEGIN_EVENT_TABLE(task_details_dialog, wxDialog)
    EVT_BUTTON(ID_SAVETASK, task_details_dialog::on_save)
    EVT_BUTTON(wxID_CANCEL, task_details_dialog::on_cancel)
    EVT_BUTTON(wxID_EXIT, task_details_dialog::on_exit)
wxEND_EVENT_TABLE()

task_details_dialog::task_details_dialog(wxWindow* parent, const wxString& name)
    : mSelectedProject(-1)
    , mStartTime()
    , mEndTime()
    , mSelectedCategory(-1)
    , mDescriptionText(wxT(""))
{
    bool success = create(parent,
        wxID_ANY,
        wxT("Add New Task"),
        wxDefaultPosition,
        wxSize(385, 488),
        wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU,
        name);

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

bool task_details_dialog::create(wxWindow* parent,
    wxWindowID windowId,
    const wxString& title,
    const wxPoint& point,
    const wxSize& size,
    long style,
    const wxString& name)
{
    bool created = wxDialog::Create(parent,
        windowId,
        title,
        point,
        size,
        style,
        name);

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
    const wxSizerFlags g_flagsV = wxSizerFlags().Border(wxALL, 5);
    const wxSizerFlags g_flagsH = wxSizerFlags(g_flagsV).Align(wxALIGN_CENTER_VERTICAL);
    const wxSizerFlags g_flagsCenter = wxSizerFlags(g_flagsV).Align(wxALIGN_CENTER);
    const wxSizerFlags g_flagsExpand = wxSizerFlags(g_flagsV).Align(wxEXPAND).Proportion(1);

    /* Window Sizing */
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    auto mainPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(mainPanelSizer, wxSizerFlags().Border(wxALL, 5));

    auto sizer = new wxBoxSizer(wxVERTICAL);
    mainPanelSizer->Add(sizer, 0);

    /* Task Details Box */
    auto detailsBox = new wxStaticBox(this, wxID_ANY, wxT("Task Details"));
    auto detailsBoxSizer = new wxStaticBoxSizer(detailsBox, wxVERTICAL);
    sizer->Add(detailsBoxSizer, g_flagsExpand);

    auto taskDetailsPanel = new wxPanel(this, wxID_STATIC);
    detailsBoxSizer->Add(taskDetailsPanel, wxSizerFlags().Border(wxALL, 5).Expand());

    auto taskFlexGridSizer = new wxFlexGridSizer(0, 2, 0, 0);
    taskDetailsPanel->SetSizer(taskFlexGridSizer);

    /* ---Controls--- */
    /* Active Project Dropdown Control */
    auto activeProject = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT("Project"));
    taskFlexGridSizer->Add(activeProject, wxSizerFlags().Border(wxALL, 5).CenterVertical());

    wxArrayString tmpProjects;
    tmpProjects.Add(wxT("Select a project"));
    tmpProjects.Add(wxT("Silica"));
    tmpProjects.Add(wxT("Entelect"));
    tmpProjects.Add(wxT("Social"));

    pActiveProject = new wxComboBox(taskDetailsPanel, wxID_ANY, wxT("Select a project"), wxDefaultPosition, wxDefaultSize, tmpProjects, wxCB_DROPDOWN);
    pActiveProject->SetToolTip(wxT("Select the project to associate this task with"));
    taskFlexGridSizer->Add(pActiveProject, wxSizerFlags().Border(wxALL, 5));

    /* Task Start Time Control */
    auto taskStartTime = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT("Start Time"));
    taskFlexGridSizer->Add(taskStartTime, wxSizerFlags().Border(wxALL, 5).CenterVertical());

    pStartTime = new wxTimePickerCtrl(taskDetailsPanel, IDC_STARTTIME, wxDefaultDateTime, wxDefaultPosition, wxSize(150, -1));
    pStartTime->SetToolTip(wxT("Enter the time when the task started"));
    taskFlexGridSizer->Add(pStartTime, wxSizerFlags().Border(wxALL, 5));

    /* Task End Time Control */
    auto taskEndTime = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT("End Time"));
    taskFlexGridSizer->Add(taskEndTime, wxSizerFlags().Border(wxALL, 5).CenterVertical());

    pEndTime = new wxTimePickerCtrl(taskDetailsPanel, wxID_ANY, wxDefaultDateTime, wxDefaultPosition, wxSize(150, -1));
    pEndTime->SetToolTip(wxT("Enter the time the task ended"));
    taskFlexGridSizer->Add(pEndTime, wxSizerFlags().Border(wxALL, 5));

    /* Task Category Dropdown Control */
    auto taskCategory = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT("Category"));
    taskFlexGridSizer->Add(taskCategory, wxSizerFlags().Border(wxALL, 5).CenterVertical());

    wxArrayString tmpCategories;
    tmpCategories.Add(wxT("Select a category"));
    tmpCategories.Add(wxT("Programming"));
    tmpCategories.Add(wxT("Meeting"));
    tmpCategories.Add(wxT("Analysis"));

    pCategories = new wxComboBox(taskDetailsPanel, wxID_ANY, wxT("Select a category"), wxDefaultPosition, wxDefaultSize, tmpCategories, wxCB_DROPDOWN);
    taskFlexGridSizer->Add(pCategories, wxSizerFlags().Border(wxALL, 5));

    /* Task Description Text Control */
    auto taskDescription = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT("Description"));
    taskFlexGridSizer->Add(taskDescription, wxSizerFlags().Border(wxALL, 5));

    auto filters = wxFILTER_NONE;
    wxTextValidator descriptionValidator(filters);

    pDescription = new wxTextCtrl(this, wxID_ANY, wxGetEmptyString(), wxDefaultPosition, wxSize(320, 180), wxTE_MULTILINE, descriptionValidator, wxT("description_text_ctrl"));
    pDescription->SetToolTip(wxT("Enter a description for the task"));
    detailsBoxSizer->Add(pDescription, 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    /* Horizontal Line*/
    auto separation_line = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL, wxT("new_task_static_line"));
    mainSizer->Add(separation_line, 0, wxEXPAND | wxALL, 1);

    /* Button Panel */
    auto buttonPanel = new wxPanel(this, wxID_STATIC);
    auto buttonPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonPanel->SetSizer(buttonPanelSizer);
    mainSizer->Add(buttonPanel, wxSizerFlags(wxSizerFlags().Border(wxALL, 5)).Center());

    auto okButton = new wxButton(buttonPanel, ID_SAVETASK, wxT("&Save "));
    auto cancelButton = new wxButton(buttonPanel, wxID_CANCEL, wxT("&Cancel"));

    buttonPanelSizer->Add(okButton, wxSizerFlags().Border(wxALL, 5));
    buttonPanelSizer->Add(cancelButton, wxSizerFlags().Border(wxALL, 5));
}

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
    bool isEmpty = mSelectedProject == -1 &&
        mStartTime == wxDefaultDateTime &&
        mEndTime == wxDefaultDateTime &&
        mSelectedCategory == -1 &&
        mDescriptionText.empty();
    return isEmpty;
}

void task_details_dialog::on_save(wxCommandEvent& event)
{
    mSelectedProject = pActiveProject->GetCurrentSelection();
    mStartTime = pStartTime->GetValue();
    mEndTime = pEndTime->GetValue();
    mSelectedCategory = pCategories->GetCurrentSelection();
    mDescriptionText = pDescription->GetValue();

    auto validationSuccess = validate();
    if (!validationSuccess) {
        return;
    }
}

void task_details_dialog::on_cancel(wxCommandEvent& event)
{
    bool areControlsEmpty = are_controls_empty();
    if (!areControlsEmpty) {

        int answer = wxMessageBox(wxT("Are you sure you want to cancel?"), wxT("Confirm"),
                                  wxYES_NO | wxICON_QUESTION);
        if (answer == wxYES) {
            Destroy();
        }
    } else {
        Destroy();
    }
}
void task_details_dialog::on_exit(wxCommandEvent& event)
{
    bool areControlsEmpty = are_controls_empty();
    if (!areControlsEmpty) {

        int answer = wxMessageBox(wxT("Are you sure you want to exit?"), wxT("Confirm"),
                                  wxYES_NO | wxICON_QUESTION);
        if (answer == wxYES) {
            Destroy();
        }
    } else {
        Destroy();
    }
}

} // namespace app::dialog
