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

class wxMenu : public wxMenuBase
{
public:
    // ctors & dtor
    wxMenu(const wxString& title, long style = 0)
        : wxMenuBase(title, style) { Init(); }

    wxMenu(long style = 0) : wxMenuBase(style) { Init(); }

    virtual ~wxMenu();

    // implement base class virtuals
    virtual bool DoAppend(wxMenuItem *item);
    virtual bool DoInsert(size_t pos, wxMenuItem *item);
    virtual wxMenuItem *DoRemove(wxMenuItem *item);

    virtual void Break();

    virtual void SetTitle(const wxString& title);

    bool ProcessCommand(wxCommandEvent& event);

    wxMenu(const wxString& title, const wxFunction func)
        : wxMenuBase(title)
    {
        Callback(func);
    }

    //// Motif-specific
    WXWidget GetButtonWidget() const { return m_buttonWidget; }
    void SetButtonWidget(WXWidget buttonWidget) { m_buttonWidget = buttonWidget; }

    WXWidget GetMainWidget() const { return m_menuWidget; }

    int GetId() const { return m_menuId; }
    void SetId(int id) { m_menuId = id; }

    void SetMenuBar(wxMenuBar* menuBar) { m_menuBar = menuBar; }
    wxMenuBar* GetMenuBar() const { return m_menuBar; }

    void CreatePopup(WXWidget logicalParent, int x, int y);
    void DestroyPopup();
    void ShowPopup(int x, int y);
    void HidePopup();

    WXWidget CreateMenu(wxMenuBar *menuBar, WXWidget parent, wxMenu *topMenu,
                        const wxString& title = wxEmptyString,
                        bool isPulldown = FALSE);

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

    WXWidget GetHandle() const { return m_menuWidget; }

    bool IsTearOff() const { return (m_style & wxMENU_TEAROFF) != 0; }

public:
    // Motif-specific data
    int               m_numColumns;
    WXWidget          m_menuWidget;
    WXWidget          m_popupShell;   // For holding the popup shell widget
    WXWidget          m_buttonWidget; // The actual string, so we can grey it etc.
    int               m_menuId;
    wxMenu*           m_topLevelMenu ;
    bool              m_ownedByMenuBar;
    wxColour          m_foregroundColour;
    wxColour          m_backgroundColour;
    wxFont            m_font;

private:
    // common code for both constructors:
    void Init();

    DECLARE_DYNAMIC_CLASS(wxMenu)
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
