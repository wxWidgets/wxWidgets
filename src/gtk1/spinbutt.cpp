/////////////////////////////////////////////////////////////////////////////
// Name:        spinbutt.cpp
// Purpose:     wxSpinButton
// Author:      Robert
// Modified by:
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "spinbutt.h"
#endif

#include "wx/spinbutt.h"
#include "wx/utils.h"
#include <math.h>

#include "gdk/gdk.h"
#include "gtk/gtk.h"

//-----------------------------------------------------------------------------
// idle system
//-----------------------------------------------------------------------------

extern void wxapp_install_idle_handler();
extern bool g_isIdle;

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool   g_blockEventsOnDrag;

static const float sensitivity = 0.2;

//-----------------------------------------------------------------------------
// "value_changed"
//-----------------------------------------------------------------------------

static void gtk_spinbutt_callback( GtkWidget *WXUNUSED(widget), wxSpinButton *win )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (!win->HasVMT()) return;
    if (g_blockEventsOnDrag) return;

    float diff = win->m_adjust->value - win->m_oldPos;
    if (fabs(diff) < sensitivity) return;
    win->m_oldPos = win->m_adjust->value;

    wxEventType command = wxEVT_NULL;

    float line_step = win->m_adjust->step_increment;
    float page_step = win->m_adjust->page_increment;

    if (fabs(diff-line_step) < sensitivity) command = wxEVT_SCROLL_LINEDOWN;
    else if (fabs(diff+line_step) < sensitivity) command = wxEVT_SCROLL_LINEUP;
    else if (fabs(diff-page_step) < sensitivity) command = wxEVT_SCROLL_PAGEDOWN;
    else if (fabs(diff+page_step) < sensitivity) command = wxEVT_SCROLL_PAGEUP;
    else command = wxEVT_SCROLL_THUMBTRACK;

    int value = (int)ceil(win->m_adjust->value);

    wxSpinEvent event( command, win->GetId());
    event.SetPosition( value );
    event.SetOrientation( wxVERTICAL );
    event.SetEventObject( win );

    win->GetEventHandler()->ProcessEvent( event );
}

//-----------------------------------------------------------------------------
// wxSpinButton
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxSpinButton,wxControl)

BEGIN_EVENT_TABLE(wxSpinButton, wxControl)
    EVT_SIZE(wxSpinButton::OnSize)
END_EVENT_TABLE()

wxSpinButton::wxSpinButton()
{
}

bool wxSpinButton::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
            long style, const wxString& name)
{
    m_needParent = TRUE;

    wxSize new_size = size;
    new_size.x = 16;
    if (new_size.y == -1)
        new_size.y = 30;

    PreCreation( parent, id, pos, new_size, style, name );

//  SetValidator( validator );

    m_oldPos = 0.0;

    m_adjust = (GtkAdjustment*) gtk_adjustment_new( 0.0, 0.0, 100.0, 1.0, 5.0, 0.0);

    m_widget = gtk_spin_button_new( m_adjust, 0, 0 );

    gtk_spin_button_set_wrap( GTK_SPIN_BUTTON(m_widget), (m_windowStyle & wxSP_WRAP) );

    gtk_signal_connect( GTK_OBJECT (m_adjust),
                        "value_changed",
                        (GtkSignalFunc) gtk_spinbutt_callback,
                        (gpointer) this );

    m_parent->AddChild( this );

    (m_parent->m_insertCallback)( m_parent, this );

    PostCreation();

    SetBackgroundColour( parent->GetBackgroundColour() );

    Show( TRUE );

    return TRUE;
}

wxSpinButton::~wxSpinButton()
{
}

int wxSpinButton::GetMin() const
{
    wxCHECK_MSG( (m_widget != NULL), 0, _T("invalid spin button") );

    return (int)ceil(m_adjust->lower);
}

int wxSpinButton::GetMax() const
{
    wxCHECK_MSG( (m_widget != NULL), 0, _T("invalid spin button") );

    return (int)ceil(m_adjust->upper);
}

int wxSpinButton::GetValue() const
{
    wxCHECK_MSG( (m_widget != NULL), 0, _T("invalid spin button") );

    return (int)ceil(m_adjust->value);
}

void wxSpinButton::SetValue( int value )
{
    wxCHECK_RET( (m_widget != NULL), _T("invalid spin button") );

    float fpos = (float)value;
    m_oldPos = fpos;
    if (fabs(fpos-m_adjust->value) < sensitivity) return;

    m_adjust->value = fpos;

    gtk_signal_emit_by_name( GTK_OBJECT(m_adjust), "value_changed" );
}

void wxSpinButton::SetRange(int minVal, int maxVal)
{
    wxCHECK_RET( (m_widget != NULL), _T("invalid spin button") );

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
}

void wxSpinButton::OnSize( wxSizeEvent &WXUNUSED(event) )
{
    wxCHECK_RET( (m_widget != NULL), _T("invalid spin button") );

    m_width = 16;
    gtk_widget_set_usize( m_widget, m_width, m_height );
}

bool wxSpinButton::IsOwnGtkWindow( GdkWindow *window )
{
    return GTK_SPIN_BUTTON(m_widget)->panel == window;
}

void wxSpinButton::ApplyWidgetStyle()
{
    SetWidgetStyle();
    gtk_widget_set_style( m_widget, m_widgetStyle );
}

//-----------------------------------------------------------------------------
// wxSpinEvent
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxSpinEvent, wxScrollEvent)

wxSpinEvent::wxSpinEvent(wxEventType commandType, int id):
  wxScrollEvent(commandType, id)
{
}
