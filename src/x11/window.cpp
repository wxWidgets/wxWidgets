/////////////////////////////////////////////////////////////////////////////
// Name:        windows.cpp
// Purpose:     wxWindow
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
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
    #pragma implementation "window.h"
#endif

#include "wx/setup.h"
#include "wx/menu.h"
#include "wx/dc.h"
#include "wx/dcclient.h"
#include "wx/utils.h"
#include "wx/app.h"
#include "wx/panel.h"
#include "wx/layout.h"
#include "wx/dialog.h"
#include "wx/listbox.h"
#include "wx/button.h"
#include "wx/settings.h"
#include "wx/msgdlg.h"
#include "wx/frame.h"
#include "wx/scrolwin.h"
#include "wx/module.h"
#include "wx/menuitem.h"
#include "wx/log.h"

#if  wxUSE_DRAG_AND_DROP
    #include "wx/dnd.h"
#endif

#include "wx/x11/private.h"
#include "X11/Xutil.h"

#include <string.h>

// ----------------------------------------------------------------------------
// global variables for this module
// ----------------------------------------------------------------------------

extern wxHashTable *wxWidgetHashTable;
static wxWindow* g_captureWindow = NULL;

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

#define event_left_is_down(x) ((x)->xbutton.state & Button1Mask)
#define event_middle_is_down(x) ((x)->xbutton.state & Button2Mask)
#define event_right_is_down(x) ((x)->xbutton.state & Button3Mask)

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxWindowX11, wxWindowBase)

BEGIN_EVENT_TABLE(wxWindowX11, wxWindowBase)
    EVT_SYS_COLOUR_CHANGED(wxWindowX11::OnSysColourChanged)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// helper functions
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// constructors
// ----------------------------------------------------------------------------

void wxWindowX11::Init()
{
    // generic initializations first
    InitBase();

    // X11-specific
    m_mainWidget = (WXWindow) 0;
    m_winCaptured = FALSE;
    m_needsInputFocus = FALSE;
    m_isShown = TRUE;
    m_isBeingDeleted = FALSE;
    m_lastTS = 0;
    m_lastButton = 0;
}

// real construction (Init() must have been called before!)
bool wxWindowX11::Create(wxWindow *parent, wxWindowID id,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxString& name)
{
    wxCHECK_MSG( parent, FALSE, "can't create wxWindow without parent" );

    CreateBase(parent, id, pos, size, style, wxDefaultValidator, name);

    parent->AddChild(this);

    int w = size.GetWidth();
    int h = size.GetHeight();
    int x = size.GetX();
    int y = size.GetY();
    if (w == -1) w = 20;
    if (h == -1) h = 20;
    if (x == -1) x = 0;
    if (y == -1) y = 0;

    Display *xdisplay = (Display*) wxGlobalDisplay();
    int xscreen = DefaultScreen( xdisplay );
    Colormap cm = DefaultColormap( xdisplay, xscreen );

    m_backgroundColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    m_backgroundColour.CalcPixel( (WXColormap) cm ); 
    
    m_foregroundColour = *wxBLACK;
    m_foregroundColour.CalcPixel( (WXColormap) cm ); 

    Window parentWindow = (Window) parent->GetMainWindow();

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
    
    Window xwindow = XCreateSimpleWindow( 
        xdisplay, parentWindow,
        pos2.x, pos2.y, size2.x, size2.y, 0, 
        m_backgroundColour.GetPixel(),
        m_backgroundColour.GetPixel() );
        
    m_mainWidget = (WXWindow) xwindow;

    // Select event types wanted
    XSelectInput( xdisplay, xwindow,
        ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
        ButtonMotionMask | EnterWindowMask | LeaveWindowMask | PointerMotionMask |
        KeymapStateMask | FocusChangeMask | ColormapChangeMask | StructureNotifyMask |
        PropertyChangeMask);

    wxAddWindowToTable( xwindow, (wxWindow*) this );

    // Is a subwindow, so map immediately
    m_isShown = TRUE;
    XMapWindow( xdisplay, xwindow );

    // Without this, the cursor may not be restored properly (e.g. in splitter
    // sample).
    SetCursor(*wxSTANDARD_CURSOR);
    SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
    
    // Set background to None which will prevent X11 from clearing the
    // background comletely.
    XSetWindowBackgroundPixmap( xdisplay, xwindow, None );

    // Don't call this, it can have nasty repercussions for composite controls,
    // for example
    //    SetSize(pos.x, pos.y, size.x, size.y);

    return TRUE;
}

