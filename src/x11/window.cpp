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

#include <string.h>

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

static const int SCROLL_MARGIN = 4;

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

IMPLEMENT_DYNAMIC_CLASS(wxWindowX11, wxWindowBase)

BEGIN_EVENT_TABLE(wxWindowX11, wxWindowBase)
    EVT_SYS_COLOUR_CHANGED(wxWindowX11::OnSysColourChanged)
    EVT_IDLE(wxWindowX11::OnIdle)
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

    // Motif-specific
    m_needsRefresh = TRUE;
    m_mainWidget = (WXWidget) 0;

    m_button1Pressed =
    m_button2Pressed =
    m_button3Pressed = FALSE;

    m_winCaptured = FALSE;

    m_isShown = TRUE;
    m_isBeingDeleted = FALSE;
    
    m_hScrollBar =
    m_vScrollBar =
    m_borderWidget =
    m_scrolledWindow =
    m_drawingArea = (WXWidget) 0;

    m_hScroll =
    m_vScroll = FALSE;

    m_scrollPosX =
    m_scrollPosY = 0;

    m_backingPixmap = (WXPixmap) 0;
    m_pixmapWidth =
    m_pixmapHeight = 0;

    m_pixmapOffsetX =
    m_pixmapOffsetY = 0;

    m_lastTS = 0;
    m_lastButton = 0;
    m_canAddEventHandler = FALSE;
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

    m_backgroundColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    m_foregroundColour = *wxBLACK;

    if (style & wxSIMPLE_BORDER)
    {
    } else if (style & wxSUNKEN_BORDER)
    {
    } else if (style & wxRAISED_BORDER)
    {
    }

    // TODO: create XWindow

#if 0
    wxAddWindowToTable((Window) m_drawingArea, this);
    wxAddWindowToTable((Window) m_scrolledWindow, this);
#endif

    // Without this, the cursor may not be restored properly (e.g. in splitter
    // sample).
    SetCursor(*wxSTANDARD_CURSOR);
    SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
    SetSize(pos.x, pos.y, size.x, size.y);

    return TRUE;
}

// Destructor
wxWindowX11::~wxWindow()
{
    if (g_captureWindow == this)
	g_captureWindow = NULL;
    
    m_isBeingDeleted = TRUE;
    
    // Motif-specific actions first
    WXWindow wMain = GetMainWindow();
    if ( wMain )
    {
        // Removes event handlers
        //DetachWidget(wMain);
    }

    ClearUpdateRects();

    if ( m_parent )
        m_parent->RemoveChild( this );

    // TODO

#if 0    
    // If m_drawingArea, we're a fully-fledged window with drawing area,
    // scrollbars etc. (what wxCanvas used to be)
    if ( m_drawingArea )
    {
        // Destroy children before destroying self
        DestroyChildren();

        if (m_backingPixmap)
            XFreePixmap (XtDisplay ((Widget) GetMainWidget()), (Pixmap) m_backingPixmap);

        Widget w = (Widget) m_drawingArea;
        wxDeleteWindowFromTable(w);

        if (w)
        {
            XtDestroyWidget(w);
	    m_drawingArea = (WXWidget) 0;
        }

        // Only if we're _really_ a canvas (not a dialog box/panel)
        if (m_scrolledWindow)
        {
            wxDeleteWindowFromTable((Widget) m_scrolledWindow);
        }

        if (m_hScrollBar)
        {
            wxDeleteWindowFromTable((Widget) m_hScrollBar);
	    XtUnmanageChild((Widget) m_hScrollBar);
        }
        if (m_vScrollBar)
        {
            wxDeleteWindowFromTable((Widget) m_vScrollBar);
	    XtUnmanageChild((Widget) m_vScrollBar);
        }

        if (m_hScrollBar)
	    XtDestroyWidget((Widget) m_hScrollBar);
        if (m_vScrollBar)
	    XtDestroyWidget((Widget) m_vScrollBar);

        UnmanageAndDestroy(m_scrolledWindow);

        if (m_borderWidget)
        {
            XtDestroyWidget ((Widget) m_borderWidget);
            m_borderWidget = (WXWidget) 0;
        }
    }
    else // Why wasn't this here before? JACS 8/3/2000
#endif
        DestroyChildren();


    // Destroy the window
    if (GetMainWindow())
    {
        // TODO
        // XtDestroyWidget((Widget) GetMainWidget());
        SetMainWindow((WXWindow) NULL);
    }
}

// ----------------------------------------------------------------------------
// scrollbar management
// ----------------------------------------------------------------------------

