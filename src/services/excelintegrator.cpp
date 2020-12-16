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

#include "excelintegrator.h"

namespace app::svc
{
static const wxString ExcelApplication = "Excel.Application";

ExcelIntegrator::ExcelIntegrator(std::shared_ptr<spdlog::logger> logger)
    : pLogger(logger)
    , mExcelApplication()
{
}

bool ExcelIntegrator::TryGetExcelInstance()
{
    mExcelApplication.SetLCID(MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT));
    if (!mExcelApplication.GetInstance(ExcelApplication)) {
        if (!mExcelApplication.CreateInstance(ExcelApplication)) {
            pLogger->error("Error getting Excel application instance. Aborted.");
            return false;
        }
    }
    return true;
}

bool ExcelIntegrator::Execute()
{
    if (!mExcelApplication.PutProperty("Visible", true)) {
        // log error
        return false;
    }

    const wxVariant workbooksCountVariant = mExcelApplication.GetProperty("Workbooks.Count");
    if (workbooksCountVariant.IsNull()) {
        // log error
        return false;
    }

    const long workbooksCount = workbooksCountVariant.GetLong();
    if (workbooksCount == 0) {
        const wxVariant workbook = mExcelApplication.CallMethod("Workbooks.Add");
        if (workbook.IsNull()) {
            // log error
            return false;
        }
    }

    wxAutomationObject activeWorkbook;
    if (!mExcelApplication.GetObject(activeWorkbook, "ActiveWorkbook")) {
        // log error
        return false;
    }

    return true;
}
} // namespace app::svc
