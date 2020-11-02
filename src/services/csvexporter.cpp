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

#include "csvexporter.h"

#include <fstream>

#include <sqlite_modern_cpp/errors.h>

#include "../config/configurationprovider.h"

namespace app::svc
{
std::string CsvExporter::Query = "SELECT "
                                 "  task_items.start_time "
                                 ", task_items.end_time "
                                 ", task_items.duration "
                                 ", task_items.description "
                                 ", task_items.calculated_rate "
                                 ", task_item_types.name "
                                 ", projects.name "
                                 ", projects.billable "
                                 ", projects.rate "
                                 ", categories.name "
                                 ", tasks.task_date "
                                 "FROM task_items "
                                 "INNER JOIN task_item_types "
                                 "ON task_items.task_item_type_id = task_item_types.task_item_type_id "
                                 "INNER JOIN projects "
                                 "ON task_items.project_id = projects.project_id "
                                 "INNER JOIN categories "
                                 "ON task_items.category_id = categories.category_id "
                                 "INNER JOIN tasks "
                                 "ON task_items.task_id = tasks.task_id "
                                 "WHERE tasks.task_date >= ? "
                                 "AND tasks.task_date <= ? "
                                 "AND task_items.is_active = 1";

CsvExporter::CsvExporter(std::shared_ptr<spdlog::logger> logger,
    const std::string& fromDate,
    const std::string& toDate,
    const std::string& fileName)
    : pLogger(logger)
    , mFromDate(fromDate)
    , mToDate(mToDate)
    , mFileName(fileName)
{
    pConnection = db::ConnectionProvider::Get().Handle()->Acquire();
}

CsvExporter::~CsvExporter()
{
    db::ConnectionProvider::Get().Handle()->Release(pConnection);
}

bool CsvExporter::ExportData()
{
    /* get raw data from database */
    std::vector<DataSet*> dataSets;
    try {
        dataSets = GetDataSet();
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in CsvExporter::ExportData - {0:d} : {1}", e.get_code(), e.what());
        return false;
    }

    /* get the delimiter */
    std::string delimiter = cfg::ConfigurationProvider::Get().Configuration->GetDelimiter();

    /* get the export path and combine with filename */
    std::string exportFilePath = cfg::ConfigurationProvider::Get().Configuration->GetExportPath();
    std::string fullFilePath = exportFilePath + "\\" + mFileName;

    /* open and create file */
    std::ofstream csvFile(fullFilePath);

    if (!csvFile) {
        pLogger->error("Error when trying to create a CSV file at specified location {0}", fullFilePath);
        return false;
    }

    /* write the headers */
    csvFile << "Start Time" << delimiter << "End Time" << delimiter << "Duration" << delimiter << "Description"
            << delimiter << "Calculated Rate" << delimiter << "Task Item Type" << delimiter << "Project" << delimiter
            << "Billable" << delimiter << "Project Rate" << delimiter << "Category" << delimiter << "Date"
            << "\n";

    /* write the data */
    for (const auto& dataSet : dataSets) {
        csvFile << (dataSet->StartTime ? *dataSet->StartTime : "N/A") << delimiter
                << (dataSet->EndTime ? *dataSet->EndTime : "N/A") << delimiter << dataSet->Duration << delimiter
                << dataSet->Description << delimiter << (dataSet->CalculatedRate ? *dataSet->CalculatedRate : -1)
                << delimiter << dataSet->TaskItemType << delimiter << dataSet->ProjectName << delimiter
                << (dataSet->Rate ? *dataSet->Rate : -1) << delimiter << dataSet->CategoryName << delimiter
                << dataSet->TaskDate << "\n";
    }

    /* clean up */
    csvFile.close();

    while (!dataSets.empty()) {
        delete dataSets.back();
        dataSets.pop_back();
    }

    return true;
}

std::vector<CsvExporter::DataSet*> CsvExporter::GetDataSet()
{
    std::vector<DataSet*> dataSets;

    *pConnection->DatabaseExecutableHandle() << CsvExporter::Query << mFromDate << mToDate >>
        [&](std::unique_ptr<std::string> taskItemsStartTime,
            std::unique_ptr<std::string> taskItemsEndTime,
            std::string taskItemsDuration,
            std::string taskItemsDescription,
            std::unique_ptr<double> taskItemsCalculatedRate,
            std::string taskItemTypesName,
            std::string projectsName,
            bool projectsBillable,
            std::unique_ptr<double> projectsRate,
            std::string categoriesName,
            std::string tasksDate) {

            DataSet* data = new DataSet;
            data->StartTime = std::move(taskItemsStartTime);
            data->EndTime = std::move(taskItemsEndTime);
            data->Duration = taskItemsDuration;
            data->Description = taskItemsDescription;
            data->CalculatedRate = std::move(taskItemsCalculatedRate);
            data->TaskItemType = taskItemTypesName;
            data->ProjectName = projectsName;
            data->Billable = projectsBillable;
            data->Rate = std::move(projectsRate);
            data->CategoryName = categoriesName;
            data->TaskDate = tasksDate;

            dataSets.push_back(data);
        };

    return dataSets;
}
} // namespace app::svc