// Helper function
void wxWindowX11::CreateScrollbar(wxOrientation orientation)
{
    // TODO
#if 0
    wxCHECK_RET( m_drawingArea, "this window can't have scrollbars" );

    XtVaSetValues((Widget) m_scrolledWindow, XmNresizePolicy, XmRESIZE_NONE, NULL);

    // Add scrollbars if required
    if (orientation == wxHORIZONTAL)
    {
        Widget hScrollBar = XtVaCreateManagedWidget ("hsb",
            xmScrollBarWidgetClass, (Widget) m_scrolledWindow,
            XmNorientation, XmHORIZONTAL,
            NULL);
        XtAddCallback (hScrollBar, XmNvalueChangedCallback, (XtCallbackProc) wxScrollBarCallback, (XtPointer) XmHORIZONTAL);
        XtAddCallback (hScrollBar, XmNdragCallback, (XtCallbackProc) wxScrollBarCallback, (XtPointer) XmHORIZONTAL);
        XtAddCallback (hScrollBar, XmNincrementCallback, (XtCallbackProc) wxScrollBarCallback, (XtPointer) XmHORIZONTAL);
        XtAddCallback (hScrollBar, XmNdecrementCallback, (XtCallbackProc) wxScrollBarCallback, (XtPointer) XmHORIZONTAL);
        XtAddCallback (hScrollBar, XmNpageIncrementCallback, (XtCallbackProc) wxScrollBarCallback, (XtPointer) XmHORIZONTAL);
        XtAddCallback (hScrollBar, XmNpageDecrementCallback, (XtCallbackProc) wxScrollBarCallback, (XtPointer) XmHORIZONTAL);
        XtAddCallback (hScrollBar, XmNtoTopCallback, (XtCallbackProc) wxScrollBarCallback, (XtPointer) XmHORIZONTAL);
        XtAddCallback (hScrollBar, XmNtoBottomCallback, (XtCallbackProc) wxScrollBarCallback, (XtPointer) XmHORIZONTAL);

        XtVaSetValues (hScrollBar,
            XmNincrement, 1,
            XmNvalue, 0,
            NULL);

        m_hScrollBar = (WXWidget) hScrollBar;

        wxColour backgroundColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
        DoChangeBackgroundColour(m_hScrollBar, backgroundColour, TRUE);

        XtRealizeWidget(hScrollBar);

        XtVaSetValues((Widget) m_scrolledWindow,
            XmNhorizontalScrollBar, (Widget) m_hScrollBar,
            NULL);

        m_hScroll = TRUE;

        wxAddWindowToTable( hScrollBar, this );
    }

    if (orientation == wxVERTICAL)
    {
        Widget vScrollBar = XtVaCreateManagedWidget ("vsb",
            xmScrollBarWidgetClass, (Widget) m_scrolledWindow,
            XmNorientation, XmVERTICAL,
            NULL);
        XtAddCallback (vScrollBar, XmNvalueChangedCallback, (XtCallbackProc) wxScrollBarCallback, (XtPointer) XmVERTICAL);
        XtAddCallback (vScrollBar, XmNdragCallback, (XtCallbackProc) wxScrollBarCallback, (XtPointer) XmVERTICAL);
        XtAddCallback (vScrollBar, XmNincrementCallback, (XtCallbackProc) wxScrollBarCallback, (XtPointer) XmVERTICAL);
        XtAddCallback (vScrollBar, XmNdecrementCallback, (XtCallbackProc) wxScrollBarCallback, (XtPointer) XmVERTICAL);
        XtAddCallback (vScrollBar, XmNpageIncrementCallback, (XtCallbackProc) wxScrollBarCallback, (XtPointer) XmVERTICAL);
        XtAddCallback (vScrollBar, XmNpageDecrementCallback, (XtCallbackProc) wxScrollBarCallback, (XtPointer) XmVERTICAL);
        XtAddCallback (vScrollBar, XmNtoTopCallback, (XtCallbackProc) wxScrollBarCallback, (XtPointer) XmVERTICAL);
        XtAddCallback (vScrollBar, XmNtoBottomCallback, (XtCallbackProc) wxScrollBarCallback, (XtPointer) XmVERTICAL);

        XtVaSetValues (vScrollBar,
            XmNincrement, 1,
            XmNvalue, 0,
            NULL);

        m_vScrollBar = (WXWidget) vScrollBar;
        wxColour backgroundColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
        DoChangeBackgroundColour(m_vScrollBar, backgroundColour, TRUE);

        XtRealizeWidget(vScrollBar);

        XtVaSetValues((Widget) m_scrolledWindow,
            XmNverticalScrollBar, (Widget) m_vScrollBar,
            NULL);

        m_vScroll = TRUE;

        wxAddWindowToTable( vScrollBar, this );
    }

    XtVaSetValues((Widget) m_scrolledWindow, XmNresizePolicy, XmRESIZE_ANY, NULL);
#endif
}

void wxWindowX11::DestroyScrollbar(wxOrientation orientation)
{
    // TODO
#if 0
    wxCHECK_RET( m_drawingArea, "this window can't have scrollbars" );

    XtVaSetValues((Widget) m_scrolledWindow, XmNresizePolicy, XmRESIZE_NONE, NULL);
    // Add scrollbars if required
    if (orientation == wxHORIZONTAL)
    {
        if (m_hScrollBar)
        {
            wxDeleteWindowFromTable((Widget)m_hScrollBar);
            XtDestroyWidget((Widget) m_hScrollBar);
        }
        m_hScrollBar = (WXWidget) 0;
        m_hScroll = FALSE;

        XtVaSetValues((Widget) m_scrolledWindow,
            XmNhorizontalScrollBar, (Widget) 0,
            NULL);

    }

    if (orientation == wxVERTICAL)
    {
        if (m_vScrollBar)
        {
            wxDeleteWindowFromTable((Widget)m_vScrollBar);
            XtDestroyWidget((Widget) m_vScrollBar);
        }
        m_vScrollBar = (WXWidget) 0;
        m_vScroll = FALSE;

        XtVaSetValues((Widget) m_scrolledWindow,
            XmNverticalScrollBar, (Widget) 0,
            NULL);

    }
    XtVaSetValues((Widget) m_scrolledWindow, XmNresizePolicy, XmRESIZE_ANY, NULL);
#endif
}

// ---------------------------------------------------------------------------
// basic operations
// ---------------------------------------------------------------------------

void wxWindowX11::SetFocus()
{
    // TODO
#if 0
    Widget wMain = (Widget) GetMainWidget();
    XmProcessTraversal(wMain, XmTRAVERSE_CURRENT);
    XmProcessTraversal((Widget) GetMainWidget(), XmTRAVERSE_CURRENT);
#endif
}

// Get the window with the focus
wxWindow *wxWindowBase::FindFocus()
{
    // TODO
    return NULL;
#if 0

    // TODO Problems:
    // (1) Can there be multiple focussed widgets in an application?
    // In which case we need to find the top-level window that's
    // currently active.
    // (2) The widget with the focus may not be in the widget table
    // depending on which widgets I put in the table
    wxWindow *winFocus = (wxWindow *)NULL;
    for ( wxWindowList::Node *node = wxTopLevelWindows.GetFirst();
          node;
          node = node->GetNext() )
    {
        wxWindow *win = node->GetData();

        Widget w = XmGetFocusWidget ((Widget) win->GetTopWidget());

        if (w != (Widget) NULL)
        {
            winFocus = wxGetWindowFromTable(w);
            if ( winFocus )
                break;
        }
    }

    return winFocus;
#endif
}

bool wxWindowX11::Enable(bool enable)
{
    if ( !wxWindowBase::Enable(enable) )
        return FALSE;

    // TODO
#if 0
    Widget wMain = (Widget)GetMainWidget();
    if ( wMain )
    {
        XtSetSensitive(wMain, enable);
        XmUpdateDisplay(wMain);
    }
#endif

    return TRUE;
}

bool wxWindowX11::Show(bool show)
{
    if ( !wxWindowBase::Show(show) )
        return FALSE;

    Window xwin = (Window) GetXWindow();
    Display *xdisp = (Display*) GetXDisplay();
    if (show)
        XMapWindow(xdisp, xwin);
    else
        XUnmapWindow(xdisp, xwin);

    return TRUE;
}

// Raise the window to the top of the Z order
void wxWindowX11::Raise()
{
    Window window = GetTopWindow();
    if (window)
        XRaiseWindow(wxGetDisplay(), window);
}

// Lower the window to the bottom of the Z order
void wxWindowX11::Lower()
{
    Window window = GetTopWindow();
    if (window)
        XLowerWindow(wxGetDisplay(), window);
}

