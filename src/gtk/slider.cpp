/////////////////////////////////////////////////////////////////////////////
// Name:        gtk/slider.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "slider.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/slider.h"

#if wxUSE_SLIDER

#include "wx/utils.h"
#include "wx/math.h"
#include "wx/gtk/private.h"

//-----------------------------------------------------------------------------
// idle system
//-----------------------------------------------------------------------------

extern void wxapp_install_idle_handler();
extern bool g_isIdle;

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool g_blockEventsOnDrag;

// ----------------------------------------------------------------------------
// helper functions
// ----------------------------------------------------------------------------

// compare 2 adjustment values up to some (hardcoded) precision
static inline bool AreSameAdjustValues(double x, double y)
{
    return fabs(x - y) < 0.02;
}

static inline int AdjustValueToInt(double x)
{
    // we want to round to the nearest integer, i.e. 0.9 is rounded to 1 and
    // -0.9 is rounded to -1
    return (int)(x < 0 ? x - 0.5 : x + 0.5);
}

// process a scroll event
static void
ProcessScrollEvent(wxSlider *win, wxEventType evtType, double dvalue)
{
    const int orient = win->HasFlag(wxSL_VERTICAL) ? wxVERTICAL
                                                   : wxHORIZONTAL;

    const int value = (int)(dvalue < 0 ? dvalue - 0.5 : dvalue + 0.5);

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

//-----------------------------------------------------------------------------
// "value_changed"
//-----------------------------------------------------------------------------

extern "C" {
static void gtk_slider_callback( GtkAdjustment *adjust,
                                 SCROLLBAR_CBACK_ARG
                                 wxSlider *win )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (!win->m_hasVMT) return;
    if (g_blockEventsOnDrag) return;

    const double dvalue = adjust->value;
    const double diff = dvalue - win->m_oldPos;
    if ( AreSameAdjustValues(diff, 0) )
        return;

    wxEventType evtType;
    if ( win->m_isScrolling )
        evtType = wxEVT_SCROLL_THUMBTRACK;
    // it could seem that UP/DOWN are inversed but this is what wxMSW does
    else if ( AreSameAdjustValues(diff, adjust->step_increment) )
        evtType = wxEVT_SCROLL_LINEDOWN;
    else if ( AreSameAdjustValues(diff, -adjust->step_increment) )
        evtType = wxEVT_SCROLL_LINEUP;
    else if ( AreSameAdjustValues(diff, adjust->page_increment) )
        evtType = wxEVT_SCROLL_PAGEDOWN;
    else if ( AreSameAdjustValues(diff, -adjust->page_increment) )
        evtType = wxEVT_SCROLL_PAGEUP;
    else if ( AreSameAdjustValues(adjust->value, adjust->lower) )
        evtType = wxEVT_SCROLL_TOP;
    else if ( AreSameAdjustValues(adjust->value, adjust->upper) )
        evtType = wxEVT_SCROLL_BOTTOM;
    else
        evtType = wxEVT_NULL; // wxEVT_SCROLL_CHANGED will still be generated

    ProcessScrollEvent(win, evtType, dvalue);

    win->m_oldPos = dvalue;
}

#ifdef __WXGTK20__
static gint gtk_slider_button_press_callback( GtkWidget * /* widget */,
                                              GdkEventButton * /* gdk_event */,
                                              wxWindowGTK *win)
{
    // indicate that the thumb is being dragged with the mouse
    win->m_isScrolling = true;

    return FALSE;
}

static gint gtk_slider_button_release_callback( GtkWidget *scale,
                                                GdkEventButton * /* gdk_event */,
                                                wxSlider *win)
{
    // not scrolling any longer
    win->m_isScrolling = false;

    ProcessScrollEvent(win, wxEVT_SCROLL_THUMBRELEASE,
                       GTK_RANGE(scale)->adjustment->value);

    return FALSE;
}
#endif

}

//-----------------------------------------------------------------------------
// wxSlider
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxSlider,wxControl)

bool wxSlider::Create(wxWindow *parent, wxWindowID id,
        int value, int minValue, int maxValue,
        const wxPoint& pos, const wxSize& size,
        long style, const wxValidator& validator, const wxString& name )
{
    m_acceptsFocus = TRUE;
    m_needParent = TRUE;

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, validator, name ))
    {
        wxFAIL_MSG( wxT("wxSlider creation failed") );
        return FALSE;
    }

    m_oldPos = 0.0;

    if (style & wxSL_VERTICAL)
        m_widget = gtk_vscale_new( (GtkAdjustment *) NULL );
    else
        m_widget = gtk_hscale_new( (GtkAdjustment *) NULL );

    if (style & wxSL_LABELS)
    {
        gtk_scale_set_draw_value( GTK_SCALE( m_widget ), TRUE );
        gtk_scale_set_digits( GTK_SCALE( m_widget ), 0 );

        /* labels need more space and too small window will
           cause junk to appear on the dialog */
        if (style & wxSL_VERTICAL)
        {
            wxSize sz( size );
            if (sz.x < 35)
            {
                sz.x = 35;
                SetSize( sz );
            }
        }
        else
        {
            wxSize sz( size );
            if (sz.y < 35)
            {
                sz.y = 35;
                SetSize( sz );
            }
        }
    }
    else
        gtk_scale_set_draw_value( GTK_SCALE( m_widget ), FALSE );

    m_adjust = gtk_range_get_adjustment( GTK_RANGE(m_widget) );

