// Productivity tool to help you track the time you spend on tasks
// Copyright (C) 2023  Szymon Welgus
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

#include <wx/wx.h>
#include <wx/hyperlink.h>
#include <wx/popupwin.h>

namespace app ::frm
{
class FeedbackPopupWindow final : public wxPopupTransientWindow
{
public:
    FeedbackPopupWindow() = delete;
    FeedbackPopupWindow(wxWindow* parent);
    virtual ~FeedbackPopupWindow() = default;

    void Popup(wxWindow* focus) override;
    virtual bool Show(bool show = true) override;

    void OnDismiss() override;

private:
    wxDECLARE_EVENT_TABLE();

    void CreateControls();

    void OnSubmitABug(wxHyperlinkEvent& event);
    void OnRequestAFeature(wxHyperlinkEvent& event);

    wxHyperlinkCtrl* pSubmitABugLink;
    wxHyperlinkCtrl* pRequestAFeatureLink;

    enum { IDC_SUBMITABUG = wxID_HIGHEST + 1, IDC_REQUESTAFEATURE };
};
} // namespace app::frm
