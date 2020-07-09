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

#include <array>
#include <memory>

#include <wx/wx.h>
#include "wx/calctrl.h"
#include <wx/dataview.h>

#ifdef wxHAS_NATIVE_CALENDARCTRL
#include <wx/generic/calctrlg.h>
#endif

#include <spdlog/spdlog.h>

#include "../common/datetraverser.h"
#include "../config/configuration.h"
#include "../dataview/weeklymodel.h"

namespace app::dlg
{
class WeeklyTaskViewDialog final : public wxDialog
{
public:
    WeeklyTaskViewDialog() = delete;
    WeeklyTaskViewDialog(wxWindow* parent,
        std::shared_ptr<spdlog::logger> logger,
        std::shared_ptr<cfg::Configuration> config,
        const wxString& name = wxT("weeklytaskviewdlg"));
    virtual ~WeeklyTaskViewDialog() = default;

private:
    bool Create(wxWindow* parent,
        wxWindowID windowId,
        const wxString& title,
        const wxPoint& position,
        const wxSize& size,
        long style,
        const wxString& name);

    void CreateControls();
    void ConfigureEventBindings();
    void FillControls();

    void OnCalendarWeekSelection(wxCalendarEvent& event);
    void OnContextMenu(wxDataViewEvent& event);
    void OnContextMenuCopyToClipboard(wxCommandEvent& event);
    void OnContextMenuEdit(wxCommandEvent& event);
    void OnContextMenuDelete(wxCommandEvent& event);

    void GetTaskItemsForDailyBreakdown();
    void GetTaskItemsByDateRange(const wxString& fromDate, const wxString& toDate);
    void GetTaskItemHoursByDateRange(const wxString& fromDate, const wxString& toDate);

    std::shared_ptr<spdlog::logger> pLogger;
    std::shared_ptr<cfg::Configuration> pConfig;

    wxWindow* pParent;
    wxStaticText* pWeekDatesLabel;
    wxCalendarCtrlBase* pCalendarCtrl;
    std::array<wxTextCtrl*, 7> pDailyHoursBreakdownTextCtrlArray;
    wxStaticText* pTotalWeekHoursLabel;
    wxObjectDataPtr<dv::WeeklyTreeModel> pWeeklyTreeModel;
    wxDataViewCtrl* pDataViewCtrl;

    DateTraverser mDateTraverser;

    wxDataViewItem mSelectedDataViewItem;
    int mSelectedTaskItemId;
    wxDateTime mDaySelected;

    enum {
        IDC_WEEK_DATES = wxID_HIGHEST + 1,
        IDC_CALENDAR,
        IDC_MONDAY_HOURS,
        IDC_TUESDAY_HOURS,
        IDC_WEDNESDAY_HOURS,
        IDC_THURSDAY_HOURS,
        IDC_FRIDAY_HOURS,
        IDC_SATURDAY_HOURS,
        IDC_SUNDAY_HOURS,
        IDC_TOTAL_WEEK_HOURS,
        IDC_DATAVIEW
    };
};
} // namespace app::dlg
