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

#include <wx/utils.h>
#include <wx/clipbrd.h>

#include "../common/common.h"
#include "../common/constants.h"
#include "../common/resources.h"
#include "../common/util.h"
#include "../data/taskitemdata.h"

#include "../dialogs/taskitemdlg.h"

namespace app::dlg
{
const wxString WeekLabel = wxT("Monday %s - Sunday %s");
const wxString SelectedDateLabel = wxT("%s");
wxString DayHoursLabels[7] = {
    wxT("Monday: %H:%M:%S"),
    wxT("Tuesday: %H:%M:%S"),
    wxT("Wednesday: %H:%M:%S"),
    wxT("Thursday: %H:%M:%S"),
    wxT("Friday: %H:%M:%S"),
    wxT("Saturday: %H:%M:%S"),
    wxT("Sunday: %H:%M:%S"),
};

wxString DayLabels[7] = {
    wxT("Monday"),
    wxT("Tuesday"),
    wxT("Wednesday"),
    wxT("Thursday"),
    wxT("Friday"),
    wxT("Saturday"),
    wxT("Sunday"),
};

WeeklyTaskViewDialog::WeeklyTaskViewDialog(wxWindow* parent,
    std::shared_ptr<spdlog::logger> logger,
    const wxString& name)
    : pParent(parent)
    , pLogger(logger)
    , pWeekDatesLabel(nullptr)
    , pCalendarCtrl(nullptr)
    , pDailyHoursBreakdownTextCtrlArray()
    , pTotalWeekHoursLabel(nullptr)
    , pWeeklyTreeModel(nullptr)
    , pDataViewCtrl(nullptr)
    , mDateTraverser()
    , mSelectedTaskItemId(-1)
    , mDaySelected(wxDefaultDateTime)
{
    long style = wxCAPTION | wxCLOSE_BOX | wxMAXIMIZE_BOX | wxMINIMIZE_BOX | wxRESIZE_BORDER;
    wxSize dialogSize = wxSize(740, 640);
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

        SetIcon(rc::GetProgramIcon());
        Center();
    }

    return created;
}

