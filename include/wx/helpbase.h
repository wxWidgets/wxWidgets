/////////////////////////////////////////////////////////////////////////////
// Name:        helpbase.h
// Purpose:     Help system base classes
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_HELPBASEH__
#define _WX_HELPBASEH__

#ifdef __GNUG__
#pragma interface "helpbase.h"
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/string.h"
#include "wx/gdicmn.h"
#include "wx/frame.h"

#if wxUSE_HELP

// Flags for SetViewer
#define wxHELP_NETSCAPE     1

// Defines the API for help controllers
class WXDLLEXPORT wxHelpControllerBase: public wxObject
{
    DECLARE_CLASS(wxHelpControllerBase)
        
public:
    inline wxHelpControllerBase() {}
    inline ~wxHelpControllerBase() {};
    
    // Must call this to set the filename and server name.
    // server is only required when implementing TCP/IP-based
    // help controllers.
    virtual bool Initialize(const wxString& WXUNUSED(file), int WXUNUSED(server) ) { return FALSE; }
    virtual bool Initialize(const wxString& WXUNUSED(file)) { return FALSE; }
    
    // Set viewer: only relevant to some kinds of controller
    virtual void SetViewer(const wxString& WXUNUSED(viewer), long WXUNUSED(flags) = 0) {}
    
    // If file is "", reloads file given  in Initialize
    virtual bool LoadFile(const wxString& file = "") = 0;
    virtual bool DisplayContents(void) = 0;
    virtual bool DisplaySection(int sectionNo) = 0;

    // By default, uses KeywordSection to display a topic. Implementations
    // may override this for more specific behaviour.
    virtual bool DisplaySection(const wxString& section) { return KeywordSearch(section); };
    virtual bool DisplayBlock(long blockNo) = 0;
    virtual bool KeywordSearch(const wxString& k) = 0;
    /// Allows one to override the default settings for the help frame.
    virtual void SetFrameParameters(const wxString& WXUNUSED(title),
        const wxSize& WXUNUSED(size),
        const wxPoint& WXUNUSED(pos) = wxDefaultPosition,
        bool WXUNUSED(newFrameEachTime) = FALSE)
    {
        // does nothing by default
    }
    /// Obtains the latest settings used by the help frame and the help 
    /// frame.
    virtual wxFrame *GetFrameParameters(wxSize *WXUNUSED(size) = NULL,
        wxPoint *WXUNUSED(pos) = NULL,
        bool *WXUNUSED(newFrameEachTime) = NULL)
    {
        return (wxFrame*) NULL;// does nothing by default
    }
    
    virtual bool Quit(void) = 0;
    virtual void OnQuit(void) {};
};

/*
 * wxContextHelp
 * Invokes context-sensitive help. When the user
 * clicks on a window, a wxEVT_HELP event will be sent to that
 * window for the application to display help for.
 */

class WXDLLEXPORT wxContextHelp: public wxObject
{
    DECLARE_DYNAMIC_CLASS(wxContextHelp)
public:
    wxContextHelp(wxWindow* win = NULL, bool beginHelp = TRUE);
    ~wxContextHelp();

    bool BeginContextHelp(wxWindow* win);
    bool EndContextHelp();

    bool EventLoop(const wxCursor& cursor, wxWindow* win);
    bool DispatchEvent(wxWindow* win, const wxPoint& pt);

#ifdef __WXMSW__
    bool ProcessHelpMessage(WXMSG* wxmsg, const wxCursor& cursor, wxWindow* win);
#endif

protected:

    bool    m_inHelp;
};

#endif // wxUSE_HELP
#endif
// _WX_HELPBASEH__
