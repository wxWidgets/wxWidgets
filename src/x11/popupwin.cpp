/////////////////////////////////////////////////////////////////////////////
// Name:        popupwin.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "popupwin.h"
#endif

#include "wx/defs.h"

#if wxUSE_POPUPWIN

#include "wx/popupwin.h"
#include "wx/app.h"

#include "wx/x11/private.h"
#include "X11/Xatom.h"
#include "X11/Xutil.h"

//-----------------------------------------------------------------------------
// wxPopupWindow
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxPopupWindow,wxPopupWindowBase)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxPopupWindow, wxWindow)

bool wxPopupWindow::Create( wxWindow *parent, int style )
{
    if (!CreateBase( parent, -1, wxDefaultPosition, wxDefaultSize, style, wxDefaultValidator, "popup" ))
    {
        wxFAIL_MSG( wxT("wxPopupWindow creation failed") );
        return FALSE;
    }

    // All dialogs should really have this style
    m_windowStyle = style;
    m_windowStyle |= wxTAB_TRAVERSAL;
    
    wxPoint pos( 20,20 );
    wxSize size( 20,20 );

    m_parent = parent;
    if (m_parent) m_parent->AddChild( this );

    Display *xdisplay = wxGlobalDisplay();
    int xscreen = DefaultScreen( xdisplay );
    Visual *xvisual = DefaultVisual( xdisplay, xscreen );
    Window xparent = RootWindow( xdisplay, xscreen );
    
#if wxUSE_NANOX
    long xattributes_mask = 0;
#else
    XSetWindowAttributes xattributes;
    
    long xattributes_mask =
        CWOverrideRedirect |
        CWSaveUnder |
        CWBorderPixel | CWBackPixel;
    xattributes.background_pixel = BlackPixel( xdisplay, xscreen );
    xattributes.border_pixel = BlackPixel( xdisplay, xscreen );
    xattributes.override_redirect = True;
    xattributes.save_under = True;
#endif
    
    Window xwindow = XCreateWindow( xdisplay, xparent, pos.x, pos.y, size.x, size.y, 
       0, DefaultDepth(xdisplay,xscreen), InputOutput, xvisual, xattributes_mask, &xattributes );
    
    XSelectInput( xdisplay, xwindow,
        ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
        ButtonMotionMask | EnterWindowMask | LeaveWindowMask | PointerMotionMask |
        KeymapStateMask | FocusChangeMask | ColormapChangeMask | StructureNotifyMask |
        PropertyChangeMask );

    m_mainWidget = (WXWindow) xwindow;
    wxAddWindowToTable( xwindow, (wxWindow*) this );

    // Set background to None which will prevent X11 from clearing the
    // background comletely.
    XSetWindowBackgroundPixmap( xdisplay, xwindow, None );

    XSetTransientForHint( xdisplay, xwindow, xparent );

#if wxUSE_NANOX
    // Switch off WM
    wxSetWMDecorations(xwindow, 0);
#else
    XWMHints wm_hints;
    wm_hints.flags = InputHint | StateHint /* | WindowGroupHint */;
    wm_hints.input = True;
    wm_hints.initial_state = NormalState;
    XSetWMHints( xdisplay, xwindow, &wm_hints);
#endif
    
    return TRUE;
}

void wxPopupWindow::DoMoveWindow(int x, int y, int width, int height )
{
    wxWindowX11::DoMoveWindow( x, y, width, height );
}

void wxPopupWindow::DoSetSize( int x, int y, int width, int height, int sizeFlags )
{
    wxWindowX11::DoSetSize(x, y, width, height, sizeFlags);
}

bool wxPopupWindow::Show( bool show )
{
    bool ret = wxWindowX11::Show( show );

    Raise();
    
    return ret;
}

#endif // wxUSE_POPUPWIN
