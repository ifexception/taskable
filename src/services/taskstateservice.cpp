//  Taskable is a desktop that aids you in tracking your timesheets
//  and seeing what work you have done.
//
//  Copyright(C) <2019> <Szymon Welgus>
//
//  This program is free software : you can redistribute it and /
//  or modify it under the terms of the GNU General Public License as published
//  by the Free Software Foundation
//  , either version 3 of the License
//  , or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful
//  , but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "taskstateservice.h"

#include <cassert>

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
} // namespace app::services
