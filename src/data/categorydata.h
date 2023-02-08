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

#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "../database/connectionprovider.h"
#include "../database/sqliteconnection.h"
#include "../models/categorymodel.h"

namespace app::data
{
class CategoryData final
{
public:
    CategoryData();
    ~CategoryData();

    int64_t Create(std::unique_ptr<model::CategoryModel> category);
    std::unique_ptr<model::CategoryModel> GetById(const int id);
    void Update(std::unique_ptr<model::CategoryModel> category);
    void Delete(int categoryId);
    std::vector<std::unique_ptr<model::CategoryModel>> GetByProjectId(const int projectId);
    std::vector<std::unique_ptr<model::CategoryModel>> GetAll();

private:
    std::shared_ptr<db::SqliteConnection> pConnection;

    static const std::string createCategory;
    static const std::string getCategoryById;
    static const std::string updateCategory;
    static const std::string deleteCategory;
    static const std::string getCategoriesByProjectId;
    static const std::string getCategories;
};
} // namespace app::data
