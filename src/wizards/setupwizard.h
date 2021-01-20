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

#include <wx/wx.h>
#include <wx/arrstr.h>
#include <wx/textctrl.h>
#include <wx/wizard.h>

#include <spdlog/spdlog.h>

#include "entitycompositor.h"

namespace app::wizard
{
class WelcomePage;

class SetupWizard final : public wxWizard
{
public:
    SetupWizard() = default;
    explicit SetupWizard(wxFrame* frame,
        std::shared_ptr<spdlog::logger> logger);
    virtual ~SetupWizard() = default;

    bool Run();

private:
    std::shared_ptr<spdlog::logger> pLogger;
    std::shared_ptr<EntityCompositor> pCompositor;

    WelcomePage* pPage1;
};

class WelcomePage final : public wxWizardPageSimple
{
public:
    WelcomePage() = delete;
    WelcomePage(wxWizard* parent);
    virtual ~WelcomePage() = default;

private:
    void CreateControls();
};

class AddEmployerPage final : public wxWizardPageSimple
{
public:
    AddEmployerPage() = default;
    explicit AddEmployerPage(wxWizard* parent, std::shared_ptr<EntityCompositor> compositor);
    virtual ~AddEmployerPage() = default;

    bool TransferDataFromWindow() override;

private:
    wxDECLARE_EVENT_TABLE();

    void OnWizardCancel(wxWizardEvent& event);

    void CreateControls();

    std::shared_ptr<EntityCompositor> pCompositor;

    wxWizard* pParent;
    wxTextCtrl* pEmployerTextCtrl;
};

class AddClientPage final : public wxWizardPageSimple
{
public:
    AddClientPage(wxWizard* parent, std::shared_ptr<EntityCompositor> compositor);
    virtual ~AddClientPage() = default;

    bool TransferDataFromWindow() override;

private:
    wxDECLARE_EVENT_TABLE();

    void OnWizardCancel(wxWizardEvent& event);

    void CreateControls();

    std::shared_ptr<EntityCompositor> pCompositor;

    wxWizard* pParent;
    wxTextCtrl* pClientTextCtrl;
};

class AddProjectPage final : public wxWizardPageSimple
{
public:
    AddProjectPage() = default;
    explicit AddProjectPage(SetupWizard* parent, std::shared_ptr<spdlog::logger> logger,
                            std::shared_ptr<EntityCompositor> compositor );
    virtual ~AddProjectPage() = default;

    bool TransferDataFromWindow() override;

private:
    wxDECLARE_EVENT_TABLE();

    void OnWizardCancel(wxWizardEvent& event);
    void OnNameChange(wxCommandEvent& event);
    void OnBillableCheck(wxCommandEvent& event);
    void OnRateChoiceSelection(wxCommandEvent& event);

    void CreateControls();
    void FillControls();

    std::shared_ptr<spdlog::logger> pLogger;
    std::shared_ptr<EntityCompositor> pCompositor;

    SetupWizard* pParent;

    wxTextCtrl* pNameTextCtrl;
    wxTextCtrl* pDisplayNameTextCtrl;
    wxCheckBox* pBillableCheckBoxCtrl;
    wxChoice* pRateChoiceCtrl;
    wxTextCtrl* pRateTextCtrl;
    wxComboBox* pCurrencyComboBoxCtrl;

    wxArrayString mCurrencyChoices;

    enum {
        IDC_NAME = wxID_HIGHEST + 1,
        IDC_BILLABLE,
        IDC_RATECHOICE,
    };
};
} // namespace app::wizard
