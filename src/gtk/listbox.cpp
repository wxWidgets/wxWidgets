/////////////////////////////////////////////////////////////////////////////
// Name:        listbox.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "listbox.h"
#endif

#include "wx/listbox.h"

//-----------------------------------------------------------------------------
// wxListBox
//-----------------------------------------------------------------------------

void gtk_listitem_select_callback( GtkWidget *widget, gpointer data )
{
  wxListBox *listbox = (wxListBox*)data;

  wxCommandEvent event(wxEVT_COMMAND_LISTBOX_SELECTED, listbox->GetId() );
  
  event.SetInt( listbox->GetIndex( widget ) );
  
  GtkBin *bin = GTK_BIN( widget );
  GtkLabel *label = GTK_LABEL( bin->child );
  wxString tmp( label->label );
  event.SetString( WXSTRINGCAST(tmp) );
  
  event.SetEventObject( listbox );
  
  listbox->ProcessEvent( event );
};

//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxListBox,wxControl)

wxListBox::wxListBox(void)
{
  m_list = NULL;
};

wxListBox::wxListBox( wxWindow *parent, wxWindowID id, 
      const wxPoint &pos, const wxSize &size, 
      const int n, const wxString choices[],
      const long style, const wxString &name )
{
  Create( parent, id, pos, size, n, choices, style, name );
};

bool wxListBox::Create( wxWindow *parent, wxWindowID id, 
      const wxPoint &pos, const wxSize &size, 
      const int n, const wxString choices[],
      const long style, const wxString &name )
{
  m_needParent = TRUE;
  
  PreCreation( parent, id, pos, size, style, name );
  
  m_widget = gtk_scrolled_window_new( NULL, NULL );
  gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW(m_widget),
    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
  
  m_list = GTK_LIST( gtk_list_new() );
  gtk_list_set_selection_mode( GTK_LIST(m_list), GTK_SELECTION_BROWSE );
  
  gtk_container_add (GTK_CONTAINER(m_widget), GTK_WIDGET(m_list) );
  gtk_widget_show( GTK_WIDGET(m_list) );
  
  for (int i = 0; i < n; i++)
  {
    GtkWidget *list_item;
    list_item = gtk_list_item_new_with_label( choices[i] ); 
  
    gtk_signal_connect( GTK_OBJECT(list_item), "select", 
      GTK_SIGNAL_FUNC(gtk_listitem_select_callback), (gpointer)this );
  
    gtk_container_add( GTK_CONTAINER(m_list), list_item );
    
    gtk_widget_show( list_item );
  };
 
  PostCreation();
  
  Show( TRUE );
  
  return TRUE;
};

void wxListBox::Append( const wxString &item )
{
  GtkWidget *list_item;
  list_item = gtk_list_item_new_with_label( item ); 
  
 gtk_signal_connect( GTK_OBJECT(list_item), "select", 
    GTK_SIGNAL_FUNC(gtk_listitem_select_callback), (gpointer)this );
  
  gtk_container_add( GTK_CONTAINER(m_list), list_item );
    
  gtk_widget_show( list_item );
};

void wxListBox::Append( const wxString &WXUNUSED(item), char *WXUNUSED(clientData) )
{
};

void wxListBox::Clear(void)
{
  gtk_list_clear_items( m_list, 0, Number() );
};

void wxListBox::Delete( int n )
{
  gtk_list_clear_items( m_list, n, n );
};

void wxListBox::Deselect( int n )
{
  gtk_list_unselect_item( m_list, n );
};

int wxListBox::FindString( const wxString &item ) const
{
  GList *child = m_list->children;
  int count = 0;
  while (child)
  {
    GtkBin *bin = GTK_BIN( child->data );
    GtkLabel *label = GTK_LABEL( bin->child );
    if (item == label->label) return count;
    count++;
    child = child->next;
  };
  return -1;
};

char *wxListBox::GetClientData( const int WXUNUSED(n) ) const
{
  return NULL;
};

int wxListBox::GetSelection(void) const
{
  GList *selection = m_list->selection;
  if (selection)
  {
    GList *child = m_list->children;
    int count = 0;
    while (child)
    {
      if (child->data == selection->data) return count;
      count++;
      child = child->next;
    };
  };
  return -1;
};

int wxListBox::GetSelections( int **WXUNUSED(selections) ) const
{
  return 0;
};

wxString wxListBox::GetString( int n ) const
{
  GList *child = g_list_nth( m_list->children, n );
  if (child)
  {
    GtkBin *bin = GTK_BIN( child->data );
    GtkLabel *label = GTK_LABEL( bin->child );
    return label->label;
  };
  return "";
};

wxString wxListBox::GetStringSelection(void) const
{
  GList *selection = m_list->selection;
  if (selection)
  {
    GtkBin *bin = GTK_BIN( selection->data );
    wxString tmp = GTK_LABEL( bin->child )->label;
    return tmp;
  };
  return "";
};

int wxListBox::Number(void)
{
  GList *child = m_list->children;
  int count = 0;
  while (child) { count++; child = child->next; };
  return count;
};

bool wxListBox::Selected( const int n )
{
  GList *target = g_list_nth( m_list->children, n );
  if (target)
  {
    GList *child = m_list->selection;
    while (child)
    {
      if (child->data == target->data) return TRUE;
      child = child->next;
    };
  };
  return FALSE;
};

void wxListBox::Set( const int WXUNUSED(n), const wxString *WXUNUSED(choices) )
{
};

void wxListBox::SetClientData( const int WXUNUSED(n), char *WXUNUSED(clientData) )
{
};

void wxListBox::SetFirstItem( int WXUNUSED(n) )
{
};

void wxListBox::SetFirstItem( const wxString &WXUNUSED(item) )
{
};

void wxListBox::SetSelection( const int n, const bool select )
{
  if (select)
    gtk_list_select_item( m_list, n );
  else
    gtk_list_unselect_item( m_list, n );
};

void wxListBox::SetString( const int WXUNUSED(n), const wxString &WXUNUSED(string) )
{
};

void wxListBox::SetStringSelection( const wxString &string, const bool select )
{
  SetSelection( FindString(string), select );
};

int wxListBox::GetIndex( GtkWidget *item ) const
{
  if (item)
  {
    GList *child = m_list->children;
    int count = 0;
    while (child)
    {
      if (GTK_WIDGET(child->data) == item) return count;
      count++;
      child = child->next;
    };
  };
  return -1;
};