void WeeklyTaskViewDialog::CreateControls()
{
    /* Main Window Sizer */
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    /* Calendar Ctrl and Total Week Hours Label Panel + Sizer */
    auto navAndInfoSizer = new wxBoxSizer(wxHORIZONTAL);
    auto navAndInfoPanel = new wxPanel(this, wxID_ANY);
    navAndInfoPanel->SetSizer(navAndInfoSizer);
    mainSizer->Add(navAndInfoPanel);

    /* Dates Label and Calander Sizer */
    auto dateLabelsAndCalenderBox = new wxStaticBox(navAndInfoPanel, wxID_ANY, wxT("Dates"));
    auto dateLabelsAndCalenderBoxSizer = new wxStaticBoxSizer(dateLabelsAndCalenderBox, wxVERTICAL);
    navAndInfoSizer->Add(dateLabelsAndCalenderBoxSizer, 1, wxEXPAND | wxALL, 5);

    /* Week Date Label */
    pWeekDatesLabel = new wxStaticText(navAndInfoPanel, IDC_WEEK_DATES, wxGetEmptyString());
    dateLabelsAndCalenderBoxSizer->Add(pWeekDatesLabel, common::sizers::ControlDefault);

    /* Calendar Ctrl */
    long calendarStyle =
        wxCAL_MONDAY_FIRST | wxCAL_SHOW_SURROUNDING_WEEKS | wxCAL_SHOW_WEEK_NUMBERS | wxCAL_SEQUENTIAL_MONTH_SELECTION;
    pCalendarCtrl = new wxGenericCalendarCtrl(
        navAndInfoPanel, IDC_CALENDAR, wxDefaultDateTime, wxDefaultPosition, wxDefaultSize, calendarStyle);
    dateLabelsAndCalenderBoxSizer->Add(pCalendarCtrl, 0, wxALIGN_CENTER | wxALL, 10);

    /* Day Breakdown Sizers and Text Ctrls */
    auto dayAndWeekHoursSizer = new wxBoxSizer(wxVERTICAL);
    navAndInfoSizer->Add(dayAndWeekHoursSizer);

    auto dayBreakdownBox = new wxStaticBox(navAndInfoPanel, wxID_ANY, wxT("Daily Hours Breakdown"));
    auto dayBreakdownBoxSizer = new wxStaticBoxSizer(dayBreakdownBox, wxVERTICAL);

    auto dailyHoursBreakdownPanel = new wxPanel(navAndInfoPanel, wxID_STATIC);
    dayBreakdownBoxSizer->Add(dailyHoursBreakdownPanel, common::sizers::ControlExpand);

    auto dailyHoursBreakdownFlexSizer = new wxFlexGridSizer(3, 3, wxSize(1, 1));
    dailyHoursBreakdownPanel->SetSizer(dailyHoursBreakdownFlexSizer);

    for (size_t i = 0; i <= constants::Sunday; i++) {
        pDailyHoursBreakdownTextCtrlArray[i] = new wxTextCtrl(
            dailyHoursBreakdownPanel, IDC_MONDAY_HOURS, DayHoursLabels[i], wxDefaultPosition, wxSize(122, -1));
        pDailyHoursBreakdownTextCtrlArray[i]->SetEditable(false);
        pDailyHoursBreakdownTextCtrlArray[i]->SetToolTip(
            wxString::Format(wxT("Total hours for %s of the week"), DayLabels[i]));
        dailyHoursBreakdownFlexSizer->Add(pDailyHoursBreakdownTextCtrlArray[i], common::sizers::ControlDefault);
    }

    dayAndWeekHoursSizer->Add(dayBreakdownBoxSizer, common::sizers::ControlDefault);

    /* Total Week Hours Label Ctrl */
    pTotalWeekHoursLabel = new wxStaticText(navAndInfoPanel, IDC_TOTAL_WEEK_HOURS, wxT("Total Week Hours: %s"));
    pTotalWeekHoursLabel->SetToolTip(wxT("Shows the total hours worked for the selected week"));
    auto totalWeekHoursLabelFont = pTotalWeekHoursLabel->GetFont();
    totalWeekHoursLabelFont.SetPointSize(12);
    totalWeekHoursLabelFont.MakeBold();
    pTotalWeekHoursLabel->SetFont(totalWeekHoursLabelFont);
    dayAndWeekHoursSizer->Add(pTotalWeekHoursLabel, common::sizers::ControlCenterHorizontal);

    /* Main Dialog Panel */
    auto mainPanelSizer = new wxBoxSizer(wxVERTICAL);
    auto dataViewPanel = new wxPanel(this, wxID_STATIC);
    dataViewPanel->SetSizer(mainPanelSizer);
    mainSizer->Add(dataViewPanel, 1, wxGROW | wxALL, 1);

    /* Data View Ctrl */
    long dataViewStyle = wxDV_SINGLE | wxDV_ROW_LINES | wxDV_HORIZ_RULES | wxDV_VERT_RULES;
    pDataViewCtrl = new wxDataViewCtrl(dataViewPanel, IDC_DATAVIEW, wxDefaultPosition, wxDefaultSize, dataViewStyle);
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
    pCalendarCtrl->Bind(
        wxEVT_CALENDAR_WEEK_CLICKED,
        &WeeklyTaskViewDialog::OnCalendarWeekSelection,
        this
    );

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
    wxString mondayISODateString = mDateTraverser.GetDayISODate(constants::Days::Monday);
    wxString sundayISODateString = mDateTraverser.GetDayISODate(constants::Days::Sunday);

    pWeekDatesLabel->SetLabel(wxString::Format(WeekLabel, mondayISODateString, sundayISODateString));

    {
        wxWindowDisabler disableAll;
        wxBusyCursor wait;
        GetTaskItemsForDailyBreakdown();
        GetTaskItemsByDateRange(mondayISODateString, sundayISODateString);
        GetTaskItemHoursByDateRange(mondayISODateString, sundayISODateString);
    }
    pDataViewCtrl->Expand(pWeeklyTreeModel->ExpandRootNode());
}

void WeeklyTaskViewDialog::OnCalendarWeekSelection(wxCalendarEvent& event)
{
    if (event.GetDate().FormatISODate() == mDateTraverser.GetDayISODate(constants::Days::Monday)) {
        return;
    }

    {
        wxWindowDisabler disableAll;
        wxBusyCursor wait;

        mDateTraverser.Recalculate(event.GetDate());
        pWeeklyTreeModel->SetDateTraverser(mDateTraverser);
        pWeeklyTreeModel->ClearAll();
        for (auto& item : pWeeklyTreeModel->CollapseDayNodes()) {
            pDataViewCtrl->Collapse(item);
        }

        wxString mondayISODateString = mDateTraverser.GetDayISODate(constants::Days::Monday);
        wxString sundayISODateString = mDateTraverser.GetDayISODate(constants::Days::Sunday);

        pWeekDatesLabel->SetLabel(wxString::Format(WeekLabel, mondayISODateString, sundayISODateString));

        GetTaskItemsForDailyBreakdown();
        GetTaskItemsByDateRange(mondayISODateString, sundayISODateString);
        GetTaskItemHoursByDateRange(mondayISODateString, sundayISODateString);
    }

    pDataViewCtrl->Refresh();
}

