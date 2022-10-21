///////////////////////////////////////////////////////////////////////////////
// Name:        debugrpt.cpp
// Purpose:     minimal sample showing wxDebugReport and related classes
// Author:      Vadim Zeitlin
// Modified by:
// Created:     2005-01-20
// Copyright:   (c) 2005 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/app.h"
#include "wx/log.h"
#include "wx/frame.h"
#include "wx/icon.h"
#include "wx/menu.h"
#include "wx/choicdlg.h"
#include "wx/msgdlg.h"
#include "wx/button.h"
#include "wx/dcclient.h"
#include "wx/timer.h"

#include "wx/datetime.h"
#include "wx/ffile.h"
#include "wx/filename.h"
#include "wx/debugrpt.h"
#include "wx/dynlib.h"

#if !wxUSE_DEBUGREPORT
    #error "This sample can't be built without wxUSE_DEBUGREPORT"
#endif // wxUSE_DEBUGREPORT

#if !wxUSE_ON_FATAL_EXCEPTION
    #error "This sample can't be built without wxUSE_ON_FATAL_EXCEPTION"
#endif // wxUSE_ON_FATAL_EXCEPTION

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// custom debug reporting class
// ----------------------------------------------------------------------------

// this is your custom debug reporter: it will use curl program (which should
// be available) to upload the crash report to the given URL (which should be
// set up by you)
class MyDebugReport : public wxDebugReportUpload
{
public:
    MyDebugReport() : wxDebugReportUpload
                       (
                        "http://your.url.here/",
                        "report:file",
                        "action"
                       )
    {
    }

protected:
    // this is called with the contents of the server response: you will
    // probably want to parse the XML document in OnServerReply() instead of
    // just dumping it as I do
    virtual bool OnServerReply(const wxArrayString& reply) override
    {
        if ( reply.IsEmpty() )
        {
            wxLogError("Didn't receive the expected server reply.");
            return false;
        }

        wxString s("Server replied:\n");

        const size_t count = reply.GetCount();
        for ( size_t n = 0; n < count; n++ )
        {
            s << '\t' << reply[n] << '\n';
        }

        wxLogMessage("%s", s);

        return true;
    }
};

// another possibility would be to build email library from contrib and use
// this class, after uncommenting it:
#if 0

#include "wx/net/email.h"

class MyDebugReport : public wxDebugReportCompress
{
public:
    virtual bool DoProcess()
    {
        if ( !wxDebugReportCompress::DoProcess() )
            return false;
        wxMailMessage msg(GetReportName() + " crash report",
                          "vadim@wxwidgets.org",
                          wxEmptyString, // mail body
                          wxEmptyString, // from address
                          GetCompressedFileName(),
                          "crashreport.zip");

        return wxEmail::Send(msg);
    }
};

#endif // 0

// ----------------------------------------------------------------------------
// helper functions
// ----------------------------------------------------------------------------

// just some functions to get a slightly deeper stack trace
static void bar(const char *p)
{
    char *pc = 0;
    *pc = *p;

    printf("bar: %s\n", p);
}

void baz(const wxString& s)
{
    printf("baz: %s\n", (const char*)s.c_str());
}

void foo(int n)
{
    if ( n % 2 )
        baz("odd");
    else
        bar("even");
}

// ----------------------------------------------------------------------------
// main window
// ----------------------------------------------------------------------------

enum
{
    DebugRpt_ListLoadedDLLs = 50,
    DebugRpt_Quit = wxID_EXIT,
    DebugRpt_Crash = 100,
    DebugRpt_Current,
    DebugRpt_Paint,
    DebugRpt_Timer,
    DebugRpt_Upload,
    DebugRpt_About = wxID_ABOUT
};

