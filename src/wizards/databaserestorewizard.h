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

#include <sqlite_modern_cpp.h>
#include <sqlite_modern_cpp/errors.h>
#include <wx/wx.h>
#include <wx/dir.h>
#include <wx/listctrl.h>
#include <wx/wizard.h>

#include <spdlog/spdlog.h>

#include "../common/common.h"
#include "../config/configuration.h"
#include "../frame/mainframe.h"
#include "../../res/database-restore-wizard.xpm"

namespace app::wizard
{
class DatabaseRestoreWelcomePage;

class DatabaseRestoreWizard final : public wxWizard
{
public:
    DatabaseRestoreWizard() = default;
    DatabaseRestoreWizard(frm::MainFrame* frame,
        std::shared_ptr<spdlog::logger> logger,
        std::shared_ptr<cfg::Configuration> config,
        sqlite::database* database,
        bool restoreWithNoPreviousFileExisting = false);
    virtual ~DatabaseRestoreWizard() = default;

    bool Run();

    const wxString GetDatabaseFileVersionToRestore() const;
    const bool IsRestoreWithNoPreviousFileExisting() const;

    void SetDatabaseVersionToRestore(const wxString& value);
    void SetNewDatabaseHandle(sqlite::database* database);

private:
    frm::MainFrame* pFrame;
    std::shared_ptr<spdlog::logger> pLogger;
    std::shared_ptr<cfg::Configuration> pConfig;

    DatabaseRestoreWelcomePage* pPage1;

    sqlite::database* pDatabase;

    wxString mDatabaseFileToRestore;
    bool bRestoreWithNoPreviousFileExisting;
};

class DatabaseRestoreWelcomePage final : public wxWizardPageSimple
{
public:
    DatabaseRestoreWelcomePage() = delete;
    DatabaseRestoreWelcomePage(DatabaseRestoreWizard* parent);
    virtual ~DatabaseRestoreWelcomePage() = default;

private:
    void CreateControls();
};

class SelectDatabaseVersionPage final : public wxWizardPageSimple
{
public:
    SelectDatabaseVersionPage() = delete;
    SelectDatabaseVersionPage(DatabaseRestoreWizard* parent, std::shared_ptr<cfg::Configuration> config);
    virtual ~SelectDatabaseVersionPage() = default;

    bool TransferDataFromWindow() override;

private:
    void CreateControls();
    void ConfigureEventBindings();
    void FillControls();

    void OnItemChecked(wxListEvent& event);
    void OnItemUnchecked(wxListEvent& event);
    void OnWizardCancel(wxWizardEvent& event);

    DatabaseRestoreWizard* pParent;
    std::shared_ptr<cfg::Configuration> pConfig;
    wxListCtrl* pListCtrl;

    int mSelectedIndex;
};

class DatabaseRestoredPage final : public wxWizardPageSimple
{
public:
    DatabaseRestoredPage() = delete;
    DatabaseRestoredPage(DatabaseRestoreWizard* parent,
        std::shared_ptr<cfg::Configuration> config,
        sqlite::database* database);
    virtual ~DatabaseRestoredPage() = default;

    //bool TransferDataFromWindow() override;

private:
    void CreateControls();
    void ConfigureEventBindings();

    void OnWizardPageShown(wxWizardEvent& event);
    void OnWizardCancel(wxWizardEvent& event);

    void FileOperationErrorFeedback();

    DatabaseRestoreWizard* pParent;
    std::shared_ptr<cfg::Configuration> pConfig;
    sqlite::database* pDatabase;
    wxStaticText* pStatusInOperationLabel;
    wxGauge* pGaugeCtrl;
    wxStaticText* pStatusCompleteLabel;
};

class DirectoryTraverser final : public wxDirTraverser
{
public:
    DirectoryTraverser(wxArrayString& files);

    wxDirTraverseResult OnFile(const wxString& file) override;
    wxDirTraverseResult OnDir(const wxString& dir) override;

private:
    wxArrayString mFiles;
};
} // namespace app::wizard
