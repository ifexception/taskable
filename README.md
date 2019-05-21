# Tasks Tracker Desktop App
A desktop app to help you manage how you've spent your time on tasks during the day by tracking the time you've spent on tasks throughout the day

## Getting Started
Ensure you have downloaded a suitable [version](https://cmake.org/download/) of CMake. CMake **3.8** is the minimum supported version.

### Windows
_This guide assumes you are using Visual Studio with CMake_

First, go to [vcpkg](https://github.com/Microsoft/vcpkg) and download the package manager.
Once you've installed and configured `vcpkg`, install the following libraries:

    * sqlite3 (3.0 +)
    * spdlog (1.2 +)
    * wxwidgets (3.1 +)
    * catch2 (2.4 +) 

Ensure that when you have started Visual Studio that the _CMake Server_ does not give any warnings about missing packages.
You can now use Visual Studio to build the project. 

### Linux
`//TODO`

## Installing
### Building from source
#### Windows
    * Open the folder in Visual Studio
    * Switch to `x86-Release` mode
    * Build the project
    * Run the install target

#### Linux
`//TODO`

### Windows Binaries
`// TODO`

## Version
_Still work in progress, no release timeline yet_

## License
This project is licensed under the GPL-3 license - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgements
    * Inspiration taken from SQLiteCpp [1]
    *

[1] - [SQLiteCPP](https://github.com/SRombauts/SQLiteCpp)

