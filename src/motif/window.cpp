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

#include "wx/listimpl.cpp"

#if  wxUSE_DRAG_AND_DROP
    #include "wx/dnd.h"
#endif

#include <Xm/Xm.h>

#include <Xm/DrawingA.h>
#include <Xm/ScrolledW.h>
#include <Xm/ScrollBar.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>           // for XmMenuPosition

#include "wx/motif/private.h"

#include <string.h>

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

static const int SCROLL_MARGIN = 4;

// ----------------------------------------------------------------------------
// global variables for this module
// ----------------------------------------------------------------------------

extern wxHashTable *wxWidgetHashTable;

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

static void wxCanvasRepaintProc(Widget, XtPointer, XmDrawingAreaCallbackStruct * cbs);
static void wxCanvasInputEvent(Widget drawingArea, XtPointer data, XmDrawingAreaCallbackStruct * cbs);
static void wxCanvasMotionEvent(Widget, XButtonEvent * event);
static void wxCanvasEnterLeave(Widget drawingArea, XtPointer clientData, XCrossingEvent * event);
static void wxScrollBarCallback(Widget widget, XtPointer clientData,
                                XmScrollBarCallbackStruct *cbs);
static void wxPanelItemEventHandler(Widget    wid,
                                    XtPointer client_data,
                                    XEvent*   event,
                                    Boolean  *continueToDispatch);

// unused for now
#if 0

// Helper function for 16-bit fonts
static int str16len(const char *s)
{
    int count = 0;

    while (s[0] && s[1]) {
        count++;
        s += 2;
    }

    return count;
}

#endif // 0

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

#define event_left_is_down(x) ((x)->xbutton.state & Button1Mask)
#define event_middle_is_down(x) ((x)->xbutton.state & Button2Mask)
#define event_right_is_down(x) ((x)->xbutton.state & Button3Mask)

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARY
    IMPLEMENT_DYNAMIC_CLASS(wxWindow, wxWindowBase)

    BEGIN_EVENT_TABLE(wxWindow, wxWindowBase)
        EVT_SYS_COLOUR_CHANGED(wxWindow::OnSysColourChanged)
        EVT_IDLE(wxWindow::OnIdle)
    END_EVENT_TABLE()
#endif // USE_SHARED_LIBRARY

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// list types
// ----------------------------------------------------------------------------

WX_DEFINE_LIST(wxRectList);

// ----------------------------------------------------------------------------
// helper functions
// ----------------------------------------------------------------------------

void wxWindow::UnmanageAndDestroy(WXWidget widget)
{
    Widget w = (Widget)widget;
    if ( w )
    {
        XtUnmanageChild(w);
        XtDestroyWidget(w);
    }
}

bool wxWindow::MapOrUnmap(WXWidget widget, bool map)
{
    Widget w = (Widget)widget;
    if ( !w )
        return FALSE;

    if ( map )
        XtMapWidget(w);
    else
        XtUnmapWidget(w);

    return TRUE;
}

// ----------------------------------------------------------------------------
// constructors
// ----------------------------------------------------------------------------

void wxWindow::Init()
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
bool wxWindow::Create(wxWindow *parent, wxWindowID id,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxString& name)
{
    wxCHECK_MSG( parent, FALSE, "can't create wxWindow without parent" );

    CreateBase(parent, id, pos, size, style, wxDefaultValidator, name);

    parent->AddChild(this);

    m_backgroundColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE);
    m_foregroundColour = *wxBLACK;

    //// TODO: we should probably optimize by only creating a
    //// a drawing area if we have one or more scrollbars (wxVSCROLL/wxHSCROLL).
    //// But for now, let's simplify things by always creating the
    //// drawing area, since otherwise the translations are different.

    // New translations for getting mouse motion feedback
    static const String translations =
"<Btn1Motion>: wxCanvasMotionEvent() DrawingAreaInput() ManagerGadgetButtonMotion()\n\
<Btn2Motion>: wxCanvasMotionEvent() DrawingAreaInput() ManagerGadgetButtonMotion()\n\
<Btn3Motion>: wxCanvasMotionEvent() DrawingAreaInput() ManagerGadgetButtonMotion()\n\
<BtnMotion>: wxCanvasMotionEvent() DrawingAreaInput() ManagerGadgetButtonMotion()\n\
<Btn1Down>: DrawingAreaInput() ManagerGadgetArm()\n\
<Btn2Down>: DrawingAreaInput() ManagerGadgetArm()\n\
<Btn3Down>: DrawingAreaInput() ManagerGadgetArm()\n\
<Btn1Up>: DrawingAreaInput() ManagerGadgetActivate()\n\
<Btn2Up>: DrawingAreaInput() ManagerGadgetActivate()\n\
<Btn3Up>: DrawingAreaInput() ManagerGadgetActivate()\n\
<Motion>: wxCanvasMotionEvent() DrawingAreaInput()\n\
<EnterWindow>: wxCanvasMotionEvent() DrawingAreaInput()\n\
<LeaveWindow>: wxCanvasMotionEvent() DrawingAreaInput()\n\
<Key>: DrawingAreaInput()";

    XtActionsRec actions[1];
    actions[0].string = "wxCanvasMotionEvent";
    actions[0].proc = (XtActionProc) wxCanvasMotionEvent;
    XtAppAddActions ((XtAppContext) wxTheApp->GetAppContext(), actions, 1);

    Widget parentWidget = (Widget) parent->GetClientWidget();
    if (style & wxBORDER)
    {
        m_borderWidget = (WXWidget)XtVaCreateManagedWidget
                                   (
                                    "canvasBorder",
                                    xmFrameWidgetClass, parentWidget,
                                    XmNshadowType, XmSHADOW_IN,
                                    NULL
                                   );
    }

    m_scrolledWindow = (WXWidget)XtVaCreateManagedWidget
                                 (
                                  "scrolledWindow",
                                  xmScrolledWindowWidgetClass,
                                  m_borderWidget ? (Widget) m_borderWidget
                                                 : parentWidget,
                                  XmNresizePolicy, XmRESIZE_NONE,
                                  XmNspacing, 0,
                                  XmNscrollingPolicy, XmAPPLICATION_DEFINED,
                                  //XmNscrollBarDisplayPolicy, XmAS_NEEDED,
                                  NULL
                                 );

    XtTranslations ptr = XtParseTranslationTable(translations);
    m_drawingArea = (WXWidget)XtVaCreateWidget
                              (
                               name,
                               xmDrawingAreaWidgetClass, (Widget) m_scrolledWindow,
                               XmNunitType, XmPIXELS,
                               // XmNresizePolicy, XmRESIZE_ANY,
                               XmNresizePolicy, XmRESIZE_NONE,
                               XmNmarginHeight, 0,
                               XmNmarginWidth, 0,
                               XmNtranslations, ptr,
                               NULL
                              );
    XtFree((char *) ptr);

