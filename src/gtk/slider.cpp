/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/slider.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_SLIDER

#include "wx/slider.h"

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/math.h"
#endif

#include "wx/gtk/private.h"

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool g_blockEventsOnDrag;

// ----------------------------------------------------------------------------
// helper functions
// ----------------------------------------------------------------------------

// process a scroll event
static void
ProcessScrollEvent(wxSlider *win, wxEventType evtType)
{
    const int orient = win->HasFlag(wxSL_VERTICAL) ? wxVERTICAL
                                                   : wxHORIZONTAL;

    const int value = win->GetValue();

    // if we have any "special" event (i.e. the value changed by a line or a
    // page), send this specific event first
    if ( evtType != wxEVT_NULL )
    {
        wxScrollEvent event( evtType, win->GetId(), value, orient );
        event.SetEventObject( win );
        win->GetEventHandler()->ProcessEvent( event );
    }

    // but, in any case, except if we're dragging the slider (and so the change
    // is not definitive), send a generic "changed" event
    if ( evtType != wxEVT_SCROLL_THUMBTRACK )
    {
        wxScrollEvent event(wxEVT_SCROLL_CHANGED, win->GetId(), value, orient);
        event.SetEventObject( win );
        win->GetEventHandler()->ProcessEvent( event );
    }

    // and also generate a command event for compatibility
    wxCommandEvent event( wxEVT_COMMAND_SLIDER_UPDATED, win->GetId() );
    event.SetEventObject( win );
    event.SetInt( value );
    win->GetEventHandler()->ProcessEvent( event );
}

static inline wxEventType GtkScrollTypeToWx(int scrollType)
{
    wxEventType eventType;
    switch (scrollType)
    {
    case GTK_SCROLL_STEP_BACKWARD:
    case GTK_SCROLL_STEP_LEFT:
    case GTK_SCROLL_STEP_UP:
        eventType = wxEVT_SCROLL_LINEUP;
        break;
    case GTK_SCROLL_STEP_DOWN:
    case GTK_SCROLL_STEP_FORWARD:
    case GTK_SCROLL_STEP_RIGHT:
        eventType = wxEVT_SCROLL_LINEDOWN;
        break;
    case GTK_SCROLL_PAGE_BACKWARD:
    case GTK_SCROLL_PAGE_LEFT:
    case GTK_SCROLL_PAGE_UP:
        eventType = wxEVT_SCROLL_PAGEUP;
        break;
    case GTK_SCROLL_PAGE_DOWN:
    case GTK_SCROLL_PAGE_FORWARD:
    case GTK_SCROLL_PAGE_RIGHT:
        eventType = wxEVT_SCROLL_PAGEDOWN;
        break;
    case GTK_SCROLL_START:
        eventType = wxEVT_SCROLL_TOP;
        break;
    case GTK_SCROLL_END:
        eventType = wxEVT_SCROLL_BOTTOM;
        break;
    case GTK_SCROLL_JUMP:
        eventType = wxEVT_SCROLL_THUMBTRACK;
        break;
    default:
        wxFAIL_MSG(_T("Unknown GtkScrollType"));
        eventType = wxEVT_NULL;
        break;
    }
    return eventType;
}

// Determine if increment is the same as +/-x, allowing for some small
//   difference due to possible inexactness in floating point arithmetic
static inline bool IsScrollIncrement(double increment, double x)
{
    wxASSERT(increment > 0);
    const double tolerance = 1.0 / 1024;
    return fabs(increment - fabs(x)) < tolerance;
}

//-----------------------------------------------------------------------------
// "value_changed"
//-----------------------------------------------------------------------------

