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

#include <memory>
#include <string>

#include <spdlog/spdlog.h>

#include "../database/sqliteconnection.h"
#include "../database/connectionprovider.h"

namespace app::svc
{
class CsvExporter
{
public:
    CsvExporter(std::shared_ptr<spdlog::logger> logger, const std::string& fromDate, const std::string& toDate, const std::string& fileName);
    ~CsvExporter();

    bool ExportData();

private:
    struct DataSet {
        std::unique_ptr<std::string> StartTime;
        std::unique_ptr<std::string> EndTime;
        std::string Duration;
        std::string Description;
        std::unique_ptr<double> CalculatedRate;
        std::string TaskItemType;
        std::string ProjectName;
        bool Billable;
        std::unique_ptr<double> Rate;
        std::string CategoryName;
        std::string TaskDate;
    };

    std::vector<CsvExporter::DataSet*> GetDataSet();

    std::shared_ptr<spdlog::logger> pLogger;
    std::shared_ptr<db::SqliteConnection> pConnection;
    std::string mFromDate;
    std::string mToDate;
    std::string mFileName;

    static std::string Query;
};
} // namespace app::svc
