/////////////////////////////////////////////////////////////////////////////
// Name:        radiobox.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "radiobox.h"
#endif

#include "wx/radiobox.h"
#include "wx/dialog.h"
#include "wx/frame.h"
#include "wx/gtk/win_gtk.h"

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool   g_blockEventsOnDrag;

//-----------------------------------------------------------------------------
// wxRadioBox
//-----------------------------------------------------------------------------

static void gtk_radiobutton_clicked_callback( GtkWidget *WXUNUSED(widget), wxRadioBox *rb )
{
  if (!rb->HasVMT()) return;
  if (g_blockEventsOnDrag) return;
  
  if (rb->m_alreadySent)
  {
    rb->m_alreadySent = FALSE;
    return;
  }

  rb->m_alreadySent = TRUE;
  
  wxCommandEvent event( wxEVT_COMMAND_RADIOBOX_SELECTED, rb->GetId() );
  event.SetInt( rb->GetSelection() );
  wxString tmp( rb->GetStringSelection() );
  event.SetString( WXSTRINGCAST(tmp) );
  event.SetEventObject( rb );
  rb->GetEventHandler()->ProcessEvent(event);
}

//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxRadioBox,wxControl)

BEGIN_EVENT_TABLE(wxRadioBox, wxControl)
  EVT_SIZE(wxRadioBox::OnSize)
END_EVENT_TABLE()

wxRadioBox::wxRadioBox(void)
{
}

bool wxRadioBox::Create( wxWindow *parent, wxWindowID id, const wxString& title,
      const wxPoint &pos, const wxSize &size,
      int n, const wxString choices[], int WXUNUSED(majorDim), 
      long style, const wxValidator& validator, const wxString &name )
{
  m_alreadySent = FALSE;
  m_needParent = TRUE;
  
  PreCreation( parent, id, pos, size, style, name );

  SetValidator( validator );

  m_widget = gtk_frame_new( title );
  
  int x = m_x+5;
  int y = m_y+15;
  int maxLen = 0;
  int height = 20;
  
//  if (((m_style & wxRA_VERTICAL) == wxRA_VERTICAL) && (n > 0))
  if (n > 0)
  {
    GSList *radio_button_group = (GSList *) NULL;
    for (int i = 0; i < n; i++)
    {
      if (i) radio_button_group = gtk_radio_button_group( GTK_RADIO_BUTTON(m_radio) );
      
      m_radio = GTK_RADIO_BUTTON( gtk_radio_button_new_with_label( radio_button_group, choices[i] ) );
      
      if (!i) gtk_toggle_button_set_state( GTK_TOGGLE_BUTTON(m_radio), TRUE );
      
      gtk_signal_connect( GTK_OBJECT(m_radio), "clicked", 
        GTK_SIGNAL_FUNC(gtk_radiobutton_clicked_callback), (gpointer*)this );
       
      gtk_myfixed_put( GTK_MYFIXED(m_parent->m_wxwindow), GTK_WIDGET(m_radio), x, y );
      
      int tmp = 22+gdk_string_measure( GTK_WIDGET(m_radio)->style->font, choices[i] );
      if (tmp > maxLen) maxLen = tmp;
      
      int width = m_width-10;
      if (size.x == -1) width = tmp;
      gtk_widget_set_usize( GTK_WIDGET(m_radio), width, 20 );
      
      y += 20;
      height += 20;
      
    }
  }

  wxSize newSize = size;
  if (newSize.x == -1) newSize.x = maxLen+10;
  if (newSize.y == -1) newSize.y = height;
  SetSize( newSize.x, newSize.y );
  
  PostCreation();
  
  Show( TRUE );
    
  return TRUE;
}

void wxRadioBox::OnSize( wxSizeEvent &WXUNUSED(event) )
{
  int x = m_x+5;
  int y = m_y+15;
  
  GSList *item = gtk_radio_button_group( m_radio );
  while (item)
  {
    GtkWidget *button = GTK_WIDGET( item->data );
    
    gtk_myfixed_move( GTK_MYFIXED(m_parent->m_wxwindow), button, x, y );
    
    y += 20;
    
    item = item->next;
  }
}

bool wxRadioBox::Show( bool show )
{
  wxWindow::Show( show );

  GSList *item = gtk_radio_button_group( m_radio );
  while (item)
  {
    GtkWidget *w = GTK_WIDGET( item->data );
    if (show) gtk_widget_show( w ); else gtk_widget_hide( w );
    item = item->next;
  }

  return TRUE;
}

