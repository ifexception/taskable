// Productivity tool to help you track the time you spend on tasks
// Copyright (C) 2020  Szymon Welgus
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

#include <wx/wx.h>

#include <spdlog/spdlog.h>

#include "../models/clientmodel.h"

namespace app::dlg
{
class ClientDialog : public wxDialog
{
public:
    ClientDialog() = delete;
    explicit ClientDialog(wxWindow* parent,
        std::shared_ptr<spdlog::logger> logger,
        const wxString& name = wxT("clientdlg"));
    explicit ClientDialog(wxWindow* parent,
        std::shared_ptr<spdlog::logger> logger,
        bool isEdit,
        int clientId,
        const wxString& name = wxT("clientdlg"));
    virtual ~ClientDialog() = default;

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

    bool TransferDataAndValidate();

    void OnOk(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnIsActiveCheck(wxCommandEvent& event);

    std::shared_ptr<spdlog::logger> pLogger;

    wxTextCtrl* pNameTextCtrl;
    wxChoice* pEmployerChoiceCtrl;
    wxCheckBox* pIsActiveCtrl;
    wxStaticText* pDateTextCtrl;
    wxButton* pOkButton;
    wxButton* pCancelButton;

    std::unique_ptr<model::ClientModel> pClient;
    int mClientId;
    bool bIsEdit;

    enum { IDC_NAME = wxID_HIGHEST + 1, IDC_EMPLOYERCHOICE, IDC_ISACTIVE };

    static const wxString& DateLabel;
};
} // namespace app::dlg
