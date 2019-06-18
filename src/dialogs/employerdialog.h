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
class EmployerDialog final : public wxDialog
{
public:
    EmployerDialog() = default;
    explicit EmployerDialog(wxWindow* parent, bool isEdit = false, int employerId = 0, const wxString & name = "EmployerDialog");
    virtual ~EmployerDialog() = default;

private:
    wxDECLARE_DYNAMIC_CLASS(EmployerDialog);
    wxDECLARE_EVENT_TABLE();

    bool Create(wxWindow* parent, wxWindowID windowId, const wxString& title, const wxPoint& point, const wxSize& size, long style, const wxString& name);

    void CreateControls();
    void DataToControls();

    bool Validate();
    bool AreControlsEmpty();

    void OnSave(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnIsActiveCheck(wxCommandEvent& event);

    wxTextCtrl* pEmployerCtrl;
    wxCheckBox* pIsActiveCtrl;
    wxStaticText* pDateCreatedTextCtrl;
    wxStaticText* pDateUpdatedTextCtrl;

    wxString mNameText;
    bool bIsEdit;
    int mEmployerId;

    enum
    {
        IDC_ISACTIVE,
        WIDTH = 330,
        HEIGHT = 300,
        MIN_WIDTH = 320,
        MIN_HEIGHT = 240
    };
};
} // namespace app::dialog
