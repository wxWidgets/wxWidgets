/////////////////////////////////////////////////////////////////////////////
// Name:        helpview.cpp
// Purpose:     wxHtml help browser
// Please note: see utils/helpview for a more fully-featured
// standalone help browser.
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/image.h"
#include "wx/wxhtml.h"
#include "wx/fs_zip.h"
#include "wx/log.h"
#include "wx/filedlg.h"

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
        virtual int OnExit();

        /// Respond to idle event
        void OnIdle(wxIdleEvent& event);

private:
        bool                    m_exitIfNoMainWindow;
        wxHtmlHelpController *  help;
DECLARE_EVENT_TABLE()
};


IMPLEMENT_APP(MyApp)

BEGIN_EVENT_TABLE(MyApp, wxApp)
    EVT_IDLE(MyApp::OnIdle)
END_EVENT_TABLE()

bool MyApp::OnInit()
{
    m_exitIfNoMainWindow = false;
#ifdef __WXMOTIF__
    delete wxLog::SetActiveTarget(new wxLogStderr); // So dialog boxes aren't used
#endif

    // Don't exit on frame deletion, since the help window is programmed
    // to cause the app to exit even if it is still open. We need to have the app
    // close by other means.
    SetExitOnFrameDelete(false);

    wxInitAllImageHandlers();
    wxFileSystem::AddHandler(new wxZipFSHandler);

    SetVendorName(wxT("wxWidgets"));
    SetAppName(wxT("wxHTMLHelp")); 
    wxConfig::Get(); // create an instance

    help = new wxHtmlHelpController(wxHF_DEFAULT_STYLE|wxHF_OPEN_FILES);

    if (argc < 2) {
        wxLogError(wxT("Usage : helpview <helpfile> [<more helpfiles>]"));
        wxLogError(wxT("  helpfile may be .hhp, .zip or .htb"));
        return false;
    }

    for (int i = 1; i < argc; i++)
        help->AddBook(wxFileName(argv[i]));

#ifdef __WXMOTIF__
    delete wxLog::SetActiveTarget(new wxLogGui);
#endif

    help -> DisplayContents();
    SetTopWindow(help->GetFrame());
    m_exitIfNoMainWindow = true;

    return true;
}

void MyApp::OnIdle(wxIdleEvent& event)
{
    if (m_exitIfNoMainWindow && !GetTopWindow())
        ExitMainLoop();

    event.Skip();
    event.RequestMore();
}

int MyApp::OnExit()
{
    delete help;
    delete wxConfig::Set(NULL);

    return 0;
}

