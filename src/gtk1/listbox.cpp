/////////////////////////////////////////////////////////////////////////////
// Name:        listbox.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
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
// "select" and "deselect"
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
}

//-----------------------------------------------------------------------------
// wxListBox
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxListBox,wxControl)

wxListBox::wxListBox(void)
{
  m_list = (GtkList *) NULL;
}

bool wxListBox::Create( wxWindow *parent, wxWindowID id,
      const wxPoint &pos, const wxSize &size,
      int n, const wxString choices[],
      long style, const wxValidator& validator, const wxString &name )
{
  m_needParent = TRUE;

  PreCreation( parent, id, pos, size, style, name );

  SetValidator( validator );

  m_widget = gtk_scrolled_window_new( (GtkAdjustment *) NULL, (GtkAdjustment *) NULL );
  gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW(m_widget),
    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );

  m_list = GTK_LIST( gtk_list_new() );

  GtkSelectionMode mode = GTK_SELECTION_BROWSE;
  if (style & wxLB_MULTIPLE)
    mode = GTK_SELECTION_MULTIPLE;
  else if (style & wxLB_EXTENDED)
    mode = GTK_SELECTION_EXTENDED;

  gtk_list_set_selection_mode( GTK_LIST(m_list), mode );

  gtk_container_add (GTK_CONTAINER(m_widget), GTK_WIDGET(m_list) );
  gtk_widget_show( GTK_WIDGET(m_list) );

  wxSize newSize = size;
  if (newSize.x == -1) newSize.x = 100;
  if (newSize.y == -1) newSize.y = 110;
  SetSize( newSize.x, newSize.y );

  for (int i = 0; i < n; i++)
  {
    GtkWidget *list_item;
    list_item = gtk_list_item_new_with_label( choices[i] );

    gtk_container_add( GTK_CONTAINER(m_list), list_item );

    gtk_signal_connect( GTK_OBJECT(list_item), "select",
      GTK_SIGNAL_FUNC(gtk_listitem_select_callback), (gpointer)this );

    if (style & wxLB_MULTIPLE)
      gtk_signal_connect( GTK_OBJECT(list_item), "deselect",
        GTK_SIGNAL_FUNC(gtk_listitem_select_callback), (gpointer)this );

    m_clientData.Append( (wxObject*)NULL );

    gtk_widget_show( list_item );
  }

  PostCreation();

  gtk_widget_realize( GTK_WIDGET(m_list) );

  SetBackgroundColour( parent->GetBackgroundColour() );

  Show( TRUE );

  return TRUE;
}

void wxListBox::Append( const wxString &item )
{
  Append( item, (char*)NULL );
}

void wxListBox::Append( const wxString &item, char *clientData )
{
  wxCHECK_RET( m_list != NULL, "invalid listbox" );

  GtkWidget *list_item = gtk_list_item_new_with_label( item );

  if (m_hasOwnStyle)
  {
    GtkBin *bin = GTK_BIN( list_item );
    gtk_widget_set_style( bin->child,
      gtk_style_ref(
        gtk_widget_get_style( m_widget ) ) );
  }
  
  gtk_signal_connect( GTK_OBJECT(list_item), "select",
    GTK_SIGNAL_FUNC(gtk_listitem_select_callback), (gpointer)this );

  if (GetWindowStyleFlag() & wxLB_MULTIPLE)
    gtk_signal_connect( GTK_OBJECT(list_item), "deselect",
      GTK_SIGNAL_FUNC(gtk_listitem_select_callback), (gpointer)this );

  m_clientData.Append( (wxObject*)clientData );

  gtk_container_add( GTK_CONTAINER(m_list), list_item );

  gtk_widget_show( list_item );

  ConnectWidget( list_item );

  ConnectDnDWidget( list_item );
}

void wxListBox::Clear(void)
{
  wxCHECK_RET( m_list != NULL, "invalid listbox" );

  gtk_list_clear_items( m_list, 0, Number() );

  m_clientData.Clear();
}

