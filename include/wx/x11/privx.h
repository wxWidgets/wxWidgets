/////////////////////////////////////////////////////////////////////////////
// Name:        privx.h
// Purpose:     Private declarations common to X11 and Motif ports
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVX_H_
#define _WX_PRIVX_H_

#include "wx/defs.h"
#include "wx/utils.h"
#include "X11/Xlib.h"
#include "X11/Xatom.h"
#include "X11/Xutil.h"

class wxMouseEvent;
class wxKeyEvent;
class wxWindow;

// ----------------------------------------------------------------------------
// key events related functions
// ----------------------------------------------------------------------------

extern int wxCharCodeXToWX(KeySym keySym);
extern KeySym wxCharCodeWXToX(int id);

int wxGetBestMatchingPixel(Display *display, XColor *desiredColor, Colormap cmap);
Pixmap XCreateInsensitivePixmap( Display *display, Pixmap pixmap );

extern XColor g_itemColors[];
extern int wxComputeColours (Display *display, wxColour * back, wxColour * fore);

// For convenience
inline Display* wxGlobalDisplay() { return (Display*) wxGetDisplay(); }

#define wxMAX_RGB           0xff
#define wxMAX_SV            1000
#define wxSIGN(x)           ((x < 0) ? -x : x)
#define wxH_WEIGHT          4
#define wxS_WEIGHT          1
#define wxV_WEIGHT          2

typedef struct wx_hsv {
                        int h,s,v;
                      } wxHSV;

#define wxMax3(x,y,z) ((x > y) ? ((x > z) ? x : z) : ((y > z) ? y : z))
#define wxMin3(x,y,z) ((x < y) ? ((x < z) ? x : z) : ((y < z) ? y : z))

void wxHSVToXColor(wxHSV *hsv,XColor *xcolor);
void wxXColorToHSV(wxHSV *hsv,XColor *xcolor);
void wxAllocNearestColor(Display *display,Colormap colormap,XColor *xcolor);
void wxAllocColor(Display *display,Colormap colormap,XColor *xcolor);

// For debugging
#ifdef __WXDEBUG__
wxString wxGetXEventName(XEvent& event);
#endif

// Is the window visible?
bool wxWindowIsVisible(Window win);

#endif
// _WX_PRIVX_H_
