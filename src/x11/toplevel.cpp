///////////////////////////////////////////////////////////////////////////////
// Name:        x11/toplevel.cpp
// Purpose:     implements wxTopLevelWindow for X11
// Author:      Julian Smart
// Modified by:
// Created:     24.09.01
// RCS-ID:      $Id$
// Copyright:   (c) 2002 Julian Smart
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "toplevel.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/toplevel.h"
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/frame.h"
    #include "wx/menu.h"
    #include "wx/statusbr.h"
#endif //WX_PRECOMP

#include "wx/settings.h"
#include "wx/x11/private.h"
#include "X11/Xutil.h"

bool wxMWMIsRunning(Window w);

// ----------------------------------------------------------------------------
// wxTopLevelWindowX11 creation
// ----------------------------------------------------------------------------

void wxTopLevelWindowX11::Init()
{
    m_iconized =
    m_maximizeOnShow = FALSE;

    // unlike (almost?) all other windows, frames are created hidden
    m_isShown = FALSE;

    // Data to save/restore when calling ShowFullScreen
    m_fsStyle = 0;
    m_fsIsMaximized = FALSE;
    m_fsIsShowing = FALSE;
}

bool wxTopLevelWindowX11::Create(wxWindow *parent,
                                 wxWindowID id,
                                 const wxString& title,
                                 const wxPoint& pos,
                                 const wxSize& size,
                                 long style,
                                 const wxString& name)
{
    // init our fields
    Init();

    m_windowStyle = style;
    m_parent = parent;

    SetName(name);

    m_windowId = id == -1 ? NewControlId() : id;

    if (parent)
        parent->AddChild(this);

    wxTopLevelWindows.Append(this);
    
    Display *xdisplay = wxGlobalDisplay();
    int xscreen = DefaultScreen( xdisplay );
    Visual *xvisual = DefaultVisual( xdisplay, xscreen );
    Window xparent = RootWindow( xdisplay, xscreen );
    Colormap cm = DefaultColormap( xdisplay, xscreen );
    
    // TODO: For dialogs, this should be wxSYS_COLOUR_3DFACE
    m_backgroundColour = wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE);
    m_backgroundColour.CalcPixel( (WXColormap) cm );
    m_hasBgCol = TRUE;
	
    XSetWindowAttributes xattributes;
    XSizeHints size_hints;
    XWMHints wm_hints;
    
    long xattributes_mask =
        CWOverrideRedirect |
        CWBorderPixel | CWBackPixel;
    xattributes.background_pixel = m_backgroundColour.GetPixel();
    xattributes.border_pixel = BlackPixel( xdisplay, xscreen );

    // TODO: if we want no border, caption etc.,
    // I think we set this to True to remove decorations
    xattributes.override_redirect = False;
    
    wxSize size2(size);
    if (size2.x == -1)
	size2.x = 100;
    if (size2.y == -1)
	size2.y = 100;

    wxPoint pos2(pos);
    if (pos2.x == -1)
	pos2.x = 100;
    if (pos2.y == -1)
	pos2.y = 100;
    
    Window xwindow = XCreateWindow( xdisplay, xparent, pos2.x, pos2.y, size2.x, size2.y, 
       0, DefaultDepth(xdisplay,xscreen), InputOutput, xvisual, xattributes_mask, &xattributes );
    m_mainWidget = (WXWindow) xwindow;

    XSelectInput( xdisplay, xwindow,
        ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
        ButtonMotionMask | EnterWindowMask | LeaveWindowMask | PointerMotionMask |
        KeymapStateMask | FocusChangeMask | ColormapChangeMask | StructureNotifyMask |
        PropertyChangeMask );

    wxAddWindowToTable( xwindow, (wxWindow*) this );

    // Messes up window management
    //    XSetTransientForHint( xdisplay, xwindow, xparent );

    size_hints.flags = PSize;
    size_hints.width = size2.x;
    size_hints.height = size2.y;
    XSetWMNormalHints( xdisplay, xwindow, &size_hints);
    
    wm_hints.flags = InputHint | StateHint /* | WindowGroupHint */;
    wm_hints.input = True;
    wm_hints.initial_state = NormalState;
    XSetWMHints( xdisplay, xwindow, &wm_hints);
    
    Atom wm_delete_window = XInternAtom( xdisplay, "WM_DELETE_WINDOW", False);
    XSetWMProtocols( xdisplay, xwindow, &wm_delete_window, 1);
    
    wxSetWMDecorations((Window) GetMainWindow(), style);

    SetTitle(title);
    
    return TRUE;
}

