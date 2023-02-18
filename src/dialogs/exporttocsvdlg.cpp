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

#include "exporttocsvdlg.h"

#include <wx/richtooltip.h>
#include <wx/statline.h>
#include <wx/stdpaths.h>
#include <wx/utils.h>

#include "../common/common.h"
#include "../common/resources.h"
#include "../config/configurationprovider.h"
#include "../services/csvexporter.h"

namespace app::dlg
{
ExportToCsvDialog::ExportToCsvDialog(wxWindow* parent, std::shared_ptr<spdlog::logger> logger, const wxString& name)
    : pLogger(logger)
    , pStartDateCtrl(nullptr)
    , pEndDateCtrl(nullptr)
    , pDelimiterTextCtrl(nullptr)
    , pExportFilePathCtrl(nullptr)
    , pBrowseExportPathButton(nullptr)
    , pExportFileNameCtrl(nullptr)
    , pFeedbackLabel(nullptr)
    , pExportButton(nullptr)
{
    Create(parent, wxID_ANY, "Export to CSV", wxDefaultPosition, wxDefaultSize, wxCAPTION | wxCLOSE_BOX, name);
}

bool ExportToCsvDialog::Create(wxWindow* parent,
    wxWindowID windowId,
    const wxString& title,
    const wxPoint& position,
    const wxSize& size,
    long style,
    const wxString& name)
{
    bool created = wxDialog::Create(parent, windowId, title, position, size, style, name);
    if (created) {
        CreateControls();
        ConfigureEventBindings();
        FillControls();

        GetSizer()->Fit(this);

        wxIconBundle iconBundle("AppIcon", 0);
        SetIcons(iconBundle);

        Centre();
    }
    return created;
}

void ExportToCsvDialog::CreateControls()
{
    /* Window Sizing */
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    auto mainPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(mainPanelSizer, common::sizers::ControlDefault);

    /* Sizer for left hand size controls */
    auto leftSizer = new wxBoxSizer(wxVERTICAL);
    mainPanelSizer->Add(leftSizer, 0);

    /* Sizer for right hand size controls */
    auto rightSizer = new wxBoxSizer(wxVERTICAL);
    mainPanelSizer->Add(rightSizer, 0);

    /* Sizer for bottom controls */
    auto mainBottomSizer = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(mainBottomSizer, wxSizerFlags().Border(wxALL, 5).Expand());

    auto bottomSizer = new wxBoxSizer(wxVERTICAL);
    mainBottomSizer->Add(bottomSizer, 1);

    /* Left Sizer */
    /* Date Range static box */
    auto dateRangeStaticBox = new wxStaticBox(this, wxID_ANY, "Date Range");
    auto dateRangeStaticBoxSizer = new wxStaticBoxSizer(dateRangeStaticBox, wxVERTICAL);
    leftSizer->Add(dateRangeStaticBoxSizer, common::sizers::ControlExpand);

    /* Date Range panel */
    auto dateRangePanel = new wxPanel(this, wxID_STATIC);
    dateRangeStaticBoxSizer->Add(dateRangePanel, common::sizers::ControlExpand);

    auto flexGridSizer = new wxFlexGridSizer(0, 2, 0, 0);
    dateRangePanel->SetSizer(flexGridSizer);

    /* Start date picker */
    auto startDateLabel = new wxStaticText(dateRangePanel, wxID_ANY, "Start");
    flexGridSizer->Add(startDateLabel, common::sizers::ControlCenter);

    pStartDateCtrl = new wxDatePickerCtrl(
        dateRangePanel, IDC_STARTDATE, wxDefaultDateTime, wxDefaultPosition, wxSize(150, -1), wxDP_DROPDOWN);
    pStartDateCtrl->SetToolTip("Set the start date range for the export");
    flexGridSizer->Add(pStartDateCtrl, common::sizers::ControlDefault);

    /* End date picker */
    auto endDateLabel = new wxStaticText(dateRangePanel, wxID_ANY, "End");
    flexGridSizer->Add(endDateLabel, common::sizers::ControlCenter);

    pEndDateCtrl = new wxDatePickerCtrl(
        dateRangePanel, IDC_ENDDATE, wxDefaultDateTime, wxDefaultPosition, wxSize(150, -1), wxDP_DROPDOWN);
    pEndDateCtrl->SetToolTip("Set the end date range for the export");
    flexGridSizer->Add(pEndDateCtrl, common::sizers::ControlDefault);

    dateRangeStaticBoxSizer->AddSpacer(6);

    /* Options static box */
    auto optionsStaticBox = new wxStaticBox(this, wxID_ANY, "Options");
    auto optionsStaticBoxSizer = new wxStaticBoxSizer(optionsStaticBox, wxVERTICAL);
    leftSizer->Add(optionsStaticBoxSizer, common::sizers::ControlExpand);

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

    /* Right Sizer */
    /* File Options static box*/
    auto fileOptionsStaticBox = new wxStaticBox(this, wxID_ANY, "File Options");
    auto fileOptionsStaticBoxSizer = new wxStaticBoxSizer(fileOptionsStaticBox, wxVERTICAL);
    rightSizer->Add(fileOptionsStaticBoxSizer, common::sizers::ControlExpand);

    /* Sizer for export path selection */
    auto exportPathSizer = new wxBoxSizer(wxHORIZONTAL);
    fileOptionsStaticBoxSizer->Add(exportPathSizer, common::sizers::ControlDefault);

    /* Export path label */
    auto exportPathLabel = new wxStaticText(fileOptionsStaticBox, wxID_ANY, "Export Path");
    exportPathSizer->Add(exportPathLabel, common::sizers::ControlCenter);

    /* Export File Path text control */
    pExportFilePathCtrl =
        new wxTextCtrl(fileOptionsStaticBox, IDC_EXPORTPATH, wxGetEmptyString(), wxDefaultPosition, wxSize(256, -1), 0);
    pExportFilePathCtrl->SetEditable(false);
    exportPathSizer->Add(pExportFilePathCtrl, common::sizers::ControlDefault);

    /* Browse Export Path button */
    pBrowseExportPathButton = new wxButton(fileOptionsStaticBox, IDC_EXPORTPATHBUTTON, "Browse...");
    exportPathSizer->Add(pBrowseExportPathButton, common::sizers::ControlDefault);

    /* Sizer for file name input */
    auto exportFileSizer = new wxBoxSizer(wxHORIZONTAL);
    fileOptionsStaticBoxSizer->Add(exportFileSizer, common::sizers::ControlDefault);

    /* Export file label*/
    auto exportFileLabel = new wxStaticText(fileOptionsStaticBox, wxID_ANY, "File Name  ");
    exportFileSizer->Add(exportFileLabel, common::sizers::ControlCenter);

    /* Export File Name text control */
    pExportFileNameCtrl = new wxTextCtrl(
        fileOptionsStaticBox, IDC_EXPORTFILE, wxGetEmptyString(), wxDefaultPosition, wxSize(150, -1), wxTE_LEFT);
    pExportFileNameCtrl->SetHint("Exported Data.csv");
    pExportFileNameCtrl->SetToolTip("Specify a file name for the exported data (can be blank)");
    exportFileSizer->Add(pExportFileNameCtrl, common::sizers::ControlDefault);

    /* Feedback label */
    rightSizer->AddSpacer(28);
    pFeedbackLabel = new wxStaticText(this, IDC_FEEDBACK, wxGetEmptyString());
    rightSizer->Add(pFeedbackLabel, common::sizers::ControlCenterHorizontal);

    /* Bottom */
    /* Horizontal Line*/
    auto bottomSeparationLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(3, 3), wxLI_HORIZONTAL);
    bottomSizer->Add(bottomSeparationLine, wxSizerFlags().Expand());