void wxListBox::Delete( int n )
{
  wxCHECK_RET( m_list != NULL, "invalid listbox" );

  GList *child = g_list_nth( m_list->children, n );

  if (!child)
  {
    wxFAIL_MSG("wrong listbox index");
    return;
  }

  GList *list = g_list_append( NULL, child->data );
  gtk_list_remove_items( m_list, list );
  g_list_free( list );

  wxNode *node = m_clientData.Nth( n );
  if (!node)
  {
    wxFAIL_MSG("wrong listbox index");
  }
  else
    m_clientData.DeleteNode( node );
}

void wxListBox::Deselect( int n )
{
  wxCHECK_RET( m_list != NULL, "invalid listbox" );

  gtk_list_unselect_item( m_list, n );
}

int wxListBox::FindString( const wxString &item ) const
{
  wxCHECK_MSG( m_list != NULL, -1, "invalid listbox" );
  
  GList *child = m_list->children;
  int count = 0;
  while (child)
  {
    GtkBin *bin = GTK_BIN( child->data );
    GtkLabel *label = GTK_LABEL( bin->child );
    if (item == label->label) return count;
    count++;
    child = child->next;
  }

  // it's not an error if the string is not found - this function may be used to
  // test for existence of the string in the listbox, so don't give any
  // errors/assert failures.

  return -1;
}

char *wxListBox::GetClientData( int n ) const
{
  wxCHECK_MSG( m_list != NULL, (char*) NULL, "invalid listbox" );
  
  wxNode *node = m_clientData.Nth( n );
  if (node) return ((char*)node->Data());

  wxFAIL_MSG("wrong listbox index");
  return (char *) NULL;
}

int wxListBox::GetSelection(void) const
{
  wxCHECK_MSG( m_list != NULL, -1, "invalid listbox" );
  
  GList *child = m_list->children;
  int count = 0;
  while (child)
  {
    if (GTK_WIDGET(child->data)->state == GTK_STATE_SELECTED) return count;
    count++;
    child = child->next;
  }
  return -1;
}

int wxListBox::GetSelections(wxArrayInt& aSelections) const
{
  wxCHECK_MSG( m_list != NULL, -1, "invalid listbox" );
  
  // get the number of selected items first
  GList *child = m_list->children;
  int count = 0;
  for ( child = m_list->children; child != NULL; child = child->next )
  {
    if ( GTK_WIDGET(child->data)->state == GTK_STATE_SELECTED )
      count++;
  }

  aSelections.Empty();

  if ( count > 0 ) {
    // now fill the list
    aSelections.Alloc(count); // optimization attempt
    int i = 0;
    for ( child = m_list->children; child != NULL; child = child->next, i++ )
    {
      if ( GTK_WIDGET(child->data)->state == GTK_STATE_SELECTED )
        aSelections.Add(i);
    }
  }

  return count;
}

wxString wxListBox::GetString( int n ) const
{
  wxCHECK_MSG( m_list != NULL, "", "invalid listbox" );
  
  GList *child = g_list_nth( m_list->children, n );
  if (child)
  {
    GtkBin *bin = GTK_BIN( child->data );
    GtkLabel *label = GTK_LABEL( bin->child );
    return label->label;
  }
  wxFAIL_MSG("wrong listbox index");
  return "";
}

wxString wxListBox::GetStringSelection(void) const
{
  wxCHECK_MSG( m_list != NULL, "", "invalid listbox" );
  
  GList *selection = m_list->selection;
  if (selection)
  {
    GtkBin *bin = GTK_BIN( selection->data );
    wxString tmp = GTK_LABEL( bin->child )->label;
    return tmp;
  }
  wxFAIL_MSG("no listbox selection available");
  return "";
}

int wxListBox::Number(void)
{
  wxCHECK_MSG( m_list != NULL, -1, "invalid listbox" );
  
  GList *child = m_list->children;
  int count = 0;
  while (child) { count++; child = child->next; }
  return count;
}

bool wxListBox::Selected( int n )
{
  wxCHECK_MSG( m_list != NULL, FALSE, "invalid listbox" );
  
  GList *target = g_list_nth( m_list->children, n );
  if (target)
  {
    GList *child = m_list->selection;
    while (child)
    {
      if (child->data == target->data) return TRUE;
      child = child->next;
    }
  }
  wxFAIL_MSG("wrong listbox index");
  return FALSE;
}

