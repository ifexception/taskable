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
class category_dialog : public wxDialog
{
public:
    category_dialog() = default;
    explicit category_dialog(wxWindow* parent, bool isEdit = false, int categoryId = 0, const wxString& name = "category_dialog");
    virtual ~category_dialog();

    void launch_dialog();

private:
    wxDECLARE_DYNAMIC_CLASS(category_dialog);
    wxDECLARE_EVENT_TABLE();

    bool create(wxWindow* parent, wxWindowID windowId, const wxString& title, const wxPoint& position, const wxSize& size, long style, const wxString& name);
    void create_controls();
    void fill_controls();
    void data_to_controls();

    bool validate();
    bool are_controls_empty();

    void on_save(wxCommandEvent &event);
    void on_cancel(wxCommandEvent &event);
    void on_is_active_check(wxCommandEvent& event);

    wxChoice* pProjectChoiceCtrl;
    wxTextCtrl* pNameCtrl;
    wxTextCtrl* pDescriptionCtrl;
    wxCheckBox* pIsActiveCtrl;
    wxStaticText* pDateCreatedTextCtrl;
    wxStaticText* pDateUpdatedTextCtrl;

    int mProjectChoiceId;
    wxString mNameText;
    wxString mDescriptionText;
    bool bIsEdit;
    int mCategoryId;

    enum
    {
        IDC_PROJECTCHOICE,
        IDC_NAME,
        IDC_DESCRIPTION,
        IDC_ISACTIVE
    };
};
} // app::dialog
