/////////////////////////////////////////////////////////////////////////////
// Name:        menu.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "menu.h"
#endif

#include "wx/menu.h"

//-----------------------------------------------------------------------------
// wxMenuBar
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMenuBar,wxWindow)

wxMenuBar::wxMenuBar(void)
{
  m_needParent = FALSE; // hmmm
  
  PreCreation( NULL, -1, wxDefaultPosition, wxDefaultSize, 0, "menu" );

  m_menus.DeleteContents( TRUE );
  
  m_widget = gtk_handle_box_new();
  
  m_menubar = gtk_menu_bar_new();
  
  gtk_container_add( GTK_CONTAINER(m_widget), m_menubar );
  
  gtk_widget_show( m_menubar );
  
  PostCreation();

  Show( TRUE );
};

void wxMenuBar::Append( wxMenu *menu, const wxString &title )
{
  m_menus.Append( menu );
  menu->m_title = title;    // ??????
  
  size_t pos;
  do {
    pos = menu->m_title.First( '&' );
    if (pos != wxString::npos) menu->m_title.Remove( pos, 1 );
  } while (pos != wxString::npos);
  
  GtkWidget *root_menu;
  root_menu = gtk_menu_item_new_with_label( WXSTRINGCAST(menu->m_title) );
  gtk_widget_show( root_menu );
  gtk_menu_item_set_submenu( GTK_MENU_ITEM(root_menu), menu->m_menu );
  
  gtk_menu_bar_append( GTK_MENU_BAR(m_menubar), root_menu );
};
    
int FindMenuItemRecursive( const wxMenu *menu, const wxString &menuString, const wxString &itemString )
{
  if (menu->m_title == menuString)
  {
    int res = menu->FindItem( itemString );
    if (res != -1) return res;
  };
  wxNode *node = menu->m_items.First();
  while (node)
  {
    wxMenuItem *item = (wxMenuItem*)node->Data();
    if (item->m_subMenu) return FindMenuItemRecursive( item->m_subMenu, menuString, itemString );
    node = node->Next();
  };
  return -1;
};

int wxMenuBar::FindMenuItem( const wxString &menuString, const wxString &itemString ) const
{
  wxNode *node = m_menus.First();
  while (node)
  {
    wxMenu *menu = (wxMenu*)node->Data();
    int res = FindMenuItemRecursive( menu, menuString, itemString);
    if (res != -1) return res;
    node = node->Next();
  };
  return -1;
};

    
//-----------------------------------------------------------------------------
// wxMenu
//-----------------------------------------------------------------------------

void gtk_menu_clicked_callback( GtkWidget *widget, gpointer data )
{
  wxMenu *menu = (wxMenu*)data;
  int id = menu->FindMenuIdByMenuItem(widget);
  
  if (!menu->Enabled(id)) return;
  
  wxCommandEvent event( wxEVENT_TYPE_MENU_COMMAND, id );
  event.SetEventObject( menu );
  event.SetInt(id );
  wxWindow *win = menu->GetInvokingWindow();
  if (win) win->ProcessEvent( event );
};

IMPLEMENT_DYNAMIC_CLASS(wxMenuItem,wxObject)
  
wxMenuItem::wxMenuItem(void)
{
  m_id = 0;
  m_text = "";
  m_isCheckMenu = FALSE;
  m_checked = FALSE;
  m_isSubMenu = FALSE; 
  m_subMenu = NULL;
  m_helpStr = "";
  m_menuItem = NULL;
};

IMPLEMENT_DYNAMIC_CLASS(wxMenu,wxEvtHandler)

wxMenu::wxMenu( const wxString &title )
{
  m_title = title;
  m_items.DeleteContents( TRUE );
  m_invokingWindow = NULL;
  m_menu = gtk_menu_new();  // Do not show!
};

void wxMenu::AppendSeparator(void)
{
  wxMenuItem *mitem = new wxMenuItem();
  mitem->m_id = -1;
  mitem->m_text = "";
  mitem->m_helpStr = "";
  mitem->m_isCheckMenu = FALSE;
  mitem->m_isEnabled = TRUE;
  mitem->m_menuItem = gtk_menu_item_new();
  gtk_menu_append( GTK_MENU(m_menu), mitem->m_menuItem );
  gtk_widget_show( mitem->m_menuItem );
  m_items.Append( mitem );
};

