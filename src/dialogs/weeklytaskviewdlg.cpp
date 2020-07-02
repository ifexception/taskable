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

#include "weeklytaskviewdlg.h"

#include <wx/clipbrd.h>

#include "../common/common.h"
#include "../data/taskitemdata.h"

#include "../dialogs/taskitemdlg.h"

namespace app::dlg
{
WeeklyTaskViewDialog::WeeklyTaskViewDialog(wxWindow* parent,
    std::shared_ptr<spdlog::logger> logger,
    std::shared_ptr<cfg::Configuration> config,
    const wxString& name)
    : pParent(parent)
    , pLogger(logger)
    , pConfig(config)
    , pWeeklyTreeModel(nullptr)
    , pDataViewCtrl(nullptr)
    , mDateTraverser()
    , mSelectedTaskItemId(-1)
    , mDaySelected(wxDefaultDateTime)
{
    long style = wxCAPTION | wxCLOSE_BOX | wxMAXIMIZE_BOX | wxMINIMIZE_BOX | wxRESIZE_BORDER;
    wxSize dialogSize = wxSize(800, 600);
    Create(pParent, wxID_ANY, wxT("Weekly Task View"), wxDefaultPosition, dialogSize, style, name);
    SetMinSize(dialogSize);
}

bool WeeklyTaskViewDialog::Create(wxWindow* parent,
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
        ConfigureEventBindings();
        FillControls();

        SetIcon(common::GetProgramIcon());
        Center();
    }

    return created;
}

void WeeklyTaskViewDialog::CreateControls()
{
    /* Main Window Sizer */
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    /* Main Dialog Panel */
    auto mainPanelSizer = new wxBoxSizer(wxVERTICAL);
    auto dataViewPanel = new wxPanel(this, wxID_STATIC);
    dataViewPanel->SetSizer(mainPanelSizer);
    mainSizer->Add(dataViewPanel, 1, wxGROW | wxALL, 1);

    /* Data View Ctrl */
    long style = wxDV_SINGLE | wxDV_ROW_LINES | wxDV_HORIZ_RULES | wxDV_VERT_RULES;
    pDataViewCtrl = new wxDataViewCtrl(dataViewPanel, IDC_DATAVIEW, wxDefaultPosition, wxDefaultSize, style);
    mainPanelSizer->Add(pDataViewCtrl, 1, wxEXPAND | wxALL, 5);

    /* Data View Model */
    pWeeklyTreeModel = new dv::WeeklyTreeModel(mDateTraverser);
    pDataViewCtrl->AssociateModel(pWeeklyTreeModel.get());

    /* Data View Columns */
    auto projectNameTextRenderer = new wxDataViewTextRenderer("string", wxDATAVIEW_CELL_INERT);
    auto durationTextRenderer = new wxDataViewTextRenderer("string", wxDATAVIEW_CELL_INERT);
    auto categoryNameTextRenderer = new wxDataViewTextRenderer("string", wxDATAVIEW_CELL_INERT);
    auto descriptionTextRenderer = new wxDataViewTextRenderer("string", wxDATAVIEW_CELL_INERT);
    descriptionTextRenderer->EnableEllipsize(wxEllipsizeMode::wxELLIPSIZE_END);

    auto idRenderer = new wxDataViewTextRenderer("long", wxDATAVIEW_CELL_INERT);

    /* Project Column */
    auto projectColumn = new wxDataViewColumn(wxT("Project"),
        projectNameTextRenderer,
        dv::WeeklyTreeModel::Col_Project,
        80,
        wxALIGN_LEFT,
        wxDATAVIEW_COL_RESIZABLE);
    projectColumn->SetWidth(wxCOL_WIDTH_AUTOSIZE);
    pDataViewCtrl->AppendColumn(projectColumn);

    /* Duration Column */
    auto durationColumn =
        new wxDataViewColumn(wxT("Duration"), durationTextRenderer, dv::WeeklyTreeModel::Col_Duration);
    durationColumn->SetWidth(wxCOL_WIDTH_AUTOSIZE);
    durationColumn->SetResizeable(false);
    pDataViewCtrl->AppendColumn(durationColumn);

    /* Category Column */
    auto categoryColumn = new wxDataViewColumn(wxT("Category"),
        categoryNameTextRenderer,
        dv::WeeklyTreeModel::Col_Category,
        80,
        wxALIGN_CENTER,
        wxDATAVIEW_COL_RESIZABLE);
    categoryColumn->SetWidth(wxCOL_WIDTH_AUTOSIZE);
    pDataViewCtrl->AppendColumn(categoryColumn);

    /* Description Column */
    auto descriptionColumn = new wxDataViewColumn(wxT("Description"),
        descriptionTextRenderer,
        dv::WeeklyTreeModel::Col_Description,
        80,
        wxALIGN_LEFT,
        wxDATAVIEW_COL_RESIZABLE);
    pDataViewCtrl->AppendColumn(descriptionColumn);

    /* ID Column */
    auto idColumn = new wxDataViewColumn(
        wxT("ID"), idRenderer, dv::WeeklyTreeModel::Col_Id, 32, wxALIGN_CENTER, wxDATAVIEW_COL_HIDDEN);
    pDataViewCtrl->AppendColumn(idColumn);
}

