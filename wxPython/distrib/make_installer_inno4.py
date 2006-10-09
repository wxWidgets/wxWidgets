# -*- coding: iso-8859-1 -*-
#----------------------------------------------------------------------
# Name:        make_installer.py
# Purpose:     A script to create the wxPython windows installer
#
# Author:      Robin Dunn
#
# Created:     30-April-2001
# RCS-ID:      $Id$
# Copyright:   (c) 2003 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------

"""
This script will generate a setup script for InnoSetup and then run it
to make the installer executable.  If all goes right the proper versions
of Python and wxWindows (including hybrid/final settings) will all be
calculated based on what _core_.pyd imports and an appropriate installer
will be created.
"""


import sys, os, time

KEEP_TEMPS = 1
# default InnoSetup installer location
ISCC = r"C:\progra~1\innose~1\ISCC.exe %s"

if os.environ.has_key("INNO4"):
    ISCC = os.environ["INNO4"]


#----------------------------------------------------------------------

ISS_Template = r'''

[Setup]
AppName = wxPython%(SHORTVER)s-%(CHARTYPE)s-%(PYVER)s
AppVerName = wxPython %(VERSION)s (%(CHARTYPE)s) for Python %(PYTHONVER)s
OutputBaseFilename = wxPython%(SHORTVER)s-win32-%(CHARTYPE)s-%(VERSION)s-%(PYVER)s
AppCopyright = Copyright © 2006 Total Control Software
DefaultDirName = {code:GetInstallDir|c:\DoNotInstallHere}
DefaultGroupName = wxPython %(VERSION)s (%(CHARTYPE)s) for Python %(PYTHONVER)s
AdminPrivilegesRequired = no
OutputDir = dist
WizardStyle = modern
UninstallStyle = modern
DisableStartupPrompt = true
Compression = bzip
DirExistsWarning = no
DisableReadyMemo = true
DisableReadyPage = true
;;DisableDirPage = true
DisableProgramGroupPage = true
;DisableAppendDir = true
UsePreviousAppDir = no
UsePreviousGroup = no

AppPublisher = Total Control Software
AppPublisherURL = http://wxPython.org/
AppSupportURL = http://wxPython.org/maillist.php
AppUpdatesURL = http://wxPython.org/download.php
AppVersion = %(VERSION)s-%(CHARTYPE)s

UninstallFilesDir = {app}\%(PKGDIR)s
LicenseFile = licence\licence.txt
%(IFSFILEREF)s

;; WizardDebug = yes

;;------------------------------------------------------------

[Components]
Name: core;     Description: "wxPython modules and library";              Types: full custom;  Flags: fixed
Name: manifest; Description: "Manifest files for XP Themed LnF";          Types: full
Name: pthfile;  Description: "Make this install be the default wxPython"; Types: full 

;;------------------------------------------------------------

[Files]
%(RTDLL)s
Source: "%(WXDIR)s\lib\vc_dll\wx*%(WXDLLVER)s_*.dll";  DestDir: "{app}\%(PKGDIR)s\wx"; Components: core
%(MSLU)s

Source: "wx\_activex.pyd";                     DestDir: "{app}\%(PKGDIR)s\wx"; Components: core
Source: "wx\_calendar.pyd";                    DestDir: "{app}\%(PKGDIR)s\wx"; Components: core
Source: "wx\_controls_.pyd";                   DestDir: "{app}\%(PKGDIR)s\wx"; Components: core
Source: "wx\_core_.pyd";                       DestDir: "{app}\%(PKGDIR)s\wx"; Components: core
Source: "wx\_gdi_.pyd";                        DestDir: "{app}\%(PKGDIR)s\wx"; Components: core
Source: "wx\_animate.pyd";                      DestDir: "{app}\%(PKGDIR)s\wx"; Components: core
Source: "wx\_gizmos.pyd";                      DestDir: "{app}\%(PKGDIR)s\wx"; Components: core
Source: "wx\_glcanvas.pyd";                    DestDir: "{app}\%(PKGDIR)s\wx"; Components: core
Source: "wx\_grid.pyd";                        DestDir: "{app}\%(PKGDIR)s\wx"; Components: core
Source: "wx\_html.pyd";                        DestDir: "{app}\%(PKGDIR)s\wx"; Components: core
Source: "wx\_media.pyd";                       DestDir: "{app}\%(PKGDIR)s\wx"; Components: core
Source: "wx\_misc_.pyd";                       DestDir: "{app}\%(PKGDIR)s\wx"; Components: core
Source: "wx\_stc.pyd";                         DestDir: "{app}\%(PKGDIR)s\wx"; Components: core
Source: "wx\_webkit.pyd";                      DestDir: "{app}\%(PKGDIR)s\wx"; Components: core
Source: "wx\_windows_.pyd";                    DestDir: "{app}\%(PKGDIR)s\wx"; Components: core
Source: "wx\_wizard.pyd";                      DestDir: "{app}\%(PKGDIR)s\wx"; Components: core
Source: "wx\_xrc.pyd";                         DestDir: "{app}\%(PKGDIR)s\wx"; Components: core
Source: "wx\_aui.pyd";                         DestDir: "{app}\%(PKGDIR)s\wx"; Components: core
Source: "wx\_richtext.pyd";                    DestDir: "{app}\%(PKGDIR)s\wx"; Components: core
;;Source: "wx\_iewin.pyd";                       DestDir: "{app}\%(PKGDIR)s\wx"; Components: core
;;Source: "wx\_ogl.pyd";                         DestDir: "{app}\%(PKGDIR)s\wx"; Components: core


Source: "wx\*.py";                             DestDir: "{app}\%(PKGDIR)s\wx"; Components: core
Source: "wx\build\*.py";                       DestDir: "{app}\%(PKGDIR)s\wx\build"; Components: core
Source: "wx\lib\*.py";                         DestDir: "{app}\%(PKGDIR)s\wx\lib"; Components: core
Source: "wx\lib\analogclock\*.py";             DestDir: "{app}\%(PKGDIR)s\wx\lib\analogclock"; Components: core
Source: "wx\lib\analogclock\lib_setup\*.py";   DestDir: "{app}\%(PKGDIR)s\wx\lib\analogclock\lib_setup"; Components: core
Source: "wx\lib\colourchooser\*.py";           DestDir: "{app}\%(PKGDIR)s\wx\lib\colourchooser"; Components: core
Source: "wx\lib\editor\*.py";                  DestDir: "{app}\%(PKGDIR)s\wx\lib\editor"; Components: core
Source: "wx\lib\editor\*.txt";                 DestDir: "{app}\%(PKGDIR)s\wx\lib\editor"; Components: core
Source: "wx\lib\mixins\*.py";                  DestDir: "{app}\%(PKGDIR)s\wx\lib\mixins"; Components: core
Source: "wx\lib\masked\*.py";                  DestDir: "{app}\%(PKGDIR)s\wx\lib\masked"; Components: core
Source: "wx\lib\ogl\*.py";                     DestDir: "{app}\%(PKGDIR)s\wx\lib\ogl"; Components: core
Source: "wx\lib\floatcanvas\*.py";             DestDir: "{app}\%(PKGDIR)s\wx\lib\floatcanvas"; Components: core
Source: "wx\py\*.py";                          DestDir: "{app}\%(PKGDIR)s\wx\py"; Components: core
Source: "wx\py\*.txt";                         DestDir: "{app}\%(PKGDIR)s\wx\py"; Components: core
Source: "wx\py\*.ico";                         DestDir: "{app}\%(PKGDIR)s\wx\py"; Components: core
Source: "wx\py\*.png";                         DestDir: "{app}\%(PKGDIR)s\wx\py"; Components: core
Source: "wx\py\tests\*.py";                    DestDir: "{app}\%(PKGDIR)s\wx\py\tests"; Components: core
Source: "wx\tools\*.py";                       DestDir: "{app}\%(PKGDIR)s\wx\tools"; Components: core
Source: "wx\tools\XRCed\*.txt";                DestDir: "{app}\%(PKGDIR)s\wx\tools\XRCed"; Components: core
Source: "wx\tools\XRCed\sawfishrc";            DestDir: "{app}\%(PKGDIR)s\wx\tools\XRCed"; Components: core
Source: "wx\tools\XRCed\*.py";                 DestDir: "{app}\%(PKGDIR)s\wx\tools\XRCed"; Components: core
Source: "wx\tools\XRCed\*.xrc";                DestDir: "{app}\%(PKGDIR)s\wx\tools\XRCed"; Components: core
Source: "wx\tools\XRCed\*.ico";                DestDir: "{app}\%(PKGDIR)s\wx\tools\XRCed"; Components: core
Source: "wx\tools\XRCed\*.png";                DestDir: "{app}\%(PKGDIR)s\wx\tools\XRCed"; Components: core
Source: "wx\tools\XRCed\*.sh";                 DestDir: "{app}\%(PKGDIR)s\wx\tools\XRCed"; Components: core
Source: "wx\tools\XRCed\src-images\*.png";     DestDir: "{app}\%(PKGDIR)s\wx\tools\XRCed\src-images"; Components: core


Source: "wxPython\*.py";                          DestDir: "{app}\%(PKGDIR)s\wxPython"; Components: core
Source: "wxPython\lib\*.py";                      DestDir: "{app}\%(PKGDIR)s\wxPython\lib"; Components: core
Source: "wxPython\lib\colourchooser\*.py";        DestDir: "{app}\%(PKGDIR)s\wxPython\lib\colourchooser"; Components: core
Source: "wxPython\lib\editor\*.py";               DestDir: "{app}\%(PKGDIR)s\wxPython\lib\editor"; Components: core
Source: "wxPython\lib\mixins\*.py";               DestDir: "{app}\%(PKGDIR)s\wxPython\lib\mixins"; Components: core
Source: "wxPython\tools\*.py";                    DestDir: "{app}\%(PKGDIR)s\wxPython\tools"; Components: core


Source: "src\winxp.manifest";               DestDir: "{code:GetPythonDir}"; DestName: "python.exe.manifest";   Flags: sharedfile; Components:  manifest
Source: "src\winxp.manifest";               DestDir: "{code:GetPythonDir}"; DestName: "pythonw.exe.manifest";  Flags: sharedfile; Components: manifest
Source: "wxversion\wxversion.py";           DestDir: "{app}";  Flags: sharedfile;  Components: core
Source: "wxaddons\*.py";                    DestDir: "{app}\wxaddons";  Flags: sharedfile;  Components: core
Source: "src\wx.pth";                       DestDir: "{app}";  Flags: sharedfile;  Components: pthfile

%(LOCALE)s


Source: "scripts\*.py";                     DestDir: "{code:GetPythonDir}\Scripts"; Flags: sharedfile;  Components: core
Source: "scripts\helpviewer";               DestDir: "{code:GetPythonDir}\Scripts"; Flags: sharedfile;  Components: core
Source: "scripts\img2png";                  DestDir: "{code:GetPythonDir}\Scripts"; Flags: sharedfile;  Components: core
Source: "scripts\img2py";                   DestDir: "{code:GetPythonDir}\Scripts"; Flags: sharedfile;  Components: core
Source: "scripts\img2xpm";                  DestDir: "{code:GetPythonDir}\Scripts"; Flags: sharedfile;  Components: core
Source: "scripts\pyalacarte";               DestDir: "{code:GetPythonDir}\Scripts"; Flags: sharedfile;  Components: core
Source: "scripts\pyalamode";                DestDir: "{code:GetPythonDir}\Scripts"; Flags: sharedfile;  Components: core
Source: "scripts\pyshell";                  DestDir: "{code:GetPythonDir}\Scripts"; Flags: sharedfile;  Components: core
Source: "scripts\pycrust";                  DestDir: "{code:GetPythonDir}\Scripts"; Flags: sharedfile;  Components: core
Source: "scripts\pywrap";                   DestDir: "{code:GetPythonDir}\Scripts"; Flags: sharedfile;  Components: core
Source: "scripts\pywxrc";                   DestDir: "{code:GetPythonDir}\Scripts"; Flags: sharedfile;  Components: core
Source: "scripts\xrced";                    DestDir: "{code:GetPythonDir}\Scripts"; Flags: sharedfile;  Components: core


Source: "distrib\README.win32.txt";         DestDir: "{app}\%(PKGDIR)s\docs";  Flags: isreadme; Components: core
Source: "licence\*.txt";                    DestDir: "{app}\%(PKGDIR)s\docs\licence"; Components: core
Source: "docs\CHANGES.*";                   DestDir: "{app}\%(PKGDIR)s\docs"; Components: core
Source: "docs\MigrationGuide.*";            DestDir: "{app}\%(PKGDIR)s\docs"; Components: core
Source: "docs\default.css";                 DestDir: "{app}\%(PKGDIR)s\docs"; Components: core



;;------------------------------------------------------------

[Run]
;; Compile the .py files
Filename: "{code:GetPythonDir}\python.exe";  Parameters: "{code:GetPythonDir}\Lib\compileall.py {app}\%(PKGDIR)s"; Description: "Compile Python .py files to .pyc"; Flags: postinstall; Components: core

;; Recreate the tool scripts to use the paths on the users machine
Filename: "{code:GetPythonDir}\python.exe";  Parameters: "CreateBatchFiles.py";  WorkingDir: "{code:GetPythonDir}\Scripts";  Description: "Create batch files for tool scripts"; Flags: postinstall; Components: core




;;------------------------------------------------------------

[UninstallDelete]
Type: files; Name: "{app}\%(PKGDIR)s\wx\*.pyc";
Type: files; Name: "{app}\%(PKGDIR)s\wx\*.pyo";
Type: files; Name: "{app}\%(PKGDIR)s\wx\*.pyd";
Type: files; Name: "{app}\%(PKGDIR)s\wx\build\*.pyc";
Type: files; Name: "{app}\%(PKGDIR)s\wx\build\*.pyo";
Type: files; Name: "{app}\%(PKGDIR)s\wx\lib\*.pyc";
Type: files; Name: "{app}\%(PKGDIR)s\wx\lib\*.pyo";
Type: files; Name: "{app}\%(PKGDIR)s\wx\lib\analogclock\*.pyc";
Type: files; Name: "{app}\%(PKGDIR)s\wx\lib\analogclock\*.pyo";
Type: files; Name: "{app}\%(PKGDIR)s\wx\lib\analogclock\lib_setup\*.pyc";
Type: files; Name: "{app}\%(PKGDIR)s\wx\lib\analogclock\lib_setup\*.pyo";
Type: files; Name: "{app}\%(PKGDIR)s\wx\lib\colourchooser\*.pyc";
Type: files; Name: "{app}\%(PKGDIR)s\wx\lib\colourchooser\*.pyo";
Type: files; Name: "{app}\%(PKGDIR)s\wx\lib\editor\*.pyc";
Type: files; Name: "{app}\%(PKGDIR)s\wx\lib\editor\*.pyo";
Type: files; Name: "{app}\%(PKGDIR)s\wx\lib\mixins\*.pyc";
Type: files; Name: "{app}\%(PKGDIR)s\wx\lib\mixins\*.pyo";
Type: files; Name: "{app}\%(PKGDIR)s\wx\lib\masked\*.pyc";
Type: files; Name: "{app}\%(PKGDIR)s\wx\lib\masked\*.pyo";
Type: files; Name: "{app}\%(PKGDIR)s\wx\lib\ogl\*.pyc";
Type: files; Name: "{app}\%(PKGDIR)s\wx\lib\ogl\*.pyo";
Type: files; Name: "{app}\%(PKGDIR)s\wx\lib\floatcanvas\*.pyc";
Type: files; Name: "{app}\%(PKGDIR)s\wx\lib\floatcanvas\*.pyo";

Type: files; Name: "{app}\%(PKGDIR)s\wx\py\*.pyc";
Type: files; Name: "{app}\%(PKGDIR)s\wx\py\*.pyo";
Type: files; Name: "{app}\%(PKGDIR)s\wx\py\tests\*.pyc";
Type: files; Name: "{app}\%(PKGDIR)s\wx\py\tests\*.pyo";
Type: files; Name: "{app}\%(PKGDIR)s\wx\tools\*.pyc";
Type: files; Name: "{app}\%(PKGDIR)s\wx\tools\*.pyo";
Type: files; Name: "{app}\%(PKGDIR)s\wx\tools\XRCed\*.pyc";
Type: files; Name: "{app}\%(PKGDIR)s\wx\tools\XRCed\*.pyo";


Type: files; Name: "{app}\%(PKGDIR)s\wxPython\*.pyc";
Type: files; Name: "{app}\%(PKGDIR)s\wxPython\*.pyo";
Type: files; Name: "{app}\%(PKGDIR)s\wxPython\lib\*.pyc";
Type: files; Name: "{app}\%(PKGDIR)s\wxPython\lib\*.pyo";
Type: files; Name: "{app}\%(PKGDIR)s\wxPython\lib\colourchooser\*.pyc";
Type: files; Name: "{app}\%(PKGDIR)s\wxPython\lib\colourchooser\*.pyo";
Type: files; Name: "{app}\%(PKGDIR)s\wxPython\lib\editor\*.pyc";
Type: files; Name: "{app}\%(PKGDIR)s\wxPython\lib\editor\*.pyo";
Type: files; Name: "{app}\%(PKGDIR)s\wxPython\lib\mixins\*.pyc";
Type: files; Name: "{app}\%(PKGDIR)s\wxPython\lib\mixins\*.pyo";
Type: files; Name: "{app}\%(PKGDIR)s\wxPython\tools\*.pyc";
Type: files; Name: "{app}\%(PKGDIR)s\wxPython\tools\*.pyo";

Type: files; Name: "{app}\wxversion.pyc";
Type: files; Name: "{app}\wxversion.pyo";

Type: files; Name: "{app}\%(PKGDIR)s\wxaddons\*.pyc";
Type: files; Name: "{app}\%(PKGDIR)s\wxaddons\*.pyo";

%(UNINSTALL_BATCH)s

'''


