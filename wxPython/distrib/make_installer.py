"""
This script will generate a setup script for InnoSetup and then run it
to make the installer executable.  If all goes right the proper versions
of Python and wxWindows (including hybrid/final settings) will all be
calculated based on what wxc.pyd imports and an appropriate installer
will be created.
"""


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
AppCopyright = Copyright © 2003 Total Control Software
DefaultDirName = {code:GetInstallDir|c:\DoNotInstallHere}
DefaultGroupName = wxPython %(SHORTVER)s for Python %(PYTHONVER)s
AlwaysCreateUninstallIcon = yes

;; SourceDir = .
;; OutputDir is relative to SourceDir
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

UninstallFilesDir = {app}\wxPython
AppPublisherURL = http://wxPython.org/
LicenseFile = licence\licence.txt
CodeFile = %(IFSFILE)s

;; WizardDebug = yes

;;------------------------------------------------------------

;;[Dirs]
;;Name: "{app}\"

;;------------------------------------------------------------

[Components]
Name: core;    Description: "wxPython modules and library";  Types: full compact custom;  Flags: fixed
Name: docs;    Description: "Documentation";                 Types: full
Name: demo;    Description: "Demo application";              Types: full
Name: samples; Description: "Sample applications";           Types: full
Name: tools;   Description: "Tools";                         Types: full


;;------------------------------------------------------------

[Files]
Source: "%(SYSDIR)s\MSVCRT.dll";            DestDir: "{sys}"; CopyMode: alwaysskipifsameorolder; Flags: sharedfile uninsneveruninstall restartreplace; Components: core
Source: "%(SYSDIR)s\MSVCIRT.dll";           DestDir: "{sys}"; CopyMode: alwaysskipifsameorolder; Flags: sharedfile uninsneveruninstall restartreplace; Components: core
Source: "%(SYSDIR)s\MSVCP60.dll";           DestDir: "{sys}"; CopyMode: alwaysskipifsameorolder; Flags: sharedfile uninsneveruninstall restartreplace; Components: core

Source: "%(WXDIR)s\lib\%(WXDLL)s";          DestDir: "{app}\wxPython"; Components: core
%(MSLU)s
Source: "wxPython\wxc.pyd";                 DestDir: "{app}\wxPython"; Components: core
Source: "wxPython\gridc.pyd";               DestDir: "{app}\wxPython"; Components: core
Source: "wxPython\helpc.pyd";               DestDir: "{app}\wxPython"; Components: core
Source: "wxPython\htmlc.pyd";               DestDir: "{app}\wxPython"; Components: core
Source: "wxPython\calendarc.pyd";           DestDir: "{app}\wxPython"; Components: core
Source: "wxPython\wizardc.pyd";             DestDir: "{app}\wxPython"; Components: core
Source: "wxPython\glcanvasc.pyd";           DestDir: "{app}\wxPython"; Components: core
Source: "wxPython\oglc.pyd";                DestDir: "{app}\wxPython"; Components: core
Source: "wxPython\stc_c.pyd";               DestDir: "{app}\wxPython"; Components: core
Source: "wxPython\xrcc.pyd";                DestDir: "{app}\wxPython"; Components: core
Source: "wxPython\gizmosc.pyd";             DestDir: "{app}\wxPython"; Components: core
Source: "wxPython\iewinc.pyd";              DestDir: "{app}\wxPython"; Components: core
Source: "wxPython\dllwidget_c.pyd";         DestDir: "{app}\wxPython"; Components: core

