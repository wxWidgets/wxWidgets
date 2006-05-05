/////////////////////////////////////////////////////////////////////////////
// Name:        src/x11/dcscreen.cpp
// Purpose:     wxScreenDC class
// Author:      Julian Smart, Robert Roebling
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/dcscreen.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/utils.h"
    #include "wx/window.h"
    #include "wx/frame.h"
#endif

#include "wx/fontutil.h"

#include "wx/x11/private.h"

//-----------------------------------------------------------------------------
// global data initialization
//-----------------------------------------------------------------------------

WXWindow *wxScreenDC::sm_overlayWindow  = (WXWindow*) NULL;
int wxScreenDC::sm_overlayWindowX = 0;
int wxScreenDC::sm_overlayWindowY = 0;

//-----------------------------------------------------------------------------
// wxScreenDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxScreenDC,wxPaintDC)

wxScreenDC::wxScreenDC()
{
    m_ok = false;

    m_display = (WXDisplay *) wxGlobalDisplay();

    int screen = DefaultScreen( (Display*) m_display );
    m_cmap = (WXColormap) DefaultColormap( (Display*) m_display, screen );

    m_window = (WXWindow) RootWindow( (Display*) m_display, screen );

    m_isScreenDC = true;

#if wxUSE_UNICODE
    m_context = wxTheApp->GetPangoContext();
    m_fontdesc = wxNORMAL_FONT->GetNativeFontInfo()->description;
#endif

    SetUpDC();

    XSetSubwindowMode( (Display*) m_display, (GC) m_penGC, IncludeInferiors );
    XSetSubwindowMode( (Display*) m_display, (GC) m_brushGC, IncludeInferiors );
    XSetSubwindowMode( (Display*) m_display, (GC) m_textGC, IncludeInferiors );
    XSetSubwindowMode( (Display*) m_display, (GC) m_bgGC, IncludeInferiors );
}

wxScreenDC::~wxScreenDC()
{
    XSetSubwindowMode( (Display*) m_display, (GC) m_penGC, ClipByChildren );
    XSetSubwindowMode( (Display*) m_display, (GC) m_brushGC, ClipByChildren );
    XSetSubwindowMode( (Display*) m_display, (GC) m_textGC, ClipByChildren );
    XSetSubwindowMode( (Display*) m_display, (GC) m_bgGC, ClipByChildren );

    EndDrawingOnTop();
}

bool wxScreenDC::StartDrawingOnTop( wxWindow *window )
{
    if (!window) return StartDrawingOnTop();

    int x = 0;
    int y = 0;
    window->GetPosition( &x, &y );
    int w = 0;
    int h = 0;
    window->GetSize( &w, &h );
    window->ClientToScreen( &x, &y );

    wxRect rect;
    rect.x = x;
    rect.y = y;
    rect.width = 0;
    rect.height = 0;

    return StartDrawingOnTop( &rect );
}

bool wxScreenDC::StartDrawingOnTop( wxRect *rectIn )
{
    // VZ: should we do the same thing that wxMotif wxScreenDC does here?
#if 0
    wxRect rect;
    if ( rectIn )
    {
        rect = *rectIn;
    }
    else
    {
        rect.x =
        rect.y = 0;

        DoGetSize(&rect.width, &rect.height);
    }
#endif // 0

    return true;
}

bool wxScreenDC::EndDrawingOnTop()
{
    return true;
}

void wxScreenDC::DoGetSize(int *width, int *height) const
{
    wxDisplaySize(width, height);
}
