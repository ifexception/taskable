// Productivity tool to help you track the time you spend on tasks
// Copyright (C) 2023  Szymon Welgus
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
//  Contact:
//    szymonwelgus at gmail dot com

#pragma once

#include <memory>
#include <vector>

#include <wx/wx.h>
#include <wx/combobox.h>

#include <spdlog/spdlog.h>

#include "../models/projectmodel.h"

#include "../data/clientdata.h"
#include "../data/projectdata.h"

namespace app::dlg
{
class ProjectDialog final : public wxDialog
{
public:
    ProjectDialog() = default;
    explicit ProjectDialog(wxWindow* parent,
        std::shared_ptr<spdlog::logger> logger,
        const wxString& name = "projectdlg");
    explicit ProjectDialog(wxWindow* parent,
        std::shared_ptr<spdlog::logger> logger,
        bool isEdit,
        int projectId,
        const wxString& name = "projectdlg");
    virtual ~ProjectDialog() = default;

private:
    bool Create(wxWindow* parent,
        wxWindowID windowId,
        const wxString& title,
        const wxPoint& position,
        const wxSize& size,
        long style,
        const wxString& name);

    void CreateControls();
    void ConfigureEventBindings();
    void FillControls();
    void DataToControls();

    void OnNameChange(wxCommandEvent& event);
    void OnEmployerChoiceSelection(wxCommandEvent& event);
    void OnIsDefaultCheck(wxCommandEvent& event);

    void OnOk(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnIsActiveCheck(wxCommandEvent& event);

    bool TryTransferValuesFromControls();

    std::shared_ptr<spdlog::logger> pLogger;

    wxTextCtrl* pNameTextCtrl;
    wxTextCtrl* pDisplayNameCtrl;
    wxChoice* pEmployerChoiceCtrl;
    wxChoice* pClientChoiceCtrl;
    wxCheckBox* pIsActiveCtrl;
    wxCheckBox* pIsDefaultCtrl;
    wxStaticText* pDateTextCtrl;
    wxButton* pOkButton;
    wxButton* pCancelButton;

    std::unique_ptr<model::ProjectModel> pProject;
    int mProjectId;
    bool bIsEdit;

    data::ClientData mClientData;
    data::ProjectData mProjectData;

    enum {
        IDC_NAME = wxID_HIGHEST + 1,
        IDC_DISPLAYNAME,
        IDC_EMPLOYERCHOICE,
        IDC_CLIENTCHOICE,
        IDC_ISDEFAULT,
        IDC_ISACTIVE
    };
};
} // namespace app::dlg
