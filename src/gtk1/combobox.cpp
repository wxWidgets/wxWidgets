/////////////////////////////////////////////////////////////////////////////
// Name:        combobox.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "combobox.h"
#endif

#include "wx/combobox.h"

//-----------------------------------------------------------------------------
// wxComboBox
//-----------------------------------------------------------------------------

void gtk_combo_clicked_callback( GtkWidget *WXUNUSED(widget), wxComboBox *combo )
{
  wxCommandEvent event(wxEVT_COMMAND_CHOICE_SELECTED, combo->GetId());
  event.SetInt( combo->GetSelection() );
  wxString tmp( combo->GetStringSelection() );
  event.SetString( WXSTRINGCAST(tmp) );
  event.SetEventObject(combo);
  combo->ProcessEvent(event);
};

//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxComboBox,wxControl)

bool wxComboBox::Create(wxWindow *parent, const wxWindowID id, const wxString& value,
  const wxPoint& pos, const wxSize& size,
  const int n, const wxString choices[],
  const long style, const wxString& name )
{
  m_needParent = TRUE;
  
  PreCreation( parent, id, pos, size, style, name );
  
  m_widget = gtk_combo_new();
  
  wxSize newSize = size;
  if (newSize.x == -1) newSize.x = 100;
  if (newSize.y == -1) newSize.y = 26;
  SetSize( newSize.x, newSize.y );
  
  GtkWidget *list = GTK_COMBO(m_widget)->list;
  
  for (int i = 0; i < n; i++)
  {
    GtkWidget *list_item;
    list_item = gtk_list_item_new_with_label( choices[i] ); 
  
    gtk_signal_connect( GTK_OBJECT(list_item), "select", 
      GTK_SIGNAL_FUNC(gtk_combo_clicked_callback), (gpointer)this );
  
    gtk_container_add( GTK_CONTAINER(list), list_item );
    
    gtk_widget_show( list_item );
  };
  
  PostCreation();

  if (!value.IsNull()) SetValue( value );
    
  Show( TRUE );
    
  return TRUE;
};

void wxComboBox::Clear(void)
{
  GtkWidget *list = GTK_COMBO(m_widget)->list;
  gtk_list_clear_items( GTK_LIST(list), 0, Number() );
};

void wxComboBox::Append( const wxString &item )
{
  GtkWidget *list = GTK_COMBO(m_widget)->list;
  
  GtkWidget *list_item;
  list_item = gtk_list_item_new_with_label( item ); 
  
 gtk_signal_connect( GTK_OBJECT(list_item), "select", 
    GTK_SIGNAL_FUNC(gtk_combo_clicked_callback), (gpointer)this );
  
  gtk_container_add( GTK_CONTAINER(list), list_item );
    
  gtk_widget_show( list_item );
};

void wxComboBox::Append( const wxString &WXUNUSED(item), char* WXUNUSED(clientData) )
{
};

void wxComboBox::Delete( const int n )
{
  GtkWidget *list = GTK_COMBO(m_widget)->list;
  gtk_list_clear_items( GTK_LIST(list), n, n );
};

int wxComboBox::FindString( const wxString &item )
{
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
  };
  return -1;
};

char* wxComboBox::GetClientData( const int n )
{
  wxNode *node = m_clientData.Nth( n );
  if (node) return (char*)node->Data();
  return NULL;
};

void wxComboBox::SetClientData( const int n, char * clientData )
{
  wxNode *node = m_clientData.Nth( n );
  if (node) node->SetData( (wxObject*) clientData );
};

int wxComboBox::GetSelection(void) const
{
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
    };
  };
  return -1;
};

wxString wxComboBox::GetString( const int n ) const
{
  GtkWidget *list = GTK_COMBO(m_widget)->list;
  
  GList *child = g_list_nth( GTK_LIST(list)->children, n );
  if (child)
  {
    GtkBin *bin = GTK_BIN( child->data );
    GtkLabel *label = GTK_LABEL( bin->child );
    return label->label;
  };
  return "";
};

