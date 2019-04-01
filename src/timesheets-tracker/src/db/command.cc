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
//

#include "command.hh"

namespace app::db
{

command::command_stream::command_stream(command& command, int index)
    : rCommand(command)
    , mIndex(index)
{
}

command::command_stream& command::command_stream::operator<<(const std::string& value)
{
    rCommand.bind(mIndex, value, copy_options::Copy);
    ++mIndex;
    return *this;
}

command::command(database& db, const std::string& query)
    : statement(db, query)
{
}

command::command_stream command::binder(int index)
{
    return command_stream(*this, index);
}

void command::execute()
{
    statement::run();
}
}
