/////////////////////////////////////////////////////////////////////////////
// Name:        wx/unix/utilsx11.h
// Purpose:     Miscellaneous X11 functions
// Author:      Mattia Barbon
// Modified by:
// Created:     25.03.02
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIX_UTILSX11_H_
#define _WX_UNIX_UTILSX11_H_

#include "wx/defs.h"

#if defined(__WXMOTIF__) || defined(__WXGTK__) || defined(__WXX11__)

#if defined(__WXGTK__)
typedef void WXDisplay;
typedef void* WXWindow;
#endif

class wxIconBundle;

void wxSetIconsX11( WXDisplay* display, WXWindow window,
                    const wxIconBundle& ib );

#endif
    // __WXMOTIF__, __WXGTK__, __WXX11__

#endif
    // _WX_UNIX_UTILSX11_H_
