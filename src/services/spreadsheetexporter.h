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

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <spdlog/spdlog.h>

#include <tinyxml2.h>

#include "../common/constants.h"
#include "../database/sqliteconnection.h"
#include "../database/connectionprovider.h"

namespace app::svc
{
const std::vector<std::string> DefaultColumns{ "TaskItemId",
    "StartTime",
    "EndTime",
    "Duration",
    "Description",
    "TaskItemType",
    "ProjectName",
    "CategoryName",
    "TaskDate" };

struct SpreadsheetExportOptions {
    std::string FromDate;
    std::string ToDate;
    constants::ExportFormats Format;
    std::string FileName;
    std::vector<std::string> AdditionalColumns;
};

struct SpreadsheetDataSet {
    /* Default Fields */
    int64_t TaskItemId;
    std::unique_ptr<std::string> StartTime;
    std::unique_ptr<std::string> EndTime;
    std::string Duration;
    std::string Description;
    std::string TaskItemType;
    std::string ProjectName;
    std::string CategoryName;
    std::string TaskDate;
    /* Optional Fields */
    std::unique_ptr<std::string> EmployerName;
    std::unique_ptr<std::string> ClientName;
    std::unique_ptr<bool> Billable;
    std::unique_ptr<double> CalculatedRate;
    std::unique_ptr<double> Rate;
    std::unique_ptr<std::string> Currency;
};

class IExporter
{
public:
    virtual bool Export() = 0;
};

class XmlExporter : public IExporter
{
public:
    XmlExporter(std::vector<SpreadsheetDataSet*>& dataSets, const std::vector<std::string>& additionalColumns, std::string filename);
    virtual ~XmlExporter() = default;

    bool Export() override;

private:
    void AddNumberNode(tinyxml2::XMLDocument& xmlDocument, tinyxml2::XMLElement* rowNode, int64_t value);
    void AddDoubleNode(tinyxml2::XMLDocument& xmlDocument, tinyxml2::XMLElement* rowNode, double value);
    void AddStringNode(tinyxml2::XMLDocument& xmlDocument, tinyxml2::XMLElement* rowNode, std::string value);
    void AddBooleanNode(tinyxml2::XMLDocument& xmlDocument, tinyxml2::XMLElement* rowNode, bool value);

    std::vector<SpreadsheetDataSet*> mDataSets;
    std::vector<std::string> mAdditionalColumns;
    std::string mFilename;
};

class ExcelExporter : public IExporter
{
public:
    ExcelExporter(std::vector<SpreadsheetDataSet*> dataSets);
    virtual ~ExcelExporter() = default;

    bool Export() override;

private:
};

class SpreadsheetExporter
{
public:
    SpreadsheetExporter() = default;
    SpreadsheetExporter(std::shared_ptr<spdlog::logger> logger, SpreadsheetExportOptions& options);
    ~SpreadsheetExporter();

    bool Export();

private:
    void BuildQuery();
    std::vector<SpreadsheetDataSet*> GetDataSet();

    std::shared_ptr<spdlog::logger> pLogger;
    std::shared_ptr<db::SqliteConnection> pConnection;

    SpreadsheetExportOptions mOptions;

    IExporter* pExporterFactory;

    std::string mQuery;
};

} // namespace app::svc
