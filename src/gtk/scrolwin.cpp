/////////////////////////////////////////////////////////////////////////////
// Name:        gtk/scrolwin.cpp
// Purpose:     wxScrolledWindow implementation
// Author:      Robert Roebling
// Modified by: Ron Lee
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "scrolwin.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/scrolwin.h"
#include "wx/utils.h"
#include "wx/dcclient.h"
#include "wx/panel.h"
#include "wx/sizer.h"

#include "wx/gtk/private.h"
#include "wx/gtk/win_gtk.h"

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxScrolledWindow, wxPanel)
    EVT_SCROLLWIN(wxScrolledWindow::OnScroll)
    EVT_SIZE(wxScrolledWindow::OnSize)
    EVT_PAINT(wxScrolledWindow::OnPaint)
    EVT_CHAR(wxScrolledWindow::OnChar)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxScrolledWindow, wxPanel)

// ============================================================================
// implementation
// ============================================================================

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool       g_blockEventsOnScroll;
extern bool g_mouseButtonDown;

//-----------------------------------------------------------------------------
// idle system
//-----------------------------------------------------------------------------

extern void wxapp_install_idle_handler();
extern bool g_isIdle;

//-----------------------------------------------------------------------------
// "value_changed" from m_[hv]Adjust
//-----------------------------------------------------------------------------

extern "C" {
static void gtk_scrolled_window_scroll_callback( GtkAdjustment *adjust,
                                                  SCROLLBAR_CBACK_ARG
                                                  wxScrolledWindow *win )
{
    if (g_isIdle)
        wxapp_install_idle_handler();

    win->HandleScrollEvent(adjust);
}
}

//-----------------------------------------------------------------------------
// "button_press_event" from scrollbar
//-----------------------------------------------------------------------------

extern "C" {
static gint gtk_scrollbar_button_press_callback( GtkRange *widget,
                                                 GdkEventButton *gdk_event,
                                                 wxWindowGTK *win)
{
    if (g_isIdle)
        wxapp_install_idle_handler();

    g_blockEventsOnScroll = TRUE;
    g_mouseButtonDown = true;

    return FALSE;
}
}

#ifdef __WXGTK20__
//-----------------------------------------------------------------------------
// "event_after" from scrollbar
//-----------------------------------------------------------------------------

extern "C" {
static void
gtk_scrollbar_event_after(GtkRange* range, GdkEvent* event, wxWindow* win)
{
    if (event->type == GDK_BUTTON_RELEASE)
    {
        g_signal_handlers_block_by_func(range, (void*)gtk_scrollbar_event_after, win);

        GtkAdjustment* adj = range->adjustment;
        wxASSERT(adj == win->m_hAdjust || adj == win->m_vAdjust);
        const int orient = adj == win->m_hAdjust ? wxHORIZONTAL : wxVERTICAL;
        wxScrollWinEvent event(wxEVT_SCROLLWIN_THUMBRELEASE, win->GetScrollPos(orient), orient);
        event.SetEventObject(win);
        win->GetEventHandler()->ProcessEvent(event);
    }
}
}
#endif // __WXGTK20__

//-----------------------------------------------------------------------------
// "button_release_event" from scrollbar
//-----------------------------------------------------------------------------

extern "C" {
static gint gtk_scrollbar_button_release_callback( GtkRange *widget,
                                                   GdkEventButton *WXUNUSED(gdk_event),
                                                   wxWindowGTK *win)
{
    g_blockEventsOnScroll = FALSE;
    g_mouseButtonDown = false;

    if (win->m_isScrolling)
    {
        win->m_isScrolling = false;
#ifdef __WXGTK20__
        g_signal_handlers_unblock_by_func(widget, (void*)gtk_scrollbar_event_after, win);
#else
        GtkAdjustment* adj = widget->adjustment;
        wxASSERT(adj == win->m_hAdjust || adj == win->m_vAdjust);
        const int orient = adj == win->m_hAdjust ? wxHORIZONTAL : wxVERTICAL;
        wxScrollWinEvent event(wxEVT_SCROLLWIN_THUMBRELEASE, win->GetScrollPos(orient), orient);
        event.SetEventObject(win);
        win->GetEventHandler()->ProcessEvent(event);
#endif
    }

    return FALSE;
}
}

