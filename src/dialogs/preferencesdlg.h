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

#include <spdlog/spdlog.h>
#include <sqlite_modern_cpp.h>
#include <wx/wx.h>
#include <wx/propdlg.h>

#include "preferencesgeneralpage.h"
#include "preferencesdatabasepage.h"
#include "preferencesstopwatchpage.h"
#include "preferencestaskitempage.h"

namespace app
{
namespace cfg
{
class Configuration;
}
namespace frm
{
class TaskBarIcon;
}
} // namespace app

namespace app::dlg
{
class PreferencesDialog final : public wxPropertySheetDialog
{
public:
    PreferencesDialog() = delete;
    PreferencesDialog(wxWindow* parent,
        std::shared_ptr<cfg::Configuration> config,
        std::shared_ptr<spdlog::logger> pLogger,
        frm::TaskBarIcon* taskBarIcon,
        const wxString& name = wxT("preferencesdlg"));
    virtual ~PreferencesDialog() = default;

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

    void OnOk(wxCommandEvent& event);

    std::shared_ptr<cfg::Configuration> pConfig;
    std::shared_ptr<spdlog::logger> pLogger;
    frm::TaskBarIcon* pTaskBarIcon;

    wxWindow* pParent;
    GeneralPage* pGeneralPage;
    DatabasePage* pDatabasePage;
    StopwatchPage* pStopwatchPage;
    TaskItemPage* pTaskItemPage;
};
} // namespace app::dlg
