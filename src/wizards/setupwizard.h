//  TimesheetsTracker is a desktop that aids you in tracking your timesheets
//  and seeing what work you have done.
//
//  Copyright(C)<2018><Szymon Welgus>
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

#include <wx/textctrl.h>
#include <wx/wizard.h>

namespace app::wizard
{
class SetupWizard final : public wxWizard
{
public:
    SetupWizard() = default;
    explicit SetupWizard(wxFrame* frame);
    virtual ~SetupWizard();

    void Run();

private:
    wxWizardPageSimple* pPage1;
};

class AddEmployerAndClientPage final : public wxWizardPageSimple
{
public:
    AddEmployerAndClientPage() = default;
    AddEmployerAndClientPage(SetupWizard* parent);
    virtual ~AddEmployerAndClientPage();

    bool TransferDataFromWindow() override;

private:
    SetupWizard* pParent;
    wxTextCtrl* pEmployerCtrl;
    wxTextCtrl* pClientCtrl;
};

class AddProjectPage final : public wxWizardPageSimple
{
public:
    AddProjectPage() = default;
    explicit AddProjectPage(SetupWizard* parent);
    virtual ~AddProjectPage();

    bool TransferDataFromWindow() override;

private:
    SetupWizard* pParent;
    wxTextCtrl* pNameCtrl;
    wxTextCtrl* pDisplayNameCtrl;
};

class AddCategoriesPage final : public wxWizardPageSimple
{
public:
    AddCategoriesPage() = default;
    explicit AddCategoriesPage(SetupWizard* parent);
    virtual ~AddCategoriesPage();

    bool TransferDataFromWindow() override;

private:
    SetupWizard* pParent;
    wxTextCtrl* pNameCtrl;
    wxTextCtrl* pDescriptionCtrl;
};
} // app::wizard
