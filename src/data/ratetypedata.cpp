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

#include "ratetypedata.h"

#include <spdlog/spdlog.h>

namespace app::data
{
RateTypeData::RateTypeData()
{
    spdlog::logger("msvc").info("ACQUIRE connection in RateTypeData");
    pConnection = db::ConnectionProvider::Get().Handle()->Acquire();
}

RateTypeData::~RateTypeData()
{
    spdlog::logger("msvc").info("RELEASE connection in RateTypeData");
    db::ConnectionProvider::Get().Handle()->Release(pConnection);
}

std::unique_ptr<model::RateTypeModel> RateTypeData::GetById(const int rateTypeId)
{
    std::unique_ptr<model::RateTypeModel> rateType = nullptr;

    *pConnection->DatabaseExecutableHandle() << RateTypeData::getRateTypeById << rateTypeId >>
        [&](int rateTypeId, std::string name) {
            rateType = std::make_unique<model::RateTypeModel>(rateTypeId, wxString(name));
        };

    return std::move(rateType);
}

std::vector<std::unique_ptr<model::RateTypeModel>> RateTypeData::GetAll()
{
    std::vector<std::unique_ptr<model::RateTypeModel>> rateTypes;

    *pConnection->DatabaseExecutableHandle() << RateTypeData::getRateTypes >>
        [&](int rateTypeId, std::string name) {
            auto rateType = std::make_unique<model::RateTypeModel>(rateTypeId, wxString(name));
            rateTypes.push_back(std::move(rateType));
        };

    return rateTypes;
}

const std::string RateTypeData::getRateTypeById = "SELECT rate_type_id, "
                                                  "name "
                                                  "FROM rate_types "
                                                  "WHERE rate_type_id = ?";

const std::string RateTypeData::getRateTypes = "SELECT rate_type_id, "
                                               "name "
                                               "FROM rate_types";
} // namespace app::data
