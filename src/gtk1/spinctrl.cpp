/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk1/spinctrl.cpp
// Purpose:     wxSpinCtrl
// Author:      Robert
// Modified by:
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_SPINCTRL

#include "wx/spinctrl.h"

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/textctrl.h"    // for wxEVT_TEXT
    #include "wx/math.h"
    #include "wx/crt.h"
#endif

#include "wx/private/spinctrl.h"

#include "wx/gtk1/private.h"

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

extern "C" {

static gboolean
wx_gtk_spin_input(GtkSpinButton* spin, gdouble* val, wxSpinCtrl* win)
{
    // We might use g_ascii_strtoll() here but it's 2.12+ only, so use our own
    // wxString function even if this requires an extra conversion.
    const wxString
        text(wxString::FromUTF8(gtk_entry_get_text(GTK_ENTRY(spin))));

    long lval;
    if ( !text.ToLong(&lval, win->GetBase()) )
        return FALSE;

    *val = lval;

    return TRUE;
}

static gint
wx_gtk_spin_output(GtkSpinButton* spin, wxSpinCtrl* win)
{
    const gint val = gtk_spin_button_get_value_as_int(spin);

    gtk_entry_set_text
    (
        GTK_ENTRY(spin),
        wxSpinCtrlImpl::FormatAsHex(val, win->GetMax()).utf8_str()
    );

    return TRUE;
}

static void gtk_spinctrl_callback( GtkWidget *WXUNUSED(widget), wxSpinCtrl *win )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (!win->m_hasVMT) return;
    if (g_blockEventsOnDrag) return;

    wxSpinEvent event( wxEVT_SPINCTRL, win->GetId());
    event.SetEventObject( win );

    // note that we don't use wxSpinCtrl::GetValue() here because it would
    // adjust the value to fit into the control range and this means that we
    // would never be able to enter an "invalid" value in the control, even
    // temporarily - and trying to enter 10 into the control which accepts the
    // values in range 5..50 is then, ummm, quite challenging (hint: you can't
    // enter 1!) (VZ)
    event.SetInt( (int)ceil(win->m_adjust->value) );
    win->HandleWindowEvent( event );
}
}

//-----------------------------------------------------------------------------
//  "changed"
//-----------------------------------------------------------------------------

extern "C" {
static void
gtk_spinctrl_text_changed_callback( GtkWidget *WXUNUSED(widget), wxSpinCtrl *win )
{
    if (!win->m_hasVMT) return;

    if (g_isIdle)
        wxapp_install_idle_handler();

    wxCommandEvent event( wxEVT_TEXT, win->GetId() );
    event.SetEventObject( win );

    // see above
    event.SetInt( (int)ceil(win->m_adjust->value) );
    win->HandleWindowEvent( event );
}
}

//-----------------------------------------------------------------------------
// wxSpinCtrl
//-----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(wxSpinCtrl, wxControl)
    EVT_CHAR(wxSpinCtrl::OnChar)
wxEND_EVENT_TABLE()

bool wxSpinCtrl::Create(wxWindow *parent, wxWindowID id,
                        const wxString& value,
                        const wxPoint& pos,  const wxSize& size,
                        long style,
                        int min, int max, int initial,
                        const wxString& name)
{
    m_needParent = true;
    m_acceptsFocus = true;

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
    {
        wxFAIL_MSG( wxT("wxSpinCtrl creation failed") );
        return false;
    }

    m_oldPos = initial;

    m_adjust = (GtkAdjustment*) gtk_adjustment_new( initial, min, max, 1.0, 5.0, 0.0);

    m_widget = gtk_spin_button_new( m_adjust, 1, 0 );

    gtk_spin_button_set_wrap( GTK_SPIN_BUTTON(m_widget),
                              (int)(m_windowStyle & wxSP_WRAP) );

    GtkEnableEvents();

    m_parent->DoAddChild( this );

    PostCreation(size);

    SetValue( value );

    return true;
}

void wxSpinCtrl::GtkDisableEvents()
{
    gtk_signal_disconnect_by_func( GTK_OBJECT(m_adjust),
                        GTK_SIGNAL_FUNC(gtk_spinctrl_callback),
                        (gpointer) this );

    gtk_signal_disconnect_by_func( GTK_OBJECT(m_widget),
                        GTK_SIGNAL_FUNC(gtk_spinctrl_text_changed_callback),
                        (gpointer) this );
}

