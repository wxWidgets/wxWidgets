/////////////////////////////////////////////////////////////////////////////
// Name:        menu.cpp
// Purpose:     wxMenu, wxMenuBar, wxMenuItem
// Author:      Julian Smart
// Modified by: Vadim Zeitlin
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "menu.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/frame.h"
#include "wx/menu.h"
#include "wx/utils.h"
#endif

#if wxUSE_OWNER_DRAWN
  #include "wx/ownerdrw.h"
#endif

#include "wx/msw/private.h"
#include "wx/msw/menu.h"
#include "wx/menuitem.h"
#include "wx/log.h"

// other standard headers
// ----------------------
#include <string.h>

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// the (popup) menu title has this special id
static const int idMenuTitle = -2;

// ----------------------------------------------------------------------------
// wxWindows macros
// ----------------------------------------------------------------------------
#if !USE_SHARED_LIBRARY
  IMPLEMENT_DYNAMIC_CLASS(wxMenu, wxEvtHandler)
  IMPLEMENT_DYNAMIC_CLASS(wxMenuBar, wxEvtHandler)
#endif

// ============================================================================
// implementation
// ============================================================================

// Menus

// Construct a menu with optional title (then use append)
wxMenu::wxMenu(const wxString& Title, const wxFunction func)
{
  m_title = Title;
  m_parent = NULL;
  m_eventHandler = this;
  m_pInvokingWindow = NULL;
  m_doBreak = FALSE ;
  m_noItems = 0;
  m_menuBar = NULL;
  m_hMenu = (WXHMENU) CreatePopupMenu();
  m_savehMenu = 0 ;
  m_topLevelMenu = this;
  m_clientData = (void*) NULL;

  if (m_title != "")
  {
    Append(idMenuTitle, m_title) ;
    AppendSeparator() ;
  }

  Callback(func);
}

// The wxWindow destructor will take care of deleting the submenus.
wxMenu::~wxMenu()
{
  if (m_hMenu)
    DestroyMenu((HMENU) m_hMenu);
  m_hMenu = 0;

  // Windows seems really bad on Menu de-allocation...
  // After many try, here is what I do: RemoveMenu() will ensure
  // that popup are "disconnected" from their parent; then call
  // delete method on each child (which in turn do a recursive job),
  // and finally, DestroyMenu()
  //
  // With that, BoundCheckers is happy, and no complaints...
/*
  int N = 0 ;
  if (m_hMenu)
    N = GetMenuItemCount(m_hMenu);
  int i;
  for (i = N-1; i >= 0; i--)
    RemoveMenu(m_hMenu, i, MF_BYPOSITION);
*/

  // How is deleting submenus in this loop any different from deleting
  // the submenus in the children list, via ~wxWindow ?
  // I'll reinstate this deletion for now and remove addition
  // from children list (which doesn't exist now)
  // Julian 1/3/97
  wxNode *node = m_menuItems.First();
  while (node)
  {
    wxMenuItem *item = (wxMenuItem *)node->Data();

    // Delete child menus.
    // Beware: they must not be appended to children list!!!
    // (because order of delete is significant)
    if (item->GetSubMenu())
      item->DeleteSubMenu();

    wxNode *next = node->Next();
    delete item;
    delete node;
    node = next;
  }
/*
  if (m_hMenu)
    DestroyMenu(m_hMenu);
  m_hMenu = 0;
*/
}

void wxMenu::Break()
{
  m_doBreak = TRUE ;
}

