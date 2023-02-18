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

#include "preferencesdlg.h"

#include <wx/bookctrl.h>
#include <wx/listbook.h>
#include <wx/listctrl.h>

#include "../common/common.h"
#include "../config/configurationprovider.h"
#include "../frame/taskbaricon.h"

namespace app::dlg
{
// clang-format off
wxBEGIN_EVENT_TABLE(PreferencesDialog, wxPropertySheetDialog)
EVT_BUTTON(wxID_OK, PreferencesDialog::OnOk)
wxEND_EVENT_TABLE()

PreferencesDialog::PreferencesDialog(wxWindow* parent,
    std::shared_ptr<spdlog::logger> logger,
    frm::TaskBarIcon* taskBarIcon,
    const wxString& name)
    : pLogger(logger)
    , pTaskBarIcon(taskBarIcon)
    , pParent(parent)
    , pGeneralPage(nullptr)
    , pDatabasePage(nullptr)
    , pStopwatchPage(nullptr)
    , pTaskItemPage(nullptr)
    , pExportPage(nullptr)
{
    SetName(name);
    SetSheetStyle(wxPROPSHEET_LISTBOOK);

    Create(pParent, wxID_ANY, "Preferences", wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER, name);
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

        wxIconBundle iconBundle("AppIcon", 0);
        SetIcons(iconBundle);

    }
    return created;
}

void PreferencesDialog::CreateControls()
{
    auto listBook = static_cast<wxListbook*>(GetBookCtrl());

    auto* config = cfg::ConfigurationProvider::Get().Configuration.get();

    pGeneralPage = new GeneralPage(listBook, config);
    pDatabasePage = new DatabasePage(listBook, config);
    pStopwatchPage = new StopwatchPage(listBook, config);
    pTaskItemPage = new TaskItemPage(listBook, config);
    pExportPage = new ExportPage(listBook, config);

    listBook->AddPage(pGeneralPage, "General", true);
    listBook->AddPage(pDatabasePage, "Database", false);
    listBook->AddPage(pStopwatchPage, "Stopwatch", false);
    listBook->AddPage(pTaskItemPage, "Task Item", false);
    listBook->AddPage(pExportPage, "Export", false);

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
    pTaskItemPage->Apply();
    pExportPage->Apply();

    if (cfg::ConfigurationProvider::Get().Configuration->IsBackupEnabled() &&
        cfg::ConfigurationProvider::Get().Configuration->GetBackupPath().length() == 0) {
        wxMessageBox("A backup path must be selected.", common::GetProgramName(), wxOK_DEFAULT | wxICON_WARNING);
        return;
    }

    if (cfg::ConfigurationProvider::Get().Configuration->IsBackupEnabled() &&
        cfg::ConfigurationProvider::Get().Configuration->GetDeleteBackupsAfter() <= 0) {
        wxMessageBox("A positive non-zero value is required if backups are enabled",
            common::GetProgramName(),
            wxOK_DEFAULT | wxICON_WARNING);
        return;
    }

    if (cfg::ConfigurationProvider::Get().Configuration->GetExportPath().length() == 0) {
        wxMessageBox("A export path must be selected",
            common::GetProgramName(),
            wxOK_DEFAULT | wxICON_WARNING);
        return;
    }

    if (cfg::ConfigurationProvider::Get().Configuration->GetDelimiter().length() == 0) {
        wxMessageBox("A delimiter is required",
            common::GetProgramName(),
            wxOK_DEFAULT | wxICON_WARNING);
        return;
    }

    cfg::ConfigurationProvider::Get().Configuration->Save();

    if (cfg::ConfigurationProvider::Get().Configuration->IsShowInTray() && !pTaskBarIcon->IsIconInstalled()) {
        pTaskBarIcon->SetTaskBarIcon();
    } else if (!cfg::ConfigurationProvider::Get().Configuration->IsShowInTray() && pTaskBarIcon->IsIconInstalled()) {
        pTaskBarIcon->RemoveIcon();
    }

    EndModal(wxID_OK);
}
} // namespace app::dlg
