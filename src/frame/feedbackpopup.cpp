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

#include "feedbackpopup.h"

namespace app::frm
{
// clang-format off
wxBEGIN_EVENT_TABLE(FeedbackPopupWindow, wxPopupTransientWindow)
EVT_HYPERLINK(FeedbackPopupWindow::IDC_SUBMITABUG, FeedbackPopupWindow::OnSubmitABug)
EVT_HYPERLINK(FeedbackPopupWindow::IDC_REQUESTAFEATURE, FeedbackPopupWindow::OnRequestAFeature)
wxEND_EVENT_TABLE()

FeedbackPopupWindow::FeedbackPopupWindow(wxWindow* parent)
    : wxPopupTransientWindow(parent)
    , pSubmitABugLink(nullptr)
    , pRequestAFeatureLink(nullptr)
// clang-format on
{
    CreateControls();
}

void FeedbackPopupWindow::Popup(wxWindow* focus)
{
    wxPopupTransientWindow::Popup();
}

bool FeedbackPopupWindow::Show(bool show)
{
    return wxPopupTransientWindow::Show(show);
}

void FeedbackPopupWindow::OnDismiss()
{
    wxPopupTransientWindow::OnDismiss();
}

void FeedbackPopupWindow::CreateControls()
{
    auto sizer = new wxBoxSizer(wxVERTICAL);

    auto panel = new wxPanel(this, wxID_ANY);
    panel->SetSizer(sizer);

    auto feedbackLabel = new wxStaticText(panel, wxID_ANY, wxT("Feedback"));
    auto feedbackLabelFont = feedbackLabel->GetFont();
    feedbackLabelFont.SetPointSize(12);
    feedbackLabel->SetFont(feedbackLabelFont);
    sizer->Add(feedbackLabel, wxSizerFlags().Border(wxALL, 5).CenterHorizontal());

    auto hyperlinkSizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(hyperlinkSizer, wxSizerFlags().Expand());

    wxString submitABugLink =
        wxT("https://github.com/ifexception/taskable/issues/new?title=BUG%3A&template=ISSUE_TEMPLATE.md");
    pSubmitABugLink = new wxHyperlinkCtrl(panel, IDC_SUBMITABUG, wxT("Submit a Bug"), submitABugLink);
    pSubmitABugLink->SetToolTip(wxT("Click the link to submit a bug in the program"));
    hyperlinkSizer->Add(pSubmitABugLink, wxSizerFlags().Border(wxALL, 5));

    wxString requestAFeatureLink = wxT("https://github.com/ifexception/taskable/issues/"
                                       "new?title=Feature+request%3A&body=Feature+request+overview%3A%0A%0A");
    pRequestAFeatureLink =
        new wxHyperlinkCtrl(panel, IDC_REQUESTAFEATURE, wxT("Request a Feature"), requestAFeatureLink);
    pRequestAFeatureLink->SetToolTip(wxT("Click the link to request a feature or improvement"));
    hyperlinkSizer->Add(pRequestAFeatureLink, wxSizerFlags().Border(wxALL, 5));

    sizer->Fit(panel);
    SetClientSize(panel->GetSize());
}

void FeedbackPopupWindow::OnSubmitABug(wxHyperlinkEvent& event)
{
    wxLaunchDefaultBrowser(event.GetURL());
}

void FeedbackPopupWindow::OnRequestAFeature(wxHyperlinkEvent& event)
{
    wxLaunchDefaultBrowser(event.GetURL());
}
} // namespace app::frm