// function appends a new item or submenu to the menu
void wxMenu::Append(wxMenuItem *pItem)
{
  wxCHECK_RET( pItem != NULL, "can't append NULL item to the menu" );

  m_menuItems.Append(pItem);

  UINT  flags = 0;
  
  if ( m_doBreak ) {
    flags |= MF_MENUBREAK;
    m_doBreak = FALSE;
  }

  if ( pItem->IsSeparator() ) {
    flags |= MF_SEPARATOR;
  }

  // id is the numeric id for normal menu items and HMENU for submenus
  UINT id;
  wxMenu *SubMenu = pItem->GetSubMenu();
  if ( SubMenu != NULL ) {
    wxASSERT( SubMenu->m_hMenu != (WXHMENU) NULL );

    id = (UINT)SubMenu->m_hMenu;

    SubMenu->m_topLevelMenu = m_topLevelMenu;
    SubMenu->m_parent       = this;
    SubMenu->m_savehMenu    = (WXHMENU)id;
    SubMenu->m_hMenu        = 0;

    flags |= MF_POPUP;
  }
  else {
    id = pItem->GetId();
  }

  LPCSTR pData;

#if wxUSE_OWNER_DRAWN
  if ( pItem->IsOwnerDrawn() ) {  // want to get {Measure|Draw}Item messages?
    // item draws itself, pass pointer to it in data parameter
    flags |= MF_OWNERDRAW;
    pData = (LPCSTR)pItem;
  }
  else
#endif
  {
    // menu is just a normal string (passed in data parameter)
    flags |= MF_STRING;
    pData = pItem->GetName();
  }

  // visually select the menu title
  if ( id == idMenuTitle )
  {
    // TODO use SetMenuItemInfo(MFS_DEFAULT) to put it in bold face
  }

  HMENU hMenu = (HMENU)((m_hMenu == 0) ? m_savehMenu : m_hMenu);
  if ( !AppendMenu(hMenu, flags, id, pData) )
  {
    wxLogLastError("AppendMenu");
  }

  m_noItems++;
}

void wxMenu::AppendSeparator()
{
  Append(new wxMenuItem(this, ID_SEPARATOR));
}

// Pullright item
void wxMenu::Append(int Id, const wxString& label, wxMenu *SubMenu, 
                    const wxString& helpString)
{
  Append(new wxMenuItem(this, Id, label, helpString, FALSE, SubMenu));
}

// Ordinary menu item
void wxMenu::Append(int Id, const wxString& label, 
                    const wxString& helpString, bool checkable)
{
  // 'checkable' parameter is useless for Windows.
  Append(new wxMenuItem(this, Id, label, helpString, checkable));
}

void wxMenu::Delete(int id)
{
  wxNode *node;
  int pos;
  HMENU menu;

  wxMenuItem *item = NULL;
  for (pos = 0, node = m_menuItems.First(); node; node = node->Next(), pos++) {
	 item = (wxMenuItem *)node->Data();
	 if (item->GetId() == id)
		break;
  }

  if (!node)
	return;

  menu = (HMENU)(m_hMenu ? m_hMenu : m_savehMenu);

  wxMenu *pSubMenu = item->GetSubMenu();
  if ( pSubMenu != NULL ) {
    RemoveMenu(menu, (UINT)pos, MF_BYPOSITION);
    pSubMenu->m_hMenu = pSubMenu->m_savehMenu;
    pSubMenu->m_savehMenu = 0;
    pSubMenu->m_parent = NULL;
    //     RemoveChild(item->subMenu);
    pSubMenu->m_topLevelMenu = NULL;
    // TODO: Why isn't subMenu deleted here???
    // Will put this in for now. Assuming this is supposed
    // to delete the menu, not just remove it.
    item->DeleteSubMenu();
  }
  else {
    DeleteMenu(menu, (UINT)pos, MF_BYPOSITION);
  }

  m_menuItems.DeleteNode(node);
  delete item;
}

void wxMenu::Enable(int Id, bool Flag)
{
  wxMenuItem *item = FindItemForId(Id);
  wxCHECK_RET( item != NULL, "can't enable non-existing menu item" );

  item->Enable(Flag);
}

bool wxMenu::Enabled(int Id) const
{
  wxMenuItem *item = FindItemForId(Id);
  wxCHECK( item != NULL, FALSE );

  return item->IsEnabled();
}

void wxMenu::Check(int Id, bool Flag)
{
  wxMenuItem *item = FindItemForId(Id);
  wxCHECK_RET( item != NULL, "can't get status of non-existing menu item" );

  item->Check(Flag);
}

bool wxMenu::Checked(int Id) const
{
  wxMenuItem *item = FindItemForId(Id);
  wxCHECK( item != NULL, FALSE );

  return item->IsChecked();
}

