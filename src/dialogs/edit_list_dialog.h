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

#pragma once

#include <wx/wx.h>
#include <wx/listctrl.h>

namespace app::dialog
{
enum class dialog_type
{
    Employer,
    Client,
    Project,
    Category
};

class edit_list_dialog final : public wxDialog
{
public:
    edit_list_dialog() = default;
    explicit edit_list_dialog(wxWindow* parent, dialog_type dialogType, const wxString& name = wxT("edit_list_dialog"));
    virtual ~edit_list_dialog() = default;

    void launch_dialog();

private:
    wxDECLARE_DYNAMIC_CLASS(edit_list_dialog);
    wxDECLARE_EVENT_TABLE(edit_list_dialog);

    bool create(wxWindow* parent, wxWindowID windowId, const wxString& title, const wxPoint& position, const wxSize& size, long style, const wxString& name);

    void create_controls();
    void data_to_controls();

    void on_item_double_click(wxListEvent& event);

    wxListCtrl* make_list_control(wxPanel* panel);
    void make_employer_list_control(wxListCtrl* listCtrl);
    wxListCtrl* make_client_list_control(wxPanel* panel);
    wxListCtrl* make_project_list_control(wxPanel* panel);
    wxListCtrl* make_project_list_control(wxPanel* panel);

    wxListCtrl* pListCtrl;
    dialog_type mType;

    enum
    {
        IDC_LIST
    };
};
}
