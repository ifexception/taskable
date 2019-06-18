//  TimesheetsTracker is a desktop that aids you in tracking your timesheets
//  and seeing what work you have done.
//
//  Copyright(C) <2018> <Szymon Welgus>
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

#include "editlistdialog.h"

#include "employerdialog.h"
#include "clientdialog.h"
#include "projectdialog.h"
#include "categorydialog.h"

#include "../common/util.h"
#include "../magic_enum/magic_enum.h"
#include "../db/database_exception.h"

namespace app::dialog
{
wxIMPLEMENT_DYNAMIC_CLASS(EditListDialog, wxDialog);

wxBEGIN_EVENT_TABLE(EditListDialog, wxDialog)
    EVT_LIST_ITEM_ACTIVATED(EditListDialog::IDC_LIST, EditListDialog::OnItemDoubleClick)
wxEND_EVENT_TABLE()

EditListDialog::EditListDialog(wxWindow* parent, dialog_type dialogType, const wxString& name)
    : mType(dialogType)
    , mStrategy(nullptr)
{
    SetStrategy();
    auto enumName = magic_enum::enum_name(mType);
    std::string nameStr = std::string(enumName);
    long style = wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU;
    bool success = Create(parent, wxID_ANY, wxString(nameStr), wxDefaultPosition, mStrategy->GetSize(), style, name);
}

EditListDialog::~EditListDialog()
{
    if (mStrategy) {
        delete mStrategy;
    }
}

void EditListDialog::Launch()
{
    ShowModal();
}

bool EditListDialog::Create(wxWindow* parent, wxWindowID windowId, const wxString& title, const wxPoint& position, const wxSize& size, long style, const wxString& name)
{
    bool created = wxDialog::Create(parent, windowId, title, position, size, style, name);
    if (created) {
        CreateControls();
        DataToControls();

        GetSizer()->Fit(this);
        // SetIcon();
        Center();
    }

    return created;
}

void EditListDialog::CreateControls()
{
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    auto panelSizer = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(panelSizer, wxSizerFlags().Border(wxALL, 5));

    auto panel = new wxPanel(this, wxID_STATIC);
    panelSizer->Add(panel, wxSizerFlags().Border(wxALL, 5).Expand());

    auto sizer = new wxBoxSizer(wxHORIZONTAL);
    panel->SetSizer(sizer);

    pListCtrl = new wxListCtrl(panel, IDC_LIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_HRULES);
    sizer->Add(pListCtrl, 1, wxEXPAND | wxALL, 5);

    mStrategy->CreateControl(pListCtrl);
}

void EditListDialog::DataToControls()
{
    mStrategy->DataToControl(pListCtrl);
}

void EditListDialog::OnItemDoubleClick(wxListEvent& event)
{
    int id = event.GetData();

    switch (mType) {
    case dialog_type::Employer:
    {
        EmployerDialog editEmployer(this, true, id);
        editEmployer.Launch();
        break;
    }
    case dialog_type::Client:
    {
        ClientDialog editClient(this, true, id);
        editClient.Launch();
        break;
    }
    case dialog_type::Project:
    {
        ProjectDialog editProject(this, true, id);
        editProject.Launch();
        break;
    }
    case dialog_type::Category:
    {
        CategoryDialog editCategory(this, true, id);
        editCategory.ShowModal();
        break;
    }
    default:
        break;
    }
    EndDialog(wxID_OK);
}

void EditListDialog::SetStrategy()
{
    if (mStrategy) {
        delete mStrategy;
    }

    switch (mType) {
    case dialog_type::Employer:
        mStrategy = new EmployerStrategy();
        break;
    case dialog_type::Client:
        mStrategy = new ClientStrategy();
        break;
    case dialog_type::Project:
        mStrategy = new ProjectStrategy();
        break;
    case dialog_type::Category:
        mStrategy = new CategoryStrategy();
        break;
    default:
        break;
    }
}

Strategy::Strategy()
    : dbService()
{ }

void EmployerStrategy::CreateControl(wxListCtrl* control)
{
    wxListItem nameColumn;
    nameColumn.SetId(0);
    nameColumn.SetText(wxT("Employer"));
    nameColumn.SetWidth(100);
    control->InsertColumn(0, nameColumn);

    wxListItem dateCreatedColumn;
    dateCreatedColumn.SetId(1);
    dateCreatedColumn.SetText(wxT("Date Created"));
    control->InsertColumn(1, dateCreatedColumn);

    wxListItem dateModifiedColumn;
    dateModifiedColumn.SetId(2);
    dateModifiedColumn.SetText(wxT("Date Modified"));
    control->InsertColumn(2, dateModifiedColumn);
}

void EmployerStrategy::DataToControl(wxListCtrl* control)
{
    std::vector<models::employer> employers;
    try {
        employers = dbService.get_employers();
    } catch (const db::database_exception&) {
        // TODO Log error
    }

    int listIndex = 0;
    int columnIndex = 0;
    for (auto employer : employers) {
        listIndex = control->InsertItem(columnIndex++, employer.employer_name);
        control->SetItem(listIndex, columnIndex++, util::ConvertUnixTimestampToString(employer.date_created_utc));
        control->SetItem(listIndex, columnIndex++, util::ConvertUnixTimestampToString(employer.date_modified_utc));
        control->SetItemPtrData(listIndex, employer.employer_id);
        columnIndex = 0;
    }
}

wxSize EmployerStrategy::GetSize()
{
    return wxSize(320, 260);
}

void ClientStrategy::CreateControl(wxListCtrl* control)
{
    wxListItem employerColumn;
    employerColumn.SetId(0);
    employerColumn.SetText(wxT("Employer"));
    employerColumn.SetWidth(100);
    control->InsertColumn(0, employerColumn);

    wxListItem nameColumn;
    nameColumn.SetId(1);
    nameColumn.SetText(wxT("Client"));
    nameColumn.SetWidth(100);
    control->InsertColumn(1, nameColumn);

    wxListItem dateCreatedColumn;
    dateCreatedColumn.SetId(2);
    dateCreatedColumn.SetText(wxT("Date Created"));
    control->InsertColumn(2, dateCreatedColumn);

    wxListItem dateModifiedColumn;
    dateModifiedColumn.SetId(3);
    dateModifiedColumn.SetText(wxT("Date Modified"));
    control->InsertColumn(3, dateModifiedColumn);
}

void ClientStrategy::DataToControl(wxListCtrl* control)
{
    std::vector<models::client> clients;
    try {
        clients = dbService.get_clients();
    } catch (const db::database_exception&) {
        // TODO Log error
    }

    int listIndex = 0;
    int columnIndex = 0;
    for (auto client : clients) {
        listIndex = control->InsertItem(columnIndex++, client.employer_name);
        control->SetItem(listIndex, columnIndex++, client.client_name);
        control->SetItem(listIndex, columnIndex++, util::ConvertUnixTimestampToString(client.date_created_utc));
        control->SetItem(listIndex, columnIndex++, util::ConvertUnixTimestampToString(client.date_modified_utc));
        control->SetItemPtrData(listIndex, client.client_id);
        columnIndex = 0;
    }
}

wxSize ClientStrategy::GetSize()
{
    return wxSize(360, 260);
}

void ProjectStrategy::CreateControl(wxListCtrl* control)
{
    wxListItem employerColumn;
    employerColumn.SetId(0);
    employerColumn.SetText(wxT("Employer"));
    employerColumn.SetWidth(100);
    control->InsertColumn(0, employerColumn);

    wxListItem clientColumn;
    clientColumn.SetId(1);
    clientColumn.SetText(wxT("Client"));
    clientColumn.SetWidth(100);
    control->InsertColumn(1, clientColumn);

    wxListItem nameColumn;
    clientColumn.SetId(2);
    clientColumn.SetText(wxT("Project"));
    clientColumn.SetWidth(100);
    control->InsertColumn(2, clientColumn);

    wxListItem dateCreatedColumn;
    dateCreatedColumn.SetId(3);
    dateCreatedColumn.SetText(wxT("Date Created"));
    control->InsertColumn(3, dateCreatedColumn);

    wxListItem dateModifiedColumn;
    dateModifiedColumn.SetId(4);
    dateModifiedColumn.SetText(wxT("Date Modified"));
    control->InsertColumn(4, dateModifiedColumn);
}

void ProjectStrategy::DataToControl(wxListCtrl* control)
{
    std::vector<models::project> projects;
    try {
        projects = dbService.get_projects();
    } catch (const db::database_exception&) {
        // TODO Log error
    }

    int listIndex = 0;
    int columnIndex = 0;
    for (auto project : projects) {
        listIndex = control->InsertItem(columnIndex++, project.employer_name);
        control->SetItem(listIndex, columnIndex++, project.client_name);
        control->SetItem(listIndex, columnIndex++, project.project_name);
        control->SetItem(listIndex, columnIndex++, util::ConvertUnixTimestampToString(project.date_created_utc));
        control->SetItem(listIndex, columnIndex++, util::ConvertUnixTimestampToString(project.date_modified_utc));
        control->SetItemPtrData(listIndex, project.project_id);
        columnIndex = 0;
    }
}

wxSize ProjectStrategy::GetSize()
{
    return wxSize(400, 260);
}

void CategoryStrategy::CreateControl(wxListCtrl* control)
{
    wxListItem projectColumn;
    projectColumn.SetId(0);
    projectColumn.SetText(wxT("Project"));
    projectColumn.SetWidth(100);
    control->InsertColumn(0, projectColumn);

    wxListItem nameColumn;
    nameColumn.SetId(1);
    nameColumn.SetText(wxT("Category"));
    nameColumn.SetWidth(100);
    control->InsertColumn(1, nameColumn);

    wxListItem dateCreatedColumn;
    dateCreatedColumn.SetId(2);
    dateCreatedColumn.SetText(wxT("Date Created"));
    control->InsertColumn(2, dateCreatedColumn);

    wxListItem dateModifiedColumn;
    dateModifiedColumn.SetId(3);
    dateModifiedColumn.SetText(wxT("Date Modified"));
    control->InsertColumn(3, dateModifiedColumn);
}

void CategoryStrategy::DataToControl(wxListCtrl * control)
{
    std::vector<models::category> categories;
    try {
        categories = dbService.get_categories();
    } catch (const db::database_exception&) {
        // TODO Log error
    }

    int listIndex = 0;
    int columnIndex = 0;
    for (auto category : categories) {
        listIndex = control->InsertItem(columnIndex++, category.project_name);
        control->SetItem(listIndex, columnIndex++, category.category_name);
        control->SetItem(listIndex, columnIndex++, util::ConvertUnixTimestampToString(category.date_created_utc));
        control->SetItem(listIndex, columnIndex++, util::ConvertUnixTimestampToString(category.date_modified_utc));
        control->SetItemPtrData(listIndex, category.category_id);
        columnIndex = 0;
    }
}

wxSize CategoryStrategy::GetSize()
{
    return wxSize(360, 260);
}

}