// Destructor
wxWindowX11::~wxWindowX11()
{
    if (g_captureWindow == this)
	g_captureWindow = NULL;
    
    m_isBeingDeleted = TRUE;
    
    // X11-specific actions first
    Window main = (Window) m_mainWidget;
    if ( main )
    {
        // Removes event handlers
        //DetachWidget(main);
    }

    if (m_parent)
        m_parent->RemoveChild( this );

    DestroyChildren();

    // Destroy the window
    if (main)
    {
        XSelectInput( wxGlobalDisplay(), main, NoEventMask);
        wxDeleteWindowFromTable( main );
        XDestroyWindow( wxGlobalDisplay(), main );
        m_mainWidget = NULL;
    }
}

// ---------------------------------------------------------------------------
// basic operations
// ---------------------------------------------------------------------------

void wxWindowX11::SetFocus()
{
    Window xwindow = (Window) GetMainWindow();
    
    wxCHECK_RET( xwindow, wxT("invalid window") );

    if (wxWindowIsVisible(xwindow))
    {
        XSetInputFocus( wxGlobalDisplay(), xwindow, RevertToParent, CurrentTime );
        m_needsInputFocus = FALSE;
    }
    else
    {
        m_needsInputFocus = TRUE;
    }
}

// Get the window with the focus
wxWindow *wxWindowBase::FindFocus()
{
    Window xfocus = (Window) 0;
    int revert = 0;

    XGetInputFocus( wxGlobalDisplay(), &xfocus, &revert);
    if (xfocus)
    {
        wxWindow *win = wxGetWindowFromTable( xfocus );

        return win;
    }

    return NULL;
}

#if 0
wxWindow *wxWindowX11::GetFocusWidget()
{
   wxWindow *win = (wxWindow*) this;
   while (!win->IsTopLevel())
   {
       win = win->GetParent();
       if (!win)
           return (wxWindow*) NULL;
   }
   
   return win;
}
#endif

// Enabling/disabling handled by event loop, and not sending events
// if disabled.
bool wxWindowX11::Enable(bool enable)
{
    if ( !wxWindowBase::Enable(enable) )
        return FALSE;
   
    return TRUE;
}

bool wxWindowX11::Show(bool show)
{
    wxWindowBase::Show(show);

    Window xwin = (Window) GetXWindow();
    Display *xdisp = (Display*) GetXDisplay();
    if (show)
    {
        wxString msg;
        msg.Printf("Mapping window of type %s", GetClassInfo()->GetClassName());
        wxLogDebug(msg);
        XMapWindow(xdisp, xwin);
        XSync(xdisp, False);
    }
    else
    {
        wxString msg;
        msg.Printf("Unmapping window of type %s", GetClassInfo()->GetClassName());
        wxLogDebug(msg);
        XUnmapWindow(xdisp, xwin);
    }

    return TRUE;
}

// Raise the window to the top of the Z order
void wxWindowX11::Raise()
{
    if (m_mainWidget)
        XRaiseWindow( wxGlobalDisplay(), (Window) m_mainWidget );
}

// Lower the window to the bottom of the Z order
void wxWindowX11::Lower()
{
    if (m_mainWidget)
        XLowerWindow( wxGlobalDisplay(), (Window) m_mainWidget );
}

void wxWindowX11::DoCaptureMouse()
{
    if ((g_captureWindow != NULL) && (g_captureWindow != this))
    {
        wxASSERT_MSG(FALSE, "Trying to capture before mouse released.");

        // Core dump now
        int *tmp = NULL;
        (*tmp) = 1;
        return;
    }
    
    if (m_winCaptured)
        return;

    Window xwindow = (Window) GetMainWindow();

    wxCHECK_RET( xwindow, wxT("invalid window") );
    
    g_captureWindow = (wxWindow*) this;

    if (xwindow)
    {
        int res = XGrabPointer(wxGlobalDisplay(), xwindow,
            FALSE,
            ButtonPressMask | ButtonReleaseMask | ButtonMotionMask | EnterWindowMask | LeaveWindowMask | PointerMotionMask,
            GrabModeAsync,
	        GrabModeAsync,
            None,
            None, /* cursor */ // TODO: This may need to be set to the cursor of this window
            CurrentTime );

        if (res != GrabSuccess)
        {
            wxString msg;
            msg.Printf("Failed to grab pointer for window %s", this->GetClassInfo()->GetClassName());
            wxLogDebug(msg);
            if (res == GrabNotViewable)
            {
                wxLogDebug("This is not a viewable window - perhaps not shown yet?");
            }
            g_captureWindow = NULL;
            return;
        }

        wxLogDebug("Grabbed pointer");

#if 0
        res = XGrabButton(wxGlobalDisplay(), AnyButton, AnyModifier,
            (Window) GetMainWindow(),
            FALSE,
            ButtonPressMask | ButtonReleaseMask | ButtonMotionMask,
	        GrabModeAsync,
	        GrabModeAsync,
            None,
            None);
	
        if (res != GrabSuccess)
        {
            wxLogDebug("Failed to grab mouse buttons.");
            XUngrabPointer(wxGlobalDisplay(), CurrentTime);
            return;
        }
#endif

#if 0
        res = XGrabKeyboard(wxGlobalDisplay(), (Window) GetMainWindow(),
            ShiftMask | LockMask | ControlMask | Mod1Mask | Mod2Mask | Mod3Mask | Mod4Mask | Mod5Mask,
            FALSE,
            GrabModeAsync,
	        GrabModeAsync,
            CurrentTime);

        if (res != GrabSuccess)
        {
            wxLogDebug("Failed to grab keyboard.");
            XUngrabPointer(wxGlobalDisplay(), CurrentTime);
            XUngrabButton(wxGlobalDisplay(), AnyButton, AnyModifier,
                (Window) GetMainWindow());
            return;
        }
#endif
	
        m_winCaptured = TRUE;
    }
}

