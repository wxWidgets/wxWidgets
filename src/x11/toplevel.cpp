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
    
    m_focusWidget = NULL;
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
    
    if (GetExtraStyle() & wxTOPLEVEL_EX_DIALOG)
        m_backgroundColour = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
    else
        m_backgroundColour = wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE);
    m_backgroundColour.CalcPixel( (WXColormap) cm );
    m_hasBgCol = TRUE;
	
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
    
#if !wxUSE_NANOX
    XSetWindowAttributes xattributes;
    XSizeHints size_hints;
    
    long xattributes_mask =
        CWOverrideRedirect |
        CWBorderPixel | CWBackPixel;

    xattributes.background_pixel = m_backgroundColour.GetPixel();
    xattributes.border_pixel = BlackPixel( xdisplay, xscreen );

    // TODO: if we want no border, caption etc.,
    // I think we set this to True to remove decorations
    // No. RR.
    xattributes.override_redirect = False;
#endif
    
#if wxUSE_NANOX
    long backColor, foreColor;
    backColor = GR_RGB(m_backgroundColour.Red(), m_backgroundColour.Green(), m_backgroundColour.Blue());
    foreColor = GR_RGB(m_foregroundColour.Red(), m_foregroundColour.Green(), m_foregroundColour.Blue());
    
    Window xwindow = XCreateWindowWithColor( xdisplay, xparent, pos2.x, pos2.y, size2.x, size2.y, 
                                    0, 0, InputOutput, xvisual, backColor, foreColor);
#else
    Window xwindow = XCreateWindow( xdisplay, xparent, pos2.x, pos2.y, size2.x, size2.y, 
                                    0, DefaultDepth(xdisplay,xscreen), InputOutput, xvisual, xattributes_mask, &xattributes );
#endif
    m_mainWidget = (WXWindow) xwindow;

    int extraFlags = 0;
#if wxUSE_NANOX
    extraFlags |= GR_EVENT_MASK_CLOSE_REQ;
#endif

    XSelectInput( xdisplay, xwindow,
                  extraFlags |
                  ExposureMask |
                  KeyPressMask |
                  KeyReleaseMask |
                  ButtonPressMask |
                  ButtonReleaseMask |
                  ButtonMotionMask |
                  EnterWindowMask |
                  LeaveWindowMask |
                  PointerMotionMask |
                  KeymapStateMask |
                  FocusChangeMask |
                  ColormapChangeMask |
                  StructureNotifyMask |
                  PropertyChangeMask
                  );
    
    wxAddWindowToTable( xwindow, (wxWindow*) this );

    // Set background to None which will prevent X11 from clearing the
    // background completely.
    XSetWindowBackgroundPixmap( xdisplay, xwindow, None );

#if !wxUSE_NANOX
    if (GetExtraStyle() & wxTOPLEVEL_EX_DIALOG)
    {
        if (GetParent() && GetParent()->GetMainWindow())
        {
             Window xparentwindow = (Window) GetParent()->GetMainWindow();
             XSetTransientForHint( xdisplay, xwindow, xparentwindow );
        }
    }

    size_hints.flags = PSize | PPosition;
    size_hints.x = pos2.x;
    size_hints.y = pos2.y;
    size_hints.width = size2.x;
    size_hints.height = size2.y;
    XSetWMNormalHints( xdisplay, xwindow, &size_hints);
    
    XWMHints wm_hints;
    wm_hints.flags = InputHint | StateHint /* | WindowGroupHint */;
    wm_hints.input = True;
    wm_hints.initial_state = NormalState;
    XSetWMHints( xdisplay, xwindow, &wm_hints);

    Atom wm_protocols[2];
    wm_protocols[0] = XInternAtom( xdisplay, "WM_DELETE_WINDOW", False );
    wm_protocols[1] = XInternAtom( xdisplay, "WM_TAKE_FOCUS", False );
    XSetWMProtocols( xdisplay, xwindow, wm_protocols, 2);
#endif
    
    wxSetWMDecorations( xwindow, style);

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
#if wxUSE_NANOX
#else
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
#endif
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
#if wxUSE_NANOX
    GR_WM_PROPERTIES wmProp;

    wmProp.flags = 0;
    wmProp.props = 0;

    if (style & wxRESIZE_BORDER)
    {
        wmProp.props |= GR_WM_PROPS_APPFRAME ;
        wmProp.flags |= GR_WM_FLAGS_PROPS ;
    }

    if (style & wxSYSTEM_MENU)
    {
        wmProp.props |= GR_WM_PROPS_CLOSEBOX ;
        wmProp.flags |= GR_WM_FLAGS_PROPS ;
    }

    if ((style & wxCAPTION) ||
        (style & wxTINY_CAPTION_HORIZ) ||
        (style & wxTINY_CAPTION_VERT))
    {
        wmProp.props |= GR_WM_PROPS_CAPTION ;
        wmProp.flags |= GR_WM_FLAGS_PROPS ;

        // The default dialog style doesn't include any kind
        // of border, which is a bit odd. Anyway, inclusion
        // of a caption surely implies a border.
        style |= wxTHICK_FRAME;
    }

    if (style & wxTHICK_FRAME)
    {
        wmProp.props |= GR_WM_PROPS_APPFRAME ;
        wmProp.flags |= GR_WM_FLAGS_PROPS ;
    }

    if (style & wxSIMPLE_BORDER)
    {
        wmProp.props |= GR_WM_PROPS_BORDER ;
        wmProp.flags |= GR_WM_FLAGS_PROPS ;
    }

    if (style & wxMINIMIZE_BOX)
    {
    }

    if (style & wxMAXIMIZE_BOX)
    {
        wmProp.props |= GR_WM_PROPS_MAXIMIZE ;
        wmProp.flags |= GR_WM_FLAGS_PROPS ;
    }

    if (((style & wxBORDER) != wxBORDER) && ((style & wxTHICK_FRAME) != wxTHICK_FRAME)
        && ((style & wxRESIZE_BORDER) != wxRESIZE_BORDER))
    {
        wmProp.props |= GR_WM_PROPS_NODECORATE ;
        wmProp.flags |= GR_WM_FLAGS_PROPS ;
    }

    GrSetWMProperties(w, & wmProp);
    
