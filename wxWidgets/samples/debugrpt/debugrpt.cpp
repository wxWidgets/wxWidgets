///////////////////////////////////////////////////////////////////////////////
// Name:        debugrpt.cpp
// Purpose:     minimal sample showing wxDebugReport and related classes
// Author:      Vadim Zeitlin
// Modified by:
// Created:     2005-01-20
// RCS-ID:      $Id$
// Copyright:   (c) 2005 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/app.h"
#include "wx/log.h"
#include "wx/frame.h"
#include "wx/icon.h"
#include "wx/menu.h"
#include "wx/msgdlg.h"
#include "wx/button.h"
#include "wx/dcclient.h"

#include "wx/datetime.h"
#include "wx/ffile.h"
#include "wx/filename.h"
#include "wx/debugrpt.h"

#if !wxUSE_DEBUGREPORT
    #error "This sample can't be built without wxUSE_DEBUGREPORT"
#endif // wxUSE_DEBUGREPORT

#if !wxUSE_ON_FATAL_EXCEPTION
    #error "This sample can't be built without wxUSE_ON_FATAL_EXCEPTION"
#endif // wxUSE_ON_FATAL_EXCEPTION

#if !defined(__WXMSW__) && !defined(__WXPM__)
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
                        _T("http://your.url.here/"),
                        _T("report:file"),
                        _T("action")
                       )
    {
    }

protected:
    // this is called with the contents of the server response: you will
    // probably want to parse the XML document in OnServerReply() instead of
    // just dumping it as I do
    virtual bool OnServerReply(const wxArrayString& reply)
    {
        if ( reply.IsEmpty() )
        {
            wxLogError(_T("Didn't receive the expected server reply."));
            return false;
        }

        wxString s(_T("Server replied:\n"));

        const size_t count = reply.GetCount();
        for ( size_t n = 0; n < count; n++ )
        {
            s << _T('\t') << reply[n] << _T('\n');
        }

        wxLogMessage(_T("%s"), s.c_str());

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
        wxMailMessage msg(GetReportName() + _T(" crash report"),
                          _T("vadim@wxwindows.org"),
                          wxEmptyString, // mail body
                          wxEmptyString, // from address
                          GetCompressedFileName(),
                          _T("crashreport.zip"));

        return wxEmail::Send(msg);
    }
};

#endif // 0

// ----------------------------------------------------------------------------
// helper functions
// ----------------------------------------------------------------------------

// just some functions to get a slightly deeper stack trace
static void bar(const wxChar *p)
{
    char *pc = 0;
    *pc = *p;

    printf("bar: %s\n", p);
}

void baz(const wxString& s)
{
    printf("baz: %s\n", s.c_str());
}

void foo(int n)
{
    if ( n % 2 )
        baz(wxT("odd"));
    else
        bar(wxT("even"));
}

// ----------------------------------------------------------------------------
// main window
// ----------------------------------------------------------------------------

enum
{
    DebugRpt_Quit = wxID_EXIT,
    DebugRpt_Crash = 100,
    DebugRpt_Current,
    DebugRpt_Paint,
    DebugRpt_Upload,
    DebugRpt_About = wxID_ABOUT
};

