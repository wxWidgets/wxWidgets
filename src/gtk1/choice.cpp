/////////////////////////////////////////////////////////////////////////////
// Name:        choice.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "choice.h"
#endif

#include "wx/choice.h"

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool   g_blockEventsOnDrag;

//-----------------------------------------------------------------------------
// wxChoice
//-----------------------------------------------------------------------------

static void gtk_choice_clicked_callback( GtkWidget *WXUNUSED(widget), wxChoice *choice )
{
  if (!choice->HasVMT()) return;
  if (g_blockEventsOnDrag) return;
  
  wxCommandEvent event(wxEVT_COMMAND_CHOICE_SELECTED, choice->GetId() );
  event.SetInt( choice->GetSelection() );
  wxString tmp( choice->GetStringSelection() );
  event.SetString( WXSTRINGCAST(tmp) );
  event.SetEventObject(choice);
  choice->GetEventHandler()->ProcessEvent(event);
}

//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxChoice,wxControl)

wxChoice::wxChoice(void)
{
}

bool wxChoice::Create( wxWindow *parent, wxWindowID id,
      const wxPoint &pos, const wxSize &size,
      int n, const wxString choices[],
      long style, const wxValidator& validator, const wxString &name )
{
  m_needParent = TRUE;
  
  PreCreation( parent, id, pos, size, style, name );
  
  SetValidator( validator );

  m_widget = gtk_option_menu_new();
  
  wxSize newSize = size;
  if (newSize.x == -1) newSize.x = 80;
  if (newSize.y == -1) newSize.y = 26;
  SetSize( newSize.x, newSize.y );
  
  GtkWidget *menu;
  menu = gtk_menu_new();
  
  for (int i = 0; i < n; i++)
  {
    GtkWidget *item;
    item = gtk_menu_item_new_with_label( choices[i] );
    gtk_signal_connect( GTK_OBJECT( item ), "activate", 
      GTK_SIGNAL_FUNC(gtk_choice_clicked_callback), (gpointer*)this );
    gtk_menu_append( GTK_MENU(menu), item );
    gtk_widget_show( item );
  }
  gtk_option_menu_set_menu( GTK_OPTION_MENU(m_widget), menu );
  
  PostCreation();
  
  Show( TRUE );
    
  return TRUE;
}
      
void wxChoice::Append( const wxString &item )
{
  GtkWidget *menu = gtk_option_menu_get_menu( GTK_OPTION_MENU(m_widget) );
  GtkWidget *menu_item;
  menu_item = gtk_menu_item_new_with_label( item );
  gtk_signal_connect( GTK_OBJECT( menu_item ), "activate", 
    GTK_SIGNAL_FUNC(gtk_choice_clicked_callback), (gpointer*)this );
  gtk_menu_append( GTK_MENU(menu), menu_item );
  gtk_widget_show( menu_item );
}
 
void wxChoice::Clear(void)
{
  gtk_option_menu_remove_menu( GTK_OPTION_MENU(m_widget) );
  GtkWidget *menu = gtk_menu_new();
  gtk_option_menu_set_menu( GTK_OPTION_MENU(m_widget), menu );
}

int wxChoice::FindString( const wxString &string ) const
{
  // If you read this code once and you think you understand
  // it, then you are very wrong. Robert Roebling.
  
  GtkMenuShell *menu_shell = GTK_MENU_SHELL( gtk_option_menu_get_menu( GTK_OPTION_MENU(m_widget) ) );
  int count = 0;
  GList *child = menu_shell->children;
  while (child)
  {
    GtkBin *bin = GTK_BIN( child->data );
    GtkLabel *label = NULL;
    if (bin->child) label = GTK_LABEL(bin->child);
    
    wxASSERT_MSG( label != NULL , "wxChoice: invalid label" );
    
    if (!label) label = GTK_LABEL( GTK_BUTTON(m_widget)->child );
    if (string == label->label) return count;
    child = child->next;
    count++;
  }
  
  wxFAIL_MSG( "wxChoice: string not found" );
  
  return -1;
}

int wxChoice::GetColumns(void) const
{
  return 1;
}

int wxChoice::GetSelection(void)
{
  GtkMenuShell *menu_shell = GTK_MENU_SHELL( gtk_option_menu_get_menu( GTK_OPTION_MENU(m_widget) ) );
  int count = 0;
  GList *child = menu_shell->children;
  while (child)
  {
    GtkBin *bin = GTK_BIN( child->data );
    if (!bin->child) return count;
    child = child->next;
    count++;
  }
  
  wxFAIL_MSG( "wxChoice: no selection" );
  
  return -1;
}

wxString wxChoice::GetString( int n ) const
{
  GtkMenuShell *menu_shell = GTK_MENU_SHELL( gtk_option_menu_get_menu( GTK_OPTION_MENU(m_widget) ) );
  int count = 0;
  GList *child = menu_shell->children;
  while (child)
  {
    GtkBin *bin = GTK_BIN( child->data );
    if (count == n)
    {
      GtkLabel *label = NULL;
      if (bin->child) label = GTK_LABEL(bin->child);
      
      wxASSERT_MSG( label != NULL , "wxChoice: invalid label" );
      
      if (!label) label = GTK_LABEL( GTK_BUTTON(m_widget)->child );
      return label->label;
    }
    child = child->next;
    count++;
  }
  
  wxFAIL_MSG( "wxChoice: string not found" );
  
  return "";
}

wxString wxChoice::GetStringSelection(void) const
{
  GtkLabel *label = GTK_LABEL( GTK_BUTTON(m_widget)->child );
  
  wxASSERT_MSG( label != NULL , "wxChoice: invalid label" );
  
  return label->label;
}

int wxChoice::Number(void) const
{
  GtkMenuShell *menu_shell = GTK_MENU_SHELL( gtk_option_menu_get_menu( GTK_OPTION_MENU(m_widget) ) );
  int count = 0;
  GList *child = menu_shell->children;
  while (child)
  {
    count++;
    child = child->next;
  }
  return count;
}

void wxChoice::SetColumns( int WXUNUSED(n) )
{
}

void wxChoice::SetSelection( int n )
{
  int tmp = n;
  gtk_option_menu_set_history( GTK_OPTION_MENU(m_widget), (gint)tmp );
  
  gtk_choice_clicked_callback( NULL, this );
}

void wxChoice::SetStringSelection( const wxString &string )
{
  int n = FindString( string );
  if (n != -1) SetSelection( n );
}

