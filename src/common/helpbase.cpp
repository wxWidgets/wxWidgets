/////////////////////////////////////////////////////////////////////////////
// Name:        helpbase.cpp
// Purpose:     Help system base classes
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "helpbase.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/defs.h"
#endif

#include "wx/helpbase.h"

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

#if wxUSE_HELP

IMPLEMENT_CLASS(wxHelpControllerBase, wxObject)

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

bool wxContextHelp::BeginContextHelp(wxWindow* win)
{
    if (!win)
        win = wxTheApp->GetTopWindow();
    if (!win)
        return FALSE;

    wxCursor cursor(wxCURSOR_QUESTION_ARROW);
    wxSetCursor(cursor);

    win->CaptureMouse();

    EventLoop(cursor, win);

    win->ReleaseMouse();

    return TRUE;
}

bool wxContextHelp::EndContextHelp()
{
    m_inHelp = FALSE;

    return TRUE;
}

bool wxContextHelp::EventLoop(const wxCursor& cursor, wxWindow* win)
{
#ifdef __WXMSW__
    m_inHelp = TRUE;
    while ( m_inHelp )
    {
        MSG msg;
        if (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
        {
            if (!ProcessHelpMessage((WXMSG*) & msg, cursor, win))
            {
                m_inHelp = FALSE;
            }
        }
        else
        {
            wxTheApp->ProcessIdle();
        }
    }
    return TRUE;
#else
    return FALSE;
#endif
}

#ifdef __WXMSW__
bool wxContextHelp::ProcessHelpMessage(WXMSG* wxmsg, const wxCursor& cursor, wxWindow* winInQuestion)
{
    MSG& msg = * (MSG*) wxmsg;

    if (msg.message == WM_KEYDOWN || msg.wParam == VK_ESCAPE)
    {
        PeekMessage(&msg, NULL, msg.message, msg.message, PM_REMOVE);
        return FALSE;
    }

    if (msg.message == WM_CAPTURECHANGED)
    {
        PeekMessage(&msg, NULL, msg.message, msg.message, PM_REMOVE);
        return FALSE;
    }

    if (msg.message == WM_ACTIVATE)
    {
        PeekMessage(&msg, NULL, msg.message, msg.message, PM_REMOVE);
        return FALSE;
    }

	if ((msg.message >= WM_MOUSEFIRST && msg.message <= WM_MOUSELAST))
//        || (msg.message >= WM_NCMOUSEFIRST && msg.message <= WM_NCMOUSELAST))
	{
        wxSetCursor(cursor);

        HWND hWndHit = ::WindowFromPoint(msg.pt);

        wxWindow* win = wxFindWinFromHandle((WXHWND) hWndHit) ;
        HWND hWnd = hWndHit;

        // Try to find a window with a wxWindow associated with it
        while (!win && (hWnd != 0))
        {
            hWnd = ::GetParent(hWnd);
            win = wxFindWinFromHandle((WXHWND) hWnd) ;
        }

        if (win)
        {
            // It's a wxWindows window
			if (msg.message != WM_LBUTTONDOWN)
			{
				// Hit one of our owned windows -- eat the message.
				PeekMessage(&msg, NULL, msg.message, msg.message, PM_REMOVE);
				return TRUE;
            }
			int iHit = (int)::SendMessage(hWndHit, WM_NCHITTEST, 0,
				MAKELONG(msg.pt.x, msg.pt.y));
			if (iHit == HTMENU || iHit == HTSYSMENU)
			{
                // Eat this message, send the event and return
				PeekMessage(&msg, NULL, msg.message, msg.message, PM_REMOVE);
                DispatchEvent(win, wxPoint(msg.pt.x, msg.pt.y));
                return FALSE;
			}
			else if (iHit == HTCLIENT)
			{
				PeekMessage(&msg, NULL, msg.message, msg.message, PM_REMOVE);
                DispatchEvent(win, wxPoint(msg.pt.x, msg.pt.y));
				return FALSE;
			}
			else
			{
				PeekMessage(&msg, NULL, msg.message, msg.message, PM_REMOVE);
				return FALSE;
			}
        }
        else
        {
            // Someone else's message
		    if (PeekMessage(&msg, NULL, msg.message, msg.message, PM_REMOVE))
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
            }
            return TRUE;
        }
    }
    else
    {
        // allow all other messages to go through (capture still set)
        if (PeekMessage(&msg, NULL, msg.message, msg.message, PM_REMOVE))
            DispatchMessage(&msg);
        return TRUE;

    }
    return TRUE;
}
#endif

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


#endif // wxUSE_HELP