Source: "wxPython\*.py";                    DestDir: "{app}\wxPython"; Components: core
Source: "wxPython\lib\*.py";                DestDir: "{app}\wxPython\lib"; Components: core
Source: "wxPython\lib\*.wdr";               DestDir: "{app}\wxPython\lib"; Components: core
Source: "wxPython\lib\editor\*.py";         DestDir: "{app}\wxPython\lib\editor"; Components: core
Source: "wxPython\lib\editor\*.txt";        DestDir: "{app}\wxPython\lib\editor"; Components: core
Source: "wxPython\lib\mixins\*.py";         DestDir: "{app}\wxPython\lib\mixins"; Components: core
Source: "wxPython\lib\PyCrust\*.py";        DestDir: "{app}\wxPython\lib\PyCrust"; Components: core
Source: "wxPython\lib\PyCrust\*.txt";       DestDir: "{app}\wxPython\lib\PyCrust"; Components: core
Source: "wxPython\lib\PyCrust\*.ico";       DestDir: "{app}\wxPython\lib\PyCrust"; Components: core
Source: "wxPython\lib\PyCrust\wxd\*.py";    DestDir: "{app}\wxPython\lib\PyCrust\wxd"; Components: core
Source: "wxPython\lib\colourchooser\*.py";  DestDir: "{app}\wxPython\lib\colourchooser"; Components: core

%(LOCALE)s

Source: "demo\*.py";                        DestDir: "{app}\wxPython\demo"; Components: demo
Source: "demo\*.xml";                       DestDir: "{app}\wxPython\demo"; Components: demo
Source: "demo\*.txt";                       DestDir: "{app}\wxPython\demo"; Components: demo
Source: "demo\*.ico";                       DestDir: "{app}\wxPython\demo"; Components: demo
Source: "demo\*.wdr";                       DestDir: "{app}\wxPython\demo"; Components: demo

Source: "demo\bitmaps\*.gif";               DestDir: "{app}\wxPython\demo\bitmaps"; Components: demo
Source: "demo\bitmaps\*.bmp";               DestDir: "{app}\wxPython\demo\bitmaps"; Components: demo
Source: "demo\bitmaps\*.jpg";               DestDir: "{app}\wxPython\demo\bitmaps"; Components: demo
Source: "demo\bitmaps\*.png";               DestDir: "{app}\wxPython\demo\bitmaps"; Components: demo
Source: "demo\bitmaps\*.ico";               DestDir: "{app}\wxPython\demo\bitmaps"; Components: demo

Source: "demo\bmp_source\*.gif";               DestDir: "{app}\wxPython\demo\bmp_source"; Components: demo
Source: "demo\bmp_source\*.bmp";               DestDir: "{app}\wxPython\demo\bmp_source"; Components: demo
;;Source: "demo\bmp_source\*.jpg";               DestDir: "{app}\wxPython\demo\bmp_source"; Components: demo
Source: "demo\bmp_source\*.png";               DestDir: "{app}\wxPython\demo\bmp_source"; Components: demo
Source: "demo\bmp_source\*.ico";               DestDir: "{app}\wxPython\demo\bmp_source"; Components: demo

Source: "demo\data\*.htm";                  DestDir: "{app}\wxPython\demo\data"; Components: demo
Source: "demo\data\*.html";                 DestDir: "{app}\wxPython\demo\data"; Components: demo
Source: "demo\data\*.py";                   DestDir: "{app}\wxPython\demo\data"; Components: demo
Source: "demo\data\*.png";                  DestDir: "{app}\wxPython\demo\data"; Components: demo
Source: "demo\data\*.bmp";                  DestDir: "{app}\wxPython\demo\data"; Components: demo
Source: "demo\data\*.i";                    DestDir: "{app}\wxPython\demo\data"; Components: demo
;;Source: "demo\data\*.h";                    DestDir: "{app}\wxPython\demo\data"; Components: demo
Source: "demo\data\*.txt";                  DestDir: "{app}\wxPython\demo\data"; Components: demo
Source: "demo\data\*.wav";                  DestDir: "{app}\wxPython\demo\data"; Components: demo
Source: "demo\data\*.wdr";                  DestDir: "{app}\wxPython\demo\data"; Components: demo
Source: "demo\data\*.xrc";                  DestDir: "{app}\wxPython\demo\data"; Components: demo
Source: "demo\data\*.gif";                  DestDir: "{app}\wxPython\demo\data"; Components: demo