//-----------------------------------------------------------------------------
// InsertChild for wxScrolledWindow
//-----------------------------------------------------------------------------

static void wxInsertChildInScrolledWindow( wxWindow* parent, wxWindow* child )
{
    // The window might have been scrolled already, do we
    // have to adapt the position.
    GtkPizza *pizza = GTK_PIZZA(parent->m_wxwindow);
    child->m_x += pizza->xoffset;
    child->m_y += pizza->yoffset;

    gtk_pizza_put( GTK_PIZZA(parent->m_wxwindow),
                     GTK_WIDGET(child->m_widget),
                     child->m_x,
                     child->m_y,
                     child->m_width,
                     child->m_height );
}

// ----------------------------------------------------------------------------
// wxScrolledWindow creation
// ----------------------------------------------------------------------------

void wxScrolledWindow::Init()
{
    m_xScrollPixelsPerLine = 0;
    m_yScrollPixelsPerLine = 0;
    m_xScrollingEnabled = TRUE;
    m_yScrollingEnabled = TRUE;
    m_xScrollPosition = 0;
    m_yScrollPosition = 0;
    m_xScrollLinesPerPage = 0;
    m_yScrollLinesPerPage = 0;
    m_targetWindow = (wxWindow*) NULL;
    m_scaleX = 1.0;
    m_scaleY = 1.0;
    m_hasScrolling = TRUE;
}

bool wxScrolledWindow::Create(wxWindow *parent,
                              wxWindowID id,
                              const wxPoint& pos,
                              const wxSize& size,
                              long style,
                              const wxString& name)
{
    Init();

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
    {
        wxFAIL_MSG( wxT("wxWindow creation failed") );
        return FALSE;
    }

    m_insertCallback = wxInsertChildInScrolledWindow;

    m_targetWindow = this;

    m_widget = gtk_scrolled_window_new( (GtkAdjustment *) NULL, (GtkAdjustment *) NULL );
    GTK_WIDGET_UNSET_FLAGS( m_widget, GTK_CAN_FOCUS );

    GtkScrolledWindow *scrolledWindow = GTK_SCROLLED_WINDOW(m_widget);

    GtkScrolledWindowClass *scroll_class = GTK_SCROLLED_WINDOW_CLASS( GTK_OBJECT_GET_CLASS(m_widget) );
    scroll_class->scrollbar_spacing = 0;

    gtk_scrolled_window_set_policy( scrolledWindow, GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );

    m_hAdjust = gtk_range_get_adjustment( GTK_RANGE(scrolledWindow->hscrollbar) );
    m_vAdjust = gtk_range_get_adjustment( GTK_RANGE(scrolledWindow->vscrollbar) );

    m_wxwindow = gtk_pizza_new();

    gtk_container_add( GTK_CONTAINER(m_widget), m_wxwindow );

    GtkPizza *pizza = GTK_PIZZA(m_wxwindow);

    if (HasFlag(wxRAISED_BORDER))
    {
        gtk_pizza_set_shadow_type( pizza, GTK_MYSHADOW_OUT );
    }
    else if (HasFlag(wxSUNKEN_BORDER))
    {
        gtk_pizza_set_shadow_type( pizza, GTK_MYSHADOW_IN );
    }
    else if (HasFlag(wxSIMPLE_BORDER))
    {
        gtk_pizza_set_shadow_type( pizza, GTK_MYSHADOW_THIN );
    }
    else
    {
        gtk_pizza_set_shadow_type( pizza, GTK_MYSHADOW_NONE );
    }

    GTK_WIDGET_SET_FLAGS( m_wxwindow, GTK_CAN_FOCUS );
    m_acceptsFocus = TRUE;

    // I _really_ don't want scrollbars in the beginning
    m_vAdjust->lower = 0.0;
    m_vAdjust->upper = 1.0;
    m_vAdjust->value = 0.0;
    m_vAdjust->step_increment = 1.0;
    m_vAdjust->page_increment = 2.0;
    gtk_adjustment_changed(m_vAdjust);
    m_hAdjust->lower = 0.0;
    m_hAdjust->upper = 1.0;
    m_hAdjust->value = 0.0;
    m_hAdjust->step_increment = 1.0;
    m_hAdjust->page_increment = 2.0;
    gtk_adjustment_changed(m_hAdjust);

    // Handlers for new scrollbar values
    gtk_signal_connect( GTK_OBJECT(m_vAdjust), "value_changed",
          (GtkSignalFunc) gtk_scrolled_window_scroll_callback, (gpointer) this );
    gtk_signal_connect( GTK_OBJECT(m_hAdjust), "value_changed",
          (GtkSignalFunc) gtk_scrolled_window_scroll_callback, (gpointer) this );

    // these handlers block mouse events to any window during scrolling such as
    // motion events and prevent GTK and wxWidgets from fighting over where the
    // slider should be

    gtk_signal_connect( GTK_OBJECT(scrolledWindow->vscrollbar), "button_press_event",
          (GtkSignalFunc)gtk_scrollbar_button_press_callback, (gpointer) this );

    gtk_signal_connect( GTK_OBJECT(scrolledWindow->hscrollbar), "button_press_event",
          (GtkSignalFunc)gtk_scrollbar_button_press_callback, (gpointer) this );

    gtk_signal_connect( GTK_OBJECT(scrolledWindow->vscrollbar), "button_release_event",
          (GtkSignalFunc)gtk_scrollbar_button_release_callback, (gpointer) this );

    gtk_signal_connect( GTK_OBJECT(scrolledWindow->hscrollbar), "button_release_event",
          (GtkSignalFunc)gtk_scrollbar_button_release_callback, (gpointer) this );

#ifdef __WXGTK20__
    gulong handler_id;
    handler_id = g_signal_connect(
        scrolledWindow->vscrollbar, "event_after", G_CALLBACK(gtk_scrollbar_event_after), this);
    g_signal_handler_block(scrolledWindow->vscrollbar, handler_id);
    handler_id = g_signal_connect(
        scrolledWindow->hscrollbar, "event_after", G_CALLBACK(gtk_scrollbar_event_after), this);
    g_signal_handler_block(scrolledWindow->hscrollbar, handler_id);
#endif

    gtk_widget_show( m_wxwindow );

    if (m_parent)
        m_parent->DoAddChild( this );

    m_focusWidget = m_wxwindow;

    PostCreation();

    Show( TRUE );
    
    return TRUE;
}

