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

#include "ratetypemodel.h"

namespace app::model
{
RateTypeModel::RateTypeModel()
    : mRateTypeId(-1)
    , mName(wxGetEmptyString())
    , mRateType(constants::RateTypes::None)
{
}

RateTypeModel::RateTypeModel(const int rateTypeId)
    : RateTypeModel()
{
    mRateTypeId = rateTypeId;
}

RateTypeModel::RateTypeModel(const int rateTypeId, wxString name)
    : RateTypeModel()
{
    mRateTypeId = rateTypeId;
    mName = name;
    mRateType = static_cast<constants::RateTypes>(mRateTypeId);
}

const int RateTypeModel::GetRateTypeId() const
{
    return mRateTypeId;
}

const wxString RateTypeModel::GetName() const
{
    return mName;
}

const constants::RateTypes RateTypeModel::GetType() const
{
    return mRateType;
}

void RateTypeModel::SetRateTypeId(const int rateTypeId)
{
    mRateTypeId = rateTypeId;
}

void RateTypeModel::SetName(const wxString& name)
{
    mName = name;
}
}
