/////////////////////////////////////////////////////////////////////////////
// Name:        radiobut.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "radiobut.h"
#endif

#include "wx/radiobut.h"

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool g_blockEventsOnDrag;

//-----------------------------------------------------------------------------
// "clicked"
//-----------------------------------------------------------------------------

static 
void gtk_radiobutton_clicked_callback( GtkWidget *WXUNUSED(widget), wxRadioButton *rb )
{
    if (!rb->HasVMT()) return;
  
    if (rb->m_blockFirstEvent)
    {
        rb->m_blockFirstEvent = FALSE;
        return;
    } 
  
    if (g_blockEventsOnDrag) return;
  
    wxCommandEvent event( wxEVT_COMMAND_RADIOBUTTON_SELECTED, rb->GetId());
    event.SetInt( rb->GetValue() );
    event.SetEventObject( rb );
    rb->GetEventHandler()->ProcessEvent( event );
}

//-----------------------------------------------------------------------------
// wxRadioButton
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxRadioButton,wxControl)
  
bool wxRadioButton::Create( wxWindow *parent, wxWindowID id, const wxString& label,
  const wxPoint& pos,  const wxSize& size, long style,
  const wxValidator& validator, const wxString& name )
{
    m_acceptsFocus = TRUE;
    m_needParent = TRUE;

    wxSize newSize = size;

    PreCreation( parent, id, pos, newSize, style, name );
  
    SetValidator( validator );

    m_widget = gtk_radio_button_new_with_label( (GSList *) NULL, label );
      
    m_theOtherRadioButtton = 
       gtk_radio_button_new_with_label(
         gtk_radio_button_group( GTK_RADIO_BUTTON(m_widget) ),
         "button2" );
  
    SetLabel(label);

    m_blockFirstEvent = FALSE;
    
    if (newSize.x == -1) newSize.x = 22+gdk_string_measure( m_widget->style->font, label );
    if (newSize.y == -1) newSize.y = 26;
    SetSize( newSize.x, newSize.y );

    gtk_signal_connect( GTK_OBJECT(m_widget), "clicked", 
      GTK_SIGNAL_FUNC(gtk_radiobutton_clicked_callback), (gpointer*)this );
       
    m_parent->AddChild( this );

    (m_parent->m_insertCallback)( m_parent, this );
  
    PostCreation();

    SetBackgroundColour( parent->GetBackgroundColour() );
    SetForegroundColour( parent->GetForegroundColour() );
  
    Show( TRUE );

    return TRUE;
}

void wxRadioButton::SetLabel( const wxString& label )
{
    wxCHECK_RET( m_widget != NULL, "invalid radiobutton" );
  
    wxControl::SetLabel( label );
    GtkButton *bin = GTK_BUTTON( m_widget );
    GtkLabel *g_label = GTK_LABEL( bin->child );
    gtk_label_set( g_label, GetLabel() );
}

void wxRadioButton::SetValue( bool val )
{
    wxCHECK_RET( m_widget != NULL, "invalid radiobutton" );
  
    m_blockFirstEvent = TRUE;
  
    if (val)
        gtk_toggle_button_set_state( GTK_TOGGLE_BUTTON(m_widget), TRUE );
    else
        gtk_toggle_button_set_state( GTK_TOGGLE_BUTTON(m_theOtherRadioButtton), TRUE );
}

bool wxRadioButton::GetValue(void) const
{
    wxCHECK_MSG( m_widget != NULL, FALSE, "invalid radiobutton" );
  
    return GTK_TOGGLE_BUTTON(m_widget)->active;
}

void wxRadioButton::Enable( bool enable )
{
    wxCHECK_RET( m_widget != NULL, "invalid radiobutton" );
  
    wxControl::Enable( enable );
  
    gtk_widget_set_sensitive( GTK_BUTTON(m_widget)->child, enable );
}

void wxRadioButton::ApplyWidgetStyle()
{
    SetWidgetStyle();
    gtk_widget_set_style( m_widget, m_widgetStyle );
    gtk_widget_set_style( GTK_BUTTON(m_widget)->child, m_widgetStyle );
}