// ----------------------------------------------------------------------------
// setting scrolling parameters
// ----------------------------------------------------------------------------

void wxScrolledWindow::DoSetVirtualSize( int x, int y )
{
    wxPanel::DoSetVirtualSize( x, y );
    AdjustScrollbars();

    if (GetAutoLayout())
        Layout();
}

// wxWindow's GetBestVirtualSize returns the actual window size,
// whereas we want to return the virtual size
wxSize wxScrolledWindow::GetBestVirtualSize() const
{
    wxSize  clientSize( GetClientSize() );
    if (GetSizer())
    {
        wxSize minSize( GetSizer()->CalcMin() );

        return wxSize( wxMax( clientSize.x, minSize.x ), wxMax( clientSize.y, minSize.y ) );
    }
    else
        return clientSize;
}

// return the size best suited for the current window
// (this isn't a virtual size, this is a sensible size for the window)
wxSize wxScrolledWindow::DoGetBestSize() const
{
    wxSize best;

    bool addClientSizeDiff = true;
    if ( GetSizer() )
    {
        wxSize b = GetSizer()->GetMinSize();

        // Only use the content to set the window size in the direction
        // where there's no scrolling; otherwise we're going to get a huge
        // window in the direction in which scrolling is enabled
        int ppuX, ppuY;
        GetScrollPixelsPerUnit(& ppuX, & ppuY);

        wxSize minSize;
        if ( GetMinSize().IsFullySpecified() )
            minSize = GetMinSize();
        else
        {
            minSize = GetSize();
            
            // If we're using the overall size, we don't want to then
            // add to it, or we'll successively grow the window over
            // multiple layouts.
            addClientSizeDiff = false;
        }

        if (ppuX > 0)
            b.x = minSize.x;
        if (ppuY > 0)
            b.y = minSize.y;
        best = b;
    }
    else
        return wxWindow::DoGetBestSize();

    if (addClientSizeDiff)
    {
        // Add any difference between size and client size
        wxSize diff = GetSize() - GetClientSize();
        best.x += wxMax(0, diff.x);
        best.y += wxMax(0, diff.y);
    }

    return best;
}

