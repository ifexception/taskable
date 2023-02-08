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

#include "taskitemmodel.h"

#include "../common/util.h"

#include "../data/projectdata.h"
#include "../data/taskdata.h"

namespace app::model
{
TaskItemModel::TaskItemModel()
    : mTaskItemId(-1)
    , pStartTime(nullptr)
    , pEndTime(nullptr)
    , mDuration(wxGetEmptyString())
    , mDescription(wxGetEmptyString())
    , mDateCreated(wxDefaultDateTime)
    , mDateModified(wxDefaultDateTime)
    , bIsActive(false)
    , mTaskItemTypeId(-1)
    , mProjectId(-1)
    , mCategoryId(-1)
    , mTaskId(-1)
    , pMeetingId(nullptr)
    , pTaskItemType(nullptr)
    , pProject(nullptr)
    , pCategory(nullptr)
    , pTask(nullptr)
    , pMeeting(nullptr)
{
}

TaskItemModel::TaskItemModel(int taskItemId)
    : TaskItemModel()
{
    mTaskItemId = taskItemId;
}

TaskItemModel::TaskItemModel(int taskItemId,
    wxString duration,
    wxString description,
    int dateCreated,
    int dateModified,
    bool isActive)
    : TaskItemModel()
{
    mTaskItemId = taskItemId;
    mDuration = duration;
    mDescription = description;
    mDateCreated = util::ToDateTime(dateCreated);
    mDateModified = util::ToDateTime(dateModified);
    bIsActive = isActive;
}

bool TaskItemModel::IsEntryTask()
{
    return pStartTime == nullptr && pEndTime == nullptr &&
           mTaskItemTypeId == static_cast<int>(constants::TaskItemTypes::EntryTask);
}

bool TaskItemModel::IsTimedTask()
{
    return pStartTime != nullptr && pEndTime != nullptr &&
           mTaskItemTypeId == static_cast<int>(constants::TaskItemTypes::TimedTask);
}

const int TaskItemModel::GetTaskItemId() const
{
    return mTaskItemId;
}

const wxDateTime* TaskItemModel::GetStartTime() const
{
    return pStartTime.get();
}

const wxDateTime* TaskItemModel::GetEndTime() const
{
    return pEndTime.get();
}

const wxDateTime* TaskItemModel::GetDurationTime() const
{
    return pDurationTime.get();
}

const wxString TaskItemModel::GetDuration() const
{
    return mDuration;
}

const wxString TaskItemModel::GetDescription() const
{
    return mDescription;
}

const wxDateTime TaskItemModel::GetDateCreated()
{
    return mDateCreated;
}

const wxDateTime TaskItemModel::GetDateModified()
{
    return mDateModified;
}

const bool TaskItemModel::IsActive() const
{
    return bIsActive;
}

const int TaskItemModel::GetTaskItemTypeId() const
{
    return mTaskItemTypeId;
}

const int TaskItemModel::GetProjectId() const
{
    return mProjectId;
}

const int TaskItemModel::GetCategoryId() const
{
    return mCategoryId;
}

const int TaskItemModel::GetTaskId() const
{
    return mTaskId;
}

const int64_t* TaskItemModel::GetMeetingId() const
{
    return pMeetingId.get();
}

TaskItemTypeModel* TaskItemModel::GetTaskItemType()
{
    return pTaskItemType.get();
}

ProjectModel* TaskItemModel::GetProject()
{
    return pProject.get();
}

CategoryModel* TaskItemModel::GetCategory()
{
    return pCategory.get();
}

TaskModel* TaskItemModel::GetTask()
{
    return pTask.get();
}

MeetingModel* TaskItemModel::GetMeeting()
{
    return pMeeting.get();
}

void TaskItemModel::SetTaskItemId(const int taskItemId)
{
    mTaskItemId = taskItemId;
}

void TaskItemModel::SetStartTime(std::unique_ptr<wxDateTime> startTime)
{
    pStartTime = std::move(startTime);
}

void TaskItemModel::SetEndTime(std::unique_ptr<wxDateTime> endTime)
{
    pEndTime = std::move(endTime);
}

void TaskItemModel::SetDurationTime(std::unique_ptr<wxDateTime> durationTime)
{
    pDurationTime = std::move(durationTime);
}

void TaskItemModel::SetStartTime(const wxString& startTime)
{
    wxDateTime startDateTime;
    startDateTime.ParseISOTime(startTime);
    pStartTime = std::make_unique<wxDateTime>(startDateTime);
}

void TaskItemModel::SetEndTime(const wxString& endTime)
{
    wxDateTime endDateTime;
    endDateTime.ParseISOTime(endTime);
    pEndTime = std::make_unique<wxDateTime>(endDateTime);
}

void TaskItemModel::SetDurationTime(const wxString& durationTime)
{
    wxDateTime durationDateTime;
    durationDateTime.ParseISOTime(durationTime);
    pDurationTime = std::make_unique<wxDateTime>(durationDateTime);
}

void TaskItemModel::SetDuration(const wxString& duration)
{
    mDuration = duration;
}

void TaskItemModel::SetDescription(const wxString& description)
{
    mDescription = description;
}

void TaskItemModel::SetDateCreated(const wxDateTime& dateCreated)
{
    mDateCreated = dateCreated;
}

void TaskItemModel::SetDateUpdated(const wxDateTime& dateModified)
{
    mDateModified = dateModified;
}

void TaskItemModel::IsActive(const bool isActive)
{
    bIsActive = isActive;
}

void TaskItemModel::SetTaskItemTypeId(const int taskItemTypeId)
{
    mTaskItemTypeId = taskItemTypeId;
}

void TaskItemModel::SetProjectId(const int projectId)
{
    mProjectId = projectId;
}

void TaskItemModel::SetCategoryId(const int categoryId)
{
    mCategoryId = categoryId;
}

void TaskItemModel::SetTaskId(const int taskId)
{
    mTaskId = taskId;
}

void TaskItemModel::SetMeetingId(std::unique_ptr<int64_t> meetingId)
{
    pMeetingId = std::move(meetingId);
}

void TaskItemModel::SetTaskItemType(std::unique_ptr<TaskItemTypeModel> taskItemType)
{
    pTaskItemType = std::move(taskItemType);
}

void TaskItemModel::SetProject(std::unique_ptr<ProjectModel> projcet)
{
    pProject = std::move(projcet);
}

void TaskItemModel::SetCategory(std::unique_ptr<CategoryModel> category)
{
    pCategory = std::move(category);
}

void TaskItemModel::SetTask(std::unique_ptr<TaskModel> task)
{
    pTask = std::move(task);
}

void TaskItemModel::SetMeeting(std::unique_ptr<MeetingModel> meeting)
{
    pMeeting = std::move(meeting);
}
} // namespace app::model