class MyFrame : public wxFrame
{
public:
    MyFrame();

private:
    void OnQuit(wxCommandEvent& event);
    void OnReportForCrash(wxCommandEvent& event);
    void OnReportForCurrent(wxCommandEvent& event);
    void OnReportPaint(wxCommandEvent& event);
    void OnReportUpload(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnPaint(wxPaintEvent& event);


    // number of lines drawn in OnPaint()
    int m_numLines;

    DECLARE_NO_COPY_CLASS(MyFrame)
    DECLARE_EVENT_TABLE()
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
    virtual bool OnInit();

    // called when a crash occurs in this application
    virtual void OnFatalException();

    // this is where we really generate the debug report
    void GenerateReport(wxDebugReport::Context ctx);

    // if this function is called, we'll use MyDebugReport which would try to
    // upload the (next) generated debug report to its URL, otherwise we just
    // generate the debug report and leave it in a local file
    void UploadReport(bool doIt) { m_uploadReport = doIt; }

private:
    bool m_uploadReport;

    DECLARE_NO_COPY_CLASS(MyApp)
};

IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(DebugRpt_Quit, MyFrame::OnQuit)
    EVT_MENU(DebugRpt_Crash, MyFrame::OnReportForCrash)
    EVT_MENU(DebugRpt_Current, MyFrame::OnReportForCurrent)
    EVT_MENU(DebugRpt_Paint, MyFrame::OnReportPaint)
    EVT_MENU(DebugRpt_Upload, MyFrame::OnReportUpload)
    EVT_MENU(DebugRpt_About, MyFrame::OnAbout)

    EVT_PAINT(MyFrame::OnPaint)
END_EVENT_TABLE()

MyFrame::MyFrame()
       : wxFrame(NULL, wxID_ANY, _T("wxWidgets Debug Report Sample"))
{
    m_numLines = 10;

    SetIcon(wxICON(sample));

    wxMenu *menuFile = new wxMenu;
    menuFile->Append(DebugRpt_Quit, _T("E&xit\tAlt-X"));

    wxMenu *menuReport = new wxMenu;
    menuReport->Append(DebugRpt_Crash, _T("Report for &crash\tCtrl-C"),
                       _T("Provoke a crash inside the program and create report for it"));
    menuReport->Append(DebugRpt_Current, _T("Report for c&urrent context\tCtrl-U"),
                       _T("Create report for the current program context"));
    menuReport->Append(DebugRpt_Paint, _T("Report for &paint handler\tCtrl-P"),
                       _T("Provoke a repeatable crash in wxEVT_PAINT handler"));
    menuReport->AppendSeparator();
    menuReport->AppendCheckItem(DebugRpt_Upload, _T("Up&load debug report"),
                       _T("You need to configure a web server accepting debug report uploads to use this function"));

    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(DebugRpt_About, _T("&About...\tF1"));

    wxMenuBar *mbar = new wxMenuBar();
    mbar->Append(menuFile, _T("&File"));
    mbar->Append(menuReport, _T("&Report"));
    mbar->Append(menuHelp, _T("&Help"));
    SetMenuBar(mbar);

    CreateStatusBar();

    Show();
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

void MyFrame::OnReportUpload(wxCommandEvent& event)
{
    wxGetApp().UploadReport(event.IsChecked());
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox
    (
        _T("wxDebugReport sample\n(c) 2005 Vadim Zeitlin <vadim@wxwindows.org>"),
        _T("wxWidgets Debug Report Sample"),
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
    // user needs to explicitely enable this
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
    wxFileName fn(report->GetDirectory(), _T("timestamp.my"));
    wxFFile file(fn.GetFullPath(), _T("w"));
    if ( file.IsOpened() )
    {
        wxDateTime dt = wxDateTime::Now();
        file.Write(dt.FormatISODate() + _T(' ') + dt.FormatISOTime());
        file.Close();
    }

    report->AddFile(fn.GetFullName(), _T("timestamp of this report"));

    // can also add an existing file directly, it will be copied
    // automatically
#ifdef __WXMSW__
    report->AddFile(_T("c:\\autoexec.bat"), _T("DOS startup file"));
#else
    report->AddFile(_T("/etc/motd"), _T("Message of the day"));
#endif

    // calling Show() is not mandatory, but is more polite
    if ( wxDebugReportPreviewStd().Show(*report) )
    {
        if ( report->Process() )
        {
            if ( m_uploadReport )
            {
                wxLogMessage(_T("Report successfully uploaded."));
            }
            else
            {
                wxLogMessage(_T("Report generated in \"%s\"."),
                             report->GetCompressedFileName().c_str());
                report->Reset();
            }
        }
    }
    //else: user cancelled the report

    delete report;
}

