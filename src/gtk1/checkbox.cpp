/////////////////////////////////////////////////////////////////////////////
// Name:        checkbox.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "checkbox.h"
#endif

#include "wx/checkbox.h"

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool   g_blockEventsOnDrag;

//-----------------------------------------------------------------------------
// "clicked"
//-----------------------------------------------------------------------------

static void gtk_checkbox_clicked_callback( GtkWidget *WXUNUSED(widget), wxCheckBox *cb )
{
  if (!cb->HasVMT()) return;
  
  if (cb->m_blockFirstEvent)
  {
    cb->m_blockFirstEvent = FALSE;
    return;
  } 
  
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

wxCheckBox::wxCheckBox(void)
{
}

bool wxCheckBox::Create(  wxWindow *parent, wxWindowID id, const wxString &label,
      const wxPoint &pos, const wxSize &size, 
      long style, const wxValidator& validator, const wxString &name )
{
  m_needParent = TRUE;
  
  PreCreation( parent, id, pos, size, style, name );

  SetValidator( validator );

  m_widget = gtk_check_button_new_with_label( m_label );
 
  m_blockFirstEvent = FALSE;
  
  wxSize newSize = size;
  if (newSize.x == -1) newSize.x = 25+gdk_string_measure( m_widget->style->font, label );
  if (newSize.y == -1) newSize.y = 26;
  SetSize( newSize.x, newSize.y );
   
  gtk_signal_connect( GTK_OBJECT(m_widget), "clicked", 
    GTK_SIGNAL_FUNC(gtk_checkbox_clicked_callback), (gpointer*)this );
    
  m_parent->AddChild( this );

  (m_parent->m_insertCallback)( m_parent, this );
  
  PostCreation();
  
  gtk_widget_realize( GTK_BUTTON( m_widget )->child );
  
  SetLabel( label );

  SetBackgroundColour( parent->GetBackgroundColour() );
  SetForegroundColour( parent->GetForegroundColour() );

  Show( TRUE );
    
  return TRUE;
}

void wxCheckBox::SetValue( bool state )
{
  wxCHECK_RET( m_widget != NULL, "invalid checkbox" );

  if (state)
    gtk_toggle_button_set_state( GTK_TOGGLE_BUTTON(m_widget), GTK_STATE_ACTIVE );
  else
    gtk_toggle_button_set_state( GTK_TOGGLE_BUTTON(m_widget), GTK_STATE_NORMAL );
    
  m_blockFirstEvent = TRUE;
}

bool wxCheckBox::GetValue() const
{
  wxCHECK_MSG( m_widget != NULL, FALSE, "invalid checkbox" );

  return GTK_TOGGLE_BUTTON(m_widget)->active;
}

void wxCheckBox::SetLabel( const wxString& label )
{
  wxCHECK_RET( m_widget != NULL, "invalid checkbox" );

  wxControl::SetLabel( label );
  
  gtk_label_set( GTK_LABEL( GTK_BUTTON(m_widget)->child ), GetLabel() );
}

void wxCheckBox::Enable( bool enable )
{
  wxCHECK_RET( m_widget != NULL, "invalid checkbox" );

  wxControl::Enable( enable );
  
  gtk_widget_set_sensitive( GTK_BUTTON(m_widget)->child, enable );
}

void wxCheckBox::ApplyWidgetStyle()
{
  SetWidgetStyle();
  gtk_widget_set_style( m_widget, m_widgetStyle );
  gtk_widget_set_style( GTK_BUTTON(m_widget)->child, m_widgetStyle );
}


