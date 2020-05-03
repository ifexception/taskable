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

#include "editlistdlg.h"

#include <memory>
#include <vector>

#include <sqlite_modern_cpp/errors.h>

#include "employerdlg.h"
#include "clientdlg.h"
#include "projectdlg.h"
#include "categorydlg.h"

#include "../models/clientmodel.h"
#include "../models/projectmodel.h"
#include "../models/categorymodel.h"

#include "../common/common.h"
#include "../common/ids.h"
#include "../common/util.h"

namespace app::dlg
{
// clang-format off
wxBEGIN_EVENT_TABLE(EditListDialog, wxDialog)
EVT_LIST_ITEM_ACTIVATED(EditListDialog::IDC_LIST, EditListDialog::OnItemDoubleClick)
wxEND_EVENT_TABLE()
    // clang-format on

    EditListDialog::EditListDialog(wxWindow* parent,
        DialogType dialogType,
        std::shared_ptr<spdlog::logger> logger,
        const wxString& name)
    : mType(dialogType)
    , pLogger(logger)
    , mStrategy(nullptr)
{
    SetStrategy();
    std::string title = MapEnumToValue();
    long style = wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU;
    bool success = Create(parent, wxID_ANY, wxString(title), wxDefaultPosition, mStrategy->GetSize(), style, name);
}

EditListDialog::~EditListDialog()
{
    if (mStrategy) {
        delete mStrategy;
    }
}

bool EditListDialog::Create(wxWindow* parent,
    wxWindowID windowId,
    const wxString& title,
    const wxPoint& position,
    const wxSize& size,
    long style,
    const wxString& name)
{
    bool created = wxDialog::Create(parent, windowId, title, position, size, style, name);
    if (created) {
        CreateControls();
        DataToControls();

        GetSizer()->Fit(this);
        SetIcon(common::GetProgramIcon());
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

    long style = wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_HRULES;
    pListCtrl = new wxListCtrl(panel, IDC_LIST, wxDefaultPosition, wxDefaultSize, style);
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
    int retCode = -1;

    switch (mType) {
    case DialogType::Employer: {
        EmployerDialog editEmployer(this, pLogger, true, id);
        retCode = editEmployer.ShowModal();
        break;
    }
    case DialogType::Client: {
        ClientDialog editClient(this, pLogger, true, id);
        retCode = editClient.ShowModal();
        break;
    }
    case DialogType::Project: {
        ProjectDialog editProject(this, pLogger, true, id);
        retCode = editProject.ShowModal();
        break;
    }
    case DialogType::Category: {
        CategoryDialog editCategory(this, pLogger, id);
        retCode = editCategory.ShowModal();
        break;
    }
    default:
        break;
    }

    EndDialog(retCode);
}

void EditListDialog::SetStrategy()
{
    if (mStrategy) {
        delete mStrategy;
    }

    switch (mType) {
    case DialogType::Employer:
        mStrategy = new EmployerStrategy(pLogger);
        break;
    case DialogType::Client:
        mStrategy = new ClientStrategy(pLogger);
        break;
    case DialogType::Project:
        mStrategy = new ProjectStrategy(pLogger);
        break;
    case DialogType::Category:
        mStrategy = new CategoryStrategy(pLogger);
        break;
    default:
        break;
    }
}

std::string EditListDialog::MapEnumToValue()
{
    switch (mType) {
    case DialogType::Employer:
        return "Employer";
    case DialogType::Client:
        return "Client";
    case DialogType::Project:
        return "Project";
    case DialogType::Category:
        return "Category";
    default:
        return "";
    }
}

Strategy::Strategy() {}

EmployerStrategy::EmployerStrategy(std::shared_ptr<spdlog::logger> logger)
    : pLogger(logger)
    , mData()
{
}

void EmployerStrategy::CreateControl(wxListCtrl* control)
{
    control->SetSize(wxSize(250, 100));

    wxListItem nameColumn;
    nameColumn.SetId(0);
    nameColumn.SetText(wxT("Employer"));
    nameColumn.SetWidth(100);
    control->InsertColumn(0, nameColumn);

    wxListItem dateModifiedColumn;
    dateModifiedColumn.SetId(1);
    dateModifiedColumn.SetText(wxT("Date Modified"));
    dateModifiedColumn.SetWidth(148);
    control->InsertColumn(1, dateModifiedColumn);
}

void EmployerStrategy::DataToControl(wxListCtrl* control)
{
    std::vector<std::unique_ptr<model::EmployerModel>> employers;
    try {
        employers = mData.GetAll();
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in EmployerData::GetAll(); - {0:d} : {1}", e.get_code(), e.what());
    }

    int listIndex = 0;
    int columnIndex = 0;
    for (auto& employer : employers) {
        listIndex = control->InsertItem(columnIndex++, employer->GetName());
        control->SetItem(listIndex, columnIndex++, employer->GetDateModified().FormatISOCombined());
        control->SetItemPtrData(listIndex, employer->GetEmployerId());
        columnIndex = 0;
    }
}

wxSize EmployerStrategy::GetSize()
{
    return wxSize(320, 260);
}

ClientStrategy::ClientStrategy(std::shared_ptr<spdlog::logger> logger)
    : pLogger(logger)
    , mData()
{
}

void ClientStrategy::CreateControl(wxListCtrl* control)
{
    control->SetSize(wxSize(350, 100));

    wxListItem employerColumn;
    employerColumn.SetId(0);
    employerColumn.SetText(wxT("Employer"));
    employerColumn.SetWidth(80);
    control->InsertColumn(0, employerColumn);

    wxListItem nameColumn;
    nameColumn.SetId(1);
    nameColumn.SetText(wxT("Client"));
    nameColumn.SetWidth(120);
    control->InsertColumn(1, nameColumn);

    wxListItem dateModifiedColumn;
    dateModifiedColumn.SetId(2);
    dateModifiedColumn.SetText(wxT("Date Modified"));
    dateModifiedColumn.SetWidth(148);
    control->InsertColumn(2, dateModifiedColumn);
}

void ClientStrategy::DataToControl(wxListCtrl* control)
{
    std::vector<std::unique_ptr<model::ClientModel>> clients;
    try {
        clients = mData.GetAll();
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in ClientData::GetAll() - {0:d} : {1}", e.get_code(), e.what());
    }

    int listIndex = 0;
    int columnIndex = 0;
    for (auto& client : clients) {
        listIndex = control->InsertItem(columnIndex++, client->GetEmployer()->GetName());
        control->SetItem(listIndex, columnIndex++, client->GetName());
        control->SetItem(listIndex, columnIndex++, client->GetDateModified().FormatISOCombined());
        control->SetItemPtrData(listIndex, client->GetClientId());
        columnIndex = 0;
    }
}

wxSize ClientStrategy::GetSize()
{
    return wxSize(360, 260);
}

ProjectStrategy::ProjectStrategy(std::shared_ptr<spdlog::logger> logger)
    : pLogger(logger)
{
}

void ProjectStrategy::CreateControl(wxListCtrl* control)
{
    control->SetSize(wxSize(450, 100));

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

    wxListItem dateModifiedColumn;
    dateModifiedColumn.SetId(3);
    dateModifiedColumn.SetText(wxT("Date Modified"));
    dateModifiedColumn.SetWidth(148);
    control->InsertColumn(3, dateModifiedColumn);
}

void ProjectStrategy::DataToControl(wxListCtrl* control)
{
    std::vector<std::unique_ptr<model::ProjectModel>> projects;
    try {
        projects = model::ProjectModel::GetAll();
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in ProjectModel::GetAll() - {0:d} : {1}", e.get_code(), e.what());
    }

    int listIndex = 0;
    int columnIndex = 0;
    for (const auto& project : projects) {
        listIndex = control->InsertItem(columnIndex++, project->GetEmployer()->GetName());
        control->SetItem(
            listIndex, columnIndex++, project->HasClientLinked() ? project->GetClient()->GetName() : wxT("n/a"));
        control->SetItem(listIndex, columnIndex++, project->GetName());
        control->SetItem(listIndex, columnIndex++, project->GetDateModified().FormatISOCombined());
        control->SetItemPtrData(listIndex, project->GetProjectId());
        columnIndex = 0;
    }
}

wxSize ProjectStrategy::GetSize()
{
    return wxSize(400, 260);
}

CategoryStrategy::CategoryStrategy(std::shared_ptr<spdlog::logger> logger)
    : pLogger(logger)
{
}

void CategoryStrategy::CreateControl(wxListCtrl* control)
{
    control->SetSize(wxSize(350, 100));

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

    wxListItem dateModifiedColumn;
    dateModifiedColumn.SetId(2);
    dateModifiedColumn.SetText(wxT("Date Modified"));
    dateModifiedColumn.SetWidth(148);
    control->InsertColumn(2, dateModifiedColumn);
}

void CategoryStrategy::DataToControl(wxListCtrl* control)
{
    std::vector<std::unique_ptr<model::CategoryModel>> categories;
    try {
        categories = model::CategoryModel::GetAll();
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in get_categories() - {0:d} : {1}", e.get_code(), e.what());
    }

    int listIndex = 0;
    int columnIndex = 0;
    for (auto& category : categories) {
        listIndex = control->InsertItem(columnIndex++, category->GetProject()->GetDisplayName());
        control->SetItem(listIndex, columnIndex++, category->GetName());
        control->SetItem(listIndex, columnIndex++, category->GetDateModified().FormatISOCombined());
        control->SetItemPtrData(listIndex, category->GetCategoryId());
        columnIndex = 0;
    }
}

wxSize CategoryStrategy::GetSize()
{
    return wxSize(360, 260);
}

} // namespace app::dlg
