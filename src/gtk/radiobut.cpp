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

#if wxUSE_RADIOBOX

#include <gdk/gdk.h>
#include <gtk/gtk.h>

//-----------------------------------------------------------------------------
// idle system
//-----------------------------------------------------------------------------

extern void wxapp_install_idle_handler();
extern bool g_isIdle;

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool       g_blockEventsOnDrag;
extern wxCursor   g_globalCursor;

//-----------------------------------------------------------------------------
// "clicked"
//-----------------------------------------------------------------------------

static 
void gtk_radiobutton_clicked_callback( GtkWidget *WXUNUSED(widget), wxRadioButton *rb )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (!rb->m_hasVMT) return;
  
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
    m_isRadioButton = TRUE;

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, validator, name ))
    {
        wxFAIL_MSG( wxT("wxRadioButton creation failed") );
	return FALSE;
    }

    if (HasFlag(wxRB_GROUP))
    {
        /* start a new group */
        m_radioButtonGroup = (GSList*) NULL;
    }
    else
    {
        /* search backward for last group start */
        wxRadioButton *chief = (wxRadioButton*) NULL;
        wxWindowList::Node *node = parent->GetChildren().GetLast();
        while (node)
	    {
	        wxWindow *child = node->GetData();
	        if (child->m_isRadioButton)
	        {
	            chief = (wxRadioButton*) child;
		        if (child->HasFlag(wxRB_GROUP)) break;
	        }
	        node = node->GetPrevious();
        }
	    if (chief)
	    {
            /* we are part of the group started by chief */
	        m_radioButtonGroup = gtk_radio_button_group( GTK_RADIO_BUTTON(chief->m_widget) );
	    }
	    else
	    {
            /* start a new group */
            m_radioButtonGroup = (GSList*) NULL;
	    }
    }

    m_widget = gtk_radio_button_new_with_label( m_radioButtonGroup, label.mbc_str() );
      
    SetLabel(label);

    gtk_signal_connect( GTK_OBJECT(m_widget), "clicked", 
      GTK_SIGNAL_FUNC(gtk_radiobutton_clicked_callback), (gpointer*)this );
       
    m_parent->DoAddChild( this );
  
    PostCreation();

    ApplyWidgetStyle();

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

void wxRadioButton::SetLabel( const wxString& label )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid radiobutton") );
  
    wxControl::SetLabel( label );
    GtkButton *bin = GTK_BUTTON( m_widget );
    GtkLabel *g_label = GTK_LABEL( bin->child );
    gtk_label_set( g_label, GetLabel().mbc_str() );
}

void wxRadioButton::SetValue( bool val )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid radiobutton") );
  
    if (val == GetValue())
        return;

    gtk_signal_disconnect_by_func( GTK_OBJECT(m_widget),
      GTK_SIGNAL_FUNC(gtk_radiobutton_clicked_callback), (gpointer*)this );

    if (val)
    {
        gtk_toggle_button_set_state( GTK_TOGGLE_BUTTON(m_widget), TRUE );
    }
    else
    {
        // should give an assert
    }
	
    gtk_signal_connect( GTK_OBJECT(m_widget), "clicked", 
      GTK_SIGNAL_FUNC(gtk_radiobutton_clicked_callback), (gpointer*)this );
}

bool wxRadioButton::GetValue() const
{
    wxCHECK_MSG( m_widget != NULL, FALSE, wxT("invalid radiobutton") );
  
    return GTK_TOGGLE_BUTTON(m_widget)->active;
}

bool wxRadioButton::Enable( bool enable )
{
    if ( !wxControl::Enable( enable ) )
        return FALSE;
  
    gtk_widget_set_sensitive( GTK_BUTTON(m_widget)->child, enable );

    return TRUE;
}

void wxRadioButton::ApplyWidgetStyle()
{
    SetWidgetStyle();
    gtk_widget_set_style( m_widget, m_widgetStyle );
    gtk_widget_set_style( GTK_BUTTON(m_widget)->child, m_widgetStyle );
}

bool wxRadioButton::IsOwnGtkWindow( GdkWindow *window )
{
    return (window == GTK_TOGGLE_BUTTON(m_widget)->event_window);
}

void wxRadioButton::OnInternalIdle()
{
    wxCursor cursor = m_cursor;
    if (g_globalCursor.Ok()) cursor = g_globalCursor;

    if (GTK_TOGGLE_BUTTON(m_widget)->event_window && cursor.Ok())
    {
        /* I now set the cursor the anew in every OnInternalIdle call
	   as setting the cursor in a parent window also effects the
	   windows above so that checking for the current cursor is
	   not possible. */
	   
	   gdk_window_set_cursor( GTK_TOGGLE_BUTTON(m_widget)->event_window, cursor.GetCursor() );
    }

    UpdateWindowUI();
}

wxSize wxRadioButton::DoGetBestSize() const
{
    return wxControl::DoGetBestSize();
}

#endif
