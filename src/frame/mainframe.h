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

#pragma once

#include <memory>

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>

#include "../config/configuration.h"

namespace app::frame
{
class TaskBarIcon;

class MainFrame : public wxFrame
{
public:
    MainFrame(std::shared_ptr<cfg::Configuration> config, const wxString& name = wxT("MainFrame"));
    MainFrame(const MainFrame&) = delete;
    virtual ~MainFrame();

    MainFrame& operator=(const MainFrame&) = delete;

    bool OnStartUp();

private:
    wxDECLARE_EVENT_TABLE();

    bool Create();

    void CreateControls();
    void DataToControls();

    void OnAbout(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnNewTask(wxCommandEvent& event);
    void OnNewEmployer(wxCommandEvent& event);
    void OnNewClient(wxCommandEvent& event);
    void OnNewProject(wxCommandEvent& event);
    void OnNewCategory(wxCommandEvent& event);
    void OnEditEmployer(wxCommandEvent& event);
    void OnEditClient(wxCommandEvent& event);
    void OnEditProject(wxCommandEvent& event);
    void OnEditCategory(wxCommandEvent& event);
    void OnTaskInserted(wxCommandEvent& event);
    void OnItemDoubleClick(wxListEvent& event);
    void OnIconize(wxIconizeEvent& event);
    void OnSettings(wxCommandEvent& event);
    void OnDateChanged(wxDateEvent& event);

    void RefreshItems(wxDateTime date = wxDateTime::Now());

    wxDatePickerCtrl* pDatePickerCtrl;
    wxStaticText* pTotalHoursText;
    wxListCtrl* pListCtrl;
    TaskBarIcon* pTaskBarIcon;
    std::shared_ptr<cfg::Configuration> pConfig;

    enum
    {
        IDC_GO_TO_DATE = 1,
        IDC_HOURS_TEXT,
        IDC_LIST
    };
};
} // namespace app::frame
