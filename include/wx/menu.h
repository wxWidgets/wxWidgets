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

#include "wx/list.h"        // for "template" list classes
#include "wx/window.h"      // base class for wxMenuBar

// also include this one to ensure compatibility with old code which only
// included wx/menu.h
#include "wx/menuitem.h"

class WXDLLEXPORT wxMenu;
class WXDLLEXPORT wxMenuBar;
class WXDLLEXPORT wxMenuItem;

// pseudo template list classes
WX_DECLARE_LIST(wxMenu, wxMenuList);
WX_DECLARE_LIST(wxMenuItem, wxMenuItemList);

// ----------------------------------------------------------------------------
// wxMenu
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxMenuBase : public wxEvtHandler
{
public:
    // create a menu
    static wxMenu *New(const wxString& title = wxEmptyString, long style = 0);

    // ctors
    wxMenuBase(const wxString& title, long style = 0) : m_title(title)
        { Init(style); }
    wxMenuBase(long style = 0)
        { Init(style); }

    // dtor deletes all the menu items we own
    virtual ~wxMenuBase();

    // menu construction
    // -----------------

    // append a separator to the menu
    void AppendSeparator() { Append(wxID_SEPARATOR, wxEmptyString); }

    // append a normal item to the menu
    void Append(int id,
                const wxString& text,
                const wxString& help = wxEmptyString,
                bool isCheckable = FALSE)
    {
        DoAppend(wxMenuItem::New((wxMenu *)this, id, text, help, isCheckable));
    }

    // append a submenu
    void Append(int id,
                const wxString& text,
                wxMenu *submenu,
                const wxString& help = wxEmptyString)
    {
        DoAppend(wxMenuItem::New((wxMenu *)this, id, text, help, FALSE, submenu));
    }

    // the most generic form of Append() - append anything
    void Append(wxMenuItem *item) { DoAppend(item); }

    // insert a break in the menu (only works when appending the items, not
    // inserting them)
    virtual void Break() { }

    // insert an item before given position
    bool Insert(size_t pos, wxMenuItem *item);

    // detach an item from the menu, but don't delete it so that it can be
    // added back later (but if it's not, the caller is responsible for
    // deleting it!)
    wxMenuItem *Remove(int id) { return Remove(FindChildItem(id)); }
    wxMenuItem *Remove(wxMenuItem *item);

    // delete an item from the menu (submenus are not destroyed by this
    // function, see Destroy)
    bool Delete(int id) { return Delete(FindChildItem(id)); }
    bool Delete(wxMenuItem *item);

    // delete the item from menu and destroy it (if it's a submenu)
    bool Destroy(int id) { return Destroy(FindChildItem(id)); }
    bool Destroy(wxMenuItem *item);

    // menu items access
    // -----------------

    // get the items
    size_t GetMenuItemCount() const { return m_items.GetCount(); }

    const wxMenuItemList& GetMenuItems() const { return m_items; }
    wxMenuItemList& GetMenuItems() { return m_items; }

    // search
    virtual int FindItem(const wxString& itemString) const;
    wxMenuItem* FindItem(int id, wxMenu **menu = NULL) const;

    // get/set items attributes
    void Enable(int id, bool enable);
    bool IsEnabled(int id) const;

    void Check(int id, bool check);
    bool IsChecked(int id) const;

    void SetLabel(int id, const wxString& label);
    wxString GetLabel(int id) const;

    virtual void SetHelpString(int id, const wxString& helpString);
    virtual wxString GetHelpString(int id) const;

    // misc accessors
    // --------------

    // the title
    virtual void SetTitle(const wxString& title) { m_title = title; }
    const wxString GetTitle() const { return m_title; }

    // client data
    void SetClientData(void* clientData) { m_clientData = clientData; }
    void* GetClientData() const { return m_clientData; }

    // event handler
    void SetEventHandler(wxEvtHandler *handler) { m_eventHandler = handler; }
    wxEvtHandler *GetEventHandler() const { return m_eventHandler; }

    // invoking window
    void SetInvokingWindow(wxWindow *win) { m_invokingWindow = win; }
    wxWindow *GetInvokingWindow() const { return m_invokingWindow; }

    // style
    long GetStyle() const { return m_style; }

    // implementation helpers
    // ----------------------

    // Updates the UI for a menu and all submenus recursively. source is the
    // object that has the update event handlers defined for it. If NULL, the
    // menu or associated window will be used.
    void UpdateUI(wxEvtHandler* source = (wxEvtHandler*)NULL);

    // is the menu attached to a menu bar (or is it a popup one)?
    bool IsAttached() const { return m_menuBar != NULL; }

    // set/get the parent of this menu
    void SetParent(wxMenu *parent) { m_menuParent = parent; }
    wxMenu *GetParent() const { return m_menuParent; }

#if WXWIN_COMPATIBILITY
    // compatibility: these functions are deprecated, use the new ones instead
    bool Enabled(int id) const { return IsEnabled(id); }
    bool Checked(int id) const { return IsChecked(id); }

    wxMenuItem* FindItemForId(int itemId, wxMenu **itemMenu) const
        { return FindItem(itemId, itemMenu); }

    wxList& GetItems() const { return (wxList &)m_items; }

    // wxWin 1.6x compatible menu event handling
    wxFunction GetCallback() const { return m_callback; }
    void Callback(const wxFunction func) { m_callback = func; }
    wxFunction m_callback;
#endif // WXWIN_COMPATIBILITY

protected:
    // virtuals to override in derived classes
    // ---------------------------------------

    virtual bool DoAppend(wxMenuItem *item);
    virtual bool DoInsert(size_t pos, wxMenuItem *item);

    virtual wxMenuItem *DoRemove(wxMenuItem *item);
    virtual bool DoDelete(wxMenuItem *item);
    virtual bool DoDestroy(wxMenuItem *item);

    // helpers
    // -------

    // common part of all ctors
    void Init(long style);

    // unlike FindItem(), this function doesn't recurse but only looks through
    // our direct children and also may return the index of the found child if
    // pos != NULL
    wxMenuItem *FindChildItem(int id, size_t *pos = NULL) const;

protected:
    wxMenuBar     *m_menuBar;           // menubar we belong to or NULL
    wxMenu        *m_menuParent;        // parent menu or NULL

    wxString       m_title;             // the menu title or label
    wxMenuItemList m_items;             // the list of menu items

    wxWindow      *m_invokingWindow;    // for popup menus
    void          *m_clientData;        // associated with the menu

    long           m_style;             // combination of wxMENU_XXX flags

    wxEvtHandler  *m_eventHandler;      // a pluggable in event handler
};

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

    // need to override these ones to avoid virtual function hiding
    virtual bool Enable(bool enable = TRUE) { return wxWindow::Enable(enable); }
    virtual void SetLabel(const wxString& s) { wxWindow::SetLabel(s); }
    virtual wxString GetLabel() const { return wxWindow::GetLabel(); }

    // compatibility only: these functions are deprecated, use the new ones
    // instead
#if WXWIN_COMPATIBILITY
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

#endif
    // _WX_MENU_H_BASE_
