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

#include <gdk/gdk.h>
#include <gtk/gtk.h>

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxButton;

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

static void gtk_button_clicked_callback( GtkWidget *WXUNUSED(widget), wxButton *button )
{
    if (g_isIdle) 
       wxapp_install_idle_handler();

    if (!button->m_hasVMT) return;
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
}

bool wxButton::Create(  wxWindow *parent, wxWindowID id, const wxString &label,
      const wxPoint &pos, const wxSize &size,
      long style, const wxValidator& validator, const wxString &name )
{
    m_needParent = TRUE;
    m_acceptsFocus = TRUE;

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, validator, name ))
    {
        wxFAIL_MSG( wxT("wxButton creation failed") );
	    return FALSE;
    }

/*
    wxString label2( label );
    for (size_t i = 0; i < label2.Len(); i++)
    {
        if (label2.GetChar(i) == wxT('&'))
	    label2.SetChar(i,wxT('_'));
    }
    
    GtkWidget *accel_label = gtk_accel_label_new( label2.mb_str() );
    gtk_widget_show( accel_label );
    
    m_widget = gtk_button_new();
    gtk_container_add( GTK_CONTAINER(m_widget), accel_label );
    
    gtk_accel_label_set_accel_widget( GTK_ACCEL_LABEL(accel_label), m_widget );
    
    guint accel_key = gtk_label_parse_uline (GTK_LABEL(accel_label), label2.mb_str() );
    gtk_accel_label_refetch( GTK_ACCEL_LABEL(accel_label) );
    
    wxControl::SetLabel( label );
*/
    
    m_widget = gtk_button_new_with_label("");

    SetLabel( label );

#if (GTK_MINOR_VERSION > 0)    
    if (style & wxNO_BORDER)
       gtk_button_set_relief( GTK_BUTTON(m_widget), GTK_RELIEF_NONE );
#endif

    gtk_signal_connect( GTK_OBJECT(m_widget), "clicked",
      GTK_SIGNAL_FUNC(gtk_button_clicked_callback), (gpointer*)this );

    m_parent->DoAddChild( this );
  
    PostCreation();
  
    SetFont( parent->GetFont() );

    wxSize best_size( DoGetBestSize() );
    wxSize new_size( size );
    if (new_size.x == -1)
        new_size.x = best_size.x;
    if (new_size.y == -1)
        new_size.y = best_size.y;
    if ((new_size.x != size.x) || (new_size.y != size.y))
        SetSize( new_size.x, new_size.y );

    SetSize( new_size );

    SetBackgroundColour( parent->GetBackgroundColour() );
    SetForegroundColour( parent->GetForegroundColour() );

    Show( TRUE );

    return TRUE;
}

void wxButton::SetDefault()
{
    GTK_WIDGET_SET_FLAGS( m_widget, GTK_CAN_DEFAULT );
    gtk_widget_grab_default( m_widget );
    
    SetSize( m_x, m_y, m_width, m_height );
}

/* static */
wxSize wxButton::GetDefaultSize()
{
    return wxSize(80,26);
}

void wxButton::SetLabel( const wxString &label )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid button") );
  
    wxControl::SetLabel( label );
  
    gtk_label_set( GTK_LABEL( GTK_BUTTON(m_widget)->child ), GetLabel().mbc_str() );
}

bool wxButton::Enable( bool enable )
{
    if ( !wxControl::Enable( enable ) )
        return FALSE;
  
    gtk_widget_set_sensitive( GTK_BUTTON(m_widget)->child, enable );

    return TRUE;
}

void wxButton::ApplyWidgetStyle()
{
    SetWidgetStyle();
    gtk_widget_set_style( m_widget, m_widgetStyle );
    gtk_widget_set_style( GTK_BUTTON(m_widget)->child, m_widgetStyle );
}

wxSize wxButton::DoGetBestSize() const
{
    wxSize ret( wxControl::DoGetBestSize() );
    if (ret.x < 80) ret.x = 80;
    return ret;
}

