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

#include <spdlog/spdlog.h>
#include <sqlite_modern_cpp.h>

#include "../models/employermodel.h"
#include "../models/clientmodel.h"
#include "../models/projectmodel.h"

#include "../data/employerdata.h"
#include "../data/clientdata.h"
#include "../data/projectdata.h"

namespace app::wizard
{
class EntityCompositor final
{
public:
    EntityCompositor() = delete;
    EntityCompositor(std::shared_ptr<spdlog::logger> logger);
    ~EntityCompositor() = default;

    bool ComposeEmployerEntity(std::unique_ptr<model::EmployerModel> employer);

    bool ComposeClientEntity(std::unique_ptr<model::ClientModel> client);

    bool ComposeProjectEntity(std::unique_ptr<model::ProjectModel> project);

private:
    std::shared_ptr<spdlog::logger> pLogger;

    int64_t mEmployerId;
    int64_t mClientId;
};
} // namespace app::wizard
