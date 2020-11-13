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

#include "exporttospreadsheetdlg.h"

#include <wx/richtooltip.h>
#include <wx/statline.h>
#include <wx/stdpaths.h>
#include <wx/utils.h>

#include "../common/common.h"
#include "../common/resources.h"
#include "../common/util.h"

#include "../config/configurationprovider.h"

#include "../services/spreadsheetexporter.h"

namespace app::dlg
{
ExportToSpreadsheetDialog::ExportToSpreadsheetDialog(wxWindow* parent,
    std::shared_ptr<spdlog::logger> logger,
    const wxString name)
    : pLogger(logger)
    , pAddColumnsChoice(nullptr)
    , pAddColumnButton(nullptr)
    , pColumnListCtrl(nullptr)
    , pExportFormatChoice(nullptr)
    , pExportFileFormatChoice(nullptr)
    , pExportFilePathCtrl(nullptr)
    , pBrowseExportPathButton(nullptr)
    , pExportFileNameCtrl(nullptr)
    , pExportButton(nullptr)
    , pOkButton(nullptr)
    , exportFormatOptions{ { "Office XML", constants::ExportFormats::OfficeXml },
        { "Excel", constants::ExportFormats::Excel } }
{
    Create(parent, wxID_ANY, "Export to Spreadsheet", wxDefaultPosition, wxDefaultSize, wxCAPTION | wxCLOSE_BOX, name);
}

bool ExportToSpreadsheetDialog::Create(wxWindow* parent,
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
        SetIcon(rc::GetProgramIcon());
        Centre();
    }
    return false;
}

