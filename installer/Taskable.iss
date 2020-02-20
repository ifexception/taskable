; -- Taskable.iss --
; Installer file for Taskable application

#define TaskableVersion "0.84.1"

[Setup]
AppName=Taskable
AppVersion={#TaskableVersion}
AppContact=szymonwelgus at gmail dot com
AppCopyright=Copyright (2019-2020) Szymon Welgus
AppPublisher=Szymon Welgus
AppId={{6BE5E5E6-68BF-4AF7-A9E5-FF919709E86C}}
; AppReadmeFile=https://github.com/ifexception/taskable
AppVerName=Taskable {#TaskableVersion}
WizardStyle=modern
DefaultDirName={commonpf}\Taskable
DefaultGroupName=Taskable
UninstallDisplayIcon={app}\Taskable.exe
Compression=lzma2
SolidCompression=yes
OutputDir=Installer
OutputBaseFilename=Taskable-x86.0.84.1-Installer
LicenseFile=License.txt
SetupIconFile=taskable.ico
WindowResizable=no
DisableWelcomePage=no

[Files]
Source: "Taskable.exe"; DestDir: "{app}"
Source: "fmt.dll"; DestDir: "{app}"
Source: "jpeg62.dll"; DestDir: "{app}"
Source: "libcurl.dll"; DestDir: "{app}"
Source: "libpng16.dll"; DestDir: "{app}"
Source: "lzma.dll"; DestDir: "{app}"
Source: "sqlite3.dll"; DestDir: "{app}"
Source: "tiff.dll"; DestDir: "{app}"
Source: "wxbase313u_vc_custom.dll"; DestDir: "{app}"
Source: "wxmsw313u_core_vc_custom.dll"; DestDir: "{app}"
Source: "zlib1.dll"; DestDir: "{app}"
Source: "taskable.ini"; DestDir: "{userappdata}\Taskable"

[Components]
Name: "main"; Description: "Core files"; Types: full; Flags: fixed

[Tasks]
Name: desktopicon; Description: "Create a &desktop icon"; GroupDescription: "Additional icons:"; Components: main
Name: quicklaunchicon; Description: "Create a &Quick Launch icon"; GroupDescription: "Additional icons:"; Components: main; Flags: unchecked

[Icons]
Name: "{group}\Taskable"; Filename: "{app}\Taskable.exe"
Name: "{autodesktop}\Taskable"; Filename: "{app}\Taskable.exe"

[Messages]
WelcomeLabel1=Welcome to the Taskable Installation Wizard

[UninstallDelete]
Type: filesandordirs; Name: "{userappdata}\Taskable\logs"
Type: filesandordirs; Name: "{userappdata}\Taskable\backups"
Type: files; Name: "{userappdata}\Taskable\data\taskable.db"
Type: files; Name: "{userappdata}\Taskable\taskable.ini"
Type: filesandordirs; Name: "{userappdata}\Taskable"

[Registry]
Root: HKCU; Subkey: "Software\Taskable"; Flags: uninsdeletekey
Root: HKCU; Subkey: "Software\Taskable"; ValueType: dword; ValueName: "IsInstalled"; ValueData: 1;  Flags: uninsdeletekey
Root: HKCU; Subkey: "Software\Taskable"; ValueType: string; ValueName: "Version"; ValueData: {#TaskableVersion};  Flags: preservestringtype uninsdeletekey

