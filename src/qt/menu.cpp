/////////////////////////////////////////////////////////////////////////////
// Name:        menu.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "menu.h"
#endif

#include "wx/menu.h"
#include "wx/log.h"

//-----------------------------------------------------------------------------
// wxMenuBar
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMenuBar,wxWindow)

wxMenuBar::wxMenuBar()
{
};

void wxMenuBar::Append( wxMenu *menu, const wxString &title )
{
  m_menus.Append( menu );
  menu->m_title = title;    // ??????

  int pos;
  do {
    pos = menu->m_title.First( '&' );
    if (pos != -1) menu->m_title.Remove( pos, 1 );
  } while (pos != -1);

};

static int FindMenuItemRecursive( const wxMenu *menu, const wxString &menuString, const wxString &itemString )
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
    if (item->IsSubMenu())
      return FindMenuItemRecursive(item->GetSubMenu(), menuString, itemString);
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

// Find a wxMenuItem using its id. Recurses down into sub-menus
static wxMenuItem* FindMenuItemByIdRecursive(const wxMenu* menu, int id)
{
  wxMenuItem* result = menu->FindItem(id);

  wxNode *node = menu->m_items.First();
  while ( node && result == NULL ) {
    wxMenuItem *item = (wxMenuItem*)node->Data();
    if ( item->IsSubMenu() )
      result = FindMenuItemByIdRecursive( item->GetSubMenu(), id );
    node = node->Next();
  };

  return result;
};

wxMenuItem* wxMenuBar::FindMenuItemById( int id ) const
{
  wxMenuItem* result = 0;
  wxNode *node = m_menus.First();
  while (node && result == 0)
  {
    wxMenu *menu = (wxMenu*)node->Data();
    result = FindMenuItemByIdRecursive( menu, id );
    node = node->Next();
  }
  return result;
}

void wxMenuBar::Check( int id, bool check )
{
  wxMenuItem* item = FindMenuItemById( id );
  if (item) item->Check(check);
};

bool wxMenuBar::Checked( int id ) const
{
  wxMenuItem* item = FindMenuItemById( id );
  if (item) return item->IsChecked();
  return FALSE;
};

void wxMenuBar::Enable( int id, bool enable )
{
  wxMenuItem* item = FindMenuItemById( id );
  if (item) item->Enable(enable);
};

bool wxMenuBar::Enabled( int id ) const
{
  wxMenuItem* item = FindMenuItemById( id );
  if (item) return item->IsEnabled();
  return FALSE;
};

//-----------------------------------------------------------------------------
// wxMenu
//-----------------------------------------------------------------------------


IMPLEMENT_DYNAMIC_CLASS(wxMenuItem,wxObject)

wxMenuItem::wxMenuItem()
{
  m_id = ID_SEPARATOR;
  m_isCheckMenu = FALSE;
  m_isChecked = FALSE;
  m_isEnabled = TRUE;
  m_subMenu = NULL;
};

void wxMenuItem::SetText(const wxString& str)
{
  for ( const char *pc = str; *pc != '\0'; pc++ ) {
    if ( *pc == '&' )
      pc++; // skip it

    m_text << *pc;
  }
}

void wxMenuItem::Check( bool check )
{
  wxCHECK_RET( IsCheckable(), "can't check uncheckable item!" )

  m_isChecked = check;
}

bool wxMenuItem::IsChecked() const
{
  wxCHECK( IsCheckable(), FALSE ); // can't get state of uncheckable item!

  return FALSE;
}

IMPLEMENT_DYNAMIC_CLASS(wxMenu,wxEvtHandler)

wxMenu::wxMenu( const wxString &title )
{
  m_title = title;
  m_items.DeleteContents( TRUE );
  m_invokingWindow = NULL;
};

void wxMenu::AppendSeparator()
{
  wxMenuItem *mitem = new wxMenuItem();
  mitem->SetId(ID_SEPARATOR);

  m_items.Append( mitem );
};

void wxMenu::Append( int id, const wxString &item, const wxString &helpStr, bool checkable )
{
  wxMenuItem *mitem = new wxMenuItem();
  mitem->SetId(id);
  mitem->SetText(item);
  mitem->SetHelpString(helpStr);
  mitem->SetCheckable(checkable);
  
  m_items.Append( mitem );
};

void wxMenu::Append( int id, const wxString &text, wxMenu *subMenu, const wxString &helpStr )
{
  wxMenuItem *mitem = new wxMenuItem();
  mitem->SetId(id);
  mitem->SetText(text);
  mitem->SetHelpString(helpStr);
  mitem->SetSubMenu(subMenu);

  m_items.Append( mitem );
};

int wxMenu::FindItem( const wxString itemString ) const
{
  wxString s( itemString );

  int pos;
  do {
    pos = s.First( '&' );
    if (pos != -1) s.Remove( pos, 1 );
  } while (pos != -1);

  wxNode *node = m_items.First();
  while (node)
  {
    wxMenuItem *item = (wxMenuItem*)node->Data();
    if (item->GetText() == s)
      return item->GetId();
    node = node->Next();
  };

  return -1;
};

void wxMenu::Enable( int id, bool enable )
{
  wxMenuItem *item = FindItem(id);
  if ( item )
    item->Enable(enable);
};

bool wxMenu::IsEnabled( int id ) const
{
  wxMenuItem *item = FindItem(id);
  if ( item )
    return item->IsEnabled();
  else
    return FALSE;
};

void wxMenu::Check( int id, bool enable )
{
  wxMenuItem *item = FindItem(id);
  if ( item )
    item->Check(enable);
};

bool wxMenu::IsChecked( int id ) const
{
  wxMenuItem *item = FindItem(id);
  if ( item )
    return item->IsChecked();
  else
    return FALSE;
};

void wxMenu::SetLabel( int id, const wxString &label )
{
  wxMenuItem *item = FindItem(id);
  if ( item )
    item->SetText(label);
};

wxMenuItem *wxMenu::FindItem(int id) const
{
  wxNode *node = m_items.First();
  while (node) {
    wxMenuItem *item = (wxMenuItem*)node->Data();
    if ( item->GetId() == id )
      return item;
    node = node->Next();
  };

  wxLogDebug("wxMenu::FindItem: item %d not found.", id);

  return NULL;
}

void wxMenu::SetInvokingWindow( wxWindow *win )
{
  m_invokingWindow = win;
};

wxWindow *wxMenu::GetInvokingWindow()
{
  return m_invokingWindow;
};