#if 0
    if (GetWindowStyleFlag() & wxOVERRIDE_KEY_TRANSLATIONS)
    {
        ptr = XtParseTranslationTable ("<Key>: DrawingAreaInput()");
        XtOverrideTranslations ((Widget) m_drawingArea, ptr);
        XtFree ((char *) ptr);
    }
#endif // 0

    wxAddWindowToTable((Widget) m_drawingArea, this);
    wxAddWindowToTable((Widget) m_scrolledWindow, this);

    // This order is very important in Motif 1.2.1
    XtRealizeWidget ((Widget) m_scrolledWindow);
    XtRealizeWidget ((Widget) m_drawingArea);
    XtManageChild ((Widget) m_drawingArea);

    ptr = XtParseTranslationTable("<Configure>: resize()");
    XtOverrideTranslations((Widget) m_drawingArea, ptr);
    XtFree ((char *) ptr);

    XtAddCallback ((Widget) m_drawingArea, XmNexposeCallback, (XtCallbackProc) wxCanvasRepaintProc, (XtPointer) this);
    XtAddCallback ((Widget) m_drawingArea, XmNinputCallback, (XtCallbackProc) wxCanvasInputEvent, (XtPointer) this);

    // TODO?
#if 0
    display = XtDisplay (scrolledWindow);
    xwindow = XtWindow (drawingArea);
#endif // 0

    XtAddEventHandler(
                      (Widget)m_drawingArea,
                       PointerMotionHintMask | EnterWindowMask |
                       LeaveWindowMask | FocusChangeMask,
                       False,
                       (XtEventHandler) wxCanvasEnterLeave,
                       (XtPointer) this
                     );

    // Scrolled widget needs to have its colour changed or we get a little blue
    // square where the scrollbars abutt
    wxColour backgroundColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE);
    DoChangeBackgroundColour(m_scrolledWindow, backgroundColour, TRUE);
    DoChangeBackgroundColour(m_drawingArea, backgroundColour, TRUE);

    XmScrolledWindowSetAreas(
                             (Widget)m_scrolledWindow,
                             (Widget) 0, (Widget) 0,
                             (Widget) m_drawingArea);

#if 0
    if (m_hScrollBar)
        XtRealizeWidget ((Widget) m_hScrollBar);
    if (m_vScrollBar)
        XtRealizeWidget ((Widget) m_vScrollBar);
#endif // 0

    // Without this, the cursor may not be restored properly (e.g. in splitter
    // sample).
    SetCursor(*wxSTANDARD_CURSOR);
    SetFont(wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT));
    SetSize(pos.x, pos.y, size.x, size.y);

    return TRUE;
}

// Destructor
wxWindow::~wxWindow()
{
    m_isBeingDeleted = TRUE;
    
    // Motif-specific actions first
    WXWidget wMain = GetMainWidget();
    if ( wMain )
    {
        // Removes event handlers
        DetachWidget(wMain);
    }

    ClearUpdateRects();

    if ( m_parent )
        m_parent->RemoveChild( this );

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
        }

        m_mainWidget = (WXWidget) 0;

        // Only if we're _really_ a canvas (not a dialog box/panel)
        if (m_scrolledWindow)
        {
            wxDeleteWindowFromTable((Widget) m_scrolledWindow);
        }

        if (m_hScrollBar)
        {
            wxDeleteWindowFromTable((Widget) m_hScrollBar);
        }
        if (m_vScrollBar)
        {
            wxDeleteWindowFromTable((Widget) m_vScrollBar);
        }

        UnmanageAndDestroy(m_hScrollBar);
        UnmanageAndDestroy(m_vScrollBar);
        UnmanageAndDestroy(m_scrolledWindow);

        if (m_borderWidget)
        {
            XtDestroyWidget ((Widget) m_borderWidget);
            m_borderWidget = (WXWidget) 0;
        }
    }

    // Destroy the window
    if (GetMainWidget())
    {
        // If this line (XtDestroyWidget) causes a crash, you may comment it out.
        // Child widgets will get destroyed automatically when a frame
        // or dialog is destroyed, but before that you may get some memory
        // leaks and potential layout problems if you delete and then add
        // child windows.
        XtDestroyWidget((Widget) GetMainWidget());
        SetMainWidget((WXWidget) NULL);
    }
}

// ----------------------------------------------------------------------------
// scrollbar management
// ----------------------------------------------------------------------------

// Helper function
void wxWindow::CreateScrollbar(wxOrientation orientation)
{
    wxCHECK_RET( m_drawingArea, "this window can't have scrollbars" );

    XtVaSetValues((Widget) m_scrolledWindow, XmNresizePolicy, XmRESIZE_NONE, NULL);

    // Add scrollbars if required
    if (orientation == wxHORIZONTAL)
    {
        Widget hScrollBar = XtVaCreateManagedWidget ("hsb",
            xmScrollBarWidgetClass, (Widget) m_scrolledWindow,
            XmNorientation, XmHORIZONTAL,
            NULL);
        //      XtAddCallback (hScrollBar, XmNvalueChangedCallback, (XtCallbackProc) wxScrollBarCallback, (XtPointer) XmHORIZONTAL);
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

        wxColour backgroundColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE);
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
        //      XtAddCallback (vScrollBar, XmNvalueChangedCallback, (XtCallbackProc) wxScrollBarCallback, (XtPointer) XmVERTICAL);
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
        wxColour backgroundColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE);
        DoChangeBackgroundColour(m_vScrollBar, backgroundColour, TRUE);

        XtRealizeWidget(vScrollBar);

        XtVaSetValues((Widget) m_scrolledWindow,
            XmNverticalScrollBar, (Widget) m_vScrollBar,
            NULL);

        m_vScroll = TRUE;

        wxAddWindowToTable( vScrollBar, this );
    }

    XtVaSetValues((Widget) m_scrolledWindow, XmNresizePolicy, XmRESIZE_ANY, NULL);
}

void wxWindow::DestroyScrollbar(wxOrientation orientation)
{
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
        m_vScroll = TRUE;

        XtVaSetValues((Widget) m_scrolledWindow,
            XmNverticalScrollBar, (Widget) 0,
            NULL);

    }
    XtVaSetValues((Widget) m_scrolledWindow, XmNresizePolicy, XmRESIZE_ANY, NULL);
}

// ---------------------------------------------------------------------------
// basic operations
// ---------------------------------------------------------------------------

void wxWindow::SetFocus()
{
    Widget wMain = (Widget) GetMainWidget();
    XmProcessTraversal(wMain, XmTRAVERSE_CURRENT);
    XmProcessTraversal((Widget) GetMainWidget(), XmTRAVERSE_CURRENT);
}

// Get the window with the focus
wxWindow *wxWindowBase::FindFocus()
{
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
}

bool wxWindow::Enable(bool enable)
{
    if ( !wxWindowBase::Enable(enable) )
        return FALSE;

    Widget wMain = (Widget)GetMainWidget();
    if ( wMain )
    {
        XtSetSensitive(wMain, enable);
        XmUpdateDisplay(wMain);
    }

    return TRUE;
}

