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

#include "new_task_dialog.hh"
#include <wx/timectrl.h>
#include <wx/statline.h>
#include <wx/datectrl.h>

namespace app::dialog
{
wxIMPLEMENT_DYNAMIC_CLASS(new_task_dialog, wxDialog);

wxBEGIN_EVENT_TABLE(new_task_dialog, wxDialog)
    EVT_BUTTON(SaveTaskId, new_task_dialog::on_save)
    EVT_BUTTON(wxID_CANCEL, new_task_dialog::on_cancel)
wxEND_EVENT_TABLE()

new_task_dialog::new_task_dialog(wxWindow* parent, const wxString& name)
{
    bool success = create(parent,
        wxID_ANY,
        wxT("Add a New Task"),
        wxDefaultPosition,
        wxSize(400, 500),
        wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU,
        name);

    SetMinClientSize(wxSize(400, 380));
}

new_task_dialog::~new_task_dialog()
{
    Destroy();
}

bool new_task_dialog::create(wxWindow* parent,
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

        Centre();
        ShowModal();
    }

    return created;
}

void new_task_dialog::create_controls()
{
    const wxSizerFlags g_flagsV = wxSizerFlags().Border(wxALL, 5);
    const wxSizerFlags g_flagsH = wxSizerFlags(g_flagsV).Align(wxALIGN_CENTER_VERTICAL);
    const wxSizerFlags g_flagsCenter = wxSizerFlags(g_flagsV).Align(wxALIGN_CENTER);
    const wxSizerFlags g_flagsExpand = wxSizerFlags(g_flagsV).Align(wxEXPAND).Proportion(1);

    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    auto mainPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(mainPanelSizer, wxSizerFlags().Border(wxALL, 5));

    auto sizer = new wxBoxSizer(wxVERTICAL);
    mainPanelSizer->Add(sizer, 0);

    auto detailsBox = new wxStaticBox(this, wxID_ANY, wxT("Task Details"));
    auto detailsBoxSizer = new wxStaticBoxSizer(detailsBox, wxVERTICAL);
    sizer->Add(detailsBoxSizer, wxSizerFlags().Border(wxALL, 5));

    auto taskDetailsPanel = new wxPanel(this, wxID_STATIC);
    detailsBoxSizer->Add(taskDetailsPanel, wxSizerFlags().Border(wxALL, 5).Expand());

    auto taskFlexGridSizer = new wxFlexGridSizer(0, 2, 0, 0);
    taskDetailsPanel->SetSizer(taskFlexGridSizer);

    auto activeProject = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT("Project"));
    taskFlexGridSizer->Add(activeProject, wxSizerFlags().Border(wxALL, 5).CenterVertical());

    wxArrayString tmpProjects;
    tmpProjects.Add(wxT("Silica"));
    tmpProjects.Add(wxT("Entelect"));
    tmpProjects.Add(wxT("Social"));

    pActiveProject = new wxComboBox(taskDetailsPanel, wxID_ANY, wxT("Silica"), wxDefaultPosition,
                                wxDefaultSize, tmpProjects, wxCB_DROPDOWN);
    taskFlexGridSizer->Add(pActiveProject, wxSizerFlags().Border(wxALL, 5).Align(wxALIGN_RIGHT));

    auto taskStartTime = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT("Start Time"));
    taskFlexGridSizer->Add(taskStartTime,
                           wxSizerFlags().Border(wxALL, 5).Align(wxALIGN_CENTER_VERTICAL));

    pStartTime = new wxTimePickerCtrl(taskDetailsPanel, wxID_ANY, wxDefaultDateTime,
                                      wxDefaultPosition, wxSize(150, -1));
    pStartTime->SetToolTip(wxT("Enter the time when the task started"));
    taskFlexGridSizer->Add(pStartTime, wxSizerFlags().Border(wxALL, 5).Align(wxALIGN_RIGHT));

    auto taskEndTime = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT("End Time"));
    taskFlexGridSizer->Add(taskEndTime, wxSizerFlags().Border(wxALL, 5).CenterVertical());

    pEndTime = new wxTimePickerCtrl(taskDetailsPanel, wxID_ANY, wxDefaultDateTime,
                                    wxDefaultPosition, wxSize(150, -1));
    pEndTime->SetToolTip(wxT("Enter the time the task ended"));
    taskFlexGridSizer->Add(pEndTime, wxSizerFlags().Border(wxALL, 5).Align(wxALIGN_RIGHT));

    auto taskDescription = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT("Description"));
    taskFlexGridSizer->Add(taskDescription, wxSizerFlags().Border(wxALL, 5).Align(wxALIGN_RIGHT));

    pDescription = new wxTextCtrl(taskDetailsPanel, wxID_ANY, wxGetEmptyString(), wxDefaultPosition,
                                  wxSize(220, 170), wxTE_MULTILINE);
    pDescription->SetToolTip(wxT("Enter a description for the task"));
    taskFlexGridSizer->Add(pDescription, 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 5);

    auto taskCategory = new wxStaticText(taskDetailsPanel, wxID_STATIC, wxT("Category"));
    taskFlexGridSizer->Add(taskCategory, wxSizerFlags().Border(wxALL, 5).Align(wxALIGN_RIGHT));

    wxArrayString tmpCategories;
    tmpCategories.Add(wxT("Programming"));
    tmpCategories.Add(wxT("Meeting"));
    tmpCategories.Add(wxT("Analysis"));

    pCategories = new wxComboBox(taskDetailsPanel, wxID_ANY, wxT("Programming"), wxDefaultPosition,
                                 wxDefaultSize, tmpCategories, wxCB_DROPDOWN);
    taskFlexGridSizer->Add(pCategories, wxSizerFlags().Border(wxALL, 5).Align(wxALIGN_RIGHT));

    /* Horizontal Line*/
    auto separation_line =
        new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
    mainSizer->Add(separation_line, 0, wxEXPAND | wxALL, 1);

    /********************************************************************
    Button Panel
    *********************************************************************/
    wxPanel* button_panel = new wxPanel(this, wxID_STATIC);
    wxBoxSizer* button_panel_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* ok_button = new wxButton(button_panel, SaveTaskId, wxT("&Save "));
    wxButton* cancel_button = new wxButton(button_panel, wxID_CANCEL, wxT("&Cancel"));

    mainSizer->Add(button_panel, wxSizerFlags(wxSizerFlags().Border(wxALL, 5)).Center());
    button_panel->SetSizer(button_panel_sizer);
    button_panel_sizer->Add(ok_button, wxSizerFlags().Border(wxALL, 5));

    button_panel_sizer->Add(cancel_button, wxSizerFlags().Border(wxALL, 5));
}

// void new_task_dialog::create_controls()
//{
//    wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
//    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
//
//    wxPanel* panel = new wxPanel(this);
//    wxStaticBox* boundedTimesBox = new wxStaticBox(panel,
//        wxID_ANY,
//        wxT("Task Duration"),
//        wxPoint(2, 4),
//        wxSize(300, 200));
//    vbox->Add(panel, 1);
//
//    pOkButton = new wxButton(this,
//        SaveTaskId,
//        wxT("&Save"));
//    hbox->Add(pOkButton, 1);
//
//    pCancelButton = new wxButton(this,
//        wxID_CANCEL,
//        wxT("&Cancel"));
//    hbox->Add(pCancelButton, 1);
//
//    vbox->Add(hbox, 0, wxALIGN_RIGHT | wxTOP | wxBOTTOM, 10);
//    SetSizer(vbox);
//}

void new_task_dialog::on_save(wxCommandEvent& event)
{
    int i = 0;
}

void new_task_dialog::on_cancel(wxCommandEvent& event)
{
    int answer = wxMessageBox(wxT("Are you sure you want to exit?"), wxT("Confirm"), wxYES_NO | wxICON_QUESTION);
    if (answer == wxYES) {
        Destroy();
    }
}
} // namespace app::dialog