wxTopLevelWindowX11::~wxTopLevelWindowX11()
{
    wxTopLevelWindows.DeleteObject(this);

    // If this is the last top-level window, exit.
    if ( wxTheApp && (wxTopLevelWindows.Number() == 0) )
    {
        wxTheApp->SetTopWindow(NULL);

        if (wxTheApp->GetExitOnFrameDelete())
        {
            // Signal to the app that we're going to close
            wxTheApp->ExitMainLoop();
        }
    }
}

// ----------------------------------------------------------------------------
// wxTopLevelWindowX11 showing
// ----------------------------------------------------------------------------

bool wxTopLevelWindowX11::Show(bool show)
{
    return wxWindowX11::Show(show);
}

// ----------------------------------------------------------------------------
// wxTopLevelWindowX11 maximize/minimize
// ----------------------------------------------------------------------------

void wxTopLevelWindowX11::Maximize(bool maximize)
{
    // TODO
}

bool wxTopLevelWindowX11::IsMaximized() const
{
    // TODO
    return TRUE;
}

void wxTopLevelWindowX11::Iconize(bool iconize)
{
    if (!m_iconized && GetMainWindow())
    {
        if (XIconifyWindow(wxGlobalDisplay(),
            (Window) GetMainWindow(), DefaultScreen(wxGlobalDisplay())) != 0)
            m_iconized = TRUE;
    }
}

bool wxTopLevelWindowX11::IsIconized() const
{
    return m_iconized;
}

void wxTopLevelWindowX11::Restore()
{
    // This is the way to deiconify the window, according to the X FAQ
    if (m_iconized && GetMainWindow())
    {
        XMapWindow(wxGlobalDisplay(), (Window) GetMainWindow());
        m_iconized = FALSE;
    }
}

// ----------------------------------------------------------------------------
// wxTopLevelWindowX11 fullscreen
// ----------------------------------------------------------------------------

bool wxTopLevelWindowX11::ShowFullScreen(bool show, long style)
{
    if (show)
    {
        if (IsFullScreen())
            return FALSE;

        m_fsIsShowing = TRUE;
        m_fsStyle = style;

        // TODO

        return TRUE;
    }
    else
    {
        if (!IsFullScreen())
            return FALSE;

        m_fsIsShowing = FALSE;

        // TODO
        return TRUE;
    }
}

// ----------------------------------------------------------------------------
// wxTopLevelWindowX11 misc
// ----------------------------------------------------------------------------

void wxTopLevelWindowX11::SetIcon(const wxIcon& icon)
{
    // this sets m_icon
    wxTopLevelWindowBase::SetIcon(icon);

    if (icon.Ok() && GetMainWindow())
    {
        XWMHints *wmHints = XAllocWMHints();
        wmHints->icon_pixmap = (Pixmap) icon.GetPixmap();

        wmHints->flags = IconPixmapHint;

        if (icon.GetMask())
        {
            wmHints->flags |= IconMaskHint;
            wmHints->icon_mask = (Pixmap) icon.GetMask()->GetBitmap();
        }

        XSetWMHints(wxGlobalDisplay(), (Window) GetMainWindow(), wmHints);
        XFree(wmHints);
    }
}

void wxTopLevelWindowX11::SetTitle(const wxString& title)
{
    m_title = title;
    if (GetMainWindow())
    {
        XStoreName(wxGlobalDisplay(), (Window) GetMainWindow(),
            (const char*) title);
        XSetIconName(wxGlobalDisplay(), (Window) GetMainWindow(),
            (const char*) title);

        // Use this if the platform doesn't supply the above functions.
#if 0
        XTextProperty textProperty;
        textProperty.value = (unsigned char*) title;
        textProperty.encoding = XA_STRING;
        textProperty.format = 8;
        textProperty.nitems = 1;

        XSetTextProperty(wxGlobalDisplay(), (Window) GetMainWindow(),
            & textProperty, WM_NAME);
#endif
    }
}

wxString wxTopLevelWindowX11::GetTitle() const
{
    return m_title;
}

#ifndef MWM_DECOR_BORDER
/* bit definitions for MwmHints.flags */
#define MWM_HINTS_FUNCTIONS (1L << 0)
#define MWM_HINTS_DECORATIONS (1L << 1)
#define MWM_HINTS_INPUT_MODE (1L << 2)
#define MWM_HINTS_STATUS (1L << 3)

#define MWM_DECOR_ALL           (1L << 0)
#define MWM_DECOR_BORDER        (1L << 1)
#define MWM_DECOR_RESIZEH       (1L << 2)
#define MWM_DECOR_TITLE         (1L << 3)
#define MWM_DECOR_MENU          (1L << 4)
#define MWM_DECOR_MINIMIZE      (1L << 5)
#define MWM_DECOR_MAXIMIZE      (1L << 6)
#endif

struct MwmHints {
    long flags;
    long functions;
    long decorations;
    long input_mode;
};

#define PROP_MOTIF_WM_HINTS_ELEMENTS 5

