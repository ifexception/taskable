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

#define TASKS_TRACKER_MAJOR 0
#define TASKS_TRACKER_MINOR 9
#define TASKS_TRACKER_PATCH 8

#define TASKS_TRACKER_VERSION (TASKS_TRACKER_MAJOR * 1000 + TASKS_TRACKER_MINOR * 100 + TASKS_TRACKER_PATCH)

#define _STRINGIFY(s) #s
#define STRINGIFY(s) _STRINGIFY(s)

#define FILE_DESCRIPTION "Taskable"
#define FILE_VERSION TASKS_TRACKER_MAJOR, TASKS_TRACKER_MINOR, TASKS_TRACKER_PATCH
#define PRODUCT_VERSION FILE_VERSION
#define FILE_VERSION_STR STRINGIFY(TASKS_TRACKER_MAJOR) \
						 "." STRINGIFY(TASKS_TRACKER_MINOR) \
						 "." STRINGIFY(TASKS_TRACKER_PATCH)
#define PRODUCT_VERSION_STR FILE_VERSION_STR
#define INTERNAL_FILE_NAME FILE_DESCRIPTION ".exe"
#define ORIGINAL_FILE_NAME INTERNAL_FILE_NAME
#define PRODUCT_NAME FILE_DESCRIPTION
