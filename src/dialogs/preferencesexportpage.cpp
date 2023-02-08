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

#include "preferencesexportpage.h"

#include <wx/stdpaths.h>

#include "../common/common.h"
#include "../config/configuration.h"

namespace app::dlg
{
ExportPage::ExportPage(wxWindow* parent, cfg::Configuration* config)
    : wxPanel(parent, wxID_ANY)
    , pConfig(config)
    , pParent(parent)
    , pDelimiterTextCtrl(nullptr)
    , pExportFilePathCtrl(nullptr)
    , pBrowseExportPathButton(nullptr)
{
    CreateControls();
    ConfigureEventBindings();
    FillControls();
}

void ExportPage::Apply()
{
    pConfig->SetDelimiter(pDelimiterTextCtrl->GetValue().ToStdString());
    pConfig->SetExportPath(pExportFilePathCtrl->GetValue().ToStdString());
}

void ExportPage::CreateControls()
{
    auto sizer = new wxBoxSizer(wxVERTICAL);

    /* export path static box */
    auto exportPathStaticBox = new wxStaticBox(this, wxID_ANY, "Export");
    auto exportPathStaticBoxSizer = new wxStaticBoxSizer(exportPathStaticBox, wxVERTICAL);

    auto exportPathLayoutSizer = new wxBoxSizer(wxHORIZONTAL);
    exportPathStaticBoxSizer->Add(exportPathLayoutSizer, common::sizers::ControlDefault);

    /* Export path label */
    auto exportPathLabel = new wxStaticText(exportPathStaticBox, wxID_ANY, "Path");
    exportPathLayoutSizer->Add(exportPathLabel, common::sizers::ControlCenter);

    /* Export File Path text control */
    pExportFilePathCtrl =
        new wxTextCtrl(exportPathStaticBox, IDC_EXPORTPATH, wxGetEmptyString(), wxDefaultPosition, wxSize(256, -1), 0);
    pExportFilePathCtrl->SetEditable(false);
    exportPathLayoutSizer->Add(pExportFilePathCtrl, common::sizers::ControlDefault);

    /* Browse Export Path button */
    pBrowseExportPathButton = new wxButton(exportPathStaticBox, IDC_EXPORTPATHBUTTON, "Browse...");
    exportPathLayoutSizer->Add(pBrowseExportPathButton, common::sizers::ControlDefault);

    sizer->Add(exportPathStaticBoxSizer, 0, wxLEFT | wxRIGHT | wxEXPAND, 5);

    /* Options static box */
    auto optionsStaticBox = new wxStaticBox(this, wxID_ANY, "Options");
    auto optionsStaticBoxSizer = new wxStaticBoxSizer(optionsStaticBox, wxVERTICAL);

    /* Options panel */
    auto optionsPanel = new wxPanel(optionsStaticBox, wxID_ANY);
    optionsStaticBoxSizer->Add(optionsPanel, common::sizers::ControlExpand);

    auto optionsFlexGridSizer = new wxFlexGridSizer(0, 2, 0, 0);
    optionsPanel->SetSizer(optionsFlexGridSizer);

    /* Delimiter text control */
    auto delimiterLabel = new wxStaticText(optionsPanel, wxID_ANY, "Delimiter");
    optionsFlexGridSizer->Add(delimiterLabel, common::sizers::ControlCenter);

    pDelimiterTextCtrl =
        new wxTextCtrl(optionsPanel, IDC_DELIMITER, wxGetEmptyString(), wxDefaultPosition, wxSize(32, -1), wxTE_CENTER);
    pDelimiterTextCtrl->SetMaxLength(1);
    pDelimiterTextCtrl->SetToolTip("Set the delimiter to use in the exported file");
    optionsFlexGridSizer->Add(pDelimiterTextCtrl, common::sizers::ControlDefault);

    sizer->Add(optionsStaticBoxSizer, 0, wxLEFT | wxRIGHT | wxEXPAND, 5);

    SetSizerAndFit(sizer);
}

// clang-format off
void ExportPage::ConfigureEventBindings()
{
    pBrowseExportPathButton->Bind(
        wxEVT_BUTTON,
        &ExportPage::OnOpenDirectoryForExportLocation,
        this,
        IDC_EXPORTPATHBUTTON
    );
}
// clang-format on

void ExportPage::FillControls()
{
    pDelimiterTextCtrl->SetValue(wxString(pConfig->GetDelimiter()));
    pExportFilePathCtrl->SetValue(wxString(pConfig->GetExportPath()));
}

void ExportPage::OnOpenDirectoryForExportLocation(wxCommandEvent& event)
{
    wxString pathDirectory = wxGetEmptyString();
    if (pConfig->GetExportPath().length() == 0 && !wxDirExists(pConfig->GetExportPath())) {
        pathDirectory = wxStandardPaths::Get().GetAppDocumentsDir();
    } else {
        pathDirectory = pConfig->GetExportPath();
    }

    auto openDirDialog =
        new wxDirDialog(this, "Select a Export Directory", pathDirectory, wxDD_DEFAULT_STYLE, wxDefaultPosition);
    int res = openDirDialog->ShowModal();

    if (res == wxID_OK) {
        auto selectedExportPath = openDirDialog->GetPath();
        pExportFilePathCtrl->SetValue(selectedExportPath);
        pExportFilePathCtrl->SetToolTip(selectedExportPath);
    }

    openDirDialog->Destroy();
}
} // namespace app::dlg