class MyFrame : public wxFrame
{
public:
    MyFrame();

private:
    void OnListLoadedDLLs(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnReportForCrash(wxCommandEvent& event);
    void OnReportForCurrent(wxCommandEvent& event);
    void OnReportPaint(wxCommandEvent& event);
    void OnReportTimer(wxCommandEvent& event);
    void OnReportUpload(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnPaint(wxPaintEvent& event);

    // a timer whose only purpose in life is to crash as soon as it's started
    class BadTimer : public wxTimer
    {
    public:
        BadTimer() { }

        virtual void Notify() override
        {
            foo(8);
        }
    } m_badTimer;

    // number of lines drawn in OnPaint()
    int m_numLines;

    wxDECLARE_NO_COPY_CLASS(MyFrame);
    wxDECLARE_EVENT_TABLE();
};

// ----------------------------------------------------------------------------
// application class
// ----------------------------------------------------------------------------

// this is a usual application class modified to work with debug reporter
//
// basically just 2 things are necessary: call wxHandleFatalExceptions() as
// early as possible and override OnFatalException() to create the report there
class MyApp : public wxApp
{
public:
    // call wxHandleFatalExceptions here
    MyApp();

    // create our main window here
    virtual bool OnInit() override;

    // called when a crash occurs in this application
    virtual void OnFatalException() override;

    // this is where we really generate the debug report
    void GenerateReport(wxDebugReport::Context ctx);

    // if this function is called, we'll use MyDebugReport which would try to
    // upload the (next) generated debug report to its URL, otherwise we just
    // generate the debug report and leave it in a local file
    void UploadReport(bool doIt) { m_uploadReport = doIt; }

private:
    bool m_uploadReport;

    wxDECLARE_NO_COPY_CLASS(MyApp);
};

wxIMPLEMENT_APP(MyApp);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(DebugRpt_ListLoadedDLLs, MyFrame::OnListLoadedDLLs)
    EVT_MENU(DebugRpt_Quit, MyFrame::OnQuit)
    EVT_MENU(DebugRpt_Crash, MyFrame::OnReportForCrash)
    EVT_MENU(DebugRpt_Current, MyFrame::OnReportForCurrent)
    EVT_MENU(DebugRpt_Paint, MyFrame::OnReportPaint)
    EVT_MENU(DebugRpt_Timer, MyFrame::OnReportTimer)
    EVT_MENU(DebugRpt_Upload, MyFrame::OnReportUpload)
    EVT_MENU(DebugRpt_About, MyFrame::OnAbout)

    EVT_PAINT(MyFrame::OnPaint)
wxEND_EVENT_TABLE()

MyFrame::MyFrame()
       : wxFrame(nullptr, wxID_ANY, "wxWidgets Debug Report Sample",
                 wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE|wxDEFAULT_FRAME_STYLE)
{
    m_numLines = 10;

    SetIcon(wxICON(sample));

    wxMenu *menuFile = new wxMenu;
    menuFile->Append(DebugRpt_ListLoadedDLLs, "&List loaded DLLs...\tCtrl-L");
    menuFile->AppendSeparator();
    menuFile->Append(DebugRpt_Quit, "E&xit\tAlt-X");

    wxMenu *menuReport = new wxMenu;
    menuReport->Append(DebugRpt_Crash, "Report for &crash\tCtrl-C",
                       "Provoke a crash inside the program and create report for it");
    menuReport->Append(DebugRpt_Current, "Report for c&urrent context\tCtrl-U",
                       "Create report for the current program context");
    menuReport->Append(DebugRpt_Paint, "Report for &paint handler\tCtrl-P",
                       "Provoke a repeatable crash in wxEVT_PAINT handler");
    menuReport->Append(DebugRpt_Timer, "Report for &timer handler\tCtrl-T",
                       "Provoke a crash in wxEVT_TIMER handler");
    menuReport->AppendSeparator();
    menuReport->AppendCheckItem(DebugRpt_Upload, "Up&load debug report",
                       "You need to configure a web server accepting debug report uploads to use this function");

    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(DebugRpt_About, "&About\tF1");

    wxMenuBar *mbar = new wxMenuBar();
    mbar->Append(menuFile, "&File");
    mbar->Append(menuReport, "&Report");
    mbar->Append(menuHelp, "&Help");
    SetMenuBar(mbar);

    CreateStatusBar();

    Show();
}

void MyFrame::OnListLoadedDLLs(wxCommandEvent& WXUNUSED(event))
{
    const wxDynamicLibraryDetailsArray loaded = wxDynamicLibrary::ListLoaded();
    const size_t count = loaded.size();
    if ( !count )
    {
        wxLogError("Failed to get the list of loaded dynamic libraries.");
        return;
    }

    wxArrayString names;
    names.reserve(count);
    for ( size_t n = 0; n < count; n++ )
    {
        names.push_back(loaded[n].GetName());
    }

    for ( ;; )
    {
        const int sel = wxGetSingleChoiceIndex
                        (
                            "Choose a library to show more information about it",
                            "List of loaded dynamic libraries",
                            names,
                            this
                        );
        if ( sel == wxNOT_FOUND )
            return;

        const wxDynamicLibraryDetails& det = loaded[sel];
        void *addr = 0;
        size_t len = 0;
        det.GetAddress(&addr, &len);
        wxLogMessage("Full path is \"%s\", memory range %p:%p, version \"%s\"",
                     det.GetPath(),
                     addr,
                     static_cast<void*>(static_cast<char*>(addr) + len),
                     det.GetVersion());
    }
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyFrame::OnReportForCrash(wxCommandEvent& WXUNUSED(event))
{
    // this call is going to crash
    foo(32);
    foo(17);
}

void MyFrame::OnReportForCurrent(wxCommandEvent& WXUNUSED(event))
{
    // example of manually generated report, this could be also
    // used in wxApp::OnAssert()
    wxGetApp().GenerateReport(wxDebugReport::Context_Current);
}

void MyFrame::OnReportPaint(wxCommandEvent& WXUNUSED(event))
{
    // this will result in a crash in OnPaint()
    m_numLines = 0;

    // ensure it's called immediately
    Refresh();
}

void MyFrame::OnReportTimer(wxCommandEvent& WXUNUSED(event))
{
    // this will result in a crash in BadTimer::OnNotify() soon
    m_badTimer.StartOnce(100);
}

void MyFrame::OnReportUpload(wxCommandEvent& event)
{
    wxGetApp().UploadReport(event.IsChecked());
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox
    (
        "wxDebugReport sample\n(c) 2005 Vadim Zeitlin <vadim@wxwidgets.org>",
        "wxWidgets Debug Report Sample",
        wxOK | wxICON_INFORMATION,
        this
    );
}

void MyFrame::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);
    const wxSize size = GetClientSize();
    for ( wxCoord x = 0; x < size.x; x += size.x/m_numLines )
        dc.DrawLine(x, 0, x, size.y);
}

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

MyApp::MyApp()
{
    // user needs to explicitly enable this
    m_uploadReport = false;

    // call this to tell the library to call our OnFatalException()
    wxHandleFatalExceptions();
}

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    new MyFrame;

