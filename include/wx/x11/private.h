/////////////////////////////////////////////////////////////////////////////
// Name:        private.h
// Purpose:     Private declarations for X11 port
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_H_
#define _WX_PRIVATE_H_

#include "wx/defs.h"
#include "wx/utils.h"
#include "X11/Xlib.h"
#include "X11/Xatom.h"
#include "X11/Xutil.h"

// Include common declarations
#include "wx/x11/privx.h"

class wxMouseEvent;
class wxKeyEvent;
class wxWindow;

// ----------------------------------------------------------------------------
// we maintain a hash table which contains the mapping from Widget to wxWindow
// corresponding to the window for this widget
// ----------------------------------------------------------------------------

extern void wxDeleteWindowFromTable(Window w);
extern wxWindow *wxGetWindowFromTable(Window w);
extern bool wxAddWindowToTable(Window w, wxWindow *win);

// ----------------------------------------------------------------------------
// TranslateXXXEvent() functions - translate X event to wxWindow one
// ----------------------------------------------------------------------------
extern bool wxTranslateMouseEvent(wxMouseEvent& wxevent, wxWindow *win, Window window, XEvent *xevent);
extern bool wxTranslateKeyEvent(wxKeyEvent& wxevent, wxWindow *win, Window window, XEvent *xevent);

extern Window wxGetWindowParent(Window window);

// Set the window manager decorations according to the
// given wxWindows style
bool wxSetWMDecorations(Window w, long style);
bool wxMWMIsRunning(Window w);

#endif
// _WX_PRIVATE_H_
