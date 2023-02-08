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

#include <array>
#include <unordered_map>
#include <vector>

#include <wx/wx.h>
#include <wx/dataview.h>

#include "../common/datetraverser.h"
#include "../models/taskitemmodel.h"

namespace app::dv
{
const int NumberOfDays = 7;

class WeeklyTreeModelNode;
WX_DEFINE_ARRAY_PTR(WeeklyTreeModelNode*, WeeklyTreeModelNodePtrArray);

class WeeklyTreeModelNode final
{
public:
    WeeklyTreeModelNode(WeeklyTreeModelNode* parent,
        const wxString& projectName,
        const wxString& duration,
        const wxString& categoryName,
        const wxString& description,
        int taskItemId);
    WeeklyTreeModelNode(WeeklyTreeModelNode* parent, const wxString& branch);
    ~WeeklyTreeModelNode();

    bool IsContainer() const;
    WeeklyTreeModelNode* GetParent();
    WeeklyTreeModelNodePtrArray& GetChildren();
    WeeklyTreeModelNode* GetNthChild(unsigned int n);

    void Insert(WeeklyTreeModelNode* child, unsigned int n);
    void Append(WeeklyTreeModelNode* child);
    const unsigned int GetChildCount() const;

    wxString GetProjectName() const;
    wxString GetDuration() const;
    wxString GetCategoryName() const;
    wxString GetDescription() const;
    int GetTaskItemId() const;

    void SetProjectName(const wxString& value);
    void SetDuration(const wxString& value);
    void SetCategoryName(const wxString& value);
    void SetDescription(const wxString& value);
    void SetTaskItemId(int taskItemId);

private:
    WeeklyTreeModelNode* pParent;
    WeeklyTreeModelNodePtrArray mChildren;

    wxString mProjectName;
    wxString mDuration;
    wxString mCategoryName;
    wxString mDescription;
    int mTaskItemId;
    bool bContainer;
};

class WeeklyTreeModel : public wxDataViewModel
{
public:
    enum { Col_Project = 0, Col_Duration, Col_Category, Col_Description, Col_Id, Col_Max };

    WeeklyTreeModel(const DateTraverser& dateTraverser);
    ~WeeklyTreeModel();

    void AddToWeek(std::vector<std::unique_ptr<model::TaskItemModel>>& taskItems);

    unsigned int GetColumnCount() const override;
    wxString GetColumnType(unsigned int col) const override;
    void GetValue(wxVariant& variant, const wxDataViewItem& item, unsigned int col) const override;
    bool SetValue(const wxVariant& variant, const wxDataViewItem& item, unsigned int col) override;
    bool IsEnabled(const wxDataViewItem& item, unsigned int col) const override;
    wxDataViewItem GetParent(const wxDataViewItem& item) const override;
    bool IsContainer(const wxDataViewItem& item) const override;
    unsigned int GetChildren(const wxDataViewItem& parent, wxDataViewItemArray& array) const override;
    void Delete(const wxDataViewItem& item);
    void ClearAll();

    wxDataViewItem ExpandRootNode();
    wxDataViewItemArray CollapseDayNodes();
    wxDateTime GetDateFromDataViewItem(const wxDataViewItem& item);

    void SetDateTraverser(const DateTraverser& dateTraverser);

private:
    void SetupNodes();

    void AddMany(WeeklyTreeModelNode* dayNodeToAdd, std::vector<std::unique_ptr<model::TaskItemModel>>& dayTasksToAdd);
    void ClearDayNodes(WeeklyTreeModelNode* node);

    void UpdateNodeLabels();

    std::unordered_map<wxString, std::vector<std::unique_ptr<model::TaskItemModel>>> mWeeklyTasksMap;

    WeeklyTreeModelNode* pRoot;
    std::array<WeeklyTreeModelNode*, NumberOfDays> pDayNodes;

    DateTraverser mDateTraverser;
};
} // namespace app::dv
