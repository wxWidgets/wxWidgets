/////////////////////////////////////////////////////////////////////////////
// Name:        popupwin.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "popupwin.h"
#endif

#include "wx/defs.h"
#include "wx/log.h"

#if wxUSE_POPUPWIN

#include "wx/popupwin.h"
#include "wx/app.h"
#include "wx/settings.h"

#include "wx/x11/private.h"
#include "X11/Xatom.h"
#include "X11/Xutil.h"

//-----------------------------------------------------------------------------
// wxPopupWindow
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxPopupWindow,wxPopupWindowBase)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxPopupWindow, wxWindow)

wxPopupWindow::~wxPopupWindow()
{
}

bool wxPopupWindow::Create( wxWindow *parent, int style )
{
    if (!CreateBase( parent, -1, wxDefaultPosition, wxDefaultSize, style, wxDefaultValidator, wxT("popup") ))
    {
        wxFAIL_MSG( wxT("wxPopupWindow creation failed") );
        return FALSE;
    }

    // All dialogs should really have this style
    m_windowStyle = style;
    m_windowStyle |= wxTAB_TRAVERSAL;
    
    wxPoint pos( 20,20 );
    wxSize size( 20,20 );
    wxPoint pos2 = pos;
    wxSize size2 = size;

    m_parent = parent;
    if (m_parent) m_parent->AddChild( this );

    Display *xdisplay = wxGlobalDisplay();
    int xscreen = DefaultScreen( xdisplay );
    Visual *xvisual = DefaultVisual( xdisplay, xscreen );
    Window xparent = RootWindow( xdisplay, xscreen );
    Colormap cm = DefaultColormap( xdisplay, xscreen);
    
#if wxUSE_TWO_WINDOWS
    bool need_two_windows = 
        ((( wxSUNKEN_BORDER | wxRAISED_BORDER | wxSIMPLE_BORDER | wxHSCROLL | wxVSCROLL ) & m_windowStyle) != 0);
#else
    bool need_two_windows = FALSE;
#endif

#if wxUSE_NANOX
    long xattributes_mask = 0;
#else
    
    XSetWindowAttributes xattributes;
    long xattributes_mask = 0;

    m_backgroundColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    m_backgroundColour.CalcPixel( (WXColormap) cm);
    
    m_foregroundColour = *wxBLACK;
    m_foregroundColour.CalcPixel( (WXColormap) cm);
    
    xattributes_mask |= CWBackPixel;
    xattributes.background_pixel = m_backgroundColour.GetPixel();
    
    xattributes_mask |= CWBorderPixel;
    xattributes.border_pixel = BlackPixel( xdisplay, xscreen );
    
    xattributes_mask |= CWOverrideRedirect | CWSaveUnder;
    xattributes.override_redirect = True;
    xattributes.save_under = True;
    
    xattributes_mask |= CWEventMask;
#endif
    
    if (need_two_windows)
    {
#if !wxUSE_NANOX
        xattributes.event_mask = 
            ExposureMask | StructureNotifyMask | ColormapChangeMask;
#endif
        
        Window xwindow = XCreateWindow( xdisplay, xparent, pos.x, pos.y, size.x, size.y, 
            0, DefaultDepth(xdisplay,xscreen), InputOutput, xvisual, xattributes_mask, &xattributes );

#if wxUSE_NANOX    
        XSelectInput( xdisplay, xwindow,
          ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
          ButtonMotionMask | EnterWindowMask | LeaveWindowMask | PointerMotionMask |
          KeymapStateMask | FocusChangeMask | ColormapChangeMask | StructureNotifyMask |
          PropertyChangeMask );
#endif
    
        // Set background to None which will prevent X11 from clearing the
        // background comletely.
        XSetWindowBackgroundPixmap( xdisplay, xwindow, None );
    
        m_mainWindow = (WXWindow) xwindow;
        wxAddWindowToTable( xwindow, (wxWindow*) this );
        
        // XMapWindow( xdisplay, xwindow );
#if !wxUSE_NANOX    
        xattributes.event_mask = 
            ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
            ButtonMotionMask | EnterWindowMask | LeaveWindowMask | PointerMotionMask |
            KeymapStateMask | FocusChangeMask | ColormapChangeMask | StructureNotifyMask |
            PropertyChangeMask | VisibilityChangeMask ;
#endif
        
        if (HasFlag( wxSUNKEN_BORDER) || HasFlag( wxRAISED_BORDER))
        {
            pos2.x = 2;
            pos2.y = 2;
            size2.x -= 4;
            size2.y -= 4;
        } else
        if (HasFlag( wxSIMPLE_BORDER ))
        {
            pos2.x = 1;
            pos2.y = 1;
            size2.x -= 2;
            size2.y -= 2;
        } else
        {
            pos2.x = 0;
            pos2.y = 0;
        }
        
        xwindow = XCreateWindow( xdisplay, xwindow, pos2.x, pos2.y, size2.x, size2.y, 
            0, DefaultDepth(xdisplay,xscreen), InputOutput, xvisual, xattributes_mask, &xattributes );
    
        // Set background to None which will prevent X11 from clearing the
        // background comletely.
        XSetWindowBackgroundPixmap( xdisplay, xwindow, None );
    
#if wxUSE_NANOX    
        XSelectInput( xdisplay, xwindow,
            ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
            ButtonMotionMask | EnterWindowMask | LeaveWindowMask | PointerMotionMask |
            KeymapStateMask | FocusChangeMask | ColormapChangeMask | StructureNotifyMask |
            PropertyChangeMask );
#endif
    
        m_clientWindow = (WXWindow) xwindow;
        wxAddClientWindowToTable( xwindow, (wxWindow*) this );
        
        m_isShown = FALSE;
        XMapWindow( xdisplay, xwindow );
    }
    else
    {
        // One window
#if !wxUSE_NANOX
        xattributes.event_mask = 
            ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
            ButtonMotionMask | EnterWindowMask | LeaveWindowMask | PointerMotionMask |
            KeymapStateMask | FocusChangeMask | ColormapChangeMask | StructureNotifyMask |
            PropertyChangeMask | VisibilityChangeMask ;
#endif
        
        Window xwindow = XCreateWindow( xdisplay, xparent, pos.x, pos.y, size.x, size.y, 
            0, DefaultDepth(xdisplay,xscreen), InputOutput, xvisual, xattributes_mask, &xattributes );

#if wxUSE_NANOX
        XSelectInput( xdisplay, xwindow,
          ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
          ButtonMotionMask | EnterWindowMask | LeaveWindowMask | PointerMotionMask |
          KeymapStateMask | FocusChangeMask | ColormapChangeMask | StructureNotifyMask |
          PropertyChangeMask );
#endif
        
        // Set background to None which will prevent X11 from clearing the
        // background comletely.
        XSetWindowBackgroundPixmap( xdisplay, xwindow, None );
    
        m_mainWindow = (WXWindow) xwindow;
        m_clientWindow = (WXWindow) xwindow;
        wxAddWindowToTable( xwindow, (wxWindow*) this );
        
        m_isShown = FALSE;
        // XMapWindow( xdisplay, xwindow );
    }

    XSetTransientForHint( xdisplay, (Window) m_mainWindow, xparent );

#if wxUSE_NANOX
    // Switch off WM
    wxSetWMDecorations( (Window) m_mainWindow, 0 );
#else
    XWMHints wm_hints;
    wm_hints.flags = InputHint | StateHint /* | WindowGroupHint */;
    wm_hints.input = True;
    wm_hints.initial_state = NormalState;
    XSetWMHints( xdisplay, (Window) m_mainWindow, &wm_hints);
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
