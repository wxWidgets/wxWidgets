/////////////////////////////////////////////////////////////////////////////
// Name:        reparent.cpp
// Purpose:     wxWindow
// Author:      Julian Smart
// Modified by:
// Created:     2002-03-09
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "reparent.h"
#endif

#include "wx/setup.h"

#if !wxUSE_NANOX

#include "wx/x11/reparent.h"
#include "wx/evtloop.h"
#include "wx/log.h"
#include "wx/app.h"
#include "wx/timer.h"

#include "wx/x11/private.h"
#include "X11/Xatom.h"

/*

  Adapted from code by Mike Yang, as follows.

From: Mike Yang (mikey@eukanuba.wpd.sgi.com)
Subject: Re: Wrapping new widget around existing windows 
Newsgroups: comp.windows.x
View: Complete Thread (17 articles) | Original Format 
Date: 1991-08-09 09:45:48 PST 
 

Enough people asked, so here's my test program which reparents another
window.  It's a single file (reparent.c), and will work with Motif or
Xaw.  Xaw users should comment out the "#define MOTIF" line.

The reparent program first prompts for the application name of the
client that it will reparent.  If you're going to start the
application override_redirect (e.g. -xrm "*overrideRedirect: true"),
then this name is ignored and the first override_redirect window is
assumed to be the one.

Input focus is supposed to be correctly handled, as is resizing with
window manager hints.  If you have input focus problems, try launching
your application override_redirect instead.  This method is preferred
anyway, since you can map it off-screen and then avoid the "window
flash" effect as the application's top-level window is reparented.

-----------------------------------------------------------------------
                 Mike Yang        Silicon Graphics, Inc.
               mikey@sgi.com           415/335-1786


------------------------------- cut here ------------------------------
*/

/*

Copyright 1991 by Mike Yang, mikey@sgi.com, Silicon Graphics, Inc.

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation, and that the name of SGI not be used in advertising or
publicity pertaining to distribution of the software without specific,
written prior permission.  SGI makes no representations about the
suitability of this software for any purpose.  It is provided "as is"
without express or implied warranty.

*/

/*
 * wxAdoptedWindow
 */

wxAdoptedWindow::wxAdoptedWindow()
{
}

wxAdoptedWindow::wxAdoptedWindow(WXWindow window)
{
    m_mainWindow = window;
}

wxAdoptedWindow::~wxAdoptedWindow()
{
}
/*
 * wxReparenter
 */

static bool Xerror;
static Atom WM_STATE = 0;
bool wxReparenter::sm_done = FALSE;
wxAdoptedWindow* wxReparenter::sm_toReparent = NULL;
wxWindow* wxReparenter::sm_newParent = NULL;
wxString wxReparenter::sm_name;
bool wxReparenter::sm_exactMatch = FALSE;

static int
ErrorHandler(Display* dpy, XErrorEvent* event)
{
  Xerror = True;
  return False;
}

// We assume that toReparent has had its X window set
// appropriately.
bool wxReparenter::Reparent(wxWindow* newParent, wxAdoptedWindow* toReparent)
{
    XWindowAttributes xwa;
    Window *children;
    unsigned int numchildren, each;
    Window returnroot, returnparent;
    XErrorHandler old;
    int parentOffset = 0;

    old = XSetErrorHandler(ErrorHandler);
    XReparentWindow( wxGlobalDisplay(),
                     (Window) toReparent->GetMainWindow(),
                     (Window) newParent->GetMainWindow(),
                     0, 0);

    if (!XQueryTree( wxGlobalDisplay(),
                     (Window) toReparent->GetMainWindow(),
                     &returnroot, &returnparent,
                     &children, &numchildren) || Xerror)
    {
        XSetErrorHandler(old);
        return TRUE;
    }

    if (numchildren > 0)
    {
        // TEST: see if we can get away with reparenting just
        // first one
        if (numchildren > 1)
        {
            wxLogDebug(wxT("Found %d, but only reparenting 1 child."), numchildren);
            numchildren = 1;
        }
        wxLogDebug(wxT("Reparenting %d children."), numchildren);
        /* Stacking order is preserved since XQueryTree returns its children in
           bottommost to topmost order
         */
        for (each=0; each<numchildren; each++)
        {
            XGetWindowAttributes( wxGlobalDisplay(),
                                  children[each], &xwa);
            fprintf(stderr,
              "Reparenting child at offset %d and position %d, %d.\n",
               parentOffset, parentOffset+xwa.x, parentOffset+xwa.y);
            XReparentWindow( wxGlobalDisplay(),
                             children[each], (Window) newParent->GetMainWindow(),
                             xwa.x, xwa.y);
        }
    }

    XSetErrorHandler(old);
    return TRUE;
}