/*
 * pixelsPerUnitX/pixelsPerUnitY: number of pixels per unit (e.g. pixels per text line)
 * noUnitsX/noUnitsY:        : no. units per scrollbar
 */
void wxScrolledWindow::SetScrollbars( int pixelsPerUnitX, int pixelsPerUnitY,
                                      int noUnitsX, int noUnitsY,
                                      int xPos, int yPos, bool noRefresh )
{
    int xs, ys;
    GetViewStart (& xs, & ys);
    
    int old_x = m_xScrollPixelsPerLine * xs;
    int old_y = m_yScrollPixelsPerLine * ys;

    m_xScrollPixelsPerLine = pixelsPerUnitX;
    m_yScrollPixelsPerLine = pixelsPerUnitY;

    m_hAdjust->value = m_xScrollPosition = xPos;
    m_vAdjust->value = m_yScrollPosition = yPos;

    // Setting hints here should arguably be deprecated, but without it
    // a sizer might override this manual scrollbar setting in old code.
    // m_targetWindow->SetVirtualSizeHints( noUnitsX * pixelsPerUnitX, noUnitsY * pixelsPerUnitY );

    int w = noUnitsX * pixelsPerUnitX;
    int h = noUnitsY * pixelsPerUnitY;
    m_targetWindow->SetVirtualSize( w ? w : wxDefaultCoord,
                                    h ? h : wxDefaultCoord);

    // If the target is not the same as the window with the scrollbars,
    // then we need to update the scrollbars here, since they won't have
    // been updated by SetVirtualSize().
    if (m_targetWindow != this)
    {
        AdjustScrollbars();
    }

    if (!noRefresh)
    {
        int new_x = m_xScrollPixelsPerLine * m_xScrollPosition;
        int new_y = m_yScrollPixelsPerLine * m_yScrollPosition;

        m_targetWindow->ScrollWindow( old_x - new_x, old_y - new_y );
    }
}

void wxScrolledWindow::AdjustScrollbars()
{
    int w, h;
    int vw, vh;

    m_targetWindow->GetClientSize( &w, &h );
    m_targetWindow->GetVirtualSize( &vw, &vh );
    
    DoAdjustScrollbars(m_hAdjust, m_xScrollPixelsPerLine, w, vw, &m_xScrollLinesPerPage);
    DoAdjustScrollbars(m_vAdjust, m_yScrollPixelsPerLine, h, vh, &m_yScrollLinesPerPage);
}

void wxScrolledWindow::DoAdjustScrollbars(GtkAdjustment* adj,
                                          int pixelsPerLine,
                                          int winSize,
                                          int virtSize,
                                          int *linesPerPage)
{
    int upper = 1;
    int page_size = 1;
    if (pixelsPerLine > 0 && winSize < virtSize)
    {
        upper = (virtSize + pixelsPerLine - 1) / pixelsPerLine;
        page_size = winSize / pixelsPerLine;
    }

    *linesPerPage = page_size;

    adj->step_increment = 1;
    adj->page_increment =
    adj->page_size = page_size;
    adj->lower = 0;
    adj->upper = upper;

    const double value = adj->value;
    if (adj->value > adj->upper - adj->page_size)
        adj->value = adj->upper - adj->page_size;
    if (adj->value < 0)
        adj->value = 0;
    if (!wxIsSameDouble(value, adj->value))
        gtk_adjustment_value_changed(adj);
    gtk_adjustment_changed(adj);
}


// ----------------------------------------------------------------------------
// target window handling
// ----------------------------------------------------------------------------

void wxScrolledWindow::SetTargetWindow( wxWindow *target, bool WXUNUSED(pushEventHandler) )
{
    wxASSERT_MSG( target, wxT("target window must not be NULL") );
    m_targetWindow = target;
}

