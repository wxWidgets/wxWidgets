/////////////////////////////////////////////////////////////////////////////
// Name:        bmpbuttn.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "bmpbuttn.h"
#endif

#include "wx/bmpbuttn.h"

#include "gdk/gdk.h"
#include "gtk/gtk.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxBitmapButton;

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

static void gtk_bmpbutton_clicked_callback( GtkWidget *WXUNUSED(widget), wxBitmapButton *button )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (!button->HasVMT()) return;
    if (g_blockEventsOnDrag) return;
  
    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, button->GetId());
    event.SetEventObject(button);
    button->GetEventHandler()->ProcessEvent(event);
}

//-----------------------------------------------------------------------------
// "enter"
//-----------------------------------------------------------------------------

static void gtk_bmpbutton_enter_callback( GtkWidget *WXUNUSED(widget), wxBitmapButton *button )
{
    if (!button->HasVMT()) return;
    if (g_blockEventsOnDrag) return;

    button->HasFocus(); 
}

//-----------------------------------------------------------------------------
// "leave"
//-----------------------------------------------------------------------------

static void gtk_bmpbutton_leave_callback( GtkWidget *WXUNUSED(widget), wxBitmapButton *button )
{
    if (!button->HasVMT()) return;
    if (g_blockEventsOnDrag) return;

    button->NotFocus(); 
}

//-----------------------------------------------------------------------------
// "pressed"
//-----------------------------------------------------------------------------

static void gtk_bmpbutton_press_callback( GtkWidget *WXUNUSED(widget), wxBitmapButton *button )
{
    if (!button->HasVMT()) return;
    if (g_blockEventsOnDrag) return;

    button->StartSelect(); 
}

//-----------------------------------------------------------------------------
// "released"
//-----------------------------------------------------------------------------

static void gtk_bmpbutton_release_callback( GtkWidget *WXUNUSED(widget), wxBitmapButton *button )
{
    if (!button->HasVMT()) return;
    if (g_blockEventsOnDrag) return;

    button->EndSelect(); 
}

//-----------------------------------------------------------------------------
// wxBitmapButton
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxBitmapButton,wxControl)

wxBitmapButton::wxBitmapButton()
{
}

bool wxBitmapButton::Create( wxWindow *parent, wxWindowID id, const wxBitmap &bitmap,
                             const wxPoint &pos, const wxSize &size, 
                             long style, const wxValidator& validator, const wxString &name )
{
    m_needParent = TRUE;
    m_acceptsFocus = TRUE;
  
    wxSize newSize = size;

    PreCreation( parent, id, pos, newSize, style, name );
  
    SetValidator( validator );

    m_bitmap   = bitmap;
    m_disabled = bitmap;
    m_focus    = bitmap;
    m_selected = bitmap;
  
    m_label = "";
  
    m_widget = gtk_button_new();
  
    if (m_bitmap.Ok())
    {
        GdkBitmap *mask = (GdkBitmap *) NULL;
        if (m_bitmap.GetMask()) mask = m_bitmap.GetMask()->GetBitmap();
        GtkWidget *pixmap = gtk_pixmap_new( m_bitmap.GetPixmap(), mask );
    
        gtk_widget_show( pixmap );
        gtk_container_add( GTK_CONTAINER(m_widget), pixmap );
    }
  
    if (newSize.x == -1) newSize.x = m_bitmap.GetHeight()+10;
    if (newSize.y == -1) newSize.y = m_bitmap.GetWidth()+10;
    SetSize( newSize.x, newSize.y );
  
    gtk_signal_connect( GTK_OBJECT(m_widget), "clicked", 
      GTK_SIGNAL_FUNC(gtk_bmpbutton_clicked_callback), (gpointer*)this );

    gtk_signal_connect( GTK_OBJECT(m_widget), "enter", 
      GTK_SIGNAL_FUNC(gtk_bmpbutton_enter_callback), (gpointer*)this );
    gtk_signal_connect( GTK_OBJECT(m_widget), "leave", 
      GTK_SIGNAL_FUNC(gtk_bmpbutton_leave_callback), (gpointer*)this );
    gtk_signal_connect( GTK_OBJECT(m_widget), "pressed", 
      GTK_SIGNAL_FUNC(gtk_bmpbutton_press_callback), (gpointer*)this );
    gtk_signal_connect( GTK_OBJECT(m_widget), "released", 
      GTK_SIGNAL_FUNC(gtk_bmpbutton_release_callback), (gpointer*)this );
 
    m_parent->AddChild( this );

    (m_parent->m_insertCallback)( m_parent, this );
  
    PostCreation();
  
    SetBackgroundColour( parent->GetBackgroundColour() );

    Show( TRUE );
    
    return TRUE;
}
      
