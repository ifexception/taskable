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

class wxTimePickerCtrl;

namespace app::dialog
{
const int SaveTaskId = wxNewId();

class new_task_dialog : public wxDialog
{
    wxDECLARE_DYNAMIC_CLASS(new_task_dialog);
    wxDECLARE_EVENT_TABLE();

  public:
    new_task_dialog() = default;
    explicit new_task_dialog(wxWindow* parent,
        const wxString& name = "new_task_dialog");
    virtual ~new_task_dialog();

  private:
    bool create(wxWindow* parent,
        wxWindowID windowId,
        const wxString& title,
        const wxPoint& point,
        const wxSize& size,
        long style,
        const wxString& name);

    void create_controls();

    void on_save(wxCommandEvent& event);
    void on_cancel(wxCommandEvent& event);

    wxTimePickerCtrl* pStartTime;
    wxTimePickerCtrl* pEndTime;
};
} // namespace app::dialog
