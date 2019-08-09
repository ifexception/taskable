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

#include <memory>

#include <wx/wx.h>
#define FMT_HEADER_ONLY
#include <spdlog/spdlog.h>

namespace app::dialog
{
class ClientDialog : public wxDialog
{
public:
    ClientDialog() = delete;
    explicit ClientDialog(wxWindow* parent,
        std::shared_ptr<spdlog::logger> logger,
        bool isEdit = false,
        int clientId = 0,
        const wxString& name = wxT("ClientDialog"));
    virtual ~ClientDialog() = default;

private:
    wxDECLARE_EVENT_TABLE();

    bool Create(wxWindow* parent,
        wxWindowID windowId,
        const wxString& title,
        const wxPoint& position,
        const wxSize& size,
        long style,
        const wxString& name);
    void CreateControls();
    void FillControls();
    void DataToControls();

    bool Validate();
    bool AreControlsEmpty();

    void OnSave(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnIsActiveCheck(wxCommandEvent& event);

    std::shared_ptr<spdlog::logger> pLogger;
    wxTextCtrl* pNameCtrl;
    wxComboBox* pEmployerChoiceCtrl;
    wxCheckBox* pIsActiveCtrl;
    wxStaticText* pDateCreatedTextCtrl;
    wxStaticText* pDateUpdatedTextCtrl;

    wxString mNameText;
    int mEmployerId;
    bool bIsEdit;
    int mClientId;

    enum { IDC_NAME, IDC_EMPLOYER, IDC_ISACTIVE };
};
} // namespace app::dialog
