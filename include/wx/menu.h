///////////////////////////////////////////////////////////////////////////////
// Name:        wx/menu.h
// Purpose:     wxMenu and wxMenuBar classes
// Author:      Vadim Zeitlin
// Modified by:
// Created:     26.10.99
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MENU_H_BASE_
#define _WX_MENU_H_BASE_

#ifdef __GNUG__
    #pragma interface "menubase.h"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/list.h"        // for wxMenuList
#include "wx/window.h"      // base class for wxMenuBar

class WXDLLEXPORT wxMenu;
class WXDLLEXPORT wxMenuBar;
class WXDLLEXPORT wxMenuItem;

// ----------------------------------------------------------------------------
// wxMenu
// ----------------------------------------------------------------------------

// for now, it's in platform-specific file

WX_DECLARE_LIST(wxMenu, wxMenuList);

// ----------------------------------------------------------------------------
// wxMenuBar
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxMenuBarBase : public wxWindow
{
public:
    // default ctor
    wxMenuBarBase();

    // dtor will delete all menus we own
    virtual ~wxMenuBarBase();

    // menu bar construction
    // ---------------------

    // append a menu to the end of menubar, return TRUE if ok
    virtual bool Append(wxMenu *menu, const wxString& title);

    // insert a menu before the given position into the menubar, return TRUE
    // if inserted ok
    virtual bool Insert(size_t pos, wxMenu *menu, const wxString& title);

    // menu bar items access
    // ---------------------

    // get the number of menus in the menu bar
    size_t GetMenuCount() const { return m_menus.GetCount(); }

    // get the menu at given position
    wxMenu *GetMenu(size_t pos) const;

    // replace the menu at given position with another one, returns the
    // previous menu (which should be deleted by the caller)
    virtual wxMenu *Replace(size_t pos, wxMenu *menu, const wxString& title);

    // delete the menu at given position from the menu bar, return the pointer
    // to the menu (which should be  deleted by the caller)
    virtual wxMenu *Remove(size_t pos);

    // enable or disable a submenu
    virtual void EnableTop(size_t pos, bool enable) = 0;

    // get or change the label of the menu at given position
    virtual void SetLabelTop(size_t pos, const wxString& label) = 0;
    virtual wxString GetLabelTop(size_t pos) const = 0;

    // item search
    // -----------

    // by menu and item names, returns wxNOT_FOUND if not found or id of the
    // found item
    virtual int FindMenuItem(const wxString& menuString,
                             const wxString& itemString) const = 0;

    // find item by id (in any menu), returns NULL if not found
    //
    // if menu is !NULL, it will be filled with wxMenu this item belongs to
    virtual wxMenuItem* FindItem(int id, wxMenu **menu = NULL) const = 0;

    // item access
    // -----------

    // all these functions just use FindItem() and then call an appropriate
    // method on it
    //
    // NB: under MSW, these methods can only be used after the menubar had
    //     been attached to the frame

    void Enable(int id, bool enable);
    void Check(int id, bool check);
    bool IsChecked(int id) const;
    bool IsEnabled(int id) const;

    void SetLabel(int id, const wxString &label);
    wxString GetLabel(int id) const;

    void SetHelpString(int id, const wxString& helpString);
    wxString GetHelpString(int id) const;

    // need to override this one to avoid virtual function hiding
    virtual wxString GetLabel() const { return wxWindow::GetLabel(); }

    // compatibility only: these functions are deprecated, use the new ones
    // instead
#ifdef WXWIN_COMPATIBILITY
    bool Enabled(int id) const { return IsEnabled(id); }
    bool Checked(int id) const { return IsChecked(id); }

    wxMenuItem* FindMenuItemById(int id) const
        { return FindItem(id); }
    wxMenuItem* FindItemForId(int id, wxMenu **menu = NULL) const
        { return FindItem(id, menu); }
#endif // WXWIN_COMPATIBILITY

protected:
    // the list of all our menus
    wxMenuList m_menus;
};

// ----------------------------------------------------------------------------
// include the real class declaration
// ----------------------------------------------------------------------------

#ifdef wxUSE_BASE_CLASSES_ONLY
    #define wxMenuItem wxMenuItemBase
#else // !wxUSE_BASE_CLASSES_ONLY
#if defined(__WXMSW__)
    #include "wx/msw/menu.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/menu.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/menu.h"
#elif defined(__WXQT__)
    #include "wx/qt/menu.h"
#elif defined(__WXMAC__)
    #include "wx/mac/menu.h"
#elif defined(__WXPM__)
    #include "wx/os2/menu.h"
#elif defined(__WXSTUBS__)
    #include "wx/stubs/menu.h"
#endif
#endif // wxUSE_BASE_CLASSES_ONLY/!wxUSE_BASE_CLASSES_ONLY

// also include this one to ensure compatibility with old code which only
// included wx/menu.h
#include "wx/menuitem.h"

#endif
    // _WX_MENU_H_BASE_
