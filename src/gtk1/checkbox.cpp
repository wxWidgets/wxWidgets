/////////////////////////////////////////////////////////////////////////////
// Name:        checkbox.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "checkbox.h"
#endif

#include "wx/checkbox.h"

#if wxUSE_CHECKBOX

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

//-----------------------------------------------------------------------------
// "clicked"
//-----------------------------------------------------------------------------

static void gtk_checkbox_clicked_callback( GtkWidget *WXUNUSED(widget), wxCheckBox *cb )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (!cb->m_hasVMT) return;

    if (g_blockEventsOnDrag) return;

    wxCommandEvent event(wxEVT_COMMAND_CHECKBOX_CLICKED, cb->GetId());
    event.SetInt( cb->GetValue() );
    event.SetEventObject(cb);
    cb->GetEventHandler()->ProcessEvent(event);
}

//-----------------------------------------------------------------------------
// wxCheckBox
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxCheckBox,wxControl)

wxCheckBox::wxCheckBox()
{
}

bool wxCheckBox::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxString &label,
                        const wxPoint &pos,
                        const wxSize &size,
                        long style,
                        const wxValidator& validator,
                        const wxString &name )
{
    m_needParent = TRUE;
    m_acceptsFocus = TRUE;

    PreCreation( parent, id, pos, size, style, name );

#if wxUSE_VALIDATORS
    SetValidator( validator );
#endif

    wxControl::SetLabel( label );

    if ( style & wxALIGN_RIGHT )
    {
        // VZ: as I don't know a way to create a right aligned checkbox with
        //     GTK we will create a checkbox without label and a label at the
        //     left of it
        m_widgetCheckbox = gtk_check_button_new();

        m_widgetLabel = gtk_label_new(m_label.mbc_str());
        gtk_misc_set_alignment(GTK_MISC(m_widgetLabel), 0.0, 0.5);

        m_widget = gtk_hbox_new(FALSE, 0);
        gtk_box_pack_start(GTK_BOX(m_widget), m_widgetLabel, FALSE, FALSE, 3);
        gtk_box_pack_start(GTK_BOX(m_widget), m_widgetCheckbox, FALSE, FALSE, 3);

        gtk_widget_show( m_widgetLabel );
        gtk_widget_show( m_widgetCheckbox );
    }
    else
    {
        m_widgetCheckbox = gtk_check_button_new_with_label( m_label.mbc_str() );
        m_widgetLabel = GTK_BUTTON( m_widgetCheckbox )->child;
        m_widget = m_widgetCheckbox;
    }

    wxSize newSize(size);
    if (newSize.x == -1)
    {
        newSize.x = 25 + gdk_string_measure( m_widgetCheckbox->style->font,
                                             m_label.mbc_str() );
    }
    if (newSize.y == -1)
        newSize.y = 26;

    SetSize( newSize.x, newSize.y );

    gtk_signal_connect( GTK_OBJECT(m_widgetCheckbox),
                        "clicked",
                        GTK_SIGNAL_FUNC(gtk_checkbox_clicked_callback),
                        (gpointer *)this );

    m_parent->DoAddChild( this );

    PostCreation();

    SetBackgroundColour( parent->GetBackgroundColour() );
    SetForegroundColour( parent->GetForegroundColour() );
    SetFont( parent->GetFont() );

    Show( TRUE );

    return TRUE;
}

void wxCheckBox::SetValue( bool state )
{
    wxCHECK_RET( m_widgetCheckbox != NULL, _T("invalid checkbox") );

    if (state == GetValue())
        return;

    gtk_signal_disconnect_by_func( GTK_OBJECT(m_widgetCheckbox),
                        GTK_SIGNAL_FUNC(gtk_checkbox_clicked_callback),
                        (gpointer *)this );

    gtk_toggle_button_set_state( GTK_TOGGLE_BUTTON(m_widgetCheckbox), state );
    
    gtk_signal_connect( GTK_OBJECT(m_widgetCheckbox),
                        "clicked",
                        GTK_SIGNAL_FUNC(gtk_checkbox_clicked_callback),
                        (gpointer *)this );
}

bool wxCheckBox::GetValue() const
{
    wxCHECK_MSG( m_widgetCheckbox != NULL, FALSE, _T("invalid checkbox") );

    return GTK_TOGGLE_BUTTON(m_widgetCheckbox)->active;
}

void wxCheckBox::SetLabel( const wxString& label )
{
    wxCHECK_RET( m_widgetLabel != NULL, _T("invalid checkbox") );

    wxControl::SetLabel( label );

    gtk_label_set( GTK_LABEL(m_widgetLabel), GetLabel().mbc_str() );
}

bool wxCheckBox::Enable( bool enable )
{
    if ( !wxControl::Enable( enable ) )
        return FALSE;

    gtk_widget_set_sensitive( m_widgetLabel, enable );

    return TRUE;
}

void wxCheckBox::ApplyWidgetStyle()
{
    SetWidgetStyle();
    gtk_widget_set_style( m_widgetCheckbox, m_widgetStyle );
    gtk_widget_set_style( m_widgetLabel, m_widgetStyle );
}

#endif
