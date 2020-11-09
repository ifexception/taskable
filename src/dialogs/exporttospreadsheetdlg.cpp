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

#include <wx/statline.h>

#include "../common/common.h"
#include "../common/resources.h"

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
    , pExportFileFormat(nullptr)
    , pExportFilePathCtrl(nullptr)
    , pBrowseExportPathButton(nullptr)
    , pExportFileNameCtrl(nullptr)
    , pExportButton(nullptr)
    , pOkButton(nullptr)
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
    /* Column static box */
    auto columnOptionsStaticBox = new wxStaticBox(this, wxID_ANY, "Column Options");
    auto columnOptionsStaticBoxSizer = new wxStaticBoxSizer(columnOptionsStaticBox, wxHORIZONTAL);
    leftSizer->Add(columnOptionsStaticBoxSizer, common::sizers::ControlExpand);

    /* Sizer for add columns controls */
    auto addColumnsSizer = new wxBoxSizer(wxVERTICAL);
    columnOptionsStaticBoxSizer->Add(addColumnsSizer, common::sizers::ControlDefault);

    /* Add column choice label */
    auto addColumnsLabel = new wxStaticText(columnOptionsStaticBox, wxID_ANY, "Additional Columns to Add");
    addColumnsSizer->Add(addColumnsLabel, common::sizers::ControlDefault);

    /* Add column choice control */
    pAddColumnsChoice = new wxChoice(columnOptionsStaticBox, IDC_ADDCOLUMNS, wxDefaultPosition, wxSize(150, -1));
    pAddColumnsChoice->AppendString("Select a column");
    pAddColumnsChoice->SetSelection(0);
    pAddColumnsChoice->SetToolTip("Optionally select additional columns to include in the export");
    addColumnsSizer->Add(pAddColumnsChoice, common::sizers::ControlDefault);

    /* Add column button control*/
    pAddColumnButton = new wxButton(columnOptionsStaticBox, IDC_ADDCOLUMNBUTTON, "+", wxDefaultPosition, wxSize(32, -1));
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
    pExportFileFormat =
        new wxChoice(spreadsheetOptionsStaticBox, IDC_EXPORTFILEFORMAT, wxDefaultPosition, wxSize(150, -1));
    pExportFileFormat->AppendString("Select a extension");
    pExportFileFormat->SetSelection(0);
    pExportFileFormat->SetToolTip("Select a file extension to export to");
    exportFlexGridSizer->Add(pExportFileFormat, common::sizers::ControlDefault);

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

void ExportToSpreadsheetDialog::ConfigureEventBindings() {}

void ExportToSpreadsheetDialog::FillControls() {}

} // namespace app::dlg
