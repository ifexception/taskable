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

    ps << meeting->GetDuration() << meeting->GetStart() << meeting->GetEnd() << meeting->GetLocation()
       << meeting->GetBody() << meeting->GetTaskId();

    ps.execute();

    return pConnection->DatabaseExecutableHandle()->last_insert_rowid();
}

std::unique_ptr<model::MeetingModel> MeetingData::GetById(const int meetingId)
{
    std::unique_ptr<model::MeetingModel> meeting = nullptr;

    *pConnection->DatabaseExecutableHandle() << MeetingData::getMeeting << meetingId >>
        [&](int meetingsMeetingId,
            std::unique_ptr<bool> meetingsAttended,
            int meetingsDuration,
            std::string meetingsStarting,
            std::string meetingsEnding,
            std::string meetingsLocation,
            std::string meetingsBody,
            int meetingsDateCreated,
            int meetingsDateModified,
            bool meetingsIsActive,
            int meetingsTaskId,
            int tasksTaskId,
            std::string tasksTaskDate,
            int tasksDateCreated,
            int tasksDateModified,
            bool tasksIsActive) {
            meeting = std::make_unique<model::MeetingModel>(meetingsMeetingId,
                meetingsDuration,
                meetingsStarting,
                meetingsEnding,
                meetingsLocation,
                meetingsBody,
                meetingsDateCreated,
                meetingsDateModified,
                meetingsIsActive);

            meeting->SetTaskId(meetingsTaskId);

            auto taskModel = std::make_unique<model::TaskModel>(
                tasksTaskId, tasksTaskDate, tasksDateCreated, tasksDateModified, tasksIsActive);
            meeting->SetTaskModel(std::move(taskModel));

            if (meetingsAttended != nullptr) {
                meeting->Attended(std::move(meetingsAttended));
            }
        };

    return meeting;
}

void MeetingData::Update(std::unique_ptr<model::MeetingModel> meeting)
{
    auto ps = *pConnection->DatabaseExecutableHandle() << MeetingData::updateMeeting;
    if (meeting->Attended() != nullptr) {
        ps << *meeting->Attended();
    } else {
        ps << nullptr;
    }

    ps << meeting->GetDuration() << meeting->GetStart().ToStdString() << meeting->GetEnd().ToStdString()
       << meeting->GetLocation().ToStdString() << meeting->GetBody().ToStdString();

    ps << util::UnixTimestamp();
    ps << meeting->GetMeetingId();

    ps.execute();
}

void MeetingData::Delete(const int meetingId)
{
    *pConnection->DatabaseExecutableHandle() << MeetingData::deleteMeeting << util::UnixTimestamp() << meetingId;
}

std::vector<std::unique_ptr<model::MeetingModel>> MeetingData::GetAll()
{
    std::vector<std::unique_ptr<model::MeetingModel>> meetings;

    *pConnection->DatabaseExecutableHandle()
        << MeetingData::getMeetings
        >> [&](int meetingsMeetingId,
            std::unique_ptr<bool> meetingsAttended,
            int meetingsDuration,
            std::string meetingsStarting,
            std::string meetingsEnding,
            std::string meetingsLocation,
            std::string meetingsBody,
            int meetingsDateCreated,
            int meetingsDateModified,
            bool meetingsIsActive,
            int meetingsTaskId,
            int tasksTaskId,
            std::string tasksTaskDate,
            int tasksDateCreated,
            int tasksDateModified,
            bool tasksIsActive) {
        auto meeting = std::make_unique<model::MeetingModel>(meetingsMeetingId,
            meetingsDuration,
            meetingsStarting,
            meetingsEnding,
            meetingsLocation,
            meetingsBody,
            meetingsDateCreated,
            meetingsDateModified,
            meetingsIsActive);

        meeting->SetTaskId(meetingsTaskId);

        auto taskModel = std::make_unique<model::TaskModel>(
            tasksTaskId, tasksTaskDate, tasksDateCreated, tasksDateModified, tasksIsActive);
        meeting->SetTaskModel(std::move(taskModel));

        if (meetingsAttended != nullptr) {
            meeting->Attended(std::move(meetingsAttended));
        }

        meetings.push_back(std::move(meeting));
    };

    return meetings;
}

const std::string MeetingData::createMeeting =
    "INSERT INTO "
    "meetings(attended, duration, starting, ending, location, body, is_active, task_id) "
    "VALUES (?, ?, ?, ?, ?, ?, 1, ?)";

const std::string MeetingData::getMeeting = "SELECT "
                                            "	  meetings.meeting_id"
                                            "	, meetings.attended"
                                            "	, meetings.duration"
                                            "	, meetings.starting"
                                            "	, meetings.ending"
                                            "	, meetings.location"
                                            "	, meetings.body"
                                            "	, meetings.date_created"
                                            "	, meetings.date_modified"
                                            "	, meetings.is_active"
                                            "	, meetings.task_id"
                                            "	, tasks.task_id"
                                            "	, tasks.task_date"
                                            "	, tasks.date_created"
                                            "	, tasks.date_modified"
                                            "	, tasks.is_active"
                                            "FROM meetings "
                                            "INNER JOIN tasks "
                                            "ON meetings.task_id = tasks.task_id "
                                            "WHERE meetings.meeting_id = ?";

const std::string MeetingData::updateMeeting = "UPDATE meetings "
                                               "SET attended = ?, duration = ?, starting = ?, "
                                               "ending = ?, location = ?, body = ?, "
                                               "date_modified = ?,"
                                               "task_id = ? "
                                               "WHERE meeting_id = ?";

const std::string MeetingData::deleteMeeting = "UPDATE meetings "
                                               "SET date_modified = ?, is_active = 0 "
                                               "WHERE meeting_id = ?";

const std::string MeetingData::getMeetings = "SELECT "
                                             "	  meetings.meeting_id"
                                             "	, meetings.attended"
                                             "	, meetings.duration"
                                             "	, meetings.starting"
                                             "	, meetings.ending"
                                             "	, meetings.location"
                                             "	, meetings.body"
                                             "	, meetings.date_created"
                                             "	, meetings.date_modified"
                                             "	, meetings.is_active"
                                             "	, meetings.task_id"
                                             "	, tasks.task_id"
                                             "	, tasks.task_date"
                                             "	, tasks.date_created"
                                             "	, tasks.date_modified"
                                             "	, tasks.is_active"
                                             "FROM meetings "
                                             "INNER JOIN tasks "
                                             "ON meetings.task_id = tasks.task_id "
                                             "WHERE meetings.is_active = 1";
} // namespace app::data
