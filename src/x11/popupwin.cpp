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

    m_parent = parent;
    if (m_parent) m_parent->AddChild( this );

    wxTopLevelWindows.Append(this);
    
    Display *xdisplay = wxGlobalDisplay();
    int xscreen = DefaultScreen( xdisplay );
    Visual *xvisual = DefaultVisual( xdisplay, xscreen );
    Window xparent = RootWindow( xdisplay, xscreen );
    
    XSetWindowAttributes xattributes;
    XSizeHints size_hints;
    XWMHints wm_hints;
    
    long xattributes_mask =
        CWEventMask |
        CWBorderPixel | CWBackPixel;
    xattributes.background_pixel = BlackPixel( xdisplay, xscreen );
    xattributes.border_pixel = BlackPixel( xdisplay, xscreen );
    xattributes.override_redirect = False;

    wxSize size(2, 2);
    
    Window xwindow = XCreateWindow( xdisplay, xparent, 0, 0, size.x, size.y, 
       0, DefaultDepth(xdisplay,xscreen), InputOutput, xvisual, xattributes_mask, &xattributes );
    
    XSelectInput( xdisplay, xwindow,
        ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
        ButtonMotionMask | EnterWindowMask | LeaveWindowMask | PointerMotionMask |
        KeymapStateMask | FocusChangeMask | ColormapChangeMask | StructureNotifyMask |
        PropertyChangeMask );

    m_mainWidget = (WXWindow) xwindow;
    wxAddWindowToTable( xwindow, (wxWindow*) this );
    
    XSetTransientForHint( xdisplay, xwindow, xparent );
    
    size_hints.flags = PSize;
    size_hints.width = size.x;
    size_hints.height = size.y;
    XSetWMNormalHints( xdisplay, xwindow, &size_hints);
    
    wm_hints.flags = InputHint | StateHint /* | WindowGroupHint */;
    wm_hints.input = True;
    wm_hints.initial_state = NormalState;
    XSetWMHints( xdisplay, xwindow, &wm_hints);
    
    Atom wm_delete_window = XInternAtom( xdisplay, "WM_DELETE_WINDOW", False);
    XSetWMProtocols( xdisplay, xwindow, &wm_delete_window, 1);
    
    wxSetWMDecorations((Window) GetMainWindow(), style);

    return TRUE;
}

void wxPopupWindow::DoMoveWindow(int WXUNUSED(x), int WXUNUSED(y), int WXUNUSED(width), int WXUNUSED(height) )
{
    wxFAIL_MSG( wxT("DoMoveWindow called for wxPopupWindow") );
}

void wxPopupWindow::DoSetSize( int x, int y, int width, int height, int sizeFlags )
{
    wxWindowX11::DoSetSize(x, y, width, height, sizeFlags);
}

bool wxPopupWindow::Show( bool show )
{
    return wxWindowX11::Show( show );
}

#endif // wxUSE_POPUPWIN
