/////////////////////////////////////////////////////////////////////////////
// Name:        wxconfigtool.cpp
// Purpose:     Generic application class
// Author:      Julian Smart
// Modified by:
// Created:     2002-09-04
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "wxconfigtool.h"
#endif

#include "wx/wx.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/config.h"
#include "wx/laywin.h"
#include "wx/image.h"
#include "wx/menuitem.h"
#include "wx/tooltip.h"
#include "wx/cshelp.h"
#include "wx/html/htmprint.h"
#include "wx/html/htmlwin.h"
#include "wx/filesys.h"
#include "wx/fs_mem.h"
#include "wx/fs_zip.h"
#include "wx/wfstream.h"
#include "wx/variant.h"

#include "wxconfigtool.h"
#include "configtooldoc.h"
#include "configtoolview.h"
#include "mainframe.h"
#include "utils.h"

IMPLEMENT_APP(ctApp)

BEGIN_EVENT_TABLE(ctApp, wxApp)
END_EVENT_TABLE()

ctApp::ctApp()
{
    m_helpController = NULL;
    m_helpControllerReference = NULL;
    m_docManager = NULL;
}

ctApp::~ctApp()
{
}

bool ctApp::OnInit(void)
{
    wxLog::SetTimestamp(NULL);
    
    wxHelpProvider::Set(new wxSimpleHelpProvider);

#if wxUSE_LIBPNG
    wxImage::AddHandler( new wxPNGHandler );
#endif
    
#if wxUSE_LIBJPEG
    wxImage::AddHandler( new wxJPEGHandler );
#endif
    
#if wxUSE_GIF
    wxImage::AddHandler( new wxGIFHandler );
#endif

#ifdef __WXMSW__
    m_helpController = new wxCHMHelpController;
    m_helpControllerReference = new wxCHMHelpController;
#else
    m_helpController = new wxHtmlHelpController;
    m_helpControllerReference = new wxHtmlHelpController;
#endif

    // Required for HTML help
#if wxUSE_STREAMS && wxUSE_ZIPSTREAM && wxUSE_ZLIB
    wxFileSystem::AddHandler(new wxZipFSHandler);
#endif

    wxString currentDir = wxGetCwd();

    // Use argv to get current app directory
    m_appDir = apFindAppPath(argv[0], currentDir, wxT("WXCONFIGTOOLDIR"));

#ifdef __WXMSW__
    // If the development version, go up a directory.
    if ((m_appDir.Right(5).CmpNoCase("DEBUG") == 0) ||
        (m_appDir.Right(11).CmpNoCase("DEBUGSTABLE") == 0) ||
        (m_appDir.Right(7).CmpNoCase("RELEASE") == 0) ||
        (m_appDir.Right(13).CmpNoCase("RELEASESTABLE") == 0) ||
        (m_appDir.Right(10).CmpNoCase("RELEASEDEV") == 0) ||
        (m_appDir.Right(8).CmpNoCase("DEBUGDEV") == 0)
        )
        m_appDir = wxPathOnly(m_appDir);
#endif

    m_docManager = new wxDocManager;
    m_docManager->SetMaxDocsOpen(1);

    //// Create a template relating documents to their views
    (void) new wxDocTemplate(m_docManager, wxGetApp().GetSettings().GetShortAppName(), wxT("*.wxs"), wxT(""), wxT("wxs"), wxT("Doc"), wxT("View"),
        CLASSINFO(ctConfigToolDoc), CLASSINFO(ctConfigToolView));

    m_settings.Init();

    LoadConfig();
 
    wxString helpFilePathReference(GetFullAppPath(_("wx")));
    m_helpControllerReference->Initialize(helpFilePathReference);
    
    wxString helpFilePath(GetFullAppPath(_("configtool")));
    m_helpController->Initialize(helpFilePath);
    
    ctMainFrame* frame = new ctMainFrame(m_docManager, NULL, -1, wxGetApp().GetSettings().GetAppName(),
        GetSettings().m_frameSize.GetPosition(), GetSettings().m_frameSize.GetSize(),
        wxDEFAULT_FRAME_STYLE);
    SetTopWindow(frame);

    switch (wxGetApp().GetSettings().m_frameStatus)
    {
    case ctSHOW_STATUS_MAXIMIZED:
        {
            frame->Maximize(TRUE);
            break;
        }
    case ctSHOW_STATUS_MINIMIZED:
        {
            frame->Iconize(TRUE);
            break;
        }
    default:
    case ctSHOW_STATUS_NORMAL:
        {
            break;
        }
    }

    // Load the file history. This has to be done AFTER the
    // main frame has been created, so that the items
    // will get appended to the file menu.
    {
        wxConfig config(wxGetApp().GetSettings().GetAppName(), wxT("Generic Organisation"));
        GetFileHistory().Load(config);
    }

    if (argc > 1)
    {
        // Concatenate strings since it could have spaces (and quotes)
        wxString arg;
        int i;
        for (i = 1; i < argc; i++)
        {
            arg += argv[i];
            if (i < (argc - 1))
                arg += wxString(wxT(" "));
        }
        if (arg[0u] == '"')
            arg = arg.Mid(1);
        if (arg.Last() == '"')
            arg = arg.Mid(0, arg.Len() - 1);

        // Load the file
        wxDocument* doc = m_docManager->CreateDocument(arg, wxDOC_SILENT);
        if (doc)
            doc->SetDocumentSaved(TRUE);
    }
    else
    {
        if (GetSettings().m_loadLastDocument)
        {
            // Load the file that was last loaded
            wxDocument* doc = m_docManager->CreateDocument(GetSettings().m_lastFilename, wxDOC_SILENT);
            if (doc)
                doc->SetDocumentSaved(TRUE);
        }
    }
    
    GetTopWindow()->Show(TRUE);
    
    return TRUE;
}

int ctApp::OnExit(void)
{
    SaveConfig();
    
    // Save the file history
    {
        wxConfig config(wxGetApp().GetSettings().GetAppName(), wxT("Generic Organisation"));
        GetFileHistory().Save(config);
    }

    delete m_docManager;
    m_docManager = NULL;
    
    return 0;
}

void ctApp::ClearHelpControllers()
{
    delete m_helpController;
    m_helpController = NULL;
        
    delete m_helpControllerReference;
    m_helpControllerReference = NULL;
}


// Prepend the current program directory to the name
wxString ctApp::GetFullAppPath(const wxString& filename) const
{
    wxString path(m_appDir);
    if (path.Last() != wxFILE_SEP_PATH && filename[0] != wxFILE_SEP_PATH)
        path += wxFILE_SEP_PATH;
    path += filename;
    
    return path;
}

// Load config info
bool ctApp::LoadConfig()
{
    return m_settings.LoadConfig();
}

// Save config info
bool ctApp::SaveConfig()
{
    return m_settings.SaveConfig();
}

bool ctApp::UsingTooltips()
{
    return GetSettings().m_useToolTips;
}

/// Returns the main frame
ctMainFrame* ctApp::GetMainFrame()
{
    return wxDynamicCast(wxTheApp->GetTopWindow(), ctMainFrame);
}
