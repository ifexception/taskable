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

#pragma once

#include <memory>

#include <wx/datetime.h>

#include "../common/constants.h"
#include "projectmodel.h"
#include "categorymodel.h"
#include "taskmodel.h"
#include "taskitemtypemodel.h"
#include "meetingmodel.h"

namespace app::model
{
class TaskItemModel final
{
public:
    TaskItemModel();
    TaskItemModel(int taskItemId);
    TaskItemModel(int taskItemId,
        wxString duration,
        wxString description,
        int dateCreated,
        int dateModified,
        bool isActive);
    ~TaskItemModel() = default;

    bool IsEntryTask();
    bool IsTimedTask();

    const int GetTaskItemId() const;
    const wxDateTime* GetStartTime() const;
    const wxDateTime* GetEndTime() const;
    const wxDateTime* GetDurationTime() const;
    const wxString GetDuration() const;
    const wxString GetDescription() const;
    const wxDateTime GetDateCreated();
    const wxDateTime GetDateModified();
    const bool IsActive() const;

    const int GetTaskItemTypeId() const;
    const int GetProjectId() const;
    const int GetCategoryId() const;
    const int GetTaskId() const;
    const int64_t* GetMeetingId() const;

    TaskItemTypeModel* GetTaskItemType();
    ProjectModel* GetProject();
    CategoryModel* GetCategory();
    TaskModel* GetTask();
    MeetingModel* GetMeeting();

    void SetTaskItemId(const int taskItemId);
    void SetStartTime(std::unique_ptr<wxDateTime> startTime);
    void SetEndTime(std::unique_ptr<wxDateTime> endTime);
    void SetDurationTime(std::unique_ptr<wxDateTime> durationTime);
    void SetStartTime(const wxString& startTime);
    void SetEndTime(const wxString& endTime);
    void SetDurationTime(const wxString& durationTime);
    void SetDuration(const wxString& duration);
    void SetDescription(const wxString& description);
    void SetDateCreated(const wxDateTime& dateCreated);
    void SetDateUpdated(const wxDateTime& dateModified);
    void IsActive(const bool isActive);

    void SetTaskItemTypeId(const int taskItemTypeId);
    void SetProjectId(const int projectId);
    void SetCategoryId(const int categoryId);
    void SetTaskId(const int taskId);
    void SetMeetingId(std::unique_ptr<int64_t> meetingId);

    void SetTaskItemType(std::unique_ptr<TaskItemTypeModel> taskItemType);
    void SetProject(std::unique_ptr<ProjectModel> projcet);
    void SetCategory(std::unique_ptr<CategoryModel> category);
    void SetTask(std::unique_ptr<TaskModel> task);
    void SetMeeting(std::unique_ptr<MeetingModel> meeting);

private:
    int mTaskItemId;
    std::unique_ptr<wxDateTime> pStartTime;
    std::unique_ptr<wxDateTime> pEndTime;
    std::unique_ptr<wxDateTime> pDurationTime;
    wxString mDuration;
    wxString mDescription;
    wxDateTime mDateCreated;
    wxDateTime mDateModified;
    bool bIsActive;

    int mTaskItemTypeId;
    int mProjectId;
    int mCategoryId;
    int mTaskId;
    std::unique_ptr<int64_t> pMeetingId;

    std::unique_ptr<TaskItemTypeModel> pTaskItemType;
    std::unique_ptr<ProjectModel> pProject;
    std::unique_ptr<CategoryModel> pCategory;
    std::unique_ptr<TaskModel> pTask;
    std::unique_ptr<MeetingModel> pMeeting;
};
} // namespace app::model
