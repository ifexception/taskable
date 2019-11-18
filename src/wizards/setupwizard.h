//  Taskable is a desktop that aids you in tracking your timesheets
//  and seeing what work you have done.
//
//  Copyright(C)<2019><Szymon Welgus>
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
//
//
//  Contact:
//    szymonwelgus at gmail dot com

#include <memory>

#include <wx/clrpicker.h>
#include <wx/textctrl.h>
#include <wx/wizard.h>

#define FMT_HEADER_ONLY
#include <spdlog/spdlog.h>

#include "setupdatabase.h"

namespace app::wizard
{
class SetupWizard final : public wxWizard
{
public:
    SetupWizard() = default;
    explicit SetupWizard(wxFrame* frame, std::shared_ptr<spdlog::logger> logger);
    virtual ~SetupWizard() = default;

    bool Run();

    wxString GetEmployer() const;
    void SetEmployer(const wxString& employer);
    wxString GetClient() const;
    void SetClient(const wxString& client);
    wxString GetProject() const;
    void SetProject(const wxString& project);
    void SetProjectDisplayName(const wxString& project);

private:
    void CreateDatabaseFile();
    void DeleteDatabaseFile();
    bool SetUpTables();
    bool SetUpEntities();

    std::shared_ptr<spdlog::logger> pLogger;
    wxWizardPageSimple* pPage1;
    wxFrame* pFrame;

    wxString mEmployer;
    wxString mClient;
    wxString mProject;
    wxString mDisplayName;
};

class AddEmployerAndClientPage final : public wxWizardPageSimple
{
public:
    AddEmployerAndClientPage() = default;
    explicit AddEmployerAndClientPage(SetupWizard* parent);
    virtual ~AddEmployerAndClientPage() = default;

    bool TransferDataFromWindow() override;

private:
    wxDECLARE_EVENT_TABLE();

    void OnWizardCancel(wxWizardEvent& event);

    SetupWizard* pParent;
    wxTextCtrl* pEmployerCtrl;
    wxTextCtrl* pClientCtrl;
};

class AddProjectPage final : public wxWizardPageSimple
{
public:
    AddProjectPage() = default;
    explicit AddProjectPage(SetupWizard* parent);
    virtual ~AddProjectPage() = default;

    bool TransferDataFromWindow() override;

private:
    wxDECLARE_EVENT_TABLE();

    void OnWizardCancel(wxWizardEvent& event);

    SetupWizard* pParent;
    wxTextCtrl* pNameCtrl;
    wxTextCtrl* pDisplayNameCtrl;
};
} // namespace app::wizard
