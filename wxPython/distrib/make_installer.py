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
calculated based on what wxc.pyd imports and an appropriate installer
will be created.
"""


# TODO:  Remove the registry entry upon uninstall.

import sys, os, time

KEEP_TEMPS = 0
ISCC = r"%s\InnoSetup2Ex\ISCC.exe %s"

#----------------------------------------------------------------------

ISS_Template = r'''
;;------------------------------------------------------------

[Setup]
AppName = wxPython
AppVerName = wxPython %(VERSION)s for Python %(PYTHONVER)s
OutputBaseFilename = wxPythonWIN32-%(VERSION)s-%(PYVER)s
AppCopyright = Copyright © 2004 Total Control Software
DefaultDirName = {code:GetInstallDir|c:\DoNotInstallHere}
DefaultGroupName = wxPython %(SHORTVER)s for Python %(PYTHONVER)s
AlwaysCreateUninstallIcon = yes
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
DisableAppendDir = true
UsePreviousAppDir = no
UsePreviousGroup = no

UninstallFilesDir = {app}\wx
AppPublisherURL = http://wxPython.org/
LicenseFile = licence\licence.txt
CodeFile = %(IFSFILE)s

;; WizardDebug = yes

;;------------------------------------------------------------

[Components]
Name: core;     Description: "wxPython modules and library";  Types: full compact custom;  Flags: fixed
Name: docs;     Description: "Documentation";                 Types: full
Name: demo;     Description: "Demo application";              Types: full
Name: samples;  Description: "Sample applications";           Types: full
Name: manifest; Description: "Manifest files for XP LnF";     Types: full

;;------------------------------------------------------------

[Files]
Source: "distrib\msw\MSVCRT.dll";        DestDir: "{code:GetPythonDir}"; CopyMode: alwaysskipifsameorolder; Flags: uninsneveruninstall; Components: core
Source: "distrib\msw\MSVCIRT.dll";       DestDir: "{code:GetPythonDir}"; CopyMode: alwaysskipifsameorolder; Flags: uninsneveruninstall; Components: core
Source: "distrib\msw\MSVCP60.dll";       DestDir: "{code:GetPythonDir}"; CopyMode: alwaysskipifsameorolder; Flags: uninsneveruninstall; Components: core

Source: "%(WXDIR)s\lib\vc_dll\wx*%(WXDLLVER)s_*.dll";  DestDir: "{app}\wx"; Components: core
%(MSLU)s
Source: "wx\_core_.pyd";                       DestDir: "{app}\wx"; Components: core
Source: "wx\_gdi_.pyd";                        DestDir: "{app}\wx"; Components: core
Source: "wx\_windows_.pyd";                    DestDir: "{app}\wx"; Components: core
Source: "wx\_controls_.pyd";                   DestDir: "{app}\wx"; Components: core
Source: "wx\_misc_.pyd";                       DestDir: "{app}\wx"; Components: core
Source: "wx\_calendar.pyd";                    DestDir: "{app}\wx"; Components: core
Source: "wx\_grid.pyd";                        DestDir: "{app}\wx"; Components: core
Source: "wx\_html.pyd";                        DestDir: "{app}\wx"; Components: core
Source: "wx\_wizard.pyd";                      DestDir: "{app}\wx"; Components: core
Source: "wx\_gizmos.pyd";                      DestDir: "{app}\wx"; Components: core
Source: "wx\_glcanvas.pyd";                    DestDir: "{app}\wx"; Components: core
Source: "wx\_iewin.pyd";                       DestDir: "{app}\wx"; Components: core
Source: "wx\_activex.pyd";                     DestDir: "{app}\wx"; Components: core
Source: "wx\_ogl.pyd";                         DestDir: "{app}\wx"; Components: core
Source: "wx\_stc.pyd";                         DestDir: "{app}\wx"; Components: core
Source: "wx\_xrc.pyd";                         DestDir: "{app}\wx"; Components: core


Source: "wx\*.py";                             DestDir: "{app}\wx"; Components: core
Source: "wx\build\*.py";                       DestDir: "{app}\wx\build"; Components: core
Source: "wx\lib\*.py";                         DestDir: "{app}\wx\lib"; Components: core
;;Source: "wx\lib\*.wdr";                        DestDir: "{app}\wx\lib"; Components: core
Source: "wx\lib\colourchooser\*.py";           DestDir: "{app}\wx\lib\colourchooser"; Components: core
Source: "wx\lib\editor\*.py";                  DestDir: "{app}\wx\lib\editor"; Components: core
Source: "wx\lib\editor\*.txt";                 DestDir: "{app}\wx\lib\editor"; Components: core
Source: "wx\lib\mixins\*.py";                  DestDir: "{app}\wx\lib\mixins"; Components: core
Source: "wx\lib\masked\*.py";                  DestDir: "{app}\wx\lib\masked"; Components: core
Source: "wx\lib\ogl\*.py";                     DestDir: "{app}\wx\lib\ogl"; Components: core
Source: "wx\lib\floatcanvas\*.py";             DestDir: "{app}\wx\lib\floatcanvas"; Components: core
Source: "wx\py\*.py";                          DestDir: "{app}\wx\py"; Components: core
Source: "wx\py\*.txt";                         DestDir: "{app}\wx\py"; Components: core
Source: "wx\py\*.ico";                         DestDir: "{app}\wx\py"; Components: core
Source: "wx\py\*.png";                         DestDir: "{app}\wx\py"; Components: core
Source: "wx\py\tests\*.py";                    DestDir: "{app}\wx\py\tests"; Components: core
Source: "wx\tools\*.py";                       DestDir: "{app}\wx\tools"; Components: core
Source: "wx\tools\XRCed\*.txt";                DestDir: "{app}\wx\tools\XRCed"; Components: core
Source: "wx\tools\XRCed\sawfishrc";            DestDir: "{app}\wx\tools\XRCed"; Components: core
Source: "wx\tools\XRCed\*.py";                 DestDir: "{app}\wx\tools\XRCed"; Components: core
Source: "wx\tools\XRCed\*.xrc";                DestDir: "{app}\wx\tools\XRCed"; Components: core
Source: "wx\tools\XRCed\*.ico";                DestDir: "{app}\wx\tools\XRCed"; Components: core
Source: "wx\tools\XRCed\*.png";                DestDir: "{app}\wx\tools\XRCed"; Components: core
Source: "wx\tools\XRCed\*.sh";                 DestDir: "{app}\wx\tools\XRCed"; Components: core
Source: "wx\tools\XRCed\src-images\*.png";     DestDir: "{app}\wx\tools\XRCed\src-images"; Components: core


Source: "wxPython\*.py";                          DestDir: "{app}\wxPython"; Components: core
Source: "wxPython\lib\*.py";                      DestDir: "{app}\wxPython\lib"; Components: core
Source: "wxPython\lib\colourchooser\*.py";        DestDir: "{app}\wxPython\lib\colourchooser"; Components: core
Source: "wxPython\lib\editor\*.py";               DestDir: "{app}\wxPython\lib\editor"; Components: core
Source: "wxPython\lib\mixins\*.py";               DestDir: "{app}\wxPython\lib\mixins"; Components: core
Source: "wxPython\tools\*.py";                    DestDir: "{app}\wxPython\tools"; Components: core



%(LOCALE)s


Source: "demo\*.py";                        DestDir: "{app}\wx\demo"; Components: demo
Source: "demo\*.xml";                       DestDir: "{app}\wx\demo"; Components: demo
Source: "demo\*.txt";                       DestDir: "{app}\wx\demo"; Components: demo
Source: "demo\*.ico";                       DestDir: "{app}\wx\demo"; Components: demo

Source: "demo\bitmaps\*.bmp";               DestDir: "{app}\wx\demo\bitmaps"; Components: demo
Source: "demo\bitmaps\*.gif";               DestDir: "{app}\wx\demo\bitmaps"; Components: demo
Source: "demo\bitmaps\*.jpg";               DestDir: "{app}\wx\demo\bitmaps"; Components: demo
Source: "demo\bitmaps\*.png";               DestDir: "{app}\wx\demo\bitmaps"; Components: demo
Source: "demo\bitmaps\*.ico";               DestDir: "{app}\wx\demo\bitmaps"; Components: demo

Source: "demo\bmp_source\*.gif";               DestDir: "{app}\wx\demo\bmp_source"; Components: demo
Source: "demo\bmp_source\*.bmp";               DestDir: "{app}\wx\demo\bmp_source"; Components: demo
Source: "demo\bmp_source\*.jpg";               DestDir: "{app}\wx\demo\bmp_source"; Components: demo
Source: "demo\bmp_source\*.png";               DestDir: "{app}\wx\demo\bmp_source"; Components: demo
Source: "demo\bmp_source\*.ico";               DestDir: "{app}\wx\demo\bmp_source"; Components: demo

Source: "demo\data\*.htm";                  DestDir: "{app}\wx\demo\data"; Components: demo
Source: "demo\data\*.html";                 DestDir: "{app}\wx\demo\data"; Components: demo
Source: "demo\data\*.py";                   DestDir: "{app}\wx\demo\data"; Components: demo
Source: "demo\data\*.png";                  DestDir: "{app}\wx\demo\data"; Components: demo
Source: "demo\data\*.bmp";                  DestDir: "{app}\wx\demo\data"; Components: demo
Source: "demo\data\*.dat";                  DestDir: "{app}\wx\demo\data"; Components: demo
Source: "demo\data\*.txt";                  DestDir: "{app}\wx\demo\data"; Components: demo
Source: "demo\data\*.wav";                  DestDir: "{app}\wx\demo\data"; Components: demo
Source: "demo\data\*.wdr";                  DestDir: "{app}\wx\demo\data"; Components: demo
Source: "demo\data\*.xrc";                  DestDir: "{app}\wx\demo\data"; Components: demo
Source: "demo\data\*.gif";                  DestDir: "{app}\wx\demo\data"; Components: demo
Source: "demo\data\*.swf";                  DestDir: "{app}\wx\demo\data"; Components: demo

;;Source: "demo\dllwidget\*.cpp";             DestDir: "{app}\wx\demo\dllwidget"; Components: demo
;;Source: "demo\dllwidget\*.py";              DestDir: "{app}\wx\demo\dllwidget"; Components: demo
;;Source: "demo\dllwidget\Makefile";          DestDir: "{app}\wx\demo\dllwidget"; Components: demo
;;Source: "demo\dllwidget\makefile.*";        DestDir: "{app}\wx\demo\dllwidget"; Components: demo


Source: "licence\*.txt";                    DestDir: "{app}\wx\docs\licence"; Components: core
Source: "%(WXDIR)s\docs\htmlhelp\wx.chm";   DestDir: "{app}\wx\docs"; Components: docs
Source: "%(WXDIR)s\docs\htmlhelp\ogl.chm";  DestDir: "{app}\wx\docs"; Components: docs
Source: "docs\README.txt";                  DestDir: "{app}\wx\docs";  Flags: isreadme; Components: core
Source: "docs\*.txt";                       DestDir: "{app}\wx\docs"; Components: docs
Source: "docs\*.css";                       DestDir: "{app}\wx\docs"; Components: docs
Source: "docs\*.html";                      DestDir: "{app}\wx\docs"; Components: docs
Source: "docs\*.conf";                      DestDir: "{app}\wx\docs"; Components: docs
Source: "docs\screenshots\*.png";           DestDir: "{app}\wx\docs\screenshots"; Components: docs
Source: "docs\xml\*.xml";                   DestDir: "{app}\wx\docs\xml"; Components: docs


Source: "scripts\*.bat";                    DestDir: "{code:GetPythonDir}\Scripts"; Components: core
Source: "scripts\*.py";                     DestDir: "{code:GetPythonDir}\Scripts"; Components: core
Source: "scripts\helpviewer";               DestDir: "{code:GetPythonDir}\Scripts"; Components: core
Source: "scripts\img2png";                  DestDir: "{code:GetPythonDir}\Scripts"; Components: core
Source: "scripts\img2py";                   DestDir: "{code:GetPythonDir}\Scripts"; Components: core
Source: "scripts\img2xpm";                  DestDir: "{code:GetPythonDir}\Scripts"; Components: core
Source: "scripts\pyalacarte";               DestDir: "{code:GetPythonDir}\Scripts"; Components: core
Source: "scripts\pyalamode";                DestDir: "{code:GetPythonDir}\Scripts"; Components: core
Source: "scripts\pyshell";                  DestDir: "{code:GetPythonDir}\Scripts"; Components: core
Source: "scripts\pycrust";                  DestDir: "{code:GetPythonDir}\Scripts"; Components: core
Source: "scripts\pywrap";                   DestDir: "{code:GetPythonDir}\Scripts"; Components: core
Source: "scripts\xrced";                    DestDir: "{code:GetPythonDir}\Scripts"; Components: core
Source: "%(WXDIR)s\lib\vc_dll\wxrc.exe";    DestDir: "{code:GetPythonDir}\Scripts"; Components: core

Source: "samples\doodle\*.py";              DestDir: "{app}\wx\samples\doodle"; Components: samples
Source: "samples\doodle\*.txt";             DestDir: "{app}\wx\samples\doodle"; Components: samples
Source: "samples\doodle\*.bat";             DestDir: "{app}\wx\samples\doodle"; Components: samples
Source: "samples\doodle\sample.ddl";        DestDir: "{app}\wx\samples\doodle"; Components: samples
Source: "samples\doodle\superdoodle.iss";   DestDir: "{app}\wx\samples\doodle"; Components: samples

Source: "samples\embedded\*.py";            DestDir: "{app}\wx\samples\embedded"; Components: samples
Source: "samples\embedded\*.cpp";           DestDir: "{app}\wx\samples\embedded"; Components: samples
Source: "samples\embedded\*.txt";           DestDir: "{app}\wx\samples\embedded"; Components: samples
Source: "samples\embedded\*.vc";            DestDir: "{app}\wx\samples\embedded"; Components: samples
Source: "samples\embedded\*.unx";           DestDir: "{app}\wx\samples\embedded"; Components: samples
Source: "samples\embedded\*.ico";           DestDir: "{app}\wx\samples\embedded"; Components: samples
Source: "samples\embedded\*.xpm";           DestDir: "{app}\wx\samples\embedded"; Components: samples
Source: "samples\embedded\*.rc";            DestDir: "{app}\wx\samples\embedded"; Components: samples

Source: "samples\frogedit\*.py";            DestDir: "{app}\wx\samples\frogedit"; Components: samples

Source: "samples\hangman\*.py";             DestDir: "{app}\wx\samples\hangman"; Components: samples

Source: "samples\pySketch\*.py";            DestDir: "{app}\wx\samples\pySketch"; Components: samples
Source: "samples\pySketch\images\*.bmp";    DestDir: "{app}\wx\samples\pySketch\images"; Components: samples

Source: "samples\simple\*.py";              DestDir: "{app}\wx\samples\simple"; Components: samples

Source: "samples\StyleEditor\*.txt";        DestDir: "{app}\wx\samples\StyleEditor"; Components: samples
Source: "samples\StyleEditor\*.py";         DestDir: "{app}\wx\samples\StyleEditor"; Components: samples
Source: "samples\StyleEditor\*.cfg";        DestDir: "{app}\wx\samples\StyleEditor"; Components: samples

Source: "samples\wxProject\*.txt";          DestDir: "{app}\wx\samples\wxProject"; Components: samples
Source: "samples\wxProject\*.py";           DestDir: "{app}\wx\samples\wxProject"; Components: samples


Source: "src\winxp.manifest";               DestDir: "{code:GetPythonDir}"; DestName: "python.exe.manifest"; Components:  manifest
Source: "src\winxp.manifest";               DestDir: "{code:GetPythonDir}"; DestName: "pythonw.exe.manifest"; Components: manifest


;;------------------------------------------------------------

[Run]
;; Compile the .py files
Filename: "{code:GetPythonDir}\python.exe";  Parameters: "{code:GetPythonDir}\Lib\compileall.py {app}\wxPython {app}\wx"; Description: "Compile Python .py files to .pyc"; Flags: postinstall; Components: core

;; Recreate the tool scripts to use the paths on the users machine
Filename: "{code:GetPythonDir}\python.exe";  Parameters: "CreateBatchFiles.py";  WorkingDir: "{code:GetPythonDir}\Scripts";  Description: "Create batch files for tool scripts"; Flags: postinstall; Components: core


;;------------------------------------------------------------

[Icons]
Name: "{group}\Run the wxPython DEMO"; Filename: "{code:GetPythonDir}\pythonw.exe";   WorkingDir: "{app}\wx\demo";   Parameters: "demo.py";                                 IconFilename: "{app}\wx\demo\wxpdemo.ico"; Components: core
Name: "{group}\PyCrust";               Filename: "{code:GetPythonDir}\pythonw.exe";   WorkingDir: "c:\";             Parameters: "{code:GetPythonDir}\Scripts\pycrust";     IconFilename: "{app}\wx\py\PyCrust.ico"; Components: core
Name: "{group}\PyShell";               Filename: "{code:GetPythonDir}\pythonw.exe";   WorkingDir: "c:\";             Parameters: "{code:GetPythonDir}\Scripts\pyshell";     IconFilename: "{app}\wx\py\PyCrust.ico"; Components: core
Name: "{group}\PyAlaMode";             Filename: "{code:GetPythonDir}\pythonw.exe";   WorkingDir: "c:\";             Parameters: "{code:GetPythonDir}\Scripts\pyalamode";   IconFilename: "{app}\wx\py\PyCrust.ico"; Components: core
Name: "{group}\PyAlaCarte";            Filename: "{code:GetPythonDir}\pythonw.exe";   WorkingDir: "c:\";             Parameters: "{code:GetPythonDir}\Scripts\pyalacarte";  IconFilename: "{app}\wx\py\PyCrust.ico"; Components: core
Name: "{group}\Resource Editor";       Filename: "{code:GetPythonDir}\pythonw.exe";   WorkingDir: "c:\";             Parameters: "{code:GetPythonDir}\Scripts\xrced";       IconFilename: "{app}\wx\tools\XRCed\xrced.ico"; Components: core

Name: "{group}\Sample Apps";           Filename: "{app}\wx\samples"; Components: samples

Name: "{group}\wxWindows Reference";   Filename: "{app}\wx\docs\wx.chm";                Components: docs
Name: "{group}\Migration Guide";       Filename: "{app}\wx\docs\MigrationGuide.html";   Components: docs
Name: "{group}\Recent Changes";        Filename: "{app}\wx\docs\CHANGES.html";          Components: docs
Name: "{group}\Other Docs";            Filename: "{app}\wx\docs";                       Components: docs


;;------------------------------------------------------------

[UninstallDelete]
Type: files; Name: "{app}\wx\*.pyc";
Type: files; Name: "{app}\wx\*.pyo";
Type: files; Name: "{app}\wx\*.pyd";
Type: files; Name: "{app}\wx\build\*.pyc";
Type: files; Name: "{app}\wx\build\*.pyo";
Type: files; Name: "{app}\wx\lib\*.pyc";
Type: files; Name: "{app}\wx\lib\*.pyo";
Type: files; Name: "{app}\wx\lib\colourchooser\*.pyc";
Type: files; Name: "{app}\wx\lib\colourchooser\*.pyo";
Type: files; Name: "{app}\wx\lib\editor\*.pyc";
Type: files; Name: "{app}\wx\lib\editor\*.pyo";
Type: files; Name: "{app}\wx\lib\mixins\*.pyc";
Type: files; Name: "{app}\wx\lib\mixins\*.pyo";
Type: files; Name: "{app}\wx\lib\masked\*.pyc";
Type: files; Name: "{app}\wx\lib\masked\*.pyo";
Type: files; Name: "{app}\wx\lib\ogl\*.pyc";
Type: files; Name: "{app}\wx\lib\ogl\*.pyo";
Type: files; Name: "{app}\wx\lib\floatcanvas\*.pyc";
Type: files; Name: "{app}\wx\lib\floatcanvas\*.pyo";

Type: files; Name: "{app}\wx\py\*.pyc";
Type: files; Name: "{app}\wx\py\*.pyo";
Type: files; Name: "{app}\wx\py\tests\*.pyc";
Type: files; Name: "{app}\wx\py\tests\*.pyo";
Type: files; Name: "{app}\wx\tools\*.pyc";
Type: files; Name: "{app}\wx\tools\*.pyo";
Type: files; Name: "{app}\wx\tools\XRCed\*.pyc";
Type: files; Name: "{app}\wx\tools\XRCed\*.pyo";

Type: files; Name: "{app}\wx\demo\*.pyc";
Type: files; Name: "{app}\wx\demo\*.pyo";
Type: files; Name: "{app}\wx\demo\data\showTips";
Type: files; Name: "{app}\wx\demo\data\*.pyc";
Type: files; Name: "{app}\wx\demo\data\*.pyo";
Type: files; Name: "{app}\wx\demo\dllwidget\*.pyc";
Type: files; Name: "{app}\wx\demo\dllwidget\*.pyo";

Type: files; Name: "{app}\wx\samples\doodle\*.pyc";
Type: files; Name: "{app}\wx\samples\doodle\*.pyo";
Type: files; Name: "{app}\wx\samples\embedded\*.pyc";
Type: files; Name: "{app}\wx\samples\embedded\*.pyo";
Type: files; Name: "{app}\wx\samples\frogedit\*.pyc";
Type: files; Name: "{app}\wx\samples\frogedit\*.pyo";
Type: files; Name: "{app}\wx\samples\hangman\*.pyc";
Type: files; Name: "{app}\wx\samples\hangman\*.pyo";
Type: files; Name: "{app}\wx\samples\hangman\*.txt";
Type: files; Name: "{app}\wx\samples\pySketch\*.pyc";
Type: files; Name: "{app}\wx\samples\pySketch\*.pyo";
Type: files; Name: "{app}\wx\samples\simple\*.pyc";
Type: files; Name: "{app}\wx\samples\simple\*.pyo";
Type: files; Name: "{app}\wx\samples\StyleEditor\*.pyc";
Type: files; Name: "{app}\wx\samples\StyleEditor\*.pyo";
Type: files; Name: "{app}\wx\samples\wx_examples\basic\*.pyc";
Type: files; Name: "{app}\wx\samples\wx_examples\basic\*.pyo";
Type: files; Name: "{app}\wx\samples\wx_examples\hello\*.pyc";
Type: files; Name: "{app}\wx\samples\wx_examples\hello\*.pyo";
Type: files; Name: "{app}\wx\samples\wxProject\*.pyc";
Type: files; Name: "{app}\wx\samples\wxProject\*.pyo";

Type: files; Name: "{app}\wxPython\*.pyc";
Type: files; Name: "{app}\wxPython\*.pyo";
Type: files; Name: "{app}\wxPython\lib\*.pyc";
Type: files; Name: "{app}\wxPython\lib\*.pyo";
Type: files; Name: "{app}\wxPython\lib\colourchooser\*.pyc";
Type: files; Name: "{app}\wxPython\lib\colourchooser\*.pyo";
Type: files; Name: "{app}\wxPython\lib\editor\*.pyc";
Type: files; Name: "{app}\wxPython\lib\editor\*.pyo";
Type: files; Name: "{app}\wxPython\lib\mixins\*.pyc";
Type: files; Name: "{app}\wxPython\lib\mixins\*.pyo";
Type: files; Name: "{app}\wxPython\tools\*.pyc";
Type: files; Name: "{app}\wxPython\tools\*.pyo";


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
            InstExec(FileName, '/SILENT', WizardDirValue()+'\wxPython', True, False, SW_SHOWNORMAL, ResultCode);

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
        UninstallOld(WizardDirValue() + '\wx\unins000.exe');
end;


begin
end.

"""