Source: "demo\dllwidget\*.cpp";             DestDir: "{app}\wxPython\demo\dllwidget"; Components: demo
Source: "demo\dllwidget\*.py";              DestDir: "{app}\wxPython\demo\dllwidget"; Components: demo
Source: "demo\dllwidget\Makefile";          DestDir: "{app}\wxPython\demo\dllwidget"; Components: demo
Source: "demo\dllwidget\makefile.*";        DestDir: "{app}\wxPython\demo\dllwidget"; Components: demo

Source: "README.txt";                       DestDir: "{app}\wxPython\docs";  Flags: isreadme; Components: core
Source: "CHANGES.txt";                      DestDir: "{app}\wxPython\docs"; Components: core
Source: "licence\*.txt";                    DestDir: "{app}\wxPython\docs\licence"; Components: core
Source: "%(WXDIR)s\docs\htmlhelp\wx.chm";   DestDir: "{app}\wxPython\docs"; Components: docs
Source: "%(WXDIR)s\docs\htmlhelp\ogl.chm";  DestDir: "{app}\wxPython\docs"; Components: docs

Source: "wxPython\tools\*.py";              DestDir: "{app}\wxPython\tools"; Components: tools
Source: "wxPython\tools\XRCed\*.txt";       DestDir: "{app}\wxPython\tools\XRCed"; Components: tools
Source: "wxPython\tools\XRCed\sawfishrc";   DestDir: "{app}\wxPython\tools\XRCed"; Components: tools
Source: "wxPython\tools\XRCed\*.py";        DestDir: "{app}\wxPython\tools\XRCed"; Components: tools
Source: "wxPython\tools\XRCed\*.xrc";       DestDir: "{app}\wxPython\tools\XRCed"; Components: tools
Source: "wxPython\tools\XRCed\*.ico";       DestDir: "{app}\wxPython\tools\XRCed"; Components: tools
Source: "wxPython\tools\XRCed\*.sh";        DestDir: "{app}\wxPython\tools\XRCed"; Components: tools

Source: "scripts\*.bat";                    DestDir: "{code:GetPythonDir}\Scripts"; Components: tools
Source: "scripts\*.py";                     DestDir: "{code:GetPythonDir}\Scripts"; Components: tools
Source: "scripts\helpviewer";               DestDir: "{code:GetPythonDir}\Scripts"; Components: tools
Source: "scripts\img2png";                  DestDir: "{code:GetPythonDir}\Scripts"; Components: tools
Source: "scripts\img2py";                   DestDir: "{code:GetPythonDir}\Scripts"; Components: tools
Source: "scripts\img2xpm";                  DestDir: "{code:GetPythonDir}\Scripts"; Components: tools
Source: "scripts\xrced";                    DestDir: "{code:GetPythonDir}\Scripts"; Components: tools
Source: "scripts\pyshell";                  DestDir: "{code:GetPythonDir}\Scripts"; Components: tools
Source: "scripts\pycrust";                  DestDir: "{code:GetPythonDir}\Scripts"; Components: tools
Source: "scripts\pycwrap";                  DestDir: "{code:GetPythonDir}\Scripts"; Components: tools

Source: "samples\doodle\*.py";              DestDir: "{app}\wxPython\samples\doodle"; Components: samples
Source: "samples\doodle\*.txt";             DestDir: "{app}\wxPython\samples\doodle"; Components: samples
Source: "samples\doodle\sample.ddl";        DestDir: "{app}\wxPython\samples\doodle"; Components: samples
Source: "samples\doodle\superdoodle.iss";   DestDir: "{app}\wxPython\samples\doodle"; Components: samples

