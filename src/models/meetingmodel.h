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

#pragma once

#include <cstdint>
#include <memory>

#include <wx/string.h>
#include <wx/datetime.h>

namespace app::model
{
class MeetingModel final
{
public:
    MeetingModel();
    MeetingModel(int meetingId);
    MeetingModel(int meetingId,
        int duration,
        wxString location,
        wxString subject,
        wxString body,
        int dateCreated,
        int dateModified,
        bool isActive);
    ~MeetingModel() = default;

    const int GetMeetingId();
    const int GetDuration();
    const wxDateTime GetStart() const;
    const wxDateTime GetEnd() const;
    const wxString GetLocation() const;
    const wxString GetSubject() const;
    const wxString GetBody();
    const bool* Attended();
    const wxDateTime GetDateCreated();
    const wxDateTime GetDateModified();
    const bool IsActive();

    void SetMeetingId(int meetingId);
    void SetDuration(int duration);
    void SetStart(const wxString& start);
    void SetEnd(const wxString& end);
    void SetStart(const wxDateTime& start);
    void SetEnd(const wxDateTime& end);
    void SetLocation(const wxString& location);
    void SetSubject(const wxString& subject);
    void SetBody(const wxString& body);
    void Attended(std::unique_ptr<bool> attended);
    void SetDateCreated(const wxDateTime& dateCreated);
    void SetDateUpdated(const wxDateTime& dateModified);
    void IsActive(bool isActive);

private:
    int mMeetingId;
    int mDuration;
    wxDateTime mStarting;
    wxDateTime mEnding;
    wxString mLocation;
    wxString mSubject;
    wxString mBody;
    std::unique_ptr<bool> pAttended;
    wxDateTime mDateCreated;
    wxDateTime mDateModified;
    bool bIsActive;
};
} // namespace app::model
