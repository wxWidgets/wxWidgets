/////////////////////////////////////////////////////////////////////////////
// Name:        cshelp.cpp
// Purpose:     Context sensitive help class implementation
// Author:      Julian Smart
// Modified by:
// Created:     08/09/2000
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "cshelp.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/defs.h"
#endif

#include "wx/app.h"

#if wxUSE_HELP

#include "wx/cshelp.h"

/*
 * Invokes context-sensitive help
 */

// This class exists in order to eat events until the left mouse
// button is pressed
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

/*
 * wxContextHelpButton
 * You can add this to your dialogs (especially on non-Windows platforms)
 * to put the application into context help mode.
 */

#if !defined(__WXMSW__)
static char * csquery_xpm[] = {
"12 11 2 1",
" 	c None",
".	c Black",
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

wxContextHelpButton::wxContextHelpButton(wxWindow* parent, wxWindowID id,
        const wxPoint& pos, const wxSize& size,
        long style):
   wxBitmapButton(parent, id, wxNullBitmap, pos, size, style)
{
#ifdef __WXMSW__
    wxBitmap bitmap(wxT("csquery_bmp"), wxBITMAP_TYPE_BMP_RESOURCE);
#else
    wxBitmap bitmap(csquery_xpm);
#endif

    SetBitmapLabel(bitmap);
}

void wxContextHelpButton::OnContextHelp(wxCommandEvent& event)
{
    wxContextHelp contextHelp;
}

#endif // wxUSE_HELP