#----------------------------------------------------------------------


IFS_Template = r"""
program Setup;
var
    PythonDir  : String;
    InstallDir : String;


function InitializeSetup(): Boolean;
begin

    (* -------------------------------------------------------------- *)
    (* Figure out what to use as a default installation dir           *)
    
    if not RegQueryStringValue(HKEY_LOCAL_MACHINE,
                               'Software\Python\PythonCore\%(PYTHONVER)s\InstallPath',
                               '', PythonDir) then begin

        if not RegQueryStringValue(HKEY_CURRENT_USER,
                                   'Software\Python\PythonCore\%(PYTHONVER)s\InstallPath',
                                   '', PythonDir) then begin

            MsgBox('No installation of Python %(PYTHONVER)s found in registry.' + #13 +
                   'Be sure to enter a pathname that places wxPython on the PYTHONPATH',
                   mbConfirmation, MB_OK);
            PythonDir := 'C:\Put a directory on PYTHONPATH here\';
        end;
    end;
    InstallDir := PythonDir;
    %(IF22)s

    Result := True;
end;


function GetPythonDir(Default: String): String;
begin
    Result := PythonDir;
end;


function GetInstallDir(Default: String): String;
begin
    Result := InstallDir;
end;



function UninstallOld(FileName: String): Boolean;
var
    ResultCode: Integer;
begin
    Result := False;
    if FileExists(FileName) then begin
        Result := True;
        ResultCode := MsgBox('A prior wxPython installation was found in this directory.  It' + #13 +
                             'is recommended that it be uninstalled first.' + #13#13 +
                             'Should I do it?',
                             mbConfirmation, MB_YESNO);
        if ResultCode = IDYES then begin
            InstExec(FileName, '/SILENT', WizardDirValue(), True, False, SW_SHOWNORMAL, ResultCode);

        end;
    end;
end;


function NextButtonClick(CurPage: Integer): Boolean;
var
    FileName: string;
    ResultCode: Integer;
begin
    Result := True;
    if CurPage <> wpSelectDir then Exit;
    if not UninstallOld(WizardDirValue() + '\wxPython\unins000.exe') then
        if not UninstallOld(WizardDirValue() + '\wx\unins000.exe') then
            UninstallOld(WizardDirValue() + '\%(PKGDIR)s\unins000.exe')
end;


begin
end.

"""