#----------------------------------------------------------------------

def find_DLLs():

    WXDLLVER = PYTHONVER = None

    proc = os.popen(r"dumpbin /imports wx\_core_.pyd", "r")
    lines = proc.readlines()
    proc.close()
    for line in lines:
        if line.startswith("    wxbase"):
            WXDLLVER = line[10:15]
            if WXDLLVER.endswith('_'):
                WXDLLVER = WXDLLVER[:-1]

        if line.startswith("    python"):
            PYTHONVER = line[10] + '.' + line[11]

        #if WXDLLVER and PYTHONVER:
        #    return WXDLLVER, PYTHONVER

    return WXDLLVER, PYTHONVER


#----------------------------------------------------------------------

locale_template = 'Source: "%s";  DestDir: "{app}\%s"; Components: core'

def build_locale_string():
    stringlst = []

    def walk_helper(lst, dirname, files):
        for f in files:
            filename = os.path.join(dirname, f)
            if not os.path.isdir(filename):
                lst.append( locale_template % (filename, dirname) )

    os.path.walk('wx\\locale', walk_helper, stringlst)
    return '\n'.join(stringlst)

def get_system_dir():
    for p in [r"C:\WINNT\SYSTEM32",
              r"C:\WINDOWS\SYSTEM32",
              ]:
        if os.path.exists(p):
            return p
    raise IOError, "System dir not found"


