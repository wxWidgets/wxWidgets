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
// wxChoice
//-----------------------------------------------------------------------------

void gtk_choice_clicked_callback( GtkWidget *WXUNUSED(widget), gpointer data )
{
  wxChoice *choice = (wxChoice*)data;
  wxCommandEvent event(wxEVT_COMMAND_CHOICE_SELECTED, choice->GetId());
  event.SetInt( choice->GetSelection() );
  wxString tmp( choice->GetStringSelection() );
  event.SetString( WXSTRINGCAST(tmp) );
  event.SetEventObject(choice);
  choice->ProcessEvent(event);
};

//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxChoice,wxControl)

wxChoice::wxChoice(void)
{
};

wxChoice::wxChoice( wxWindow *parent, const wxWindowID id,
      const wxPoint &pos, const wxSize &size, 
      const int n, const wxString choices[],
      const long style, const wxString &name )
{
  Create( parent, id, pos, size, n, choices, style, name );
};

bool wxChoice::Create( wxWindow *parent, const wxWindowID id,
      const wxPoint &pos, const wxSize &size, 
      const int n, const wxString choices[],
      const long style, const wxString &name )
{
  m_needParent = TRUE;
  
  PreCreation( parent, id, pos, size, style, name );
  
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
  };
  gtk_option_menu_set_menu( GTK_OPTION_MENU(m_widget), menu );
  
  PostCreation();
  
  Show( TRUE );
    
  return TRUE;
};
      
void wxChoice::Append( const wxString &item )
{
  GtkWidget *menu = gtk_option_menu_get_menu( GTK_OPTION_MENU(m_widget) );
  GtkWidget *menu_item;
  menu_item = gtk_menu_item_new_with_label( item );
  gtk_signal_connect( GTK_OBJECT( menu_item ), "activate", 
    GTK_SIGNAL_FUNC(gtk_choice_clicked_callback), (gpointer*)this );
  gtk_menu_append( GTK_MENU(menu), menu_item );
  gtk_widget_show( menu_item );
};
 
void wxChoice::Clear(void)
{
  gtk_option_menu_remove_menu( GTK_OPTION_MENU(m_widget) );
  GtkWidget *menu = gtk_menu_new();
  gtk_option_menu_set_menu( GTK_OPTION_MENU(m_widget), menu );
};

int wxChoice::FindString( const wxString &string ) const
{
  // If you read this code once and you think you undestand
  // it, then you are very wrong. RR
  
  GtkMenuShell *menu_shell = GTK_MENU_SHELL( gtk_option_menu_get_menu( GTK_OPTION_MENU(m_widget) ) );
  int count = 0;
  GList *child = menu_shell->children;
  while (child)
  {
    GtkBin *bin = GTK_BIN( child->data );
    GtkLabel *label = GTK_LABEL(bin->child);
    if (!label) label = GTK_LABEL( GTK_BUTTON(m_widget)->child );
    if (string == label->label) return count;
    child = child->next;
    count++;
  };
  return -1;
};

int wxChoice::GetColumns(void) const
{
  return 1;
};

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
  };
  return -1;
};

wxString wxChoice::GetString( const int n ) const
{
  GtkMenuShell *menu_shell = GTK_MENU_SHELL( gtk_option_menu_get_menu( GTK_OPTION_MENU(m_widget) ) );
  int count = 0;
  GList *child = menu_shell->children;
  while (child)
  {
    GtkBin *bin = GTK_BIN( child->data );
    if (count == n)
    {
      GtkLabel *label = GTK_LABEL(bin->child);
      if (!label) label = GTK_LABEL( GTK_BUTTON(m_widget)->child );
      return label->label;
    };
    child = child->next;
    count++;
  };
  return "";
};

wxString wxChoice::GetStringSelection(void) const
{
  GtkLabel *label = GTK_LABEL( GTK_BUTTON(m_widget)->child );
  return label->label;
};

int wxChoice::Number(void) const
{
  GtkMenu *menu = GTK_MENU( gtk_option_menu_get_menu( GTK_OPTION_MENU(m_widget) ) );
  int count = 0;
  GList *child = menu->children;
  while (child)
  {
    count++;
    child = child->next;
  };
  return count;
};

void wxChoice::SetColumns( const int WXUNUSED(n) )
{
};

void wxChoice::SetSelection( const int n )
{
  int tmp = n;
  gtk_option_menu_set_history( GTK_OPTION_MENU(m_widget), (gint)tmp );
};

void wxChoice::SetStringSelection( const wxString &string )
{
  int n = FindString( string );
  if (n != -1) SetSelection( n );
};

