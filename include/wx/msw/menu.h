/////////////////////////////////////////////////////////////////////////////
// Name:        menu.h
// Purpose:     wxMenu, wxMenuBar classes
// Author:      Julian Smart
// Modified by: Vadim Zeitlin (wxMenuItem is now in separate file)
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MENU_H_
#define _WX_MENU_H_

#ifdef __GNUG__
    #pragma interface "menu.h"
#endif

#include "wx/defs.h"
#include "wx/event.h"
#include "wx/dynarray.h"
#include "wx/string.h"

#if wxUSE_ACCEL
#include "wx/accel.h"
#endif // wxUSE_ACCEL

class WXDLLEXPORT wxMenuItem;
class WXDLLEXPORT wxMenuBar;
class WXDLLEXPORT wxMenu;
class WXDLLEXPORT wxFrame;

WXDLLEXPORT_DATA(extern const wxChar*) wxEmptyString;

// ----------------------------------------------------------------------------
// Menu
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxMenu : public wxEvtHandler
{
    DECLARE_DYNAMIC_CLASS(wxMenu)

public:
    // ctors & dtor
    wxMenu(const wxString& title,
           const wxFunction func)
    {
        Init(title, func);
    }

    wxMenu( long WXUNUSED(style) )
    {
	Init( wxEmptyString );
    }

    wxMenu(const wxString& title = wxEmptyString, long WXUNUSED(style) = 0)
    {
        Init(title);
    }

    virtual ~wxMenu();

    // construct menu
        // append a separator to the menu
    void AppendSeparator();
        // append a normal item to the menu
    void Append(int id, const wxString& label,
                const wxString& helpString = wxEmptyString,
                bool checkable = FALSE);
        // append a submenu
    void Append(int id, const wxString& label,
                wxMenu *submenu,
                const wxString& helpString = wxEmptyString);
        // append anything (create wxMenuItem first)
    void Append(wxMenuItem *pItem);

        // insert a break in the menu
    void Break();

        // delete an item
        // If it's a submenu, menu is not destroyed.
        // VZ: why? shouldn't it return "wxMenu *" then?
    void Delete(int id);

    // client data
    void SetClientData(void* clientData) { m_clientData = clientData; }
    void* GetClientData() const { return m_clientData; }

    // menu item control
        // enable/disable item
    void Enable(int id, bool enable);
        // TRUE if enabled
    bool IsEnabled(int id) const;

        // check/uncheck item - only for checkable items, of course
    void Check(int id, bool check);
        // TRUE if checked
    bool IsChecked(int id) const;

    // other properties
        // the menu title
    void SetTitle(const wxString& label);
    const wxString GetTitle() const;
        // the item label
    void SetLabel(int id, const wxString& label);
    wxString GetLabel(int id) const;
        // help string
    virtual void SetHelpString(int id, const wxString& helpString);
    virtual wxString GetHelpString(int id) const;

        // get the list of items
    wxList& GetItems() const { return (wxList &)m_menuItems; }

    // find item
        // returns id of the item matching the given string or wxNOT_FOUND
    virtual int FindItem(const wxString& itemString) const;
        // returns NULL if not found
    wxMenuItem* FindItem(int id) const { return FindItemForId(id); }
        // find wxMenuItem by ID, and item's menu too if itemMenu is !NULL
    wxMenuItem *FindItemForId(int itemId, wxMenu **itemMenu = NULL) const;

    // Updates the UI for a menu and all submenus recursively. source is the
    // object that has the update event handlers defined for it. If NULL, the
    // menu or associated window will be used.
    void UpdateUI(wxEvtHandler* source = (wxEvtHandler*)NULL);

    bool ProcessCommand(wxCommandEvent& event);

    virtual void SetParent(wxEvtHandler *parent) { m_parent = parent; }
    void SetEventHandler(wxEvtHandler *handler) { m_eventHandler = handler; }
    wxEvtHandler *GetEventHandler() const { return m_eventHandler; }

    // IMPLEMENTATION
    bool MSWCommand(WXUINT param, WXWORD id);

    void SetInvokingWindow(wxWindow *pWin) { m_pInvokingWindow = pWin; }
    wxWindow *GetInvokingWindow() const { return m_pInvokingWindow; }

    // semi-private accessors
        // get the window which contains this menu
    wxWindow *GetWindow() const;
        // get the menu handle
    WXHMENU GetHMenu() const;

    // only for wxMenuBar
    void Attach(wxMenuBar *menubar);
    void Detach();

#if wxUSE_ACCEL
    size_t GetAccelCount() const { return m_accelKeyCodes.GetCount(); }
    size_t CopyAccels(wxAcceleratorEntry *accels) const;
#endif // wxUSE_ACCEL

    wxFunction GetCallback() const { return m_callback; }
    void Callback(const wxFunction func) { m_callback = func; }
    wxFunction        m_callback;

#ifdef WXWIN_COMPATIBILITY
    // compatibility: these functions are deprecated
    bool Enabled(int id) const { return IsEnabled(id); }
    bool Checked(int id) const { return IsChecked(id); }

#endif // WXWIN_COMPATIBILITY

private:
    // common part of all ctors
    void Init(const wxString& title, const wxFunction func = NULL );

    bool              m_doBreak;

    // This is used when m_hMenu is NULL because we don't want to
    // delete it in ~wxMenu (it's been added to a parent menu).
    // But we'll still need the handle for other purposes.
    // Might be better to have a flag saying whether it's deleteable or not.
    WXHMENU           m_savehMenu ; // Used for Enable() on popup
    WXHMENU           m_hMenu;

    int               m_noItems;
    wxString          m_title;
    wxMenu *          m_topLevelMenu;
    wxMenuBar *       m_menuBar;
    wxList            m_menuItems;
    wxEvtHandler *    m_parent;
    wxEvtHandler *    m_eventHandler;
    wxWindow         *m_pInvokingWindow;
    void*             m_clientData;

#if wxUSE_ACCEL
    // the accelerators data
    wxArrayInt        m_accelKeyCodes, m_accelFlags, m_accelIds;
#endif // wxUSE_ACCEL
};

