/////////////////////////////////////////////////////////////////////////////
// Name:        slider.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:           wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "slider.h"
#endif

#include "wx/slider.h"

#if wxUSE_SLIDER

#include "wx/utils.h"

#include <math.h>

#include <gdk/gdk.h>
#include <gtk/gtk.h>

//-----------------------------------------------------------------------------
// idle system
//-----------------------------------------------------------------------------

extern void wxapp_install_idle_handler();
extern bool g_isIdle;

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool   g_blockEventsOnDrag;

static const float sensitivity = 0.02;

//-----------------------------------------------------------------------------
// "value_changed"
//-----------------------------------------------------------------------------

static void gtk_slider_callback( GtkAdjustment *adjust, wxSlider *win )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (!win->m_hasVMT) return;
    if (g_blockEventsOnDrag) return;

    float diff = adjust->value - win->m_oldPos;
    if (fabs(diff) < sensitivity) return;

    win->m_oldPos = adjust->value;

    GtkRange *range = GTK_RANGE( win->m_widget );

    wxEventType command = wxEVT_SCROLL_THUMBTRACK;
    if      (range->scroll_type == GTK_SCROLL_STEP_BACKWARD) command = wxEVT_SCROLL_LINEUP;
    else if (range->scroll_type == GTK_SCROLL_STEP_FORWARD)  command = wxEVT_SCROLL_LINEDOWN;
    else if (range->scroll_type == GTK_SCROLL_PAGE_BACKWARD) command = wxEVT_SCROLL_PAGEUP;
    else if (range->scroll_type == GTK_SCROLL_PAGE_FORWARD)  command = wxEVT_SCROLL_PAGEDOWN;

    double dvalue = adjust->value;
    int value = (int)(dvalue >= 0 ? dvalue - 0.5 : dvalue + 0.5);

    int orient = wxHORIZONTAL;
    if ( (win->GetWindowStyleFlag() & wxSB_VERTICAL) == wxSB_VERTICAL)
        orient = wxVERTICAL;

    wxScrollEvent event( command, win->GetId(), value, orient );
    event.SetEventObject( win );
    win->GetEventHandler()->ProcessEvent( event );

    wxCommandEvent cevent( wxEVT_COMMAND_SLIDER_UPDATED, win->GetId() );
    cevent.SetEventObject( win );
    cevent.SetInt( value );
    win->GetEventHandler()->ProcessEvent( cevent );
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

    gtk_signal_connect( GTK_OBJECT(m_adjust),
                        "value_changed",
                        (GtkSignalFunc) gtk_slider_callback,
                        (gpointer) this );

    SetRange( minValue, maxValue );
    SetValue( value );

    m_parent->DoAddChild( this );

    PostCreation();

    SetBackgroundColour( parent->GetBackgroundColour() );

    Show( TRUE );

    return TRUE;
}

int wxSlider::GetValue() const
{
    // we want to round to the nearest integer, i.e. 0.9 is rounded to 1 and
    // -0.9 is rounded to -1
    double val = m_adjust->value;
    return (int)(val >= 0 ? val - 0.5 : val + 0.5);
}

void wxSlider::SetValue( int value )
{
    float fpos = (float)value;
    m_oldPos = fpos;
    if (fabs(fpos-m_adjust->value) < 0.2) return;

    m_adjust->value = fpos;

    gtk_signal_emit_by_name( GTK_OBJECT(m_adjust), "value_changed" );
}

void wxSlider::SetRange( int minValue, int maxValue )
{
    float fmin = (float)minValue;
    float fmax = (float)maxValue;

    if ((fabs(fmin-m_adjust->lower) < 0.2) &&
        (fabs(fmax-m_adjust->upper) < 0.2))
    {
        return;
    }

    m_adjust->lower = fmin;
    m_adjust->upper = fmax;
    m_adjust->step_increment = 1.0;
    m_adjust->page_increment = ceil((fmax-fmin) / 10.0);

    gtk_signal_emit_by_name( GTK_OBJECT(m_adjust), "changed" );
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
    float fpage = (float)pageSize;

    if (fabs(fpage-m_adjust->page_increment) < 0.2) return;

    m_adjust->page_increment = fpage;

    gtk_signal_emit_by_name( GTK_OBJECT(m_adjust), "changed" );
}

int wxSlider::GetPageSize() const
{
    return (int)ceil(m_adjust->page_increment);
}

void wxSlider::SetThumbLength( int len )
{
    float flen = (float)len;

    if (fabs(flen-m_adjust->page_size) < 0.2) return;

    m_adjust->page_size = flen;

    gtk_signal_emit_by_name( GTK_OBJECT(m_adjust), "changed" );
}

int wxSlider::GetThumbLength() const
{
    return (int)ceil(m_adjust->page_size);
}

void wxSlider::SetLineSize( int WXUNUSED(lineSize) )
{
}

int wxSlider::GetLineSize() const
{
    return 0;
}

void wxSlider::SetTick( int WXUNUSED(tickPos) )
{
}

void wxSlider::SetTickFreq( int WXUNUSED(n), int WXUNUSED(pos) )
{
}

int wxSlider::GetTickFreq() const
{
    return 0;
}

void wxSlider::ClearTicks()
{
}

void wxSlider::SetSelection( int WXUNUSED(minPos), int WXUNUSED(maxPos) )
{
}

int wxSlider::GetSelEnd() const
{
    return 0;
}

int wxSlider::GetSelStart() const
{
    return 0;
}

void wxSlider::ClearSel()
{
}

bool wxSlider::IsOwnGtkWindow( GdkWindow *window )
{
    GtkRange *range = GTK_RANGE(m_widget);
    return ( (window == GTK_WIDGET(range)->window) ||
             (window == range->trough) ||
             (window == range->slider) ||
             (window == range->step_forw) ||
             (window == range->step_back) );
}

void wxSlider::ApplyWidgetStyle()
{
    SetWidgetStyle();
    gtk_widget_set_style( m_widget, m_widgetStyle );
}

#endif
