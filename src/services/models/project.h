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

#include <string>

namespace app::models
{
struct project
{
    project() = default;
    explicit project(int projectId,
        std::string projectName,
        std::string displayName,
        int dateCreatedUtc,
        int dateModifiedUtc,
        int isActive,
        std::string employerName,
        std::string clientName);
    explicit project(int projectId,
        std::string projectName,
        std::string displayName,
        int dateCreatedUtc,
        int dateModifiedUtc,
        int isActive,
        int employerId,
        std::string employerName,
        std::string clientName);
    ~project() = default;

    int project_id;
    std::string project_name;
    std::string display_name;
    int date_created_utc;
    int date_modified_utc;
    int is_active;
    int employer_id;
    int client_id;
    std::string employer_name;
    std::string client_name;

    static const std::string createNewProject;
    static const std::string getProjects;
    static const std::string getProjectById;
    static const std::string updateProject;
    static const std::string deleteProject;
};
}
