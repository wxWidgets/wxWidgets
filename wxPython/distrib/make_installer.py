"""
This script will generate a setup script for InnoSetup and then run it
to make the installer executable.  If all goes right the proper versions
of Python and wxWindows (including hybrid/final settings) will all be
calculated based on what wxc.pyd imports and an appropriate installer
will be created.
"""


import os, string

KEEP_TEMPS = 0

#----------------------------------------------------------------------

ISS_Template = r'''
;;------------------------------------------------------------

[Setup]
AppName = wxPython
AppVerName = wxPython %(VERSION)s for Python %(PYTHONVER)s
OutputBaseFilename = wxPython-%(VERSION)s-%(PYVER)s
AppCopyright = Copyright © 2001 Total Control Software
DefaultDirName = {code:GetInstallDir|c:\DoNotInstallHere}
DefaultGroupName = wxPython %(SHORTVER)s for Python %(PYTHONVER)s
AlwaysCreateUninstallIcon = yes

;; SourceDir = .
;; OutputDir is relative to SourceDir
OutputDir = dist
WizardStyle = modern
UninstallStyle = modern
DisableStartupPrompt = true
CompressLevel = 9
DirExistsWarning = no
DisableReadyMemo = true
DisableReadyPage = true
;;DisableDirPage = true
DisableProgramGroupPage = true
DisableAppendDir = true
UsePreviousAppDir = no
UsePreviousGroup = no

UninstallFilesDir = {app}\wxPython
AppPublisherURL = http://wxPython.org/
LicenseFile = licence\licence.txt
CodeFile = %(IFSFILE)s

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

Source: "%(WXDIR)s\lib\%(WXDLL)s";          DestDir: "{app}\wxPython"; Components: core
Source: "wxPython\wxc.pyd";                 DestDir: "{app}\wxPython"; Components: core
Source: "wxPython\gridc.pyd";               DestDir: "{app}\wxPython"; Components: core
Source: "wxPython\helpc.pyd";               DestDir: "{app}\wxPython"; Components: core
Source: "wxPython\htmlc.pyd";               DestDir: "{app}\wxPython"; Components: core
Source: "wxPython\utilsc.pyd";              DestDir: "{app}\wxPython"; Components: core
Source: "wxPython\calendarc.pyd";           DestDir: "{app}\wxPython"; Components: core
Source: "wxPython\glcanvasc.pyd";           DestDir: "{app}\wxPython"; Components: core
Source: "wxPython\oglc.pyd";                DestDir: "{app}\wxPython"; Components: core
Source: "wxPython\stc_c.pyd";               DestDir: "{app}\wxPython"; Components: core
Source: "wxPython\xrcc.pyd";                DestDir: "{app}\wxPython"; Components: core

Source: "wxPython\*.py";                    DestDir: "{app}\wxPython"; Components: core
Source: "wxPython\lib\*.py";                DestDir: "{app}\wxPython\lib"; Components: core
Source: "wxPython\lib\editor\*.py";         DestDir: "{app}\wxPython\lib\editor"; Components: core
Source: "wxPython\lib\editor\*.txt";        DestDir: "{app}\wxPython\lib\editor"; Components: core
Source: "wxPython\lib\mixins\*.py";         DestDir: "{app}\wxPython\lib\mixins"; Components: core
Source: "wxPython\lib\PyCrust\*.py";        DestDir: "{app}\wxPython\lib\PyCrust"; Components: core
Source: "wxPython\lib\PyCrust\*.txt";       DestDir: "{app}\wxPython\lib\PyCrust"; Components: core
Source: "wxPython\lib\PyCrust\*.ico";       DestDir: "{app}\wxPython\lib\PyCrust"; Components: core

Source: "demo\*.py";                        DestDir: "{app}\wxPython\demo"; Components: demo
Source: "demo\*.xml";                       DestDir: "{app}\wxPython\demo"; Components: demo
Source: "demo\*.txt";                       DestDir: "{app}\wxPython\demo"; Components: demo

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

Source: "README.txt";                       DestDir: "{app}\wxPython\docs";  Flags: isreadme; Components: core
Source: "CHANGES.txt";                      DestDir: "{app}\wxPython\docs"; Components: core
Source: "licence\*.txt";                    DestDir: "{app}\wxPython\docs\licence"; Components: core
Source: "%(WXDIR)s\docs\htmlhelp\wx.chm";   DestDir: "{app}\wxPython\docs"; Components: docs
Source: "%(WXDIR)s\docs\htmlhelp\ogl.chm";  DestDir: "{app}\wxPython\docs"; Components: docs

Source: "tools\*.py";                       DestDir: "{app}\wxPython\tools"; Components: tools
Source: "tools\XRCed\CHANGES";              DestDir: "{app}\wxPython\tools\XRCed"; Components: tools
Source: "tools\XRCed\TODO";                 DestDir: "{app}\wxPython\tools\XRCed"; Components: tools
Source: "tools\XRCed\README";               DestDir: "{app}\wxPython\tools\XRCed"; Components: tools
Source: "tools\XRCed\*.py";                 DestDir: "{app}\wxPython\tools\XRCed"; Components: tools
Source: "tools\XRCed\*.xrc";                DestDir: "{app}\wxPython\tools\XRCed"; Components: tools

Source: "samples\doodle\*.py";              DestDir: "{app}\wxPython\samples\doodle"; Components: samples
Source: "samples\doodle\*.txt";             DestDir: "{app}\wxPython\samples\doodle"; Components: samples
Source: "samples\doodle\sample.ddl";        DestDir: "{app}\wxPython\samples\doodle"; Components: samples
Source: "samples\doodle\superdoodle.iss";   DestDir: "{app}\wxPython\samples\doodle"; Components: samples

Source: "samples\wxProject\*.txt";          DestDir: "{app}\wxPython\samples\wxProject"; Components: samples
Source: "samples\wxProject\*.py";           DestDir: "{app}\wxPython\samples\wxProject"; Components: samples

Source: "samples\stxview\*.py";                  DestDir: "{app}\wxPython\samples\stxview"; Components: samples
Source: "samples\stxview\*.stx";                 DestDir: "{app}\wxPython\samples\stxview"; Components: samples
Source: "samples\stxview\*.txt";                 DestDir: "{app}\wxPython\samples\stxview"; Components: samples
Source: "samples\stxview\StructuredText\*.py";   DestDir: "{app}\wxPython\samples\stxview\StructuredText"; Components: samples
Source: "samples\stxview\StructuredText\*.txt";   DestDir: "{app}\wxPython\samples\stxview\StructuredText"; Components: samples

Source: "samples\StyleEditor\*.txt";           DestDir: "{app}\wxPython\samples\StyleEditor"; Components: samples
Source: "samples\StyleEditor\*.py";            DestDir: "{app}\wxPython\samples\StyleEditor"; Components: samples
Source: "samples\StyleEditor\*.cfg";           DestDir: "{app}\wxPython\samples\StyleEditor"; Components: samples

Source: "samples\pySketch\*.py";           DestDir: "{app}\wxPython\samples\pySketch"; Components: samples
Source: "samples\pySketch\images\*.bmp";   DestDir: "{app}\wxPython\samples\pySketch\images"; Components: samples


;;------------------------------------------------------------

[Icons]
Name: "{group}\Run the DEMO";          Filename: "{code:GetPythonDir}\pythonw.exe";         WorkingDir: "{app}\wxPython\demo";      Parameters: "demo.py";
Name: "{group}\wxWindows Reference";   Filename: "{app}\wxPython\docs\wx.chm";
Name: "{group}\wxOGL Reference";       Filename: "{app}\wxPython\docs\ogl.chm";
Name: "{group}\licence.txt";           Filename: "{app}\wxPython\docs\licence\licence.txt";
Name: "{group}\README.txt";            Filename: "{app}\wxPython\docs\README.txt";
Name: "{group}\CHANGES.txt";           Filename: "{app}\wxPython\docs\CHANGES.txt";
Name: "{group}\Sample Apps";           Filename: "{app}\wxPython\samples"; Components: samples


;;------------------------------------------------------------

[UninstallDelete]
Type: files; Name: "{app}\wxPython\*.pyc";
Type: files; Name: "{app}\wxPython\*.pyo";
Type: files; Name: "{app}\wxPython\lib\*.pyc";
Type: files; Name: "{app}\wxPython\lib\*.pyo";
Type: files; Name: "{app}\wxPython\lib\editor\*.pyc";
Type: files; Name: "{app}\wxPython\lib\editor\*.pyo";
Type: files; Name: "{app}\wxPython\lib\mixins\*.pyc";
Type: files; Name: "{app}\wxPython\lib\mixins\*.pyo";
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
Type: files; Name: "{app}\wxPython\samples\stxview\*.pyc";
Type: files; Name: "{app}\wxPython\samples\stxview\*.pyo";
Type: files; Name: "{app}\wxPython\samples\stxview\StructuredText\*.pyc";
Type: files; Name: "{app}\wxPython\samples\stxview\StructuredText\*.pyo";

'''