// clang-format off
void WeeklyTaskViewDialog::ConfigureEventBindings()
{
    pDataViewCtrl->Bind(
        wxEVT_DATAVIEW_ITEM_CONTEXT_MENU,
        &WeeklyTaskViewDialog::OnContextMenu,
        this
    );

    Bind(
        wxEVT_MENU,
        &WeeklyTaskViewDialog::OnContextMenuCopyToClipboard,
        this,
        wxID_COPY
    );

    Bind(
        wxEVT_MENU,
        &WeeklyTaskViewDialog::OnContextMenuEdit,
        this,
        wxID_EDIT
    );

    Bind(
        wxEVT_MENU,
        &WeeklyTaskViewDialog::OnContextMenuDelete,
        this,
        wxID_DELETE
    );
}
// clang-format on

void WeeklyTaskViewDialog::FillControls()
{
    wxString mondayISODateString = mDateTraverser.GetDayISODate(DateTraverser::Days::Monday);
    wxString sundayISODateString = mDateTraverser.GetDayISODate(DateTraverser::Days::Sunday);

    data::TaskItemData taskItemData;
    std::vector<std::unique_ptr<model::TaskItemModel>> taskItems;
    try {
        taskItems = taskItemData.GetByWeek(mondayISODateString, sundayISODateString);
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured on TaskItemData::GetByWeek({0}, {1}) - {2:d} : {3}",
            mondayISODateString.ToStdString(),
            sundayISODateString.ToStdString(),
            e.get_code(),
            e.what());
    }

    pWeeklyTreeModel->InitBatch(taskItems);
    pDataViewCtrl->Expand(pWeeklyTreeModel->ExpandRootNode());
}

void WeeklyTaskViewDialog::OnContextMenu(wxDataViewEvent& event)
{
    wxDataViewItem item = event.GetItem();

    if (item.IsOk()) {
        auto contextModel = (dv::WeeklyTreeModelNode*) item.GetID();
        if (!contextModel->IsContainer()) {
            mSelectedTaskItemId = contextModel->GetTaskItemId();
            mDaySelected = pWeeklyTreeModel->GetDateFromDataViewItem(item);

            wxMenu menu;

            menu.Append(wxID_COPY, wxT("&Copy to Clipboard"));
            menu.Append(wxID_EDIT, wxT("&Edit"));
            menu.Append(wxID_DELETE, wxT("&Delete"));

            PopupMenu(&menu);
        }
    }
}
void WeeklyTaskViewDialog::OnContextMenuCopyToClipboard(wxCommandEvent& event)
{
    data::TaskItemData data;
    wxString description = wxGetEmptyString();
    try {
        description = data.GetDescriptionById(mSelectedTaskItemId);
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error(
            "Error occured on TaskItemData::GetTaskItemTypeIdByTaskItemId() - {0:d} : {1}", e.get_code(), e.what());
        return;
    }

    auto canOpen = wxTheClipboard->Open();
    if (canOpen) {
        auto textData = new wxTextDataObject(description);
        wxTheClipboard->SetData(textData);
        wxTheClipboard->Close();
    }
}

void WeeklyTaskViewDialog::OnContextMenuEdit(wxCommandEvent& event)
{
    data::TaskItemData data;
    int taskItemTypeId = 0;
    try {
        taskItemTypeId = data.GetTaskItemTypeIdByTaskItemId(mSelectedTaskItemId);
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error(
            "Error occured on TaskItemData::GetTaskItemTypeIdByTaskItemId() - {0:d} : {1}", e.get_code(), e.what());
        return;
    }

    constants::TaskItemTypes type = static_cast<constants::TaskItemTypes>(taskItemTypeId);
    wxDateTime dateContext = mDaySelected;

    dlg::TaskItemDialog editTask(this, pLogger, pConfig, type, true, mSelectedTaskItemId, dateContext);
    editTask.ShowModal();
}

void WeeklyTaskViewDialog::OnContextMenuDelete(wxCommandEvent& event)
{
    data::TaskItemData data;
    try {
        data.Delete(mSelectedTaskItemId);
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured on TaskItemModel::Delete() - {0:d} : {1}", e.get_code(), e.what());
        return;
    }
}
} // namespace app::dlg