void wxBitmapButton::SetDefault()
{
    GTK_WIDGET_SET_FLAGS( m_widget, GTK_CAN_DEFAULT );
    gtk_widget_grab_default( m_widget );
    
    SetSize( m_x, m_y, m_width, m_height );
}

void wxBitmapButton::SetLabel( const wxString &label )
{
    wxCHECK_RET( m_widget != NULL, _T("invalid button") );

    wxControl::SetLabel( label );
}

wxString wxBitmapButton::GetLabel() const
{
    wxCHECK_MSG( m_widget != NULL, _T(""), _T("invalid button") );

    return wxControl::GetLabel();
}

void wxBitmapButton::ApplyWidgetStyle()
{
}

void wxBitmapButton::SetBitmap()
{
    wxCHECK_RET( m_widget != NULL, _T("invalid button") );

    wxBitmap the_one;
  
    if ( ! m_isEnabled ) 
        the_one = m_disabled;
    else 
    {
        if ( m_isSelected ) 
	{
	    the_one = m_selected;
	}
        else 
	{
            if ( m_hasFocus ) 
	        the_one = m_focus;
            else 
	        the_one = m_bitmap;
        }
    }

    if ( ! the_one.Ok() ) the_one = m_bitmap;
    if ( ! the_one.Ok() ) return;
  
    GtkButton *bin = GTK_BUTTON( m_widget );
    GtkPixmap *g_pixmap = GTK_PIXMAP( bin->child );
  
    GdkBitmap *mask = (GdkBitmap *) NULL;
    if (the_one.GetMask()) mask = the_one.GetMask()->GetBitmap();
  
    gtk_pixmap_set( g_pixmap, the_one.GetPixmap(), mask );
}

void wxBitmapButton::SetBitmapDisabled( const wxBitmap& bitmap ) 
{
  wxCHECK_RET( m_widget != NULL, _T("invalid button") );

  if ( ! m_disabled.Ok() ) return;
  m_disabled = bitmap;

  SetBitmap();
}

void wxBitmapButton::SetBitmapFocus( const wxBitmap& bitmap ) 
{
    wxCHECK_RET( m_widget != NULL, _T("invalid button") );

    if ( ! m_focus.Ok() ) return;
    m_focus = bitmap;

    SetBitmap();
}

void wxBitmapButton::SetBitmapLabel( const wxBitmap& bitmap )
{
    wxCHECK_RET( m_widget != NULL, _T("invalid button") );

    if (!m_bitmap.Ok()) return;
    m_bitmap = bitmap;
  
    SetBitmap();
}

void wxBitmapButton::SetBitmapSelected( const wxBitmap& bitmap )
{
    wxCHECK_RET( m_widget != NULL, _T("invalid button") );

    if ( ! m_selected.Ok() ) return;
    m_selected = bitmap;

    SetBitmap();
}

void wxBitmapButton::Enable( const bool enable )
{
    wxWindow::Enable(enable);

    SetBitmap();  
}

void wxBitmapButton::HasFocus()
{
    m_hasFocus = TRUE;
    SetBitmap();
}

void wxBitmapButton::NotFocus()
{
    m_hasFocus = FALSE;
    SetBitmap();
}

void wxBitmapButton::StartSelect()
{
    m_isSelected = TRUE;
    SetBitmap();
}

void wxBitmapButton::EndSelect()
{
    m_isSelected = FALSE;
    SetBitmap();
}
