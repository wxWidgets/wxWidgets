/////////////////////////////////////////////////////////////////////////////
// Name:        listbox.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "listbox.h"
#endif

#include "wx/dynarray.h"
#include "wx/listbox.h"
#include "wx/utils.h"
#include <wx/intl.h>

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool   g_blockEventsOnDrag;

//-----------------------------------------------------------------------------
// wxListBox
//-----------------------------------------------------------------------------

static void gtk_listitem_select_callback( GtkWidget *WXUNUSED(widget), wxListBox *listbox )
{
  if (!listbox->HasVMT()) return;
  if (g_blockEventsOnDrag) return;
  
  wxCommandEvent event(wxEVT_COMMAND_LISTBOX_SELECTED, listbox->GetId() );
  
    wxArrayInt aSelections;
    int count = listbox->GetSelections(aSelections);
    if ( count > 0 )
    {
      event.m_commandInt = aSelections[0] ;
      event.m_clientData = listbox->GetClientData(event.m_commandInt);
      wxString str(listbox->GetString(event.m_commandInt));
      if (str != "")
        event.m_commandString = copystring((char *)(const char *)str);
    }
    else
    {
      event.m_commandInt = -1 ;
      event.m_commandString = copystring("") ;
    }

  event.SetEventObject( listbox );

  listbox->GetEventHandler()->ProcessEvent( event );
  if (event.m_commandString) delete[] event.m_commandString ;
};

//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxListBox,wxControl)

wxListBox::wxListBox(void)
{
  m_list = NULL;
};

wxListBox::wxListBox( wxWindow *parent, wxWindowID id, 
      const wxPoint &pos, const wxSize &size, 
      int n, const wxString choices[],
      long style, const wxString &name )
{
  Create( parent, id, pos, size, n, choices, style, name );
};

bool wxListBox::Create( wxWindow *parent, wxWindowID id, 
      const wxPoint &pos, const wxSize &size, 
      int n, const wxString choices[],
      long style, const wxString &name )
{
  m_needParent = TRUE;
  
  PreCreation( parent, id, pos, size, style, name );
  
  m_widget = gtk_scrolled_window_new( NULL, NULL );
  gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW(m_widget),
    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
  
  m_list = GTK_LIST( gtk_list_new() );
  
  // @@ what's the difference between BROWSE and SINGLE?
  GtkSelectionMode mode = GTK_SELECTION_BROWSE;
  if ( style & wxLB_MULTIPLE )
    mode = GTK_SELECTION_MULTIPLE;
  else if ( style & wxLB_EXTENDED )
    mode = GTK_SELECTION_EXTENDED;

  gtk_list_set_selection_mode( GTK_LIST(m_list), mode );
  
  gtk_container_add (GTK_CONTAINER(m_widget), GTK_WIDGET(m_list) );
  gtk_widget_show( GTK_WIDGET(m_list) );
  
  for (int i = 0; i < n; i++)
  {
    GtkWidget *list_item;
    list_item = gtk_list_item_new_with_label( choices[i] ); 
  
    gtk_container_add( GTK_CONTAINER(m_list), list_item );
    
    gtk_signal_connect( GTK_OBJECT(list_item), "select", 
      GTK_SIGNAL_FUNC(gtk_listitem_select_callback), (gpointer)this );
  
    if ( style & wxLB_MULTIPLE )
      gtk_signal_connect( GTK_OBJECT(list_item), "deselect", 
        GTK_SIGNAL_FUNC(gtk_listitem_select_callback), (gpointer)this );
  
    m_clientData.Append( (wxObject*)NULL );
    
    gtk_widget_show( list_item );
  };
 
  PostCreation();
  
  gtk_widget_realize( GTK_WIDGET(m_list) );
  
  Show( TRUE );
  
  return TRUE;
};

void wxListBox::Append( const wxString &item )
{
  Append( item, (char*)NULL );
};

void wxListBox::Append( const wxString &item, char *clientData )
{
  GtkWidget *list_item;
  list_item = gtk_list_item_new_with_label( item ); 
  
  gtk_signal_connect( GTK_OBJECT(list_item), "select", 
    GTK_SIGNAL_FUNC(gtk_listitem_select_callback), (gpointer)this );
  
  if ( GetWindowStyleFlag() & wxLB_MULTIPLE )
    gtk_signal_connect( GTK_OBJECT(list_item), "deselect", 
      GTK_SIGNAL_FUNC(gtk_listitem_select_callback), (gpointer)this );
  
  gtk_container_add( GTK_CONTAINER(m_list), list_item );
  
  m_clientData.Append( (wxObject*)clientData );
  
  gtk_widget_show( list_item );
};

void wxListBox::Clear(void)
{
  gtk_list_clear_items( m_list, 0, Number() );
  
  m_clientData.Clear();
};

void wxListBox::Delete( int n )
{
  gtk_list_clear_items( m_list, n, n );
  
  wxNode *node = m_clientData.Nth( n );
  if (!node)
  {
    wxFAIL_MSG(_("wxListBox::Delete wrong index"));
  }
  else
    m_clientData.DeleteNode( node );
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

char *wxListBox::GetClientData( int n ) const
{
  wxNode *node = m_clientData.Nth( n );
  if (node) return ((char*)node->Data());
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

int wxListBox::GetSelections(wxArrayInt& aSelections) const
{
  // get the number of selected items first
  GList *child = m_list->children;
  int count = 0;
  for ( child = m_list->children; child != NULL; child = child->next ) {
    if ( GTK_WIDGET(child->data)->state == GTK_STATE_SELECTED ) 
      count++;
  }

  aSelections.Empty();
  
  if ( count > 0 ) {
    // now fill the list
    aSelections.Alloc(count); // optimization attempt
    int i = 0;
    for ( child = m_list->children; child != NULL; child = child->next, i++ ) {
      if ( GTK_WIDGET(child->data)->state == GTK_STATE_SELECTED ) 
        aSelections.Add(i);
    }
  }
  
  return count;
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

bool wxListBox::Selected( int n )
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

void wxListBox::Set( int WXUNUSED(n), const wxString *WXUNUSED(choices) )
{
};

void wxListBox::SetClientData( int n, char *clientData )
{
  wxNode *node = m_clientData.Nth( n );
  if (node) node->SetData( (wxObject*)clientData );
};

void wxListBox::SetFirstItem( int WXUNUSED(n) )
{
};

void wxListBox::SetFirstItem( const wxString &WXUNUSED(item) )
{
};

void wxListBox::SetSelection( int n, bool select )
{
  if (select)
    gtk_list_select_item( m_list, n );
  else
    gtk_list_unselect_item( m_list, n );
};

void wxListBox::SetString( int n, const wxString &string )
{
  GList *child = g_list_nth( m_list->children, n );
  if (child)
  {
    GtkBin *bin = GTK_BIN( child->data );
    GtkLabel *label = GTK_LABEL( bin->child );
    gtk_label_set( label, string );
  };
};

void wxListBox::SetStringSelection( const wxString &string, bool select )
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

GtkWidget *wxListBox::GetDropTargetWidget(void)
{
  return GTK_WIDGET(m_list);
};