void wxWindowX11::SetTitle(const wxString& title)
{
    // TODO
//    XtVaSetValues((Widget)GetMainWidget(), XmNtitle, title.c_str(), NULL);
}

wxString wxWindowX11::GetTitle() const
{
    // TODO
    return wxEmptyString;
#if 0
    char *title;
    XtVaGetValues((Widget)GetMainWidget(), XmNtitle, &title, NULL);

    return wxString(title);
#endif
}

void wxWindowX11::DoCaptureMouse()
{
    g_captureWindow = this;
    if ( m_winCaptured )
        return;

    // TODO
#if 0
    Widget wMain = (Widget)GetMainWidget();
    if ( wMain )
        XtAddGrab(wMain, TRUE, FALSE);
#endif

    m_winCaptured = TRUE;
}

void wxWindowX11::DoReleaseMouse()
{
    g_captureWindow = NULL;
    if ( !m_winCaptured )
        return;

    // TODO
#if 0
    Widget wMain = (Widget)GetMainWidget();
    if ( wMain )
        XtRemoveGrab(wMain);
#endif

    m_winCaptured = FALSE;
}

bool wxWindowX11::SetFont(const wxFont& font)
{
    if ( !wxWindowBase::SetFont(font) )
    {
        // nothing to do
        return FALSE;
    }

    ChangeFont();

    return TRUE;
}

bool wxWindowX11::SetCursor(const wxCursor& cursor)
{
    if ( !wxWindowBase::SetCursor(cursor) )
    {
        // no change
        return FALSE;
    }

    wxCursor* cursor2 = NULL;
    if (m_cursor.Ok())
        cursor2 = & m_cursor;
    else
        cursor2 = wxSTANDARD_CURSOR;

    WXDisplay *dpy = GetXDisplay();
    WXCursor x_cursor = cursor2->GetXCursor(dpy);

    Window win = (Window) GetMainWindow();
    XDefineCursor((Display*) dpy, win, (Cursor) x_cursor);

    return TRUE;
}

// Coordinates relative to the window
void wxWindowX11::WarpPointer (int x, int y)
{
    Window wClient = (Window) GetClientWindow();

    XWarpPointer(wxGetDisplay(), None, wClient, 0, 0, 0, 0, x, y);
}

// ---------------------------------------------------------------------------
// scrolling stuff
// ---------------------------------------------------------------------------

int wxWindowX11::GetScrollPos(int orient) const
{
    if (orient == wxHORIZONTAL)
        return m_scrollPosX;
    else
        return m_scrollPosY;
}

// This now returns the whole range, not just the number of positions that we
// can scroll.
int wxWindowX11::GetScrollRange(int WXUNUSED(orient)) const
{
    // TODO
    return 0;
#if 0
    Widget scrollBar = (Widget)GetScrollbar((wxOrientation)orient);
    wxCHECK_MSG( scrollBar, 0, "no such scrollbar" );

    int range;
    XtVaGetValues(scrollBar, XmNmaximum, &range, NULL);
    return range;
#endif
}

int wxWindowX11::GetScrollThumb(int orient) const
{
    // TODO
    return 0;

#if 0
    Widget scrollBar = (Widget)GetScrollbar((wxOrientation)orient);
    wxCHECK_MSG( scrollBar, 0, "no such scrollbar" );

    int thumb;
    XtVaGetValues(scrollBar, XmNsliderSize, &thumb, NULL);
    return thumb;
#endif
}

void wxWindowX11::SetScrollPos(int WXUNUSED(orient), int WXUNUSED(pos), bool WXUNUSED(refresh))
{
    // TODO

#if 0
    Widget scrollBar = (Widget)GetScrollbar((wxOrientation)orient);

    if ( scrollBar )
    {
        XtVaSetValues (scrollBar, XmNvalue, pos, NULL);
    }
#endif
    SetInternalScrollPos((wxOrientation)orient, pos);
}

// New function that will replace some of the above.
void wxWindowX11::SetScrollbar(int WXUNUSED(orient), int WXUNUSED(pos), int WXUNUSED(thumbVisible),
                            int WXUNUSED(range), bool WXUNUSED(refresh))
{
    // TODO
#if 0
    int oldW, oldH;
    GetSize(& oldW, & oldH);

    if (range == 0)
        range = 1;
    if (thumbVisible == 0)
        thumbVisible = 1;

    if (thumbVisible > range)
        thumbVisible = range;

    // Save the old state to see if it changed
    WXWidget oldScrollBar = GetScrollbar((wxOrientation)orient);

    if (orient == wxHORIZONTAL)
    {
        if (thumbVisible == range)
        {
            if (m_hScrollBar)
                DestroyScrollbar(wxHORIZONTAL);
        }
        else
        {
            if (!m_hScrollBar)
                CreateScrollbar(wxHORIZONTAL);
        }
    }
    if (orient == wxVERTICAL)
    {
        if (thumbVisible == range)
        {
            if (m_vScrollBar)
                DestroyScrollbar(wxVERTICAL);
        }
        else
        {
            if (!m_vScrollBar)
                CreateScrollbar(wxVERTICAL);
        }
    }
    WXWidget newScrollBar =  GetScrollbar((wxOrientation)orient);

    if (oldScrollBar != newScrollBar)
    {
        // This is important! Without it, scrollbars misbehave badly.
        XtUnrealizeWidget((Widget) m_scrolledWindow);
        XmScrolledWindowSetAreas ((Widget) m_scrolledWindow, (Widget) m_hScrollBar, (Widget) m_vScrollBar, (Widget) m_drawingArea);
        XtRealizeWidget((Widget) m_scrolledWindow);
        XtManageChild((Widget) m_scrolledWindow);
    }

    if (newScrollBar)
    {
        XtVaSetValues((Widget) newScrollBar,
        XmNvalue, pos,
        XmNminimum, 0,
        XmNmaximum, range,
        XmNsliderSize, thumbVisible,
        NULL);
    }

    SetInternalScrollPos((wxOrientation)orient, pos);

    int newW, newH;
    GetSize(& newW, & newH);

    // Adjusting scrollbars can resize the canvas accidentally
    if (newW != oldW || newH != oldH)
        SetSize(-1, -1, oldW, oldH);
#endif
}

