/////////////////////////////////////////////////////////////////////////////
// Name:        menu.h
// Purpose:     wxMenu, wxMenuBar classes
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOTIF_MENU_H_
#define _WX_MOTIF_MENU_H_

#ifdef __GNUG__
    #pragma interface "menu.h"
#endif

#include "wx/colour.h"
#include "wx/font.h"

class wxFrame;

// ----------------------------------------------------------------------------
// Menu
// ----------------------------------------------------------------------------

class wxMenu : public wxEvtHandler
{
  DECLARE_DYNAMIC_CLASS(wxMenu)

public:
  // ctor & dtor
#ifdef WXWIN_COMPATIBILITY
    wxMenu( const wxString& title, const wxFunction func)
    {
        Init(title, 0, func);
    }
#endif // WXWIN_COMPATIBILITY
    wxMenu( const wxString& title = wxEmptyString, long style = 0 )
    {
        Init(title, style);
    }
  virtual ~wxMenu();

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
  bool IsEnabled(int id) const { return Enabled(id); };
  void Check(int id, bool Flag);
  bool Checked(int id) const;
  bool IsChecked(int id) const { return IsChecked(id); };

  // Client data
  void SetClientData(void* clientData) { m_clientData = clientData; }
  void* GetClientData() const { return m_clientData; }

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

  // Updates the UI for a menu and all submenus recursively.
  // source is the object that has the update event handlers
  // defined for it. If NULL, the menu or associated window
  // will be used.
  void UpdateUI(wxEvtHandler* source = (wxEvtHandler*) NULL);

  void ProcessCommand(wxCommandEvent& event);

#ifdef WXWIN_COMPATIBILITY
  void Callback(const wxFunction func) { m_callback = func; }
#endif // WXWIN_COMPATIBILITY

  void SetEventHandler(wxEvtHandler *handler) { m_eventHandler = handler; }
  wxEvtHandler *GetEventHandler() { return m_eventHandler; }

  wxList& GetItems() const { return (wxList&) m_menuItems; }

  void      SetInvokingWindow(wxWindow *pWin) { m_pInvokingWindow = pWin; }
  wxWindow *GetInvokingWindow()         const { return m_pInvokingWindow; }

  //// Motif-specific
  WXWidget GetButtonWidget() const { return m_buttonWidget; }
  void SetButtonWidget(WXWidget buttonWidget) { m_buttonWidget = buttonWidget; }
  WXWidget GetMainWidget() const { return m_menuWidget; }
  wxMenu* GetParent() const { return m_menuParent; }
  int GetId() const { return m_menuId; }
  void SetId(int id) { m_menuId = id; }
  void SetMenuBar(wxMenuBar* menuBar) { m_menuBar = menuBar; }
  wxMenuBar* GetMenuBar() const { return m_menuBar; }

  void CreatePopup (WXWidget logicalParent, int x, int y);
  void DestroyPopup (void);
  void ShowPopup (int x, int y);
  void HidePopup (void);

  WXWidget CreateMenu(wxMenuBar *menuBar, WXWidget parent, wxMenu *topMenu,
            const wxString& title = "", bool isPulldown = FALSE);

  // For popups, need to destroy, then recreate menu for a different (or
  // possibly same) window, since the parent may change.
  void DestroyMenu(bool full);
  WXWidget FindMenuItem(int id, wxMenuItem **it = NULL) const;

  const wxColour& GetBackgroundColour() const { return m_backgroundColour; }
  const wxColour& GetForegroundColour() const { return m_foregroundColour; }
  const wxFont& GetFont() const { return m_font; }

  void SetBackgroundColour(const wxColour& colour);
  void SetForegroundColour(const wxColour& colour);
  void SetFont(const wxFont& colour);
  void ChangeFont(bool keepOriginalSize = FALSE);

  // implementation from now on
  WXWidget GetHandle() const { return m_menuWidget; }
  bool IsTearOff() const { return (m_style & wxMENU_TEAROFF) != 0; }

public:
#ifdef WXWIN_COMPATIBILITY
  wxFunction        m_callback;
#endif // WXWIN_COMPATIBILITY

  int               m_noItems;
  wxString          m_title;
  wxMenuBar *       m_menuBar;
  wxList            m_menuItems;
  wxEvtHandler *    m_eventHandler;
  void*             m_clientData;
  wxWindow*         m_pInvokingWindow;

  long              m_style;

  //// Motif-specific
  int               m_numColumns;
  WXWidget          m_menuWidget;
  WXWidget          m_popupShell;   // For holding the popup shell widget
  WXWidget          m_buttonWidget; // The actual string, so we can grey it etc.
  int               m_menuId;
  wxMenu*           m_topLevelMenu ;
  wxMenu*           m_menuParent;
  bool              m_ownedByMenuBar;
  wxColour          m_foregroundColour;
  wxColour          m_backgroundColour;
  wxFont            m_font;

private:
    // common code for both constructors:
    void Init( const wxString& title,
               long style
#ifdef WXWIN_COMPATIBILITY
               , const wxFunction func = (wxFunction) NULL
#endif
               );
};

// ----------------------------------------------------------------------------
// Menu Bar
// ----------------------------------------------------------------------------

class wxMenuBar : public wxMenuBarBase
{
public:
    wxMenuBar() { Init(); }
    wxMenuBar(long WXUNUSED(style)) { Init(); }
    wxMenuBar(int n, wxMenu *menus[], const wxString titles[]);
    virtual ~wxMenuBar();

    // implement base class (pure) virtuals
    // ------------------------------------

    virtual bool Append( wxMenu *menu, const wxString &title );
    virtual bool Insert(size_t pos, wxMenu *menu, const wxString& title);
    virtual wxMenu *Replace(size_t pos, wxMenu *menu, const wxString& title);
    virtual wxMenu *Remove(size_t pos);

    virtual int FindMenuItem(const wxString& menuString,
                             const wxString& itemString) const;
    virtual wxMenuItem* FindItem( int id, wxMenu **menu = NULL ) const;

    virtual void EnableTop( size_t pos, bool flag );
    virtual void SetLabelTop( size_t pos, const wxString& label );
    virtual wxString GetLabelTop( size_t pos ) const;

    // implementation only from now on
    // -------------------------------

    wxFrame* GetMenuBarFrame() const { return m_menuBarFrame; }
    void SetMenuBarFrame(wxFrame* frame) { m_menuBarFrame = frame; }
    WXWidget GetMainWidget() const { return m_mainWidget; }
    void SetMainWidget(WXWidget widget) { m_mainWidget = widget; }

    // Create menubar
    bool CreateMenuBar(wxFrame* frame);

    // Destroy menubar, but keep data structures intact so we can recreate it.
    bool DestroyMenuBar();

    const wxColour& GetBackgroundColour() const { return m_backgroundColour; }
    const wxColour& GetForegroundColour() const { return m_foregroundColour; }
    const wxFont& GetFont() const { return m_font; }

    virtual bool SetBackgroundColour(const wxColour& colour);
    virtual bool SetForegroundColour(const wxColour& colour);
    virtual bool SetFont(const wxFont& colour);
    void ChangeFont(bool keepOriginalSize = FALSE);

public:
    // common part of all ctors
    void Init();

    wxArrayString m_titles;
    wxFrame      *m_menuBarFrame;

    WXWidget      m_mainWidget;

    wxColour      m_foregroundColour;
    wxColour      m_backgroundColour;
    wxFont        m_font;

    DECLARE_DYNAMIC_CLASS(wxMenuBar)
};

#endif // _WX_MOTIF_MENU_H_
