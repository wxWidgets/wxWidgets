/////////////////////////////////////////////////////////////////////////////
// Name:        menu.h
// Purpose:     wxMenu, wxMenuBar classes
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MENU_H_
#define _WX_MENU_H_

#ifdef __GNUG__
#pragma interface "menu.h"
#endif

#include "wx/defs.h"
#include "wx/event.h"

class WXDLLEXPORT wxMenuItem;
class WXDLLEXPORT wxMenuBar;
class WXDLLEXPORT wxMenu;

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
  void Delete(int id);

  // menu item control
  void Enable(int id, bool Flag);
  bool Enabled(int id) const;
  inline bool IsEnabled(int id) const { return Enabled(id); };
  void Check(int id, bool Flag);
  bool Checked(int id) const;
  inline bool IsChecked(int id) const { return IsChecked(id); };

  // Client data
  inline void SetClientData(void* clientData) { m_clientData = clientData; }
  inline void* GetClientData() const { return m_clientData; }

  void      SetInvokingWindow(wxWindow *pWin) { m_pInvokingWindow = pWin; }
  wxWindow *GetInvokingWindow()         const { return m_pInvokingWindow; }

  // item properties
    // title
  void SetTitle(const wxString& label);
  const wxString GetTitle() const;
    // label
  void SetLabel(int id, const wxString& label);
  wxString GetLabel(int id) const;
    // help string
  virtual void SetHelpString(int id, const wxString& helpString);
  virtual wxString GetHelpString(int id) const ;

  // find item
    // Finds the item id matching the given string, -1 if not found.
  virtual int FindItem(const wxString& itemString) const ;
    // Find wxMenuItem by ID, and item's menu too if itemMenu is !NULL.
  wxMenuItem *FindItemForId(int itemId, wxMenu **itemMenu = NULL) const;

  void ProcessCommand(wxCommandEvent& event);
  inline void Callback(const wxFunction func) { m_callback = func; }

  // Updates the UI for a menu and all submenus recursively.
  // source is the object that has the update event handlers
  // defined for it. If NULL, the menu or associated window
  // will be used.
  void UpdateUI(wxEvtHandler* source = (wxEvtHandler*) NULL);

  virtual void SetParent(wxEvtHandler *parent) { m_parent = parent; }
  inline void SetEventHandler(wxEvtHandler *handler) { m_eventHandler = handler; }
  inline wxEvtHandler *GetEventHandler() { return m_eventHandler; }

  inline wxList& GetItems() const { return (wxList&) m_menuItems; }

public:
  wxFunction        m_callback;

  int               m_noItems;
  wxString          m_title;
  wxMenuBar *       m_menuBar;
  wxList            m_menuItems;
  wxEvtHandler *    m_parent;
  wxEvtHandler *    m_eventHandler;
  void*             m_clientData;
  wxWindow*         m_pInvokingWindow;
};

// ----------------------------------------------------------------------------
// Menu Bar (a la Windows)
// ----------------------------------------------------------------------------
class WXDLLEXPORT wxFrame;
class WXDLLEXPORT wxMenuBar: public wxEvtHandler
{
  DECLARE_DYNAMIC_CLASS(wxMenuBar)

  wxMenuBar();
  wxMenuBar(int n, wxMenu *menus[], const wxString titles[]);
  ~wxMenuBar();

  void Append(wxMenu *menu, const wxString& title);
  // Must only be used AFTER menu has been attached to frame,
  // otherwise use individual menus to enable/disable items
  void Enable(int Id, bool Flag);
  bool Enabled(int Id) const ;
  inline bool IsEnabled(int Id) const { return Enabled(Id); };
  void EnableTop(int pos, bool Flag);
  void Check(int id, bool Flag);
  bool Checked(int id) const ;
  inline bool IsChecked(int Id) const { return Checked(Id); };
  void SetLabel(int id, const wxString& label) ;
  wxString GetLabel(int id) const ;
  void SetLabelTop(int pos, const wxString& label) ;
  wxString GetLabelTop(int pos) const ;
  virtual void Delete(wxMenu *menu, int index = 0); /* Menu not destroyed */
  virtual bool OnAppend(wxMenu *menu, const char *title);
  virtual bool OnDelete(wxMenu *menu, int index);

  virtual void SetHelpString(int Id, const wxString& helpString);
  virtual wxString GetHelpString(int Id) const ;

  virtual int FindMenuItem(const wxString& menuString, const wxString& itemString) const ;

  // Find wxMenuItem for item ID, and return item's
  // menu too if itemMenu is non-NULL.
  wxMenuItem *FindItemForId(int itemId, wxMenu **menuForItem = NULL) const ;

  inline void SetEventHandler(wxEvtHandler *handler) { m_eventHandler = handler; }
  inline wxEvtHandler *GetEventHandler() { return m_eventHandler; }

  inline int GetMenuCount() const { return m_menuCount; }
  inline wxMenu* GetMenu(int i) const { return m_menus[i]; }

 public:
  wxEvtHandler *            m_eventHandler;
  int                       m_menuCount;
  wxMenu **                 m_menus;
  wxString *                m_titles;
  wxFrame *                 m_menuBarFrame;
/* TODO: data that represents the actual menubar when created.
 */
};

#endif // _WX_MENU_H_
