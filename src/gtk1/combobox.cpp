/////////////////////////////////////////////////////////////////////////////
// Name:        combobox.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "combobox.h"
#endif

#include "wx/combobox.h"
#include <wx/intl.h>

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool   g_blockEventsOnDrag;

//-----------------------------------------------------------------------------
// "select"
//-----------------------------------------------------------------------------

static void gtk_combo_clicked_callback( GtkWidget *WXUNUSED(widget), wxComboBox *combo )
{
  if (!combo->HasVMT()) return;
  if (g_blockEventsOnDrag) return;
  
  if (combo->m_alreadySent)
  {
    combo->m_alreadySent = FALSE;
    return;
  }

  combo->m_alreadySent = TRUE;
  
  wxCommandEvent event(wxEVT_COMMAND_CHOICE_SELECTED, combo->GetId());
  event.SetInt( combo->GetSelection() );
  wxString tmp( combo->GetStringSelection() );
  event.SetString( WXSTRINGCAST(tmp) );
  event.SetEventObject(combo);
  combo->GetEventHandler()->ProcessEvent(event);
}

//-----------------------------------------------------------------------------
// wxComboBox
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxComboBox,wxControl)

BEGIN_EVENT_TABLE(wxComboBox, wxControl)
  EVT_SIZE(wxComboBox::OnSize)
END_EVENT_TABLE()

bool wxComboBox::Create(wxWindow *parent, wxWindowID id, const wxString& value,
  const wxPoint& pos, const wxSize& size,
  int n, const wxString choices[],
  long style, const wxValidator& validator, const wxString& name )
{
  m_alreadySent = FALSE;
  m_needParent = TRUE;
  
  PreCreation( parent, id, pos, size, style, name );
  
  SetValidator( validator );

  m_widget = gtk_combo_new();
  
  wxSize newSize = size;
  if (newSize.x == -1) newSize.x = 100;
  if (newSize.y == -1) newSize.y = 26;
  SetSize( newSize.x, newSize.y );
  
  GtkWidget *list = GTK_COMBO(m_widget)->list;
  
  for (int i = 0; i < n; i++)
  {
    GtkWidget *list_item = gtk_list_item_new_with_label( choices[i] ); 
  
    m_clientData.Append( (wxObject*)NULL );
    
    gtk_container_add( GTK_CONTAINER(list), list_item );
    
    gtk_widget_realize( list_item );
    gtk_widget_realize( GTK_BIN(list_item)->child );
    
    gtk_widget_show( list_item );
    
    gtk_signal_connect( GTK_OBJECT(list_item), "select", 
      GTK_SIGNAL_FUNC(gtk_combo_clicked_callback), (gpointer)this );
  }
  
  PostCreation();

  ConnectWidget( GTK_COMBO(m_widget)->button );
  
  if (!value.IsNull()) SetValue( value );
    
  gtk_widget_realize( GTK_COMBO(m_widget)->list );
  gtk_widget_realize( GTK_COMBO(m_widget)->entry );
  gtk_widget_realize( GTK_COMBO(m_widget)->button );
  
  SetBackgroundColour( parent->GetBackgroundColour() );
  SetForegroundColour( parent->GetForegroundColour() );

  Show( TRUE );
    
  return TRUE;
}

void wxComboBox::Clear(void)
{
  wxCHECK_RET( m_widget != NULL, "invalid combobox" );
  
  GtkWidget *list = GTK_COMBO(m_widget)->list;
  gtk_list_clear_items( GTK_LIST(list), 0, Number() );
  
  m_clientData.Clear();
}

void wxComboBox::Append( const wxString &item )
{
  wxCHECK_RET( m_widget != NULL, "invalid combobox" );
  
  Append( item, (char*)NULL );
}

void wxComboBox::Append( const wxString &item, char *clientData )
{
  wxCHECK_RET( m_widget != NULL, "invalid combobox" );
  
  GtkWidget *list = GTK_COMBO(m_widget)->list;
  
  GtkWidget *list_item = gtk_list_item_new_with_label( item ); 
  
  if (m_widgetStyle) ApplyWidgetStyle();
  
  gtk_signal_connect( GTK_OBJECT(list_item), "select", 
    GTK_SIGNAL_FUNC(gtk_combo_clicked_callback), (gpointer)this );
  
  m_clientData.Append( (wxObject*)clientData );
  
  gtk_container_add( GTK_CONTAINER(list), list_item );
    
  gtk_widget_show( list_item );
}

void wxComboBox::Delete( int n )
{
  wxCHECK_RET( m_widget != NULL, "invalid combobox" );
  
  GtkList *listbox = GTK_LIST( GTK_COMBO(m_widget)->list );
  
  GList *child = g_list_nth( listbox->children, n );
  
  if (!child)
  {
    wxFAIL_MSG("wrong index");
    return;
  }
  
  GList *list = g_list_append( NULL, child->data );
  gtk_list_remove_items( listbox, list );
  g_list_free( list );
  
  wxNode *node = m_clientData.Nth( n );
  if (!node)
  {
    wxFAIL_MSG( "wrong index" );
  }
  else
    m_clientData.DeleteNode( node );
}

