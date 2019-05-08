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

struct strategy
{
public:
    strategy() = default;
    virtual ~strategy() = default;

    virtual void create_control(wxListCtrl* control) = 0;
    virtual void data_to_control(wxListCtrl* control) = 0;
    virtual wxSize get_size() = 0;
};

struct employer_strategy final : public strategy
{
public:
    employer_strategy() = default;
    virtual ~employer_strategy() = default;;

    void create_control(wxListCtrl* control) override;
    void data_to_control(wxListCtrl* control) override;
    wxSize get_size() override;
};

struct client_strategy final : public strategy
{
public:
    client_strategy() = default;
    virtual ~client_strategy() = default;;

    void create_control(wxListCtrl* control) override;
    void data_to_control(wxListCtrl* control) override;
    wxSize get_size() override;
};

struct project_strategy final : public strategy
{
public:
    project_strategy() = default;
    virtual ~project_strategy() = default;;

    void create_control(wxListCtrl* control) override;
    void data_to_control(wxListCtrl* control) override;
    wxSize get_size() override;
};

struct category_strategy final : public strategy
{
public:
    category_strategy() = default;
    virtual ~category_strategy() = default;;

    void create_control(wxListCtrl* control) override;
    void data_to_control(wxListCtrl* control) override;
    wxSize get_size() override;
};

class edit_list_dialog final : public wxDialog
{
public:
    edit_list_dialog() = default;
    explicit edit_list_dialog(wxWindow* parent, dialog_type dialogType, const wxString& name = wxT("edit_list_dialog"));
    virtual ~edit_list_dialog();

    void launch_dialog();

private:
    wxDECLARE_DYNAMIC_CLASS(edit_list_dialog);
    wxDECLARE_EVENT_TABLE();

    bool create(wxWindow* parent, wxWindowID windowId, const wxString& title, const wxPoint& position, const wxSize& size, long style, const wxString& name);

    void create_controls();
    void data_to_controls();

    void on_item_double_click(wxListEvent& event);

    void set_strategy();

    wxListCtrl* pListCtrl;
    strategy* mStrategy;
    dialog_type mType;

    enum
    {
        IDC_LIST
    };
};
}
