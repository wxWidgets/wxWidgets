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

#include "wx/defs.h"

#if wxUSE_CHECKBOX

#include "wx/checkbox.h"

#include "wx/gtk/private.h"

//-----------------------------------------------------------------------------
// idle system
//-----------------------------------------------------------------------------

extern void wxapp_install_idle_handler();
extern bool g_isIdle;

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool           g_blockEventsOnDrag;
extern wxCursor       g_globalCursor;
extern wxWindowGTK   *g_delayedFocus;

//-----------------------------------------------------------------------------
// "clicked"
//-----------------------------------------------------------------------------

static void gtk_checkbox_clicked_callback( GtkWidget *WXUNUSED(widget), wxCheckBox *cb )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (!cb->m_hasVMT) return;

    if (g_blockEventsOnDrag) return;
    
    if (cb->m_blockEvent) return;

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
    m_blockEvent = FALSE;

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, validator, name ))
    {
        wxFAIL_MSG( wxT("wxCheckBox creation failed") );
        return FALSE;
    }

    if ( style & wxALIGN_RIGHT )
    {
        // VZ: as I don't know a way to create a right aligned checkbox with
        //     GTK we will create a checkbox without label and a label at the
        //     left of it
        m_widgetCheckbox = gtk_check_button_new();

        m_widgetLabel = gtk_label_new("");
        gtk_misc_set_alignment(GTK_MISC(m_widgetLabel), 0.0, 0.5);

        m_widget = gtk_hbox_new(FALSE, 0);
        gtk_box_pack_start(GTK_BOX(m_widget), m_widgetLabel, FALSE, FALSE, 3);
        gtk_box_pack_start(GTK_BOX(m_widget), m_widgetCheckbox, FALSE, FALSE, 3);

        gtk_widget_show( m_widgetLabel );
        gtk_widget_show( m_widgetCheckbox );
    }
    else
    {
        m_widgetCheckbox = gtk_check_button_new_with_label("");
        m_widgetLabel = BUTTON_CHILD( m_widgetCheckbox );
        m_widget = m_widgetCheckbox;
    }
    SetLabel( label );

    gtk_signal_connect( GTK_OBJECT(m_widgetCheckbox),
                        "clicked",
                        GTK_SIGNAL_FUNC(gtk_checkbox_clicked_callback),
                        (gpointer *)this );

    m_parent->DoAddChild( this );

    PostCreation();

    SetFont( parent->GetFont() );

    wxSize size_best( DoGetBestSize() );
    wxSize new_size( size );
    if (new_size.x == -1)
        new_size.x = size_best.x;
    if (new_size.y == -1)
        new_size.y = size_best.y;
    if ((new_size.x != size.x) || (new_size.y != size.y))
        SetSize( new_size.x, new_size.y );

    SetBackgroundColour( parent->GetBackgroundColour() );
    SetForegroundColour( parent->GetForegroundColour() );

    Show( TRUE );

    return TRUE;
}

void wxCheckBox::SetValue( bool state )
{
    wxCHECK_RET( m_widgetCheckbox != NULL, wxT("invalid checkbox") );

    if (state == GetValue())
        return;

    m_blockEvent = TRUE;

    gtk_toggle_button_set_state( GTK_TOGGLE_BUTTON(m_widgetCheckbox), state );

    m_blockEvent = FALSE;
}

bool wxCheckBox::GetValue() const
{
    wxCHECK_MSG( m_widgetCheckbox != NULL, FALSE, wxT("invalid checkbox") );

    return GTK_TOGGLE_BUTTON(m_widgetCheckbox)->active;
}

void wxCheckBox::SetLabel( const wxString& label )
{
    wxCHECK_RET( m_widgetLabel != NULL, wxT("invalid checkbox") );

    wxControl::SetLabel( label );

#ifdef __WXGTK20__
    wxString label2 = PrepareLabelMnemonics( label );
    gtk_label_set_text_with_mnemonic( GTK_LABEL(m_widgetLabel), wxGTK_CONV( label2 ) );
#else
    gtk_label_set( GTK_LABEL(m_widgetLabel), wxGTK_CONV( GetLabel() ) );
#endif
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

bool wxCheckBox::IsOwnGtkWindow( GdkWindow *window )
{
    return window == TOGGLE_BUTTON_EVENT_WIN(m_widget);
}

void wxCheckBox::OnInternalIdle()
{
    wxCursor cursor = m_cursor;
    if (g_globalCursor.Ok()) cursor = g_globalCursor;

    GdkWindow *event_window = TOGGLE_BUTTON_EVENT_WIN(m_widgetCheckbox);
    if ( event_window && cursor.Ok() )
    {
        /* I now set the cursor the anew in every OnInternalIdle call
           as setting the cursor in a parent window also effects the
           windows above so that checking for the current cursor is
           not possible. */

       gdk_window_set_cursor( event_window, cursor.GetCursor() );
    }

    if (g_delayedFocus == this)
    {
        if (GTK_WIDGET_REALIZED(m_widget))
        {
            gtk_widget_grab_focus( m_widget );
            g_delayedFocus = NULL;
        }
    }
    
    if (wxUpdateUIEvent::CanUpdate())
        UpdateWindowUI();
}

wxSize wxCheckBox::DoGetBestSize() const
{
    return wxControl::DoGetBestSize();
}

#endif
