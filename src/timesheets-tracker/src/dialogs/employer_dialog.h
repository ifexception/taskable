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

namespace app::dialog
{
class employer_dialog final : public wxDialog
{
    wxDECLARE_DYNAMIC_CLASS(employer_dialog);
    wxDECLARE_EVENT_TABLE();

  public:
    employer_dialog() = default;
    explicit employer_dialog(wxWindow* parent, bool isEdit = false, int employerId = 0, const wxString& name = "employer_dialog");
    virtual ~employer_dialog();

    void launch_employer_dialog();

  private:
    bool create(wxWindow* parent, wxWindowID windowId, const wxString& title, const wxPoint& point,
                const wxSize& size, long style, const wxString& name);

    void create_controls();

    bool validate();
    bool are_controls_empty();

    void on_save(wxCommandEvent& event);
    void on_cancel(wxCommandEvent& event);

    wxTextCtrl* pEmployerCtrl;

    wxString mEmployerText;
    bool bIsEdit;
    int mEmployerId;

    enum
    {
        WIDTH = 330,
        HEIGHT = 300,
        MIN_WIDTH = 320,
        MIN_HEIGHT = 240
    };
};
} // namespace app::dialog