#----------------------------------------------------------------------

ISS_DocDemo_Template = r'''

[Setup]
AppName = wxPython%(SHORTVER)s-docs-demos
AppVerName = wxPython Docs and Demos %(VERSION)s
OutputBaseFilename = wxPython%(SHORTVER)s-win32-docs-demos-%(VERSION)s
AppCopyright = Copyright © 2006 Total Control Software
DefaultDirName = {pf}\wxPython%(SHORTVER)s Docs and Demos
DefaultGroupName = wxPython%(SHORTVER)s Docs Demos and Tools
;AlwaysCreateUninstallIcon = yes
AdminPrivilegesRequired = no
OutputDir = dist
WizardStyle = modern
UninstallStyle = modern
DisableStartupPrompt = true
Compression = bzip
DirExistsWarning = no
DisableReadyMemo = true
DisableReadyPage = true
;;DisableDirPage = true
DisableProgramGroupPage = no
;DisableAppendDir = no
UsePreviousAppDir = no
UsePreviousGroup = no

AppPublisher = Total Control Software
AppPublisherURL = http://wxPython.org/
AppSupportURL = http://wxPython.org/maillist.php
AppUpdatesURL = http://wxPython.org/download.php
AppVersion = %(VERSION)s

UninstallDisplayIcon = {app}\demo\wxpdemo.ico
UninstallFilesDir = {app}
;UninstallIconName = Uninstall
LicenseFile = licence\licence.txt

;; WizardDebug = yes


;;------------------------------------------------------------


[Files]
Source: "demo\demo.py";                     DestDir: "{app}\demo"; DestName: "demo.pyw"; 
Source: "demo\*.py";                        DestDir: "{app}\demo"; 
Source: "demo\*.xml";                       DestDir: "{app}\demo"; 
Source: "demo\*.txt";                       DestDir: "{app}\demo"; 

Source: "demo\bitmaps\*.bmp";               DestDir: "{app}\demo\bitmaps"; 
Source: "demo\bitmaps\*.gif";               DestDir: "{app}\demo\bitmaps"; 
Source: "demo\bitmaps\*.jpg";               DestDir: "{app}\demo\bitmaps"; 
Source: "demo\bitmaps\*.png";               DestDir: "{app}\demo\bitmaps";
Source: "demo\bitmaps\*.ico";               DestDir: "{app}\demo\bitmaps";

Source: "demo\bmp_source\*.gif";               DestDir: "{app}\demo\bmp_source"; 
Source: "demo\bmp_source\*.bmp";               DestDir: "{app}\demo\bmp_source"; 
Source: "demo\bmp_source\*.jpg";               DestDir: "{app}\demo\bmp_source"; 
Source: "demo\bmp_source\*.png";               DestDir: "{app}\demo\bmp_source"; 
Source: "demo\bmp_source\*.ico";               DestDir: "{app}\demo\bmp_source"; 

Source: "demo\data\*.htm";                  DestDir: "{app}\demo\data"; 
Source: "demo\data\*.html";                 DestDir: "{app}\demo\data"; 
Source: "demo\data\*.py";                   DestDir: "{app}\demo\data"; 
Source: "demo\data\*.png";                  DestDir: "{app}\demo\data"; 
Source: "demo\data\*.bmp";                  DestDir: "{app}\demo\data"; 
Source: "demo\data\*.dat";                  DestDir: "{app}\demo\data"; 
Source: "demo\data\*.txt";                  DestDir: "{app}\demo\data"; 
Source: "demo\data\*.wav";                  DestDir: "{app}\demo\data"; 
Source: "demo\data\*.wdr";                  DestDir: "{app}\demo\data"; 
Source: "demo\data\*.xrc";                  DestDir: "{app}\demo\data"; 
Source: "demo\data\*.swf";                  DestDir: "{app}\demo\data";
Source: "demo\data\*.mpg";                  DestDir: "{app}\demo\data";

;;Source: "demo\dllwidget\*.cpp";             DestDir: "{app}\demo\dllwidget"; 
;;Source: "demo\dllwidget\*.py";              DestDir: "{app}\demo\dllwidget"; 
;;Source: "demo\dllwidget\Makefile";          DestDir: "{app}\demo\dllwidget"; 
;;Source: "demo\dllwidget\makefile.*";        DestDir: "{app}\demo\dllwidget"; 

Source: "licence\*.txt";                    DestDir: "{app}\docs\licence"; 
;;Source: "%(WXDIR)s\docs\htmlhelp\wx.chm";   DestDir: "{app}\docs"; 
;;Source: "%(WXDIR)s\docs\htmlhelp\ogl.chm";  DestDir: "{app}\docs"; 
Source: "docs\README.txt";                  DestDir: "{app}\docs";  Flags: isreadme; 
Source: "docs\*.txt";                       DestDir: "{app}\docs"; 
Source: "docs\*.css";                       DestDir: "{app}\docs"; 
Source: "docs\*.html";                      DestDir: "{app}\docs"; 
Source: "docs\*.conf";                      DestDir: "{app}\docs"; 
Source: "docs\screenshots\*.png";           DestDir: "{app}\docs\screenshots"; 


Source: "samples\doodle\*.py";              DestDir: "{app}\samples\doodle"; 
Source: "samples\doodle\*.txt";             DestDir: "{app}\samples\doodle"; 
Source: "samples\doodle\*.bat";             DestDir: "{app}\samples\doodle"; 
Source: "samples\doodle\sample.ddl";        DestDir: "{app}\samples\doodle"; 
Source: "samples\doodle\superdoodle.iss";   DestDir: "{app}\samples\doodle"; 

Source: "samples\docview\*.py";                DestDir: "{app}\samples\docview"; 
Source: "samples\pydocview\*.py";              DestDir: "{app}\samples\pydocview";
Source: "samples\pydocview\*.png";             DestDir: "{app}\samples\pydocview";
Source: "samples\pydocview\*.txt";             DestDir: "{app}\samples\pydocview";

Source: "samples\ide\*.py";                       DestDir: "{app}\samples\ide"; 
Source: "samples\ide\activegrid\*.py";            DestDir: "{app}\samples\ide\activegrid"; 
Source: "samples\ide\activegrid\tool\*.py";       DestDir: "{app}\samples\ide\activegrid\tool"; 
Source: "samples\ide\activegrid\tool\data\*.txt"; DestDir: "{app}\samples\ide\activegrid\tool\data"; 
Source: "samples\ide\activegrid\util\*.py";       DestDir: "{app}\samples\ide\activegrid\util"; 
Source: "samples\ide\activegrid\model\*.py";      DestDir: "{app}\samples\ide\activegrid\model";

Source: "samples\embedded\*.py";            DestDir: "{app}\samples\embedded"; 
Source: "samples\embedded\*.cpp";           DestDir: "{app}\samples\embedded"; 
Source: "samples\embedded\*.txt";           DestDir: "{app}\samples\embedded"; 
Source: "samples\embedded\*.vc";            DestDir: "{app}\samples\embedded"; 
Source: "samples\embedded\*.unx";           DestDir: "{app}\samples\embedded"; 
Source: "samples\embedded\*.ico";           DestDir: "{app}\samples\embedded"; 
Source: "samples\embedded\*.xpm";           DestDir: "{app}\samples\embedded"; 
Source: "samples\embedded\*.rc";            DestDir: "{app}\samples\embedded"; 

Source: "samples\frogedit\*.py";            DestDir: "{app}\samples\frogedit"; 

Source: "samples\hangman\*.py";             DestDir: "{app}\samples\hangman"; 

Source: "samples\mainloop\*.py";            DestDir: "{app}\samples\mainloop"; 

Source: "samples\pySketch\*.py";            DestDir: "{app}\samples\pySketch"; 
Source: "samples\pySketch\images\*.bmp";    DestDir: "{app}\samples\pySketch\images"; 

Source: "samples\simple\*.py";              DestDir: "{app}\samples\simple"; 

Source: "samples\StyleEditor\*.txt";        DestDir: "{app}\samples\StyleEditor"; 
Source: "samples\StyleEditor\*.py";         DestDir: "{app}\samples\StyleEditor"; 
Source: "samples\StyleEditor\*.cfg";        DestDir: "{app}\samples\StyleEditor"; 

Source: "samples\wxProject\*.txt";          DestDir: "{app}\samples\wxProject"; 
Source: "samples\wxProject\*.py";           DestDir: "{app}\samples\wxProject"; 



Source: "scripts\pyshell";                  DestDir: "{app}\scripts"; DestName: "pyshell.pyw";   
Source: "scripts\pycrust";                  DestDir: "{app}\scripts"; DestName: "pycrust.pyw";   
Source: "scripts\pyalamode";                DestDir: "{app}\scripts"; DestName: "pyalamode.pyw"; 
Source: "scripts\pyalacarte";               DestDir: "{app}\scripts"; DestName: "pyalacarte.pyw";
Source: "scripts\xrced";                    DestDir: "{app}\scripts"; DestName: "xrced.pyw";     

Source: "wx\py\PyCrust.ico";                DestDir: "{app}\scripts"; 
Source: "wx\tools\XRCed\xrced.ico";         DestDir: "{app}\scripts"; 


;;------------------------------------------------------------

[Icons]
Name: "{group}\Run the wxPython DEMO"; Filename: "{app}\demo\demo.pyw";           WorkingDir: "{app}\demo";   IconFilename: "{app}\demo\wxpdemo.ico";    
Name: "{group}\PyCrust";               Filename: "{app}\scripts\pycrust.pyw";     WorkingDir: "c:\";          IconFilename: "{app}\scripts\PyCrust.ico"; 
Name: "{group}\PyShell";               Filename: "{app}\scripts\pyshell.pyw";     WorkingDir: "c:\";          IconFilename: "{app}\scripts\PyCrust.ico"; 
Name: "{group}\PyAlaMode";             Filename: "{app}\scripts\pyalamode.pyw";   WorkingDir: "c:\";          IconFilename: "{app}\scripts\PyCrust.ico"; 
Name: "{group}\PyAlaCarte";            Filename: "{app}\scripts\pyalacarte.pyw";  WorkingDir: "c:\";          IconFilename: "{app}\scripts\PyCrust.ico"; 
Name: "{group}\Resource Editor";       Filename: "{app}\scripts\xrced.pyw";       WorkingDir: "c:\";          IconFilename: "{app}\scripts\xrced.ico";   

Name: "{group}\Sample Apps";           Filename: "{app}\samples"; 

Name: "{group}\wxWidgets Reference";   Filename: "{app}\docs\wx.chm";                
Name: "{group}\Migration Guide";       Filename: "{app}\docs\MigrationGuide.html";   
Name: "{group}\Recent Changes";        Filename: "{app}\docs\CHANGES.html";          
Name: "{group}\Other Docs";            Filename: "{app}\docs";                       




;;------------------------------------------------------------

[UninstallDelete]
Type: files; Name: "{app}\demo\*.pyc";
Type: files; Name: "{app}\demo\*.pyo";
Type: files; Name: "{app}\demo\data\showTips";
Type: files; Name: "{app}\demo\data\*.pyc";
Type: files; Name: "{app}\demo\data\*.pyo";
Type: files; Name: "{app}\demo\dllwidget\*.pyc";
Type: files; Name: "{app}\demo\dllwidget\*.pyo";

Type: files; Name: "{app}\samples\doodle\*.pyc";
Type: files; Name: "{app}\samples\doodle\*.pyo";
Type: files; Name: "{app}\samples\embedded\*.pyc";
Type: files; Name: "{app}\samples\embedded\*.pyo";
Type: files; Name: "{app}\samples\frogedit\*.pyc";
Type: files; Name: "{app}\samples\frogedit\*.pyo";
Type: files; Name: "{app}\samples\hangman\*.pyc";
Type: files; Name: "{app}\samples\hangman\*.pyo";
Type: files; Name: "{app}\samples\hangman\*.txt";
Type: files; Name: "{app}\samples\mainloop\*.pyc";
Type: files; Name: "{app}\samples\mainloop\*.pyo";
Type: files; Name: "{app}\samples\pySketch\*.pyc";
Type: files; Name: "{app}\samples\pySketch\*.pyo";
Type: files; Name: "{app}\samples\simple\*.pyc";
Type: files; Name: "{app}\samples\simple\*.pyo";
Type: files; Name: "{app}\samples\StyleEditor\*.pyc";
Type: files; Name: "{app}\samples\StyleEditor\*.pyo";
Type: files; Name: "{app}\samples\wx_examples\basic\*.pyc";
Type: files; Name: "{app}\samples\wx_examples\basic\*.pyo";
Type: files; Name: "{app}\samples\wx_examples\hello\*.pyc";
Type: files; Name: "{app}\samples\wx_examples\hello\*.pyo";
Type: files; Name: "{app}\samples\wxProject\*.pyc";
Type: files; Name: "{app}\samples\wxProject\*.pyo";

Type: files; Name: "{app}\samples\ide\*.pyc";
Type: files; Name: "{app}\samples\ide\activegrid\*.pyc";
Type: files; Name: "{app}\samples\ide\activegrid\tool\*.pyc";
Type: files; Name: "{app}\samples\ide\activegrid\util\*.pyc";
Type: files; Name: "{app}\samples\ide\activegrid\model\*.pyc";
Type: files; Name: "{app}\samples\ide\*.pyo";
Type: files; Name: "{app}\samples\ide\activegrid\*.pyo";
Type: files; Name: "{app}\samples\ide\activegrid\tool\*.pyo";
Type: files; Name: "{app}\samples\ide\activegrid\util\*.pyo";
Type: files; Name: "{app}\samples\ide\activegrid\model\*.pyo";

Type: files; Name: "{app}\samples\docview\*.pyc";
Type: files; Name: "{app}\samples\pydocview\*.pyc";
Type: files; Name: "{app}\samples\docview\*.pyo";
Type: files; Name: "{app}\samples\pydocview\*.pyo";


'''

