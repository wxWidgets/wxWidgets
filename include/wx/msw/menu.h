/////////////////////////////////////////////////////////////////////////////
// Name:        menu.h
// Purpose:     wxMenu, wxMenuBar classes
// Author:      Julian Smart
// Modified by: Vadim Zeitlin (wxMenuItem is now in separate file)
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __MENUH__
#define __MENUH__

#ifdef __GNUG__
#pragma interface "menu.h"
#endif

#include "wx/defs.h"
#include "wx/event.h"

class wxMenuItem;
class wxMenuBar;
class wxMenu;

WXDLLEXPORT_DATA(extern const char*) wxEmptyString;

// ----------------------------------------------------------------------------
// Menu
// ----------------------------------------------------------------------------
class WXDLLEXPORT wxMenu: public wxEvtHandler
{
  DECLARE_DYNAMIC_CLASS(wxMenu)

public:
  // ctor & dtor
  wxMenu(const wxString& title = wxEmptyString, const wxFunction func = NULL);
  ~wxMenu();

  // construct menu
    // append items to the menu
      // separator line
  void AppendSeparator();
      // normal item
  void Append(int id, const wxString& Label, const wxString& helpString = wxEmptyString,
              bool checkable = FALSE);
      // a submenu
  void Append(int id, const wxString& Label, wxMenu *SubMenu, 
              const wxString& helpString = wxEmptyString);
      // the most generic form (create wxMenuItem first and use it's functions)
  void Append(wxMenuItem *pItem);
    // insert a break in the menu
  void Break();
    // delete an item
  void Delete(int id); /* If it's a submenu, menu is not destroyed. VZ: why? */

  // menu item control
  void Enable(int id, bool Flag);
  bool Enabled(int id) const;
  inline bool IsEnabled(int id) const { return Enabled(id); };
  void Check(int id, bool Flag);
  bool Checked(int id) const;
  inline bool IsChecked(int id) const { return IsChecked(id); };

  // item properties
    // title
  void SetTitle(const wxString& label);
  const wxString& GetTitle() const;
    // label
  void SetLabel(int id, const wxString& label);
  wxString GetLabel(int id) const;
    // help string
  virtual void SetHelpString(const int id, const wxString& helpString);
  virtual wxString GetHelpString(const int id) const ;

  // find item
    // Finds the item id matching the given string, NOT_FOUND if not found.
  virtual int FindItem(const wxString& itemString) const ;
    // Find wxMenuItem by ID, and item's menu too if itemMenu is !NULL.
  wxMenuItem *FindItemForId(const int itemId, wxMenu **itemMenu = NULL) const;

  void ProcessCommand(wxCommandEvent& event);
  inline void Callback(const wxFunction func) { m_callback = func; }

  virtual void SetParent(wxEvtHandler *parent) { m_parent = parent; }
  inline void SetEventHandler(wxEvtHandler *handler) { m_eventHandler = handler; }
  inline wxEvtHandler *GetEventHandler(void) { return m_eventHandler; }

  // IMPLEMENTATION
  bool MSWCommand(const WXUINT param, const WXWORD id);

  void      SetInvokingWindow(wxWindow *pWin) { m_pInvokingWindow = pWin; }
  wxWindow *GetInvokingWindow()         const { return m_pInvokingWindow; }

  // semi-private accessors
    // get the window which contains this menu
  wxWindow *GetWindow() const;
    // get the menu handle
  WXHMENU GetHMenu() const;

private:
  bool              m_doBreak ;

public:
  // This is used when m_hMenu is NULL because we don't want to
  // delete it in ~wxMenu (it's been added to a parent menu).
  // But we'll still need the handle for other purposes.
  // Might be better to have a flag saying whether it's deleteable or not.
  WXHMENU           m_savehMenu ; // Used for Enable() on popup
  WXHMENU           m_hMenu;
  wxFunction        m_callback;

  int               m_noItems;
  wxString          m_title;
  wxMenu *          m_topLevelMenu;
  wxMenuBar *       m_menuBar;
  wxList            m_menuItems;
  wxEvtHandler *    m_parent;
  wxEvtHandler *    m_eventHandler;
  wxWindow         *m_pInvokingWindow;
};

// ----------------------------------------------------------------------------
// Menu Bar (a la Windows)
// ----------------------------------------------------------------------------
class wxFrame;
class WXDLLEXPORT wxMenuBar: public wxEvtHandler
{
  DECLARE_DYNAMIC_CLASS(wxMenuBar)

  wxMenuBar(void);
  wxMenuBar(const int n, wxMenu *menus[], const wxString titles[]);
  ~wxMenuBar(void);

  void Append(wxMenu *menu, const wxString& title);
  // Must only be used AFTER menu has been attached to frame,
  // otherwise use individual menus to enable/disable items
  void Enable(const int Id, const bool Flag);
  bool Enabled(const int Id) const ;
  inline bool IsEnabled(const int Id) const { return Enabled(Id); };
  void EnableTop(const int pos, const bool Flag);
  void Check(const int id, const bool Flag);
  bool Checked(const int id) const ;
  inline bool IsChecked(const int Id) const { return Checked(Id); };
  void SetLabel(const int id, const wxString& label) ;
  wxString GetLabel(const int id) const ;
  void SetLabelTop(const int pos, const wxString& label) ;
  wxString GetLabelTop(const int pos) const ;
  virtual void Delete(wxMenu *menu, const int index = 0); /* Menu not destroyed */
  virtual bool OnAppend(wxMenu *menu, const char *title);
  virtual bool OnDelete(wxMenu *menu, const int index);

  virtual void SetHelpString(const int Id, const wxString& helpString);
  virtual wxString GetHelpString(const int Id) const ;

  virtual int FindMenuItem(const wxString& menuString, const wxString& itemString) const ;

  // Find wxMenuItem for item ID, and return item's
  // menu too if itemMenu is non-NULL.
  wxMenuItem *FindItemForId(const int itemId, wxMenu **menuForItem = NULL) const ;

  inline void SetEventHandler(wxEvtHandler *handler) { m_eventHandler = handler; }
  inline wxEvtHandler *GetEventHandler(void) { return m_eventHandler; }

 public:
  wxEvtHandler *            m_eventHandler;
  int                       m_menuCount;
  wxMenu **                 m_menus;
  wxString *                m_titles;
  wxFrame *                 m_menuBarFrame;
  WXHMENU                   m_hMenu;
};

#endif // __MENUH__
