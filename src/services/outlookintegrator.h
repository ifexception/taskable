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

#include <vector>

#include <wx/wx.h>
#include <wx/msw/ole/automtn.h>
#include <wx/msw/ole/oleutils.h>

#include "../models/meetingmodel.h"

#ifdef __WINDOWS__
BOOL IsElevated();
#endif // __WINDOWS__

namespace app::svc
{
struct Meeting {
    wxWindowID Identifier;
    wxString Subject;
    wxString Body;
    wxString Start;
    wxString End;
    int Duration;
    wxString Location;

    Meeting() {
        Identifier = wxID_ANY;
        Subject = wxGetEmptyString();
        Body = wxGetEmptyString();
        Start = wxGetEmptyString();
        End = wxGetEmptyString();
        Duration = -1;
        Location = wxGetEmptyString();
    }
};

class OutlookIntegrator
{
public:
    OutlookIntegrator();
    ~OutlookIntegrator() = default;

    bool TryGetOutlookInstance();

    bool Execute();

    std::vector<Meeting*> GetMeetings();

private:
    bool IterateAndGetCalendarMeetings();

    wxAutomationObject mOutlookApplication;

    std::vector<Meeting*> mMeetingsList;
};
} // namespace app::svc
