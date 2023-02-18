; -- Taskable.iss --
; Installer file for Taskable application

#define TaskableVersion "1.6.3"

[Setup]
AppName=Taskable
AppVersion={#TaskableVersion}
AppContact=szymonwelgus at gmail dot com
AppCopyright=Copyright (C) 2023 Szymon Welgus
AppPublisher=Szymon Welgus
AppId={{6BE5E5E6-68BF-4AF7-A9E5-FF919709E86C}
AppVerName=Taskable {#TaskableVersion}
WizardStyle=modern
DefaultDirName={commonpf}\Taskable
DefaultGroupName=Taskable
UninstallDisplayIcon={app}\Taskable.exe
Compression=lzma2
SolidCompression=yes
OutputDir=Installer
OutputBaseFilename=Taskable-x86.1.6.3-Installer
LicenseFile=License.txt
WindowResizable=no
DisableWelcomePage=no
SetupIconFile=taskable.ico

[Files]
Source: "Taskable.exe"; DestDir: "{app}"
Source: "fmt.dll"; DestDir: "{app}"
Source: "jpeg62.dll"; DestDir: "{app}"
Source: "libcurl.dll"; DestDir: "{app}"
Source: "liblzma.dll"; DestDir: "{app}"
Source: "libpng16.dll"; DestDir: "{app}"
Source: "pcre2-16.dll"; DestDir: "{app}"
Source: "spdlog.dll"; DestDir: "{app}"
Source: "sqlite3.dll"; DestDir: "{app}"
Source: "tiff.dll"; DestDir: "{app}"
Source: "wxbase32u_vc.dll"; DestDir: "{app}"
Source: "wxmsw32u_core_vc.dll"; DestDir: "{app}"
Source: "zlib1.dll"; DestDir: "{app}"
Source: "taskable.toml"; DestDir: "{userappdata}\Taskable"; Flags: onlyifdoesntexist
Source: "create-taskable.sql"; DestDir: "{app}"
Source: "seed-taskable.sql"; DestDir: "{app}"

[Dirs]
Name: "{userdocs}\Taskable"

[Components]
Name: "main"; Description: "Core files"; Types: full; Flags: fixed

[Tasks]
Name: desktopicon; Description: "Create a &desktop icon"; GroupDescription: "Additional icons:"; Components: main
Name: quicklaunchicon; Description: "Create a &Quick Launch icon"; GroupDescription: "Additional icons:"; Components: main; Flags: unchecked

[Icons]
Name: "{group}\Taskable"; Filename: "{app}\Taskable.exe"; AppUserModelID: "ifexception.Taskable"
Name: "{autodesktop}\Taskable"; Filename: "{app}\Taskable.exe"; AppUserModelID: "ifexception.Taskable"

[Messages]
WelcomeLabel1=Welcome to the Taskable Installation Wizard

[UninstallDelete]
Type: filesandordirs; Name: "{userappdata}\Taskable\logs"
Type: files; Name: "{userappdata}\Taskable\taskable.toml"
Type: filesandordirs; Name: "{userappdata}\Taskable"
Type: filesandordirs; Name: "{userdocs}\Taskable"

[Registry]
Root: HKCU; Subkey: "Software\Taskable"; Flags: uninsdeletekey
Root: HKCU; Subkey: "Software\Taskable"; ValueType: dword; ValueName: "Installed"; ValueData: 1;  Flags: uninsdeletekey
Root: HKCU; Subkey: "Software\Taskable"; ValueType: string; ValueName: "Version"; ValueData: {#TaskableVersion};  Flags: preservestringtype uninsdeletekey
Root: HKCU; Subkey: "Software\Taskable"; ValueType: dword; ValueName: "DatabaseUpgrade"; ValueData: 1;  Flags: uninsdeletekey
Root: HKCR; Subkey: "Applications\Taskable.exe"; ValueType: none; ValueName: "IsHostApp"