    return true;
}

void MyApp::OnFatalException()
{
    GenerateReport(wxDebugReport::Context_Exception);
}

void MyApp::GenerateReport(wxDebugReport::Context ctx)
{
    wxDebugReportCompress *report = m_uploadReport ? new MyDebugReport
                                                   : new wxDebugReportCompress;

    // add all standard files: currently this means just a minidump and an
    // XML file with system info and stack trace
    report->AddAll(ctx);

    // you can also call report->AddFile(...) with your own log files, files
    // created using wxRegKey::Export() and so on, here we just add a test
    // file containing the date of the crash
    wxFileName fn(report->GetDirectory(), "timestamp.my");
    wxFFile file(fn.GetFullPath(), "w");
    if ( file.IsOpened() )
    {
        wxDateTime dt = wxDateTime::Now();
        file.Write(dt.FormatISODate() + ' ' + dt.FormatISOTime());
        file.Close();
    }

    report->AddFile(fn.GetFullName(), "timestamp of this report");

    // can also add an existing file directly, it will be copied
    // automatically
#ifdef __WXMSW__
    wxString windir;
    if ( !wxGetEnv("WINDIR", &windir) )
        windir = "C:\\Windows";
    fn.AssignDir(windir);
    fn.AppendDir("system32");
    fn.AppendDir("drivers");
    fn.AppendDir("etc");
#else // !__WXMSW__
    fn.AssignDir("/etc");
#endif // __WXMSW__/!__WXMSW__
    fn.SetFullName("hosts");

    if ( fn.FileExists() )
        report->AddFile(fn.GetFullPath(), "Local hosts file");

    // calling Show() is not mandatory, but is more polite
    if ( wxDebugReportPreviewStd().Show(*report) )
    {
        if ( report->Process() )
        {
            if ( m_uploadReport )
            {
                wxLogMessage("Report successfully uploaded.");
            }
            else
            {
                wxLogMessage("Report generated in \"%s\".",
                             report->GetCompressedFileName());
                report->Reset();
            }
        }
    }
    //else: user cancelled the report

    delete report;
}

