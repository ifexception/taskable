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

#include "employer_service.hh"

#include "db_connection.hh"
#include "../db/command.hh"
#include "../db/query.hh"
#include "../db/copy_options.hh"

namespace app::services
{
void employer_service::create_new_employer(const std::string& employerName)
{
    std::string cmd("INSERT INTO employers (name, is_active) VALUES (?, 1);");
    auto instance = db_connection::get_instance().get_database();
    db::command command(*instance, cmd);
    command.bind(1, employerName, db::copy_options::NoCopy);
    command.execute();
}

std::vector<models::employer> employer_service::get_active_employers()
{
    std::string qry("SELECT * FROM employers WHERE is_active = 1;");
    auto instance = db_connection::get_instance().get_database();
    db::query query(*instance, qry);

    std::vector<models::employer> employers;
    while (query.run()) {
        db::column column(query.get_handle());
        models::employer employer;

        employer.employer_id = column.get<int>(0);
        employer.employer_name = column.get<std::string>(1);
        employer.date_created_utc = column.get<int>(2);
        employer.date_modified_utc = column.get<int>(3);
        employer.is_active = column.get<int>(4);
        employers.push_back(employer);
    }

    return employers;
}
} // namespace app::services
