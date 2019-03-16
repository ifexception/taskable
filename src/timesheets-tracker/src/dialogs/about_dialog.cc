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

#include "about_dialog.hh"

namespace app::dialog
{
wxIMPLEMENT_DYNAMIC_CLASS(about_dialog, wxDialog);

wxBEGIN_EVENT_TABLE(about_dialog, wxDialog)
wxEND_EVENT_TABLE()

about_dialog::about_dialog(wxWindow* parent, const wxString &name)
{
    bool success = create(parent,
        wxID_ANY,
        wxT("About Timesheets Tracker"),
        wxDefaultPosition,
        wxSize(320, 240),
        wxCAPTION | wxCLOSE_BOX,
        name);

    SetMinClientSize(wxSize(320, 240));
}

about_dialog::~about_dialog()
{
    Destroy();
}

bool about_dialog::create(wxWindow* parent,
    wxWindowID windowId,
    const wxString& title,
    const wxPoint& point,
    const wxSize& size,
    long style,
    const wxString& name)
{
    bool created = wxDialog::Create(parent,
        windowId,
        title,
        point,
        size,
        style,
        name);

    if (created) {
        create_controls();

        Centre();
        ShowModal();
    }

    return created;
}

void about_dialog::create_controls()
{
    wxPanel* panel = new wxPanel(this, -1);

    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);

    wxStaticBox* staticBox =
        new wxStaticBox(panel,
            wxID_ANY,
            wxT("About Timesheets Tracker"),
            wxPoint(5, 5),
            wxSize(240, 140));

    wxButton* okButton = new wxButton(this,
        wxID_OK,
        wxT("&Ok"));

    okButton->SetDefault();
    okButton->SetFocus();

    hbox->Add(okButton, 1);
    vbox->Add(panel, 1);
    vbox->Add(hbox, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
    SetSizer(vbox);
}
}