void wxWindowX11::DoReleaseMouse()
{
    g_captureWindow = NULL;
    
    if ( !m_winCaptured )
        return;

    Window xwindow = (Window) GetMainWindow();

    if (xwindow)
    {
        XUngrabPointer( wxGlobalDisplay(), CurrentTime );
#if 0
        XUngrabButton( wxGlobalDisplay(), AnyButton, AnyModifier, xwindow);
        XUngrabKeyboard( wxGlobalDisplay(), CurrentTime );
#endif
    }
    wxLogDebug("Ungrabbed pointer");

    m_winCaptured = FALSE;
}

bool wxWindowX11::SetFont(const wxFont& font)
{
    if ( !wxWindowBase::SetFont(font) )
    {
        // nothing to do
        return FALSE;
    }

    return TRUE;
}

bool wxWindowX11::SetCursor(const wxCursor& cursor)
{
    if ( !wxWindowBase::SetCursor(cursor) )
    {
        // no change
        return FALSE;
    }

    Window xwindow = (Window) GetMainWindow();

    wxCHECK_MSG( xwindow, FALSE, wxT("invalid window") );
    
    wxCursor cursorToUse;
    if (m_cursor.Ok())
        cursorToUse = m_cursor;
    else
        cursorToUse = *wxSTANDARD_CURSOR;

    Cursor xcursor = (Cursor) cursorToUse.GetCursor();

    XDefineCursor( (Display*) wxGlobalDisplay(), xwindow, xcursor );

    return TRUE;
}

// Coordinates relative to the window
void wxWindowX11::WarpPointer (int x, int y)
{
    Window xwindow = (Window) GetMainWindow();

    wxCHECK_RET( xwindow, wxT("invalid window") );
    
    XWarpPointer( wxGlobalDisplay(), None, xwindow, 0, 0, 0, 0, x, y);
}

// Does a physical scroll
void wxWindowX11::ScrollWindow(int dx, int dy, const wxRect *rect)
{
    // No scrolling requested.
    if ((dx == 0) && (dy == 0)) return;
    
    if (!m_updateRegion.IsEmpty())
    {
        m_updateRegion.Offset( dx, dy );
        
        int cw = 0;
        int ch = 0;
        GetSize( &cw, &ch );  // GetClientSize() ??
        m_updateRegion.Intersect( 0, 0, cw, ch );
    }
    
    if (!m_clearRegion.IsEmpty())
    {
        m_clearRegion.Offset( dx, dy );
        
        int cw = 0;
        int ch = 0;
        GetSize( &cw, &ch );  // GetClientSize() ??
        m_clearRegion.Intersect( 0, 0, cw, ch );
    }
    
    Window xwindow = (Window) GetMainWindow();

    wxCHECK_RET( xwindow, wxT("invalid window") );

    Display *xdisplay = wxGlobalDisplay();

    GC xgc = XCreateGC( xdisplay, xwindow, 0, NULL );
    XSetGraphicsExposures( xdisplay, xgc, True );

    int s_x;
    int s_y;
    int cw;
    int ch;
    if (rect)
    {
        s_x = rect->x;
        s_y = rect->y;
        cw = rect->width;
        ch = rect->height;
    }
    else
    {
        s_x = 0;
        s_y = 0;
        GetClientSize( &cw, &ch );
    }
    
    wxPoint offset = GetClientAreaOrigin();
    s_x += offset.x;
    s_y += offset.y;

    int w = cw - abs(dx);
    int h = ch - abs(dy);
        
    if ((h < 0) || (w < 0))
    {
        Refresh();
    }
    else
    {
        wxRect rect;
        if (dx < 0) rect.x = cw+dx; else rect.x = s_x;
        if (dy < 0) rect.y = ch+dy; else rect.y = s_y;
        if (dy != 0) rect.width = cw; else rect.width = abs(dx);
        if (dx != 0) rect.height = ch; else rect.height = abs(dy);
    
        int d_x = s_x;
        int d_y = s_y;
        if (dx < 0) s_x += -dx;
        if (dy < 0) s_y += -dy;
        if (dx > 0) d_x = dx;
        if (dy > 0) d_y = dy;

        XCopyArea( xdisplay, xwindow, xwindow, xgc, s_x, s_y, w, h, d_x, d_y );
        
        // printf( "s_x %d s_y %d w %d h %d d_x %d d_y %d\n", s_x, s_y, w, h, d_x, d_y );

        // printf( "rect %d %d %d %d\n", rect.x, rect.y, rect.width, rect.height );

        m_updateRegion.Union( rect );
        m_clearRegion.Union( rect );
    }
    
    XFreeGC( xdisplay, xgc );
}