int wxComboBox::FindString( const wxString &item )
{
  wxCHECK_MSG( m_widget != NULL, -1, "invalid combobox" );
  
  GtkWidget *list = GTK_COMBO(m_widget)->list;
  
  GList *child = GTK_LIST(list)->children;
  int count = 0;
  while (child)
  {
    GtkBin *bin = GTK_BIN( child->data );
    GtkLabel *label = GTK_LABEL( bin->child );
    if (item == label->label) return count;
    count++;
    child = child->next;
  }
  
  wxFAIL_MSG( "wxComboBox: string not found" );
  
  return -1;
}

char* wxComboBox::GetClientData( int n )
{
  wxCHECK_MSG( m_widget != NULL, (char*)NULL, "invalid combobox" );
  
  wxNode *node = m_clientData.Nth( n );
  if (node) return (char*)node->Data();
  
  wxFAIL_MSG( "wxComboBox: wrong index" );
  
  return (char *) NULL;
}

void wxComboBox::SetClientData( int n, char * clientData )
{
  wxCHECK_RET( m_widget != NULL, "invalid combobox" );
  
  wxNode *node = m_clientData.Nth( n );
  if (node) node->SetData( (wxObject*) clientData );
  
  wxFAIL_MSG( "wxComboBox: wrong index" );
}

int wxComboBox::GetSelection(void) const
{
  wxCHECK_MSG( m_widget != NULL, -1, "invalid combobox" );
  
  GtkWidget *list = GTK_COMBO(m_widget)->list;
  
  GList *selection = GTK_LIST(list)->selection;
  if (selection)
  {
    GList *child = GTK_LIST(list)->children;
    int count = 0;
    while (child)
    {
      if (child->data == selection->data) return count;
      count++;
      child = child->next;
    }
  }
  
  wxFAIL_MSG( "wxComboBox: no selection" );
  
  return -1;
}

wxString wxComboBox::GetString( int n ) const
{
  wxCHECK_MSG( m_widget != NULL, "", "invalid combobox" );
  
  GtkWidget *list = GTK_COMBO(m_widget)->list;
  
  GList *child = g_list_nth( GTK_LIST(list)->children, n );
  if (child)
  {
    GtkBin *bin = GTK_BIN( child->data );
    GtkLabel *label = GTK_LABEL( bin->child );
    return label->label;
  }
  
  wxFAIL_MSG( "wxComboBox: wrong index" );
  
  return "";
}

wxString wxComboBox::GetStringSelection(void) const
{
  wxCHECK_MSG( m_widget != NULL, "", "invalid combobox" );
  
  GtkWidget *list = GTK_COMBO(m_widget)->list;
  
  GList *selection = GTK_LIST(list)->selection;
  if (selection)
  {
    GtkBin *bin = GTK_BIN( selection->data );
    wxString tmp = GTK_LABEL( bin->child )->label;
    return tmp;
  }
  
  wxFAIL_MSG( "wxComboBox: no selection" );
  
  return "";
}

int wxComboBox::Number(void) const
{
  wxCHECK_MSG( m_widget != NULL, 0, "invalid combobox" );
  
  GtkWidget *list = GTK_COMBO(m_widget)->list;
  
  GList *child = GTK_LIST(list)->children;
  int count = 0;
  while (child) { count++; child = child->next; }
  return count;
}

void wxComboBox::SetSelection( int n )
{
  wxCHECK_RET( m_widget != NULL, "invalid combobox" );
  
  GtkWidget *list = GTK_COMBO(m_widget)->list;
  gtk_list_select_item( GTK_LIST(list), n );
}

void wxComboBox::SetStringSelection( const wxString &string )
{
  wxCHECK_RET( m_widget != NULL, "invalid combobox" );
  
  int res = FindString( string );
  if (res == -1) return;
  SetSelection( res );
}

wxString wxComboBox::GetValue(void) const
{
  GtkWidget *entry = GTK_COMBO(m_widget)->entry;
  wxString tmp = gtk_entry_get_text( GTK_ENTRY(entry) );
  return tmp;
}

void wxComboBox::SetValue( const wxString& value )
{
  wxCHECK_RET( m_widget != NULL, "invalid combobox" );
  
  GtkWidget *entry = GTK_COMBO(m_widget)->entry;
  wxString tmp = "";
  if (!value.IsNull()) tmp = value;
  gtk_entry_set_text( GTK_ENTRY(entry), tmp );
}

void wxComboBox::Copy(void)
{
  wxCHECK_RET( m_widget != NULL, "invalid combobox" );
  
  GtkWidget *entry = GTK_COMBO(m_widget)->entry;
#if (GTK_MINOR_VERSION == 1)
  gtk_editable_copy_clipboard( GTK_EDITABLE(entry) );
#else
  gtk_editable_copy_clipboard( GTK_EDITABLE(entry), 0 );
#endif
}