void wxListBox::Set( int WXUNUSED(n), const wxString *WXUNUSED(choices) )
{
  wxFAIL_MSG("wxListBox::Set not implemented");
}

void wxListBox::SetClientData( int n, char *clientData )
{
  wxCHECK_RET( m_list != NULL, "invalid listbox" );
  
  wxNode *node = m_clientData.Nth( n );
  if (node)
  {
    node->SetData( (wxObject*)clientData );
  }
  else
  {
    wxFAIL_MSG("wrong listbox index");
  }
}

void wxListBox::SetFirstItem( int WXUNUSED(n) )
{
  wxFAIL_MSG("wxListBox::SetFirstItem not implemented");
}

void wxListBox::SetFirstItem( const wxString &WXUNUSED(item) )
{
  wxFAIL_MSG("wxListBox::SetFirstItem not implemented");
}

void wxListBox::SetSelection( int n, bool select )
{
  wxCHECK_RET( m_list != NULL, "invalid listbox" );
  
  if (select)
    gtk_list_select_item( m_list, n );
  else
    gtk_list_unselect_item( m_list, n );
}

void wxListBox::SetString( int n, const wxString &string )
{
  wxCHECK_RET( m_list != NULL, "invalid listbox" );
  
  GList *child = g_list_nth( m_list->children, n );
  if (child)
  {
    GtkBin *bin = GTK_BIN( child->data );
    GtkLabel *label = GTK_LABEL( bin->child );
    gtk_label_set( label, string );
  }
  else
  {
    wxFAIL_MSG("wrong listbox index");
  }
}

void wxListBox::SetStringSelection( const wxString &string, bool select )
{
  wxCHECK_RET( m_list != NULL, "invalid listbox" );
  
  SetSelection( FindString(string), select );
}

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
    }
  }
  return -1;
}

void wxListBox::SetDropTarget( wxDropTarget *dropTarget )
{
  wxCHECK_RET( m_list != NULL, "invalid listbox" );
  
  GList *child = m_list->children;
  while (child)
  {
    DisconnectDnDWidget( GTK_WIDGET( child->data ) );
    child = child->next;
  }

  wxWindow::SetDropTarget( dropTarget  );

  child = m_list->children;
  while (child)
  {
    ConnectDnDWidget( GTK_WIDGET( child->data ) );
    child = child->next;
  }
}

GtkWidget *wxListBox::GetConnectWidget(void)
{
  return GTK_WIDGET(m_list);
}

bool wxListBox::IsOwnGtkWindow( GdkWindow *window )
{
  if (wxWindow::IsOwnGtkWindow( window )) return TRUE;

  GList *child = m_list->children;
  while (child)
  {
    GtkBin *bin = GTK_BIN( child->data );
    if (bin->child->window == window) return TRUE;
    child = child->next;
  }

  return FALSE;
}

void wxListBox::SetFont( const wxFont &font )
{
  wxCHECK_RET( m_list != NULL, "invalid listbox" );
  
  wxControl::SetFont( font );

  GList *child = m_list->children;
  while (child)
  {
    gtk_widget_set_style( GTK_BIN(child->data)->child, 
      gtk_style_ref( 
        gtk_widget_get_style( m_widget ) ) );
	
    child = child->next;
  }
}

void wxListBox::SetBackgroundColour( const wxColour &colour )
{
  return;

  wxCHECK_RET( m_list != NULL, "invalid listbox" );
  
  wxControl::SetBackgroundColour( colour );
  
  return;
  
  if (!m_backgroundColour.Ok()) return;
  
  gtk_widget_set_style( GTK_WIDGET(m_list), 
    gtk_style_ref(
      gtk_widget_get_style( m_widget ) ) );
      
  GList *child = m_list->children;
  while (child)
  {
    gtk_widget_set_style( GTK_WIDGET(child->data), 
      gtk_style_ref( 
        gtk_widget_get_style( m_widget ) ) );
	
    gtk_widget_set_style( GTK_BIN(child->data)->child, 
      gtk_style_ref( 
        gtk_widget_get_style( m_widget ) ) );
	
    child = child->next;
  }
}

