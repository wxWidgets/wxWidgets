/////////////////////////////////////////////////////////////////////////////
// Name:        button.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "button.h"
#endif

#include "wx/button.h"

#include "gdk/gdk.h"
#include "gtk/gtk.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxButton;

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool   g_blockEventsOnDrag;

//-----------------------------------------------------------------------------
// "clicked"
//-----------------------------------------------------------------------------

static void gtk_button_clicked_callback( GtkWidget *WXUNUSED(widget), wxButton *button )
{
  if (!button->HasVMT()) return;
  if (g_blockEventsOnDrag) return;
  
  wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, button->GetId());
  event.SetEventObject(button);
  button->GetEventHandler()->ProcessEvent(event);
}

//-----------------------------------------------------------------------------
// wxButton
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxButton,wxControl)

wxButton::wxButton()
{
}

wxButton::~wxButton()
{
  if (m_clientData) delete m_clientData;
}

bool wxButton::Create(  wxWindow *parent, wxWindowID id, const wxString &label,
      const wxPoint &pos, const wxSize &size,
      long style, const wxValidator& validator, const wxString &name )
{
    m_clientData = (wxClientData*) NULL;
    m_needParent = TRUE;
    m_acceptsFocus = TRUE;

    wxSize newSize = size;

    PreCreation( parent, id, pos, newSize, style, name );
  
    SetValidator( validator );

    m_widget = gtk_button_new_with_label( m_label.mbc_str() );
    SetLabel(label);

    if (newSize.x == -1) newSize.x = 15+gdk_string_measure( m_widget->style->font, label.mbc_str() );
    if (newSize.y == -1) newSize.y = 26;
    SetSize( newSize.x, newSize.y );

    gtk_signal_connect( GTK_OBJECT(m_widget), "clicked",
      GTK_SIGNAL_FUNC(gtk_button_clicked_callback), (gpointer*)this );

    m_parent->AddChild( this );

    (m_parent->m_insertCallback)( m_parent, this );
  
    PostCreation();
  
    SetBackgroundColour( parent->GetBackgroundColour() );
    SetForegroundColour( parent->GetForegroundColour() );
    SetFont( parent->GetFont() );

    Show( TRUE );

    return TRUE;
}

void wxButton::SetDefault(void)
{
    GTK_WIDGET_SET_FLAGS( m_widget, GTK_CAN_DEFAULT );
    gtk_widget_grab_default( m_widget );
    
    SetSize( m_x, m_y, m_width, m_height );
}

void wxButton::SetLabel( const wxString &label )
{
    wxCHECK_RET( m_widget != NULL, _T("invalid button") );
  
    wxControl::SetLabel( label );
  
    gtk_label_set( GTK_LABEL( GTK_BUTTON(m_widget)->child ), GetLabel().mbc_str() );
}

void wxButton::Enable( bool enable )
{
    wxCHECK_RET( m_widget != NULL, _T("invalid button") );
  
    wxControl::Enable( enable );
  
    gtk_widget_set_sensitive( GTK_BUTTON(m_widget)->child, enable );
}

void wxButton::ApplyWidgetStyle()
{
    SetWidgetStyle();
    gtk_widget_set_style( m_widget, m_widgetStyle );
    gtk_widget_set_style( GTK_BUTTON(m_widget)->child, m_widgetStyle );
}