wxWindow *wxScrolledWindow::GetTargetWindow() const
{
    return m_targetWindow;
}

// Override this function if you don't want to have wxScrolledWindow
// automatically change the origin according to the scroll position.
void wxScrolledWindow::DoPrepareDC(wxDC& dc)
{
    dc.SetDeviceOrigin( -m_xScrollPosition * m_xScrollPixelsPerLine,
                        -m_yScrollPosition * m_yScrollPixelsPerLine );
    dc.SetUserScale( m_scaleX, m_scaleY );
}

void wxScrolledWindow::SetScrollRate( int xstep, int ystep )
{
    int old_x = m_xScrollPixelsPerLine * m_xScrollPosition;
    int old_y = m_yScrollPixelsPerLine * m_yScrollPosition;

    m_xScrollPixelsPerLine = xstep;
    m_yScrollPixelsPerLine = ystep;

    int new_x = m_xScrollPixelsPerLine * m_xScrollPosition;
    int new_y = m_yScrollPixelsPerLine * m_yScrollPosition;

    m_targetWindow->ScrollWindow( old_x - new_x, old_y - new_y );

    AdjustScrollbars();
}

void wxScrolledWindow::GetScrollPixelsPerUnit (int *x_unit, int *y_unit) const
{
    if ( x_unit )
        *x_unit = m_xScrollPixelsPerLine;
    if ( y_unit )
        *y_unit = m_yScrollPixelsPerLine;
}

int wxScrolledWindow::GetScrollPageSize(int orient) const
{
    if ( orient == wxHORIZONTAL )
        return m_xScrollLinesPerPage;
    else
        return m_yScrollLinesPerPage;
}

void wxScrolledWindow::SetScrollPageSize(int orient, int pageSize)
{
    if ( orient == wxHORIZONTAL )
        m_xScrollLinesPerPage = pageSize;
    else
        m_yScrollLinesPerPage = pageSize;
}

void wxScrolledWindow::OnScroll(wxScrollWinEvent& event)
{
    int orient = event.GetOrientation();

    int nScrollInc = CalcScrollInc(event);
    if (nScrollInc == 0) return;

    if (orient == wxHORIZONTAL)
    {
        m_xScrollPosition += nScrollInc;
        SetScrollPos(wxHORIZONTAL, m_xScrollPosition);
        if (m_xScrollingEnabled)
            m_targetWindow->ScrollWindow(-m_xScrollPixelsPerLine * nScrollInc, 0, (const wxRect *) NULL);
        else
            m_targetWindow->Refresh();
    }
    else
    {
        m_yScrollPosition += nScrollInc;
        SetScrollPos(wxVERTICAL, m_yScrollPosition);
        if (m_yScrollingEnabled)
            m_targetWindow->ScrollWindow(0, -m_yScrollPixelsPerLine * nScrollInc, (const wxRect *) NULL);
        else
            m_targetWindow->Refresh();
    }
}

void wxScrolledWindow::Scroll( int x_pos, int y_pos )
{
    wxASSERT_MSG( m_targetWindow != 0, _T("No target window") );

    DoScroll(wxHORIZONTAL, x_pos, &m_xScrollPosition, m_xScrollPixelsPerLine);
    DoScroll(wxVERTICAL, y_pos, &m_yScrollPosition, m_yScrollPixelsPerLine);
}

void wxScrolledWindow::DoScroll(int orient, int pos, int* posOld, int pixelsPerLine)
{
    if (pos != -1 && pos != *posOld && pixelsPerLine > 0)
    {
        SetScrollPos(orient, pos);
        pos = GetScrollPos(orient);

        const int diff = (*posOld - pos) * pixelsPerLine;
        m_targetWindow->ScrollWindow(
            orient == wxHORIZONTAL ? diff : 0,
            orient == wxHORIZONTAL ? 0 : diff);

        *posOld = pos;
    }
}

void wxScrolledWindow::EnableScrolling (bool x_scroll, bool y_scroll)
{
    m_xScrollingEnabled = x_scroll;
    m_yScrollingEnabled = y_scroll;
}

