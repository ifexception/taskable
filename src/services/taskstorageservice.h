//  Tasks Tracker is a desktop that aids you in tracking your timesheets
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

#pragma once

#include <memory>
#include <tuple>
#include <vector>

#include "taskstateservice.h"

namespace app::services
{
struct TaskStorage
{
    TaskStorage();
    ~TaskStorage() = default;

    void Store(std::shared_ptr<TaskStateService> taskState);
    void Restore(std::shared_ptr<TaskStateService> taskState);

    std::vector<std::tuple<wxDateTime, wxDateTime>> mTimes;
};
} // namespace app::services
