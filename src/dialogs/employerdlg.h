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

#include "../models/employermodel.h"
#include "../data/employerdata.h"

namespace app::dlg
{
class EmployerDialog final : public wxDialog
{
public:
    EmployerDialog() = default;
    explicit EmployerDialog(wxWindow* parent,
        std::shared_ptr<spdlog::logger> logger,
        const wxString& name = "employerdlg");
    explicit EmployerDialog(wxWindow* parent,
        std::shared_ptr<spdlog::logger> logger,
        bool isEdit,
        int employerId,
        const wxString& name = wxT("employerdlg"));
    virtual ~EmployerDialog() = default;

private:

    bool Create(wxWindow* parent,
        wxWindowID windowId,
        const wxString& title,
        const wxPoint& point,
        const wxSize& size,
        long style,
        const wxString& name);

    void CreateControls();
    void ConfigureEventBindings();
    void DataToControls();

    void OnOk(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnIsActiveCheck(wxCommandEvent& event);

    bool TransferDataAndValidate();

    std::shared_ptr<spdlog::logger> pLogger;

    wxTextCtrl* pNameTextCtrl;
    wxCheckBox* pIsActiveCtrl;
    wxStaticText* pDateTextCtrl;
    wxButton* pOkButton;
    wxButton* pCancelButton;

    bool bIsEdit;
    int mEmployerId;
    std::unique_ptr<model::EmployerModel> pEmployer;
    data::EmployerData mData;

    enum {
        IDC_EMPLOYERTEXT = wxID_HIGHEST + 1,
        IDC_ISACTIVE,
        WIDTH = 330,
        HEIGHT = 300,
        MIN_WIDTH = 320,
        MIN_HEIGHT = 240
    };

    static const wxString& DateLabel;
};
} // namespace app::dlg