void ExportToSpreadsheetDialog::CreateControls()
{
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

    auto flexGridSizer = new wxFlexGridSizer(0, 2, 0, 0);
    dateRangeStaticBoxSizer->Add(flexGridSizer, common::sizers::ControlDefault);

    /* Start date picker */
    auto startDateLabel = new wxStaticText(dateRangeStaticBox, wxID_ANY, "Start");
    flexGridSizer->Add(startDateLabel, common::sizers::ControlCenter);

    pStartDateCtrl = new wxDatePickerCtrl(
        dateRangeStaticBox, IDC_STARTDATE, wxDefaultDateTime, wxDefaultPosition, wxSize(150, -1), wxDP_DROPDOWN);
    pStartDateCtrl->SetToolTip("Set the start date range for the export");
    flexGridSizer->Add(pStartDateCtrl, common::sizers::ControlDefault);

    /* End date picker */
    auto endDateLabel = new wxStaticText(dateRangeStaticBox, wxID_ANY, "End");
    flexGridSizer->Add(endDateLabel, common::sizers::ControlCenter);

    pEndDateCtrl = new wxDatePickerCtrl(
        dateRangeStaticBox, IDC_ENDDATE, wxDefaultDateTime, wxDefaultPosition, wxSize(150, -1), wxDP_DROPDOWN);
    pEndDateCtrl->SetToolTip("Set the end date range for the export");
    flexGridSizer->Add(pEndDateCtrl, common::sizers::ControlDefault);

    /* Column static box */
    auto columnOptionsStaticBox = new wxStaticBox(this, wxID_ANY, "Column Options");
    auto columnOptionsStaticBoxSizer = new wxStaticBoxSizer(columnOptionsStaticBox, wxHORIZONTAL);
    leftSizer->Add(columnOptionsStaticBoxSizer, common::sizers::ControlExpand);

    /* Sizer for add columns controls */
    auto addColumnsSizer = new wxBoxSizer(wxVERTICAL);
    columnOptionsStaticBoxSizer->Add(addColumnsSizer, common::sizers::ControlDefault);

    /* Add column choice label */
    auto addColumnsLabel = new wxStaticText(columnOptionsStaticBox, wxID_ANY, "Additional Columns");
    addColumnsSizer->Add(addColumnsLabel, common::sizers::ControlDefault);

    /* Add column choice control */
    pAddColumnsChoice = new wxChoice(columnOptionsStaticBox, IDC_ADDCOLUMNS, wxDefaultPosition, wxSize(150, -1));
    pAddColumnsChoice->AppendString("Select a column");
    pAddColumnsChoice->SetSelection(0);
    pAddColumnsChoice->SetToolTip("Optionally select additional columns to include in the export");
    addColumnsSizer->Add(pAddColumnsChoice, common::sizers::ControlDefault);

    /* Add column button control*/
    pAddColumnButton =
        new wxButton(columnOptionsStaticBox, IDC_ADDCOLUMNBUTTON, "+", wxDefaultPosition, wxSize(32, -1));
    pAddColumnButton->SetFont(
        wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxEmptyString));
    addColumnsSizer->Add(pAddColumnButton, common::sizers::ControlRight);

    /* Column list control */
    pColumnListCtrl = new wxListCtrl(
        columnOptionsStaticBox, IDC_COLUMNLIST, wxDefaultPosition, wxSize(200, 186), wxLC_REPORT | wxLC_HRULES);

    wxListItem nameColumn;
    nameColumn.SetAlign(wxLIST_FORMAT_CENTER);
    nameColumn.SetText("Columns");
    long mask = nameColumn.GetMask();
    mask |= wxLIST_MASK_FORMAT;
    nameColumn.SetId(0);
    nameColumn.SetMask(mask);
    nameColumn.SetWidth(wxLIST_AUTOSIZE);
    pColumnListCtrl->InsertColumn(0, nameColumn);

    columnOptionsStaticBoxSizer->Add(pColumnListCtrl, 1, wxEXPAND | wxALL, 5);

    /* Right Sizer */
    /* Spreadsheet options static box*/
    auto spreadsheetOptionsStaticBox = new wxStaticBox(this, wxID_ANY, "Spreadsheet Options");
    auto spreadsheetOptionsStaticBoxSizer = new wxStaticBoxSizer(spreadsheetOptionsStaticBox, wxVERTICAL);
    rightSizer->Add(spreadsheetOptionsStaticBoxSizer, common::sizers::ControlExpand);

    auto exportFlexGridSizer = new wxFlexGridSizer(0, 2, 0, 0);
    spreadsheetOptionsStaticBoxSizer->Add(exportFlexGridSizer, common::sizers::ControlDefault);

    /* Export format label*/
    auto exportFormatLabel = new wxStaticText(spreadsheetOptionsStaticBox, wxID_ANY, "Export Formats");
    exportFlexGridSizer->Add(exportFormatLabel, common::sizers::ControlCenterVertical);

    /* Export Format choice */
    pExportFormatChoice =
        new wxChoice(spreadsheetOptionsStaticBox, IDC_EXPORTFORMATCHOICE, wxDefaultPosition, wxSize(150, -1));
    pExportFormatChoice->AppendString("Select a format");
    pExportFormatChoice->SetSelection(0);
    pExportFormatChoice->SetToolTip("Select a format to export the data to");
    exportFlexGridSizer->Add(pExportFormatChoice, common::sizers::ControlDefault);

    /* Export File label */
    auto extensionTypeLabel = new wxStaticText(spreadsheetOptionsStaticBox, wxID_ANY, "File Extension");
    exportFlexGridSizer->Add(extensionTypeLabel, common::sizers::ControlCenterVertical);

    /* Export File choice */
    pExportFileFormatChoice =
        new wxChoice(spreadsheetOptionsStaticBox, IDC_EXPORTFILEFORMAT, wxDefaultPosition, wxSize(150, -1));
    pExportFileFormatChoice->AppendString("Select a extension");
    pExportFileFormatChoice->SetSelection(0);
    pExportFileFormatChoice->SetToolTip("Select a file extension to export to");
    pExportFileFormatChoice->Disable();
    exportFlexGridSizer->Add(pExportFileFormatChoice, common::sizers::ControlDefault);

    exportFlexGridSizer->AddSpacer(4);

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
    auto exportFileLabel = new wxStaticText(fileOptionsStaticBox, wxID_ANY, "File Name   ");
    exportFileSizer->Add(exportFileLabel, common::sizers::ControlCenter);

    /* Export File Name text control */
    pExportFileNameCtrl = new wxTextCtrl(
        fileOptionsStaticBox, IDC_EXPORTFILE, wxGetEmptyString(), wxDefaultPosition, wxSize(150, -1), wxTE_LEFT);
    pExportFileNameCtrl->SetHint("Exported Data.xls");
    pExportFileNameCtrl->SetToolTip("Specify a file name for the exported data (can be blank)");
    exportFileSizer->Add(pExportFileNameCtrl, common::sizers::ControlDefault);

    /* Bottom */
    /* Horizontal Line*/
    auto bottomSeparationLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(2, 2), wxLI_HORIZONTAL);
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
void ExportToSpreadsheetDialog::ConfigureEventBindings()
{
    pStartDateCtrl->Bind(
        wxEVT_KILL_FOCUS,
        &ExportToSpreadsheetDialog::OnStartDateFocusLost,
        this
    );

    pEndDateCtrl->Bind(
        wxEVT_KILL_FOCUS,
        &ExportToSpreadsheetDialog::OnEndDateFocusLost,
        this
    );

    pAddColumnButton->Bind(
        wxEVT_BUTTON,
        &ExportToSpreadsheetDialog::OnAddColumn,
        this,
        IDC_ADDCOLUMNBUTTON
    );

    pExportFormatChoice->Bind(
        wxEVT_CHOICE,
        &ExportToSpreadsheetDialog::OnExportFormatChoice,
        this
    );

    pBrowseExportPathButton->Bind(
        wxEVT_BUTTON,
        &ExportToSpreadsheetDialog::OnOpenDirectoryForExportLocation,
        this,
        IDC_EXPORTPATHBUTTON
    );

    pExportButton->Bind(
        wxEVT_BUTTON,
        &ExportToSpreadsheetDialog::OnExport,
        this,
        IDC_EXPORTBUTTON
    );
}
// clang-format on