// Where the current view starts from
void wxScrolledWindow::GetViewStart (int *x, int *y) const
{
    if ( x )
        *x = m_xScrollPosition;
    if ( y )
        *y = m_yScrollPosition;
}

void wxScrolledWindow::DoCalcScrolledPosition(int x, int y, int *xx, int *yy) const
{
    int xs, ys;
    GetViewStart (& xs, & ys);
    
    if ( xx )
        *xx = x - xs * m_xScrollPixelsPerLine;
    if ( yy )
        *yy = y - ys * m_yScrollPixelsPerLine;
}

void wxScrolledWindow::DoCalcUnscrolledPosition(int x, int y, int *xx, int *yy) const
{
    int xs, ys;
    GetViewStart (& xs, & ys);
    
    if ( xx )
        *xx = x + xs * m_xScrollPixelsPerLine;
    if ( yy )
        *yy = y + ys * m_yScrollPixelsPerLine;
}

int wxScrolledWindow::CalcScrollInc(wxScrollWinEvent& event)
{
    int pos = event.GetPosition();
    int orient = event.GetOrientation();

    int nScrollInc = 0;
    if (event.GetEventType() == wxEVT_SCROLLWIN_TOP)
    {
            if (orient == wxHORIZONTAL)
                nScrollInc = - m_xScrollPosition;
            else
                nScrollInc = - m_yScrollPosition;
    } else
    if (event.GetEventType() == wxEVT_SCROLLWIN_BOTTOM)
    {
            if (orient == wxHORIZONTAL)
                nScrollInc = GetVirtualSize().GetWidth() / m_xScrollPixelsPerLine - m_xScrollPosition;
            else
                nScrollInc = GetVirtualSize().GetHeight() / m_yScrollPixelsPerLine - m_yScrollPosition;
    } else
    if (event.GetEventType() == wxEVT_SCROLLWIN_LINEUP)
    {
            nScrollInc = -1;
    } else
    if (event.GetEventType() == wxEVT_SCROLLWIN_LINEDOWN)
    {
            nScrollInc = 1;
    } else
    if (event.GetEventType() == wxEVT_SCROLLWIN_PAGEUP)
    {
            if (orient == wxHORIZONTAL)
                nScrollInc = -GetScrollPageSize(wxHORIZONTAL);
            else
                nScrollInc = -GetScrollPageSize(wxVERTICAL);
    } else
    if (event.GetEventType() == wxEVT_SCROLLWIN_PAGEDOWN)
    {
            if (orient == wxHORIZONTAL)
                nScrollInc = GetScrollPageSize(wxHORIZONTAL);
            else
                nScrollInc = GetScrollPageSize(wxVERTICAL);
    } else
    if ((event.GetEventType() == wxEVT_SCROLLWIN_THUMBTRACK) ||
        (event.GetEventType() == wxEVT_SCROLLWIN_THUMBRELEASE))
    {
            if (orient == wxHORIZONTAL)
                nScrollInc = pos - m_xScrollPosition;
            else
                nScrollInc = pos - m_yScrollPosition;
    }

    if (orient == wxHORIZONTAL)
    {
        if (m_xScrollPixelsPerLine > 0)
        {
            int max = (int)(m_hAdjust->upper - m_hAdjust->page_size + 0.5);
            if (max < 0) max = 0;

            if ( (m_xScrollPosition + nScrollInc) < 0 )
                nScrollInc = -m_xScrollPosition; // As -ve as we can go
            else if ( (m_xScrollPosition + nScrollInc) > max )
                nScrollInc = max - m_xScrollPosition; // As +ve as we can go
        }
        else
            m_targetWindow->Refresh();
    }
    else
    {
        if (m_yScrollPixelsPerLine > 0)
        {
            int max = (int)(m_vAdjust->upper - m_vAdjust->page_size + 0.5);
            if (max < 0) max = 0;

            if ( (m_yScrollPosition + nScrollInc) < 0 )
                nScrollInc = -m_yScrollPosition; // As -ve as we can go
            else if ( (m_yScrollPosition + nScrollInc) > max )
                nScrollInc = max - m_yScrollPosition; // As +ve as we can go
        }
        else
            m_targetWindow->Refresh();
    }

    return nScrollInc;
}