#----------------------------------------------------------------------

def find_DLLs():

    WXDLLVER = PYTHONVER = None

    proc = os.popen(r"dumpbin /imports wx\_core_.pyd", "r")
    lines = proc.readlines()
    proc.close()
    for line in lines:
        if line.startswith("    wxmsw"):
            WXDLLVER = line[9:14].split('_')[0]
            
        if line.startswith("    python"):
            PYTHONVER = line[10] + '.' + line[11]

    return WXDLLVER, PYTHONVER


#----------------------------------------------------------------------

locale_template = 'Source: "%s";  DestDir: "{app}\%s\%s"; Components: core'

def build_locale_string(pkgdir):
    stringlst = []

    def walk_helper(lst, dirname, files):
        for f in files:
            filename = os.path.join(dirname, f)
            if not os.path.isdir(filename):
                lst.append( locale_template % (filename, pkgdir, dirname) )

    os.path.walk('wx\\locale', walk_helper, stringlst)
    return '\n'.join(stringlst)


def get_system_dir():
    for p in [r"C:\WINNT\SYSTEM32",
              r"C:\WINDOWS\SYSTEM32",
              ]:
        if os.path.exists(p):
            return p
    raise IOError, "System dir not found"


def get_batch_files():
    globs = {}
    execfile("scripts/CreateBatchFiles.py", globs)
    scripts = globs["scripts"]
    scripts = ['Type: files; Name: "{code:GetPythonDir}\Scripts\%s.bat";' % i[0] for i in scripts]
    return '\n'.join(scripts)


