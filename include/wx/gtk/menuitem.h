///////////////////////////////////////////////////////////////////////////////
// Name:        menuitem.h
// Purpose:     wxMenuItem class
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef __GTKMENUITEMH__
#define __GTKMENUITEMH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/string.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

#define   ID_SEPARATOR    (-1)

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxMenuItem;

class wxMenu;

//-----------------------------------------------------------------------------
// wxMenuItem
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

    // the item's text = name
  void SetName(const wxString& str);
  const wxString& GetName() const { return m_text; }

    // what kind of menu item we are
  void SetCheckable(bool checkable) { m_isCheckMenu = checkable; }
  bool IsCheckable() const { return m_isCheckMenu; }
  void SetSubMenu(wxMenu *menu) { m_subMenu = menu; }
  wxMenu *GetSubMenu() const { return m_subMenu; }
  bool IsSubMenu() const { return m_subMenu != NULL; }

    // state
  void Enable( bool enable = TRUE ); 
  bool IsEnabled() const { return m_isEnabled; }
  void Check( bool check = TRUE );
  bool IsChecked() const;

    // help string (displayed in the status bar by default)
  void SetHelp(const wxString& str) { m_helpStr = str; }
  const wxString& GetHelp() const { return m_helpStr; }

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


#endif  
        //__GTKMENUITEMH__
