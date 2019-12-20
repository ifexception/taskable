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

#pragma once

#include <string>
#include <tuple>
#include <vector>

#include <wx/wx.h>

namespace app::services
{
struct TaskStateService
{
public:
    TaskStateService();
    ~TaskStateService() = default;

    void PushTimes(wxDateTime startTime, wxDateTime endTime);
    void StoreDescription(const wxString& description);
    wxTimeSpan GetAccumulatedTime();
    wxString GetStoredDescription() const;

    std::vector<std::tuple<wxDateTime, wxDateTime>> mTimes;
    wxString mDescription;
};
} // namespace app::services
