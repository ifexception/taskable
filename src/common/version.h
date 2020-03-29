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

#define TASKABLE_MAJOR 0
#define TASKABLE_MINOR 95
#define TASKABLE_PATCH 15

#define TASKABLE_VERSION (TASKABLE_MAJOR * 1000 + TASKABLE_MINOR * 100 + TASKABLE_PATCH)

#define _STRINGIFY(s) #s
#define STRINGIFY(s) _STRINGIFY(s)

#define FILE_DESCRIPTION "Taskable"
#define FILE_VERSION TASKABLE_MAJOR, TASKABLE_MINOR, TASKABLE_PATCH
#define PRODUCT_VERSION FILE_VERSION
#define FILE_VERSION_STR STRINGIFY(TASKABLE_MAJOR) \
                         "." STRINGIFY(TASKABLE_MINOR) \
                         "." STRINGIFY(TASKABLE_PATCH)
#define PRODUCT_VERSION_STR FILE_VERSION_STR
#define INTERNAL_FILE_NAME FILE_DESCRIPTION ".exe"
#define ORIGINAL_FILE_NAME INTERNAL_FILE_NAME
#define PRODUCT_NAME FILE_DESCRIPTION
