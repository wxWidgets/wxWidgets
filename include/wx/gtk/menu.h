/////////////////////////////////////////////////////////////////////////////
// Name:        menu.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
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
// wxMenuBar
//-----------------------------------------------------------------------------

class wxMenuBar: public wxWindow
{
  DECLARE_DYNAMIC_CLASS(wxMenuBar)

  public:

    wxMenuBar(void);
    void Append( wxMenu *menu, const wxString &title );
    int FindMenuItem( const wxString &menuString, const wxString &itemString ) const;
    
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
  
    wxMenuItem(void);
  
    int           m_id;
    wxString      m_text;
    bool          m_isCheckMenu;
    bool          m_checked;
    bool          m_isSubMenu;
    bool          m_isEnabled;
    wxMenu       *m_subMenu;
    wxString      m_helpStr;
    
    GtkWidget    *m_menuItem;  // GtkMenuItem
  
};

class wxMenu: public wxEvtHandler
{
  DECLARE_DYNAMIC_CLASS(wxMenu)

  public:

    wxMenu( const wxString &title = "" );
    void AppendSeparator(void);
    void Append( const int id, const wxString &item, 
      const wxString &helpStr = "", const bool checkable = FALSE );
    void Append( const int id, const wxString &item, 
      wxMenu *subMenu, const wxString &helpStr = "" );
    int FindItem( const wxString itemString ) const;
    void Break(void) {};
    void Enable( const int id, const bool enable );
    bool Enabled( const int id ) const;
    void SetLabel( const int id, const wxString &label );
      
  public:
      
    int FindMenuIdByMenuItem( GtkWidget *menuItem ) const;
    void SetInvokingWindow( wxWindow *win );
    wxWindow *GetInvokingWindow(void);

    wxString    m_title;
    wxList      m_items;
    wxWindow   *m_invokingWindow;
    
    GtkWidget  *m_menu;  // GtkMenu
      
};

#endif // __GTKMENUH__
