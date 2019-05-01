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

#include "edit_list_dialog.h"

#include "../magic_enum/magic_enum.h"

namespace app::dialog
{
wxIMPLEMENT_DYNAMIC_CLASS(edit_list_dialog, wxDialog);

wxBEGIN_EVENT_TABLE(edit_list_dialog, wxDialog)
wxEND_EVENT_TABLE()

edit_list_dialog::edit_list_dialog(wxWindow* parent, dialog_type dialogType, const wxString& name)
    : mType(dialogType)
{
    long style = wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU;
    auto type = magic_enum::enum_name(mType);
    wxString title = type.data;
    bool success = create(parent, wxID_ANY, title, wxDefaultPosition, wxSize(320, 260), style, name);
}

void edit_list_dialog::launch_dialog()
{
    ShowModal();
}

bool edit_list_dialog::create(wxWindow* parent, wxWindowID windowId, const wxString& title, const wxPoint& position, const wxSize& size, long style, const wxString& name)
{
    bool created = Create(parent, windowId, title, position, size, style, name);
    if (created) {
        create_controls();
        data_to_controls();

        GetSizer()->Fit(this);
        // SetIcon();
        Center();
    }

    return false;
}

void edit_list_dialog::create_controls()
{
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    auto panelSizer = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(panelSizer, wxSizerFlags().Border(wxALL, 5));

    auto panel = new wxPanel(this, wxID_STATIC);
    panelSizer->Add(panel, wxSizerFlags().Border(wxALL, 5).Expand());

    auto sizer = new wxBoxSizer(wxHORIZONTAL);
    panel->SetSizer(sizer);

    auto type = magic_enum::enum_name(mType);
    wxString name = type.data;
    pListCtrl = make_list_control(panel);
    sizer->Add(pListCtrl, 1, wxEXPAND | wxALL, 5);
}

wxListCtrl* edit_list_dialog::make_list_control(wxPanel* panel)
{
    wxListCtrl* listCtrl;
    switch (mType) {
    case dialog_type::Employer:
        listCtrl = new wxListCtrl(panel, IDC_LIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_HRULES);
        make_employer_list_control(listCtrl);
        break;
    case dialog_type::Client:
        break;
    case dialog_type::Project:
        break;
    case dialog_type::Category:
        break;
    default:
        break;
    }
    return listCtrl;
}

void edit_list_dialog::make_employer_list_control(wxListCtrl* listCtrl)
{
    auto type = magic_enum::enum_name(mType);
    wxString name = type.data;

    wxListItem nameColumn;
    nameColumn.SetId(0);
    nameColumn.SetText(name);
    nameColumn.SetWidth(100);
    listCtrl->InsertColumn(0, nameColumn);

    wxListItem dateCreatedColumn;
    dateCreatedColumn.SetId(1);
    dateCreatedColumn.SetText(wxT("Date Created"));
    listCtrl->InsertColumn(1, dateCreatedColumn);

    wxListItem dateModifiedColumn;
    dateModifiedColumn.SetId(2);
    dateModifiedColumn.SetText(wxT("Date Modified"));
    listCtrl->InsertColumn(2, dateModifiedColumn);
}
}
