/////////////////////////////////////////////////////////////////////////////
// Name:        makeproj.cpp
// Purpose:     Generate sample VC++ project files
// Author:      Julian Smart
// Modified by:
// Created:     10/12/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "makeproj.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if defined(__BORLANDC__)
#pragma hdrstop
#endif

#include "wx/wx.h"
#include "wx/resource.h"

#include "iostream.h"
#include "fstream.h"

#include "makeproj.h"
#include "projgenrc.h"

// ----------------------------------------------------------------------------
// ressources
// ----------------------------------------------------------------------------
// the application icon
#if defined(__WXGTK__) || defined(__WXMOTIF__)
    #include "mondrian.xpm"
#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();

    bool GenerateSample(const wxString& projectName, const wxString& targetName,
        const wxString& path, const wxStringList& sourceFiles, const wxString& relativeRootPath = "../..");
    void GenerateSamples(const wxString& dir); // Takes wxWindows directory path
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnGenerate(wxCommandEvent& event);

    bool GenerateSample(const wxString& projectName, const wxString& targetName,
        const wxString& path, const wxStringList& sourceFiles, const wxString& relativeRootPath = "../..");

private:
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};

// Define a dialog: this will be our main dialog
class MyDialog : public wxDialog
{
public:
    // ctor(s)
    MyDialog(const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnGenerate(wxCommandEvent& event);
    void OnGenerateSamples(wxCommandEvent& event);

private:
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};


// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    MakeProject_Quit = 1,
    MakeProject_About,
    MakeProject_Generate,
    MakeProject_GenerateSamples,

    // controls start here (the numbers are, of course, arbitrary)
    MakeProject_Text = 1000,
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(MakeProject_Quit,  MyFrame::OnQuit)
    EVT_MENU(MakeProject_About, MyFrame::OnAbout)
    EVT_MENU(MakeProject_Generate, MyFrame::OnGenerate)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWindows to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
#if 0
    // Create the main application window
    MyFrame *frame = new MyFrame("MakeProject wxWindows App",
                                 wxPoint(50, 50), wxSize(450, 340));

    frame->Show(TRUE);
    SetTopWindow(frame);
#endif
    wxResourceParseFile("projgenrc.wxr");

    MyDialog* dialog = new MyDialog("VC++ MakeProject");
    dialog->ShowModal();

    delete dialog;

    return FALSE;
}

bool MyApp::GenerateSample(const wxString& projectName, const wxString& targetName,
    const wxString& path, const wxStringList& sourceFiles, const wxString& relativeRootPath)
{
    wxString relativeIncludePath(relativeRootPath + wxString("/include"));
    wxString relativeLibPath(relativeRootPath + wxString("/lib"));
    wxString relativeDebugPath(relativeRootPath + wxString("/src/Debug"));
    wxString relativeReleasePath(relativeRootPath + wxString("/src/Release"));
    wxString relativeDebugPathJPEG(relativeRootPath + wxString("/src/jpeg/Debug"));
    wxString relativeReleasePathJPEG(relativeRootPath + wxString("/src/jpeg/Release"));
    wxString relativeDebugPathTIFF(relativeRootPath + wxString("/src/tiff/Debug"));
    wxString relativeReleasePathTIFF(relativeRootPath + wxString("/src/tiff/Release"));

    wxProject project;

    // For all samples
    project.SetIncludeDirs(wxStringList((const char*) relativeIncludePath, 0));
    project.SetResourceIncludeDirs(wxStringList((const char*) relativeIncludePath, 0));
    project.SetLibDirs(wxStringList((const char*) relativeLibPath, 0));
    project.SetDebugLibDirs(wxStringList((const char*) relativeDebugPath, (const char*) relativeDebugPathJPEG, (const char*) relativeDebugPathTIFF, 0));
    project.SetReleaseLibDirs(wxStringList((const char*) relativeReleasePath, (const char*) relativeReleasePathJPEG, (const char*) relativeReleasePathTIFF, 0));

    project.SetProjectName(projectName);
    project.SetTargetName(targetName);
    project.SetProjectPath(path);
    project.SetSourceFiles(sourceFiles);

    if (!project.GenerateVCProject())
    {
        wxString msg("Could not generate ");
        msg += projectName;
        wxMessageBox(msg);
        return FALSE;
    }
    return TRUE;
}