void wxMenu::Append( const int id, const wxString &item, const wxString &helpStr, const bool checkable )
{
  wxMenuItem *mitem = new wxMenuItem();
  mitem->m_id = id;
  
  mitem->m_text = item;
  size_t pos;
  do {
    pos = mitem->m_text.First( '&' );
    if (pos != wxString::npos) mitem->m_text.Remove( pos, 1 );
  } while (pos != wxString::npos);
  
  mitem->m_helpStr = helpStr;
  mitem->m_isCheckMenu = checkable;
  mitem->m_isEnabled = TRUE;
  if (checkable)
    mitem->m_menuItem = gtk_check_menu_item_new_with_label( WXSTRINGCAST(mitem->m_text) );
  else
    mitem->m_menuItem = gtk_menu_item_new_with_label( WXSTRINGCAST(mitem->m_text) );
    
  gtk_signal_connect( GTK_OBJECT(mitem->m_menuItem), "activate", 
    GTK_SIGNAL_FUNC(gtk_menu_clicked_callback), (gpointer*)this );
  
  gtk_menu_append( GTK_MENU(m_menu), mitem->m_menuItem );
  gtk_widget_show( mitem->m_menuItem );
  m_items.Append( mitem );
};

void wxMenu::Append( const int id, const wxString &item, wxMenu *subMenu, const wxString &helpStr )
{
  wxMenuItem *mitem = new wxMenuItem();
  mitem->m_id = id;
  mitem->m_text = item;
  mitem->m_isEnabled = TRUE;
  
  size_t pos;
  do {
    pos = mitem->m_text.First( '&' );
    if (pos != wxString::npos) mitem->m_text.Remove( pos, 1 );
  } while (pos != wxString::npos);
  
  mitem->m_helpStr = helpStr;
  mitem->m_menuItem = gtk_menu_item_new_with_label( WXSTRINGCAST(mitem->m_text) );
  
  mitem->m_subMenu = subMenu;
  gtk_menu_item_set_submenu( GTK_MENU_ITEM(mitem->m_menuItem), subMenu->m_menu );
  gtk_menu_append( GTK_MENU(m_menu), mitem->m_menuItem );
  gtk_widget_show( mitem->m_menuItem );
  m_items.Append( mitem );
};

int wxMenu::FindItem( const wxString itemString ) const
{
  wxString s( itemString );
  
  size_t pos;
  do {
    pos = s.First( '&' );
    if (pos != wxString::npos) s.Remove( pos, 1 );
  } while (pos != wxString::npos);
  
  wxNode *node = m_items.First();
  while (node)
  {
    wxMenuItem *item = (wxMenuItem*)node->Data();
    if (item->m_text == s) return item->m_id;
    node = node->Next();
  };
  return -1;
};

void wxMenu::Enable( const int id, const bool enable )
{
  wxNode *node = m_items.First();
  while (node)
  {
    wxMenuItem *item = (wxMenuItem*)node->Data();
    if (item->m_id == id)
    {
      item->m_isEnabled = enable;
      return;
    };
    node = node->Next();
  };
};

bool wxMenu::Enabled( const int id ) const
{
  wxNode *node = m_items.First();
  while (node)
  {
    wxMenuItem *item = (wxMenuItem*)node->Data();
    if (item->m_id == id) return item->m_isEnabled;
    node = node->Next();
  };
  return FALSE;
};

void wxMenu::SetLabel( const int id, const wxString &label )
{
  wxString s( label );
  size_t pos;
  do {
    pos = s.First( '&' );
    if (pos != wxString::npos) s.Remove( pos, 1 );
  } while (pos != wxString::npos);
  
  wxNode *node = m_items.First();
  while (node)
  {
    wxMenuItem *item = (wxMenuItem*)node->Data();
    item->m_text = s;
    if (item->m_id == id)
    {
      gtk_label_set( GTK_LABEL( GTK_BIN(item->m_menuItem)->child ), WXSTRINGCAST(s) );
    };
    node = node->Next();
  };
};

int wxMenu::FindMenuIdByMenuItem( GtkWidget *menuItem ) const
{
  wxNode *node = m_items.First();
  while (node)
  {
    wxMenuItem *item = (wxMenuItem*)node->Data();
    if (item->m_menuItem == menuItem) return item->m_id;
    node = node->Next();
  };
  return -1;
};

void wxMenu::SetInvokingWindow( wxWindow *win )
{
  m_invokingWindow = win;
};

wxWindow *wxMenu::GetInvokingWindow(void)
{
  return m_invokingWindow;
};


