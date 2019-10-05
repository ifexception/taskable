//  Tasks Tracker is a desktop that aids you in tracking your timesheets
//  and seeing what work you have done.
//
//  Copyright(C) <2019> <Szymon Welgus>
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
class ProjectDialog final : public wxDialog
{
public:
    ProjectDialog() = default;
    explicit ProjectDialog(wxWindow* parent,
        std::shared_ptr<spdlog::logger> logger,
        const wxString& name = "ProjectDialog");
    explicit ProjectDialog(wxWindow* parent,
        std::shared_ptr<spdlog::logger> logger,
        bool isEdit,
        int projectId,
        const wxString& name = "ProjectDialog");
    virtual ~ProjectDialog() = default;

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

    void OnProjectNameLostFocus(wxFocusEvent& event);
    void OnDisplayNameLostFocus(wxFocusEvent& event);
    void OnEmployerSelect(wxCommandEvent& event);
    void OnNameTextEntered(wxCommandEvent& event);
    void OnOk(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnIsActiveCheck(wxCommandEvent& event);

    std::shared_ptr<spdlog::logger> pLogger;

    wxButton* pOkButton;
    wxTextCtrl* pNameCtrl;
    wxTextCtrl* pDisplayNameCtrl;
    wxChoice* pEmployerChoiceCtrl;
    wxChoice* pClientChoiceCtrl;
    wxCheckBox* pIsActiveCtrl;
    wxStaticText* pDateCreatedTextCtrl;
    wxStaticText* pDateUpdatedTextCtrl;

    wxString mNameText;
    wxString mDisplayNameText;
    int mEmployerId;
    int mClientId;
    int mProjectId;
    bool bIsEdit;

    enum { IDC_NAME = wxID_HIGHEST + 1, IDC_DISPLAYNAME, IDC_EMPLOYERCHOICE, IDC_CLIENTCHOICE, IDC_ISACTIVE };
};
} // namespace app::dialog