void MyApp::GenerateSamples(const wxString& dir)
{
    // Small bug. Because we don't distinguish between Debug/DebugDLL, Release/ReleaseDLL,
    // we can't yet make a sample that uses other wxWindows static libraries + the wxWindows DLL library.

    //// Samples

    GenerateSample("CalendarVC", "calendar", dir + wxString("/samples/calendar"), wxStringList("calendar.cpp", 0));
    GenerateSample("CaretVC", "caret", dir + wxString("/samples/caret"), wxStringList("caret.cpp", 0));
    GenerateSample("CheckLstVC", "checklst", dir + wxString("/samples/checklst"), wxStringList("checklst.cpp", 0));
    GenerateSample("ConfigVC", "conftest", dir + wxString("/samples/config"), wxStringList("conftest.cpp", 0));
    GenerateSample("ControlsVC", "controls", dir + wxString("/samples/controls"), wxStringList("controls.cpp", 0));
    GenerateSample("DbVC", "dbtest", dir + wxString("/samples/db"),
        wxStringList("dbtest.cpp", "listdb.cpp", "dbtest.h", "listdb.h", 0));
    GenerateSample("DialogsVC", "dialogs", dir + wxString("/samples/dialogs"),
        wxStringList("dialogs.cpp", "dialogs.h", 0));
    GenerateSample("DndVC", "dnd", dir + wxString("/samples/dnd"), wxStringList("dnd.cpp", 0));
    GenerateSample("DocViewVC", "docview", dir + wxString("/samples/docview"),
        wxStringList("docview.cpp", "doc.cpp", "view.cpp", "docview.h", "doc.h", "view.h", 0));
    GenerateSample("DocVwMDIVC", "docview", dir + wxString("/samples/docvwmdi"),
        wxStringList("docview.cpp", "doc.cpp", "view.cpp", "docview.h", "doc.h", "view.h", 0));
    GenerateSample("DynamicVC", "dynamic", dir + wxString("/samples/dynamic"), wxStringList("dynamic.cpp", 0));
    GenerateSample("DrawingVC", "drawing", dir + wxString("/samples/drawing"), wxStringList("drawing.cpp", 0));
    GenerateSample("ExecVC", "exec", dir + wxString("/samples/exec"), wxStringList("exec.cpp", 0));
    GenerateSample("GridVC", "test", dir + wxString("/samples/grid"), wxStringList("test.cpp", 0));
    GenerateSample("NewGridVC", "griddemo", dir + wxString("/samples/newgrid"), wxStringList("griddemo.cpp", 0));
    GenerateSample("HelpVC", "demo", dir + wxString("/samples/help"), wxStringList("demo.cpp", 0));

    // wxHTML samples
    GenerateSample("AboutVC", "about", dir + wxString("/samples/html/about"), wxStringList("about.cpp", 0),
        "../../..");
    GenerateSample("HelpVC", "help", dir + wxString("/samples/html/help"), wxStringList("help.cpp", 0),
        "../../..");
    GenerateSample("PrintingVC", "printing", dir + wxString("/samples/html/printing"), wxStringList("printing.cpp", 0),
        "../../..");
    GenerateSample("TestVC", "test", dir + wxString("/samples/html/test"), wxStringList("test.cpp", 0),
        "../../..");
    GenerateSample("VirtualVC", "virtual", dir + wxString("/samples/html/virtual"), wxStringList("virtual.cpp", 0),
        "../../..");
    GenerateSample("WidgetVC", "widget", dir + wxString("/samples/html/widget"), wxStringList("widget.cpp", 0),
        "../../..");
    GenerateSample("ZipVC", "zip", dir + wxString("/samples/html/zip"), wxStringList("zip.cpp", 0),
        "../../..");
    GenerateSample("HelpViewVC", "helpview", dir + wxString("/samples/html/helpview"), wxStringList("helpview.cpp", 0),
        "../../..");

    GenerateSample("ImageVC", "image", dir + wxString("/samples/image"), wxStringList("image.cpp", 0));
    GenerateSample("InternatVC", "internat", dir + wxString("/samples/internat"), wxStringList("internat.cpp", 0));
    GenerateSample("JoytestVC", "joytest", dir + wxString("/samples/joytest"), wxStringList("joytest.cpp", "joytest.h", 0));
    GenerateSample("LayoutVC", "layout", dir + wxString("/samples/layout"), wxStringList("layout.cpp", "layout.h", 0));
    GenerateSample("ListctrlVC", "listtest", dir + wxString("/samples/listctrl"), wxStringList("listtest.cpp", "listtest.h", 0));
    GenerateSample("MdiVC", "mdi", dir + wxString("/samples/mdi"), wxStringList("mdi.cpp", "mdi.h", 0));
    GenerateSample("MemcheckVC", "memcheck", dir + wxString("/samples/memcheck"), wxStringList("memcheck.cpp", 0));
    // Note: MFC sample will be different.
    GenerateSample("MfcVC", "mfc", dir + wxString("/samples/mfc"), wxStringList("mfctest.cpp", "mfctest.h", 0));
    GenerateSample("MiniframVC", "test", dir + wxString("/samples/minifram"), wxStringList("test.cpp", "test.h", 0));
    GenerateSample("MinimalVC", "minimal", dir + wxString("/samples/minimal"), wxStringList("minimal.cpp", 0));
    GenerateSample("NativdlgVC", "nativdlg", dir + wxString("/samples/nativdlg"), wxStringList("nativdlg.cpp", "nativdlg.h", "resource.h", 0));
    GenerateSample("DialupVC", "nettest", dir + wxString("/samples/dialup"), wxStringList("nettest.cpp", 0));
    GenerateSample("NotebookVC", "test", dir + wxString("/samples/notebook"), wxStringList("test.cpp", "test.h", 0));
    GenerateSample("OleautoVC", "oleauto", dir + wxString("/samples/oleauto"), wxStringList("oleauto.cpp", 0));
    GenerateSample("OwnerdrwVC", "ownerdrw", dir + wxString("/samples/ownerdrw"), wxStringList("ownerdrw.cpp", 0));
    GenerateSample("PngVC", "pngdemo", dir + wxString("/samples/png"), wxStringList("pngdemo.cpp", "pngdemo.h", 0));
    GenerateSample("PrintingVC", "printing", dir + wxString("/samples/printing"), wxStringList("printing.cpp", "printing.h", 0));
    GenerateSample("ProplistVC", "test", dir + wxString("/samples/proplist"), wxStringList("test.cpp", "test.h", 0));
    GenerateSample("PropsizeVC", "propsize", dir + wxString("/samples/propsize"), wxStringList("propsize.cpp", 0));
    GenerateSample("RegtestVC", "regtest", dir + wxString("/samples/regtest"), wxStringList("regtest.cpp", 0));
    GenerateSample("ResourceVC", "resource", dir + wxString("/samples/resource"), wxStringList("resource.cpp", "resource.h", 0));
    GenerateSample("RichEditVC", "wxLayout", dir + wxString("/samples/richedit"), wxStringList("wxLayout.cpp",
        "kbList.cpp", "wxllist.cpp", "wxlparser.cpp", "wxlwindow.cpp", 0));
    GenerateSample("SashtestVC", "sashtest", dir + wxString("/samples/sashtest"), wxStringList("sashtest.cpp", "sashtest.h", 0));
    GenerateSample("ScrollVC", "scroll", dir + wxString("/samples/scroll"), wxStringList("scroll.cpp", 0));
    GenerateSample("ScrollsubVC", "scrollsub", dir + wxString("/samples/scrollsub"), wxStringList("scrollsub.cpp", 0));
    GenerateSample("SplitterVC", "test", dir + wxString("/samples/splitter"), wxStringList("test.cpp", 0));
    GenerateSample("TabVC", "test", dir + wxString("/samples/tab"), wxStringList("test.cpp", "test.h", 0));
    GenerateSample("TaskbarVC", "tbtest", dir + wxString("/samples/taskbar"), wxStringList("tbtest.cpp", "tbtest.h", 0));
    GenerateSample("TextVC", "text", dir + wxString("/samples/text"), wxStringList("text.cpp", 0));
    GenerateSample("ThreadVC", "test", dir + wxString("/samples/thread"), wxStringList("test.cpp", 0));
    GenerateSample("ToolbarVC", "toolbar", dir + wxString("/samples/toolbar"), wxStringList("toolbar.cpp", 0));
    GenerateSample("TreectrlVC", "treetest", dir + wxString("/samples/treectrl"), wxStringList("treetest.cpp", "treetest.h", 0));
    GenerateSample("TypetestVC", "typetest", dir + wxString("/samples/typetest"), wxStringList("typetest.cpp", "typetest.h", 0));
    GenerateSample("ValidateVC", "validate", dir + wxString("/samples/validate"), wxStringList("validate.cpp", "validate.h", 0));
    GenerateSample("ClientVC", "client", dir + wxString("/samples/sockets"), wxStringList("client.cpp", 0));
    GenerateSample("ServerVC", "server", dir + wxString("/samples/sockets"), wxStringList("server.cpp", 0));
    GenerateSample("ClientVC", "client", dir + wxString("/samples/ipc"), wxStringList("client.cpp", "client.h", "ddesetup.h", 0));
    GenerateSample("ServerVC", "server", dir + wxString("/samples/ipc"), wxStringList("server.cpp", "server.h", "ddesetup.h", 0));
    GenerateSample("CaretVC", "caret", dir + wxString("/samples/caret"), wxStringList("caret.cpp", 0));
    GenerateSample("DrawingVC", "drawing", dir + wxString("/samples/drawing"), wxStringList("drawing.cpp", 0));
    GenerateSample("ScrollVC", "scroll", dir + wxString("/samples/scroll"), wxStringList("scroll.cpp", 0));
    GenerateSample("WizardVC", "wizard", dir + wxString("/samples/wizard"), wxStringList("wiztest.cpp", 0));
    GenerateSample("RotateVC", "rotate", dir + wxString("/samples/rotate"), wxStringList("rotate.cpp", 0));
    GenerateSample("ExecVC", "exec", dir + wxString("/samples/exec"), wxStringList("exec.cpp", 0));
    GenerateSample("FontVC", "font", dir + wxString("/samples/font"), wxStringList("font.cpp", 0));
    GenerateSample("MenuVC", "menu", dir + wxString("/samples/menu"), wxStringList("menu.cpp", 0));

    //// Demos

    GenerateSample("BombsVC", "bombs", dir + wxString("/demos/bombs"),
        wxStringList("bombs.cpp", "bombs1.cpp", "game.cpp", "bombs.h", "game.h", 0));

    GenerateSample("FortyVC", "forty", dir + wxString("/demos/forty"),
       wxStringList("forty.cpp", "canvas.cpp", "card.cpp", "game.cpp", "pile.cpp", "playerdg.cpp", "scoredg.cpp", "scorefil.cpp",
       "canvas.h", "forty.h", "card.h", "game.h", "pile.h", "playerdg.h", "scoredg.h", "scorefil.h",
       0));

    GenerateSample("FractalVC", "fractal", dir + wxString("/demos/fractal"), wxStringList("fractal.cpp", 0));

    GenerateSample("LifeVC", "life", dir + wxString("/demos/life"),
        wxStringList("life.cpp", "game.cpp", "dialogs.cpp", "life.h", "game.h", "dialogs.h", 0));

    GenerateSample("PoemVC", "wxpoem", dir + wxString("/demos/poem"), wxStringList("wxpoem.cpp", "wxpoem.h", 0));

    GenerateSample("DbbrowseVC", "dbbrowse", dir + wxString("/demos/dbbrowse"),
       wxStringList("dbbrowse.cpp", "browsedb.cpp", "dbgrid.cpp", "dbtree.cpp", "dlguser.cpp", "doc.cpp",
        "pgmctrl.cpp", "tabpgwin.cpp",
        "dbbrowse.h", "browsedb.h", "dbgrid.h", "dbtree.h", "dlguser.h", "doc.h", "pgmctrl.h", "std.h", "tabpgwin.h",
        0));

    //// Utilities

    // Dialog Editor
    wxProject project;

    project.SetIncludeDirs(wxStringList("../../../include", 0));
    project.SetResourceIncludeDirs(wxStringList("../../../include", 0));
    project.SetLibDirs(wxStringList("../../../lib", 0));
    project.SetDebugLibDirs(wxStringList("../../../src/Debug", "../../../src/jpeg/Debug", "../../../src/tiff/Debug", 0));
    project.SetReleaseLibDirs(wxStringList("../../../src/Release", "../../../src/jpeg/Release", "../../../src/tiff/Release", 0));

    project.SetProjectName("DialogEdVC");
    project.SetTargetName("dialoged");
    project.SetProjectPath(dir + wxString("/utils/dialoged/src"));
    project.SetSourceFiles(wxStringList("dialoged.cpp", "dlghndlr.cpp", "edlist.cpp", "edtree.cpp",
        "reseditr.cpp", "reswrite.cpp", "symbtabl.cpp", "winstyle.cpp", "winprop.cpp",
        "dialoged.h", "dlghndlr.h", "edlist.h", "edtree.h", "reseditr.h", "symbtabl.h", "winprop.h",
        "winstyle.h",
        0));

    if (!project.GenerateVCProject())
    {
        wxString msg("Could not generate Dialog Editor project");
        wxMessageBox(msg);
    }

    // Tex2RTF
    project.SetIncludeDirs(wxStringList("../../../include", 0));
    project.SetResourceIncludeDirs(wxStringList("../../../include", 0));
    project.SetLibDirs(wxStringList("../../../lib", 0));
    project.SetDebugLibDirs(wxStringList("../../../src/Debug", "../../../src/jpeg/Debug", "../../../src/tiff/Debug", 0));
    project.SetReleaseLibDirs(wxStringList("../../../src/Release", "../../../src/jpeg/Release", "../../../src/tiff/Release", 0));

    project.SetProjectName("Tex2RTFVC");
    project.SetTargetName("tex2rtf");
    project.SetProjectPath(dir + wxString("/utils/tex2rtf/src"));
    project.SetSourceFiles(wxStringList("tex2rtf.cpp", "htmlutil.cpp", "readshg.cpp", "rtfutils.cpp",
        "table.cpp", "tex2any.cpp", "texutils.cpp", "xlputils.cpp",
        "bmputils.h", "readshg.h", "rtfutils.h", "table.h", "tex2any.h", "tex2rtf.h", "wxhlpblk.h",
        0));

    if (!project.GenerateVCProject())
    {
        wxString msg("Could not generate Tex2RTF project");
        wxMessageBox(msg);
    }

    // HelpGen
    project.SetIncludeDirs(wxStringList("../../../include", 0));
    project.SetResourceIncludeDirs(wxStringList("../../../include", 0));
    project.SetLibDirs(wxStringList("../../../lib", 0));
    project.SetDebugLibDirs(wxStringList("../../../src/Debug", "../../../src/jpeg/Debug", "../../../src/tiff/Debug", 0));
    project.SetReleaseLibDirs(wxStringList("../../../src/Release", "../../../src/jpeg/Release", "../../../src/tiff/Release", 0));

    project.SetProjectName("HelpGenVC");
    project.SetTargetName("helpgen");
    project.SetProjectPath(dir + wxString("/utils/helpgen/src"));
    project.SetSourceFiles(wxStringList("helpgen.cpp", "cjparser.cpp", "docripper.cpp", "ifcontext.cpp",
        "markup.cpp", "ripper_main.cpp", "scriptbinder.cpp", "sourcepainter.cpp",
        "srcparser.cpp",
        "cjparser.h", "docripper.h", "ifcontext.h", "markup.h", "scriptbinder.h", "sourcepainter.h",
        "srcparser.h", "wxstlac.h", "wxstllst.h", "wxstlvec.h", 0));

    if (!project.GenerateVCProject())
    {
        wxString msg("Could not generate HelpGen project");
        wxMessageBox(msg);
    }

    // ProjGen
    project.SetIncludeDirs(wxStringList("../../include", 0));
    project.SetResourceIncludeDirs(wxStringList("../../include", 0));
    project.SetLibDirs(wxStringList("../../lib", 0));
    project.SetDebugLibDirs(wxStringList("../../src/Debug", "../../src/jpeg/Debug", "../../src/tiff/Debug", 0));
    project.SetReleaseLibDirs(wxStringList("../../src/Release", "../../src/jpeg/Release", "../../src/tiff/Release", 0));

    project.SetProjectName("ProjGenVC");
    project.SetTargetName("makeproj");
    project.SetProjectPath(dir + wxString("/utils/projgen"));
    project.SetSourceFiles(wxStringList("makeproj.cpp", "makeproj.h", 0));

    if (!project.GenerateVCProject())
    {
        wxString msg("Could not generate ProjGen project");
        wxMessageBox(msg);
    }

    // hhp2cached
    project.SetIncludeDirs(wxStringList("../../include", 0));
    project.SetResourceIncludeDirs(wxStringList("../../include", 0));
    project.SetLibDirs(wxStringList("../../lib", 0));
    project.SetDebugLibDirs(wxStringList("../../src/Debug", "../../src/jpeg/Debug", "../../src/tiff/Debug", 0));
    project.SetReleaseLibDirs(wxStringList("../../src/Release", "../../src/jpeg/Release", "../../src/tiff/Release", 0));

    project.SetProjectName("hhp2cachedVC");
    project.SetTargetName("hhp2cached");
    project.SetProjectPath(dir + wxString("/utils/hhp2cached"));
    project.SetSourceFiles(wxStringList("hhp2cached.cpp", 0));

    if (!project.GenerateVCProject())
    {
        wxString msg("Could not generate hhp2cached project");
        wxMessageBox(msg);
    }

    // wxTreeLayout sample

    project.SetIncludeDirs(wxStringList("../../../include", 0));
    project.SetResourceIncludeDirs(wxStringList("../../../include", 0));
    project.SetLibDirs(wxStringList("../../../lib", 0));
    project.SetDebugLibDirs(wxStringList("../../../src/Debug", "../../../src/jpeg/Debug", "../../../src/tiff/Debug", 0));
    project.SetReleaseLibDirs(wxStringList("../../../src/Release", "../../../src/jpeg/Release", "../../../src/tiff/Release", 0));

    project.SetProjectName("TreeSampleVC");
    project.SetTargetName("test");
    project.SetProjectPath(dir + wxString("/utils/wxtree/src"));
    project.SetSourceFiles(wxStringList("test.cpp", "wxtree.cpp", "test.h", "wxtree.h", 0));

    if (!project.GenerateVCProject())
    {
        wxString msg("Could not generate wxTreeLayout project");
        wxMessageBox(msg);
    }

    // OGLEdit

    project.SetIncludeDirs(wxStringList("../../../../include", "../../src", 0));
    project.SetResourceIncludeDirs(wxStringList("../../../../include", 0));
    project.SetLibDirs(wxStringList("../../../../lib", 0));
    project.SetDebugLibDirs(wxStringList("../../../../src/Debug", "../../src/Debug", "../../../../src/jpeg/Debug", "../../../../src/tiff/Debug", 0));
    project.SetReleaseLibDirs(wxStringList("../../../../src/Release", "../../src/Release", "../../../../src/jpeg/Release", "../../../../src/tiff/Release", 0));

    project.SetExtraLibs(wxStringList("ogl.lib", 0));

    project.SetProjectName("OGLEditVC");
    project.SetTargetName("ogledit");
    project.SetProjectPath(dir + wxString("/utils/ogl/samples/ogledit"));
    project.SetSourceFiles(wxStringList("ogledit.cpp", "doc.cpp", "palette.cpp", "view.cpp",
        "doc.h", "ogledit.h", "palette.h", "view.h",
        0));

    if (!project.GenerateVCProject())
    {
        wxString msg("Could not generate OGLEdit project");
        wxMessageBox(msg);
    }

    // OGL Studio

    project.SetIncludeDirs(wxStringList("../../../../include", "../../src", 0));
    project.SetResourceIncludeDirs(wxStringList("../../../../include", 0));
    project.SetLibDirs(wxStringList("../../../../lib", 0));
    project.SetDebugLibDirs(wxStringList("../../../../src/Debug", "../../src/Debug", "../../../../src/jpeg/Debug", "../../../../src/tiff/Debug", 0));
    project.SetReleaseLibDirs(wxStringList("../../../../src/Release", "../../src/Release", "../../../../src/jpeg/Release", "../../../../src/tiff/Release", 0));
    project.SetExtraLibs(wxStringList("ogl.lib", 0));

    project.SetProjectName("StudioVC");
    project.SetTargetName("studio");
    project.SetProjectPath(dir + wxString("/utils/ogl/samples/studio"));
    project.SetSourceFiles(wxStringList("studio.cpp", "cspalette.cpp", "dialogs.cpp", "view.cpp",
        "doc.cpp", "mainfrm.cpp", "project.cpp", "shapes.cpp", "symbols.cpp", "csprint.cpp",
        "studio.h", "cspalette.h", "dialogs.h", "view.h",
        "doc.h", "mainfrm.h", "project.h", "shapes.h", "symbols.h",
        0));

    if (!project.GenerateVCProject())
    {
        wxString msg("Could not generate OGL Studio project");
        wxMessageBox(msg);
    }

    // GLCanvas cube sample

    project.SetIncludeDirs(wxStringList("../../../../include", "../../win", 0));
    project.SetResourceIncludeDirs(wxStringList("../../../../include", 0));
    project.SetLibDirs(wxStringList("../../../../lib", 0));
    project.SetDebugLibDirs(wxStringList("../../../../src/Debug", "../../win/Debug", "../../../../src/jpeg/Debug", "../../../../src/tiff/Debug", 0));
    project.SetReleaseLibDirs(wxStringList("../../../../src/Release", "../../win/Release", "../../../../src/jpeg/Release", "../../../../src/tiff/Release", 0));

    project.SetExtraLibs(wxStringList("glcanvas.lib", "opengl32.lib", "glu32.lib", 0));

    project.SetProjectName("CubeVC");
    project.SetTargetName("cube");
    project.SetProjectPath(dir + wxString("/utils/glcanvas/samples/cube"));
    project.SetSourceFiles(wxStringList("cube.cpp", "cube.h",
        0));

    if (!project.GenerateVCProject())
    {
        wxString msg("Could not generate GLCanvas Cube project");
        wxMessageBox(msg);
    }

    // GLCanvas isosurf sample

    project.SetIncludeDirs(wxStringList("../../../../include", "../../win", 0));
    project.SetResourceIncludeDirs(wxStringList("../../../../include", 0));
    project.SetLibDirs(wxStringList("../../../../lib", 0));
    project.SetDebugLibDirs(wxStringList("../../../../src/Debug", "../../win/Debug", "../../../../src/jpeg/Debug", "../../../../src/tiff/Debug", 0));
    project.SetReleaseLibDirs(wxStringList("../../../../src/Release", "../../win/Release", "../../../../src/jpeg/Release", "../../../../src/tiff/Release", 0));
    project.SetExtraLibs(wxStringList("glcanvas.lib", "opengl32.lib", "glu32.lib", 0));

    project.SetProjectName("IsoSurfVC");
    project.SetTargetName("isosurf");
    project.SetProjectPath(dir + wxString("/utils/glcanvas/samples/isosurf"));
    project.SetSourceFiles(wxStringList("isosurf.cpp", "isosurf.h",
        0));

    if (!project.GenerateVCProject())
    {
        wxString msg("Could not generate GLCanvas IsoSurf project");
        wxMessageBox(msg);
    }

    // GLCanvas penguin sample

    project.SetIncludeDirs(wxStringList("../../../../include", "../../win", 0));
    project.SetResourceIncludeDirs(wxStringList("../../../../include", 0));
    project.SetLibDirs(wxStringList("../../../../lib", 0));
    project.SetDebugLibDirs(wxStringList("../../../../src/Debug", "../../win/Debug", "../../../../src/jpeg/Debug", "../../../../src/tiff/Debug", 0));
    project.SetReleaseLibDirs(wxStringList("../../../../src/Release", "../../win/Release", "../../../../src/jpeg/Release", "../../../../src/tiff/Release", 0));
    project.SetExtraLibs(wxStringList("glcanvas.lib", "opengl32.lib", "glu32.lib", 0));

    project.SetProjectName("PenguinVC");
    project.SetTargetName("penguin");
    project.SetProjectPath(dir + wxString("/utils/glcanvas/samples/penguin"));
    project.SetSourceFiles(wxStringList("penguin.cpp", "penguin.h",
        "lw.cpp", "lw.h",
        "trackball.c", "trackball.h",
        0));

    if (!project.GenerateVCProject())
    {
        wxString msg("Could not generate GLCanvas Penguin project");
        wxMessageBox(msg);
    }
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
    // set the frame icon
    SetIcon(wxICON(mondrian));

    // create a menu bar
    wxMenu *menuFile = new wxMenu;

    menuFile->Append(MakeProject_Generate, "&Generate");
    menuFile->Append(MakeProject_About, "&About...");
    menuFile->AppendSeparator();
    menuFile->Append(MakeProject_Quit, "E&xit");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText("Welcome to wxWindows!");
}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox("MakeProject: generates VC++ project files",
                 "About MakeProject", wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnGenerate(wxCommandEvent& WXUNUSED(event))
{
    wxGetApp().GenerateSamples("d:/wx2/wxWindows");
}

bool MyFrame::GenerateSample(const wxString& projectName, const wxString& targetName,
    const wxString& path, const wxStringList& sourceFiles, const wxString& relativeRootPath)
{
    return wxGetApp().GenerateSample(projectName, targetName, path, sourceFiles, relativeRootPath);
}

/*
 * wxProject
 */

wxProject::wxProject()
{
}

wxProject::~wxProject()
{
}


bool wxProject::GenerateVCProject()
{
    wxString fullProjectName = m_path + wxString("/") + m_projectName + ".dsp";

    ofstream stream(fullProjectName);
    if (stream.bad())
        return FALSE;

    /////////////////////// General stuff

    stream << "# Microsoft Developer Studio Project File - Name=\"" << m_projectName << "\" - Package Owner=<4>\n";
    stream << "# Microsoft Developer Studio Generated Build File, Format Version 5.00\n";
    stream << "# (Actually, generated by MakeProject, (c) Julian Smart, 1998)\n";
    stream << "# ** DO NOT EDIT **\n\n";
    stream << "# TARGTYPE \"Win32 (x86) Application\" 0x0101\n\n";
    stream << "CFG=" << m_projectName << " - Win32 Debug\n";
    stream << "!MESSAGE This is not a valid makefile. To build this project using NMAKE,\n";
    stream << "!MESSAGE use the Export Makefile command and run\n";
    stream << "!MESSAGE\n";
    stream << "!MESSAGE NMAKE /f \"" << m_projectName << ".mak\".\n";
    stream << "!MESSAGE\n";
    stream << "!MESSAGE You can specify a configuration when running NMAKE\n";
    stream << "!MESSAGE by defining the macro CFG on the command line. For example:\n";
    stream << "!MESSAGE\n";
    stream << "!MESSAGE NMAKE /f \"" << m_projectName << ".mak\" CFG=\"" << m_projectName << " - Win32 Debug\"\n";
    stream << "!MESSAGE\n";
    stream << "!MESSAGE Possible choices for configuration are:\n";
    stream << "!MESSAGE\n";
    stream << "!MESSAGE \"" << m_projectName << " - Win32 Release\" (based on \"Win32 (x86) Application\")\n";
    stream << "!MESSAGE \"" << m_projectName << " - Win32 Debug\" (based on \"Win32 (x86) Application\")\n";
    stream << "!MESSAGE \"" << m_projectName << " - Win32 Debug DLL\" (based on \"Win32 (x86) Application\")\n";
    stream << "!MESSAGE \"" << m_projectName << " - Win32 Release DLL\" (based on \"Win32 (x86) Application\")\n";
    stream << "!MESSAGE\n";
    stream << "\n";
    stream << "# Begin Project\n";
    stream << "# PROP Scc_ProjName \"\"\n";
    stream << "# PROP Scc_LocalPath \"\"\n";
    stream << "CPP=cl.exe\n";
    stream << "MTL=midl.exe\n";
    stream << "RSC=rc.exe\n";
    stream << "\n";

    /////////////////////// Win32 Release target

    stream << "!IF  \"$(CFG)\" == \"" << m_projectName << " - Win32 Release\"\n";
    stream << "\n";
    stream << "# PROP BASE Use_MFC 0\n";
    stream << "# PROP BASE Use_Debug_Libraries 0\n";
    stream << "# PROP BASE Output_Dir \"Release\"\n";
    stream << "# PROP BASE Intermediate_Dir \"Release\"\n";
    stream << "# PROP BASE Target_Dir \"\"\n";
    stream << "# PROP Use_MFC 0\n";
    stream << "# PROP Use_Debug_Libraries 0\n";
    stream << "# PROP Output_Dir \"Release\"\n";
    stream << "# PROP Intermediate_Dir \"Release\"\n";
    stream << "# PROP Ignore_Export_Lib 0\n";
    stream << "# PROP Target_Dir \"\"\n";
    stream << "# ADD BASE CPP /nologo /W3 /GX /O2 /D \"WIN32\" /D \"NDEBUG\" /D \"_WINDOWS\" /YX /FD /c\n";
    stream << "# ADD CPP /nologo /MD /W3 /GX /O1 /Ob2";

    int n = m_includeDirs.Number();
    int i;
    for (i = 0; i < n; i++)
    {
        wxString includeDir = m_includeDirs[i];
        stream << " /I \"" << includeDir << "\"";
    }

    stream << " /D \"NDEBUG\" /D \"WIN32\" /D \"_WINDOWS\" /D \"__WINDOWS__\" /D \"__WXMSW__\" /D \"__WIN95__\" /D \"__WIN32__\" /D WINVER=0x0400 /D \"STRICT\" /FD /c\n";
    stream << "# SUBTRACT CPP /YX\n";
    stream << "# ADD BASE MTL /nologo /D \"NDEBUG\" /mktyplib203 /o NUL /win32\n";
    stream << "# ADD MTL /nologo /D \"NDEBUG\" /mktyplib203 /o NUL /win32\n";
    stream << "# ADD BASE RSC /l 0x809 /d \"NDEBUG\"\n";
    stream << "# ADD RSC /l 0x809 /d \"NDEBUG\"\n";
    stream << "BSC32=bscmake.exe\n";
    stream << "# ADD BASE BSC32 /nologo\n";
    stream << "# ADD BSC32 /nologo\n";
    stream << "LINK32=link.exe\n";
    stream << "# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:windows /machine:I386\n";
    stream << "# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib wxvc.lib jpeg.lib tiff.lib ";
    n = m_extraLibs.Number();
    for (i = 0; i < n; i++)
    {
        wxString lib = m_extraLibs[i];
        stream << lib << " ";
    }

    stream << "/nologo /subsystem:windows /machine:I386 /nodefaultlib:\"libc.lib,libci.lib,msvcrtd.lib\" /out:\"Release/" << m_targetName << ".exe\"";

    n = m_releaseLibDirs.Number();
    for (i = 0; i < n; i++)
    {
        wxString libDir = m_releaseLibDirs[i];
        stream << " /libpath:\"" << libDir << "\"";
    }
    n = m_libDirs.Number();
    for (i = 0; i < n; i++)
    {
        wxString libDir = m_libDirs[i];
        stream << " /libpath:\"" << libDir << "\"";
    }
    stream << "\n";
    stream << "\n";

    /////////////////////// Win32 Debug target

    stream << "!ELSEIF  \"$(CFG)\" == \"" << m_projectName << " - Win32 Debug\"\n";
    stream << "\n";
    stream << "# PROP BASE Use_MFC 0\n";
    stream << "# PROP BASE Use_Debug_Libraries 1\n";
    stream << "# PROP BASE Output_Dir \"Debug\"\n";
    stream << "# PROP BASE Intermediate_Dir \"Debug\"\n";
    stream << "# PROP BASE Target_Dir \"\"\n";
    stream << "# PROP Use_MFC 0\n";
    stream << "# PROP Use_Debug_Libraries 1\n";
    stream << "# PROP Output_Dir \"Debug\"\n";
    stream << "# PROP Intermediate_Dir \"Debug\"\n";
    stream << "# PROP Ignore_Export_Lib 0\n";
    stream << "# PROP Target_Dir \"\"\n";
    stream << "# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D \"WIN32\" /D \"_DEBUG\" /D \"_WINDOWS\" /YX /FD /c\n";
    stream << "# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od";

    n = m_includeDirs.Number();
    for (i = 0; i < n; i++)
    {
        wxString includeDir = m_includeDirs[i];
        stream << " /I \"" << includeDir << "\"";
    }

    stream << " /D \"WIN32\" /D \"_DEBUG\" /D \"_WINDOWS\" /D \"__WINDOWS__\" /D \"__WXMSW__\" /D DEBUG=1 /D \"__WXDEBUG__\" /D \"__WIN95__\" /D \"__WIN32__\" /D WINVER=0x0400 /D \"STRICT\" /Yu\"wx/wxprec.h\" /FD /c\n";
    stream << "# ADD BASE MTL /nologo /D \"_DEBUG\" /mktyplib203 /o NUL /win32\n";
    stream << "# ADD MTL /nologo /D \"_DEBUG\" /mktyplib203 /o NUL /win32\n";
    stream << "# ADD BASE RSC /l 0x809 /d \"_DEBUG\"\n";
    stream << "# ADD RSC /l 0x809 /d \"_DEBUG\"\n";
    stream << "BSC32=bscmake.exe\n";
    stream << "# ADD BASE BSC32 /nologo\n";
    stream << "# ADD BSC32 /nologo\n";
    stream << "LINK32=link.exe\n";
    stream << "# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept\n";
    stream << "# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib wxvc.lib jpeg.lib tiff.lib ";
    n = m_extraLibs.Number();
    for (i = 0; i < n; i++)
    {
        wxString lib = m_extraLibs[i];
        stream << lib << " ";
    }
    stream << "/nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:\"libcd.lib,libcid.lib,msvcrt.lib\" /out:\"Debug/" << m_targetName << ".exe\" /pdbtype:sept";

    n = m_debugLibDirs.Number();
    for (i = 0; i < n; i++)
    {
        wxString libDir = m_debugLibDirs[i];
        stream << " /libpath:\"" << libDir << "\"";
    }
    n = m_libDirs.Number();
    for (i = 0; i < n; i++)
    {
        wxString libDir = m_libDirs[i];
        stream << " /libpath:\"" << libDir << "\"";
    }
    stream << "\n";
    stream << "\n";
//    stream << "!ENDIF\n";
//    stream << "\n";

    /////////////////////// Win32 Debug DLL target

    stream << "!ELSEIF  \"$(CFG)\" == \"" << m_projectName << " - Win32 Debug DLL\"\n";
    stream << "\n";
    stream << "# PROP BASE Use_MFC 0\n";
    stream << "# PROP BASE Use_Debug_Libraries 1\n";
    stream << "# PROP BASE Output_Dir \"DebugDLL\"\n";
    stream << "# PROP BASE Intermediate_Dir \"DebugDLL\"\n";
    stream << "# PROP BASE Target_Dir \"\"\n";
    stream << "# PROP Use_MFC 0\n";
    stream << "# PROP Use_Debug_Libraries 1\n";
    stream << "# PROP Output_Dir \"DebugDLL\"\n";
    stream << "# PROP Intermediate_Dir \"DebugDLL\"\n";
    stream << "# PROP Ignore_Export_Lib 0\n";
    stream << "# PROP Target_Dir \"\"\n";
    stream << "# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D \"WIN32\" /D \"_DEBUG\" /D \"_WINDOWS\" /YX /FD /c\n";
    stream << "# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od";

    n = m_includeDirs.Number();
    for (i = 0; i < n; i++)
    {
        wxString includeDir = m_includeDirs[i];
        stream << " /I \"" << includeDir << "\"";
    }

    stream << " /D \"WIN32\" /D \"_DEBUG\" /D \"_WINDOWS\" /D \"__WINDOWS__\" /D \"__WXMSW__\" /D DEBUG=1 /D \"__WXDEBUG__\" /D \"__WIN95__\" /D \"__WIN32__\" /D WINVER=0x0400 /D \"STRICT\" /D WXUSINGDLL=1 /Yu\"wx/wxprec.h\" /FD /c\n";
    stream << "# ADD BASE MTL /nologo /D \"_DEBUG\" /mktyplib203 /o NUL /win32\n";
    stream << "# ADD MTL /nologo /D \"_DEBUG\" /mktyplib203 /o NUL /win32\n";
    stream << "# ADD BASE RSC /l 0x809 /d \"_DEBUG\"\n";
    stream << "# ADD RSC /l 0x809 /d \"_DEBUG\"\n";
    stream << "BSC32=bscmake.exe\n";
    stream << "# ADD BASE BSC32 /nologo\n";
    stream << "# ADD BSC32 /nologo\n";
    stream << "LINK32=link.exe\n";
    stream << "# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept\n";
    stream << "# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib wxvc.lib ";
    n = m_extraLibs.Number();
    for (i = 0; i < n; i++)
    {
        wxString lib = m_extraLibs[i];
        stream << lib << " ";
    }
    stream << "/nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:\"libcd.lib\" /nodefaultlib:\"libcid.lib\" /out:\"DebugDLL/" << m_targetName << ".exe\" /pdbtype:sept";

    n = m_debugLibDirs.Number();
    for (i = 0; i < n; i++)
    {
        wxString libDir = m_debugLibDirs[i];
        libDir += "DLL"; // Assume that we have e.g. Debug so make it DebugDLL
        stream << " /libpath:\"" << libDir << "\"";
    }
    n = m_libDirs.Number();
    for (i = 0; i < n; i++)
    {
        wxString libDir = m_libDirs[i];
        stream << " /libpath:\"" << libDir << "\"";
    }
    stream << "\n";
    stream << "\n";
//    stream << "!ENDIF\n";
//    stream << "\n";

    /////////////////////// Win32 Release DLL target

    stream << "!ELSEIF  \"$(CFG)\" == \"" << m_projectName << " - Win32 Release DLL\"\n";
    stream << "\n";
    stream << "# PROP BASE Use_MFC 0\n";
    stream << "# PROP BASE Use_Debug_Libraries 0\n";
    stream << "# PROP BASE Output_Dir \"ReleaseDLL\"\n";
    stream << "# PROP BASE Intermediate_Dir \"ReleaseDLL\"\n";
    stream << "# PROP BASE Target_Dir \"\"\n";
    stream << "# PROP Use_MFC 0\n";
    stream << "# PROP Use_Debug_Libraries 0\n";
    stream << "# PROP Output_Dir \"ReleaseDLL\"\n";
    stream << "# PROP Intermediate_Dir \"ReleaseDLL\"\n";
    stream << "# PROP Ignore_Export_Lib 0\n";
    stream << "# PROP Target_Dir \"\"\n";
    stream << "# ADD BASE CPP /nologo /W3 /GX /O2 /D \"WIN32\" /D \"NDEBUG\" /D \"_WINDOWS\" /YX /FD /c\n";
    stream << "# ADD CPP /nologo /MD /W3 /GX /O1 /Ob2";

    n = m_includeDirs.Number();
    for (i = 0; i < n; i++)
    {
        wxString includeDir = m_includeDirs[i];
        stream << " /I \"" << includeDir << "\"";
    }

    stream << " /D \"NDEBUG\" /D \"WIN32\" /D \"_WINDOWS\" /D \"__WINDOWS__\" /D \"__WXMSW__\" /D \"__WIN95__\" /D \"__WIN32__\" /D WINVER=0x0400 /D \"STRICT\" /D WXUSINGDLL=1 /FD /c\n";
    stream << "# SUBTRACT CPP /YX\n";
    stream << "# ADD BASE MTL /nologo /D \"NDEBUG\" /mktyplib203 /o NUL /win32\n";
    stream << "# ADD MTL /nologo /D \"NDEBUG\" /mktyplib203 /o NUL /win32\n";
    stream << "# ADD BASE RSC /l 0x809 /d \"NDEBUG\"\n";
    stream << "# ADD RSC /l 0x809 /d \"NDEBUG\"\n";
    stream << "BSC32=bscmake.exe\n";
    stream << "# ADD BASE BSC32 /nologo\n";
    stream << "# ADD BSC32 /nologo\n";
    stream << "LINK32=link.exe\n";
    stream << "# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:windows /machine:I386\n";
    stream << "# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib wxvc.lib ";
    n = m_extraLibs.Number();
    for (i = 0; i < n; i++)
    {
        wxString lib = m_extraLibs[i];
        stream << lib << " ";
    }
    stream << "/nologo /subsystem:windows /machine:I386 /nodefaultlib:\"libc.lib\" /nodefaultlib:\"libci.lib\" /out:\"ReleaseDLL/" << m_targetName << ".exe\"";

    n = m_releaseLibDirs.Number();
    for (i = 0; i < n; i++)
    {
        wxString libDir = m_releaseLibDirs[i];
        libDir += "DLL"; // Assume that we have e.g. Release so make it ReleaseDLL
        stream << " /libpath:\"" << libDir << "\"";
    }
    n = m_libDirs.Number();
    for (i = 0; i < n; i++)
    {
        wxString libDir = m_libDirs[i];
        stream << " /libpath:\"" << libDir << "\"";
    }
    stream << "\n";
    stream << "\n";
    stream << "!ENDIF\n";
    stream << "\n";

    /////////////////////// Source code for targets

    stream << "# Begin Target\n";
    stream << "\n";
    stream << "# Name \"" << m_projectName << " - Win32 Release\"\n";
    stream << "# Name \"" << m_projectName << " - Win32 Debug\"\n";
    stream << "# Name \"" << m_projectName << " - Win32 Debug DLL\"\n";
    stream << "# Name \"" << m_projectName << " - Win32 Release DLL\"\n";

    // C++ source files
    n = m_sourceFiles.Number();
    for (i = 0; i < n; i++)
    {
        wxString sourceFile = m_sourceFiles[i];

        stream << "# Begin Source File\n";
        stream << "\n";
        stream << "SOURCE=.\\" << sourceFile << "\n";
        stream << "\n";
        stream << "!IF  \"$(CFG)\" == \"" << m_projectName << " - Win32 Release\"\n";
        stream << "\n";
        stream << "!ELSEIF  \"$(CFG)\" == \"" << m_projectName << " - Win32 Debug\"\n";
        stream << "\n";
        stream << "# SUBTRACT CPP /YX /Yc /Yu\n";
        stream << "\n";
        stream << "!ELSEIF  \"$(CFG)\" == \"" << m_projectName << " - Win32 Debug DLL\"\n";
        stream << "\n";
        stream << "# SUBTRACT BASE CPP /YX /Yc /Yu\n";
        stream << "# SUBTRACT CPP /YX /Yc /Yu\n";
        stream << "\n";
        stream << "!ELSEIF  \"$(CFG)\" == \"" << m_projectName << " - Win32 Release DLL\"\n";
        stream << "\n";
        stream << "!ENDIF\n";
        stream << "\n";
        stream << "# End Source File\n";
    }

    // The .rc file: assume it has the target name + rc extension.
    stream << "# Begin Source File\n";
    stream << "\n";
    stream << "SOURCE=.\\" << m_targetName << ".rc\n";
    stream << "# ADD BASE RSC /l 0x809\n";
    stream << "# ADD RSC /l 0x809";

    n = m_resourceIncludeDirs.Number();
    for (i = 0; i < n; i++)
    {
        wxString includeDir = m_resourceIncludeDirs[i];
        stream << " /i \"" << includeDir << "\"";
    }

    stream << "\n";
    stream << "# End Source File\n";
    stream << "# End Target\n";
    stream << "# End Project\n";

    // Now generate the .dsw workspace file

    wxString fullWorkSpaceName = m_path + wxString("/") + m_projectName + ".dsw";

    ofstream stream2(fullWorkSpaceName);
    if (stream2.bad())
        return FALSE;

    stream2 << "Microsoft Developer Studio Workspace File, Format Version 5.00\n";
    stream2 << "# WARNING: DO NOT EDIT OR DELETE THIS WORKSPACE FILE!\n";
    stream2 << "\n";
    stream2 << "###############################################################################\n";
    stream2 << "\n";
    stream2 << "Project: \"" << m_projectName << "\"=.\\" << m_projectName << ".dsp - Package Owner=<4>\n";
    stream2 << "\n";
    stream2 << "Package=<5>\n";
    stream2 << "{{{\n";
    stream2 << "}}}\n";
    stream2 << "\n";
    stream2 << "Package=<4>\n";
    stream2 << "{{{\n";
    stream2 << "}}}\n";
    stream2 << "\n";
    stream2 << "###############################################################################\n";
    stream2 << "\n";
    stream2 << "Global:\n";
    stream2 << "\n";
    stream2 << "Package=<5>\n";
    stream2 << "{{{\n";
    stream2 << "}}}\n";
    stream2 << "\n";
    stream2 << "Package=<3>\n";
    stream2 << "{{{\n";
    stream2 << "}}}\n";
    stream2 << "\n";
    stream2 << "###############################################################################\n";
    stream2 << "\n";

    return TRUE;
}

BEGIN_EVENT_TABLE(MyDialog, wxDialog)
    EVT_BUTTON(wxID_EXIT,  MyDialog::OnQuit)
    EVT_BUTTON(ID_GENERATE_PROJECT, MyDialog::OnGenerate)
    EVT_BUTTON(ID_GENERATE_SAMPLES, MyDialog::OnGenerateSamples)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyDialog::MyDialog(const wxString& title, const wxPoint& pos, const wxSize& size):
  wxDialog()
{
    LoadFromResource((wxWindow*) NULL, "project_dialog");

}

void MyDialog::OnQuit(wxCommandEvent& event)
{
    this->EndModal(wxID_OK);
}

void MyDialog::OnAbout(wxCommandEvent& event)
{
}

void MyDialog::OnGenerate(wxCommandEvent& event)
{
}

void MyDialog::OnGenerateSamples(wxCommandEvent& event)
{
    char* dir = getenv("WXWIN");
    wxString dirStr;
    if (dir)
        dirStr = dir;
    wxTextEntryDialog dialog(this, "Please enter the wxWindows directory", "Text entry", dirStr, wxOK|wxCANCEL);
    if (dialog.ShowModal() == wxID_OK)
    {
        if (wxDirExists(dialog.GetValue()))
        {
//            wxGetApp().GenerateSample("MinimalVC", "minimal", dir + wxString("/samples/minimal"),
//                wxStringList("minimal.cpp", 0));

            wxGetApp().GenerateSamples(dialog.GetValue());
        }
        else
        {
            wxMessageBox("This directory doesn't exist.");
        }
    }
}