#----------------------------------------------------------------------

def main():

    verglob = {}
    execfile("wx/__version__.py", verglob)

    VERSION    = verglob["VERSION_STRING"]
    SHORTVER   = VERSION[:3]

    WXDLLVER, PYTHONVER = find_DLLs()

    PYVER      = "Py" + PYTHONVER[0] + PYTHONVER[2]
    WXDIR      = os.environ["WXWIN"]
    WXPYDIR    = os.path.join(WXDIR, "wxPython")
    SYSDIR     = get_system_dir()
    ISSFILE    = "__wxPython.iss"
    IFSFILE    = "__wxPython.ifs"
    LOCALE     = build_locale_string()

    print """Building Win32 installer for wxPython:
    VERSION    = %(VERSION)s
    SHORTVER   = %(SHORTVER)s
    WXDLLVER   = %(WXDLLVER)s
    PYTHONVER  = %(PYTHONVER)s
    PYVER      = %(PYVER)s
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
    if len(sys.argv) > 1 and sys.argv[1] == "UNICODE=1":
        MSLU=r'Source: "%(WXDIR)s\lib\vc_dll\unicows.dll";  DestDir: "{code:GetPythonDir}"; Components: core' % vars()

    f = open(ISSFILE, "w")
    f.write(ISS_Template % vars())
    f.close()

    f = open(IFSFILE, "w")
    f.write(IFS_Template % vars())
    f.close()

    TOOLS = os.environ['TOOLS']
    if TOOLS.startswith('/cygdrive'):
        TOOLS = r"c:\TOOLS"  # temporary hack until I convert everything over to bash
    os.system(ISCC % (TOOLS, ISSFILE))

    if not KEEP_TEMPS:
        time.sleep(1)
        os.remove(ISSFILE)
        os.remove(IFSFILE)


#----------------------------------------------------------------------

if __name__ == "__main__":
    main()



#----------------------------------------------------------------------