void wxMenu::SetTitle(const wxString& label)
{
  bool hasNoTitle = m_title.IsEmpty();
  m_title = label;

  HMENU hMenu = (HMENU)((m_hMenu == 0) ? m_savehMenu : m_hMenu);

  if ( hasNoTitle )
  {
    if ( !label.IsEmpty() )
    {
      if ( !InsertMenu(hMenu, 0u, MF_BYPOSITION | MF_STRING,
                       (unsigned)idMenuTitle, m_title) ||
           !InsertMenu(hMenu, 1u, MF_BYPOSITION, (unsigned)-1, NULL) )
      {
        wxLogLastError("InsertMenu");
      }
    }
  }
  else
  {
    if ( label.IsEmpty() )
    {
      // remove the title and the separator after it
      if ( !RemoveMenu(hMenu, 0, MF_BYPOSITION) ||
           !RemoveMenu(hMenu, 0, MF_BYPOSITION) )
      {
        wxLogLastError("RemoveMenu");
      }
    }
    else
    {
      // modify the title
      if ( !ModifyMenu(hMenu, 0u,
                       MF_BYPOSITION | MF_STRING,
                       (unsigned)idMenuTitle, m_title) )
      {
        wxLogLastError("ModifyMenu");
      }
    }
  }

  // put the title string in bold face
  if ( !m_title.IsEmpty() )
  {
    MENUITEMINFO mii;
    mii.cbSize = sizeof(mii);
    mii.fMask = MIIM_STATE;
    mii.fState = MFS_DEFAULT;

    if ( !SetMenuItemInfo(hMenu, (unsigned)idMenuTitle, FALSE, &mii) )
    {
      wxLogLastError("SetMenuItemInfo");
    }
  }
}

const wxString wxMenu::GetTitle() const
{
   return m_title;
}

void wxMenu::SetLabel(int Id, const wxString& label)
{
  wxMenuItem *item = FindItemForId(Id) ;
  if (item==NULL)
    return;

  if (item->GetSubMenu()==NULL)
  {
    if (m_hMenu)
    {
      UINT was_flag = GetMenuState((HMENU)m_hMenu,Id,MF_BYCOMMAND) ;
      ModifyMenu((HMENU)m_hMenu,Id,MF_BYCOMMAND|MF_STRING|was_flag,Id,(const char *)label) ;
    }
    else if (m_savehMenu)
    {
      UINT was_flag = GetMenuState((HMENU)m_savehMenu,Id,MF_BYCOMMAND) ;
      ModifyMenu((HMENU)m_savehMenu,Id,MF_BYCOMMAND|MF_STRING|was_flag,Id,(const char *)label) ;
    }
  }
  else
  {
    wxMenu *father = item->GetSubMenu()->m_topLevelMenu ;
    wxNode *node = father->m_menuItems.First() ;
    int i = 0 ;
    while (node)
    {
      wxMenuItem *matched = (wxMenuItem*)node->Data() ;
      if (matched==item)
        break ;
      i++ ;
      node = node->Next() ;
    }
    // Here, we have the position.
    ModifyMenu((HMENU)father->m_savehMenu,i,
               MF_BYPOSITION|MF_STRING|MF_POPUP,
               (UINT)item->GetSubMenu()->m_savehMenu,(const char *)label) ;
  }
  item->SetName(label);
}

wxString wxMenu::GetLabel(int id) const
{
/*
  static char tmp[128] ;
  int len;
  if (m_hMenu)
    len = GetMenuString((HMENU)m_hMenu,Id,tmp,WXSIZEOF(tmp) - 1,MF_BYCOMMAND);
  else if (m_savehMenu)
    len = GetMenuString((HMENU)m_savehMenu,Id,tmp,WXSIZEOF(tmp) - 1,MF_BYCOMMAND);
  else
    len = 0 ;
  tmp[len] = '\0' ;
  return wxString(tmp) ;

*/
    wxMenuItem *pItem = FindItemForId(id) ;
    if (pItem)
        return pItem->GetName() ;
    else
        return wxEmptyString;
}

bool wxMenu::MSWCommand(WXUINT WXUNUSED(param), WXWORD id)
{
    // ignore commands from the menu title

    // NB: VC++ generates wrong assembler for `if ( id != idMenuTitle )'!!
    if ( id != (WXWORD)idMenuTitle )
    {
        wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED);
        event.SetEventObject( this );
        event.SetId( id );
        event.SetInt( id );
        ProcessCommand(event);
    }

    return TRUE;
}

