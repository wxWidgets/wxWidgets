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
#include "wx/app.h"

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

#ifdef __WXGTK__
#include <gtk/gtk.h>
#include <gtk/gtkprivate.h>
#include "wx/gtk/win_gtk.h"
#include "wx/msgdlg.h"
#endif

#if wxUSE_HELP

IMPLEMENT_CLASS(wxHelpControllerBase, wxObject)

/*
 * Invokes context-sensitive help
 */

#ifdef __WXGTK__
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
#endif

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

#ifdef __WXGTK__
wxWindow* wxFindWindowForGdkWindow(wxWindow* win, GdkWindow* gdkWindow)
{
    GdkWindow* thisGdkWindow1 = 0;
    GdkWindow* thisGdkWindow2 = 0;

    if (win->m_wxwindow)
        thisGdkWindow1 = GTK_PIZZA(win->m_wxwindow)->bin_window;

    thisGdkWindow2 = win->m_widget->window;

    if (gdkWindow == thisGdkWindow1 || gdkWindow == thisGdkWindow2)
      return win;

    wxNode* node = win->GetChildren().First();
    while (node)
    {
        wxWindow* child = (wxWindow*) node->Data();
        wxWindow* found = wxFindWindowForGdkWindow(child, gdkWindow);
        if (found)
          return found;

        node = node->Next();
    }
    return NULL;    
}
#endif

bool wxContextHelp::BeginContextHelp(wxWindow* win)
{
    if (!win)
        win = wxTheApp->GetTopWindow();
    if (!win)
        return FALSE;
#ifdef __WXMSW__
    wxCursor cursor(wxCURSOR_QUESTION_ARROW);
    wxSetCursor(cursor);

    win->CaptureMouse();

    EventLoop(cursor, win);

    win->ReleaseMouse();
#endif

#ifdef __WXGTK__
    m_status = FALSE;
    GdkCursor* query_cursor = gdk_cursor_new (GDK_QUESTION_ARROW);

    GdkWindow* gdkWindow = 0;
    GtkWidget* gtkWidget = 0;

    if (win->m_wxwindow)
    {
        gtkWidget = win->m_wxwindow;
        gdkWindow = GTK_PIZZA(win->m_wxwindow)->bin_window;
    }
    else
    {
        gtkWidget = win->m_widget;
        gdkWindow = win->m_widget->window;
    }

    gint failure = gdk_pointer_grab (gdkWindow,
			      FALSE,
			      GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK |
			      GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK,
			      NULL,
			      query_cursor,
			      GDK_CURRENT_TIME);
    if (failure)
    {
        gdk_cursor_destroy (query_cursor);
        query_cursor = NULL;
    }
    gdk_keyboard_grab (gdkWindow, FALSE, GDK_CURRENT_TIME);
    gtk_grab_add (gtkWidget);

    win->PushEventHandler(new wxContextHelpEvtHandler(this));

    //    wxLogDebug("Entering loop.");

    EventLoop(wxNullCursor, win);

    //    wxLogDebug("Exiting loop.");

    win->PopEventHandler(TRUE);

    gtk_grab_remove (gtkWidget);
    gdk_keyboard_ungrab (GDK_CURRENT_TIME);
    if (query_cursor)
    {
        gdk_pointer_ungrab (GDK_CURRENT_TIME);
        gdk_cursor_destroy (query_cursor);
        query_cursor = NULL;
    }

    if (m_status)
      {
	//wxMessageBox("Left-clicked");
        //wxPoint screenPt = win->ClientToScreen(m_mousePos);
        int x, y;
        GdkWindow* windowAtPtr = gdk_window_at_pointer(& x, & y);
        if (windowAtPtr)
	  {
            wxWindow* wxWinAtPtr = wxFindWindowForGdkWindow(win, windowAtPtr);
            if (wxWinAtPtr)
            {
              DispatchEvent(wxWinAtPtr, wxPoint(x, y));
            }
	  }
      }
    else
      {
	//wxMessageBox("Cancelled");
      }
#endif

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
#elif defined(__WXGTK__)
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
#else
    return FALSE;
#endif
}

// PROBLEM: If you click on the panel or other descendant of the
// given window, then it doesn't go to this handler, even though
// there's a grab.
bool wxContextHelpEvtHandler::ProcessEvent(wxEvent& event)
{
  //wxLogDebug("Got event");

    if (event.GetEventType() == wxEVT_LEFT_DOWN)
    {
      //wxLogDebug("Mouse event");
        wxMouseEvent& mouseEvent = (wxMouseEvent&) event;
        m_contextHelp->SetStatus(TRUE, mouseEvent.GetPosition());
        m_contextHelp->EndContextHelp();
    }
    // Don't know why these aren't being caught
    else if (event.GetEventType() == wxEVT_CHAR || event.GetEventType() == wxEVT_KEY_DOWN)
    {
      //wxKeyEvent& keyEvent = (wxKeyEvent&) event;
        if (TRUE) // keyEvent.GetKeyCode() == WXK_ESCAPE)
        {
            m_contextHelp->SetStatus(FALSE, wxPoint(0, 0));
            m_contextHelp->EndContextHelp();
        }
    }

    return TRUE;
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
