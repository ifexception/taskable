//  TimesheetsTracker is a desktop that aids you in tracking your timesheets
//  and seeing what work you have done.
//
//  Copyright(C) <2018> <Szymon Welgus>
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
#include <vector>

#include "models/employer.hh"
#include "models/client.hh"

namespace app::services
{
struct db_service
{
    db_service() = default;
    ~db_service() = default;

    void create_new_employer(const std::string& employerName);

    std::vector<models::employer> get_active_employers();

    void create_new_client(const std::string& name, const int employerId);

    std::vector<models::client> get_clients_by_employer_id(const int employerId);
};
}
