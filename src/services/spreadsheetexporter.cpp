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

#include "spreadsheetexporter.h"

#include <algorithm>

#include "../config/configurationprovider.h"

namespace app::svc
{
XmlExporter::XmlExporter(std::vector<SpreadsheetDataSet*>& dataSets,
    const std::vector<std::string>& additionalColumns,
    std::string filename)
    : mDataSets(dataSets)
    , mAdditionalColumns(additionalColumns)
    , mFilename(filename)
{
}

// https://en.wikipedia.org/wiki/Microsoft_Office_XML_formats#Excel_XML_Spreadsheet_example
bool XmlExporter::Export()
{
    tinyxml2::XMLDocument xmlDocument;
    tinyxml2::XMLDeclaration* xmlDeclaration = xmlDocument.NewDeclaration();
    xmlDocument.InsertFirstChild(xmlDeclaration);

    const char* progId = "<?mso-application progid=\"Excel.Sheet\"?>";
    xmlDocument.Parse(progId);

    tinyxml2::XMLElement* workbookNode = xmlDocument.NewElement("Workbook");
    workbookNode->SetAttribute("xmlns", "urn:schemas-microsoft-com:office:spreadsheet");
    workbookNode->SetAttribute("xmlns:x", "urn:schemas-microsoft-com:office:excel");
    workbookNode->SetAttribute("xmlns:ss", "urn:schemas-microsoft-com:office:spreadsheet");
    workbookNode->SetAttribute("xmlns:html", "https://www.w3.org/TR/html401/");

    tinyxml2::XMLElement* worksheetNode = xmlDocument.NewElement("Worksheet");
    worksheetNode->SetAttribute("ss:Name", "Taskable");
    workbookNode->InsertEndChild(worksheetNode);

    tinyxml2::XMLElement* tableNode = xmlDocument.NewElement("Table");
    worksheetNode->InsertEndChild(tableNode);

    tinyxml2::XMLElement* columnNode = xmlDocument.NewElement("Column");
    columnNode->SetAttribute("ss:Index", "1");
    columnNode->SetAttribute("ss:AutoFitWidth", "0");
    columnNode->SetAttribute("ss:Width", "110");
    tableNode->InsertEndChild(columnNode);

    tinyxml2::XMLElement* rowNode = xmlDocument.NewElement("Row");

    for (const auto& columnName : DefaultColumns) {
        tinyxml2::XMLElement* cellNode = xmlDocument.NewElement("Cell");
        tinyxml2::XMLElement* dataNode = xmlDocument.NewElement("Data");
        dataNode->SetAttribute("ss:Type", "String");
        dataNode->SetText(columnName.c_str());
        cellNode->InsertEndChild(dataNode);
        rowNode->InsertEndChild(cellNode);
    }

    for (const auto& additionalColumnName : mAdditionalColumns) {
        tinyxml2::XMLElement* cellNode = xmlDocument.NewElement("Cell");
        tinyxml2::XMLElement* dataNode = xmlDocument.NewElement("Data");
        dataNode->SetAttribute("ss:Type", "String");
        dataNode->SetText(additionalColumnName.c_str());
        cellNode->InsertEndChild(dataNode);
        rowNode->InsertEndChild(cellNode);
    }

    tableNode->InsertEndChild(rowNode);

    for (auto dataSet : mDataSets) {
        rowNode = xmlDocument.NewElement("Row");
        // TaskItemId
        AddNumberNode(xmlDocument, rowNode, dataSet->TaskItemId);

        // StartTime
        AddStringNode(xmlDocument, rowNode, dataSet->StartTime ? *dataSet->StartTime : "");

        // EndTime
        AddStringNode(xmlDocument, rowNode, dataSet->EndTime ? *dataSet->EndTime : "");

        // Duration
        AddStringNode(xmlDocument, rowNode, dataSet->Duration);

        // Description
        AddStringNode(xmlDocument, rowNode, dataSet->Description);

        // TaskItemType
        AddStringNode(xmlDocument, rowNode, dataSet->TaskItemType);

        // ProjectName
        AddStringNode(xmlDocument, rowNode, dataSet->ProjectName);

        // CategoryName
        AddStringNode(xmlDocument, rowNode, dataSet->CategoryName);

        // TaskDate
        AddStringNode(xmlDocument, rowNode, dataSet->TaskDate);

        if (!mAdditionalColumns.empty()) {
            if (std::find(mAdditionalColumns.begin(), mAdditionalColumns.end(), "Employer") !=
                mAdditionalColumns.end()) {
                AddStringNode(xmlDocument, rowNode, dataSet->EmployerName ? *dataSet->EmployerName : "");
            }
            if (std::find(mAdditionalColumns.begin(), mAdditionalColumns.end(), "Client") != mAdditionalColumns.end()) {
                AddStringNode(xmlDocument, rowNode, dataSet->ClientName ? *dataSet->ClientName : "");
            }
            if (std::find(mAdditionalColumns.begin(), mAdditionalColumns.end(), "Billable") !=
                mAdditionalColumns.end()) {
                AddBooleanNode(xmlDocument, rowNode, dataSet->Billable ? *dataSet->Billable : false);
            }
            if (std::find(mAdditionalColumns.begin(), mAdditionalColumns.end(), "Rate") != mAdditionalColumns.end()) {
                AddDoubleNode(xmlDocument, rowNode, dataSet->Rate ? *dataSet->Rate : 0.0);
            }
            if (std::find(mAdditionalColumns.begin(), mAdditionalColumns.end(), "Currency") !=
                mAdditionalColumns.end()) {
                AddStringNode(xmlDocument, rowNode, dataSet->Currency ? *dataSet->Currency : "");
            }
            if (std::find(mAdditionalColumns.begin(), mAdditionalColumns.end(), "Calculated Task Rate") !=
                mAdditionalColumns.end()) {
                AddDoubleNode(xmlDocument, rowNode, dataSet->CalculatedRate ? *dataSet->CalculatedRate : 0.0);
            }
        }
        tableNode->InsertEndChild(rowNode);
    }

    xmlDocument.InsertEndChild(workbookNode);

    auto exportPath = cfg::ConfigurationProvider::Get().Configuration->GetExportPath();
    auto fullFilePath = exportPath + "\\" + mFilename + ".xml";

    tinyxml2::XMLError result = xmlDocument.SaveFile(fullFilePath.c_str());

    return true;
}

void XmlExporter::AddNumberNode(tinyxml2::XMLDocument& xmlDocument, tinyxml2::XMLElement* rowNode, int64_t value)
{
    tinyxml2::XMLElement* cellNode = xmlDocument.NewElement("Cell");
    tinyxml2::XMLElement* dataNode = xmlDocument.NewElement("Data");
    dataNode->SetAttribute("ss:Type", "Number");
    dataNode->SetText(value);
    cellNode->InsertEndChild(dataNode);
    rowNode->InsertEndChild(cellNode);
}

void XmlExporter::AddDoubleNode(tinyxml2::XMLDocument& xmlDocument, tinyxml2::XMLElement* rowNode, double value)
{
    tinyxml2::XMLElement* cellNode = xmlDocument.NewElement("Cell");
    tinyxml2::XMLElement* dataNode = xmlDocument.NewElement("Data");
    dataNode->SetAttribute("ss:Type", "Number");
    dataNode->SetText(value);
    cellNode->InsertEndChild(dataNode);
    rowNode->InsertEndChild(cellNode);
}

void XmlExporter::AddStringNode(tinyxml2::XMLDocument& xmlDocument, tinyxml2::XMLElement* rowNode, std::string value)
{
    tinyxml2::XMLElement* cellNode = xmlDocument.NewElement("Cell");
    tinyxml2::XMLElement* dataNode = xmlDocument.NewElement("Data");
    dataNode->SetAttribute("ss:Type", "String");
    dataNode->SetText(value.c_str());
    cellNode->InsertEndChild(dataNode);
    rowNode->InsertEndChild(cellNode);
}

void XmlExporter::AddBooleanNode(tinyxml2::XMLDocument& xmlDocument, tinyxml2::XMLElement* rowNode, bool value)
{
    tinyxml2::XMLElement* cellNode = xmlDocument.NewElement("Cell");
    tinyxml2::XMLElement* dataNode = xmlDocument.NewElement("Data");
    dataNode->SetAttribute("ss:Type", "Boolean");
    dataNode->SetText(value);
    cellNode->InsertEndChild(dataNode);
    rowNode->InsertEndChild(cellNode);
}

ExcelExporter::ExcelExporter(std::vector<SpreadsheetDataSet*> dataSets) {}

bool ExcelExporter::Export()
{
    return false;
}

SpreadsheetExporter::SpreadsheetExporter(std::shared_ptr<spdlog::logger> logger, SpreadsheetExportOptions& options)
    : pLogger(logger)
    , mOptions(options)
    , pExporterFactory(nullptr)
    , mQuery()
{
    pConnection = db::ConnectionProvider::Get().Handle()->Acquire();

    BuildQuery();
}

SpreadsheetExporter::~SpreadsheetExporter()
{
    db::ConnectionProvider::Get().Handle()->Release(pConnection);
    delete pExporterFactory;
}

bool SpreadsheetExporter::Export()
{
    /* get raw data from database */
    std::vector<SpreadsheetDataSet*> dataSets;
    try {
        dataSets = GetDataSet();
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in SpreadsheetExporter::Export - {0:d} : {1}", e.get_code(), e.what());
        return false;
    }

    if (dataSets.empty()) {
        return true;
    }

    /* initialize relevant factory type */
    switch (mOptions.Format) {
    case constants::ExportFormats::OfficeXml:
        break;
    case constants::ExportFormats::Excel:
        break;
    default:
        break;
    }

    pExporterFactory = new XmlExporter(dataSets, mOptions.AdditionalColumns, mOptions.FileName);

    return pExporterFactory->Export();
}

void SpreadsheetExporter::BuildQuery()
{
    std::string selectStatement = "SELECT "
                                  "  task_items.task_item_id "
                                  ", task_items.start_time "
                                  ", task_items.end_time "
                                  ", task_items.duration "
                                  ", task_items.description "
                                  ", task_item_types.name "
                                  ", projects.name "
                                  ", categories.name "
                                  ", tasks.task_date ";

    std::string fromStatement = "FROM task_items ";

    std::string joinsStatement = "INNER JOIN task_item_types "
                                 "ON task_items.task_item_type_id = task_item_types.task_item_type_id "
                                 "INNER JOIN projects "
                                 "ON task_items.project_id = projects.project_id "
                                 "INNER JOIN categories "
                                 "ON task_items.category_id = categories.category_id "
                                 "INNER JOIN tasks "
                                 "ON task_items.task_id = tasks.task_id ";

    std::string whereStatement = "WHERE tasks.task_date >= ? "
                                 "AND tasks.task_date <= ? "
                                 "AND task_items.is_active = 1";

    if (!mOptions.AdditionalColumns.empty()) {
        if (std::find(mOptions.AdditionalColumns.begin(), mOptions.AdditionalColumns.end(), "Employer") !=
            mOptions.AdditionalColumns.end()) {
            selectStatement += ", employers.name ";
            joinsStatement += "INNER JOIN employers "
                              "ON projects.employer_id = employers.employer_id";
        }
        if (std::find(mOptions.AdditionalColumns.begin(), mOptions.AdditionalColumns.end(), "Client") !=
            mOptions.AdditionalColumns.end()) {
            selectStatement += ", clients.name ";
            joinsStatement += "INNER JOIN clients "
                              "ON projects.client_id = clients.client_id";
        }
        if (std::find(mOptions.AdditionalColumns.begin(), mOptions.AdditionalColumns.end(), "Billable") !=
            mOptions.AdditionalColumns.end()) {
            selectStatement += ", projects.billable ";
        }
        if (std::find(mOptions.AdditionalColumns.begin(), mOptions.AdditionalColumns.end(), "Rate") !=
            mOptions.AdditionalColumns.end()) {
            selectStatement += ", projects.rate ";
        }
        if (std::find(mOptions.AdditionalColumns.begin(), mOptions.AdditionalColumns.end(), "Currency") !=
            mOptions.AdditionalColumns.end()) {
            selectStatement += ", currencies.name ";
            joinsStatement += "INNER JOIN currencies "
                              "ON projects.currency_id = currencies.currency_id";
        }
        if (std::find(mOptions.AdditionalColumns.begin(), mOptions.AdditionalColumns.end(), "Calculated Task Rate") !=
            mOptions.AdditionalColumns.end()) {
            selectStatement += ", task_items.calculated_rate ";
        }
    }

    mQuery = selectStatement + fromStatement + joinsStatement + whereStatement;
}

std::vector<SpreadsheetDataSet*> SpreadsheetExporter::GetDataSet()
{
    std::vector<SpreadsheetDataSet*> dataSets;

    *pConnection->DatabaseExecutableHandle() << mQuery << mOptions.FromDate << mOptions.ToDate >>
        [&](int64_t taskItemsTaskItemId,
            std::unique_ptr<std::string> taskItemsStartTime,
            std::unique_ptr<std::string> taskItemsEndTime,
            std::string taskItemsDuration,
            std::string taskItemsDescription,
            std::string taskItemTypesName,
            std::string projectsName,
            std::string categoriesName,
            std::string tasksTaskDate,
            std::unique_ptr<std::string> employersEmployerName,
            std::unique_ptr<std::string> clientsClientName,
            std::unique_ptr<bool> projectsBillable,
            std::unique_ptr<double> taskItemsCalculatedRate,
            std::unique_ptr<double> projectsRate,
            std::unique_ptr<std::string> currenciesCurrencyName) {
            SpreadsheetDataSet* dataSet = new SpreadsheetDataSet;

            /* Required fields */
            dataSet->TaskItemId = taskItemsTaskItemId;
            dataSet->StartTime = std::move(taskItemsStartTime);
            dataSet->EndTime = std::move(taskItemsEndTime);
            dataSet->Duration = taskItemsDuration;
            dataSet->Description = taskItemsDescription;
            dataSet->TaskItemType = taskItemTypesName;
            dataSet->ProjectName = projectsName;
            dataSet->CategoryName = categoriesName;
            dataSet->TaskDate = tasksTaskDate;

            /* Optional fields */
            dataSet->EmployerName = std::move(employersEmployerName);
            dataSet->ClientName = std::move(clientsClientName);
            dataSet->CalculatedRate = std::move(taskItemsCalculatedRate);
            dataSet->Billable = std::move(projectsBillable);
            dataSet->Rate = std::move(projectsRate);
            dataSet->Currency = std::move(currenciesCurrencyName);

            dataSets.push_back(dataSet);
        };

    return dataSets;
}

} // namespace app::svc