// Set the window manager decorations according to the
// given wxWindows style
bool wxSetWMDecorations(Window w, long style)
{
    if (!wxMWMIsRunning(w))
        return FALSE;

    Atom mwm_wm_hints = XInternAtom(wxGlobalDisplay(),"_MOTIF_WM_HINTS", False);
    MwmHints hints;
    hints.flags = 0;
    hints.decorations = 0;

    if (style & wxRESIZE_BORDER)
    {
	wxLogDebug("MWM_DECOR_RESIZEH");
        hints.flags |= MWM_HINTS_DECORATIONS;
        hints.decorations |= MWM_DECOR_RESIZEH;
    }

    if (style & wxSYSTEM_MENU)
    {
	wxLogDebug("MWM_DECOR_MENU");
        hints.flags |= MWM_HINTS_DECORATIONS;
        hints.decorations |= MWM_DECOR_MENU;
    }

    if ((style & wxCAPTION) ||
        (style & wxTINY_CAPTION_HORIZ) ||
        (style & wxTINY_CAPTION_VERT))
    {
	wxLogDebug("MWM_DECOR_TITLE");
        hints.flags |= MWM_HINTS_DECORATIONS;
        hints.decorations |= MWM_DECOR_TITLE;
    }

    if ((style & wxTHICK_FRAME) || (style & wxSIMPLE_BORDER) || (style & wxCAPTION))
    {
	wxLogDebug("MWM_DECOR_BORDER");
        hints.flags |= MWM_HINTS_DECORATIONS;
        hints.decorations |= MWM_DECOR_BORDER;
    }

    if (style & wxMINIMIZE_BOX)
    {
	wxLogDebug("MWM_DECOR_MINIMIZE");
        hints.flags |= MWM_HINTS_DECORATIONS;
        hints.decorations |= MWM_DECOR_MINIMIZE;
    }

    if (style & wxMAXIMIZE_BOX)
    {
	wxLogDebug("MWM_DECOR_MAXIMIZE");
        hints.flags |= MWM_HINTS_DECORATIONS;
        hints.decorations |= MWM_DECOR_MAXIMIZE;
    }

    XChangeProperty(wxGlobalDisplay(),
		    w,
		    mwm_wm_hints, mwm_wm_hints,
		    32, PropModeReplace,
		    (unsigned char *) &hints, PROP_MOTIF_WM_HINTS_ELEMENTS);

    return TRUE;
}

bool wxMWMIsRunning(Window w)
{
    Display *dpy = (Display*)wxGetDisplay();
    Atom motifWmInfo = XInternAtom(dpy, "_MOTIF_WM_INFO", False);

    unsigned long length, bytesafter;
    unsigned char value[20];
    unsigned char *ptr = &value[0];
    int ret, format;
    Atom type;

    type = format = length = 0;
    value[0] = 0;

    ret = XGetWindowProperty(wxGlobalDisplay(), w, motifWmInfo,
	    0L, 2, False, motifWmInfo, 
	    &type, &format, &length, &bytesafter, &ptr);

    return (ret == Success);
}
    
// For implementation purposes - sometimes decorations make the client area
// smaller
wxPoint wxTopLevelWindowX11::GetClientAreaOrigin() const
{
    // In fact wxFrame::GetClientAreaOrigin
    // does the required calculation already.
#if 0
    if (this->IsKindOf(CLASSINFO(wxFrame)))
    {
	wxFrame* frame = (wxFrame*) this;
	if (frame->GetMenuBar())
	    return wxPoint(0, frame->GetMenuBar()->GetSize().y);
    }
#endif
    return wxPoint(0, 0);
}

void wxTopLevelWindowX11::DoGetClientSize( int *width, int *height ) const
{
    wxWindowX11::DoGetClientSize(width, height);
    // Done by wxTopLevelWindow
#if 0
    if (this->IsKindOf(CLASSINFO(wxFrame)))
    {
	wxFrame* frame = (wxFrame*) this;
	if (frame->GetMenuBar())
	    (*height) -= frame->GetMenuBar()->GetSize().y;
	if (frame->GetStatusBar())
	    (*height) -= frame->GetStatusBar()->GetSize().y;
    }
#endif
}

void wxTopLevelWindowX11::DoSetClientSize(int width, int height)
{
    wxWindowX11::DoSetClientSize(width, height);
#if 0
    if (!GetMainWindow())
        return;

    XWindowChanges windowChanges;
    int valueMask = 0;

    if (width != -1)
    {
        windowChanges.width = width ;
        valueMask |= CWWidth;
    }
    if (height != -1)
    {
        windowChanges.height = height ;
        valueMask |= CWHeight;
    }
    XConfigureWindow(wxGlobalDisplay(), (Window) GetMainWindow(),
        valueMask, & windowChanges);
#endif
}