bool wxWindow::Show(bool show)
{
    if ( !wxWindowBase::Show(show) )
        return FALSE;

    if (m_borderWidget || m_scrolledWindow)
    {
        MapOrUnmap(m_drawingArea, show);
        MapOrUnmap(m_borderWidget ? m_borderWidget : m_scrolledWindow, show);
    }
    else
    {
        if ( !MapOrUnmap(GetTopWidget(), show) )
            MapOrUnmap(GetMainWidget(), show);
    }

#if 0
    Window xwin = (Window) GetXWindow();
    Display *xdisp = (Display*) GetXDisplay();
    if (show)
        XMapWindow(xdisp, xwin);
    else
        XUnmapWindow(xdisp, xwin);
#endif

    return TRUE;
}

// Raise the window to the top of the Z order
void wxWindow::Raise()
{
    Widget wTop = (Widget) GetTopWidget();
    Window window = XtWindow(wTop);
    XRaiseWindow(XtDisplay(wTop), window);
}

// Lower the window to the bottom of the Z order
void wxWindow::Lower()
{
    Widget wTop = (Widget) GetTopWidget();
    Window window = XtWindow(wTop);
    XLowerWindow(XtDisplay(wTop), window);
}

void wxWindow::SetTitle(const wxString& title)
{
    XtVaSetValues((Widget)GetMainWidget(), XmNtitle, title.c_str(), NULL);
}

wxString wxWindow::GetTitle() const
{
    char *title;
    XtVaGetValues((Widget)GetMainWidget(), XmNtitle, &title, NULL);

    return wxString(title);
}

void wxWindow::CaptureMouse()
{
    if ( m_winCaptured )
        return;

    Widget wMain = (Widget)GetMainWidget();
    if ( wMain )
        XtAddGrab(wMain, TRUE, FALSE);

    m_winCaptured = TRUE;
}

void wxWindow::ReleaseMouse()
{
    if ( !m_winCaptured )
        return;

    Widget wMain = (Widget)GetMainWidget();
    if ( wMain )
        XtRemoveGrab(wMain);

    m_winCaptured = FALSE;
}

bool wxWindow::SetFont(const wxFont& font)
{
    if ( !wxWindowBase::SetFont(font) )
    {
        // nothing to do
        return FALSE;
    }

    ChangeFont();

    return TRUE;
}

bool wxWindow::SetCursor(const wxCursor& cursor)
{
    if ( !wxWindowBase::SetCursor(cursor) )
    {
        // no change
        return FALSE;
    }

    wxASSERT_MSG( m_cursor.Ok(),
                  T("cursor must be valid after call to the base version"));

    WXDisplay *dpy = GetXDisplay();
    WXCursor x_cursor = m_cursor.GetXCursor(dpy);

    Widget w = (Widget) GetMainWidget();
    Window win = XtWindow(w);
    XDefineCursor((Display*) dpy, win, (Cursor) x_cursor);

    return TRUE;
}

// Coordinates relative to the window
void wxWindow::WarpPointer (int x, int y)
{
    Widget wClient = (Widget)GetClientWidget();

    XWarpPointer(XtDisplay(wClient), None, XtWindow(wClient), 0, 0, 0, 0, x, y);
}

// ---------------------------------------------------------------------------
// scrolling stuff
// ---------------------------------------------------------------------------

int wxWindow::GetScrollPos(int orient) const
{
    if (orient == wxHORIZONTAL)
        return m_scrollPosX;
    else
        return m_scrollPosY;

#if 0
    Widget scrollBar = (Widget) ((orient == wxHORIZONTAL) ? m_hScrollBar : m_vScrollBar);
    if (scrollBar)
    {
        int pos;
        XtVaGetValues(scrollBar, XmNvalue, &pos, NULL);
        return pos;
    }
    else
        return 0;
#endif // 0
}

// This now returns the whole range, not just the number of positions that we
// can scroll.
int wxWindow::GetScrollRange(int orient) const
{
    Widget scrollBar = (Widget)GetScrollbar((wxOrientation)orient);
    wxCHECK_MSG( scrollBar, 0, "no such scrollbar" );

    int range;
    XtVaGetValues(scrollBar, XmNmaximum, &range, NULL);
    return range;
}

int wxWindow::GetScrollThumb(int orient) const
{
    Widget scrollBar = (Widget)GetScrollbar((wxOrientation)orient);
    wxCHECK_MSG( scrollBar, 0, "no such scrollbar" );

    int thumb;
    XtVaGetValues(scrollBar, XmNsliderSize, &thumb, NULL);
    return thumb;
}

void wxWindow::SetScrollPos(int orient, int pos, bool WXUNUSED(refresh))
{
    Widget scrollBar = (Widget)GetScrollbar((wxOrientation)orient);

    if ( scrollBar )
    {
        XtVaSetValues (scrollBar, XmNvalue, pos, NULL);
    }

    SetInternalScrollPos((wxOrientation)orient, pos);
}

// New function that will replace some of the above.
void wxWindow::SetScrollbar(int orient, int pos, int thumbVisible,
                            int range, bool WXUNUSED(refresh))
{
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
}

// Does a physical scroll
void wxWindow::ScrollWindow(int dx, int dy, const wxRect *rect)
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

    int x1 = (dx >= 0) ? x : x - dx;
    int y1 = (dy >= 0) ? y : y - dy;
    int w1 = w - abs(dx);
    int h1 = h - abs(dy);
    int x2 = (dx >= 0) ? x + dx : x;
    int y2 = (dy >= 0) ? y + dy : y;

    wxClientDC dc(this);

    dc.SetLogicalFunction (wxCOPY);

    Widget widget = (Widget) GetMainWidget();
    Window window = XtWindow(widget);
    Display* display = XtDisplay(widget);

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

    XmUpdateDisplay((Widget) GetMainWidget());
}

// ---------------------------------------------------------------------------
// drag and drop
// ---------------------------------------------------------------------------

#if wxUSE_DRAG_AND_DROP

void wxWindow::SetDropTarget(wxDropTarget * WXUNUSED(pDropTarget))
{
    // TODO
}

#endif

// Old style file-manager drag&drop
void wxWindow::DragAcceptFiles(bool WXUNUSED(accept))
{
    // TODO
}

// ----------------------------------------------------------------------------
// tooltips
// ----------------------------------------------------------------------------

#if wxUSE_TOOLTIPS

void wxWindow::DoSetToolTip(wxToolTip * WXUNUSED(tooltip))
{
    // TODO
}

#endif // wxUSE_TOOLTIPS

// ----------------------------------------------------------------------------
// popup menus
// ----------------------------------------------------------------------------