// Does a physical scroll
void wxWindowX11::ScrollWindow(int dx, int dy, const wxRect *rect)
{
    int x, y, w, h;
    if (rect)
    {
        // Use specified rectangle
        x = rect->x; y = rect->y; w = rect->width; h = rect->height;
    }
    else
    {
        // Use whole client area
        x = 0; y = 0;
        GetClientSize(& w, & h);
    }
    
    wxNode *cnode = m_children.First();
    while (cnode)
    {
        wxWindow *child = (wxWindow*) cnode->Data();
        int sx = 0;
        int sy = 0;
        child->GetSize( &sx, &sy );
        wxPoint pos( child->GetPosition() );
        child->SetSize( pos.x + dx, pos.y + dy, sx, sy, wxSIZE_ALLOW_MINUS_ONE );
        cnode = cnode->Next();
    }
    
    int x1 = (dx >= 0) ? x : x - dx;
    int y1 = (dy >= 0) ? y : y - dy;
    int w1 = w - abs(dx);
    int h1 = h - abs(dy);
    int x2 = (dx >= 0) ? x + dx : x;
    int y2 = (dy >= 0) ? y + dy : y;
    
    wxClientDC dc(this);
    
    dc.SetLogicalFunction (wxCOPY);
    
    Window window = (Window) GetMainWindow();
    Display* display = wxGetDisplay();
    
    XCopyArea(display, window, window, (GC) dc.GetGC(),
        x1, y1, w1, h1, x2, y2);
    
    dc.SetAutoSetting(TRUE);
    wxBrush brush(GetBackgroundColour(), wxSOLID);
    dc.SetBrush(brush); // FIXME: needed?
    
    // We'll add rectangles to the list of update rectangles according to which
    // bits we've exposed.
    wxList updateRects;
    
    if (dx > 0)
    {
        wxRect *rect = new wxRect;
        rect->x = x;
        rect->y = y;
        rect->width = dx;
        rect->height = h;
        
        XFillRectangle(display, window,
            (GC) dc.GetGC(), rect->x, rect->y, rect->width, rect->height);
        
        rect->x = rect->x;
        rect->y = rect->y;
        rect->width = rect->width;
        rect->height = rect->height;
        
        updateRects.Append((wxObject*) rect);
    }
    else if (dx < 0)
    {
        wxRect *rect = new wxRect;
        
        rect->x = x + w + dx;
        rect->y = y;
        rect->width = -dx;
        rect->height = h;
        
        XFillRectangle(display, window,
            (GC) dc.GetGC(), rect->x, rect->y, rect->width,
            rect->height);
        
        rect->x = rect->x;
        rect->y = rect->y;
        rect->width = rect->width;
        rect->height = rect->height;
        
        updateRects.Append((wxObject*) rect);
    }
    if (dy > 0)
    {
        wxRect *rect = new wxRect;
        
        rect->x = x;
        rect->y = y;
        rect->width = w;
        rect->height = dy;
        
        XFillRectangle(display, window,
            (GC) dc.GetGC(), rect->x, rect->y, rect->width, rect->height);
        
        rect->x = rect->x;
        rect->y = rect->y;
        rect->width = rect->width;
        rect->height = rect->height;
        
        updateRects.Append((wxObject*) rect);
    }
    else if (dy < 0)
    {
        wxRect *rect = new wxRect;
        
        rect->x = x;
        rect->y = y + h + dy;
        rect->width = w;
        rect->height = -dy;
        
        XFillRectangle(display, window,
            (GC) dc.GetGC(), rect->x, rect->y, rect->width, rect->height);
        
        rect->x = rect->x;
        rect->y = rect->y;
        rect->width = rect->width;
        rect->height = rect->height;
        
        updateRects.Append((wxObject*) rect);
    }
    dc.SetBrush(wxNullBrush);
    
    // Now send expose events
    
    wxNode* node = updateRects.First();
    while (node)
    {
        wxRect* rect = (wxRect*) node->Data();
        XExposeEvent event;
        
        event.type = Expose;
        event.display = display;
        event.send_event = True;
        event.window = window;
        
        event.x = rect->x;
        event.y = rect->y;
        event.width = rect->width;
        event.height = rect->height;
        
        event.count = 0;
        
        XSendEvent(display, window, False, ExposureMask, (XEvent *)&event);
        
        node = node->Next();
        
    }
    
    // Delete the update rects
    node = updateRects.First();
    while (node)
    {
        wxRect* rect = (wxRect*) node->Data();
        delete rect;
        node = node->Next();
    }
    
    // TODO

    // XmUpdateDisplay((Widget) GetMainWidget());
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
    // TODO
#if 0
    if (m_drawingArea)
    {
        CanvasGetSize(x, y);
        return;
    }

    Widget widget = (Widget) GetTopWidget();
    Dimension xx, yy;
    XtVaGetValues(widget, XmNwidth, &xx, XmNheight, &yy, NULL);
    if(x) *x = xx; if(y) *y = yy;
#endif
}

void wxWindowX11::DoGetPosition(int *x, int *y) const
{
    // TODO
#if 0
    if (m_drawingArea)
    {
        CanvasGetPosition(x, y);
        return;
    }
    Widget widget = (Widget) GetTopWidget();
    Position xx, yy;
    XtVaGetValues(widget, XmNx, &xx, XmNy, &yy, NULL);

    // We may be faking the client origin. So a window that's really at (0, 30)
    // may appear (to wxWin apps) to be at (0, 0).
    if (GetParent())
    {
        wxPoint pt(GetParent()->GetClientAreaOrigin());
        xx -= pt.x;
        yy -= pt.y;
    }

    if(x) *x = xx; if(y) *y = yy;
#endif
}

void wxWindowX11::DoScreenToClient(int *x, int *y) const
{
    Display *display = wxGetDisplay();
    Window rootWindow = RootWindowOfScreen(DefaultScreenOfDisplay(display));
    Window thisWindow = (Window) GetClientWindow();

    Window childWindow;
    int xx = *x;
    int yy = *y;
    XTranslateCoordinates(display, rootWindow, thisWindow, xx, yy, x, y, &childWindow);
}

void wxWindowX11::DoClientToScreen(int *x, int *y) const
{
    Display *display = wxGetDisplay();
    Window rootWindow = RootWindowOfScreen(DefaultScreenOfDisplay(display));
    Window thisWindow = (Window) GetClientWindow();

    Window childWindow;
    int xx = *x;
    int yy = *y;
    XTranslateCoordinates(display, thisWindow, rootWindow, xx, yy, x, y, &childWindow);
}


// Get size *available for subwindows* i.e. excluding menu bar etc.
void wxWindowX11::DoGetClientSize(int *x, int *y) const
{
    // TODO
#if 0
    Widget widget = (Widget) GetClientWidget();
    Dimension xx, yy;
    XtVaGetValues(widget, XmNwidth, &xx, XmNheight, &yy, NULL);
    if(x) *x = xx; if(y) *y = yy;
#endif
}

void wxWindowX11::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
    // TODO
