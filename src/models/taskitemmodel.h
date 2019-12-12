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

#include <memory>
#include <string>
#include <vector>

#include <wx/datetime.h>

#include "../common/constants.h"
#include "projectmodel.h"
#include "categorymodel.h"
#include "taskmodel.h"

namespace app::model
{
class TaskItemTypeModel final
{
public:
    TaskItemTypeModel();
    TaskItemTypeModel(const int taskItemTypeId);
    TaskItemTypeModel(const int taskItemTypeId, bool initializeFromDatabase);
    TaskItemTypeModel(int taskItemTypeId, wxString name);
    ~TaskItemTypeModel() = default;

    const int GetTaskItemTypeId() const;
    const wxString GetName() const;
    const constants::TaskItemTypes GetType() const;

    void SetTaskItemTypeId(const int taskItemTypeId);
    void SetName(const wxString& name);

    static std::unique_ptr<TaskItemTypeModel> GetById(const int taskItemTypeId);
    static std::vector<std::unique_ptr<TaskItemTypeModel>> GetAll();

private:
    int mTaskItemTypeId;
    wxString mName;
    constants::TaskItemTypes mType;

    static const std::string getTaskItemTypeById;
    static const std::string getTaskItemTypes;
};

class TaskItemModel final
{
public:
    TaskItemModel();
    TaskItemModel(int taskItemId);
    TaskItemModel(int taskItemId, bool initializeFromDatabase);
    TaskItemModel(int taskItemId,
        wxString duration,
        wxString description,
        bool billable,
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
    const bool IsBillable() const;
    const double* GetCalculatedRate() const;
    const wxDateTime GetDateCreated();
    const wxDateTime GetDateModified();
    const bool IsActive() const;

    const int GetTaskItemTypeId() const;
    const int GetProjectId() const;
    const int GetCategoryId() const;
    const int GetTaskId() const;

    TaskItemTypeModel* GetTaskItemType();
    ProjectModel* GetProject();
    CategoryModel* GetCategory();
    TaskModel* GetTask();

    void SetTaskItemId(const int taskItemId);
    void SetStartTime(std::unique_ptr<wxDateTime> startTime);
    void SetEndTime(std::unique_ptr<wxDateTime> endTime);
    void SetDurationTime(std::unique_ptr<wxDateTime> durationTime);
    void SetStartTime(const wxString& startTime);
    void SetEndTime(const wxString& endTime);
    void SetDurationTime(const wxString& durationTime);
    void SetDuration(const wxString& duration);
    void SetDescription(const wxString& description);
    void IsBillable(const bool billable);
    void SetCalculatedRate(std::unique_ptr<double> calculatedRate);
    void SetDateCreated(const wxDateTime& dateCreated);
    void SetDateUpdated(const wxDateTime& dateModified);
    void IsActive(const bool isActive);

    void SetTaskItemTypeId(const int taskItemTypeId);
    void SetProjectId(const int projectId);
    void SetCategoryId(const int categoryId);
    void SetTaskId(const int taskId);

    void SetTaskItemType(std::unique_ptr<TaskItemTypeModel> taskItemType);
    void SetProject(std::unique_ptr<ProjectModel> projcet);
    void SetCategory(std::unique_ptr<CategoryModel> category);
    void SetTask(std::unique_ptr<TaskModel> task);

    static void Create(std::unique_ptr<TaskItemModel> taskItem);
    static std::unique_ptr<TaskItemModel> GetById(const int taskItemId);
    static void Update(std::unique_ptr<TaskItemModel> taskItem);
    static void Delete(std::unique_ptr<TaskItemModel> taskItem);
    static std::vector<std::unique_ptr<TaskItemModel>> GetByDate(const wxString& date);
    static std::vector<wxString> GetHours(const wxString& date);

private:
    int mTaskItemId;
    std::unique_ptr<wxDateTime> pStartTime;
    std::unique_ptr<wxDateTime> pEndTime;
    std::unique_ptr<wxDateTime> pDurationTime;
    wxString mDuration;
    wxString mDescription;
    bool bBillable;
    std::unique_ptr<double> pCalculatedRate;
    wxDateTime mDateCreated;
    wxDateTime mDateModified;
    bool bIsActive;

    int mTaskItemTypeId;
    int mProjectId;
    int mCategoryId;
    int mTaskId;

    std::unique_ptr<TaskItemTypeModel> pTaskItemType;
    std::unique_ptr<ProjectModel> pProject;
    std::unique_ptr<CategoryModel> pCategory;
    std::unique_ptr<TaskModel> pTask;

    static const std::string createTaskItem;
    static const std::string getTaskItemsByDate;
    static const std::string getTaskItemById;
    static const std::string updateTaskItem;
    static const std::string deleteTaskItem;
    static const std::string getTaskHoursByTaskId;
};
} // namespace app::model