void wxComboBox::Cut(void)
{
  wxCHECK_RET( m_widget != NULL, "invalid combobox" );
  
  GtkWidget *entry = GTK_COMBO(m_widget)->entry;
#if (GTK_MINOR_VERSION == 1)
  gtk_editable_cut_clipboard( GTK_EDITABLE(entry) );
#else
  gtk_editable_cut_clipboard( GTK_EDITABLE(entry), 0 );
#endif
}

void wxComboBox::Paste(void)
{
  wxCHECK_RET( m_widget != NULL, "invalid combobox" );
  
  GtkWidget *entry = GTK_COMBO(m_widget)->entry;
#if (GTK_MINOR_VERSION == 1)
  gtk_editable_paste_clipboard( GTK_EDITABLE(entry) );
#else
  gtk_editable_paste_clipboard( GTK_EDITABLE(entry), 0 );
#endif
}

void wxComboBox::SetInsertionPoint( long pos )
{
  wxCHECK_RET( m_widget != NULL, "invalid combobox" );
  
  GtkWidget *entry = GTK_COMBO(m_widget)->entry;
  int tmp = (int) pos;
  gtk_entry_set_position( GTK_ENTRY(entry), tmp );
}

void wxComboBox::SetInsertionPointEnd(void)
{
  wxCHECK_RET( m_widget != NULL, "invalid combobox" );
  
  GtkWidget *entry = GTK_COMBO(m_widget)->entry;
  int pos = GTK_ENTRY(entry)->text_length;
  SetInsertionPoint( pos-1 );
}

long wxComboBox::GetInsertionPoint(void) const
{
  GtkWidget *entry = GTK_COMBO(m_widget)->entry;
  return (long) GTK_EDITABLE(entry)->current_pos;
}

long wxComboBox::GetLastPosition(void) const
{
  GtkWidget *entry = GTK_COMBO(m_widget)->entry;
  int pos = GTK_ENTRY(entry)->text_length;
  return (long) pos-1;
}

void wxComboBox::Replace( long from, long to, const wxString& value )
{
  wxCHECK_RET( m_widget != NULL, "invalid combobox" );
  
  GtkWidget *entry = GTK_COMBO(m_widget)->entry;
  gtk_editable_delete_text( GTK_EDITABLE(entry), (gint)from, (gint)to );
  if (value.IsNull()) return;
  gint pos = (gint)to;
  gtk_editable_insert_text( GTK_EDITABLE(entry), value, value.Length(), &pos );
}

void wxComboBox::Remove(long from, long to)
{
  wxCHECK_RET( m_widget != NULL, "invalid combobox" );
  
  GtkWidget *entry = GTK_COMBO(m_widget)->entry;
  gtk_editable_delete_text( GTK_EDITABLE(entry), (gint)from, (gint)to );
}

void wxComboBox::SetSelection( long WXUNUSED(from), long WXUNUSED(to) )
{
  wxFAIL_MSG( "wxComboBox::SetSelection not implemented" );
}

void wxComboBox::SetEditable( bool WXUNUSED(editable) )
{
  wxFAIL_MSG( "wxComboBox::SetEditable not implemented" );
}

void wxComboBox::OnSize( wxSizeEvent &event )
{
  wxControl::OnSize( event );
  
  int w = 21;
  
  gtk_widget_set_usize( GTK_COMBO(m_widget)->entry, m_width-w-1, m_height );
  
  gtk_widget_set_uposition( GTK_COMBO(m_widget)->button, m_x+m_width-w, m_y );
  gtk_widget_set_usize( GTK_COMBO(m_widget)->button, w, m_height );
}

void wxComboBox::ApplyWidgetStyle()
{
  SetWidgetStyle();
  
  gtk_widget_set_style( GTK_COMBO(m_widget)->button, m_widgetStyle );
  gtk_widget_set_style( GTK_COMBO(m_widget)->entry, m_widgetStyle );
  gtk_widget_set_style( GTK_COMBO(m_widget)->list, m_widgetStyle );
  
  GtkList *list = GTK_LIST( GTK_COMBO(m_widget)->list );
  GList *child = list->children;
  while (child)
  {
    gtk_widget_set_style( GTK_WIDGET(child->data), m_widgetStyle );
  
    GtkBin *bin = GTK_BIN(child->data);
    gtk_widget_set_style( bin->child, m_widgetStyle );
        
    child = child->next;
  }
}

GtkWidget* wxComboBox::GetConnectWidget(void)
{
  return GTK_COMBO(m_widget)->entry;
}

bool wxComboBox::IsOwnGtkWindow( GdkWindow *window )
{
  return ( (window == GTK_ENTRY( GTK_COMBO(m_widget)->entry )->text_area) ||
           (window == GTK_COMBO(m_widget)->button->window ) );
}
