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

extern bool g_blockEventsOnDrag;

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

    wxSize newSize = size;

    PreCreation( parent, id, pos, newSize, style, name );
    
    m_isRadioButton = TRUE;
  
#if wxUSE_VALIDATORS
    SetValidator( validator );
#endif


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
	    node = node->GetPrevious();
        }
    }

    m_widget = gtk_radio_button_new_with_label( m_radioButtonGroup, label.mbc_str() );
      
    SetLabel(label);

    if (newSize.x == -1) newSize.x = 22+gdk_string_measure( m_widget->style->font, label.mbc_str() );
    if (newSize.y == -1) newSize.y = 26;
    SetSize( newSize.x, newSize.y );

    gtk_signal_connect( GTK_OBJECT(m_widget), "clicked", 
      GTK_SIGNAL_FUNC(gtk_radiobutton_clicked_callback), (gpointer*)this );
       
    m_parent->DoAddChild( this );
  
    PostCreation();

    SetBackgroundColour( parent->GetBackgroundColour() );
    SetForegroundColour( parent->GetForegroundColour() );
    SetFont( parent->GetFont() );
  
    Show( TRUE );

    return TRUE;
}

void wxRadioButton::SetLabel( const wxString& label )
{
    wxCHECK_RET( m_widget != NULL, _T("invalid radiobutton") );
  
    wxControl::SetLabel( label );
    GtkButton *bin = GTK_BUTTON( m_widget );
    GtkLabel *g_label = GTK_LABEL( bin->child );
    gtk_label_set( g_label, GetLabel().mbc_str() );
}

void wxRadioButton::SetValue( bool val )
{
    wxCHECK_RET( m_widget != NULL, _T("invalid radiobutton") );
  
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
    wxCHECK_MSG( m_widget != NULL, FALSE, _T("invalid radiobutton") );
  
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

#endif