#if 0
    // A bit of optimization to help sort out the flickers.
    int oldX, oldY, oldW, oldH;
    GetSize(& oldW, & oldH);
    GetPosition(& oldX, & oldY);

    if ( !(sizeFlags & wxSIZE_ALLOW_MINUS_ONE) )
    {
        if ( x == -1 )
            x = oldX;
        if ( y == -1 )
            y = oldY;
    }

    if ( width == -1 )
        width = oldW;
    if ( height == -1 )
        height = oldH;

    bool nothingChanged = (x == oldX) && (y == oldY) &&
                          (width == oldW) && (height == oldH);

    if (!wxNoOptimize::CanOptimize())
    {
        nothingChanged = FALSE;
    }

    if ( !nothingChanged )
    {
        if (m_drawingArea)
        {
            CanvasSetSize(x, y, width, height, sizeFlags);
            return;
        }

        Widget widget = (Widget) GetTopWidget();
        if (!widget)
            return;

        bool managed = XtIsManaged( widget );
        if (managed)
            XtUnmanageChild(widget);

        int xx = x;
        int yy = y;
        AdjustForParentClientOrigin(xx, yy, sizeFlags);

        DoMoveWindow(xx, yy, width, height);

        if (managed)
            XtManageChild(widget);

    }
#endif
}

void wxWindowX11::DoSetClientSize(int width, int height)
{
    // TODO
#if 0
    if (m_drawingArea)
    {
        CanvasSetClientSize(width, height);
        return;
    }

    Widget widget = (Widget) GetTopWidget();

    if (width > -1)
        XtVaSetValues(widget, XmNwidth, width, NULL);
    if (height > -1)
        XtVaSetValues(widget, XmNheight, height, NULL);

    wxSizeEvent sizeEvent(wxSize(width, height), GetId());
    sizeEvent.SetEventObject(this);

    GetEventHandler()->ProcessEvent(sizeEvent);
#endif
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
    // TODO
#if 0
    m_minWidth = minW;
    m_minHeight = minH;
    m_maxWidth = maxW;
    m_maxHeight = maxH;

    wxFrame *frame = wxDynamicCast(this, wxFrame);
    if ( !frame )
    {
        // TODO what about dialogs?
        return;
    }

    Widget widget = (Widget) frame->GetShellWidget();

    if (minW > -1)
        XtVaSetValues(widget, XmNminWidth, minW, NULL);
    if (minH > -1)
        XtVaSetValues(widget, XmNminHeight, minH, NULL);
    if (maxW > -1)
        XtVaSetValues(widget, XmNmaxWidth, maxW, NULL);
    if (maxH > -1)
        XtVaSetValues(widget, XmNmaxHeight, maxH, NULL);
    if (incW > -1)
        XtVaSetValues(widget, XmNwidthInc, incW, NULL);
    if (incH > -1)
        XtVaSetValues(widget, XmNheightInc, incH, NULL);
#endif
}

