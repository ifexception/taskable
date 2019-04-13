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

namespace app::frame
{
class main_frame : public wxFrame
{
public:
    main_frame(wxWindow* parent, const wxString& name = wxT("main_frame"));
    main_frame(const main_frame&) = delete;
    virtual ~main_frame();

    main_frame& operator=(const main_frame&) = delete;

private:
    wxDECLARE_EVENT_TABLE();
    bool create(/*wxWindow* parent, wxWindowID windowId, const wxString& title, const wxPoint& point, const wxSize& size, long style, const wxString& name*/);

    void create_controls();
    void data_to_controls();

    void on_about(wxCommandEvent& event);
    void on_close(wxCommandEvent& event);
    void on_new_task(wxCommandEvent& event);
    void on_new_employer(wxCommandEvent& event);
    void on_new_client(wxCommandEvent& event);
    void on_new_project(wxCommandEvent& event);
    void on_new_category(wxCommandEvent& event);

    wxListCtrl* pListCtrl;

};
} // namespace app::frame