// Finds the item id matching the given string, -1 if not found.
int wxMenu::FindItem (const wxString& itemString) const
{
  char buf1[200];
  char buf2[200];
  wxStripMenuCodes ((char *)(const char *)itemString, buf1);

  for (wxNode * node = m_menuItems.First (); node; node = node->Next ())
    {
      wxMenuItem *item = (wxMenuItem *) node->Data ();
      if (item->GetSubMenu())
      {
        int ans = item->GetSubMenu()->FindItem(itemString);
        if (ans > -1)
          return ans;
      }
      if ( !item->IsSeparator() )
      {
        wxStripMenuCodes((char *)item->GetName().c_str(), buf2);
        if (strcmp(buf1, buf2) == 0)
          return item->GetId();
      }
    }

  return -1;
}

wxMenuItem *wxMenu::FindItemForId(int itemId, wxMenu ** itemMenu) const
{
  if (itemMenu)
    *itemMenu = NULL;
  for (wxNode * node = m_menuItems.First (); node; node = node->Next ())
  {
    wxMenuItem *item = (wxMenuItem *) node->Data ();

    if (item->GetId() == itemId)
    {
      if (itemMenu)
        *itemMenu = (wxMenu *) this;
      return item;
    }

    if (item->GetSubMenu())
    {
      wxMenuItem *ans = item->GetSubMenu()->FindItemForId (itemId, itemMenu);
      if (ans)
        return ans;
    }
  }

  if (itemMenu)
    *itemMenu = NULL;
  return NULL;
}

void wxMenu::SetHelpString(int itemId, const wxString& helpString)
{
  wxMenuItem *item = FindItemForId (itemId);
  if (item)
    item->SetHelp(helpString);
}

wxString wxMenu::GetHelpString (int itemId) const
{
  wxMenuItem *item = FindItemForId (itemId);
  if (item)
    return item->GetHelp();
  else
    return wxEmptyString;
}

void wxMenu::ProcessCommand(wxCommandEvent & event)
{
  bool processed = FALSE;

  // Try a callback
  if (m_callback)
  {
      (void)(*(m_callback))(*this, event);
      processed = TRUE;
  }

  // Try the menu's event handler
  if ( !processed && GetEventHandler())
  {
      processed = GetEventHandler()->ProcessEvent(event);
  }

  // Try the window the menu was popped up from (and up
  // through the hierarchy)
  if ( !processed && GetInvokingWindow())
    processed = GetInvokingWindow()->GetEventHandler()->ProcessEvent(event);
}

extern wxMenu *wxCurrentPopupMenu;
bool wxWindow::PopupMenu(wxMenu *menu, int x, int y)
{
  menu->SetInvokingWindow(this);
  menu->UpdateUI();

  HWND hWnd = (HWND) GetHWND();
  HMENU hMenu = (HMENU)menu->m_hMenu;
  POINT point;
  point.x = x;
  point.y = y;
  ::ClientToScreen(hWnd, &point);
  wxCurrentPopupMenu = menu;
  ::TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, point.x, point.y, 0, hWnd, NULL);
  wxYield();
  wxCurrentPopupMenu = NULL;

  menu->SetInvokingWindow(NULL);

  return TRUE;
}

// Menu Bar
wxMenuBar::wxMenuBar()
{
  m_eventHandler = this;

  m_menuCount = 0;
  m_menus = NULL;
  m_titles = NULL;
  m_menuBarFrame = NULL;
  m_hMenu = 0;
}

wxMenuBar::wxMenuBar(int N, wxMenu *Menus[], const wxString Titles[])
{
  m_eventHandler = this;
  m_menuCount = N;
  m_menus = Menus;
  m_titles = new wxString[N];
  int i;
  for ( i = 0; i < N; i++ )
	m_titles[i] = Titles[i];
  m_menuBarFrame = NULL;
  for (i = 0; i < N; i++)
    m_menus[i]->m_menuBar = (wxMenuBar *) this;

  m_hMenu = 0;
}

wxMenuBar::~wxMenuBar()
{
  // In fact, don't want menu to be destroyed before MDI
  // shuffling has taken place. Let it be destroyed
  // automatically when the window is destroyed.

//  DestroyMenu(menu);
//  m_hMenu = NULL;

  int i;
/*
  // See remarks in ::~wxMenu() method
  // BEWARE - this may interfere with MDI fixes, so
  // may need to remove
  int N = 0 ;

  if (m_menuBarFrame && ((m_menuBarFrame->GetWindowStyleFlag() & wxSDI) == wxSDI))
  {
    if (menu)
      N = GetMenuItemCount(menu) ;
    for (i = N-1; i >= 0; i--)
      RemoveMenu(menu, i, MF_BYPOSITION);
  }
*/
  for (i = 0; i < m_menuCount; i++)
  {
    delete m_menus[i];
  }
  delete[] m_menus;
  delete[] m_titles;

/* Don't destroy menu here, in case we're MDI and
   need to do some shuffling with VALID menu handles.
  if (menu)
    DestroyMenu(menu);
  m_hMenu = 0;
*/
}