void wxSpinCtrl::GtkEnableEvents()
{
    gtk_signal_connect( GTK_OBJECT (m_adjust),
                        "value_changed",
                        GTK_SIGNAL_FUNC(gtk_spinctrl_callback),
                        (gpointer) this );

    gtk_signal_connect( GTK_OBJECT(m_widget),
                        "changed",
                        GTK_SIGNAL_FUNC(gtk_spinctrl_text_changed_callback),
                        (gpointer)this);
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
        gtk_entry_set_text( GTK_ENTRY(m_widget), wxGTK_CONV( value ) );
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

void wxSpinCtrl::SetSelection(long from, long to)
{
    // translate from wxWidgets conventions to GTK+ ones: (-1, -1) means the
    // entire range
    if ( from == -1 && to == -1 )
    {
        from = 0;
        to = INT_MAX;
    }

    gtk_editable_select_region( GTK_EDITABLE(m_widget), (gint)from, (gint)to );
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

    if (event.GetKeyCode() == WXK_RETURN)
    {
        wxWindow *top_frame = m_parent;
        while (top_frame->GetParent() && !(top_frame->IsTopLevel()))
            top_frame = top_frame->GetParent();

        if ( GTK_IS_WINDOW(top_frame->m_widget) )
        {
            GtkWindow *window = GTK_WINDOW(top_frame->m_widget);
            if ( window )
            {
                GtkWidget *widgetDef = window->default_widget;

                if ( widgetDef )
                {
                    gtk_widget_activate(widgetDef);
                    return;
                }
            }
        }
    }

    if ((event.GetKeyCode() == WXK_RETURN) && (m_windowStyle & wxTE_PROCESS_ENTER))
    {
        wxCommandEvent evt( wxEVT_TEXT_ENTER, m_windowId );
        evt.SetEventObject(this);
        GtkSpinButton *gsb = GTK_SPIN_BUTTON(m_widget);
        wxString val = wxGTK_CONV_BACK( gtk_entry_get_text( &gsb->entry ) );
        evt.SetString( val );
        if (HandleWindowEvent(evt)) return;
    }

    event.Skip();
}

bool wxSpinCtrl::IsOwnGtkWindow( GdkWindow *window )
{
    if (GTK_SPIN_BUTTON(m_widget)->entry.text_area == window) return true;

    if (GTK_SPIN_BUTTON(m_widget)->panel == window) return true;

    return false;
}

wxSize wxSpinCtrl::DoGetBestSize() const
{
    return wxSize(95, wxControl::DoGetBestSize().y);
}

bool wxSpinCtrl::SetBase(int base)
{
    // Currently we only support base 10 and 16. We could add support for base
    // 8 quite easily but wxMSW doesn't support it natively so don't bother
    // with doing something wxGTK-specific here.
    if ( base != 10 && base != 16 )
        return false;

    if ( base == m_base )
        return true;

    m_base = base;

    // We need to be able to enter letters for any base greater than 10.
    gtk_spin_button_set_numeric( GTK_SPIN_BUTTON(m_widget), m_base <= 10 );

    if ( m_base != 10 )
    {
        gtk_signal_connect( GTK_OBJECT(m_widget), "input",
                              GTK_SIGNAL_FUNC(wx_gtk_spin_input), this);
        gtk_signal_connect( GTK_OBJECT(m_widget), "output",
                              GTK_SIGNAL_FUNC(wx_gtk_spin_output), this);
    }
    else
    {
        gtk_signal_disconnect_by_func(GTK_OBJECT(m_widget),
                                             GTK_SIGNAL_FUNC(wx_gtk_spin_input),
                                             this);
        gtk_signal_disconnect_by_func(GTK_OBJECT(m_widget),
                                             GTK_SIGNAL_FUNC(wx_gtk_spin_output),
                                             this);
    }

    return true;
}

// static
wxVisualAttributes
wxSpinCtrl::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    // TODO: overload to accept functions like gtk_spin_button_new?
    // Until then use a similar type
    return GetDefaultAttributesFromGTKWidget(gtk_entry_new, true);
}

#endif
   // wxUSE_SPINCTRL
