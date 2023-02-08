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
//


#include "constants.h"

app::constants::Days app::constants::MapIndexToEnum(int index)
{
    if (index == 0) {
        return Days::Monday;
    } else if (index == 1) {
        return Days::Tuesday;
    } else if (index == 2) {
        return Days::Wednesday;
    } else if (index == 3) {
        return Days::Thursday;
    } else if (index == 4) {
        return Days::Friday;
    } else if (index == 5) {
        return Days::Saturday;
    } else if (index == 6) {
        return Days::Sunday;
    }
    return Days::Monday;
}