// Must only be used AFTER menu has been attached to frame,
// otherwise use individual menus to enable/disable items
void wxMenuBar::Enable(int Id, bool Flag)
{
  int ms_flag;
  if (Flag)
    ms_flag = MF_ENABLED;
  else
    ms_flag = MF_GRAYED;

  wxMenu *itemMenu = NULL;
  wxMenuItem *item = FindItemForId(Id, &itemMenu) ;
  if (!item)
    return;

  if (itemMenu->m_hMenu)
    EnableMenuItem((HMENU)itemMenu->m_hMenu, Id, MF_BYCOMMAND | ms_flag);
  else if (itemMenu->m_savehMenu)
    EnableMenuItem((HMENU)itemMenu->m_savehMenu, Id, MF_BYCOMMAND | ms_flag);

}

void wxMenuBar::EnableTop(int pos, bool flag)
{
  int ms_flag;
  if (flag)
    ms_flag = MF_ENABLED;
  else
    ms_flag = MF_GRAYED;

  EnableMenuItem((HMENU)m_hMenu, pos, MF_BYPOSITION | ms_flag);
  DrawMenuBar((HWND) m_menuBarFrame->GetHWND()) ;
}

// Must only be used AFTER menu has been attached to frame,
// otherwise use individual menus
void wxMenuBar::Check(int Id, bool Flag)
{
  wxMenu *itemMenu = NULL;
  wxMenuItem *item = FindItemForId(Id, &itemMenu) ;
  if (!item)
    return;
    
  if (!item->IsCheckable())
    return ;
  int ms_flag;
  if (Flag)
    ms_flag = MF_CHECKED;
  else
    ms_flag = MF_UNCHECKED;

  if (itemMenu->m_hMenu)
    CheckMenuItem((HMENU)itemMenu->m_hMenu, Id, MF_BYCOMMAND | ms_flag);
  else if (itemMenu->m_savehMenu)
    CheckMenuItem((HMENU)itemMenu->m_savehMenu, Id, MF_BYCOMMAND | ms_flag);

//  CheckMenuItem((HMENU)m_hMenu, Id, MF_BYCOMMAND | ms_flag);
}

bool wxMenuBar::Checked(int Id) const
{
  wxMenu *itemMenu = NULL;
  wxMenuItem *item = FindItemForId(Id, &itemMenu) ;
  if (!item)
    return FALSE;

  int Flag = 0;

  if (itemMenu->m_hMenu)
    Flag=GetMenuState((HMENU)itemMenu->m_hMenu, Id, MF_BYCOMMAND) ;
  else if (itemMenu->m_savehMenu)
    Flag=GetMenuState((HMENU)itemMenu->m_savehMenu, Id, MF_BYCOMMAND) ;

//  Flag=GetMenuState((HMENU)m_hMenu, Id, MF_BYCOMMAND) ;

  if (Flag&MF_CHECKED)
    return TRUE ;
  else
    return FALSE ;
}

bool wxMenuBar::Enabled(int Id) const
{
  wxMenu *itemMenu = NULL;
  wxMenuItem *item = FindItemForId(Id, &itemMenu) ;
  if (!item)
    return FALSE;

  int Flag ;

  if (itemMenu->m_hMenu)
    Flag=GetMenuState((HMENU)itemMenu->m_hMenu, Id, MF_BYCOMMAND) ;
  else if (itemMenu->m_savehMenu)
    Flag=GetMenuState((HMENU)itemMenu->m_savehMenu, Id, MF_BYCOMMAND) ;

  if (Flag&MF_ENABLED)
    return TRUE ;
  else
    return FALSE ;
}