runtime_template = 'Source: "%s"; DestDir: "{code:GetPythonDir}"; CopyMode: alwaysskipifsameorolder; Flags: uninsneveruninstall; Components: core'

def get_runtime_dlls(PYVER):
    if PYVER >= "py24":
        source = [ r"distrib\msw\msvcr71.dll",
                   r"distrib\msw\msvcp71.dll" ]
    else:
        source = [ r"distrib\msw\MSVCRT.dll",
                   r"distrib\msw\MSVCIRT.dll",
                   r"distrib\msw\MSVCP60.dll" ]
    DLLs = [runtime_template % dll for dll in source]    
    return '\n'.join(DLLs)


#----------------------------------------------------------------------

def main():

    verglob = {}
    execfile("wx/__version__.py", verglob)

    VERSION    = verglob["VERSION_STRING"]
    SHORTVER   = VERSION[:3]

    WXDLLVER, PYTHONVER = find_DLLs()

    PYVER           = "py" + PYTHONVER[0] + PYTHONVER[2]
    WXDIR           = os.environ["WXWIN"]
    WXPYDIR         = os.path.join(WXDIR, "wxPython")
    SYSDIR          = get_system_dir()
    ISSFILE         = "__wxPython.iss"
    ISSDEMOFILE     = "__wxPythonDemo.iss"
    IFSFILE         = "__wxPython.ifs"
    IFSFILEREF      = "CodeFile = " + IFSFILE
    IFSFILEREF = ""
    UNINSTALL_BATCH = get_batch_files()
    PKGDIR          = open('src/wx.pth').read()
    LOCALE          = build_locale_string(PKGDIR)
    RTDLL           = get_runtime_dlls(PYVER)
    
    print """Building Win32 installer for wxPython:
    VERSION    = %(VERSION)s
    SHORTVER   = %(SHORTVER)s
    WXDLLVER   = %(WXDLLVER)s
    PYTHONVER  = %(PYTHONVER)s
    PYVER      = %(PYVER)s
    PKGDIR     = %(PKGDIR)s
    WXDIR      = %(WXDIR)s
    WXPYDIR    = %(WXPYDIR)s
    SYSDIR     = %(SYSDIR)s
    """ % vars()

    if PYTHONVER >= "2.2":
        IF22 = r"InstallDir := InstallDir + '\Lib\site-packages';"
    else:
        IF22 = ""

    # Starting with 2.3.3 the hybrid build is the release build too, so
    # no need to label it that way.
    ##if WXDLL.find("h") != -1:
    ##    PYVER = PYVER + "-hybrid"

    MSLU=''
    CHARTYPE='ansi'
    if "UNICODE=1" in sys.argv:
        MSLU=r'Source: "distrib\msw\unicows.dll";  DestDir: "{code:GetPythonDir}"; Components: core' % vars()
        CHARTYPE='unicode'

    global ISS_Template
    global IFS_Template
    global ISS_DocDemo_Template

    ISS_Template = ISS_Template + "\n[Code]\n" + IFS_Template

    f = open(ISSFILE, "w")
    f.write(ISS_Template % vars())
    f.close()

    f = open(ISSDEMOFILE, "w")
    f.write(ISS_DocDemo_Template % vars())
    f.close()

    TOOLS = os.environ['TOOLS']
    if TOOLS.startswith('/cygdrive'):
        TOOLS = r"c:\TOOLS"  # temporary hack until I convert everything over to bash
        
    os.system(ISCC % (ISSFILE))
    os.system(ISCC % (ISSDEMOFILE))
    
    if not KEEP_TEMPS:
        time.sleep(1)
        os.remove(ISSFILE)
        os.remove(ISSDEMOFILE)
        os.remove(IFSFILE)


#----------------------------------------------------------------------

if __name__ == "__main__":
    main()



#----------------------------------------------------------------------


