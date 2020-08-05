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

#include "meetingmodel.h"

#include "../common/util.h"

namespace app::model
{
MeetingModel::MeetingModel()
    : mMeetingId(-1)
    , mDuration(-1)
    , mStarting(wxGetEmptyString())
    , mEnding(wxGetEmptyString())
    , mLocation(wxGetEmptyString())
    , mBody(wxGetEmptyString())
    , pAttended(nullptr)
    , mDateCreated(wxDefaultDateTime)
    , mDateModified(wxDefaultDateTime)
    , bIsActive(false)
    , mTaskId(-1)
    , pTaskModel(nullptr)
{}

MeetingModel::MeetingModel(int meetingId)
    : MeetingModel()
{
    mMeetingId = meetingId;
}

MeetingModel::MeetingModel(int meetingId,
    int duration,
    wxString start,
    wxString end,
    wxString location,
    wxString body,
    int dateCreated,
    int dateModified,
    bool isActive)
    : MeetingModel()
{
    mMeetingId = meetingId;
    mDuration = duration;
    mStarting = start;
    mEnding = end;
    mLocation = location;
    mBody = body;
    mDateCreated = util::ToDateTime(dateCreated);
    mDateModified = util::ToDateTime(dateModified);
    bIsActive = isActive;
}

const int MeetingModel::GetMeetingId()
{
    return mMeetingId;
}

const int MeetingModel::GetDuration()
{
    return mDuration;
}

const wxString MeetingModel::GetStart()
{
    return mStarting;
}

const wxString MeetingModel::GetEnd()
{
    return mEnding;
}

const wxString MeetingModel::GetLocation()
{
    return mLocation;
}

const wxString MeetingModel::GetBody()
{
    return mBody;
}

const bool* MeetingModel::Attended()
{
    return pAttended.get();
}

const wxDateTime MeetingModel::GetDateCreated()
{
    return mDateCreated;
}

const wxDateTime MeetingModel::GetDateModified()
{
    return mDateModified;
}

const bool MeetingModel::IsActive()
{
    return bIsActive;
}

const int MeetingModel::GetTaskId()
{
    return mTaskId;
}

TaskModel* MeetingModel::GetTaskModel()
{
    return pTaskModel.get();
}

void MeetingModel::SetMeetingId(int meetingId)
{
    mMeetingId = meetingId;
}

void MeetingModel::SetDuration(int duration)
{
    mDuration = duration;
}

void MeetingModel::SetStart(const wxString& start)
{
    mStarting = start;
}

void MeetingModel::SetEnd(const wxString& end)
{
    mEnding = end;
}

void MeetingModel::SetLocation(const wxString& location)
{
    mLocation = location;
}

void MeetingModel::SetBody(const wxString& body)
{
    mBody = body;
}

void MeetingModel::Attended(std::unique_ptr<bool> attended)
{
    pAttended = std::move(attended);
}

void MeetingModel::SetDateCreated(const wxDateTime& dateCreated)
{
    mDateCreated = dateCreated;
}

void MeetingModel::SetDateUpdated(const wxDateTime& dateModified)
{
    mDateModified = dateModified;
}

void MeetingModel::IsActive(bool isActive)
{
    bIsActive = isActive;
}

void MeetingModel::SetTaskId(int taskId)
{
    mTaskId = taskId;
}

void MeetingModel::SetTaskModel(std::unique_ptr<TaskModel> taskModel)
{
    pTaskModel = std::move(taskModel);
}
} // namespace app::model
