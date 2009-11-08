/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/spinbutt.cpp
// Purpose:     wxSpinCtrl
// Author:      Robert
// Modified by:
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_SPINCTRL

#include "wx/spinctrl.h"

#ifndef WX_PRECOMP
    #include "wx/textctrl.h"    // for wxEVT_COMMAND_TEXT_UPDATED
    #include "wx/utils.h"
#endif

#include "wx/gtk/private.h"

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool   g_blockEventsOnDrag;

//-----------------------------------------------------------------------------
// "value_changed"
//-----------------------------------------------------------------------------

extern "C" {
static void
gtk_value_changed(GtkSpinButton* spinbutton, wxSpinCtrl* win)
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (!win->m_hasVMT || g_blockEventsOnDrag || win->m_blockScrollEvent)
        return;

    wxCommandEvent event( wxEVT_COMMAND_SPINCTRL_UPDATED, win->GetId());
    event.SetEventObject( win );
    event.SetInt(win->GetValue());
    win->GetEventHandler()->ProcessEvent( event );
}
}

//-----------------------------------------------------------------------------
//  "changed"
//-----------------------------------------------------------------------------

extern "C" {
static void
gtk_changed(GtkSpinButton* spinbutton, wxSpinCtrl* win)
{
    if (g_isIdle)
        wxapp_install_idle_handler();

    if (!win->m_hasVMT || win->m_blockScrollEvent)
        return;

    wxCommandEvent event( wxEVT_COMMAND_TEXT_UPDATED, win->GetId() );
    event.SetEventObject( win );
    event.SetInt(win->GetValue());
    win->GetEventHandler()->ProcessEvent( event );
}
}

//-----------------------------------------------------------------------------
// wxSpinCtrl
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxSpinCtrl,wxControl)

BEGIN_EVENT_TABLE(wxSpinCtrl, wxControl)
    EVT_CHAR(wxSpinCtrl::OnChar)
END_EVENT_TABLE()

wxSpinCtrl::wxSpinCtrl()
{
}

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

    m_widget = gtk_spin_button_new_with_range(min, max, 1);
    gtk_spin_button_set_value( GTK_SPIN_BUTTON(m_widget), initial);

    gtk_spin_button_set_wrap( GTK_SPIN_BUTTON(m_widget),
                              (int)(m_windowStyle & wxSP_WRAP) );

    g_signal_connect_after(m_widget, "value_changed", G_CALLBACK(gtk_value_changed), this);
    g_signal_connect_after(m_widget, "changed", G_CALLBACK(gtk_changed), this);

    m_parent->DoAddChild( this );

    PostCreation(size);

    if (!value.empty())
    {
        SetValue(value);
    }

    return true;
}

int wxSpinCtrl::GetMin() const
{
    wxCHECK_MSG( (m_widget != NULL), 0, wxT("invalid spin button") );

    double min;
    gtk_spin_button_get_range( GTK_SPIN_BUTTON(m_widget), &min, NULL);
    return int(min);
}

int wxSpinCtrl::GetMax() const
{
    wxCHECK_MSG( (m_widget != NULL), 0, wxT("invalid spin button") );

    double max;
    gtk_spin_button_get_range( GTK_SPIN_BUTTON(m_widget), NULL, &max);
    return int(max);
}

static void wxSpinCtrl_GtkDisableEvents( const wxSpinCtrl *spinctrl )
{
    g_signal_handlers_block_by_func( spinctrl->m_widget,
        (gpointer)gtk_value_changed, (void*) spinctrl );
        
    g_signal_handlers_block_by_func( spinctrl->m_widget,
        (gpointer)gtk_changed, (void*) spinctrl );
}

static void wxSpinCtrl_GtkEnableEvents( const wxSpinCtrl *spinctrl )
{
    g_signal_handlers_unblock_by_func( spinctrl->m_widget,
        (gpointer)gtk_value_changed, (void*) spinctrl );
        
    g_signal_handlers_unblock_by_func( spinctrl->m_widget,
        (gpointer)gtk_changed, (void*) spinctrl );
}

int wxSpinCtrl::GetValue() const
{
    wxCHECK_MSG( (m_widget != NULL), 0, wxT("invalid spin button") );

    // Get value directly from current control text, just as
    // gtk_spin_button_update() would do. Calling gtk_spin_button_update() causes
    // a redraw, which causes an idle event, so if GetValue() is called from
    // a UI update handler, you get a never ending sequence of idle events. It
    // also forces the text into valid range, which wxMSW GetValue() does not do.
    static unsigned sig_id;
    if (sig_id == 0)
        sig_id = g_signal_lookup("input", GTK_TYPE_SPIN_BUTTON);
    double value;
    int handled = 0;
    g_signal_emit(m_widget, sig_id, 0, &value, &handled);
    if (!handled)
        value = g_strtod(gtk_entry_get_text(GTK_ENTRY(m_widget)), NULL);
    const GtkAdjustment* adj =
        gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(m_widget));
    if (value < adj->lower)
        value = adj->lower;
    else if (value > adj->upper)
        value = adj->upper;

    return value;
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
        wxSpinCtrl_GtkDisableEvents( this );
        gtk_entry_set_text( GTK_ENTRY(m_widget), wxGTK_CONV( value ) );
        wxSpinCtrl_GtkEnableEvents( this );
    }
}

void wxSpinCtrl::SetValue( int value )
{
    wxCHECK_RET( (m_widget != NULL), wxT("invalid spin button") );

    wxSpinCtrl_GtkDisableEvents( this );
    gtk_spin_button_set_value( GTK_SPIN_BUTTON(m_widget), value);
    wxSpinCtrl_GtkEnableEvents( this );
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

    wxSpinCtrl_GtkDisableEvents( this );
    gtk_spin_button_set_range( GTK_SPIN_BUTTON(m_widget), minVal, maxVal);
    wxSpinCtrl_GtkEnableEvents( this );
}

void wxSpinCtrl::OnChar( wxKeyEvent &event )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid spin ctrl") );

    if (event.GetKeyCode() == WXK_RETURN)
    {
        wxWindow *top_frame = wxGetTopLevelParent(m_parent);

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
        wxCommandEvent evt( wxEVT_COMMAND_TEXT_ENTER, m_windowId );
        evt.SetEventObject(this);
        GtkSpinButton *gsb = GTK_SPIN_BUTTON(m_widget);
        wxString val = wxGTK_CONV_BACK( gtk_entry_get_text( &gsb->entry ) );
        evt.SetString( val );
        if (GetEventHandler()->ProcessEvent(evt)) return;
    }

    event.Skip();
}

GdkWindow *wxSpinCtrl::GTKGetWindow(wxArrayGdkWindows& windows) const
{
    GtkSpinButton* spinbutton = GTK_SPIN_BUTTON(m_widget);

    windows.push_back(spinbutton->entry.text_area);
    windows.push_back(spinbutton->panel);

    return NULL;
}

wxSize wxSpinCtrl::DoGetBestSize() const
{
    wxSize ret( wxControl::DoGetBestSize() );
    wxSize best(95, ret.y); // FIXME: 95?
    CacheBestSize(best);
    return best;
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