void ExportToSpreadsheetDialog::FillControls()
{
    for (const auto& exportFormatOption : exportFormatOptions) {
        int integerEnumValue = static_cast<int>(exportFormatOption.Value);
        pExportFormatChoice->Append(exportFormatOption.DisplayValue, util::IntToVoidPointer(integerEnumValue));
    }

    wxString exportPath = cfg::ConfigurationProvider::Get().Configuration->GetExportPath();
    if (exportPath.length() > 0 && wxDirExists(exportPath)) {
        pExportFilePathCtrl->ChangeValue(exportPath);
        pExportFilePathCtrl->SetToolTip(exportPath);
    }
}

void ExportToSpreadsheetDialog::OnStartDateFocusLost(wxFocusEvent& event)
{
    DateValidationProcedure();
    event.Skip();
}

void ExportToSpreadsheetDialog::OnEndDateFocusLost(wxFocusEvent& event)
{
    DateValidationProcedure();
    event.Skip();
}

void ExportToSpreadsheetDialog::OnAddColumn(wxCommandEvent& event)
{
    auto selectedColumn = pAddColumnsChoice->GetSelection();
    if (selectedColumn == 0) {
        wxRichToolTip tooltip("Column", "A valid column selection is required");
        tooltip.SetIcon(wxICON_WARNING);
        tooltip.ShowFor(pAddColumnsChoice);
        return;
    }

    auto selectedColumnName = pAddColumnsChoice->GetStringSelection();

    pColumnListCtrl->InsertItem(0, selectedColumnName);
}

void ExportToSpreadsheetDialog::OnExportFormatChoice(wxCommandEvent& event)
{
    auto selection = event.GetSelection();
    if (selection == 0) {
        wxRichToolTip tooltip("Export Format", "A valid export format selection is required");
        tooltip.SetIcon(wxICON_WARNING);
        tooltip.ShowFor(pExportFormatChoice);
        pExportFileFormatChoice->Disable();
        return;
    }

    pExportFileFormatChoice->Clear();
    pExportFileFormatChoice->AppendString("Select a extension");
    pExportFileFormatChoice->SetSelection(0);

    auto selectionData = event.GetClientData();
    auto data = util::VoidPointerToInt(selectionData);
    auto enumData = static_cast<constants::ExportFormats>(data);

    wxArrayString exportFileOptions;

    if (enumData == constants::ExportFormats::OfficeXml) {
        exportFileOptions.Add("XML");
        exportFileOptions.Add("XLS");
    } else if (enumData == constants::ExportFormats::Excel) {
        exportFileOptions.Add("XLSX");
    }

    pExportFileFormatChoice->Append(exportFileOptions);
    pExportFileFormatChoice->Enable();
}

void ExportToSpreadsheetDialog::OnOpenDirectoryForExportLocation(wxCommandEvent& event)
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

void ExportToSpreadsheetDialog::OnExport(wxCommandEvent& event)
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

    auto formatChoiceSelection =
        util::VoidPointerToInt(pExportFormatChoice->GetClientData(pExportFormatChoice->GetSelection()));
    auto selectedEnum = static_cast<constants::ExportFormats>(formatChoiceSelection);
    auto fileChoiceSelection = pExportFileFormatChoice->GetStringSelection().ToStdString();

    svc::SpreadsheetExportOptions options{ startDate, endDate, selectedEnum, fileChoiceSelection };

    wxBeginBusyCursor();

    svc::SpreadsheetExporter spreadsheetExporter(pLogger, options);

    wxEndBusyCursor();
}

void ExportToSpreadsheetDialog::DateValidationProcedure()
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
