/////////////////////////////////////////////////////////////////////////////
// Name:        menu.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart
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
#include "wx/menuitem.h"

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
  
  void Check( int id, bool check );
  bool Checked( int id ) const;
  void Enable( int id, bool enable );
  bool Enabled( int id ) const;
  inline bool IsEnabled(int Id) const { return Enabled(Id); };
  inline bool IsChecked(int Id) const { return Checked(Id); };

  int     GetMenuCount() const { return m_menus.Number(); }
  wxMenu *GetMenu(int n) const { return (wxMenu *)m_menus.Nth(n)->Data(); }

  wxList       m_menus;
  GtkWidget   *m_menubar;
};

//-----------------------------------------------------------------------------
// wxMenu
//-----------------------------------------------------------------------------

class wxMenu: public wxEvtHandler
{
DECLARE_DYNAMIC_CLASS(wxMenu)

public:
  // construction
  wxMenu( const wxString& title = wxEmptyString, const wxFunction func = (wxFunction) NULL );

  // operations
    // title
  void SetTitle(const wxString& label);
  const wxString GetTitle() const;
    // menu creation
  void AppendSeparator();
  void Append(int id, const wxString &item,
              const wxString &helpStr = "", bool checkable = FALSE);
  void Append(int id, const wxString &item,
              wxMenu *subMenu, const wxString &helpStr = "" );
  void Break() {};

    // find item by name/id
  int FindItem( const wxString itemString ) const;
  wxMenuItem *FindItem( int id ) const;
  wxMenuItem *FindItemForId( int id ) const { return FindItem( id ); }

    // get/set item's state
  void Enable( int id, bool enable );
  bool IsEnabled( int id ) const;
  void Check( int id, bool check );
  bool IsChecked( int id ) const;

  void SetLabel( int id, const wxString &label );
  wxString GetLabel(int id) const;

  // helpstring
  virtual void SetHelpString(int id, const wxString& helpString);
  virtual wxString GetHelpString(int id) const ;
  
  // accessors
  wxList& GetItems() { return m_items; }

  inline void Callback(const wxFunction func) { m_callback = func; }
  
  inline void SetEventHandler(wxEvtHandler *handler) { m_eventHandler = handler; }
  inline wxEvtHandler *GetEventHandler() { return m_eventHandler; }
  
  inline void SetClientData( void* clientData ) { m_clientData = clientData; }
  inline void* GetClientData() const { return m_clientData; }
  
// implementation
  
  int FindMenuIdByMenuItem( GtkWidget *menuItem ) const;
  void SetInvokingWindow( wxWindow *win );
  wxWindow *GetInvokingWindow();

  wxString       m_title;
  wxList         m_items;
  wxWindow      *m_invokingWindow;
  wxFunction     m_callback;
  wxEvtHandler  *m_eventHandler;
  void          *m_clientData;

  GtkWidget     *m_menu;  // GtkMenu
};

#endif // __GTKMENUH__