void wxMenuBar::SetLabel(int Id, const wxString& label)
{
  wxMenu *itemMenu = NULL;
  wxMenuItem *item = FindItemForId(Id, &itemMenu) ;

  if (!item)
    return;

  if (itemMenu->m_hMenu)
  {
    UINT was_flag = GetMenuState((HMENU)itemMenu->m_hMenu,Id,MF_BYCOMMAND) ;
    ModifyMenu((HMENU)itemMenu->m_hMenu,Id,MF_BYCOMMAND|MF_STRING|was_flag,Id,(const char *)label) ;
  }
  else if (itemMenu->m_savehMenu)
  {
    UINT was_flag = GetMenuState((HMENU)itemMenu->m_savehMenu,Id,MF_BYCOMMAND) ;
    ModifyMenu((HMENU)itemMenu->m_savehMenu,Id,MF_BYCOMMAND|MF_STRING|was_flag,Id,(const char *)label) ;
  }
}

wxString wxMenuBar::GetLabel(int Id) const
{
  wxMenu *itemMenu = NULL;
  wxMenuItem *item = FindItemForId(Id, &itemMenu) ;

  if (!item)
    return wxString("");

  static char tmp[128] ;
  int len = 0;
  if (itemMenu->m_hMenu)
  {
    len = GetMenuString((HMENU)itemMenu->m_hMenu,Id,tmp,127,MF_BYCOMMAND) ;
  }
  else if (itemMenu->m_savehMenu)
  {
    len = GetMenuString((HMENU)itemMenu->m_savehMenu,Id,tmp,127,MF_BYCOMMAND) ;
  }

//  int len = GetMenuString((HMENU)m_hMenu,Id,tmp,127,MF_BYCOMMAND) ;
  tmp[len] = '\0' ;
  return wxString(tmp) ;
}

void wxMenuBar::SetLabelTop(int pos, const wxString& label)
{
  UINT was_flag = GetMenuState((HMENU)m_hMenu,pos,MF_BYPOSITION) ;
  if (was_flag&MF_POPUP)
  {
    was_flag &= 0xff ;
    HMENU popup = GetSubMenu((HMENU)m_hMenu,pos) ;
    ModifyMenu((HMENU)m_hMenu,pos,MF_BYPOSITION|MF_STRING|was_flag,(UINT)popup,(const char *)label) ;
  }
  else
    ModifyMenu((HMENU)m_hMenu,pos,MF_BYPOSITION|MF_STRING|was_flag,pos,(const char *)label) ;
}

wxString wxMenuBar::GetLabelTop(int pos) const
{
  static char tmp[128] ;
  int len = GetMenuString((HMENU)m_hMenu,pos,tmp,127,MF_BYPOSITION) ;
  tmp[len] = '\0' ;
  return wxString(tmp);
}

bool wxMenuBar::OnDelete(wxMenu *a_menu, int pos)
{
  if (!m_menuBarFrame)
	 return TRUE;

  if (RemoveMenu((HMENU)m_hMenu, (UINT)pos, MF_BYPOSITION)) {
	 m_menus[pos]->m_hMenu = m_menus[pos]->m_savehMenu;
	 m_menus[pos]->m_savehMenu = 0;

	 if (m_menuBarFrame) {
		DrawMenuBar((HWND) m_menuBarFrame->GetHWND()) ;
	 }

	 return TRUE;
  }

  return FALSE;
}

bool wxMenuBar::OnAppend(wxMenu *a_menu, const char *title)
{
  if (!a_menu->m_hMenu)
	 return FALSE;

  if (!m_menuBarFrame)
	 return TRUE;

  a_menu->m_savehMenu = a_menu->m_hMenu;
  a_menu->m_hMenu = 0;

  AppendMenu((HMENU)m_hMenu, MF_POPUP | MF_STRING, (UINT)a_menu->m_savehMenu, title);

  DrawMenuBar((HWND) m_menuBarFrame->GetHWND());

  return TRUE;
}

void wxMenuBar::Append (wxMenu * menu, const wxString& title)
{
  if (!OnAppend(menu, title))
	 return;

  m_menuCount ++;
  wxMenu **new_menus = new wxMenu *[m_menuCount];
  wxString *new_titles = new wxString[m_menuCount];
  int i;

  for (i = 0; i < m_menuCount - 1; i++)
	 {
		new_menus[i] = m_menus[i];
		m_menus[i] = NULL;
		new_titles[i] = m_titles[i];
		m_titles[i] = "";
	 }
  if (m_menus)
	 {
		delete[]m_menus;
		delete[]m_titles;
	 }
  m_menus = new_menus;
  m_titles = new_titles;

  m_menus[m_menuCount - 1] = (wxMenu *)menu;
  m_titles[m_menuCount - 1] = title;

  ((wxMenu *)menu)->m_menuBar = (wxMenuBar *) this;
  ((wxMenu *)menu)->SetParent(this);
}

