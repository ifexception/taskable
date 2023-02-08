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

#include "weeklymodel.h"

#include <algorithm>
#include <iterator>

#include "../data/taskitemdata.h"

namespace app::dv
{
wxString DayDateLabels[7] = {
    wxT("Monday %s"),
    wxT("Tuesday %s"),
    wxT("Wednesday %s"),
    wxT("Thursday %s"),
    wxT("Friday %s"),
    wxT("Saturday %s"),
    wxT("Sunday %s"),
};

WeeklyTreeModelNode::WeeklyTreeModelNode(WeeklyTreeModelNode* parent,
    const wxString& projectName,
    const wxString& duration,
    const wxString& categoryName,
    const wxString& description,
    int taskItemId)
    : pParent(parent)
    , mProjectName(projectName)
    , mDuration(duration)
    , mCategoryName(categoryName)
    , mDescription(description)
    , mTaskItemId(taskItemId)
    , bContainer(false)
{
}

WeeklyTreeModelNode::WeeklyTreeModelNode(WeeklyTreeModelNode* parent, const wxString& branch)
    : pParent(parent)
    , mProjectName(branch)
    , bContainer(true)
{
}

WeeklyTreeModelNode::~WeeklyTreeModelNode()
{
    std::size_t count = mChildren.GetCount();
    for (std::size_t i = 0; i < count; i++) {
        WeeklyTreeModelNode* child = mChildren[i];
        delete child;
    }
}

bool WeeklyTreeModelNode::IsContainer() const
{
    return bContainer;
}

WeeklyTreeModelNode* WeeklyTreeModelNode::GetParent()
{
    return pParent;
}

WeeklyTreeModelNodePtrArray& WeeklyTreeModelNode::GetChildren()
{
    return mChildren;
}

WeeklyTreeModelNode* WeeklyTreeModelNode::GetNthChild(unsigned int n)
{
    return mChildren.Item(n);
}

void WeeklyTreeModelNode::Insert(WeeklyTreeModelNode* child, unsigned int n)
{
    mChildren.Insert(child, n);
}

void WeeklyTreeModelNode::Append(WeeklyTreeModelNode* child)
{
    mChildren.Add(child);
}

const unsigned int WeeklyTreeModelNode::GetChildCount() const
{
    return mChildren.Count();
}

wxString WeeklyTreeModelNode::GetProjectName() const
{
    return mProjectName;
}

wxString WeeklyTreeModelNode::GetDuration() const
{
    return mDuration;
}

wxString WeeklyTreeModelNode::GetCategoryName() const
{
    return mCategoryName;
}

wxString WeeklyTreeModelNode::GetDescription() const
{
    return mDescription;
}

int WeeklyTreeModelNode::GetTaskItemId() const
{
    return mTaskItemId;
}

void WeeklyTreeModelNode::SetProjectName(const wxString& value)
{
    mProjectName = value;
}

void WeeklyTreeModelNode::SetDuration(const wxString& value)
{
    mDuration = value;
}

void WeeklyTreeModelNode::SetCategoryName(const wxString& value)
{
    mCategoryName = value;
}

void WeeklyTreeModelNode::SetDescription(const wxString& value)
{
    mDescription = value;
}

void WeeklyTreeModelNode::SetTaskItemId(int taskItemId)
{
    mTaskItemId = taskItemId;
}

const wxString WeekLabel = wxT("Monday %s - Sunday %s");
// WeeklyTreeModel
WeeklyTreeModel::WeeklyTreeModel(const DateTraverser& dateTraverser)
    : mWeeklyTasksMap()
    , pDayNodes()
    , mDateTraverser(dateTraverser)
{
    SetupNodes();
}

WeeklyTreeModel::~WeeklyTreeModel()
{
    delete pRoot;
}

// This method should only be used once from WeeklyTaskViewDialog::FillControls
void WeeklyTreeModel::AddToWeek(std::vector<std::unique_ptr<model::TaskItemModel>>& taskItems)
{
    for (auto& taskItem : taskItems) {
        wxString taskDateIndex = taskItem->GetTask()->GetTaskDate();
        mWeeklyTasksMap[taskDateIndex].push_back(std::move(taskItem));
    }

    auto& mondayTaskItems = mWeeklyTasksMap[mDateTraverser.GetDayISODate(constants::Days::Monday)];
    AddMany(pDayNodes[constants::Days::Monday], mondayTaskItems);

    auto& tuesdayTaskItems = mWeeklyTasksMap[mDateTraverser.GetDayISODate(constants::Days::Tuesday)];
    AddMany(pDayNodes[constants::Days::Tuesday], tuesdayTaskItems);

    auto& wednesdayTaskItems = mWeeklyTasksMap[mDateTraverser.GetDayISODate(constants::Days::Wednesday)];
    AddMany(pDayNodes[constants::Days::Wednesday], wednesdayTaskItems);

    auto& thursdayTaskItems = mWeeklyTasksMap[mDateTraverser.GetDayISODate(constants::Days::Thursday)];
    AddMany(pDayNodes[constants::Days::Thursday], thursdayTaskItems);

    auto& fridayTaskItems = mWeeklyTasksMap[mDateTraverser.GetDayISODate(constants::Days::Friday)];
    AddMany(pDayNodes[constants::Days::Friday], fridayTaskItems);

    auto& saturdayTaskItems = mWeeklyTasksMap[mDateTraverser.GetDayISODate(constants::Days::Saturday)];
    AddMany(pDayNodes[constants::Days::Saturday], saturdayTaskItems);

    auto& sundayTaskItems = mWeeklyTasksMap[mDateTraverser.GetDayISODate(constants::Days::Sunday)];
    AddMany(pDayNodes[constants::Days::Sunday], sundayTaskItems);
}

unsigned int WeeklyTreeModel::GetColumnCount() const
{
    return Col_Max;
}

wxString WeeklyTreeModel::GetColumnType(unsigned int col) const
{
    if (col == Col_Id) {
        return "long";
    } else {
        return "string";
    }
}

void WeeklyTreeModel::GetValue(wxVariant& variant, const wxDataViewItem& item, unsigned int col) const
{
    wxASSERT(item.IsOk());

    WeeklyTreeModelNode* node = (WeeklyTreeModelNode*) item.GetID();
    switch (col) {
    case Col_Project:
        variant = node->GetProjectName();
        break;
    case Col_Duration:
        variant = node->GetDuration();
        break;
    case Col_Category:
        variant = node->GetCategoryName();
        break;
    case Col_Description:
        variant = node->GetDescription();
        break;
    case Col_Id:
        variant = (long) node->GetTaskItemId();
        break;
    case Col_Max:
    default:
        wxLogError("WeeklyTreeModel::GetValue: wrong column %d", col);
        break;
    }
}

bool WeeklyTreeModel::SetValue(const wxVariant& variant, const wxDataViewItem& item, unsigned int col)
{
    wxASSERT(item.IsOk());

    WeeklyTreeModelNode* node = (WeeklyTreeModelNode*) item.GetID();
    switch (col) {
    case Col_Project:
        node->SetProjectName(variant.GetString());
        break;
    case Col_Duration:
        node->SetDuration(variant.GetString());
        break;
    case Col_Category:
        node->SetCategoryName(variant.GetString());
        break;
    case Col_Description:
        node->SetDescription(variant.GetString());
        break;
    case Col_Id:
        node->SetTaskItemId(variant.GetInteger());
        break;
    case Col_Max:
    default:
        wxLogError("WeeklyTreeModel::SetValue: wrong column %d", col);
        break;
    }

    return false;
}

bool WeeklyTreeModel::IsEnabled(const wxDataViewItem& item, unsigned int col) const
{
    return true;
}

wxDataViewItem WeeklyTreeModel::GetParent(const wxDataViewItem& item) const
{
    if (!item.IsOk()) {
        return wxDataViewItem(0);
    }

    WeeklyTreeModelNode* node = (WeeklyTreeModelNode*) item.GetID();

    if (node == pRoot) {
        return wxDataViewItem(0);
    }

    return wxDataViewItem((void*) node->GetParent());
}

bool WeeklyTreeModel::IsContainer(const wxDataViewItem& item) const
{
    if (!item.IsOk()) {
        return true;
    }

    WeeklyTreeModelNode* node = (WeeklyTreeModelNode*) item.GetID();
    return node->IsContainer();
}

unsigned int WeeklyTreeModel::GetChildren(const wxDataViewItem& parent, wxDataViewItemArray& array) const
{
    WeeklyTreeModelNode* node = (WeeklyTreeModelNode*) parent.GetID();
    if (!node) {
        array.Add(wxDataViewItem((void*) pRoot));
        return 1;
    }

    if (node->GetChildCount() == 0) {
        return 0;
    }

    unsigned int count = node->GetChildren().GetCount();
    for (unsigned int pos = 0; pos < count; pos++) {
        WeeklyTreeModelNode* child = node->GetChildren().Item(pos);
        array.Add(wxDataViewItem((void*) child));
    }

    return count;
}

void WeeklyTreeModel::Delete(const wxDataViewItem& item)
{
    WeeklyTreeModelNode* node = (WeeklyTreeModelNode*) item.GetID();
    if (!node) { // happens if item.IsOk()==false
        return;
    }

    wxDataViewItem parent(node->GetParent());
    if (!parent.IsOk()) { // this means that the root node was clicked and has no parent
        return;
    }

    // do not delete the special nodes
    for (std::size_t i = 0; i < NumberOfDays; i++) {
        if (node == pDayNodes[i]) {
            return;
        }
    }

    node->GetParent()->GetChildren().Remove(node);
    delete node;

    ItemDeleted(parent, item);
}

void WeeklyTreeModel::ClearAll()
{
    UpdateNodeLabels();

    for (std::size_t i = 0; i < NumberOfDays; i++) {
        ClearDayNodes(pDayNodes[i]);
    }
}

wxDataViewItem WeeklyTreeModel::ExpandRootNode()
{
    return wxDataViewItem((void*) pRoot);
}

wxDataViewItemArray WeeklyTreeModel::CollapseDayNodes()
{
    wxDataViewItemArray array;
    for (std::size_t i = 0; i < NumberOfDays; i++) {
        array.Add(wxDataViewItem((void*)pDayNodes[i]));
    }
    return array;
}

wxDateTime WeeklyTreeModel::GetDateFromDataViewItem(const wxDataViewItem& item)
{
    WeeklyTreeModelNode* node = (WeeklyTreeModelNode*) item.GetID();

    if (node->GetParent() == pDayNodes[constants::Days::Monday]) {
        return mDateTraverser.GetDayDate(constants::Days::Monday);
    } else if (node->GetParent() == pDayNodes[constants::Days::Tuesday]) {
        return mDateTraverser.GetDayDate(constants::Days::Tuesday);
    } else if (node->GetParent() == pDayNodes[constants::Days::Wednesday]) {
        return mDateTraverser.GetDayDate(constants::Days::Wednesday);
    } else if (node->GetParent() == pDayNodes[constants::Days::Thursday]) {
        return mDateTraverser.GetDayDate(constants::Days::Thursday);
    } else if (node->GetParent() == pDayNodes[constants::Days::Friday]) {
        return mDateTraverser.GetDayDate(constants::Days::Friday);
    } else if (node->GetParent() == pDayNodes[constants::Days::Saturday]) {
        return mDateTraverser.GetDayDate(constants::Days::Saturday);
    } else if (node->GetParent() == pDayNodes[constants::Days::Sunday]) {
        return mDateTraverser.GetDayDate(constants::Days::Sunday);
    } else {
        return wxDateTime::Now();
    }
}

void WeeklyTreeModel::SetDateTraverser(const DateTraverser& dateTraverser)
{
    mDateTraverser = dateTraverser;
}

void WeeklyTreeModel::SetupNodes()
{
    wxString weekLabel = wxString::Format(WeekLabel,
        mDateTraverser.GetDayISODate(constants::Days::Monday),
        mDateTraverser.GetDayISODate(constants::Days::Sunday));

    pRoot = new WeeklyTreeModelNode(nullptr, weekLabel);

    for (std::size_t i = 0; i < NumberOfDays; i++) {
        wxString label = wxString::Format(DayDateLabels[i], mDateTraverser.GetDayISODate(constants::MapIndexToEnum(i)));
        pDayNodes[i] = new WeeklyTreeModelNode(pRoot, label);
        pRoot->Append(pDayNodes[i]);
    }
}

void WeeklyTreeModel::AddMany(WeeklyTreeModelNode* dayNodeToAdd,
    std::vector<std::unique_ptr<model::TaskItemModel>>& dayTasksToAdd)
{
    for (const auto& taskToAdd : dayTasksToAdd) {
        auto node = new WeeklyTreeModelNode(dayNodeToAdd,
            taskToAdd->GetProject()->GetDisplayName(),
            taskToAdd->GetDuration(),
            taskToAdd->GetCategory()->GetName(),
            taskToAdd->GetDescription(),
            taskToAdd->GetTaskItemId());

        dayNodeToAdd->Append(node);
    }
}

void WeeklyTreeModel::ClearDayNodes(WeeklyTreeModelNode* node)
{
    wxDataViewItemArray itemsRemoved;
    unsigned int count = node->GetChildCount();

    for (unsigned int pos = 0; pos < count; pos++) {
        WeeklyTreeModelNode* child = node->GetChildren().Item(pos);
        itemsRemoved.Add(wxDataViewItem((void*) child));
    }

    for (auto child : node->GetChildren()) {
        delete child;
        child = nullptr;
    }

    node->GetChildren().clear();

    count = node->GetChildCount();

    wxDataViewItem parent((void*) node);
    ItemsDeleted(parent, itemsRemoved);
}

void WeeklyTreeModel::UpdateNodeLabels()
{
    wxString weekLabel = wxString::Format(WeekLabel,
        mDateTraverser.GetDayISODate(constants::Days::Monday),
        mDateTraverser.GetDayISODate(constants::Days::Sunday));

    pRoot->SetProjectName(weekLabel);
    for (std::size_t i = 0; i < NumberOfDays; i++) {
        wxString label = wxString::Format(DayDateLabels[i], mDateTraverser.GetDayISODate(constants::MapIndexToEnum(i)));
        pDayNodes[i]->SetProjectName(label);
    }
}
} // namespace app::dv