// Wait for an appropriate window to be created.
// If exactMatch is FALSE, a substring match is OK.
// If windowName is empty, then wait for the next overrideRedirect window.
bool wxReparenter::WaitAndReparent(wxWindow* newParent, wxAdoptedWindow* toReparent,
                                   const wxString& windowName,
                                   bool exactMatch)
{
    sm_newParent = newParent;
    sm_toReparent = toReparent;
    sm_exactMatch = exactMatch;
    sm_name = windowName;
    
    Display* display = wxGlobalDisplay();
    XSelectInput(display,
        RootWindowOfScreen(DefaultScreenOfDisplay(display)),
        SubstructureNotifyMask);

    if (!WM_STATE)
        WM_STATE = XInternAtom(display, "WM_STATE", False);

#ifdef __WXDEBUG__
    if (!windowName.IsEmpty())
        wxLogDebug(_T("Waiting for window %s"), windowName.c_str());
#endif
    
    sm_done = FALSE;

    wxEventLoop eventLoop;
    while (!sm_done)
    {
        if (eventLoop.Pending())
        {
            XEvent xevent;
            XNextEvent(display, & xevent);
            if (!wxTheApp->ProcessXEvent((WXEvent*) & xevent))
            {
                // Do the local event processing
                ProcessXEvent((WXEvent*) & xevent);
            }
        }
        else
        {
#if wxUSE_TIMER
            wxTimer::NotifyTimers();
            wxTheApp->SendIdleEvents();
#endif
        }
    }
    return TRUE;
}

bool wxReparenter::ProcessXEvent(WXEvent* event)
{
    XEvent* xevent = (XEvent*) event;
    Window client;

    if (!sm_done)
    {
        if (xevent->type == MapNotify)
        {
            wxLogDebug(_T("Window was mapped"));
        }
        
        if (xevent->type == MapNotify && !xevent->xmap.override_redirect &&
            (client = (Window) FindAClientWindow((WXWindow) xevent->xmap.window, sm_name)))
        {
            wxLogDebug(_T("Found a client window, about to reparent"));
            wxASSERT(sm_toReparent->GetParent() == NULL);
            
            sm_toReparent->SetHandle((WXWindow) client);
            sm_newParent->AddChild(sm_toReparent);
            sm_done = Reparent(sm_newParent, sm_toReparent);
            return sm_done;
        } else if (xevent->type == MapNotify &&
                   xevent->xmap.override_redirect &&
                   xevent->xmap.window)
        {
            wxLogDebug(_T("Found an override redirect window, about to reparent"));
            sm_toReparent->SetHandle((WXWindow) xevent->xmap.window);
            sm_newParent->AddChild(sm_toReparent);
            wxASSERT(sm_toReparent->GetParent() == NULL);
            
            sm_done = Reparent(sm_newParent, sm_toReparent);
            return sm_done;
        }
    }
    return FALSE;
}

WXWindow wxReparenter::FindAClientWindow(WXWindow window, const wxString& name)
{
  int rvalue, i;
  Atom actualtype;
  int actualformat;
  unsigned long nitems, bytesafter;
  unsigned char *propreturn;
  Window *children;
  unsigned int numchildren;
  Window returnroot, returnparent;
  Window result = 0;
  XErrorHandler old;
  char *clientName;

  Xerror = False;
  old = XSetErrorHandler(ErrorHandler);
  rvalue = XGetWindowProperty((Display*) wxGetDisplay(),
                              (Window) window, WM_STATE,
                              0, 1, False,
                              AnyPropertyType, &actualtype, &actualformat,
                              &nitems, &bytesafter, &propreturn);
  XSetErrorHandler(old);
  if (!Xerror && rvalue == Success && actualtype != None)
  {
      if (rvalue == Success)
      {
          XFree((char *) propreturn);
      }
      XFetchName((Display*) wxGetDisplay(), (Window) window, &clientName);

      wxString str1(name);
      wxString str2 = wxString::FromAscii(clientName);
      str1.Lower();
      str2.Lower();

      bool matches;
      if (sm_exactMatch)
          matches = (name == wxString::FromAscii(clientName));
      else
          matches = (str1.Contains(str2) || str2.Contains(str1));
      
      XFree(clientName);

      if (matches)
          return (WXWindow) window;
      else
          return NULL;
  }

  old = XSetErrorHandler(ErrorHandler);
  if (!XQueryTree((Display*) wxGetDisplay(), (Window) window, &returnroot, &returnparent,
    &children, &numchildren) || Xerror)
  {
    XSetErrorHandler(old);
    return NULL;
  }
  XSetErrorHandler(old);

  result = 0;
  for (i=0; i<(int)numchildren && !result ;i++) {
    result = (Window) FindAClientWindow((WXWindow) children[i], name);
  }
  if (numchildren) {
    XFree((char *) children);
  } return (WXWindow) result;
}

#endif
