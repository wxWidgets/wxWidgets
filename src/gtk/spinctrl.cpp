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

    win->m_pos = int(gtk_spin_button_get_value(spinbutton));
    if (!win->m_hasVMT || g_blockEventsOnDrag || win->m_blockScrollEvent)
        return;

    wxCommandEvent event( wxEVT_COMMAND_SPINCTRL_UPDATED, win->GetId());
    event.SetEventObject( win );

    // note that we don't use wxSpinCtrl::GetValue() here because it would
    // adjust the value to fit into the control range and this means that we
    // would never be able to enter an "invalid" value in the control, even
    // temporarily - and trying to enter 10 into the control which accepts the
    // values in range 5..50 is then, ummm, quite challenging (hint: you can't
    // enter 1!) (VZ)
    event.SetInt(win->m_pos);
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
    event.SetString( GTK_ENTRY(spinbutton)->text );

    // see above
    event.SetInt(win->m_pos);
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
    m_pos = 0;
}

bool wxSpinCtrl::Create(wxWindow *parent, wxWindowID id,
                        const wxString& value,
                        const wxPoint& pos,  const wxSize& size,
                        long style,
                        int min, int max, int initial,
                        const wxString& name)
{
    m_needParent = true;

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
    {
        wxFAIL_MSG( wxT("wxSpinCtrl creation failed") );
        return false;
    }

    m_widget = gtk_spin_button_new_with_range(min, max, 1);
    gtk_spin_button_set_value((GtkSpinButton*)m_widget, initial);
    m_pos = int(gtk_spin_button_get_value((GtkSpinButton*)m_widget));

    gtk_spin_button_set_wrap( GTK_SPIN_BUTTON(m_widget),
                              (int)(m_windowStyle & wxSP_WRAP) );

    g_signal_connect(m_widget, "value_changed", G_CALLBACK(gtk_value_changed), this);
    g_signal_connect(m_widget, "changed", G_CALLBACK(gtk_changed), this);

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
    gtk_spin_button_get_range((GtkSpinButton*)m_widget, &min, NULL);
    return int(min);
}

int wxSpinCtrl::GetMax() const
{
    wxCHECK_MSG( (m_widget != NULL), 0, wxT("invalid spin button") );

    double max;
    gtk_spin_button_get_range((GtkSpinButton*)m_widget, NULL, &max);
    return int(max);
}

int wxSpinCtrl::GetValue() const
{
    wxCHECK_MSG( (m_widget != NULL), 0, wxT("invalid spin button") );

    wx_const_cast(wxSpinCtrl*, this)->BlockScrollEvent();
    gtk_spin_button_update( GTK_SPIN_BUTTON(m_widget) );
    wx_const_cast(wxSpinCtrl*, this)->UnblockScrollEvent();

    return m_pos;
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
        BlockScrollEvent();
        gtk_entry_set_text( GTK_ENTRY(m_widget), wxGTK_CONV( value ) );
        UnblockScrollEvent();
    }
}

void wxSpinCtrl::SetValue( int value )
{
    wxCHECK_RET( (m_widget != NULL), wxT("invalid spin button") );

    BlockScrollEvent();
    gtk_spin_button_set_value((GtkSpinButton*)m_widget, value);
    UnblockScrollEvent();
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

    BlockScrollEvent();
    gtk_spin_button_set_range((GtkSpinButton*)m_widget, minVal, maxVal);
    UnblockScrollEvent();
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
