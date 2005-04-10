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
#include "wx/datetime.h"
#include "wx/ffile.h"
#include "wx/filename.h"
#include "wx/dynlib.h"
#include "wx/debugrpt.h"

#include "wx/msgdlg.h"

#if !wxUSE_DEBUGREPORT
    #error "This sample can't be built without wxUSE_DEBUGREPORT"
#endif // wxUSE_DEBUGREPORT

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
// application class
// ----------------------------------------------------------------------------

// this is a usual application class modified to work with debug reporter
//
// basically just 2 things are necessary: call wxHandleFatalExceptions() as
// early as possible and override OnFatalException() to create the report there
class MyApp : public wxApp
{
public:
    virtual bool OnInit()
    {
        wxHandleFatalExceptions();

        if ( !wxApp::OnInit() )
            return false;

        return true;
    }

    virtual int OnRun()
    {
        // a real program would be presumably be a bit harder to crash than
        // just pressing "yes" in a dialog... but this is just an example
        switch ( wxMessageBox
                 (
                    _T("Generate report for crash (or just current context)?"),
                    _T("wxDebugReport Test"),
                    wxYES_NO | wxCANCEL
                 ) )
        {
            case wxYES:
                // this call is going to crash
                foo(32);
                foo(17);
                break;

            case wxNO:
                // example of manually generated report, this could be also
                // used in wxApp::OnAssert()
                GenerateReport(wxDebugReport::Context_Current);
                break;

            case wxCANCEL:
                break;
        }

        return 0;
    }

    virtual void OnFatalException()
    {
        GenerateReport(wxDebugReport::Context_Exception);
    }

    void GenerateReport(wxDebugReport::Context ctx)
    {
        MyDebugReport report;

        // add all standard files: currently this means just a minidump and an
        // XML file with system info and stack trace
        report.AddAll(ctx);

        // you can also call report.AddFile(...) with your own log files, files
        // created using wxRegKey::Export() and so on, here we just add a test
        // file containing the date of the crash
        wxFileName fn(report.GetDirectory(), _T("timestamp.my"));
        wxFFile file(fn.GetFullPath(), _T("w"));
        if ( file.IsOpened() )
        {
            wxDateTime dt = wxDateTime::Now();
            file.Write(dt.FormatISODate() + _T(' ') + dt.FormatISOTime());
            file.Close();
        }

        report.AddFile(fn.GetFullName(), _T("timestamp of this report"));

        // can also add an existing file directly, it will be copied
        // automatically
#ifdef __WXMSW__
        report.AddFile(_T("c:\\autoexec.bat"), _T("DOS startup file"));
#else
        report.AddFile(_T("/etc/motd"), _T("Message of the day"));
#endif

        // calling Show() is not mandatory, but is more polite
        if ( wxDebugReportPreviewStd().Show(report) )
        {
            if ( report.Process() )
            {
                // report successfully uploaded
            }
        }
        //else: user cancelled the report
    }
};

IMPLEMENT_APP(MyApp)

