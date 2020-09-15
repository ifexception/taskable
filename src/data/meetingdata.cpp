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

#include "meetingdata.h"

#include <spdlog/spdlog.h>

#include "../common/util.h"

namespace app::data
{
MeetingData::MeetingData()
{
    pConnection = db::ConnectionProvider::Get().Handle()->Acquire();
    spdlog::get("msvc")->debug("ACQUIRE connection in TaskItemData|ConnectionTally: {0:d}",
        db::ConnectionProvider::Get().Handle()->ConnectionsInUse());
}

MeetingData::~MeetingData()
{
    db::ConnectionProvider::Get().Handle()->Release(pConnection);
    spdlog::get("msvc")->debug("RELEASE connection in TaskItemData|ConnectionTally: {0:d}",
        db::ConnectionProvider::Get().Handle()->ConnectionsInUse());
}

int64_t MeetingData::Create(std::unique_ptr<model::MeetingModel> meeting)
{
    auto ps = *pConnection->DatabaseExecutableHandle() << MeetingData::createMeeting;

    if (meeting->Attended() != nullptr) {
        ps << *meeting->Attended();
    } else {
        ps << nullptr;
    }

    ps << meeting->GetDuration() << meeting->GetStart().FormatISOCombined().ToStdString()
       << meeting->GetEnd().FormatISOCombined().ToStdString() << meeting->GetLocation() << meeting->GetSubject()
       << meeting->GetBody();

    ps.execute();

    return pConnection->DatabaseExecutableHandle()->last_insert_rowid();
}

void MeetingData::Delete(const int64_t taskItemId)
{
    *pConnection->DatabaseExecutableHandle() << MeetingData::deleteMeeting << util::UnixTimestamp() << taskItemId;
}

const std::string MeetingData::createMeeting =
    "INSERT INTO "
    "meetings(attended, duration, starting, ending, location, subject, body, is_active) "
    "VALUES (?, ?, ?, ?, ?, ?, ?, 1)";

const std::string MeetingData::deleteMeeting = "UPDATE meetings "
                                               "SET date_modified = ?, is_active = 0 "
                                               "WHERE meeting_id = ?";
} // namespace app::data