int wxRadioBox::FindString( const wxString &s ) const
{
  GSList *item = gtk_radio_button_group( m_radio );
  
  int count = g_slist_length(item)-1;
  
  while (item)
  {
    GtkButton *b = GTK_BUTTON( item->data );
    GtkLabel *l = GTK_LABEL( b->child );
    if (s == l->label) return count;
    count--;
    item = item->next;
  }

  return -1;
}

void wxRadioBox::SetSelection( int n )
{
  GSList *item = gtk_radio_button_group( m_radio );
  item = g_slist_nth( item, g_slist_length(item)-n-1 );
  if (!item) return;
  
  GtkToggleButton *button = GTK_TOGGLE_BUTTON( item->data );
  
  gtk_toggle_button_set_state( button, 1 );
}

int wxRadioBox::GetSelection(void) const
{
  GSList *item = gtk_radio_button_group( m_radio );
  int count = 0;
  int found = -1;
  while (item)
  {
    GtkButton *button = GTK_BUTTON( item->data );
    if (GTK_TOGGLE_BUTTON(button)->active) found = count;
    count++;
    item = item->next;
  }
  
  return found != -1 ? count-found-1 : -1;
}

wxString wxRadioBox::GetString( int n ) const
{
  GSList *item = gtk_radio_button_group( m_radio );
  
  item = g_slist_nth( item, g_slist_length(item)-n-1 );
  if (!item) return "";
  
  GtkToggleButton *button = GTK_TOGGLE_BUTTON( item->data );
  
  GtkLabel *label = GTK_LABEL( GTK_BUTTON(button)->child );
  
  return wxString( label->label );
}

wxString wxRadioBox::GetLabel(void) const
{
  return wxControl::GetLabel();
}

void wxRadioBox::SetLabel( const wxString& WXUNUSED(label) )
{
  wxFAIL_MSG("wxRadioBox::SetLabel not implemented.");
}

void wxRadioBox::SetLabel( int WXUNUSED(item), const wxString& WXUNUSED(label) )
{
  wxFAIL_MSG("wxRadioBox::SetLabel not implemented.");
}

void wxRadioBox::SetLabel( int WXUNUSED(item), wxBitmap *WXUNUSED(bitmap) )
{
  wxFAIL_MSG("wxRadioBox::SetLabel not implemented.");
}

wxString wxRadioBox::GetLabel( int WXUNUSED(item) ) const
{
  wxFAIL_MSG("wxRadioBox::GetLabel not implemented.");
  return "";
}

void wxRadioBox::Enable( bool WXUNUSED(enable) )
{
  wxFAIL_MSG("wxRadioBox::Enable not implemented.");
}

void wxRadioBox::Enable( int WXUNUSED(item), bool WXUNUSED(enable) )
{
  wxFAIL_MSG("wxRadioBox::Enable not implemented.");
}

void wxRadioBox::Show( int WXUNUSED(item), bool WXUNUSED(show) )
{
  wxFAIL_MSG("wxRadioBox::Show not implemented.");
}

wxString wxRadioBox::GetStringSelection(void) const
{
  GSList *item = gtk_radio_button_group( m_radio );
  while (item)
  {
    GtkButton *button = GTK_BUTTON( item->data );
    if (GTK_TOGGLE_BUTTON(button)->active)
    {
      GtkLabel *label = GTK_LABEL( button->child );
      return label->label;
    }
    item = item->next;
  }
  return "";
}

bool wxRadioBox::SetStringSelection( const wxString&s )
{
  int res = FindString( s );
  if (res == -1) return FALSE;
  SetSelection( res );
  return TRUE;
}

int wxRadioBox::Number(void) const
{
  int count = 0;
  GSList *item = gtk_radio_button_group( m_radio );
  while (item)
  {
    item = item->next;
    count++;
  }
  return count;
}

int wxRadioBox::GetNumberOfRowsOrCols(void) const
{
  return 1;
}

void wxRadioBox::SetNumberOfRowsOrCols( int WXUNUSED(n) )
{
  wxFAIL_MSG("wxRadioBox::SetNumberOfRowsOrCols not implemented.");
}

void wxRadioBox::SetFont( const wxFont &font )
{
  wxWindow::SetFont( font );
   
  GSList *item = gtk_radio_button_group( m_radio );
  while (item)
  {
    GtkButton *button = GTK_BUTTON( item->data );
    
    gtk_widget_set_style( button->child, 
      gtk_style_ref(
        gtk_widget_get_style( m_widget ) ) ); 
    
    item = item->next;
  }
}
