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

#include <wx/wx.h>
#include <wx/listctrl.h>
#define FMT_HEADER_ONLY
#include <spdlog/spdlog.h>

#include "../services/db_service.h"

namespace app::dialog
{
enum class dialog_type { Employer, Client, Project, Category };

struct Strategy
{
public:
    Strategy();
    virtual ~Strategy() = default;

    virtual void CreateControl(wxListCtrl* control) = 0;
    virtual void DataToControl(wxListCtrl* control) = 0;
    virtual wxSize GetSize() = 0;

protected:
    services::db_service dbService;
};

struct EmployerStrategy final : public Strategy
{
public:
    EmployerStrategy() = default;
    virtual ~EmployerStrategy() = default;

    void CreateControl(wxListCtrl* control) override;
    void DataToControl(wxListCtrl* control) override;
    wxSize GetSize() override;
};

struct ClientStrategy final : public Strategy
{
public:
    ClientStrategy() = default;
    virtual ~ClientStrategy() = default;

    void CreateControl(wxListCtrl* control) override;
    void DataToControl(wxListCtrl* control) override;
    wxSize GetSize() override;
};

struct ProjectStrategy final : public Strategy
{
public:
    ProjectStrategy() = default;
    virtual ~ProjectStrategy() = default;

    void CreateControl(wxListCtrl* control) override;
    void DataToControl(wxListCtrl* control) override;
    wxSize GetSize() override;
};

struct CategoryStrategy final : public Strategy
{
public:
    CategoryStrategy() = default;
    virtual ~CategoryStrategy() = default;

    void CreateControl(wxListCtrl* control) override;
    void DataToControl(wxListCtrl* control) override;
    wxSize GetSize() override;
};

class EditListDialog final : public wxDialog
{
public:
    EditListDialog() = delete;
    explicit EditListDialog(wxWindow* parent,
        dialog_type dialogType,
        std::shared_ptr<spdlog::logger> logger,
        const wxString& name = wxT("EditListDialog"));
    virtual ~EditListDialog();

private:
    wxDECLARE_EVENT_TABLE();

    bool Create(wxWindow* parent,
        wxWindowID windowId,
        const wxString& title,
        const wxPoint& position,
        const wxSize& size,
        long style,
        const wxString& name);

    void CreateControls();
    void DataToControls();

    void OnItemDoubleClick(wxListEvent& event);

    void SetStrategy();

    std::shared_ptr<spdlog::logger> pLogger;
    wxListCtrl* pListCtrl;
    Strategy* mStrategy;
    dialog_type mType;

    enum { IDC_LIST };
};
} // namespace app::dialog