#ifdef __WXGTK20__
    if (style & wxSL_INVERSE)
        gtk_range_set_inverted( GTK_RANGE(m_widget), TRUE );
#endif

    GtkEnableEvents();

#ifdef __WXGTK20__
    g_signal_connect (m_widget, "button_press_event",
                      G_CALLBACK (gtk_slider_button_press_callback),
                      this);
    g_signal_connect (m_widget, "button_release_event",
                      G_CALLBACK (gtk_slider_button_release_callback),
                      this);
#endif

    SetRange( minValue, maxValue );
    SetValue( value );

    m_parent->DoAddChild( this );

    PostCreation(size);

    return TRUE;
}

int wxSlider::GetValue() const
{
    return AdjustValueToInt(m_adjust->value);
}

void wxSlider::SetValue( int value )
{
    double fpos = (double)value;
    m_oldPos = fpos;
    if ( AreSameAdjustValues(fpos, m_adjust->value) )
        return;

    m_adjust->value = fpos;

    GtkDisableEvents();

    gtk_signal_emit_by_name( GTK_OBJECT(m_adjust), "value_changed" );

    GtkEnableEvents();
}

void wxSlider::SetRange( int minValue, int maxValue )
{
    double fmin = (double)minValue;
    double fmax = (double)maxValue;

    if ((fabs(fmin-m_adjust->lower) < 0.2) &&
        (fabs(fmax-m_adjust->upper) < 0.2))
    {
        return;
    }

    m_adjust->lower = fmin;
    m_adjust->upper = fmax;
    m_adjust->step_increment = 1.0;
    m_adjust->page_increment = ceil((fmax-fmin) / 10.0);

    GtkDisableEvents();

    gtk_signal_emit_by_name( GTK_OBJECT(m_adjust), "changed" );

    GtkEnableEvents();
}

int wxSlider::GetMin() const
{
    return (int)ceil(m_adjust->lower);
}

int wxSlider::GetMax() const
{
    return (int)ceil(m_adjust->upper);
}

void wxSlider::SetPageSize( int pageSize )
{
    double fpage = (double)pageSize;

    if (fabs(fpage-m_adjust->page_increment) < 0.2) return;

    m_adjust->page_increment = fpage;

    GtkDisableEvents();

    gtk_signal_emit_by_name( GTK_OBJECT(m_adjust), "changed" );

    GtkEnableEvents();
}

int wxSlider::GetPageSize() const
{
    return (int)ceil(m_adjust->page_increment);
}

void wxSlider::SetThumbLength( int len )
{
    double flen = (double)len;

    if (fabs(flen-m_adjust->page_size) < 0.2) return;

    m_adjust->page_size = flen;

    GtkDisableEvents();

    gtk_signal_emit_by_name( GTK_OBJECT(m_adjust), "changed" );

    GtkEnableEvents();
}

int wxSlider::GetThumbLength() const
{
    return (int)ceil(m_adjust->page_size);
}

void wxSlider::SetLineSize( int lineSize )
{
    double fline = (double)lineSize;

    if (fabs(fline-m_adjust->step_increment) < 0.2) return;

    m_adjust->step_increment = fline;

    GtkDisableEvents();

    gtk_signal_emit_by_name( GTK_OBJECT(m_adjust), "changed" );

    GtkEnableEvents();
}

int wxSlider::GetLineSize() const
{
    return (int)ceil(m_adjust->step_increment);
}

bool wxSlider::IsOwnGtkWindow( GdkWindow *window )
{
    GtkRange *range = GTK_RANGE(m_widget);
#ifdef __WXGTK20__
    return (range->event_window == window);
#else
    return ( (window == GTK_WIDGET(range)->window)
                || (window == range->trough)
                || (window == range->slider)
                || (window == range->step_forw)
                || (window == range->step_back) );
#endif
}

void wxSlider::GtkDisableEvents()
{
    gtk_signal_disconnect_by_func( GTK_OBJECT(m_adjust),
                        GTK_SIGNAL_FUNC(gtk_slider_callback),
                        (gpointer) this );
}

void wxSlider::GtkEnableEvents()
{
    gtk_signal_connect( GTK_OBJECT (m_adjust),
                        "value_changed",
                        GTK_SIGNAL_FUNC(gtk_slider_callback),
                        (gpointer) this );
}

// static
wxVisualAttributes
wxSlider::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    return GetDefaultAttributesFromGTKWidget(gtk_vscale_new);
}

#endif