void wxMenuBar::Delete(wxMenu * menu, int i)
{
  int j;
  int ii = (int) i;

  if (menu != 0) {
	 for (ii = 0; ii < m_menuCount; ii++) {
		if (m_menus[ii] == menu)
	break;
	 }
	 if (ii >= m_menuCount)
		return;
  } else {
	 if (ii < 0 || ii >= m_menuCount)
		return;
	 menu = m_menus[ii];
  }

  if (!OnDelete(menu, ii))
	 return;

  menu->SetParent(NULL);

  -- m_menuCount;
  for (j = ii; j < m_menuCount; j++) {
	 m_menus[j] = m_menus[j + 1];
	 m_titles[j] = m_titles[j + 1];
  }
}

// Find the menu menuString, item itemString, and return the item id.
// Returns -1 if none found.
int wxMenuBar::FindMenuItem (const wxString& menuString, const wxString& itemString) const
{
  char buf1[200];
  char buf2[200];
  wxStripMenuCodes ((char *)(const char *)menuString, buf1);
  int i;
  for (i = 0; i < m_menuCount; i++)
    {
      wxStripMenuCodes ((char *)(const char *)m_titles[i], buf2);
      if (strcmp (buf1, buf2) == 0)
	return m_menus[i]->FindItem (itemString);
    }
  return -1;
}

wxMenuItem *wxMenuBar::FindItemForId (int Id, wxMenu ** itemMenu) const
{
  if (itemMenu)
    *itemMenu = NULL;

  wxMenuItem *item = NULL;
  int i;
  for (i = 0; i < m_menuCount; i++)
  {
    item = m_menus[i]->FindItemForId (Id, itemMenu);
    if (item)
      return item;
  }
  return NULL;
}

void wxMenuBar::SetHelpString (int Id, const wxString& helpString)
{
  int i;
  for (i = 0; i < m_menuCount; i++)
    {
      if (m_menus[i]->FindItemForId (Id))
	{
	  m_menus[i]->SetHelpString (Id, helpString);
	  return;
	}
    }
}

wxString wxMenuBar::GetHelpString (int Id) const
{
  int i;
  for (i = 0; i < m_menuCount; i++)
    {
      if (m_menus[i]->FindItemForId (Id))
	    return wxString(m_menus[i]->GetHelpString (Id));
    }
  return wxString("");
}

wxWindow *wxMenu::GetWindow() const
{ 
  if ( m_pInvokingWindow != NULL )
    return m_pInvokingWindow;
  if ( m_menuBar != NULL)       
    return m_menuBar->m_menuBarFrame;
  return NULL;
}

WXHMENU wxMenu::GetHMenu() const
{
  if ( m_hMenu != 0 )
    return m_hMenu;
  else if ( m_savehMenu != 0 )
    return m_savehMenu;

  return 0;
}

// Update a menu and all submenus recursively.
// source is the object that has the update event handlers
// defined for it. If NULL, the menu or associated window
// will be used.
void wxMenu::UpdateUI(wxEvtHandler* source)
{
  if (!source && GetInvokingWindow())
    source = GetInvokingWindow()->GetEventHandler();
  if (!source)
    source = GetEventHandler();
  if (!source)
    source = this;

  wxNode* node = GetItems().First();
  while (node)
  {
    wxMenuItem* item = (wxMenuItem*) node->Data();
    if ( !item->IsSeparator() )
    {
      wxWindowID id = item->GetId();
      wxUpdateUIEvent event(id);
      event.SetEventObject( source );

      if (source->ProcessEvent(event))
      {
        if (event.GetSetText())
          SetLabel(id, event.GetText());
        if (event.GetSetChecked())
          Check(id, event.GetChecked());
        if (event.GetSetEnabled())
          Enable(id, event.GetEnabled());
      }

      if (item->GetSubMenu())
        item->GetSubMenu()->UpdateUI(source);
    }
    node = node->Next();
  }
}