Source: "samples\wxProject\*.txt";          DestDir: "{app}\wxPython\samples\wxProject"; Components: samples
Source: "samples\wxProject\*.py";           DestDir: "{app}\wxPython\samples\wxProject"; Components: samples

Source: "samples\StyleEditor\*.txt";           DestDir: "{app}\wxPython\samples\StyleEditor"; Components: samples
Source: "samples\StyleEditor\*.py";            DestDir: "{app}\wxPython\samples\StyleEditor"; Components: samples
Source: "samples\StyleEditor\*.cfg";           DestDir: "{app}\wxPython\samples\StyleEditor"; Components: samples

Source: "samples\pySketch\*.py";           DestDir: "{app}\wxPython\samples\pySketch"; Components: samples
Source: "samples\pySketch\images\*.bmp";   DestDir: "{app}\wxPython\samples\pySketch\images"; Components: samples

Source: "samples\frogedit\*.py";           DestDir: "{app}\wxPython\samples\frogedit"; Components: samples

Source: "samples\embedded\*.py";           DestDir: "{app}\wxPython\samples\embedded"; Components: samples
Source: "samples\embedded\*.cpp";          DestDir: "{app}\wxPython\samples\embedded"; Components: samples
Source: "samples\embedded\*.txt";          DestDir: "{app}\wxPython\samples\embedded"; Components: samples
Source: "samples\embedded\*.vc";           DestDir: "{app}\wxPython\samples\embedded"; Components: samples
Source: "samples\embedded\*.unx";          DestDir: "{app}\wxPython\samples\embedded"; Components: samples
Source: "samples\embedded\*.ico";          DestDir: "{app}\wxPython\samples\embedded"; Components: samples
Source: "samples\embedded\*.xpm";          DestDir: "{app}\wxPython\samples\embedded"; Components: samples

Source: "src\winxp.manifest";              DestDir: "{code:GetPythonDir}"; DestName: "python.exe.manifest"; Components: core
Source: "src\winxp.manifest";              DestDir: "{code:GetPythonDir}"; DestName: "pythonw.exe.manifest"; Components: core


;;------------------------------------------------------------

[Run]
;; Recreate the tool scripts to use the paths on the users machine
Filename: "{code:GetPythonDir}\python.exe";  Parameters: "{code:GetPythonDir}\Lib\compileall.py {app}\wxPython"; Description: "Compile Python .py files to .pyc"; Flags: postinstall; Components: core
Filename: "{code:GetPythonDir}\python.exe";  Parameters: "CreateBatchFiles.py";  WorkingDir: "{code:GetPythonDir}\Scripts";  Description: "Create batch files for tool scripts"; Flags: postinstall; Components: tools


;;------------------------------------------------------------

[Icons]
Name: "{group}\Run the DEMO";          Filename: "{code:GetPythonDir}\pythonw.exe";   WorkingDir: "{app}\wxPython\demo";   Parameters: "demo.py"; IconFilename: "{app}\wxPython\demo\wxpdemo.ico"; Components: core
Name: "{group}\PyCrust";               Filename: "{code:GetPythonDir}\pythonw.exe";   WorkingDir: "c:\";                   Parameters: "{code:GetPythonDir}\Scripts\pycrust";  IconFilename: "{app}\wxPython\lib\PyCrust\PyCrust.ico"; Components: core
Name: "{group}\PyShell";               Filename: "{code:GetPythonDir}\pythonw.exe";   WorkingDir: "c:\";                   Parameters: "{code:GetPythonDir}\Scripts\pyshell";  IconFilename: "{app}\wxPython\lib\PyCrust\PyCrust.ico"; Components: core
Name: "{group}\wxWindows Reference";   Filename: "{app}\wxPython\docs\wx.chm";                Components: docs
Name: "{group}\wxOGL Reference";       Filename: "{app}\wxPython\docs\ogl.chm";               Components: docs
Name: "{group}\licence.txt";           Filename: "{app}\wxPython\docs\licence\licence.txt";   Components: core
Name: "{group}\README.txt";            Filename: "{app}\wxPython\docs\README.txt";            Components: core
Name: "{group}\CHANGES.txt";           Filename: "{app}\wxPython\docs\CHANGES.txt";           Components: core
Name: "{group}\Sample Apps";           Filename: "{app}\wxPython\samples"; Components: samples
Name: "{group}\Resource Editor";       Filename: "{code:GetPythonDir}\pythonw.exe";   WorkingDir: "c:\";   Parameters: "{code:GetPythonDir}\Scripts\xrced"; IconFilename: "{app}\wxPython\Tools\XRCed\xrced.ico"; Components: tools