    /* Button panel */
    auto buttonPanelSizer = new wxBoxSizer(wxHORIZONTAL);

    auto buttonPanel = new wxPanel(this, wxID_STATIC);
    buttonPanel->SetSizer(buttonPanelSizer);

    bottomSizer->Add(buttonPanel, common::sizers::ControlRight);

    pExportButton = new wxButton(buttonPanel, IDC_EXPORTBUTTON, "Export");
    buttonPanelSizer->Add(pExportButton, common::sizers::ControlDefault);

    pOkButton = new wxButton(buttonPanel, wxID_OK, "OK");
    buttonPanelSizer->Add(pOkButton, wxSizerFlags().Border(wxALL, 5));
}

// clang-format off
void ExportToCsvDialog::ConfigureEventBindings()
{
    pStartDateCtrl->Bind(
        wxEVT_KILL_FOCUS,
        &ExportToCsvDialog::OnStartDateFocusLost,
        this
    );

    pEndDateCtrl->Bind(
        wxEVT_KILL_FOCUS,
        &ExportToCsvDialog::OnEndDateFocusLost,
        this
    );

    pDelimiterTextCtrl->Bind(
        wxEVT_TEXT,
        &ExportToCsvDialog::OnDelimiterChange,
        this
    );

    pBrowseExportPathButton->Bind(
        wxEVT_BUTTON,
        &ExportToCsvDialog::OnOpenDirectoryForExportLocation,
        this,
        IDC_EXPORTPATHBUTTON
    );

    pExportButton->Bind(
        wxEVT_BUTTON,
        &ExportToCsvDialog::OnExport,
        this,
        IDC_EXPORTBUTTON
    );
}
// clang-format on

