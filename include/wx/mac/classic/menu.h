/////////////////////////////////////////////////////////////////////////////
// Name:        menu.h
// Purpose:     wxMenu, wxMenuBar classes
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MENU_H_
#define _WX_MENU_H_

class WXDLLEXPORT wxFrame;

#include "wx/arrstr.h"

// ----------------------------------------------------------------------------
// Menu
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxMenu : public wxMenuBase
{
public:
    // ctors & dtor
    wxMenu(const wxString& title, long style = 0)
        : wxMenuBase(title, style) { Init(); }

    wxMenu(long style = 0) : wxMenuBase(style) { Init(); }

    virtual ~wxMenu();

    // implement base class virtuals
    virtual wxMenuItem* DoAppend(wxMenuItem *item);
    virtual wxMenuItem* DoInsert(size_t pos, wxMenuItem *item);
    virtual wxMenuItem* DoRemove(wxMenuItem *item);
    virtual void Attach(wxMenuBarBase *menubar) ;

    virtual void Break();

    virtual void SetTitle(const wxString& title);

    // MSW-specific
    bool ProcessCommand(wxCommandEvent& event);

    // implementation only from now on
    // -------------------------------

      int    MacGetIndexFromId( int id ) ;
      int    MacGetIndexFromItem( wxMenuItem *pItem ) ;
      void MacEnableMenu( bool bDoEnable ) ;
      // MacOS needs to know about submenus somewhere within this menu
      // before it can be displayed , also hide special menu items like preferences
      // that are handled by the OS
      void  MacBeforeDisplay( bool isSubMenu ) ;
      // undo all changes from the MacBeforeDisplay call
      void  MacAfterDisplay( bool isSubMenu ) ;

    // semi-private accessors
        // get the window which contains this menu
    wxWindow *GetWindow() const;
        // get the menu handle
    WXHMENU GetHMenu() const { return m_hMenu; }

    short MacGetMenuId() { return m_macMenuId ; }

private:
    // common part of all ctors
    void Init();

    // common part of Append/Insert (behaves as Append is pos == (size_t)-1)
    bool DoInsertOrAppend(wxMenuItem *item, size_t pos = (size_t)-1);

    // terminate the current radio group, if any
    void EndRadioGroup();

    // if TRUE, insert a breal before appending the next item
    bool m_doBreak;

    // the position of the first item in the current radio group or -1
    int m_startRadioGroup;

    // the menu handle of this menu
    WXHMENU m_hMenu;

      short                m_macMenuId;

      static short        s_macNextMenuId ;

    DECLARE_DYNAMIC_CLASS(wxMenu)
};

// ----------------------------------------------------------------------------
// Menu Bar (a la Windows)
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxMenuBar : public wxMenuBarBase
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

    // implementation from now on
    WXHMENU Create();
    int  FindMenu(const wxString& title);
    void Detach();

        // returns TRUE if we're attached to a frame
    bool IsAttached() const { return m_menuBarFrame != NULL; }
        // get the frame we live in
    wxFrame *GetFrame() const { return m_menuBarFrame; }
        // attach to a frame
    void Attach(wxFrame *frame);

        // clear the invoking window for all menus and submenus
    void UnsetInvokingWindow() ;

        // set the invoking window for all menus and submenus
    void SetInvokingWindow( wxFrame* frame ) ;

    // if the menubar is modified, the display is not updated automatically,
    // call this function to update it (m_menuBarFrame should be !NULL)
    void Refresh(bool eraseBackground = TRUE, const wxRect *rect = (const wxRect *) NULL);

  void MacInstallMenuBar() ;
  static wxMenuBar* MacGetInstalledMenuBar() { return s_macInstalledMenuBar ; }
  static void MacSetCommonMenuBar(wxMenuBar* menubar) { s_macCommonMenuBar=menubar; }
  static wxMenuBar* MacGetCommonMenuBar() { return s_macCommonMenuBar; }

protected:
    // common part of all ctors
    void Init();
    wxWindow        *m_invokingWindow;

    wxArrayString m_titles;

private:
  static wxMenuBar*            s_macInstalledMenuBar ;
  static wxMenuBar*            s_macCommonMenuBar ;

    DECLARE_DYNAMIC_CLASS(wxMenuBar)

public:

#if wxABI_VERSION >= 20805
    // Gets the original label at the top-level of the menubar
    wxString GetMenuLabel(size_t pos) const;
#endif
};

#endif // _WX_MENU_H_