// ---------------------------------------------------------------------------
// drag and drop
// ---------------------------------------------------------------------------

#if wxUSE_DRAG_AND_DROP

void wxWindowX11::SetDropTarget(wxDropTarget * WXUNUSED(pDropTarget))
{
    // TODO
}

#endif

// Old style file-manager drag&drop
void wxWindowX11::DragAcceptFiles(bool WXUNUSED(accept))
{
    // TODO
}

// ----------------------------------------------------------------------------
// tooltips
// ----------------------------------------------------------------------------

#if wxUSE_TOOLTIPS

void wxWindowX11::DoSetToolTip(wxToolTip * WXUNUSED(tooltip))
{
    // TODO
}

#endif // wxUSE_TOOLTIPS

// ---------------------------------------------------------------------------
// moving and resizing
// ---------------------------------------------------------------------------

bool wxWindowX11::PreResize()
{
    return TRUE;
}

// Get total size
void wxWindowX11::DoGetSize(int *x, int *y) const
{
    Window xwindow = (Window) GetMainWindow();

    wxCHECK_RET( xwindow, wxT("invalid window") );
    
    XSync(wxGlobalDisplay(), False);
    XWindowAttributes attr;
    Status status = XGetWindowAttributes( wxGlobalDisplay(), xwindow, &attr );
    wxASSERT(status);
        
    if (status)
    {
        *x = attr.width /* + 2*m_borderSize */ ;
        *y = attr.height /* + 2*m_borderSize */ ;
    }
}

void wxWindowX11::DoGetPosition(int *x, int *y) const
{
    Window window = (Window) m_mainWidget;
    if (window)
    {
        XSync(wxGlobalDisplay(), False);
        XWindowAttributes attr;
        Status status = XGetWindowAttributes(wxGlobalDisplay(), window, & attr);
        wxASSERT(status);
        
        if (status)
        {
            *x = attr.x;
            *y = attr.y;
            
            // We may be faking the client origin. So a window that's really at (0, 30)
            // may appear (to wxWin apps) to be at (0, 0).
            if (GetParent())
            {
                wxPoint pt(GetParent()->GetClientAreaOrigin());
                *x -= pt.x;
                *y -= pt.y;
            }
        }
    }
}

void wxWindowX11::DoScreenToClient(int *x, int *y) const
{
    Display *display = wxGlobalDisplay();
    Window rootWindow = RootWindowOfScreen(DefaultScreenOfDisplay(display));
    Window thisWindow = (Window) m_mainWidget;

    Window childWindow;
    int xx = *x;
    int yy = *y;
    XTranslateCoordinates(display, rootWindow, thisWindow, xx, yy, x, y, &childWindow);
}

void wxWindowX11::DoClientToScreen(int *x, int *y) const
{
    Display *display = wxGlobalDisplay();
    Window rootWindow = RootWindowOfScreen(DefaultScreenOfDisplay(display));
    Window thisWindow = (Window) m_mainWidget;

    Window childWindow;
    int xx = *x;
    int yy = *y;
    XTranslateCoordinates(display, thisWindow, rootWindow, xx, yy, x, y, &childWindow);
}


// Get size *available for subwindows* i.e. excluding menu bar etc.
void wxWindowX11::DoGetClientSize(int *x, int *y) const
{
    Window window = (Window) m_mainWidget;

    if (window)
    {
        XSync(wxGlobalDisplay(), False);  // Is this really a good idea?
        XWindowAttributes attr;
        Status status = XGetWindowAttributes( wxGlobalDisplay(), window, &attr );
        wxASSERT(status);
        
        if (status)
        {
            *x = attr.width ;
            *y = attr.height ;
        }
    }
}

