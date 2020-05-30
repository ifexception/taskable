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

#include "taskstateservice.h"

namespace app::services
{
TaskStateService::TaskStateService()
    : mTimes()
{
}

void TaskStateService::PushTimes(wxDateTime startTime, wxDateTime endTime)
{
    auto tuple = std::make_tuple(startTime, endTime);
    mTimes.push_back(tuple);
}

void TaskStateService::StoreDescription(const wxString& description)
{
    mDescription = description;
}

wxTimeSpan TaskStateService::GetAccumulatedTime()
{
    wxTimeSpan accumulated;
    for (size_t i = 0; i < mTimes.size(); i++) {
        auto [startTime, endTime] = mTimes[i];
        auto difference = endTime - startTime;
        accumulated += difference;
    }

    return accumulated;
}

wxString TaskStateService::GetStoredDescription() const
{
    return mDescription;
}
} // namespace app::services
