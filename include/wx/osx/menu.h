/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/menu.h
// Purpose:     wxMenu, wxMenuBar classes
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MENU_H_
#define _WX_MENU_H_

class WXDLLIMPEXP_FWD_CORE wxFrame;

#include "wx/arrstr.h"

class wxMenuRadioItemsData;

// ----------------------------------------------------------------------------
// Menu
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_FWD_CORE wxMenuImpl ;

class WXDLLIMPEXP_CORE wxMenu : public wxMenuBase
{
public:
    // ctors & dtor
    wxMenu(const wxString& title, long style = 0)
        : wxMenuBase(title, style) { Init(); }

    wxMenu(long style = 0) : wxMenuBase(style) { Init(); }

    virtual ~wxMenu();

    virtual void SetTitle(const wxString& title) wxOVERRIDE;

    bool ProcessCommand(wxCommandEvent& event);

    // get the menu handle
    WXHMENU GetHMenu() const ;

    // implementation only from now on
    // -------------------------------

    bool HandleCommandUpdateStatus( wxMenuItem* menuItem, wxWindow* senderWindow = NULL);
    bool HandleCommandProcess( wxMenuItem* menuItem, wxWindow* senderWindow = NULL);
    void HandleMenuItemHighlighted( wxMenuItem* menuItem );
    void HandleMenuOpened();
    void HandleMenuClosed();

    wxMenuImpl* GetPeer() { return m_peer; }

    // make sure we can veto
    void SetAllowRearrange( bool allow );
    bool AllowRearrange() const { return m_allowRearrange; }

    // if a menu is used purely for internal implementation reasons (eg wxChoice)
    // we don't want native menu events being triggered
    void SetNoEventsMode( bool noEvents );
    bool GetNoEventsMode() const { return m_noEventsMode; }

    // Returns the start and end position of the radio group to which the item
    // at given position belongs. Return false if there is no radio group
    // containing this position.
    bool OSXGetRadioGroupRange(int pos, int *start, int *end) const;

protected:
    // hide special menu items like exit, preferences etc
    // that are expected in the app menu
    void DoRearrange() ;

    virtual wxMenuItem* DoAppend(wxMenuItem *item) wxOVERRIDE;
    virtual wxMenuItem* DoInsert(size_t pos, wxMenuItem *item) wxOVERRIDE;
    virtual wxMenuItem* DoRemove(wxMenuItem *item) wxOVERRIDE;

private:
    // common part of all ctors
    void Init();

    // common part of Do{Append,Insert}(): behaves as Append if pos == -1
    bool DoInsertOrAppend(wxMenuItem *item, size_t pos = (size_t)-1);

    // Common part of HandleMenu{Opened,Closed}().
    void DoHandleMenuOpenedOrClosed(wxEventType evtType);


    // if TRUE, insert a break before appending the next item
    bool m_doBreak;

    // in this menu rearranging of menu items (esp hiding) is allowed
    bool m_allowRearrange;

    // don't trigger native events
    bool m_noEventsMode;

    wxMenuRadioItemsData* m_radioData;

    wxMenuImpl* m_peer;

    wxDECLARE_DYNAMIC_CLASS(wxMenu);
};

#if wxUSE_MENUBAR

// the iphone only has popup-menus

// ----------------------------------------------------------------------------
// Menu Bar (a la Windows)
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxMenuBar : public wxMenuBarBase
{
public:
    // ctors & dtor
        // default constructor
    wxMenuBar();
        // unused under MSW
    wxMenuBar(long style);
        // menubar takes ownership of the menus arrays but copies the titles
    wxMenuBar(size_t n, wxMenu *menus[], const wxString titles[], long style = 0);
    virtual ~wxMenuBar();

    // menubar construction
    virtual bool Append( wxMenu *menu, const wxString &title ) wxOVERRIDE;
    virtual bool Insert(size_t pos, wxMenu *menu, const wxString& title) wxOVERRIDE;
    virtual wxMenu *Replace(size_t pos, wxMenu *menu, const wxString& title) wxOVERRIDE;
    virtual wxMenu *Remove(size_t pos) wxOVERRIDE;

    virtual void EnableTop( size_t pos, bool flag ) wxOVERRIDE;
    virtual bool IsEnabledTop(size_t pos) const wxOVERRIDE;
    virtual void SetMenuLabel( size_t pos, const wxString& label ) wxOVERRIDE;
    virtual wxString GetMenuLabel( size_t pos ) const wxOVERRIDE;
    virtual bool Enable( bool enable = true ) wxOVERRIDE;
    // for virtual function hiding
    virtual void Enable( int itemid, bool enable )
    {
        wxMenuBarBase::Enable( itemid, enable );
    }

    // implementation from now on

        // returns TRUE if we're attached to a frame
    bool IsAttached() const { return m_menuBarFrame != NULL; }
        // get the frame we live in
    wxFrame *GetFrame() const { return m_menuBarFrame; }

    // if the menubar is modified, the display is not updated automatically,
    // call this function to update it (m_menuBarFrame should be !NULL)
    void Refresh(bool eraseBackground = true, const wxRect *rect = NULL) wxOVERRIDE;

#if wxABI_VERSION >= 30001
    wxMenu *OSXGetAppleMenu() const { return m_appleMenu; }
#endif

    static void SetAutoWindowMenu( bool enable ) { s_macAutoWindowMenu = enable ; }
    static bool GetAutoWindowMenu() { return s_macAutoWindowMenu ; }

    void MacUninstallMenuBar() ;
    void MacInstallMenuBar() ;
    static wxMenuBar* MacGetInstalledMenuBar() { return s_macInstalledMenuBar ; }
    static void MacSetCommonMenuBar(wxMenuBar* menubar) { s_macCommonMenuBar=menubar; }
    static wxMenuBar* MacGetCommonMenuBar() { return s_macCommonMenuBar; }


    static WXHMENU MacGetWindowMenuHMenu() { return s_macWindowMenuHandle ; }

    virtual void DoGetPosition(int *x, int *y) const wxOVERRIDE;
    virtual void DoGetSize(int *width, int *height) const wxOVERRIDE;
    virtual void DoGetClientSize(int *width, int *height) const wxOVERRIDE;

protected:
    // common part of all ctors
    void Init();

    static bool     s_macAutoWindowMenu ;
    static WXHMENU  s_macWindowMenuHandle ;

private:
    static wxMenuBar*            s_macInstalledMenuBar ;
    static wxMenuBar*            s_macCommonMenuBar ;

    wxMenu* m_rootMenu;
    wxMenu* m_appleMenu;

    wxDECLARE_DYNAMIC_CLASS(wxMenuBar);
};

#endif

#endif // _WX_MENU_H_
