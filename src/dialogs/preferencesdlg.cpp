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

#include "preferencesdlg.h"

#include <wx/bookctrl.h>
#include <wx/listbook.h>
#include <wx/listctrl.h>

#include "../common/common.h"
#include "../config/configuration.h"
#include "../frame/taskbaricon.h"

namespace app::dlg
{
// clang-format off
wxBEGIN_EVENT_TABLE(PreferencesDialog, wxPropertySheetDialog)
EVT_BUTTON(wxID_OK, PreferencesDialog::OnOk)
wxEND_EVENT_TABLE()

PreferencesDialog::PreferencesDialog(wxWindow* parent,
    std::shared_ptr<cfg::Configuration> config,
    std::shared_ptr<spdlog::logger> logger,
    frm::TaskBarIcon* taskBarIcon,
    const wxString& name)
    : pConfig(config)
    , pLogger(logger)
    , pTaskBarIcon(taskBarIcon)
    , pParent(parent)
    , pGeneralPage(nullptr)
    , pDatabasePage(nullptr)
    , pStopwatchPage(nullptr)
    , pTaskItemPage(nullptr)
{
    SetName(name);
    SetSheetStyle(wxPROPSHEET_LISTBOOK);

    Create(pParent, wxID_ANY, wxT("Preferences"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER, name);
    SetMinClientSize(wxSize(510, 380));
    SetSize(wxSize(510, 380));
}
// clang-format on

bool PreferencesDialog::Create(wxWindow* parent,
    wxWindowID windowId,
    const wxString& title,
    const wxPoint& position,
    const wxSize& size,
    long style,
    const wxString& name)
{
    bool created = wxPropertySheetDialog::Create(parent, windowId, title, position, size, style, name);
    if (created) {
        CreateControls();
    }
    return created;
}

void PreferencesDialog::CreateControls()
{
    auto listBook = static_cast<wxListbook*>(GetBookCtrl());

    pGeneralPage = new GeneralPage(listBook, pConfig);
    pDatabasePage = new DatabasePage(listBook, pConfig);
    pStopwatchPage = new StopwatchPage(listBook, pConfig);
    pTaskItemPage = new TaskItemPage(listBook, pConfig);

    listBook->AddPage(pGeneralPage, wxT("General"), true);
    listBook->AddPage(pDatabasePage, wxT("Database"), false);
    listBook->AddPage(pStopwatchPage, wxT("Stopwatch"), false);
    listBook->AddPage(pTaskItemPage, wxT("Task Item"), false);

    CreateButtons(wxOK | wxCANCEL);
    LayoutDialog();

    auto listView = listBook->GetListView();

    listView->SetColumnWidth(0, wxMulDivInt32(listView->GetColumnWidth(0), listView->GetColumnWidth(0), 96) + 4);
    listView->Refresh();
}

void PreferencesDialog::OnOk(wxCommandEvent& event)
{
    pGeneralPage->Apply();
    pDatabasePage->Apply();
    pStopwatchPage->Apply();

    if (pConfig->IsBackupEnabled() && pConfig->GetBackupPath().length() == 0) {
        wxMessageBox(wxT("A backup path must be selected."), common::GetProgramName(), wxOK_DEFAULT | wxICON_WARNING);
        return;
    }

    pConfig->Save();

    if (pConfig->IsShowInTray() && !pTaskBarIcon->IsIconInstalled()) {
        pTaskBarIcon->SetTaskBarIcon();
    } else if (!pConfig->IsShowInTray() && pTaskBarIcon->IsIconInstalled()) {
        pTaskBarIcon->RemoveIcon();
    }

    EndModal(wxID_OK);
}
} // namespace app::dlg