extern "C" {
static void
gtk_value_changed(GtkRange* range, wxSlider* win)
{
    if (g_isIdle) wxapp_install_idle_handler();

    GtkAdjustment* adj = gtk_range_get_adjustment (range);
    const int pos = wxRound(adj->value);
    const double oldPos = win->m_pos;
    win->m_pos = adj->value;

    if (!win->m_hasVMT || g_blockEventsOnDrag)
        return;

    if (win->m_blockScrollEvent)
    {
        win->m_scrollEventType = GTK_SCROLL_NONE;
        return;
    }

    wxEventType eventType = wxEVT_NULL;
    if (win->m_isScrolling)
    {
        eventType = wxEVT_SCROLL_THUMBTRACK;
    }
    else if (win->m_scrollEventType != GTK_SCROLL_NONE)
    {
        // Scroll event from "move-slider" (keyboard)
        eventType = GtkScrollTypeToWx(win->m_scrollEventType);
    }
    else if (win->m_mouseButtonDown)
    {
        // Difference from last change event
        const double diff = adj->value - oldPos;
        const bool isDown = diff > 0;

        if (IsScrollIncrement(adj->page_increment, diff))
        {
            eventType = isDown ? wxEVT_SCROLL_PAGEDOWN : wxEVT_SCROLL_PAGEUP;
        }
        else if (wxIsSameDouble(adj->value, 0))
        {
            eventType = wxEVT_SCROLL_PAGEUP;
        }
        else if (wxIsSameDouble(adj->value, adj->upper))
        {
            eventType = wxEVT_SCROLL_PAGEDOWN;
        }
        else
        {
            // Assume track event
            eventType = wxEVT_SCROLL_THUMBTRACK;
            // Remember that we're tracking
            win->m_isScrolling = true;
        }
    }

    win->m_scrollEventType = GTK_SCROLL_NONE;

    // If integral position has changed
    if (wxRound(oldPos) != pos)
    {
        wxCHECK_RET(eventType != wxEVT_NULL, _T("Unknown slider scroll event type"));
        ProcessScrollEvent(win, eventType);
        win->m_needThumbRelease = eventType == wxEVT_SCROLL_THUMBTRACK;
    }
}
}

//-----------------------------------------------------------------------------
// "move_slider" (keyboard event)
//-----------------------------------------------------------------------------

extern "C" {
static void
gtk_move_slider(GtkRange*, GtkScrollType scrollType, wxSlider* win)
{
    // Save keyboard scroll type for "value_changed" handler
    win->m_scrollEventType = scrollType;
}
}

//-----------------------------------------------------------------------------
// "button_press_event"
//-----------------------------------------------------------------------------

extern "C" {
static gboolean
gtk_button_press_event(GtkWidget*, GdkEventButton*, wxSlider* win)
{
    win->m_mouseButtonDown = true;

    return false;
}
}

//-----------------------------------------------------------------------------
// "event_after"
//-----------------------------------------------------------------------------

extern "C" {
static void
gtk_event_after(GtkRange* range, GdkEvent* event, wxSlider* win)
{
    if (event->type == GDK_BUTTON_RELEASE)
    {
        g_signal_handlers_block_by_func(range, (gpointer) gtk_event_after, win);

        if (win->m_needThumbRelease)
        {
            win->m_needThumbRelease = false;
            ProcessScrollEvent(win, wxEVT_SCROLL_THUMBRELEASE);
        }
        // Keep slider at an integral position
        win->BlockScrollEvent();
        gtk_range_set_value(GTK_RANGE (win->m_widget), win->GetValue());
        win->UnblockScrollEvent();
    }
}
}

//-----------------------------------------------------------------------------
// "button_release_event"
//-----------------------------------------------------------------------------

extern "C" {
static gboolean
gtk_button_release_event(GtkRange* range, GdkEventButton*, wxSlider* win)
{
    win->m_mouseButtonDown = false;
    if (win->m_isScrolling)
    {
        win->m_isScrolling = false;
        g_signal_handlers_unblock_by_func(range, (gpointer) gtk_event_after, win);
    }
    return false;
}
}

//-----------------------------------------------------------------------------
// "format_value"
//-----------------------------------------------------------------------------

extern "C" {
static gchar* gtk_format_value(GtkScale*, double value, void*)
{
    // Format value as nearest integer
    return g_strdup_printf("%d", wxRound(value));
}
}

//-----------------------------------------------------------------------------
// wxSlider
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxSlider,wxControl)

wxSlider::wxSlider()
{
    m_pos = 0;
    m_scrollEventType = 0;
    m_needThumbRelease = false;
}