#else
    if (!wxMWMIsRunning(w))
        return FALSE;

    Atom mwm_wm_hints = XInternAtom(wxGlobalDisplay(),"_MOTIF_WM_HINTS", False);
    MwmHints hints;
    hints.flags = 0;
    hints.decorations = 0;

    if (style & wxRESIZE_BORDER)
    {
        // wxLogDebug("MWM_DECOR_RESIZEH");
        hints.flags |= MWM_HINTS_DECORATIONS;
        hints.decorations |= MWM_DECOR_RESIZEH;
    }

    if (style & wxSYSTEM_MENU)
    {
        // wxLogDebug("MWM_DECOR_MENU");
        hints.flags |= MWM_HINTS_DECORATIONS;
        hints.decorations |= MWM_DECOR_MENU;
    }

    if ((style & wxCAPTION) ||
        (style & wxTINY_CAPTION_HORIZ) ||
        (style & wxTINY_CAPTION_VERT))
    {
        // wxLogDebug("MWM_DECOR_TITLE");
        hints.flags |= MWM_HINTS_DECORATIONS;
        hints.decorations |= MWM_DECOR_TITLE;
    }

    if ((style & wxTHICK_FRAME) || (style & wxSIMPLE_BORDER) || (style & wxCAPTION))
    {
        // wxLogDebug("MWM_DECOR_BORDER");
        hints.flags |= MWM_HINTS_DECORATIONS;
        hints.decorations |= MWM_DECOR_BORDER;
    }

    if (style & wxMINIMIZE_BOX)
    {
        // wxLogDebug("MWM_DECOR_MINIMIZE");
        hints.flags |= MWM_HINTS_DECORATIONS;
        hints.decorations |= MWM_DECOR_MINIMIZE;
    }

    if (style & wxMAXIMIZE_BOX)
    {
        // wxLogDebug("MWM_DECOR_MAXIMIZE");
        hints.flags |= MWM_HINTS_DECORATIONS;
        hints.decorations |= MWM_DECOR_MAXIMIZE;
    }

    XChangeProperty(wxGlobalDisplay(),
		    w,
		    mwm_wm_hints, mwm_wm_hints,
		    32, PropModeReplace,
		    (unsigned char *) &hints, PROP_MOTIF_WM_HINTS_ELEMENTS);

#endif
    return TRUE;
}

bool wxMWMIsRunning(Window w)
{
#if wxUSE_NANOX
    return FALSE;
#else
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
#endif
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

void wxTopLevelWindowX11::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
    // wxLogDebug( "Setting pos: %d, %d", x, y );
    wxWindowX11::DoSetSize(x, y, width, height, sizeFlags);

#if 0
    wxPoint pt = GetPosition();
    // wxLogDebug( "After, pos: %d, %d", pt.x, pt.y );

    XSync(wxGlobalDisplay(), False);
    int w, h;
    GetSize(& w, & h);
    wxString msg;
    msg.Printf("Before setting size: %d, %d", w, h);
    wxLogDebug(msg);
    if (!GetMainWindow())
        return;

    XWindowChanges windowChanges;
    int valueMask = 0;

    if (x != -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    {
        int yy = 0;
        AdjustForParentClientOrigin( x, yy, sizeFlags);
        windowChanges.x = x;
        valueMask |= CWX;
    }
    if (y != -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    {
        int xx = 0;
        AdjustForParentClientOrigin( xx, y, sizeFlags);
        windowChanges.y = y;
        valueMask |= CWY;
    }
    if (width != -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    {
        windowChanges.width = width /* - m_borderSize*2 */;
        valueMask |= CWWidth;
    }
    if (height != -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    {
        windowChanges.height = height /* -m_borderSize*2*/;
        valueMask |= CWHeight;
    }

    XConfigureWindow(wxGlobalDisplay(), (Window) GetMainWindow(),
		     valueMask, & windowChanges);
    XSync(wxGlobalDisplay(), False);
    GetSize(& w, & h);
    msg.Printf("Tried to set to %d, %d. After setting size: %d, %d", width, height, w, h);
    wxLogDebug(msg);
#endif
}

void wxTopLevelWindowX11::DoGetPosition(int *x, int *y) const
{
    XSync(wxGlobalDisplay(), False);
    Window window = (Window) m_mainWidget;
    if (window)
    {
        int offsetX = 0;
        int offsetY = 0;
	
#if !wxUSE_NANOX
        // wxLogDebug("Translating...");
        Window childWindow;
        XTranslateCoordinates(wxGlobalDisplay(), window, XDefaultRootWindow(wxGlobalDisplay()),
				  0, 0, & offsetX, & offsetY, & childWindow);

        // wxLogDebug("Offset: %d, %d", offsetX, offsetY);
#endif
	
        XWindowAttributes attr;
        Status status = XGetWindowAttributes(wxGlobalDisplay(), window, & attr);
        wxASSERT(status);
        
        if (status)
        {
            *x = attr.x + offsetX;
            *y = attr.y + offsetY;
        }
    }
}
