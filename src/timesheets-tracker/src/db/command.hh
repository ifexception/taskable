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

#pragma once

#include <string>

#include "statement.hh"

namespace app::db
{

class database;

class command : public statement
{
  public:
    class command_stream
    {
      public:
        command_stream() = delete;
        explicit command_stream(command& command, int index);
        command_stream(const command_stream&) = delete;

        command_stream& operator=(const command_stream&) = delete;

        template <class T>
        command_stream& operator<<(T value);
        command_stream& operator<<(const std::string& value);

      private:
        command& rCommand;
        int mIndex;
    };

    command() = delete;
    command(const command&) = delete;
    explicit command(database& db, const std::string& query);
    virtual ~command() = default;

    command& operator=(const command&) = delete;

    command_stream binder(int index = 1);
    void execute();
};

template <class T>
command::command_stream& command::command_stream::operator << (T value)
{
    rCommand.bind(mIndex, value);
    ++mIndex++;
    return *this;
}
} // namespace app::db