;;------------------------------------------------------------

[UninstallDelete]
Type: files; Name: "{app}\wxPython\*.pyc";
Type: files; Name: "{app}\wxPython\*.pyo";
Type: files; Name: "{app}\wxPython\*.pyd";
Type: files; Name: "{app}\wxPython\lib\*.pyc";
Type: files; Name: "{app}\wxPython\lib\*.pyo";
Type: files; Name: "{app}\wxPython\lib\editor\*.pyc";
Type: files; Name: "{app}\wxPython\lib\editor\*.pyo";
Type: files; Name: "{app}\wxPython\lib\mixins\*.pyc";
Type: files; Name: "{app}\wxPython\lib\mixins\*.pyo";
Type: files; Name: "{app}\wxPython\lib\PyCrust\*.pyc";
Type: files; Name: "{app}\wxPython\lib\PyCrust\*.pyo";
Type: files; Name: "{app}\wxPython\lib\PyCrust\decor\*.pyc";
Type: files; Name: "{app}\wxPython\lib\PyCrust\decor\*.pyo";
Type: files; Name: "{app}\wxPython\lib\PyCrust\wxd\*.pyc";
Type: files; Name: "{app}\wxPython\lib\PyCrust\wxd\*.pyo";
Type: files; Name: "{app}\wxPython\lib\colourchooser\*.pyc";
Type: files; Name: "{app}\wxPython\lib\colourchooser\*.pyo";
Type: files; Name: "{app}\wxPython\tools\*.pyc";
Type: files; Name: "{app}\wxPython\tools\*.pyo";
Type: files; Name: "{app}\wxPython\tools\XRCed\*.pyc";
Type: files; Name: "{app}\wxPython\tools\XRCed\*.pyo";
Type: files; Name: "{app}\wxPython\demo\*.pyc";
Type: files; Name: "{app}\wxPython\demo\*.pyo";
Type: files; Name: "{app}\wxPython\demo\data\showTips";
Type: files; Name: "{app}\wxPython\tools\*.pyc";
Type: files; Name: "{app}\wxPython\tools\*.pyo";
Type: files; Name: "{app}\wxPython\tools\XRCed\*.pyc";
Type: files; Name: "{app}\wxPython\tools\XRCed\*.pyo";
Type: files; Name: "{app}\wxPython\samples\doodle\*.pyc";
Type: files; Name: "{app}\wxPython\samples\doodle\*.pyo";
Type: files; Name: "{app}\wxPython\samples\wxProject\*.pyc";
Type: files; Name: "{app}\wxPython\samples\wxProject\*.pyo";
Type: files; Name: "{app}\wxPython\samples\StyleEditor\*.pyc";
Type: files; Name: "{app}\wxPython\samples\StyleEditor\*.pyo";
Type: files; Name: "{app}\wxPython\samples\frogedit\*.pyc";
Type: files; Name: "{app}\wxPython\samples\frogedit\*.pyo";
Type: files; Name: "{app}\wxPython\demo\data\*.pyc";
Type: files; Name: "{app}\wxPython\demo\data\*.pyo";
Type: files; Name: "{app}\wxPython\demo\dllwidget\*.pyc";
Type: files; Name: "{app}\wxPython\demo\dllwidget\*.pyo";
Type: files; Name: "{app}\wxPython\samples\embedded\*.pyc";
Type: files; Name: "{app}\wxPython\samples\embedded\*.pyo";
Type: files; Name: "{app}\wxPython\samples\pySketch\*.pyc";
Type: files; Name: "{app}\wxPython\samples\pySketch\*.pyo";