void ExportToCsvDialog::FillControls()
{
    const wxString DefaultDelimiter = ",";
    auto configDelimiter = cfg::ConfigurationProvider::Get().Configuration->GetDelimiter();
    if (configDelimiter.empty()) {
        pDelimiterTextCtrl->ChangeValue(DefaultDelimiter);
    } else {
        pDelimiterTextCtrl->ChangeValue(configDelimiter);
    }

    auto exportPath = cfg::ConfigurationProvider::Get().Configuration->GetExportPath();
    if (exportPath.length() > 0 && wxDirExists(exportPath)) {
        pExportFilePathCtrl->ChangeValue(exportPath);
        pExportFilePathCtrl->SetToolTip(exportPath);
    }
}

void ExportToCsvDialog::OnStartDateFocusLost(wxFocusEvent& event)
{
    DateValidationProcedure();
    event.Skip();
}

void ExportToCsvDialog::OnEndDateFocusLost(wxFocusEvent& event)
{
    DateValidationProcedure();
    event.Skip();
}

void ExportToCsvDialog::OnOpenDirectoryForExportLocation(wxCommandEvent& event)
{
    wxString pathDirectory = wxGetEmptyString();
    if (cfg::ConfigurationProvider::Get().Configuration->GetExportPath().length() == 0 &&
        !wxDirExists(cfg::ConfigurationProvider::Get().Configuration->GetExportPath())) {
        pathDirectory = wxStandardPaths::Get().GetAppDocumentsDir();
    } else {
        pathDirectory = cfg::ConfigurationProvider::Get().Configuration->GetExportPath();
    }

    auto openDirDialog =
        new wxDirDialog(this, "Select a Export Directory", pathDirectory, wxDD_DEFAULT_STYLE, wxDefaultPosition);
    int res = openDirDialog->ShowModal();

    if (res == wxID_OK) {
        auto selectedExportPath = openDirDialog->GetPath();
        pExportFilePathCtrl->SetValue(selectedExportPath);
        pExportFilePathCtrl->SetToolTip(selectedExportPath);
        cfg::ConfigurationProvider::Get().Configuration->SetExportPath(selectedExportPath.ToStdString());
        cfg::ConfigurationProvider::Get().Configuration->Save();
    }

    openDirDialog->Destroy();
}

void ExportToCsvDialog::OnExport(wxCommandEvent& event)
{
    /* check if dates are correctly selected */
    auto start = pStartDateCtrl->GetValue();
    auto end = pEndDateCtrl->GetValue();

    if (start.IsLaterThan(end)) {
        return;
    }

    if (end.IsEarlierThan(start)) {
        return;
    }

    /* format dates to ISO strings */
    auto startDate = start.FormatISODate();
    auto endDate = end.FormatISODate();

    /* get the export path and validate it */
    auto exportPath = pExportFilePathCtrl->GetValue();
    if (exportPath.empty() || !wxDirExists(exportPath)) {
        wxRichToolTip tooltip("Export Path", "Please specify a valid export directory");
        tooltip.SetIcon(wxICON_WARNING);
        tooltip.ShowFor(pExportFilePathCtrl);
        return;
    }

    /* get the filename and validate or generate it */
    auto fileName = pExportFileNameCtrl->GetValue();
    if (fileName.empty()) {
        fileName = "Taskable_Export_" + startDate + "_" + endDate + ".csv";
    } else {
        if (!fileName.Contains(".csv")) {
            fileName += ".csv";
        }
    }

    wxBeginBusyCursor();

    /* do the export and display the operation output */
    svc::CsvExporter csvExporter(pLogger, startDate.ToStdString(), endDate.ToStdString(), fileName.ToStdString());
    if (csvExporter.ExportData()) {
        pFeedbackLabel->SetLabel("Success! Click 'OK' to close the dialog.");
    } else {
        wxString errorMessage = "Data export encountered an error!";
        pFeedbackLabel->SetLabel(errorMessage);
    }

    wxEndBusyCursor();

    // TODO: Wrap this if Windows is defined
    {
        /* open file explorer */
        ShellExecute(nullptr, wxT("open"), exportPath.c_str(), nullptr, nullptr, SW_SHOWDEFAULT);
    }

    GetSizer()->Layout();
}

void ExportToCsvDialog::OnDelimiterChange(wxCommandEvent& event)
{
    auto text = event.GetString();
    if (!text.empty()) {
        cfg::ConfigurationProvider::Get().Configuration->SetDelimiter(text.ToStdString());
        cfg::ConfigurationProvider::Get().Configuration->Save();
    }
}

void ExportToCsvDialog::DateValidationProcedure()
{
    auto startDate = pStartDateCtrl->GetValue();
    auto endDate = pEndDateCtrl->GetValue();

    if (startDate.IsLaterThan(endDate)) {
        wxRichToolTip tooltip("Start Date", "End date cannot go before start date");
        tooltip.SetIcon(wxICON_WARNING);
        tooltip.ShowFor(pStartDateCtrl);
    }

    if (endDate.IsEarlierThan(startDate)) {
        wxRichToolTip tooltip("End Date", "End date cannot precede start date");
        tooltip.SetIcon(wxICON_WARNING);
        tooltip.ShowFor(pEndDateCtrl);
    }
}
} // namespace app::dlg