bool wxWindow::DoPopupMenu(wxMenu *menu, int x, int y)
{
    Widget widget = (Widget) GetMainWidget();

    /* The menuId field seems to be usused, so we'll use it to
    indicate whether a menu is popped up or not:
    0: Not currently created as a popup
    -1: Created as a popup, but not active
    1: Active popup.
    */

    if (menu->GetParent() && (menu->GetId() != -1))
        return FALSE;

    if (menu->GetMainWidget()) {
        menu->DestroyMenu(TRUE);
    }

    menu->SetId(1); /* Mark as popped-up */
    menu->CreateMenu(NULL, widget, menu);
    menu->SetInvokingWindow(this);

    menu->UpdateUI();

    //  menu->SetParent(parent);
    //  parent->children->Append(menu);  // Store menu for later deletion

    Widget menuWidget = (Widget) menu->GetMainWidget();

    int rootX = 0;
    int rootY = 0;

    int deviceX = x;
    int deviceY = y;
    /*
    if (this->IsKindOf(CLASSINFO(wxCanvas)))
    {
    wxCanvas *canvas = (wxCanvas *) this;
    deviceX = canvas->GetDC ()->LogicalToDeviceX (x);
    deviceY = canvas->GetDC ()->LogicalToDeviceY (y);
    }
    */

    Display *display = XtDisplay (widget);
    Window rootWindow = RootWindowOfScreen (XtScreen((Widget)widget));
    Window thisWindow = XtWindow (widget);
    Window childWindow;
    XTranslateCoordinates (display, thisWindow, rootWindow, (int) deviceX, (int) deviceY,
        &rootX, &rootY, &childWindow);

    XButtonPressedEvent event;
    event.type = ButtonPress;
    event.button = 1;

    event.x = deviceX;
    event.y = deviceY;

    event.x_root = rootX;
    event.y_root = rootY;

    XmMenuPosition (menuWidget, &event);
    XtManageChild (menuWidget);

    return TRUE;
}

// ---------------------------------------------------------------------------
// moving and resizing
// ---------------------------------------------------------------------------

bool wxWindow::PreResize()
{
    return TRUE;
}

// Get total size
void wxWindow::DoGetSize(int *x, int *y) const
{
    if (m_drawingArea)
    {
        CanvasGetSize(x, y);
        return;
    }

    Widget widget = (Widget) GetTopWidget();
    Dimension xx, yy;
    XtVaGetValues(widget, XmNwidth, &xx, XmNheight, &yy, NULL);
    *x = xx; *y = yy;
}

void wxWindow::DoGetPosition(int *x, int *y) const
{
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

    *x = xx; *y = yy;
}

void wxWindow::DoScreenToClient(int *x, int *y) const
{
    Widget widget = (Widget) GetClientWidget();
    Display *display = XtDisplay((Widget) GetMainWidget());
    Window rootWindow = RootWindowOfScreen(XtScreen(widget));
    Window thisWindow = XtWindow(widget);

    Window childWindow;
    int xx = *x;
    int yy = *y;
    XTranslateCoordinates(display, rootWindow, thisWindow, xx, yy, x, y, &childWindow);
}

void wxWindow::DoClientToScreen(int *x, int *y) const
{
    Widget widget = (Widget) GetClientWidget();
    Display *display = XtDisplay(widget);
    Window rootWindow = RootWindowOfScreen(XtScreen(widget));
    Window thisWindow = XtWindow(widget);

    Window childWindow;
    int xx = *x;
    int yy = *y;
    XTranslateCoordinates(display, thisWindow, rootWindow, xx, yy, x, y, &childWindow);
}


// Get size *available for subwindows* i.e. excluding menu bar etc.
void wxWindow::DoGetClientSize(int *x, int *y) const
{
    Widget widget = (Widget) GetClientWidget();
    Dimension xx, yy;
    XtVaGetValues(widget, XmNwidth, &xx, XmNheight, &yy, NULL);
    *x = xx; *y = yy;
}

