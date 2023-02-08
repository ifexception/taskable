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

#include "meetingdata.h"

#include "../common/util.h"

namespace app::data
{
MeetingData::MeetingData()
{
    pConnection = db::ConnectionProvider::Get().Handle()->Acquire();
}

MeetingData::~MeetingData()
{
    db::ConnectionProvider::Get().Handle()->Release(pConnection);
}

int64_t MeetingData::Create(std::unique_ptr<model::MeetingModel> meeting, int64_t taskId)
{
    auto ps = *pConnection->DatabaseExecutableHandle() << MeetingData::createMeeting;

    if (meeting->Attended() != nullptr) {
        ps << *meeting->Attended();
    } else {
        ps << nullptr;
    }

    ps << meeting->GetDuration() << meeting->GetStart().FormatISOCombined().ToStdString()
       << meeting->GetEnd().FormatISOCombined().ToStdString() << meeting->GetLocation().ToStdString()
       << meeting->GetSubject().ToStdString() << meeting->GetBody().ToStdString();

    ps << taskId;

    ps.execute();

    return pConnection->DatabaseExecutableHandle()->last_insert_rowid();
}

void MeetingData::Delete(const int64_t taskItemId)
{
    *pConnection->DatabaseExecutableHandle() << MeetingData::deleteMeeting << util::UnixTimestamp() << taskItemId;
}

std::vector<std::unique_ptr<model::MeetingModel>> MeetingData::GetByDate(const wxString& date)
{
    std::vector<std::unique_ptr<model::MeetingModel>> meetings;

    *pConnection->DatabaseExecutableHandle() << MeetingData::getByDate << date.ToStdString() >>
        [&](int meetingsMeetingId,
            std::unique_ptr<bool> meetingsAttended,
            int meetingsDuration,
            std::string meetingsStarting,
            std::string meetingsEnding,
            std::string meetingsLocation,
            std::string meetingsSubject,
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
                meetingsLocation,
                meetingsSubject,
                meetingsBody,
                meetingsDateCreated,
                meetingsDateModified,
                meetingsIsActive);

            meeting->SetStart(wxString(meetingsStarting));
            meeting->SetEnd(wxString(meetingsEnding));

            meeting->SetTaskId(meetingsTaskId);

            auto taskModel = std::make_unique<model::TaskModel>(
                tasksTaskId, tasksTaskDate, tasksDateCreated, tasksDateModified, tasksIsActive);
            meeting->SetTask(std::move(taskModel));

            if (meetingsAttended != nullptr) {
                meeting->Attended(std::move(meetingsAttended));
            }

            meetings.push_back(std::move(meeting));
        };

    return meetings;
}

const std::string MeetingData::createMeeting =
    "INSERT INTO "
    "meetings(attended, duration, starting, ending, location, subject, body, is_active, task_id) "
    "VALUES (?, ?, ?, ?, ?, ?, ?, 1, ?)";

const std::string MeetingData::deleteMeeting = "UPDATE meetings "
                                               "SET date_modified = ?, is_active = 0 "
                                               "WHERE meeting_id = ?";

const std::string MeetingData::getByDate = "SELECT "
                                           "	  meetings.meeting_id"
                                           "	, meetings.attended"
                                           "	, meetings.duration"
                                           "	, meetings.starting"
                                           "	, meetings.ending"
                                           "	, meetings.location"
                                           "	, meetings.subject"
                                           "	, meetings.body"
                                           "	, meetings.date_created"
                                           "	, meetings.date_modified"
                                           "	, meetings.is_active"
                                           "	, meetings.task_id"
                                           "	, tasks.task_id"
                                           "	, tasks.task_date"
                                           "	, tasks.date_created"
                                           "	, tasks.date_modified"
                                           "	, tasks.is_active "
                                           "FROM meetings "
                                           "INNER JOIN tasks "
                                           "ON meetings.task_id = tasks.task_id "
                                           "WHERE tasks.task_date = ?";
} // namespace app::data
