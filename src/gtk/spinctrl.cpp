/////////////////////////////////////////////////////////////////////////////
// Name:        spinbutt.cpp
// Purpose:     wxSpinCtrl
// Author:      Robert
// Modified by:
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "spinctrl.h"
#endif

#include "wx/spinctrl.h"

#if wxUSE_SPINCTRL

#include "wx/utils.h"

#include <math.h>

#include <gdk/gdk.h>
#include <gtk/gtk.h>

//-----------------------------------------------------------------------------
// idle system
//-----------------------------------------------------------------------------

extern void wxapp_install_idle_handler();
extern bool g_isIdle;

static const float sensitivity = 0.02;

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool   g_blockEventsOnDrag;

//-----------------------------------------------------------------------------
// "value_changed"
//-----------------------------------------------------------------------------

static void gtk_spinctrl_callback( GtkWidget *WXUNUSED(widget), wxSpinCtrl *win )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (!win->m_hasVMT) return;
    if (g_blockEventsOnDrag) return;

    wxCommandEvent event( wxEVT_COMMAND_SPINCTRL_UPDATED, win->GetId());
    event.SetEventObject( win );
    event.SetInt( win->GetValue() );
    win->GetEventHandler()->ProcessEvent( event );
}

//-----------------------------------------------------------------------------
// wxSpinCtrl
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxSpinCtrl,wxControl)

BEGIN_EVENT_TABLE(wxSpinCtrl, wxControl)
    EVT_CHAR(wxSpinCtrl::OnChar)
END_EVENT_TABLE()

bool wxSpinCtrl::Create(wxWindow *parent, wxWindowID id,
                        const wxString& value,
                        const wxPoint& pos,  const wxSize& size,
                        long style,
                        int min, int max, int initial,
                        const wxString& name)
{
    m_needParent = TRUE;
    m_acceptsFocus = TRUE;

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
    {
        wxFAIL_MSG( wxT("wxSpinCtrl creation failed") );
        return FALSE;
    }

    m_oldPos = initial;

    m_adjust = (GtkAdjustment*) gtk_adjustment_new( initial, min, max, 1.0, 5.0, 0.0);

    m_widget = gtk_spin_button_new( m_adjust, 1, 0 );
    
    wxSize new_size = size,
           sizeBest = DoGetBestSize();
    if (new_size.x == -1)
        new_size.x = sizeBest.x;
    if (new_size.y == -1)
        new_size.y = sizeBest.y;

    if ((new_size.x != size.x) || (new_size.y != size.y))
        SetSize( new_size.x, new_size.y );

    gtk_spin_button_set_wrap( GTK_SPIN_BUTTON(m_widget),
                              (int)(m_windowStyle & wxSP_WRAP) );

    GtkEnableEvents();
    m_parent->DoAddChild( this );

    PostCreation();

    SetBackgroundColour( parent->GetBackgroundColour() );

    SetValue( value );

    Show( TRUE );

    return TRUE;
}

void wxSpinCtrl::GtkDisableEvents()
{
    gtk_signal_disconnect_by_func( GTK_OBJECT(m_adjust),
                        GTK_SIGNAL_FUNC(gtk_spinctrl_callback),
                        (gpointer) this );

}

void wxSpinCtrl::GtkEnableEvents()
{
    gtk_signal_connect( GTK_OBJECT (m_adjust),
                        "value_changed",
                        GTK_SIGNAL_FUNC(gtk_spinctrl_callback),
                        (gpointer) this );
}

int wxSpinCtrl::GetMin() const
{
    wxCHECK_MSG( (m_widget != NULL), 0, wxT("invalid spin button") );

    return (int)ceil(m_adjust->lower);
}

int wxSpinCtrl::GetMax() const
{
    wxCHECK_MSG( (m_widget != NULL), 0, wxT("invalid spin button") );

    return (int)ceil(m_adjust->upper);
}

int wxSpinCtrl::GetValue() const
{
    wxCHECK_MSG( (m_widget != NULL), 0, wxT("invalid spin button") );

    gtk_spin_button_update( GTK_SPIN_BUTTON(m_widget) );

    return (int)ceil(m_adjust->value);
}

void wxSpinCtrl::SetValue( const wxString& value )
{
    wxCHECK_RET( (m_widget != NULL), wxT("invalid spin button") );

    int n;
    if ( (wxSscanf(value, wxT("%d"), &n) == 1) )
    {
        // a number - set it
        SetValue(n);
    }
    else
    {
        // invalid number - set text as is (wxMSW compatible)
        GtkDisableEvents();
        gtk_entry_set_text( GTK_ENTRY(m_widget), value.mbc_str() );
        GtkEnableEvents();
    }
}

void wxSpinCtrl::SetValue( int value )
{
    wxCHECK_RET( (m_widget != NULL), wxT("invalid spin button") );

    float fpos = (float)value;
    m_oldPos = fpos;
    if (fabs(fpos-m_adjust->value) < sensitivity) return;

    m_adjust->value = fpos;

    GtkDisableEvents();
    gtk_signal_emit_by_name( GTK_OBJECT(m_adjust), "value_changed" );
    GtkEnableEvents();
}

void wxSpinCtrl::SetRange(int minVal, int maxVal)
{
    wxCHECK_RET( (m_widget != NULL), wxT("invalid spin button") );

    float fmin = (float)minVal;
    float fmax = (float)maxVal;

    if ((fabs(fmin-m_adjust->lower) < sensitivity) &&
        (fabs(fmax-m_adjust->upper) < sensitivity))
    {
        return;
    }

    m_adjust->lower = fmin;
    m_adjust->upper = fmax;

    gtk_signal_emit_by_name( GTK_OBJECT(m_adjust), "changed" );

    // these two calls are required due to some bug in GTK
    Refresh();
    SetFocus();
}

void wxSpinCtrl::OnChar( wxKeyEvent &event )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid spin ctrl") );

    if (event.KeyCode() == WXK_RETURN)
    {
        wxWindow *top_frame = m_parent;
        while (top_frame->GetParent() && !(top_frame->GetParent()->m_isFrame))
            top_frame = top_frame->GetParent();
        GtkWindow *window = GTK_WINDOW(top_frame->m_widget);

        if (window->default_widget)
        {
            gtk_widget_activate (window->default_widget);
            return;
        }
    }

    event.Skip();
}

bool wxSpinCtrl::IsOwnGtkWindow( GdkWindow *window )
{
    return GTK_SPIN_BUTTON(m_widget)->panel == window;
}

void wxSpinCtrl::ApplyWidgetStyle()
{
    SetWidgetStyle();
    gtk_widget_set_style( m_widget, m_widgetStyle );
}

wxSize wxSpinCtrl::DoGetBestSize() const
{
    wxSize ret( wxControl::DoGetBestSize() );
    return wxSize(95, ret.y);
}

#endif
   // wxUSE_SPINCTRL
