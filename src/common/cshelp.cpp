/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/cshelp.cpp
// Purpose:     Context sensitive help class implementation
// Author:      Julian Smart, Vadim Zeitlin
// Modified by:
// Created:     08/09/2000
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Julian Smart, Vadim Zeitlin
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

#ifdef __GNUG__
    #pragma implementation "cshelp.h"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_HELP

#ifndef WX_PRECOMP
#endif

#include "wx/tipwin.h"
#include "wx/app.h"

#include "wx/cshelp.h"

// ----------------------------------------------------------------------------
// wxContextHelpEvtHandler private class
// ----------------------------------------------------------------------------

// This class exists in order to eat events until the left mouse button is
// pressed
class wxContextHelpEvtHandler: public wxEvtHandler
{
public:
    wxContextHelpEvtHandler(wxContextHelp* contextHelp)
    {
        m_contextHelp = contextHelp;
    }

    virtual bool ProcessEvent(wxEvent& event);

//// Data
    wxContextHelp* m_contextHelp;
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxContextHelp
// ----------------------------------------------------------------------------

/*
 * Invokes context-sensitive help
 */


IMPLEMENT_DYNAMIC_CLASS(wxContextHelp, wxObject)

wxContextHelp::wxContextHelp(wxWindow* win, bool beginHelp)
{
    m_inHelp = FALSE;

    if (beginHelp)
        BeginContextHelp(win);
}

wxContextHelp::~wxContextHelp()
{
    if (m_inHelp)
        EndContextHelp();
}

// Begin 'context help mode'
bool wxContextHelp::BeginContextHelp(wxWindow* win)
{
    if (!win)
        win = wxTheApp->GetTopWindow();
    if (!win)
        return FALSE;

    wxCursor cursor(wxCURSOR_QUESTION_ARROW);
    wxCursor oldCursor = win->GetCursor();
    win->SetCursor(cursor);

#ifdef __WXMSW__
    //    wxSetCursor(cursor);
#endif

    win->PushEventHandler(new wxContextHelpEvtHandler(this));

    win->CaptureMouse();

    EventLoop();

    win->ReleaseMouse();

    win->PopEventHandler(TRUE);

    win->SetCursor(oldCursor);

    if (m_status)
    {
        wxPoint pt;
        wxWindow* winAtPtr = wxFindWindowAtPointer(pt);
        if (winAtPtr)
            DispatchEvent(winAtPtr, pt);
    }

    return TRUE;
}

bool wxContextHelp::EndContextHelp()
{
    m_inHelp = FALSE;

    return TRUE;
}

bool wxContextHelp::EventLoop()
{
    m_inHelp = TRUE;
    while ( m_inHelp )
    {
        if (wxTheApp->Pending())
        {
            wxTheApp->Dispatch();
        }
        else
        {
            wxTheApp->ProcessIdle();
        }
    }
    return TRUE;
}

bool wxContextHelpEvtHandler::ProcessEvent(wxEvent& event)
{
    switch (event.GetEventType())
    {
        case wxEVT_LEFT_DOWN:
        {
	  //wxMouseEvent& mouseEvent = (wxMouseEvent&) event;
            m_contextHelp->SetStatus(TRUE);
            m_contextHelp->EndContextHelp();
            return TRUE;
            break;
        }
        case wxEVT_CHAR:
        case wxEVT_KEY_DOWN:
        case wxEVT_ACTIVATE:
        case wxEVT_MOUSE_CAPTURE_CHANGED:
        {
            m_contextHelp->SetStatus(FALSE);
            m_contextHelp->EndContextHelp();
            return TRUE;
            break;
        }
        case wxEVT_PAINT:
        case wxEVT_ERASE_BACKGROUND:
        {
            event.Skip();
            return FALSE;
            break;
        }
    }

    return TRUE;
}

// Dispatch the help event to the relevant window
bool wxContextHelp::DispatchEvent(wxWindow* win, const wxPoint& pt)
{
    wxWindow* subjectOfHelp = win;
    bool eventProcessed = FALSE;
    while (subjectOfHelp && !eventProcessed)
    {
        wxHelpEvent helpEvent(wxEVT_HELP, subjectOfHelp->GetId(), pt) ;
        helpEvent.SetEventObject(this);
        eventProcessed = win->GetEventHandler()->ProcessEvent(helpEvent);
        
        // Go up the window hierarchy until the event is handled (or not).
        // I.e. keep submitting ancestor windows until one is recognised
        // by the app code that processes the ids and displays help.
        subjectOfHelp = subjectOfHelp->GetParent();
    }
    return eventProcessed;
}

// ----------------------------------------------------------------------------
// wxContextHelpButton
// ----------------------------------------------------------------------------

/*
 * wxContextHelpButton
 * You can add this to your dialogs (especially on non-Windows platforms)
 * to put the application into context help mode.
 */

#if !defined(__WXMSW__)
static char * csquery_xpm[] = {
"12 11 2 1",
"  c None",
". c #000000",
"            ",
"    ....    ",
"   ..  ..   ",
"   ..  ..   ",
"      ..    ",
"     ..     ",
"     ..     ",
"            ",
"     ..     ",
"     ..     ",
"            "};
#endif

IMPLEMENT_CLASS(wxContextHelpButton, wxBitmapButton)

BEGIN_EVENT_TABLE(wxContextHelpButton, wxBitmapButton)
    EVT_BUTTON(wxID_CONTEXT_HELP, wxContextHelpButton::OnContextHelp)
END_EVENT_TABLE()

wxContextHelpButton::wxContextHelpButton(wxWindow* parent,
                                         wxWindowID id,
                                         const wxPoint& pos,
                                         const wxSize& size,
                                         long style)
                   : wxBitmapButton(parent, id, wxBITMAP(csquery),
                                    pos, size, style)
{
}

void wxContextHelpButton::OnContextHelp(wxCommandEvent& event)
{
    wxContextHelp contextHelp(GetParent());
}

// ----------------------------------------------------------------------------
// wxHelpProvider
// ----------------------------------------------------------------------------

wxHelpProvider *wxHelpProvider::ms_helpProvider = (wxHelpProvider *)NULL;

// trivial implementation of some methods which we don't want to make pure
// virtual for convenience

void wxHelpProvider::AddHelp(wxWindowBase * WXUNUSED(window),
                             const wxString& WXUNUSED(text))
{
}

void wxHelpProvider::AddHelp(wxWindowID WXUNUSED(id),
                             const wxString& WXUNUSED(text))
{
}

wxHelpProvider::~wxHelpProvider()
{
}

// ----------------------------------------------------------------------------
// wxSimpleHelpProvider
// ----------------------------------------------------------------------------

wxString wxSimpleHelpProvider::GetHelp(const wxWindowBase *window)
{
    bool wasFound;
    wxString text = m_hashWindows.Get((long)window, &wasFound);
    if ( !wasFound )
        text = m_hashIds.Get(window->GetId());

    return text;
}

void wxSimpleHelpProvider::AddHelp(wxWindowBase *window, const wxString& text)
{
    m_hashWindows.Put((long)window, text);
}

void wxSimpleHelpProvider::AddHelp(wxWindowID id, const wxString& text)
{
    m_hashIds.Put(id, text);
}

bool wxSimpleHelpProvider::ShowHelp(wxWindowBase *window)
{
    wxString text = GetHelp(window);
    if ( !text.empty() )
    {
        new wxTipWindow((wxWindow *)window, text);

        return TRUE;
    }

    return FALSE;
}

#endif // wxUSE_HELP