void wxWindowX11::DoMoveWindow(int x, int y, int width, int height)
{
    // TODO
#if 0
    XtVaSetValues((Widget)GetTopWidget(),
                  XmNx, x,
                  XmNy, y,
                  XmNwidth, width,
                  XmNheight, height,
                  NULL);
#endif
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
    wxFont *fontToUse = (wxFont *)theFont;
    if (!fontToUse)
        fontToUse = (wxFont *) & m_font;

    wxCHECK_RET( fontToUse->Ok(), "valid window font needed" );
    
    WXFontStructPtr pFontStruct = theFont->GetFontStruct(1.0, GetXDisplay());

    int direction, ascent, descent2;
    XCharStruct overall;
    int slen = string.Len();

#if 0
    if (use16)
        XTextExtents16((XFontStruct*) pFontStruct, (XChar2b *) (char*) (const char*) string, slen, &direction,
        &ascent, &descent2, &overall);
#endif

    XTextExtents((XFontStruct*) pFontStruct, string, slen,
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
    m_needsRefresh = TRUE;
    Display *display = wxGetDisplay();
    Window thisWindow = (Widget) GetMainWindow();

    XExposeEvent dummyEvent;
    int width, height;
    GetSize(&width, &height);

    dummyEvent.type = Expose;
    dummyEvent.display = display;
    dummyEvent.send_event = True;
    dummyEvent.window = thisWindow;
    if (rect)
    {
        dummyEvent.x = rect->x;
        dummyEvent.y = rect->y;
        dummyEvent.width = rect->width;
        dummyEvent.height = rect->height;
    }
    else
    {
        dummyEvent.x = 0;
        dummyEvent.y = 0;
        dummyEvent.width = width;
        dummyEvent.height = height;
    }
    dummyEvent.count = 0;

    if (eraseBack)
    {
        wxClientDC dc(this);
        wxBrush backgroundBrush(GetBackgroundColour(), wxSOLID);
        dc.SetBackground(backgroundBrush);
        if (rect)
            dc.Clear(*rect);
        else
            dc.Clear();
    }

    XSendEvent(display, thisWindow, False, ExposureMask, (XEvent *)&dummyEvent);
}

void wxWindowX11::Clear()
{
    wxClientDC dc(this);
    wxBrush brush(GetBackgroundColour(), wxSOLID);
    dc.SetBackground(brush);
    dc.Clear();
}

void wxWindowX11::ClearUpdateRects()
{
    wxRectList::Node* node = m_updateRects.GetFirst();
    while (node)
    {
        wxRect* rect = node->GetData();
        delete rect;
        node = node->GetNext();
    }

    m_updateRects.Clear();
}

void wxWindowX11::DoPaint()
{
    // Set an erase event first
    wxEraseEvent eraseEvent(GetId());
    eraseEvent.SetEventObject(this);
    GetEventHandler()->ProcessEvent(eraseEvent);
    
    wxPaintEvent event(GetId());
    event.SetEventObject(this);
    GetEventHandler()->ProcessEvent(event);
    
    m_needsRefresh = FALSE;
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

void wxWindowX11::OnIdle(wxIdleEvent& WXUNUSED(event))
{
    // This calls the UI-update mechanism (querying windows for
    // menu/toolbar/control state information)
    UpdateWindowUI();
}

// ----------------------------------------------------------------------------
// accelerators
// ----------------------------------------------------------------------------

bool wxWindowX11::ProcessAccelerator(wxKeyEvent& event)
{
    if (!m_acceleratorTable.Ok())
        return FALSE;

    int count = m_acceleratorTable.GetCount();
    wxAcceleratorEntry* entries = m_acceleratorTable.GetEntries();
    int i;
    for (i = 0; i < count; i++)
    {
        wxAcceleratorEntry* entry = & (entries[i]);
        if (entry->MatchesEvent(event))
        {
            // Bingo, we have a match. Now find a control that matches the
	    // entry command id.

            // Need to go up to the top of the window hierarchy, since it might
            // be e.g. a menu item
            wxWindow* parent = this;
            while ( parent && !parent->IsTopLevel() )
                parent = parent->GetParent();

            if (!parent)
                return FALSE;

            wxFrame* frame = wxDynamicCast(parent, wxFrame);
            if ( frame )
            {
                // Try for a menu command
                if (frame->GetMenuBar())
                {
                    wxMenuItem* item = frame->GetMenuBar()->FindItem(entry->GetCommand());
                    if (item)
                    {
                        wxCommandEvent commandEvent(wxEVT_COMMAND_MENU_SELECTED, entry->GetCommand());
                        commandEvent.SetEventObject(frame);

                        // If ProcessEvent returns TRUE (it was handled), then
                        // the calling code will skip the event handling.
                        return frame->GetEventHandler()->ProcessEvent(commandEvent);
                    }
                }
            }

            // Find a child matching the command id
            wxWindow* child = parent->FindWindow(entry->GetCommand());

            // No such child
            if (!child)
                return FALSE;

            // Now we process those kinds of windows that we can.
            // For now, only buttons.
            if ( wxDynamicCast(child, wxButton) )
            {
                wxCommandEvent commandEvent (wxEVT_COMMAND_BUTTON_CLICKED, child->GetId());
                commandEvent.SetEventObject(child);
                return child->GetEventHandler()->ProcessEvent(commandEvent);
            }

            return FALSE;
        } // matches event
    }// for

    // We didn't match the key event against an accelerator.
    return FALSE;
}

// ============================================================================
// X11-specific stuff from here on
// ============================================================================

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

// Add to hash table, add event handler
bool wxWindowX11::AttachWidget (wxWindow* WXUNUSED(parent), WXWindow mainWidget,
                             int x, int y, int width, int height)
{
    wxAddWindowToTable((Window ) mainWidget, this);

    // TODO
#if 0
    if (CanAddEventHandler())
    {
        XtAddEventHandler((Widget) mainWidget,
            ButtonPressMask | ButtonReleaseMask | PointerMotionMask, // | KeyPressMask,
            False,
            wxPanelItemEventHandler,
            (XtPointer) this);
    }

    if (!formWidget)
    {
        XtTranslations ptr;
        XtOverrideTranslations ((Widget) mainWidget,
            ptr = XtParseTranslationTable ("<Configure>: resize()"));
        XtFree ((char *) ptr);
    }

    // Some widgets have a parent form widget, e.g. wxRadioBox
    if (formWidget)
    {
        if (!wxAddWindowToTable((Widget) formWidget, this))
            return FALSE;

        XtTranslations ptr;
        XtOverrideTranslations ((Widget) formWidget,
            ptr = XtParseTranslationTable ("<Configure>: resize()"));
        XtFree ((char *) ptr);
    }
#endif
    if (x == -1)
        x = 0;
    if (y == -1)
        y = 0;

    SetSize (x, y, width, height);

    return TRUE;
}

// Remove event handler, remove from hash table
bool wxWindowX11::DetachWidget(WXWindow widget)
{
    // TODO
#if 0
    if (CanAddEventHandler())
    {
        XtRemoveEventHandler((Widget) widget,
            ButtonPressMask | ButtonReleaseMask | PointerMotionMask, // | KeyPressMask,
            False,
            wxPanelItemEventHandler,
            (XtPointer)this);
    }
#endif

    wxDeleteWindowFromTable((Window) widget);
    return TRUE;
}

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
    if (m_drawingArea)
        return m_drawingArea;
    else
        return m_mainWidget;
}

WXWindow wxWindowX11::GetClientWidget() const
{
    if (m_drawingArea != (WXWindow) 0)
        return m_drawingArea;
    else
        return GetMainWindow();
}

WXWindow wxWindowX11::GetTopWindow() const
{
    return GetMainWindow();
}

WXWindow wxWindowX11::GetLabelWindow() const
{
    return GetMainWindow();
}

// ----------------------------------------------------------------------------
// callbacks
// ----------------------------------------------------------------------------

// TODO: implement wxWindow scrollbar, presumably using wxScrollBar
#if 0
static void wxScrollBarCallback(Widget scrollbar,
                                XtPointer clientData,
                                XmScrollBarCallbackStruct *cbs)
{
    wxWindow *win = wxGetWindowFromTable(scrollbar);
    int orientation = (int) clientData;

    wxEventType eventType = wxEVT_NULL;
    switch (cbs->reason)
    {
    case XmCR_INCREMENT:
        {
            eventType = wxEVT_SCROLLWIN_LINEDOWN;
            break;
        }
    case XmCR_DECREMENT:
        {
            eventType = wxEVT_SCROLLWIN_LINEUP;
            break;
        }
    case XmCR_DRAG:
        {
            eventType = wxEVT_SCROLLWIN_THUMBTRACK;
            break;
        }
    case XmCR_VALUE_CHANGED:
        {
            eventType = wxEVT_SCROLLWIN_THUMBRELEASE;
            break;
        }
    case XmCR_PAGE_INCREMENT:
        {
            eventType = wxEVT_SCROLLWIN_PAGEDOWN;
            break;
        }
    case XmCR_PAGE_DECREMENT:
        {
            eventType = wxEVT_SCROLLWIN_PAGEUP;
            break;
        }
    case XmCR_TO_TOP:
        {
            eventType = wxEVT_SCROLLWIN_TOP;
            break;
        }
    case XmCR_TO_BOTTOM:
        {
            eventType = wxEVT_SCROLLWIN_BOTTOM;
            break;
        }
    default:
        {
            // Should never get here
            wxFAIL_MSG("Unknown scroll event.");
            break;
        }
    }

    wxScrollWinEvent event(eventType,
                           cbs->value,
                           ((orientation == XmHORIZONTAL) ?
                            wxHORIZONTAL : wxVERTICAL));
    event.SetEventObject( win );
    win->GetEventHandler()->ProcessEvent(event);
}
#endif


// ----------------------------------------------------------------------------
// CanvaseXXXSize() functions
// ----------------------------------------------------------------------------

// SetSize, but as per old wxCanvas (with drawing widget etc.)
void wxWindowX11::CanvasSetSize (int x, int y, int w, int h, int sizeFlags)
{
    // TODO
#if 0
    // A bit of optimization to help sort out the flickers.
    int oldX, oldY, oldW, oldH;
    GetSize(& oldW, & oldH);
    GetPosition(& oldX, & oldY);

    bool useOldPos = FALSE;
    bool useOldSize = FALSE;

    if ((x == -1) && (x == -1) && ((sizeFlags & wxSIZE_ALLOW_MINUS_ONE) == 0))
        useOldPos = TRUE;
    else if (x == oldX && y == oldY)
        useOldPos = TRUE;

    if ((w == -1) && (h == -1))
        useOldSize = TRUE;
    else if (w == oldW && h == oldH)
        useOldSize = TRUE;

    if (!wxNoOptimize::CanOptimize())
    {
        useOldSize = FALSE; useOldPos = FALSE;
    }

    if (useOldPos && useOldSize)
        return;

    Widget drawingArea = (Widget) m_drawingArea;
    bool managed = XtIsManaged(m_borderWidget ? (Widget) m_borderWidget : (Widget) m_scrolledWindow);

    if (managed)
        XtUnmanageChild (m_borderWidget ? (Widget) m_borderWidget : (Widget) m_scrolledWindow);
    XtVaSetValues(drawingArea, XmNresizePolicy, XmRESIZE_ANY, NULL);

    int xx = x; int yy = y;
    AdjustForParentClientOrigin(xx, yy, sizeFlags);

    if (!useOldPos)
    {
        if (x > -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
        {
            XtVaSetValues (m_borderWidget ? (Widget) m_borderWidget : (Widget) m_scrolledWindow,
                XmNx, xx, NULL);
        }

        if (y > -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
        {
            XtVaSetValues (m_borderWidget ? (Widget) m_borderWidget : (Widget) m_scrolledWindow,
                XmNy, yy, NULL);
        }
    }

    if (!useOldSize)
    {

        if (w > -1)
        {
            if (m_borderWidget)
            {
                XtVaSetValues ((Widget) m_borderWidget, XmNwidth, w, NULL);
                short thick, margin;
                XtVaGetValues ((Widget) m_borderWidget,
                    XmNshadowThickness, &thick,
                    XmNmarginWidth, &margin,
                    NULL);
                w -= 2 * (thick + margin);
            }

            XtVaSetValues ((Widget) m_scrolledWindow, XmNwidth, w, NULL);

            Dimension spacing;
            Widget sbar;
            XtVaGetValues ((Widget) m_scrolledWindow,
                XmNspacing, &spacing,
                XmNverticalScrollBar, &sbar,
                NULL);
            Dimension wsbar;
            if (sbar)
                XtVaGetValues (sbar, XmNwidth, &wsbar, NULL);
            else
                wsbar = 0;

            w -= (spacing + wsbar);

#if 0
            XtVaSetValues(drawingArea, XmNwidth, w, NULL);
#endif // 0
        }
        if (h > -1)
        {
            if (m_borderWidget)
            {
                XtVaSetValues ((Widget) m_borderWidget, XmNheight, h, NULL);
                short thick, margin;
                XtVaGetValues ((Widget) m_borderWidget,
                    XmNshadowThickness, &thick,
                    XmNmarginHeight, &margin,
                    NULL);
                h -= 2 * (thick + margin);
            }

            XtVaSetValues ((Widget) m_scrolledWindow, XmNheight, h, NULL);

            Dimension spacing;
            Widget sbar;
            XtVaGetValues ((Widget) m_scrolledWindow,
                XmNspacing, &spacing,
                XmNhorizontalScrollBar, &sbar,
                NULL);
            Dimension wsbar;
            if (sbar)
                XtVaGetValues (sbar, XmNheight, &wsbar, NULL);
            else
                wsbar = 0;

            h -= (spacing + wsbar);

#if 0
            XtVaSetValues(drawingArea, XmNheight, h, NULL);
#endif // 0
        }
    }

    if (managed)
        XtManageChild (m_borderWidget ? (Widget) m_borderWidget : (Widget) m_scrolledWindow);
    XtVaSetValues(drawingArea, XmNresizePolicy, XmRESIZE_NONE, NULL);
#endif
    // 0
}

void wxWindowX11::CanvasSetClientSize (int w, int h)
{
// TODO
#if 0
    Widget drawingArea = (Widget) m_drawingArea;

    XtVaSetValues(drawingArea, XmNresizePolicy, XmRESIZE_ANY, NULL);

    if (w > -1)
        XtVaSetValues(drawingArea, XmNwidth, w, NULL);
    if (h > -1)
        XtVaSetValues(drawingArea, XmNheight, h, NULL);

    XtVaSetValues(drawingArea, XmNresizePolicy, XmRESIZE_NONE, NULL);
#endif // 0
}

void wxWindowX11::CanvasGetClientSize (int *w, int *h) const
{
// TODO
#if 0
    // Must return the same thing that was set via SetClientSize
    Dimension xx, yy;
    XtVaGetValues ((Widget) m_drawingArea, XmNwidth, &xx, XmNheight, &yy, NULL);
    *w = xx;
    *h = yy;
#endif
}

void wxWindowX11::CanvasGetSize (int *w, int *h) const
{
// TODO
#if 0
    Dimension xx, yy;
    if ((Widget) m_borderWidget)
        XtVaGetValues ((Widget) m_borderWidget, XmNwidth, &xx, XmNheight, &yy, NULL);
    else if ((Widget) m_scrolledWindow)
        XtVaGetValues ((Widget) m_scrolledWindow, XmNwidth, &xx, XmNheight, &yy, NULL);
    else
        XtVaGetValues ((Widget) m_drawingArea, XmNwidth, &xx, XmNheight, &yy, NULL);

    *w = xx;
    *h = yy;
#endif
}

void wxWindowX11::CanvasGetPosition (int *x, int *y) const
{
// TODO
#if 0
    Position xx, yy;
    XtVaGetValues (m_borderWidget ? (Widget) m_borderWidget : (Widget) m_scrolledWindow, XmNx, &xx, XmNy, &yy, NULL);

    // We may be faking the client origin.
    // So a window that's really at (0, 30) may appear
    // (to wxWin apps) to be at (0, 0).
    if (GetParent())
    {
        wxPoint pt(GetParent()->GetClientAreaOrigin());
        xx -= pt.x;
        yy -= pt.y;
    }

    *x = xx;
    *y = yy;
#endif
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
                    win->SetButton1(TRUE);
                    button = 1;
                }
                else if (xevent->xbutton.button == Button2)
                {
                    eventType = wxEVT_MIDDLE_DOWN;
                    win->SetButton2(TRUE);
                    button = 2;
                }
                else if (xevent->xbutton.button == Button3)
                {
                    eventType = wxEVT_RIGHT_DOWN;
                    win->SetButton3(TRUE);
                    button = 3;
                }

                // check for a double click
                // TODO: where can we get this value from?
                //long dclickTime = XtGetMultiClickTime((Display*) wxGetDisplay());
                long dClickTime = 200;
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
                    win->SetButton1(FALSE);
                }
                else if (xevent->xbutton.button == Button2)
                {
                    eventType = wxEVT_MIDDLE_UP;
                    win->SetButton2(FALSE);
                }
                else if (xevent->xbutton.button == Button3)
                {
                    eventType = wxEVT_RIGHT_UP;
                    win->SetButton3(FALSE);
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

#if 0

#define YAllocColor XAllocColor
XColor g_itemColors[5];
int wxComputeColours (Display *display, wxColour * back, wxColour * fore)
{
    int result;
    static XmColorProc colorProc;

    result = wxNO_COLORS;

    if (back)
    {
        g_itemColors[0].red = (((long) back->Red ()) << 8);
        g_itemColors[0].green = (((long) back->Green ()) << 8);
        g_itemColors[0].blue = (((long) back->Blue ()) << 8);
        g_itemColors[0].flags = DoRed | DoGreen | DoBlue;
        if (colorProc == (XmColorProc) NULL)
        {
            // Get a ptr to the actual function
            colorProc = XmSetColorCalculation ((XmColorProc) NULL);
            // And set it back to motif.
            XmSetColorCalculation (colorProc);
        }
        (*colorProc) (&g_itemColors[wxBACK_INDEX],
            &g_itemColors[wxFORE_INDEX],
            &g_itemColors[wxSELE_INDEX],
            &g_itemColors[wxTOPS_INDEX],
            &g_itemColors[wxBOTS_INDEX]);
        result = wxBACK_COLORS;
    }
    if (fore)
    {
        g_itemColors[wxFORE_INDEX].red = (((long) fore->Red ()) << 8);
        g_itemColors[wxFORE_INDEX].green = (((long) fore->Green ()) << 8);
        g_itemColors[wxFORE_INDEX].blue = (((long) fore->Blue ()) << 8);
        g_itemColors[wxFORE_INDEX].flags = DoRed | DoGreen | DoBlue;
        if (result == wxNO_COLORS)
            result = wxFORE_COLORS;
    }

    Display *dpy = display;
    Colormap cmap = (Colormap) wxTheApp->GetMainColormap((WXDisplay*) dpy);

    if (back)
    {
        /* 5 Colours to allocate */
        for (int i = 0; i < 5; i++)
            if (!YAllocColor (dpy, cmap, &g_itemColors[i]))
                result = wxNO_COLORS;
    }
    else if (fore)
    {
        /* Only 1 colour to allocate */
        if (!YAllocColor (dpy, cmap, &g_itemColors[wxFORE_INDEX]))
            result = wxNO_COLORS;
    }

    return (result);

}
#endif

// Changes the foreground and background colours to be derived from the current
// background colour. To change the foreground colour, you must call
// SetForegroundColour explicitly.
void wxWindowX11::ChangeBackgroundColour()
{
    // TODO
#if 0
    WXWidget mainWidget = GetMainWidget();
    if ( mainWidget )
        DoChangeBackgroundColour(mainWidget, m_backgroundColour);
#endif
}

void wxWindowX11::ChangeForegroundColour()
{
    // TODO
#if 0
    WXWidget mainWidget = GetMainWidget();
    if ( mainWidget )
        DoChangeForegroundColour(mainWidget, m_foregroundColour);
    if ( m_scrolledWindow && mainWidget != m_scrolledWindow )
        DoChangeForegroundColour(m_scrolledWindow, m_foregroundColour);
#endif
}

// Change a widget's foreground and background colours.
void wxWindowX11::DoChangeForegroundColour(WXWindow widget, wxColour& foregroundColour)
{
    // TODO
#if 0
    // When should we specify the foreground, if it's calculated
    // by wxComputeColours?
    // Solution: say we start with the default (computed) foreground colour.
    // If we call SetForegroundColour explicitly for a control or window,
    // then the foreground is changed.
    // Therefore SetBackgroundColour computes the foreground colour, and
    // SetForegroundColour changes the foreground colour. The ordering is
    // important.

    Widget w = (Widget)widget;
    XtVaSetValues(
                  w,
                  XmNforeground, foregroundColour.AllocColour(XtDisplay(w)),
                  NULL
                 );
#endif
}

void wxWindowX11::DoChangeBackgroundColour(WXWindow widget, wxColour& backgroundColour, bool changeArmColour)
{
    // TODO
#if 0
    wxComputeColours (XtDisplay((Widget) widget), & backgroundColour,
        (wxColour*) NULL);

    XtVaSetValues ((Widget) widget,
        XmNbackground, g_itemColors[wxBACK_INDEX].pixel,
        XmNtopShadowColor, g_itemColors[wxTOPS_INDEX].pixel,
        XmNbottomShadowColor, g_itemColors[wxBOTS_INDEX].pixel,
        XmNforeground, g_itemColors[wxFORE_INDEX].pixel,
        NULL);

    if (changeArmColour)
        XtVaSetValues ((Widget) widget,
        XmNarmColor, g_itemColors[wxSELE_INDEX].pixel,
        NULL);
#endif
}

bool wxWindowX11::SetBackgroundColour(const wxColour& col)
{
    if ( !wxWindowBase::SetBackgroundColour(col) )
        return FALSE;

    ChangeBackgroundColour();

    return TRUE;
}

bool wxWindowX11::SetForegroundColour(const wxColour& col)
{
    if ( !wxWindowBase::SetForegroundColour(col) )
        return FALSE;

    ChangeForegroundColour();

    return TRUE;
}

void wxWindowX11::ChangeFont(bool keepOriginalSize)
{
    // TODO
#if 0
    // Note that this causes the widget to be resized back
    // to its original size! We therefore have to set the size
    // back again. TODO: a better way in Motif?
    Widget w = (Widget) GetLabelWidget(); // Usually the main widget
    if (w && m_font.Ok())
    {
        int width, height, width1, height1;
        GetSize(& width, & height);

        // lesstif 0.87 hangs here
#ifndef LESSTIF_VERSION
        XtVaSetValues (w,
            XmNfontList, (XmFontList) m_font.GetFontList(1.0, XtDisplay(w)),
            NULL);
#endif

        GetSize(& width1, & height1);
        if (keepOriginalSize && (width != width1 || height != height1))
        {
            SetSize(-1, -1, width, height);
        }
    }
#endif
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
    Display *display = (Display*) wxGetDisplay();
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

