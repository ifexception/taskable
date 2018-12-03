//  TimesheetsTracker is a desktop that aids you in tracking your timesheets
//  and seeing what work you have done.
//
//  Copyright(C)<2018><Szymon Welgus>
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
//
//
//  Contact:
//    szymonwelgus at gmail dot com

#include "main_frame.hh"

namespace app::frame
{
main_frame::main_frame()
    : wxFrame(nullptr, wxID_ANY, "Timesheets Tracker")
{
    wxMenu* fileMenu = new wxMenu();
    fileMenu->Append(wxID_EXIT);

    wxMenu* helpMenu = new wxMenu();
    helpMenu->Append(wxID_ABOUT);

    wxMenuBar* menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");

    SetMenuBar(menuBar);

    Bind(wxEVT_MENU,
         [=](wxCommandEvent&) {
             wxMessageBox("About Timesheets Tracker",
                          "About Timesheets Tracker", wxOK | wxICON_INFORMATION);
         },
         wxID_ABOUT);
    Bind(wxEVT_MENU, [=](wxCommandEvent&) { Close(true); }, wxID_EXIT);
}

main_frame::~main_frame()
{
}
}