void wxScrolledWindow::SetScrollPos( int orient, int pos, bool refresh )
{
    wxPanel::SetScrollPos(orient, pos, refresh);
}

bool wxScrolledWindow::Layout()
{
    if (GetSizer() && m_targetWindow == this)
    {
        // If we're the scroll target, take into account the
        // virtual size and scrolled position of the window.

        int x, y, w, h;
        CalcScrolledPosition(0,0, &x,&y);
        GetVirtualSize(&w, &h);
        GetSizer()->SetDimension(x, y, w, h);
        return TRUE;
    }
    else
        return wxPanel::Layout();  // fall back to default for LayoutConstraints
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

// Default OnSize resets scrollbars, if any
void wxScrolledWindow::OnSize(wxSizeEvent& WXUNUSED(event))
{
    if ( m_targetWindow->GetAutoLayout() )
    {
        wxSize size = m_targetWindow->GetBestVirtualSize();
        
        // This will call ::Layout() and ::AdjustScrollbars()
        SetVirtualSize( size );
    }
    else
    {
        AdjustScrollbars();
    }
}

// This calls OnDraw, having adjusted the origin according to the current
// scroll position
void wxScrolledWindow::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);
    PrepareDC(dc);

    OnDraw(dc);
}

// kbd handling: notice that we use OnChar() and not OnKeyDown() for
// compatibility here - if we used OnKeyDown(), the programs which process
// arrows themselves in their OnChar() would never get the message and like
// this they always have the priority
void wxScrolledWindow::OnChar(wxKeyEvent& event)
{
    int stx, sty,       // view origin
        szx, szy,       // view size (total)
        clix, cliy;     // view size (on screen)

    GetViewStart(&stx, &sty);
    GetClientSize(&clix, &cliy);
    GetVirtualSize(&szx, &szy);

    if( m_xScrollPixelsPerLine )
    {
        clix /= m_xScrollPixelsPerLine;
        szx /= m_xScrollPixelsPerLine;
    }
    else
    {
        clix = 0;
        szx = -1;
    }
    if( m_yScrollPixelsPerLine )
    {
        cliy /= m_yScrollPixelsPerLine;
        szy /= m_yScrollPixelsPerLine;
    }
    else
    {
        cliy = 0;
        szy = -1;
    }

    int xScrollOld = GetScrollPos(wxHORIZONTAL),
        yScrollOld = GetScrollPos(wxVERTICAL);

    int dsty;
    switch ( event.GetKeyCode() )
    {
        case WXK_PAGEUP:
        case WXK_PRIOR:
            dsty = sty - (5 * cliy / 6);
            Scroll(-1, (dsty == -1) ? 0 : dsty);
            break;

        case WXK_PAGEDOWN:
        case WXK_NEXT:
            Scroll(-1, sty + (5 * cliy / 6));
            break;

        case WXK_HOME:
            Scroll(0, event.ControlDown() ? 0 : -1);
            break;

        case WXK_END:
            Scroll(szx - clix, event.ControlDown() ? szy - cliy : -1);
            break;

        case WXK_UP:
            Scroll(-1, sty - 1);
            break;

        case WXK_DOWN:
            Scroll(-1, sty + 1);
            break;

        case WXK_LEFT:
            Scroll(stx - 1, -1);
            break;

        case WXK_RIGHT:
            Scroll(stx + 1, -1);
            break;

        default:
            // not for us
            event.Skip();
            return;
    }

    int xScroll = GetScrollPos(wxHORIZONTAL);
    if ( xScroll != xScrollOld )
    {
        wxScrollWinEvent event(wxEVT_SCROLLWIN_THUMBTRACK, xScroll,
                               wxHORIZONTAL);
        event.SetEventObject(this);
        GetEventHandler()->ProcessEvent(event);
    }

    int yScroll = GetScrollPos(wxVERTICAL);
    if ( yScroll != yScrollOld )
    {
        wxScrollWinEvent event(wxEVT_SCROLLWIN_THUMBTRACK, yScroll,
                               wxVERTICAL);
        event.SetEventObject(this);
        GetEventHandler()->ProcessEvent(event);
    }
}


// vi:sts=4:sw=4:et
