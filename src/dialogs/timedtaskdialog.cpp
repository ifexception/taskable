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

#include "timedtaskdialog.h"

#include "../common/common.h"

namespace app::dialog
{
wxBEGIN_EVENT_TABLE(TimedTaskDialog, wxDialog)
wxEND_EVENT_TABLE()

TimedTaskDialog::TimedTaskDialog(wxWindow* parent, const wxString& name)
    : pParent(parent)
    , pElapsedTimeText(nullptr)
    , pTimer(std::make_unique<wxTimer>(this, IDC_TIMER))
    , mStartTime()
{
    long style = wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU;
    Create(parent, wxID_ANY, wxT("Timed Task"), wxDefaultPosition, wxSize(320, 240), style, name);
    SetMinClientSize(wxSize(320, 240));
}

bool TimedTaskDialog::Create(wxWindow* parent, wxWindowID windowId, const wxString& title, const wxPoint& position, const wxSize& size, long style, const wxString& name)
{
    bool created = wxDialog::Create(parent, windowId, title, position, size, style, name);
    if (created) {
        CreateControls();
        SetIcon(common::GetProgramIcon());
        GetSizer()->Fit(this);
        CenterOnScreen();
    }

    return created;
}

void TimedTaskDialog::CreateControls()
{
    /* Sizer and Panel Control */
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    auto mainPanel = new wxPanel(this);
    mainPanel->SetSizer(mainSizer);

    /* Elapsed Time Text Control */
    pElapsedTimeText = new wxStaticText(mainPanel, IDC_ELAPSED, wxT("Elapsed Time: %s"));
    auto font = pElapsedTimeText->GetFont();
    font.SetPointSize(16);
    pElapsedTimeText->SetFont(font);

    mainSizer->Add(pElapsedTimeText, common::sizers::ControlCenter);
}
} // app::dialog
