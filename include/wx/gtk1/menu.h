/////////////////////////////////////////////////////////////////////////////
// Name:        menu.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKMENUH__
#define __GTKMENUH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/list.h"
#include "wx/window.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxMenuBar;
class wxMenuItem;
class wxMenu;

//-----------------------------------------------------------------------------
// const
//-----------------------------------------------------------------------------

#define   ID_SEPARATOR    (-1)

//-----------------------------------------------------------------------------
// wxMenuBar
//-----------------------------------------------------------------------------

class wxMenuBar: public wxWindow
{
DECLARE_DYNAMIC_CLASS(wxMenuBar)

public:
  wxMenuBar();
  void Append( wxMenu *menu, const wxString &title );

  int FindMenuItem( const wxString &menuString, const wxString &itemString ) const;
  wxMenuItem* FindMenuItemById( int id ) const;

  int     GetMenuCount() const { return m_menus.Number(); }
  wxMenu *GetMenu(int n) const { return (wxMenu *)m_menus.Nth(n)->Data(); }

  bool IsChecked( int id ) const;
  bool IsEnabled( int id ) const;

  wxList       m_menus;
  GtkWidget   *m_menubar;
};

//-----------------------------------------------------------------------------
// wxMenu
//-----------------------------------------------------------------------------

class wxMenuItem: public wxObject
{
DECLARE_DYNAMIC_CLASS(wxMenuItem)

public:
  wxMenuItem();

  // accessors
    // id
  void SetId(int id) { m_id = id; }
  int  GetId() const { return m_id; }
  bool IsSeparator() const { return m_id == ID_SEPARATOR; }

    // the item's text
  void SetText(const wxString& str);
  const wxString& GetText() const { return m_text; }

    // what kind of menu item we are
  void SetCheckable(bool checkable) { m_isCheckMenu = checkable; }
  bool IsCheckable() const { return m_isCheckMenu; }
  void SetSubMenu(wxMenu *menu) { m_subMenu = menu; }
  wxMenu *GetSubMenu() const { return m_subMenu; }
  bool IsSubMenu() const { return m_subMenu != NULL; }

    // state
  void Enable(bool enable = TRUE) { m_isEnabled = enable; }
  bool IsEnabled() const { return m_isEnabled; }
  void Check(bool check = TRUE);
  bool IsChecked() const;

    // help string (displayed in the status bar by default)
  void SetHelpString(const wxString& str) { m_helpStr = str; }

  // implementation
  void SetMenuItem(GtkWidget *menuItem) { m_menuItem = menuItem; }
  GtkWidget *GetMenuItem() const { return m_menuItem; }

private:
  int           m_id;
  wxString      m_text;
  bool          m_isCheckMenu;
  bool          m_isChecked;
  bool          m_isEnabled;
  wxMenu       *m_subMenu;
  wxString      m_helpStr;

  GtkWidget    *m_menuItem;  // GtkMenuItem
};

class wxMenu: public wxEvtHandler
{
DECLARE_DYNAMIC_CLASS(wxMenu)

public:
  // construction
  wxMenu( const wxString &title = "" );

  // operations
    // menu creation
  void AppendSeparator();
  void Append(int id, const wxString &item,
              const wxString &helpStr = "", bool checkable = FALSE);
  void Append(int id, const wxString &item,
              wxMenu *subMenu, const wxString &helpStr = "" );
  void Break() {};

    // find item by name/id
  int FindItem( const wxString itemString ) const;
  wxMenuItem *FindItem(int id) const;

    // get/set item's state
  void Enable( int id, bool enable );
  bool IsEnabled( int id ) const;
  void Check( int id, bool check );
  bool IsChecked( int id ) const;

  void SetLabel( int id, const wxString &label );

  // accessors
  wxList& GetItems() { return m_items; }

public:
  int FindMenuIdByMenuItem( GtkWidget *menuItem ) const;
  void SetInvokingWindow( wxWindow *win );
  wxWindow *GetInvokingWindow();

  wxString    m_title;
  wxList      m_items;
  wxWindow   *m_invokingWindow;

  GtkWidget  *m_menu;  // GtkMenu
};

#endif // __GTKMENUH__
