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

#include "employer_list_dialog.h"

namespace app::dialog
{
wxIMPLEMENT_DYNAMIC_CLASS(employer_list_dialog, wxDialog);

wxBEGIN_EVENT_TABLE(employer_list_dialog, wxDialog)
EVT_LIST_ITEM_ACTIVATED(employer_list_dialog::IDC_LIST, employer_list_dialog::on_item_double_click)
wxEND_EVENT_TABLE()

employer_list_dialog::employer_list_dialog(wxWindow* parent, const wxString& name)
    : pListCtrl(nullptr)
{
    long style = wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU;
    wxString title = wxT("Select a Employer");
    bool success = create(parent, wxID_ANY, title, wxDefaultPosition, wxSize(320, 240), style, name);
    SetMinClientSize(wxSize(320, 240));
}

void employer_list_dialog::launch_dialog()
{
    ShowModal();
}

bool employer_list_dialog::create(wxWindow* parent, wxWindowID windowId, const wxString& title, const wxPoint& position, const wxSize& size, long style, const wxString& name)
{
    bool created = wxDialog::Create(parent, windowId, title, position, size, style, name);
    if (created) {
        create_controls();
        data_to_controls();

        GetSizer()->Fit(this);
        // SetIcon();
        Center();
    }

    return created;
}

void employer_list_dialog::create_controls()
{
    auto panel = new wxPanel(this);
    auto sizer = new wxBoxSizer(wxHORIZONTAL);

    pListCtrl = new wxListCtrl(panel, IDC_LIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_HRULES);
    sizer->Add(pListCtrl, 1, wxEXPAND | wxALL, 5);
    panel->SetSizer(sizer);

    wxListItem nameColumn;
    nameColumn.SetId(0);
    nameColumn.SetText(wxT("Employer"));
    nameColumn.SetWidth(100);
    pListCtrl->InsertColumn(0, nameColumn);

    wxListItem dateCreatedColumn;
    dateCreatedColumn.SetId(1);
    dateCreatedColumn.SetText(wxT("Date Created"));
    pListCtrl->InsertColumn(1, dateCreatedColumn);
}

void employer_list_dialog::data_to_controls()
{}

void employer_list_dialog::on_item_double_click(wxListEvent & event)
{}
}
