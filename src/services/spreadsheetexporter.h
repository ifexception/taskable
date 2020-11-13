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
#include <tinyxml2.h>

#include "../common/constants.h"
#include "../database/sqliteconnection.h"
#include "../database/connectionprovider.h"

namespace app::svc
{
struct SpreadsheetExportOptions {
    std::string FromDate;
    std::string ToDate;
    constants::ExportFormats Format;
    std::string Extension;
};

class SpreadsheetExporter
{
public:
    SpreadsheetExporter() = default;
    SpreadsheetExporter(std::shared_ptr<spdlog::logger> logger, SpreadsheetExportOptions& options);
    ~SpreadsheetExporter();

private:
    std::shared_ptr<spdlog::logger> pLogger;
    std::shared_ptr<db::SqliteConnection> pConnection;

    SpreadsheetExportOptions mOptions;
};
} // namespace app::svc