// ----------------------------------------------------------------------------
// Menu Bar (a la Windows)
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxMenuBar : public wxEvtHandler
{
    DECLARE_DYNAMIC_CLASS(wxMenuBar)

public:
    // ctors & dtor
        // default constructor
    wxMenuBar();
        // unused under MSW
    wxMenuBar(long style);
        // menubar takes ownership of the menus arrays but copies the titles
    wxMenuBar(int n, wxMenu *menus[], const wxString titles[]);
    virtual ~wxMenuBar();

    // menubar construction
    WXHMENU Create();
    void Append(wxMenu *menu, const wxString& title);
    void Insert(int pos, wxMenu * menu, const wxString& title);
    void ReplaceMenu(int pos, wxMenu * new_menu, const wxString& title);
    int  FindMenu(const wxString& title);
    void Detach();
    virtual void Delete(wxMenu *menu, int index = 0); /* Menu not destroyed */

    // state control
    // NB: must only be used AFTER menu has been attached to frame,
    //     otherwise use individual menus to enable/disable items
        // enable the item
    void Enable(int id, bool enable);
        // TRUE if item enabled
    bool IsEnabled(int id) const;
        //
    void EnableTop(int pos, bool enable);

        // works only with checkable items
    void Check(int id, bool check);
        // TRUE if checked
    bool IsChecked(int id) const;

    void SetLabel(int id, const wxString& label) ;
    wxString GetLabel(int id) const ;

    virtual void SetHelpString(int id, const wxString& helpString);
    virtual wxString GetHelpString(int id) const ;

    void SetLabelTop(int pos, const wxString& label) ;
    wxString GetLabelTop(int pos) const ;

    // notifications: return FALSE to prevent the menu from being
    // appended/deleted
    virtual bool OnAppend(wxMenu *menu, const wxChar *title);
    virtual bool OnDelete(wxMenu *menu, int index);

    // item search
        // by menu and item names, returns wxNOT_FOUND if not found
    virtual int FindMenuItem(const wxString& menuString,
                             const wxString& itemString) const;
        // returns NULL if not found
    wxMenuItem* FindItem(int id) const { return FindItemForId(id); }
        // returns NULL if not found, fills menuForItem if !NULL
    wxMenuItem *FindItemForId(int itemId, wxMenu **menuForItem = NULL) const;

    // submenus access
    int GetMenuCount() const { return m_menuCount; }
    wxMenu *GetMenu(int i) const { return m_menus[i]; }

    void SetEventHandler(wxEvtHandler *handler) { m_eventHandler = handler; }
    wxEvtHandler *GetEventHandler() { return m_eventHandler; }

#ifdef WXWIN_COMPATIBILITY
    // compatibility: these functions are deprecated
    bool Enabled(int id) const { return IsEnabled(id); }
    bool Checked(int id) const { return IsChecked(id); }
#endif // WXWIN_COMPATIBILITY

    // IMPLEMENTATION
        // returns TRUE if we're attached to a frame
    bool IsAttached() const { return m_menuBarFrame != NULL; }
        // get the frame we live in
    wxFrame *GetFrame() const { return m_menuBarFrame; }
        // attach to a frame
    void Attach(wxFrame *frame);

#if wxUSE_ACCEL
        // get the accel table for the menus
    const wxAcceleratorTable& GetAccelTable() const { return m_accelTable; }
#endif // wxUSE_ACCEL

        // get the menu handle
    WXHMENU GetHMenu() const { return m_hMenu; }

    // if the menubar is modified, the display is not updated automatically,
    // call this function to update it (m_menuBarFrame should be !NULL)
    void Refresh();

protected:
    // common part of all ctors
    void Init();

    wxEvtHandler *m_eventHandler;
    int           m_menuCount;
    wxMenu      **m_menus;
    wxString     *m_titles;
    wxFrame      *m_menuBarFrame;
    WXHMENU       m_hMenu;

#if wxUSE_ACCEL
    // the accelerator table for all accelerators in all our menus
    wxAcceleratorTable m_accelTable;
#endif // wxUSE_ACCEL
};

#endif // _WX_MENU_H_