void wxWindow::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
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

    if ((width == -1) && (height == -1))
        useOldSize = TRUE;
    else if (width == oldW && height == oldH)
        useOldSize = TRUE;

    if (!wxNoOptimize::CanOptimize())
    {
        useOldSize = FALSE; useOldPos = FALSE;
    }

    if (useOldPos && useOldSize)
        return;

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

    int xx = x; int yy = y;
    AdjustForParentClientOrigin(xx, yy, sizeFlags);

    if (!useOldPos)
    {
        if (x > -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
            XtVaSetValues(widget, XmNx, xx, NULL);
        if (y > -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
            XtVaSetValues(widget, XmNy, yy, NULL);
    }
    if (!useOldSize)
    {
        if (width > -1)
            XtVaSetValues(widget, XmNwidth, width, NULL);
        if (height > -1)
            XtVaSetValues(widget, XmNheight, height, NULL);
    }

    if (managed)
        XtManageChild(widget);

    // How about this bit. Maybe we don't need to generate size events
    // all the time -- they'll be generated when the window is sized anyway.
#if 0
    wxSizeEvent sizeEvent(wxSize(width, height), GetId());
    sizeEvent.SetEventObject(this);

      GetEventHandler()->ProcessEvent(sizeEvent);
#endif // 0
}

void wxWindow::DoSetClientSize(int width, int height)
{
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
}

// For implementation purposes - sometimes decorations make the client area
// smaller
wxPoint wxWindow::GetClientAreaOrigin() const
{
    return wxPoint(0, 0);
}

// Makes an adjustment to the window position (for example, a frame that has
// a toolbar that it manages itself).
void wxWindow::AdjustForParentClientOrigin(int& x, int& y, int sizeFlags)
{
    if (((sizeFlags & wxSIZE_NO_ADJUSTMENTS) == 0) && GetParent())
    {
        wxPoint pt(GetParent()->GetClientAreaOrigin());
        x += pt.x; y += pt.y;
    }
}

void wxWindow::SetSizeHints(int minW, int minH, int maxW, int maxH, int incW, int incH)
{
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
}

// ---------------------------------------------------------------------------
// text metrics
// ---------------------------------------------------------------------------

int wxWindow::GetCharHeight() const
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

int wxWindow::GetCharWidth() const
{
    wxCHECK_MSG( m_font.Ok(), 0, "valid window font needed" );

    WXFontStructPtr pFontStruct = m_font.GetFontStruct(1.0, GetXDisplay());

    int direction, ascent, descent;
    XCharStruct overall;
    XTextExtents ((XFontStruct*) pFontStruct, "x", 1, &direction, &ascent,
        &descent, &overall);

    return overall.width;
}

void wxWindow::GetTextExtent(const wxString& string,
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

void wxWindow::Refresh(bool eraseBack, const wxRect *rect)
{
    m_needsRefresh = TRUE;
    Display *display = XtDisplay((Widget) GetMainWidget());
    Window thisWindow = XtWindow((Widget) GetMainWidget());

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

void wxWindow::Clear()
{
    wxClientDC dc(this);
    wxBrush brush(GetBackgroundColour(), wxSOLID);
    dc.SetBackground(brush);
    dc.Clear();
}

void wxWindow::ClearUpdateRects()
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

void wxWindow::DoPaint()
{
    //TODO : make a temporary gc so we can do the XCopyArea below
    if (m_backingPixmap && !m_needsRefresh)
    {
      wxPaintDC dc(this);

      GC tempGC = (GC) dc.GetBackingGC();

      Widget widget = (Widget) GetMainWidget();

      int scrollPosX = 0;
      int scrollPosY = 0;

      // We have to test whether it's a wxScrolledWindow (hack!) because
      // otherwise we don't know how many pixels have been scrolled. We might
      // solve this in the future by defining virtual wxWindow functions to get
      // the scroll position in pixels. Or, each kind of scrolled window has to
      // implement backing stores itself, using generic wxWindows code.
      wxScrolledWindow* scrolledWindow = wxDynamicCast(this, wxScrolledWindow);
      if ( scrolledWindow )
      {
          int x, y;
          scrolledWindow->CalcScrolledPosition(0, 0, &x, &y);

          scrollPosX = - x;
          scrollPosY = - y;
      }

      // TODO: This could be optimized further by only copying the areas in the
      //       current update region.

      // Only blit the part visible in the client area. The backing pixmap
      // always starts at 0, 0 but we may be looking at only a portion of it.
      wxSize clientArea = GetClientSize();
      int toBlitX = m_pixmapWidth - scrollPosX;
      int toBlitY = m_pixmapHeight - scrollPosY;

      // Copy whichever is samller, the amount of pixmap we have to copy,
      // or the size of the client area.
      toBlitX = wxMin(toBlitX, clientArea.x);
      toBlitY = wxMin(toBlitY, clientArea.y);

      // Make sure we're not negative
      toBlitX = wxMax(0, toBlitX);
      toBlitY = wxMax(0, toBlitY);

      XCopyArea
      (
       XtDisplay(widget),
       (Pixmap) m_backingPixmap,
       XtWindow (widget),
       tempGC,
       scrollPosX, scrollPosY, // Start at the scroll position
       toBlitX, toBlitY,       // How much of the pixmap to copy
       0, 0                    // Destination
      );
    }
    else
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
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

// Responds to colour changes: passes event on to children.
void wxWindow::OnSysColourChanged(wxSysColourChangedEvent& event)
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

void wxWindow::OnIdle(wxIdleEvent& event)
{
    // This calls the UI-update mechanism (querying windows for
    // menu/toolbar/control state information)
    UpdateWindowUI();
}

// ----------------------------------------------------------------------------
// accelerators
// ----------------------------------------------------------------------------

bool wxWindow::ProcessAccelerator(wxKeyEvent& event)
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
            // Bingo, we have a match. Now find a control that matches the entry
            // command id.

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
                    wxMenuItem* item = frame->GetMenuBar()->FindItemForId(entry->GetCommand());
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
// Motif-specific stuff from here on
// ============================================================================

// ----------------------------------------------------------------------------
// function which maintain the global hash table mapping Widgets to wxWindows
// ----------------------------------------------------------------------------

bool wxAddWindowToTable(Widget w, wxWindow *win)
{
    wxWindow *oldItem = NULL;
    if ((oldItem = (wxWindow *)wxWidgetHashTable->Get ((long) w)))
    {
        wxLogDebug("Widget table clash: new widget is %ld, %s",
                   (long)w, win->GetClassInfo()->GetClassName());
        return FALSE;
    }

    wxWidgetHashTable->Put((long) w, win);

    wxLogDebug("Widget 0x%08x <-> window %p (%s)",
               w, win, win->GetClassInfo()->GetClassName());

    return TRUE;
}

wxWindow *wxGetWindowFromTable(Widget w)
{
    return (wxWindow *)wxWidgetHashTable->Get((long) w);
}

void wxDeleteWindowFromTable(Widget w)
{
    wxWidgetHashTable->Delete((long)w);
}

// ----------------------------------------------------------------------------
// add/remove window from the table
// ----------------------------------------------------------------------------

// Add to hash table, add event handler
bool wxWindow::AttachWidget (wxWindow* parent, WXWidget mainWidget,
                             WXWidget formWidget, int x, int y, int width, int height)
{
    wxAddWindowToTable((Widget) mainWidget, this);
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

    if (x == -1)
        x = 0;
    if (y == -1)
        y = 0;
    SetSize (x, y, width, height);

    return TRUE;
}

// Remove event handler, remove from hash table
bool wxWindow::DetachWidget(WXWidget widget)
{
    if (CanAddEventHandler())
    {
        XtRemoveEventHandler((Widget) widget,
            ButtonPressMask | ButtonReleaseMask | PointerMotionMask, // | KeyPressMask,
            False,
            wxPanelItemEventHandler,
            (XtPointer)this);
    }

    wxDeleteWindowFromTable((Widget) widget);
    return TRUE;
}

// ----------------------------------------------------------------------------
// Motif-specific accessors
// ----------------------------------------------------------------------------

// Get the underlying X window
WXWindow wxWindow::GetXWindow() const
{
    Widget wMain = (Widget)GetMainWidget();
    if ( wMain )
        return (WXWindow) XtWindow(wMain);
    else
        return (WXWindow) 0;
}

// Get the underlying X display
WXDisplay *wxWindow::GetXDisplay() const
{
    Widget wMain = (Widget)GetMainWidget();
    if ( wMain )
        return (WXDisplay*) XtDisplay(wMain);
    else
        return (WXDisplay*) NULL;
}

WXWidget wxWindow::GetMainWidget() const
{
    if (m_drawingArea)
        return m_drawingArea;
    else
        return m_mainWidget;
}

WXWidget wxWindow::GetClientWidget() const
{
    if (m_drawingArea != (WXWidget) 0)
        return m_drawingArea;
    else
        return GetMainWidget();
}

WXWidget wxWindow::GetTopWidget() const
{
    return GetMainWidget();
}

WXWidget wxWindow::GetLabelWidget() const
{
    return GetMainWidget();
}

// ----------------------------------------------------------------------------
// Motif callbacks
// ----------------------------------------------------------------------------

// All widgets should have this as their resize proc.
// OnSize sent to wxWindow via client data.
void wxWidgetResizeProc(Widget w, XConfigureEvent *event, String args[], int *num_args)
{
    wxWindow *win = wxGetWindowFromTable(w);
    if (!win)
        return;

    if (win->PreResize())
    {
        int width, height;
        win->GetSize(&width, &height);
        wxSizeEvent sizeEvent(wxSize(width, height), win->GetId());
        sizeEvent.SetEventObject(win);
        win->GetEventHandler()->ProcessEvent(sizeEvent);
    }
}

static void wxCanvasRepaintProc(Widget drawingArea,
                                XtPointer clientData,
                                XmDrawingAreaCallbackStruct * cbs)
{
    if (!wxGetWindowFromTable(drawingArea))
        return;

    XEvent * event = cbs->event;
    wxWindow * win = (wxWindow *) clientData;

    switch (event->type)
    {
    case Expose:
        {
            win->AddUpdateRect(event->xexpose.x, event->xexpose.y,
                               event->xexpose.width, event->xexpose.height);
            
            if (event -> xexpose.count == 0)
            {
                win->DoPaint();
                win->ClearUpdateRects();
            }
            break;
        }
    }
}

// Unable to deal with Enter/Leave without a separate EventHandler (Motif 1.1.4)
static void wxCanvasEnterLeave(Widget drawingArea,
                               XtPointer clientData,
                               XCrossingEvent * event)
{
    XmDrawingAreaCallbackStruct cbs;
    XEvent ev;

    ((XCrossingEvent &) ev) = *event;

    cbs.reason = XmCR_INPUT;
    cbs.event = &ev;

    wxCanvasInputEvent(drawingArea, (XtPointer) NULL, &cbs);
}

// Fix to make it work under Motif 1.0 (!)
static void wxCanvasMotionEvent (Widget drawingArea, XButtonEvent * event)
{
#if XmVersion <= 1000
    XmDrawingAreaCallbackStruct cbs;
    XEvent ev;

    ev = *((XEvent *) event);
    cbs.reason = XmCR_INPUT;
    cbs.event = &ev;

    wxCanvasInputEvent (drawingArea, (XtPointer) NULL, &cbs);
#endif // XmVersion <= 1000
}

static void wxCanvasInputEvent(Widget drawingArea,
                               XtPointer data,
                               XmDrawingAreaCallbackStruct * cbs)
{
    wxWindow *canvas = wxGetWindowFromTable(drawingArea);
    XEvent local_event;

    if (canvas==NULL)
        return;

    if (cbs->reason != XmCR_INPUT)
        return;

    local_event = *(cbs->event);    // We must keep a copy!

    switch (local_event.xany.type)
    {
    case EnterNotify:
    case LeaveNotify:
    case ButtonPress:
    case ButtonRelease:
    case MotionNotify:
        {
            // FIXME: most of this mouse event code is more or less
            // duplicated in wxTranslateMouseEvent
            //
            wxEventType eventType = wxEVT_NULL;

            if (local_event.xany.type == EnterNotify)
            {
                //if (local_event.xcrossing.mode!=NotifyNormal)
                //  return ; // Ignore grab events
                eventType = wxEVT_ENTER_WINDOW;
                //            canvas->GetEventHandler()->OnSetFocus();
            }
            else if (local_event.xany.type == LeaveNotify)
            {
                //if (local_event.xcrossingr.mode!=NotifyNormal)
                //  return ; // Ignore grab events
                eventType = wxEVT_LEAVE_WINDOW;
                //            canvas->GetEventHandler()->OnKillFocus();
            }
            else if (local_event.xany.type == MotionNotify)
            {
                eventType = wxEVT_MOTION;
                if (local_event.xmotion.is_hint == NotifyHint)
                {
                    Window root, child;
                    Display *dpy = XtDisplay (drawingArea);

                    XQueryPointer (dpy, XtWindow (drawingArea),
                        &root, &child,
                        &local_event.xmotion.x_root,
                        &local_event.xmotion.y_root,
                        &local_event.xmotion.x,
                        &local_event.xmotion.y,
                        &local_event.xmotion.state);
                }
                else
                {
                }
            }

            else if (local_event.xany.type == ButtonPress)
            {
                if (local_event.xbutton.button == Button1)
                {
                    eventType = wxEVT_LEFT_DOWN;
                    canvas->SetButton1(TRUE);
                }
                else if (local_event.xbutton.button == Button2)
                {
                    eventType = wxEVT_MIDDLE_DOWN;
                    canvas->SetButton2(TRUE);
                }
                else if (local_event.xbutton.button == Button3)
                {
                    eventType = wxEVT_RIGHT_DOWN;
                    canvas->SetButton3(TRUE);
                }
            }
            else if (local_event.xany.type == ButtonRelease)
            {
                if (local_event.xbutton.button == Button1)
                {
                    eventType = wxEVT_LEFT_UP;
                    canvas->SetButton1(FALSE);
                }
                else if (local_event.xbutton.button == Button2)
                {
                    eventType = wxEVT_MIDDLE_UP;
                    canvas->SetButton2(FALSE);
                }
                else if (local_event.xbutton.button == Button3)
                {
                    eventType = wxEVT_RIGHT_UP;
                    canvas->SetButton3(FALSE);
                }
            }

            wxMouseEvent wxevent (eventType);

            wxevent.m_leftDown = ((eventType == wxEVT_LEFT_DOWN)
                || (event_left_is_down (&local_event)
                && (eventType != wxEVT_LEFT_UP)));
            wxevent.m_middleDown = ((eventType == wxEVT_MIDDLE_DOWN)
                || (event_middle_is_down (&local_event)
                && (eventType != wxEVT_MIDDLE_UP)));
            wxevent.m_rightDown = ((eventType == wxEVT_RIGHT_DOWN)
                || (event_right_is_down (&local_event)
                && (eventType != wxEVT_RIGHT_UP)));

            wxevent.m_shiftDown = local_event.xbutton.state & ShiftMask;
            wxevent.m_controlDown = local_event.xbutton.state & ControlMask;
            wxevent.m_altDown = local_event.xbutton.state & Mod3Mask;
            wxevent.m_metaDown = local_event.xbutton.state & Mod1Mask;
            wxevent.SetTimestamp(local_event.xbutton.time);

            // Now check if we need to translate this event into a double click
            if (TRUE) // canvas->doubleClickAllowed)
            {
                if (wxevent.ButtonDown())
                {
                    long dclickTime = XtGetMultiClickTime((Display*) wxGetDisplay());

                    // get button and time-stamp
                    int button = 0;
                    if (wxevent.LeftDown())
                        button = 1;
                    else if (wxevent.MiddleDown())
                        button = 2;
                    else if (wxevent.RightDown())
                        button = 3;
                    long ts = wxevent.GetTimestamp();

                    // check, if single or double click
                    int buttonLast = canvas->GetLastClickedButton();
                    long lastTS = canvas->GetLastClickTime();
                    if ( buttonLast && buttonLast == button && (ts - lastTS) < dclickTime )
                    {
                        // I have a dclick
                        canvas->SetLastClick(0, ts);
                        switch ( eventType )
                        {
                        case wxEVT_LEFT_DOWN:
                            wxevent.SetEventType(wxEVT_LEFT_DCLICK);
                            break;
                        case wxEVT_MIDDLE_DOWN:
                            wxevent.SetEventType(wxEVT_MIDDLE_DCLICK);
                            break;
                        case wxEVT_RIGHT_DOWN:
                            wxevent.SetEventType(wxEVT_RIGHT_DCLICK);
                            break;

                        default :
                            break;
                        }

                    }
                    else
                    {
                        // not fast enough or different button
                        canvas->SetLastClick(button, ts);
                    }
                }
            }

            wxevent.SetId(canvas->GetId());
            wxevent.SetEventObject(canvas);
            wxevent.m_x = local_event.xbutton.x;
            wxevent.m_y = local_event.xbutton.y;
            canvas->GetEventHandler()->ProcessEvent (wxevent);
#if 0
            if (eventType == wxEVT_ENTER_WINDOW ||
                    eventType == wxEVT_LEAVE_WINDOW ||
                    eventType == wxEVT_MOTION
               )
                return;
#endif // 0
            break;
      }
    case KeyPress:
        {
            KeySym keySym;
#if 0
            XComposeStatus compose;
            (void) XLookupString ((XKeyEvent *) & local_event, wxBuffer, 20, &keySym, &compose);
#endif // 0

            (void) XLookupString ((XKeyEvent *) & local_event, wxBuffer, 20, &keySym, NULL);
            int id = wxCharCodeXToWX (keySym);

            wxEventType eventType = wxEVT_CHAR;

            wxKeyEvent event (eventType);

            if (local_event.xkey.state & ShiftMask)
                event.m_shiftDown = TRUE;
            if (local_event.xkey.state & ControlMask)
                event.m_controlDown = TRUE;
            if (local_event.xkey.state & Mod3Mask)
                event.m_altDown = TRUE;
            if (local_event.xkey.state & Mod1Mask)
                event.m_metaDown = TRUE;
            event.SetEventObject(canvas);
            event.m_keyCode = id;
            event.SetTimestamp(local_event.xkey.time);

            if (id > -1)
            {
                // Implement wxFrame::OnCharHook by checking ancestor.
                wxWindow *parent = canvas->GetParent();
                while (parent && !parent->IsKindOf(CLASSINFO(wxFrame)))
                    parent = parent->GetParent();

                if (parent)
                {
                    event.SetEventType(wxEVT_CHAR_HOOK);
                    if (parent->GetEventHandler()->ProcessEvent(event))
                        return;
                }

                // For simplicity, OnKeyDown is the same as OnChar
                // TODO: filter modifier key presses from OnChar
                event.SetEventType(wxEVT_KEY_DOWN);

                // Only process OnChar if OnKeyDown didn't swallow it
                if (!canvas->GetEventHandler()->ProcessEvent (event))
                {
                  event.SetEventType(wxEVT_CHAR);
                  canvas->GetEventHandler()->ProcessEvent (event);
        }
            }
            break;
        }
    case KeyRelease:
        {
            KeySym keySym;
            (void) XLookupString ((XKeyEvent *) & local_event, wxBuffer, 20, &keySym, NULL);
            int id = wxCharCodeXToWX (keySym);

            wxKeyEvent event (wxEVT_KEY_UP);

            if (local_event.xkey.state & ShiftMask)
                event.m_shiftDown = TRUE;
            if (local_event.xkey.state & ControlMask)
                event.m_controlDown = TRUE;
            if (local_event.xkey.state & Mod3Mask)
                event.m_altDown = TRUE;
            if (local_event.xkey.state & Mod1Mask)
                event.m_metaDown = TRUE;
            event.SetEventObject(canvas);
            event.m_keyCode = id;
            event.SetTimestamp(local_event.xkey.time);

            if (id > -1)
            {
                canvas->GetEventHandler()->ProcessEvent (event);
            }
            break;
        }
    case FocusIn:
        {
            if (local_event.xfocus.detail != NotifyPointer)
            {
                wxFocusEvent event(wxEVT_SET_FOCUS, canvas->GetId());
                event.SetEventObject(canvas);
                canvas->GetEventHandler()->ProcessEvent(event);
            }
            break;
        }
    case FocusOut:
        {
            if (local_event.xfocus.detail != NotifyPointer)
            {
                wxFocusEvent event(wxEVT_KILL_FOCUS, canvas->GetId());
                event.SetEventObject(canvas);
                canvas->GetEventHandler()->ProcessEvent(event);
            }
            break;
        }
    default:
        break;
    }
}

static void wxPanelItemEventHandler(Widget    wid,
                                    XtPointer client_data,
                                    XEvent*   event,
                                    Boolean  *continueToDispatch)
{
    // Widget can be a label or the actual widget.

    wxWindow *window = wxGetWindowFromTable(wid);
    if (window)
    {
        wxMouseEvent wxevent(0);
        if (wxTranslateMouseEvent(wxevent, window, wid, event))
        {
            window->GetEventHandler()->ProcessEvent(wxevent);
        }
    }

    // TODO: probably the key to allowing default behaviour to happen. Say we
    // set a m_doDefault flag to FALSE at the start of this function. Then in
    // e.g. wxWindow::OnMouseEvent we can call Default() which sets this flag to
    // TRUE, indicating that default processing can happen. Thus, behaviour can
    // appear to be overridden just by adding an event handler and not calling
    // wxWindow::OnWhatever. ALSO, maybe we can use this instead of the current
    // way of handling drawing area events, to simplify things.
    *continueToDispatch = True;
}

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
            // TODO: Should this be intercepted too, or will it cause
            // duplicate events?
            eventType = wxEVT_SCROLLWIN_THUMBTRACK;
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

// For repainting arbitrary windows
void wxUniversalRepaintProc(Widget w, XtPointer WXUNUSED(c_data), XEvent *event, char *)
{
    Window window;
    Display *display;

    wxWindow* win = wxGetWindowFromTable(w);
    if (!win)
        return;

    switch(event -> type)
    {
    case Expose:
        {
            window = (Window) win -> GetXWindow();
            display = (Display *) win -> GetXDisplay();

            if (event -> xexpose.count == 0)
            {
                win->DoPaint();

                win->ClearUpdateRects();
            }
            else
            {
                win->AddUpdateRect(event->xexpose.x, event->xexpose.y,
                                   event->xexpose.width, event->xexpose.height);
            }

            break;
        }
    }
}

// ----------------------------------------------------------------------------
// CanvaseXXXSize() functions
// ----------------------------------------------------------------------------

// SetSize, but as per old wxCanvas (with drawing widget etc.)
void wxWindow::CanvasSetSize (int x, int y, int w, int h, int sizeFlags)
{
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

#if 0
    int ww, hh;
    GetClientSize (&ww, &hh);
    wxSizeEvent sizeEvent(wxSize(ww, hh), GetId());
    sizeEvent.SetEventObject(this);

    GetEventHandler()->ProcessEvent(sizeEvent);
#endif // 0
}

void wxWindow::CanvasSetClientSize (int w, int h)
{
    Widget drawingArea = (Widget) m_drawingArea;

    XtVaSetValues(drawingArea, XmNresizePolicy, XmRESIZE_ANY, NULL);

    if (w > -1)
        XtVaSetValues(drawingArea, XmNwidth, w, NULL);
    if (h > -1)
        XtVaSetValues(drawingArea, XmNheight, h, NULL);

#if 0
    // TODO: is this necessary?
    allowRepainting = FALSE;

    XSync (XtDisplay (drawingArea), FALSE);
    XEvent event;
    while (XtAppPending (wxTheApp->appContext))
    {
        XFlush (XtDisplay (drawingArea));
        XtAppNextEvent (wxTheApp->appContext, &event);
        XtDispatchEvent (&event);
    }
#endif // 0

    XtVaSetValues(drawingArea, XmNresizePolicy, XmRESIZE_NONE, NULL);

#if 0
    allowRepainting = TRUE;
    DoRefresh ();

    wxSizeEvent sizeEvent(wxSize(w, h), GetId());
    sizeEvent.SetEventObject(this);

    GetEventHandler()->ProcessEvent(sizeEvent);
#endif // 0
}

void wxWindow::CanvasGetClientSize (int *w, int *h) const
{
    // Must return the same thing that was set via SetClientSize
    Dimension xx, yy;
    XtVaGetValues ((Widget) m_drawingArea, XmNwidth, &xx, XmNheight, &yy, NULL);
    *w = xx;
    *h = yy;
}

void wxWindow::CanvasGetSize (int *w, int *h) const
{
    Dimension xx, yy;
    if ((Widget) m_borderWidget)
        XtVaGetValues ((Widget) m_borderWidget, XmNwidth, &xx, XmNheight, &yy, NULL);
    else if ((Widget) m_scrolledWindow)
        XtVaGetValues ((Widget) m_scrolledWindow, XmNwidth, &xx, XmNheight, &yy, NULL);
    else
        XtVaGetValues ((Widget) m_drawingArea, XmNwidth, &xx, XmNheight, &yy, NULL);

    *w = xx;
    *h = yy;
}

void wxWindow::CanvasGetPosition (int *x, int *y) const
{
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
}

// ----------------------------------------------------------------------------
// TranslateXXXEvent() functions
// ----------------------------------------------------------------------------

bool wxTranslateMouseEvent(wxMouseEvent& wxevent, wxWindow *win, Widget widget, XEvent *xevent)
{
    switch (xevent->xany.type)
    {
        case EnterNotify:  // never received here - yes ? MB
        case LeaveNotify:  // never received here - yes ? MB
        case ButtonPress:
        case ButtonRelease:
        case MotionNotify:
        {
            wxEventType eventType = wxEVT_NULL;

            // FIXME: this is never true I think - MB
            //
            if (xevent->xany.type == LeaveNotify)
            {
                win->SetButton1(FALSE);
                win->SetButton2(FALSE);
                win->SetButton3(FALSE);
                return FALSE;
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
                //
                long dclickTime = XtGetMultiClickTime((Display*) wxGetDisplay());
                long ts = wxevent.GetTimestamp();

                int buttonLast = win->GetLastClickedButton();
                long lastTS = win->GetLastClickTime();
                if ( buttonLast && buttonLast == button && (ts - lastTS) < dclickTime )
                {
                    // I have a dclick
                    win->SetLastClick(0, ts);
                    switch ( eventType )
                    {
                        case wxEVT_LEFT_DOWN:
                            eventType = wxEVT_LEFT_DCLICK;
                            break;
                        case wxEVT_MIDDLE_DOWN:
                            eventType = wxEVT_MIDDLE_DCLICK;
                            break;
                        case wxEVT_RIGHT_DOWN:
                            eventType = wxEVT_RIGHT_DCLICK;
                            break;

                        default :
                            break;
                    }
                    
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

            Position x1, y1;
            XtVaGetValues(widget, XmNx, &x1, XmNy, &y1, NULL);

            int x2, y2;
            win->GetPosition(&x2, &y2);

            // The button x/y must be translated to wxWindows
            // window space - the widget might be a label or button,
            // within a form.
            int dx = 0;
            int dy = 0;
            if (widget != (Widget)win->GetMainWidget())
            {
                dx = x1;
                dy = y1;
            }

            wxevent.m_x = xevent->xbutton.x + dx;
            wxevent.m_y = xevent->xbutton.y + dy;

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

bool wxTranslateKeyEvent(wxKeyEvent& wxevent, wxWindow *win, Widget widget, XEvent *xevent)
{
    switch (xevent->xany.type)
    {
    case KeyPress:
    case KeyRelease:
        {
            char buf[20];

            KeySym keySym;
#if 0
            XComposeStatus compose;
            (void) XLookupString ((XKeyEvent *) xevent, buf, 20, &keySym, &compose);
#endif // 0
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

// Changes the foreground and background colours to be derived from the current
// background colour. To change the foreground colour, you must call
// SetForegroundColour explicitly.
void wxWindow::ChangeBackgroundColour()
{
    WXWidget mainWidget = GetMainWidget();
    if ( mainWidget )
        DoChangeBackgroundColour(mainWidget, m_backgroundColour);

    // This not necessary
#if 0

    if (m_scrolledWindow && (GetMainWidget() != m_scrolledWindow))
    {
        DoChangeBackgroundColour(m_scrolledWindow, m_backgroundColour);
        // Have to set the scrollbar colours back since
        // the scrolled window seemed to change them
        wxColour backgroundColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE);

        if (m_hScrollBar)
            DoChangeBackgroundColour(m_hScrollBar, backgroundColour);
        if (m_vScrollBar)
            DoChangeBackgroundColour(m_vScrollBar, backgroundColour);
    }
#endif
}

void wxWindow::ChangeForegroundColour()
{
    WXWidget mainWidget = GetMainWidget();
    if ( mainWidget )
        DoChangeForegroundColour(mainWidget, m_foregroundColour);
    if ( m_scrolledWindow && mainWidget != m_scrolledWindow )
        DoChangeForegroundColour(m_scrolledWindow, m_foregroundColour);
}

// Change a widget's foreground and background colours.
void wxWindow::DoChangeForegroundColour(WXWidget widget, wxColour& foregroundColour)
{
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
}

void wxWindow::DoChangeBackgroundColour(WXWidget widget, wxColour& backgroundColour, bool changeArmColour)
{
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
}

bool wxWindow::SetBackgroundColour(const wxColour& col)
{
    if ( !wxWindowBase::SetBackgroundColour(col) )
        return FALSE;

    ChangeBackgroundColour();

    return TRUE;
}

bool wxWindow::SetForegroundColour(const wxColour& col)
{
    if ( !wxWindowBase::SetForegroundColour(col) )
        return FALSE;

    ChangeForegroundColour();

    return TRUE;
}

void wxWindow::ChangeFont(bool keepOriginalSize)
{
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
}

// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------

wxWindow *wxGetActiveWindow()
{
    // TODO
    return NULL;
}

// ----------------------------------------------------------------------------
// wxNoOptimize: switch off size optimization
// ----------------------------------------------------------------------------

int wxNoOptimize::ms_count = 0;