bool wxSlider::Create(wxWindow *parent, wxWindowID id,
        int value, int minValue, int maxValue,
        const wxPoint& pos, const wxSize& size,
        long style, const wxValidator& validator, const wxString& name )
{
    m_acceptsFocus = true;
    m_needParent = true;

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, validator, name ))
    {
        wxFAIL_MSG( wxT("wxSlider creation failed") );
        return false;
    }

    m_pos = 0;
    m_scrollEventType = 0;
    m_needThumbRelease = false;

    if (style & wxSL_VERTICAL)
        m_widget = gtk_vscale_new( (GtkAdjustment *) NULL );
    else
        m_widget = gtk_hscale_new( (GtkAdjustment *) NULL );

    gtk_scale_set_draw_value(GTK_SCALE (m_widget), (style & wxSL_LABELS) != 0);
    // Keep full precision in position value
    gtk_scale_set_digits(GTK_SCALE (m_widget), -1);

    if (style & wxSL_INVERSE)
        gtk_range_set_inverted( GTK_RANGE(m_widget), TRUE );

    g_signal_connect(m_widget, "button_press_event", G_CALLBACK(gtk_button_press_event), this);
    g_signal_connect(m_widget, "button_release_event", G_CALLBACK(gtk_button_release_event), this);
    g_signal_connect(m_widget, "move_slider", G_CALLBACK(gtk_move_slider), this);
    g_signal_connect(m_widget, "format_value", G_CALLBACK(gtk_format_value), NULL);
    g_signal_connect(m_widget, "value_changed", G_CALLBACK(gtk_value_changed), this);
    gulong handler_id;
    handler_id = g_signal_connect(
        m_widget, "event_after", G_CALLBACK(gtk_event_after), this);
    g_signal_handler_block(m_widget, handler_id);

    SetRange( minValue, maxValue );
    SetValue( value );

    m_parent->DoAddChild( this );

    PostCreation(size);

    return true;
}

int wxSlider::GetValue() const
{
    return wxRound(m_pos);
}

void wxSlider::SetValue( int value )
{
    if (GetValue() != value)
    {
        BlockScrollEvent();
        gtk_range_set_value(GTK_RANGE (m_widget), value);
        UnblockScrollEvent();
    }
}

void wxSlider::SetRange( int minValue, int maxValue )
{
    BlockScrollEvent();
    gtk_range_set_range(GTK_RANGE (m_widget), minValue, maxValue);
    gtk_range_set_increments(GTK_RANGE (m_widget), 1, (maxValue - minValue + 9) / 10);
    UnblockScrollEvent();
}

int wxSlider::GetMin() const
{
    return int(gtk_range_get_adjustment (GTK_RANGE (m_widget))->lower);
}

int wxSlider::GetMax() const
{
    return int(gtk_range_get_adjustment (GTK_RANGE (m_widget))->upper);
}

void wxSlider::SetPageSize( int pageSize )
{
    BlockScrollEvent();
    gtk_range_set_increments(GTK_RANGE (m_widget), GetLineSize(), pageSize);
    UnblockScrollEvent();
}

int wxSlider::GetPageSize() const
{
    return int(gtk_range_get_adjustment (GTK_RANGE (m_widget))->page_increment);
}

// GTK does not support changing the size of the slider
void wxSlider::SetThumbLength(int)
{
}

int wxSlider::GetThumbLength() const
{
    return 0;
}

void wxSlider::SetLineSize( int lineSize )
{
    BlockScrollEvent();
    gtk_range_set_increments(GTK_RANGE (m_widget), lineSize, GetPageSize());
    UnblockScrollEvent();
}

int wxSlider::GetLineSize() const
{
    return int(gtk_range_get_adjustment (GTK_RANGE (m_widget))->step_increment);
}

GdkWindow *wxSlider::GTKGetWindow(wxArrayGdkWindows& WXUNUSED(windows)) const
{
    return GTK_RANGE(m_widget)->event_window;
}

// static
wxVisualAttributes
wxSlider::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    return GetDefaultAttributesFromGTKWidget(gtk_vscale_new);
}

#endif // wxUSE_SLIDER