void WeeklyTaskViewDialog::OnContextMenu(wxDataViewEvent& event)
{
    wxDataViewItem item = event.GetItem();

    if (item.IsOk()) {
        auto contextModel = (dv::WeeklyTreeModelNode*) item.GetID();
        if (!contextModel->IsContainer()) {
            mSelectedTaskItemId = contextModel->GetTaskItemId();
            mDaySelected = pWeeklyTreeModel->GetDateFromDataViewItem(item);
            mSelectedDataViewItem = item;

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
        pLogger->error("Error occured on TaskItemData::GetDescriptionById({0:d}) - {1:d} : {2}",
            mSelectedTaskItemId,
            e.get_code(),
            e.what());
        return;
    }

    auto canOpen = wxTheClipboard->Open();
    if (canOpen) {
        auto textData = new wxTextDataObject(description);
        wxTheClipboard->SetData(textData);
        wxTheClipboard->Close();
    }
}

void WeeklyTaskViewDialog::OnContextMenuEdit(wxCommandEvent& WXUNUSED(event))
{
    data::TaskItemData data;
    int taskItemTypeId = 0;
    try {
        taskItemTypeId = data.GetTaskItemTypeIdByTaskItemId(mSelectedTaskItemId);
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured on TaskItemData::GetTaskItemTypeIdByTaskItemId({0:d}) - {1:d} : {2}",
            mSelectedTaskItemId,
            e.get_code(),
            e.what());
        return;
    }

    constants::TaskItemTypes type = static_cast<constants::TaskItemTypes>(taskItemTypeId);
    wxDateTime dateContext = mDaySelected;

    dlg::TaskItemDialog editTask(this, pLogger, type, true, mSelectedTaskItemId, dateContext);
    editTask.ShowModal();
}

void WeeklyTaskViewDialog::OnContextMenuDelete(wxCommandEvent& WXUNUSED(event))
{
    data::TaskItemData data;
    try {
        data.Delete(mSelectedTaskItemId);
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error(
            "Error occured on TaskItemData::Delete({0:d}) - {1:d} : {2}", mSelectedTaskItemId, e.get_code(), e.what());
        return;
    }

    pWeeklyTreeModel->Delete(mSelectedDataViewItem);
}

void WeeklyTaskViewDialog::GetTaskItemsForDailyBreakdown()
{
    const auto& dateArray = mDateTraverser.GetISODates();
    data::TaskItemData taskItemData;
    for (std::size_t i = 0; i <= constants::Sunday; i++) {
        std::vector<wxString> durations;
        try {
            durations = taskItemData.GetHours(dateArray[i]);
        } catch (const sqlite::sqlite_exception& e) {
            pLogger->error("Error occured on TaskItemData::GetHours({0}) - {1:d} : {2}",
                dateArray[i].ToStdString(),
                e.get_code(),
                e.what());
        }

        wxTimeSpan totalDuration;
        for (auto& duration : durations) {
            std::vector<std::string> durationSplit = util::lib::split(duration.ToStdString(), ':');

            wxTimeSpan currentDuration(std::atol(durationSplit[0].c_str()),
                std::atol(durationSplit[1].c_str()),
                (wxLongLong) std::atoll(durationSplit[2].c_str()));

            totalDuration += currentDuration;
        }

        pDailyHoursBreakdownTextCtrlArray[i]->SetLabel(totalDuration.Format(DayHoursLabels[i]));
    }
}

void WeeklyTaskViewDialog::GetTaskItemsByDateRange(const wxString& fromDate, const wxString& toDate)
{
    data::TaskItemData taskItemData;
    std::vector<std::unique_ptr<model::TaskItemModel>> taskItems;
    try {
        taskItems = taskItemData.GetByWeek(fromDate, toDate);
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured on TaskItemData::GetByWeek({0}, {1}) - {2:d} : {3}",
            fromDate.ToStdString(),
            toDate.ToStdString(),
            e.get_code(),
            e.what());
    }

    pWeeklyTreeModel->AddToWeek(taskItems);
}

void WeeklyTaskViewDialog::GetTaskItemHoursByDateRange(const wxString& fromDate, const wxString& toDate)
{
    data::TaskItemData taskItemData;
    std::vector<wxString> durations;
    try {
        durations = taskItemData.GetHoursByWeek(fromDate, toDate);
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured on TaskItemData::GetHoursByWeek({0}, {1}) - {2:d} : {3}",
            fromDate.ToStdString(),
            toDate.ToStdString(),
            e.get_code(),
            e.what());
    }

    wxTimeSpan totalDuration;
    for (auto& duration : durations) {
        std::vector<std::string> durationSplit = util::lib::split(duration.ToStdString(), ':');

        wxTimeSpan currentDuration(std::atol(durationSplit[0].c_str()),
            std::atol(durationSplit[1].c_str()),
            (wxLongLong) std::atoll(durationSplit[2].c_str()));

        totalDuration += currentDuration;
    }

    pTotalWeekHoursLabel->SetLabel(totalDuration.Format(constants::TotalHours));
}
} // namespace app::dlg