wxString wxComboBox::GetStringSelection(void) const
{
  GtkWidget *list = GTK_COMBO(m_widget)->list;
  
  GList *selection = GTK_LIST(list)->selection;
  if (selection)
  {
    GtkBin *bin = GTK_BIN( selection->data );
    wxString tmp = GTK_LABEL( bin->child )->label;
    return tmp;
  };
  return "";
};

int wxComboBox::Number(void) const
{
  GtkWidget *list = GTK_COMBO(m_widget)->list;
  
  GList *child = GTK_LIST(list)->children;
  int count = 0;
  while (child) { count++; child = child->next; };
  return count;
};

void wxComboBox::SetSelection( const int n )
{
  GtkWidget *list = GTK_COMBO(m_widget)->list;
  gtk_list_select_item( GTK_LIST(list), n );
};

wxString wxComboBox::GetValue(void) const
{
  GtkWidget *entry = GTK_COMBO(m_widget)->entry;
  wxString tmp = gtk_entry_get_text( GTK_ENTRY(entry) );
  return tmp;
};

void wxComboBox::SetValue( const wxString& value )
{
  GtkWidget *entry = GTK_COMBO(m_widget)->entry;
  wxString tmp = "";
  if (!value.IsNull()) tmp = value;
  gtk_entry_set_text( GTK_ENTRY(entry), tmp );
};

void wxComboBox::Copy(void)
{
  GtkWidget *entry = GTK_COMBO(m_widget)->entry;
  gtk_editable_copy_clipboard( GTK_EDITABLE(entry), 0 );
};

void wxComboBox::Cut(void)
{
  GtkWidget *entry = GTK_COMBO(m_widget)->entry;
  gtk_editable_cut_clipboard( GTK_EDITABLE(entry), 0 );
};

void wxComboBox::Paste(void)
{
  GtkWidget *entry = GTK_COMBO(m_widget)->entry;
  gtk_editable_paste_clipboard( GTK_EDITABLE(entry), 0 );
};

void wxComboBox::SetInsertionPoint( const long pos )
{
  GtkWidget *entry = GTK_COMBO(m_widget)->entry;
  int tmp = (int) pos;
  gtk_entry_set_position( GTK_ENTRY(entry), tmp );
};

void wxComboBox::SetInsertionPointEnd(void)
{
  GtkWidget *entry = GTK_COMBO(m_widget)->entry;
  int pos = GTK_ENTRY(entry)->text_length;
  SetInsertionPoint( pos-1 );
};

long wxComboBox::GetInsertionPoint(void) const
{
  GtkWidget *entry = GTK_COMBO(m_widget)->entry;
  return (long) GTK_EDITABLE(entry)->current_pos;
};

long wxComboBox::GetLastPosition(void) const
{
  GtkWidget *entry = GTK_COMBO(m_widget)->entry;
  int pos = GTK_ENTRY(entry)->text_length;
  return (long) pos-1;
};

void wxComboBox::Replace( const long from, const long to, const wxString& value )
{
  GtkWidget *entry = GTK_COMBO(m_widget)->entry;
  gtk_editable_delete_text( GTK_EDITABLE(entry), (gint)from, (gint)to );
  if (value.IsNull()) return;
  gint pos = (gint)to;
  gtk_editable_insert_text( GTK_EDITABLE(entry), value, value.Length(), &pos );
};

void wxComboBox::Remove(const long from, const long to)
{
  GtkWidget *entry = GTK_COMBO(m_widget)->entry;
  gtk_editable_delete_text( GTK_EDITABLE(entry), (gint)from, (gint)to );
};

void wxComboBox::SetSelection( const long WXUNUSED(from), const long WXUNUSED(to) )
{
};

void wxComboBox::SetEditable( const bool WXUNUSED(editable) )
{
};

      
