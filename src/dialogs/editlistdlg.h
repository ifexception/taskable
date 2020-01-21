//  Taskable is a desktop that aids you in tracking your timesheets
//  and seeing what work you have done.
//
//  Copyright(C) <2019> <Szymon Welgus>
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

#include <spdlog/spdlog.h>

namespace app::dlg
{
enum class DialogType { Employer, Client, Project, Category };

struct Strategy
{
public:
    Strategy();
    virtual ~Strategy() = default;

    virtual void CreateControl(wxListCtrl* control) = 0;
    virtual void DataToControl(wxListCtrl* control) = 0;
    virtual wxSize GetSize() = 0;
};

struct EmployerStrategy final : public Strategy
{
public:
    EmployerStrategy(std::shared_ptr<spdlog::logger> logger);
    virtual ~EmployerStrategy() = default;

    void CreateControl(wxListCtrl* control) override;
    void DataToControl(wxListCtrl* control) override;
    wxSize GetSize() override;

private:
    std::shared_ptr<spdlog::logger> pLogger;
};

struct ClientStrategy final : public Strategy
{
public:
    ClientStrategy(std::shared_ptr<spdlog::logger> logger);
    virtual ~ClientStrategy() = default;

    void CreateControl(wxListCtrl* control) override;
    void DataToControl(wxListCtrl* control) override;
    wxSize GetSize() override;

private:
    std::shared_ptr<spdlog::logger> pLogger;
};

struct ProjectStrategy final : public Strategy
{
public:
    ProjectStrategy(std::shared_ptr<spdlog::logger> logger);
    virtual ~ProjectStrategy() = default;

    void CreateControl(wxListCtrl* control) override;
    void DataToControl(wxListCtrl* control) override;
    wxSize GetSize() override;

private:
    std::shared_ptr<spdlog::logger> pLogger;
};

struct CategoryStrategy final : public Strategy
{
public:
    CategoryStrategy(std::shared_ptr<spdlog::logger> logger);
    virtual ~CategoryStrategy() = default;

    void CreateControl(wxListCtrl* control) override;
    void DataToControl(wxListCtrl* control) override;
    wxSize GetSize() override;

private:
    std::shared_ptr<spdlog::logger> pLogger;
};

class EditListDialog final : public wxDialog
{
public:
    EditListDialog() = delete;
    explicit EditListDialog(wxWindow* parent,
        DialogType dialogType,
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

    std::string MapEnumToValue();

    std::shared_ptr<spdlog::logger> pLogger;
    wxListCtrl* pListCtrl;
    Strategy* mStrategy;
    DialogType mType;

    enum { IDC_LIST  = wxID_HIGHEST + 1};
};
} // namespace app::dlg