'''


#----------------------------------------------------------------------


IFS_Template = r"""
program Setup;
var
    PythonDir : String;
    InstallDir : String;


function InitializeSetup(): Boolean;
begin
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
    Result := true;
end;


function GetPythonDir(Default: String): String;
begin
    Result := PythonDir;
end;

function GetInstallDir(Default: String): String;
begin
    Result := InstallDir;
end;



function NextButtonClick(CurPage: Integer): Boolean;
var
    FileName: string;
    ResultCode: Integer;
begin
    Result := True;
    if CurPage <> wpSelectDir then Exit;
    FileName := WizardDirValue() + '\wxPython\unins000.exe';
    if FileExists(FileName) then begin
        ResultCode := MsgBox('A prior wxPython installation was found in this directory.  It' + #13 +
                             'is recommended that it be uninstalled first.' + #13#13 +
                             'Should I do it?',
                             mbConfirmation, MB_YESNO);
        if ResultCode = IDYES then begin
            InstExec(FileName, '/SILENT', WizardDirValue()+'\wxPython', True, False, SW_SHOWNORMAL, ResultCode);

        end;
    end;
end;


begin
end.

"""

#----------------------------------------------------------------------

def find_DLLs():

    WXDLL = PYTHONVER = None

    proc = os.popen(r"dumpbin /imports wxPython\wxc.pyd", "r")
    lines = proc.readlines()
    proc.close()
    for line in lines:
        if line[:6] == "    wx":
            WXDLL = line.strip()

        if line[:10] == "    python":
            PYTHONVER = line[10] + '.' + line[11]

        if WXDLL and PYTHONVER:
            return WXDLL, PYTHONVER


#----------------------------------------------------------------------

locale_template = 'Source: "%s";  DestDir: "{app}\%s"; Components: core'

def build_locale_string():
    stringlst = []

    def walk_helper(lst, dirname, files):
        for f in files:
            filename = os.path.join(dirname, f)
            if not os.path.isdir(filename):
                lst.append( locale_template % (filename, dirname) )

    os.path.walk('wxPython\\locale', walk_helper, stringlst)
    return '\n'.join(stringlst)


#----------------------------------------------------------------------

def main():

    verglob = {}
    execfile("wxPython/__version__.py", verglob)

    VERSION    = verglob["ver"]
    SHORTVER   = VERSION[:3]

    WXDLL, PYTHONVER = find_DLLs()

    PYVER      = "Py" + PYTHONVER[0] + PYTHONVER[2]
    WXDIR      = os.environ["WXWIN"]
    WXPYDIR    = os.path.join(WXDIR, "wxPython")
    SYSDIR     = r"C:\WINNT\SYSTEM32"
    ISSFILE    = "__wxPython.iss"
    IFSFILE    = "__wxPython.ifs"
    LOCALE     = build_locale_string()

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
        MSLU=r'Source: "%(WXDIR)s\lib\unicows.dll";  DestDir: "{code:GetPythonDir}"; Components: core' % vars()

    f = open(ISSFILE, "w")
    f.write(ISS_Template % vars())
    f.close()

    f = open(IFSFILE, "w")
    f.write(IFS_Template % vars())
    f.close()

    os.system(ISCC % (os.environ['TOOLS'], ISSFILE))

    if not KEEP_TEMPS:
        time.sleep(1)
        os.remove(ISSFILE)
        os.remove(IFSFILE)


#----------------------------------------------------------------------

if __name__ == "__main__":
    main()



#----------------------------------------------------------------------


