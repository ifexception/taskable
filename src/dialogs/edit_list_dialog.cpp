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

#include "edit_list_dialog.h"

#include "employer_dialog.h"
#include "client_dialog.h"
#include "project_dialog.h"
#include "category_dialog.h"

#include "../common/util.h"
#include "../magic_enum/magic_enum.h"
#include "../db/database_exception.h"

namespace app::dialog
{
wxIMPLEMENT_DYNAMIC_CLASS(edit_list_dialog, wxDialog);

wxBEGIN_EVENT_TABLE(edit_list_dialog, wxDialog)
wxEND_EVENT_TABLE()

edit_list_dialog::edit_list_dialog(wxWindow* parent, dialog_type dialogType, const wxString& name)
    : mType(dialogType)
{
    set_strategy();
    auto enumName = magic_enum::enum_name(mType);
    std::string nameStr = std::string(enumName);
    long style = wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU;
    bool success = create(parent, wxID_ANY, wxString(nameStr), wxDefaultPosition, mStrategy->get_size(), style, name);
}

edit_list_dialog::~edit_list_dialog()
{
    if (mStrategy) {
        delete mStrategy;
    }
}

void edit_list_dialog::launch_dialog()
{
    ShowModal();
}

bool edit_list_dialog::create(wxWindow* parent, wxWindowID windowId, const wxString& title, const wxPoint& position, const wxSize& size, long style, const wxString& name)
{
    bool created = Create(parent, windowId, title, position, size, style, name);
    if (created) {
        create_controls();
        data_to_controls();

        GetSizer()->Fit(this);
        // SetIcon();
        Center();
    }

    return false;
}

void edit_list_dialog::create_controls()
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

    mStrategy->create_control(pListCtrl);
}

void edit_list_dialog::data_to_controls()
{
    mStrategy->data_to_control(pListCtrl);
}

void edit_list_dialog::on_item_double_click(wxListEvent& event)
{
    int id = event.GetData();

    switch (mType) {
    case dialog_type::Employer:
    {
        employer_dialog editEmployer(this, true, id);
        editEmployer.launch_employer_dialog();
        break;
    }
    case dialog_type::Client:
    {
        client_dialog editClient(this, true, id);
        editClient.launch_client_dialog();
        break;
    }
    case dialog_type::Project:
    {
        project_dialog editProject(this, true, id);
        editProject.launch_project_dialog();
        break;
    }
    case dialog_type::Category:
    {
        category_dialog editCategory(this, true, id);
        editCategory.launch_dialog();
        break;
    }
    default:
        break;
    }
}

void edit_list_dialog::set_strategy()
{
    if (mStrategy) {
        delete mStrategy;
    }

    switch (mType) {
    case dialog_type::Employer:
        mStrategy = new employer_strategy();
        break;
    case dialog_type::Client:
        mStrategy = new client_strategy();
        break;
    case dialog_type::Project:
        mStrategy = new project_strategy();
        break;
    case dialog_type::Category:
        mStrategy = new category_strategy();
        break;
    default:
        break;
    }
}

strategy::strategy()
    : dbService()
{ }

void employer_strategy::create_control(wxListCtrl* control)
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

void employer_strategy::data_to_control(wxListCtrl* control)
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
        control->SetItem(listIndex, columnIndex++, util::convert_unix_timestamp_to_wxdatetime(employer.date_created_utc));
        control->SetItem(listIndex, columnIndex++, util::convert_unix_timestamp_to_wxdatetime(employer.date_modified_utc));
        control->SetItemPtrData(listIndex, employer.employer_id);
        columnIndex = 0;
    }
}

wxSize employer_strategy::get_size()
{
    return wxSize(320, 260);
}

void client_strategy::create_control(wxListCtrl* control)
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

void client_strategy::data_to_control(wxListCtrl* control)
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
        control->SetItem(listIndex, columnIndex++, util::convert_unix_timestamp_to_wxdatetime(client.date_created_utc));
        control->SetItem(listIndex, columnIndex++, util::convert_unix_timestamp_to_wxdatetime(client.date_modified_utc));
        control->SetItemPtrData(listIndex, client.client_id);
        columnIndex = 0;
    }
}

wxSize client_strategy::get_size()
{
    return wxSize(360, 260);
}

void project_strategy::create_control(wxListCtrl* control)
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

void project_strategy::data_to_control(wxListCtrl* control)
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
        control->SetItem(listIndex, columnIndex++, util::convert_unix_timestamp_to_wxdatetime(project.date_created_utc));
        control->SetItem(listIndex, columnIndex++, util::convert_unix_timestamp_to_wxdatetime(project.date_modified_utc));
        control->SetItemPtrData(listIndex, project.project_id);
        columnIndex = 0;
    }
}

wxSize project_strategy::get_size()
{
    return wxSize(400, 260);
}

void category_strategy::create_control(wxListCtrl* control)
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

void category_strategy::data_to_control(wxListCtrl * control)
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
        control->SetItem(listIndex, columnIndex, category.category_name);
        control->SetItem(listIndex, columnIndex++, util::convert_unix_timestamp_to_wxdatetime(category.date_created_utc));
        control->SetItem(listIndex, columnIndex++, util::convert_unix_timestamp_to_wxdatetime(category.date_modified_utc));
        control->SetItemPtrData(listIndex, category.category_id);
        columnIndex = 0;
    }
}

wxSize category_strategy::get_size()
{
    return wxSize(360, 260);
}

}