void wxWindowX11::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
    Window xwindow = (Window) GetMainWindow();

    wxCHECK_RET( xwindow, wxT("invalid window") );

    XWindowAttributes attr;
    Status status = XGetWindowAttributes( wxGlobalDisplay(), xwindow, &attr );
    wxCHECK_RET( status, wxT("invalid window attributes") );
        
    int new_x = attr.x;
    int new_y = attr.y;
    int new_w = attr.width;
    int new_h = attr.height;
            

    if (x != -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    {
        int yy = 0;
        AdjustForParentClientOrigin( x, yy, sizeFlags);
        new_x = x;
    }
    if (y != -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    {
        int xx = 0;
        AdjustForParentClientOrigin( xx, y, sizeFlags);
        new_y = y;
    }
    if (width != -1)
    {
        new_w = width;
        if (new_w <= 0)
            new_w = 20;
    }
    if (height != -1)
    {
        new_h = height;
        if (new_h <= 0)
            new_h = 20;
    }
    
    DoMoveWindow( new_x, new_y, new_w, new_h );
}

void wxWindowX11::DoSetClientSize(int width, int height)
{
    Window xwindow = (Window) GetMainWindow();

    wxCHECK_RET( xwindow, wxT("invalid window") );

    XWindowAttributes attr;
    Status status = XGetWindowAttributes( wxGlobalDisplay(), xwindow, &attr );
    wxCHECK_RET( status, wxT("invalid window attributes") );
        
    int new_x = attr.x;
    int new_y = attr.y;
    int new_w = attr.width;
    int new_h = attr.height;
            
    if (width != -1)
        new_w = width;
        
    if (height != -1)
        new_h = height;
    
    DoMoveWindow( new_x, new_y, new_w, new_h );
}

// For implementation purposes - sometimes decorations make the client area
// smaller
wxPoint wxWindowX11::GetClientAreaOrigin() const
{
    return wxPoint(0, 0);
}

// Makes an adjustment to the window position (for example, a frame that has
// a toolbar that it manages itself).
void wxWindowX11::AdjustForParentClientOrigin(int& x, int& y, int sizeFlags)
{
    if (((sizeFlags & wxSIZE_NO_ADJUSTMENTS) == 0) && GetParent())
    {
        wxPoint pt(GetParent()->GetClientAreaOrigin());
        x += pt.x; y += pt.y;
    }
}

void wxWindowX11::SetSizeHints(int minW, int minH, int maxW, int maxH, int incW, int incH)
{
    m_minWidth = minW;
    m_minHeight = minH;
    m_maxWidth = maxW;
    m_maxHeight = maxH;

    XSizeHints sizeHints;
    sizeHints.flags = 0;
    
    if (minW > -1 && minH > -1)
    {
        sizeHints.flags |= PMinSize;
        sizeHints.min_width = minW;
        sizeHints.min_height = minH;
    }
    if (maxW > -1 && maxH > -1)
    {
        sizeHints.flags |= PMaxSize;
        sizeHints.max_width = maxW;
        sizeHints.max_height = maxH;
    }
    if (incW > -1 && incH > -1)
    {
        sizeHints.flags |= PResizeInc;
        sizeHints.width_inc = incW;
        sizeHints.height_inc = incH;
    }

    XSetWMNormalHints(wxGlobalDisplay(), (Window) GetMainWindow(), & sizeHints);
}

void wxWindowX11::DoMoveWindow(int x, int y, int width, int height)
{
    Window xwindow = (Window) GetMainWindow();

    wxCHECK_RET( xwindow, wxT("invalid window") );

    XWindowChanges windowChanges;
    windowChanges.x = x;
    windowChanges.y = y;
    windowChanges.width = width;
    windowChanges.height = height;
    windowChanges.stack_mode = 0;
    int valueMask = CWX | CWY | CWWidth | CWHeight;

    XConfigureWindow( wxGlobalDisplay(), xwindow, valueMask, &windowChanges );
}

// ---------------------------------------------------------------------------
// text metrics
// ---------------------------------------------------------------------------

int wxWindowX11::GetCharHeight() const
{
    wxCHECK_MSG( m_font.Ok(), 0, "valid window font needed" );

    WXFontStructPtr pFontStruct = m_font.GetFontStruct(1.0, GetXDisplay());

    int direction, ascent, descent;
    XCharStruct overall;
    XTextExtents ((XFontStruct*) pFontStruct, "x", 1, &direction, &ascent,
        &descent, &overall);

    //  return (overall.ascent + overall.descent);
    return (ascent + descent);
}

int wxWindowX11::GetCharWidth() const
{
    wxCHECK_MSG( m_font.Ok(), 0, "valid window font needed" );

    WXFontStructPtr pFontStruct = m_font.GetFontStruct(1.0, GetXDisplay());

    int direction, ascent, descent;
    XCharStruct overall;
    XTextExtents ((XFontStruct*) pFontStruct, "x", 1, &direction, &ascent,
        &descent, &overall);

    return overall.width;
}

void wxWindowX11::GetTextExtent(const wxString& string,
                             int *x, int *y,
                             int *descent, int *externalLeading,
                             const wxFont *theFont) const
{
    wxFont fontToUse = m_font;
    if (theFont) fontToUse = *theFont;

    wxCHECK_RET( fontToUse.Ok(), wxT("invalid font") );

    WXFontStructPtr pFontStruct = fontToUse.GetFontStruct(1.0, GetXDisplay());

    int direction, ascent, descent2;
    XCharStruct overall;
    int slen = string.Len();

#if 0
    if (use16)
        XTextExtents16((XFontStruct*) pFontStruct, (XChar2b *) (char*) (const char*) string, slen, &direction,
        &ascent, &descent2, &overall);
#endif

    XTextExtents((XFontStruct*) pFontStruct, string.c_str(), slen,
                 &direction, &ascent, &descent2, &overall);

    if ( x )
        *x = (overall.width);
    if ( y )
        *y = (ascent + descent2);
    if (descent)
        *descent = descent2;
    if (externalLeading)
        *externalLeading = 0;

}

// ----------------------------------------------------------------------------
// painting
// ----------------------------------------------------------------------------

void wxWindowX11::Refresh(bool eraseBack, const wxRect *rect)
{
    if (eraseBack)
    {
        if (rect)
        {
            // Schedule for later Updating in ::Update() or ::OnInternalIdle().
            m_clearRegion.Union( rect->x, rect->y, rect->width, rect->height );
        }
        else
        {
            int height,width;
            GetSize( &width, &height );
            
            // Schedule for later Updating in ::Update() or ::OnInternalIdle().
            m_clearRegion.Clear();
            m_clearRegion.Union( 0, 0, width, height );
        }
    }

    if (rect)
    {
         // Schedule for later Updating in ::Update() or ::OnInternalIdle().
         m_updateRegion.Union( rect->x, rect->y, rect->width, rect->height );
    }
    else
    {
        int height,width;
        GetSize( &width, &height );
            
        // Schedule for later Updating in ::Update() or ::OnInternalIdle().
        m_updateRegion.Clear();
        m_updateRegion.Union( 0, 0, width, height );
    }
}

void wxWindowX11::Update()
{
    if (!m_updateRegion.IsEmpty())
    {
        // Actually send erase and paint events.
        X11SendPaintEvents();
    }
}

void wxWindowX11::Clear()
{
    wxClientDC dc((wxWindow*) this);
    wxBrush brush(GetBackgroundColour(), wxSOLID);
    dc.SetBackground(brush);
    dc.Clear();
}

void wxWindowX11::X11SendPaintEvents()
{
    m_clipPaintRegion = TRUE;

    if (!m_clearRegion.IsEmpty())
    {
        wxWindowDC dc( (wxWindow*)this );
        dc.SetClippingRegion( m_clearRegion );
        
        wxEraseEvent erase_event( GetId(), &dc );
        erase_event.SetEventObject( this );
    
        if (!GetEventHandler()->ProcessEvent(erase_event))
        {
            Window xwindow = (Window) GetMainWindow();
            Display *xdisplay = wxGlobalDisplay();
            GC xgc = XCreateGC( xdisplay, xwindow, 0, NULL );
            XSetFillStyle( xdisplay, xgc, FillSolid );
            XSetForeground( xdisplay, xgc, m_backgroundColour.GetPixel() );
            wxRegionIterator upd( m_clearRegion );
            while (upd)
            {
                XFillRectangle( xdisplay, xwindow, xgc,
                                upd.GetX(), upd.GetY(), upd.GetWidth(), upd.GetHeight() );
                upd ++;
            }
            XFreeGC( xdisplay, xgc );
        }
        m_clearRegion.Clear();
    }

    wxNcPaintEvent nc_paint_event( GetId() );
    nc_paint_event.SetEventObject( this );
    GetEventHandler()->ProcessEvent( nc_paint_event );

    wxPaintEvent paint_event( GetId() );
    paint_event.SetEventObject( this );
    GetEventHandler()->ProcessEvent( paint_event );

    m_updateRegion.Clear();
    
    m_clipPaintRegion = FALSE;
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

// Responds to colour changes: passes event on to children.
void wxWindowX11::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    wxWindowList::Node *node = GetChildren().GetFirst();
    while ( node )
    {
        // Only propagate to non-top-level windows
        wxWindow *win = node->GetData();
        if ( win->GetParent() )
        {
            wxSysColourChangedEvent event2;
            event.m_eventObject = win;
            win->GetEventHandler()->ProcessEvent(event2);
        }

        node = node->GetNext();
    }
}

void wxWindowX11::OnInternalIdle()
{
    // Update invalidated regions.
    Update();
    
    // This calls the UI-update mechanism (querying windows for
    // menu/toolbar/control state information)
    UpdateWindowUI();

    // Set the input focus if couldn't do it before
    if (m_needsInputFocus)
	SetFocus();
}

// ----------------------------------------------------------------------------
// function which maintain the global hash table mapping Widgets to wxWindows
// ----------------------------------------------------------------------------

bool wxAddWindowToTable(Window w, wxWindow *win)
{
    wxWindow *oldItem = NULL;
    if ((oldItem = (wxWindow *)wxWidgetHashTable->Get ((long) w)))
    {
        wxLogDebug("Widget table clash: new widget is %ld, %s",
                   (long)w, win->GetClassInfo()->GetClassName());
        return FALSE;
    }

    wxWidgetHashTable->Put((long) w, win);

    wxLogTrace("widget", "XWindow 0x%08x <-> window %p (%s)",
               w, win, win->GetClassInfo()->GetClassName());

    return TRUE;
}

wxWindow *wxGetWindowFromTable(Window w)
{
    return (wxWindow *)wxWidgetHashTable->Get((long) w);
}

void wxDeleteWindowFromTable(Window w)
{
    wxWidgetHashTable->Delete((long)w);
}

// ----------------------------------------------------------------------------
// add/remove window from the table
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// X11-specific accessors
// ----------------------------------------------------------------------------

// Get the underlying X window
WXWindow wxWindowX11::GetXWindow() const
{
    return GetMainWindow();
}

// Get the underlying X display
WXDisplay *wxWindowX11::GetXDisplay() const
{
    return wxGetDisplay();
}

WXWindow wxWindowX11::GetMainWindow() const
{
    return m_mainWidget;
}

// ----------------------------------------------------------------------------
// TranslateXXXEvent() functions
// ----------------------------------------------------------------------------

bool wxTranslateMouseEvent(wxMouseEvent& wxevent, wxWindow *win, Window window, XEvent *xevent)
{
    switch (xevent->xany.type)
    {
        case EnterNotify:
        case LeaveNotify:
        case ButtonPress:
        case ButtonRelease:
        case MotionNotify:
        {
            wxEventType eventType = wxEVT_NULL;

            if (xevent->xany.type == EnterNotify)
            {
                //if (local_event.xcrossing.mode!=NotifyNormal)
                //  return ; // Ignore grab events
                eventType = wxEVT_ENTER_WINDOW;
                //            canvas->GetEventHandler()->OnSetFocus();
            }
            else if (xevent->xany.type == LeaveNotify)
            {
                //if (local_event.xcrossingr.mode!=NotifyNormal)
                //  return ; // Ignore grab events
                eventType = wxEVT_LEAVE_WINDOW;
                //            canvas->GetEventHandler()->OnKillFocus();
            }
            else if (xevent->xany.type == MotionNotify)
            {
                eventType = wxEVT_MOTION;
            }
            else if (xevent->xany.type == ButtonPress)
            {
                wxevent.SetTimestamp(xevent->xbutton.time);
                int button = 0;
                if (xevent->xbutton.button == Button1)
                {
                    eventType = wxEVT_LEFT_DOWN;
                    button = 1;
                }
                else if (xevent->xbutton.button == Button2)
                {
                    eventType = wxEVT_MIDDLE_DOWN;
                    button = 2;
                }
                else if (xevent->xbutton.button == Button3)
                {
                    eventType = wxEVT_RIGHT_DOWN;
                    button = 3;
                }

                // check for a double click
                // TODO: where can we get this value from?
                //long dclickTime = XtGetMultiClickTime(wxGlobalDisplay());
                long dclickTime = 200;
                long ts = wxevent.GetTimestamp();

                int buttonLast = win->GetLastClickedButton();
                long lastTS = win->GetLastClickTime();
                if ( buttonLast && buttonLast == button && (ts - lastTS) < dclickTime )
                {
                    // I have a dclick
                    win->SetLastClick(0, ts);
                    if ( eventType == wxEVT_LEFT_DOWN )
                        eventType = wxEVT_LEFT_DCLICK;
                    else if ( eventType == wxEVT_MIDDLE_DOWN )
                        eventType = wxEVT_MIDDLE_DCLICK;
                    else if ( eventType == wxEVT_RIGHT_DOWN )
                        eventType = wxEVT_RIGHT_DCLICK;
                }
                else
                {
                    // not fast enough or different button
                    win->SetLastClick(button, ts);
                }
            }
            else if (xevent->xany.type == ButtonRelease)
            {
                if (xevent->xbutton.button == Button1)
                {
                    eventType = wxEVT_LEFT_UP;
                }
                else if (xevent->xbutton.button == Button2)
                {
                    eventType = wxEVT_MIDDLE_UP;
                }
                else if (xevent->xbutton.button == Button3)
                {
                    eventType = wxEVT_RIGHT_UP;
                }
                else return FALSE;
            }
            else
            {
                return FALSE;
            }

            wxevent.SetEventType(eventType);

            wxevent.m_x = xevent->xbutton.x;
            wxevent.m_y = xevent->xbutton.y;

            wxevent.m_leftDown = ((eventType == wxEVT_LEFT_DOWN)
                || (event_left_is_down (xevent)
                && (eventType != wxEVT_LEFT_UP)));
            wxevent.m_middleDown = ((eventType == wxEVT_MIDDLE_DOWN)
                || (event_middle_is_down (xevent)
                && (eventType != wxEVT_MIDDLE_UP)));
            wxevent.m_rightDown = ((eventType == wxEVT_RIGHT_DOWN)
                || (event_right_is_down (xevent)
                && (eventType != wxEVT_RIGHT_UP)));

            wxevent.m_shiftDown = xevent->xbutton.state & ShiftMask;
            wxevent.m_controlDown = xevent->xbutton.state & ControlMask;
            wxevent.m_altDown = xevent->xbutton.state & Mod3Mask;
            wxevent.m_metaDown = xevent->xbutton.state & Mod1Mask;

            wxevent.SetId(win->GetId());
            wxevent.SetEventObject(win);

            return TRUE;
        }
    }
    return FALSE;
}

bool wxTranslateKeyEvent(wxKeyEvent& wxevent, wxWindow *win, Window WXUNUSED(win), XEvent *xevent)
{
    switch (xevent->xany.type)
    {
    case KeyPress:
    case KeyRelease:
        {
            char buf[20];

            KeySym keySym;
            (void) XLookupString ((XKeyEvent *) xevent, buf, 20, &keySym, NULL);
            int id = wxCharCodeXToWX (keySym);

            if (xevent->xkey.state & ShiftMask)
                wxevent.m_shiftDown = TRUE;
            if (xevent->xkey.state & ControlMask)
                wxevent.m_controlDown = TRUE;
            if (xevent->xkey.state & Mod3Mask)
                wxevent.m_altDown = TRUE;
            if (xevent->xkey.state & Mod1Mask)
                wxevent.m_metaDown = TRUE;
            wxevent.SetEventObject(win);
            wxevent.m_keyCode = id;
            wxevent.SetTimestamp(xevent->xkey.time);

            wxevent.m_x = xevent->xbutton.x;
            wxevent.m_y = xevent->xbutton.y;

            if (id > -1)
                return TRUE;
            else
                return FALSE;
            break;
        }
    default:
        break;
    }
    return FALSE;
}

// ----------------------------------------------------------------------------
// Colour stuff
// ----------------------------------------------------------------------------

bool wxWindowX11::SetBackgroundColour(const wxColour& col)
{
    wxWindowBase::SetBackgroundColour(col);

    Display *xdisplay = (Display*) wxGlobalDisplay();
    int xscreen = DefaultScreen( xdisplay );
    Colormap cm = DefaultColormap( xdisplay, xscreen );

    m_backgroundColour.CalcPixel( (WXColormap) cm );
    
    if (!GetMainWindow())
        return FALSE;

/*
    XSetWindowAttributes attrib;
    attrib.background_pixel = colour.GetPixel();

    XChangeWindowAttributes(wxGlobalDisplay(),
        (Window) GetMainWindow(),
        CWBackPixel,
        & attrib);
*/

    return TRUE;
}

bool wxWindowX11::SetForegroundColour(const wxColour& col)
{
    if ( !wxWindowBase::SetForegroundColour(col) )
        return FALSE;

    return TRUE;
}

// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------

wxWindow *wxGetActiveWindow()
{
    // TODO
    wxFAIL_MSG("Not implemented");
    return NULL;
}

/* static */
wxWindow *wxWindowBase::GetCapture()
{
    return (wxWindow *)g_captureWindow;
}


// Find the wxWindow at the current mouse position, returning the mouse
// position.
wxWindow* wxFindWindowAtPointer(wxPoint& pt)
{
    return wxFindWindowAtPoint(wxGetMousePosition());
}

// Get the current mouse position.
wxPoint wxGetMousePosition()
{
    Display *display = wxGlobalDisplay();
    Window rootWindow = RootWindowOfScreen (DefaultScreenOfDisplay(display));
    Window rootReturn, childReturn;
    int rootX, rootY, winX, winY;
    unsigned int maskReturn;

    XQueryPointer (display,
		   rootWindow,
		   &rootReturn,
                   &childReturn,
                   &rootX, &rootY, &winX, &winY, &maskReturn);
    return wxPoint(rootX, rootY);
}


// ----------------------------------------------------------------------------
// wxNoOptimize: switch off size optimization
// ----------------------------------------------------------------------------

int wxNoOptimize::ms_count = 0;