#----------------------------------------------------------------------

## TODO:  For Python 2.2 wxPython should go into
#         os.path.join(sys.prefix, 'Lib', 'site-packages')


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

            MsgBox('No installation of Python %(PYTHONVER)s found in registry.\nBe sure to enter a pathname that places wxPython\non the PYTHONPATH',
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
            WXDLL =  string.strip(line)

        if line[:10] == "    python":
            PYTHONVER = line[10] + '.' + line[11]

        if WXDLL and PYTHONVER:
            return WXDLL, PYTHONVER


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

    if PYTHONVER >= "2.2":
        IF22 = r"InstallDir := InstallDir + '\Lib\site-packages';"
    else:
        IF22 = ""

    if string.find(WXDLL, "h") != -1:
        PYVER = PYVER + "-hybrid"

    f = open(ISSFILE, "w")
    f.write(ISS_Template % vars())
    f.close()

    f = open(IFSFILE, "w")
    f.write(IFS_Template % vars())
    f.close()

    os.system(r"C:\TOOLS\InnoSetup2Ex\ISCC.exe %s" % ISSFILE)

    if not KEEP_TEMPS:
        os.remove(ISSFILE)
        os.remove(IFSFILE)


#----------------------------------------------------------------------

if __name__ == "__main__":
    main()



#----------------------------------------------------------------------


