/////////////////////////////////////////////////////////////////////////////
// Name:        menu.cpp
// Purpose:     wxMenu, wxMenuBar, wxMenuItem
// Author:      Julian Smart
// Modified by: Vadim Zeitlin
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "menu.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/frame.h"
    #include "wx/menu.h"
    #include "wx/utils.h"
    #include "wx/intl.h"
#endif

#if wxUSE_OWNER_DRAWN
    #include "wx/ownerdrw.h"
#endif

#include "wx/msw/private.h"
#include "wx/msw/menu.h"
#include "wx/menuitem.h"
#include "wx/log.h"

// other standard headers
#include <string.h>

// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------

extern wxMenu *wxCurrentPopupMenu;

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// the (popup) menu title has this special id
static const int idMenuTitle = -2;

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARY
    IMPLEMENT_DYNAMIC_CLASS(wxMenu, wxEvtHandler)
    IMPLEMENT_DYNAMIC_CLASS(wxMenuBar, wxEvtHandler)
#endif

// ============================================================================
// implementation
// ============================================================================

// ---------------------------------------------------------------------------
// wxMenu construction, adding and removing menu items
// ---------------------------------------------------------------------------

// Construct a menu with optional title (then use append)
void wxMenu::Init(const wxString& title, const wxFunction func )
{
    m_title = title;
    m_parent = NULL;
    m_eventHandler = this;
    m_pInvokingWindow = NULL;
    m_doBreak = FALSE ;
    m_noItems = 0;
    m_menuBar = NULL;
    m_hMenu = (WXHMENU) CreatePopupMenu();
    m_savehMenu = 0 ;
    m_topLevelMenu = this;
    m_clientData = (void*) NULL;

    if ( !!m_title )
    {
        Append(idMenuTitle, m_title) ;
        AppendSeparator() ;
    }

    Callback(func);
}

// The wxWindow destructor will take care of deleting the submenus.
wxMenu::~wxMenu()
{
    // free Windows resources
    if ( m_hMenu )
    {
        ::DestroyMenu((HMENU)m_hMenu);
        m_hMenu = 0;
    }

    // delete submenus
    wxNode *node = m_menuItems.First();
    while ( node )
    {
        wxMenuItem *item = (wxMenuItem *)node->Data();

        // Delete child menus.
        // Beware: they must not be appended to children list!!!
        // (because order of delete is significant)
        if ( item->IsSubMenu() )
            item->DeleteSubMenu();

        wxNode *next = node->Next();
        delete item;
        delete node;
        node = next;
    }
}

void wxMenu::Break()
{
    m_doBreak = TRUE;
}

// function appends a new item or submenu to the menu
void wxMenu::Append(wxMenuItem *pItem)
{
    wxCHECK_RET( pItem != NULL, wxT("can't append NULL item to the menu") );

#if wxUSE_ACCEL
    wxAcceleratorEntry *accel = wxGetAccelFromString(pItem->GetText());
    if ( accel ) {
        m_accels.Add(accel);
    }
#endif // wxUSE_ACCEL

    UINT flags = 0;

    // if "Break" has just been called, insert a menu break before this item
    // (and don't forget to reset the flag)
    if ( m_doBreak ) {
        flags |= MF_MENUBREAK;
        m_doBreak = FALSE;
    }

    if ( pItem->IsSeparator() ) {
        flags |= MF_SEPARATOR;
    }

    // id is the numeric id for normal menu items and HMENU for submenus as
    // required by ::AppendMenu() API
    UINT id;
    wxMenu *submenu = pItem->GetSubMenu();
    if ( submenu != NULL ) {
        wxASSERT( submenu->GetHMenu() != (WXHMENU) NULL );

        id = (UINT)submenu->GetHMenu();
        submenu->m_topLevelMenu = m_topLevelMenu;
        submenu->m_parent       = this;
        submenu->m_savehMenu    = (WXHMENU)id;
        submenu->m_hMenu        = 0;

        flags |= MF_POPUP;
    }
    else {
        id = pItem->GetId();
    }

    LPCTSTR pData;

#if wxUSE_OWNER_DRAWN
    if ( pItem->IsOwnerDrawn() ) {  // want to get {Measure|Draw}Item messages?
        // item draws itself, pass pointer to it in data parameter
        flags |= MF_OWNERDRAW;
        pData = (LPCTSTR)pItem;
    }
    else
#endif
    {
        // menu is just a normal string (passed in data parameter)
        flags |= MF_STRING;

        pData = (char*)pItem->GetText().c_str();
    }

    if ( !::AppendMenu(GetHmenu(), flags, id, pData) )
    {
        wxLogLastError("AppendMenu");
    }
    else
    {
#ifdef __WIN32__
        if ( (int)id == idMenuTitle )
        {
            // visually select the menu title
            MENUITEMINFO mii;
            mii.cbSize = sizeof(mii);
            mii.fMask = MIIM_STATE;
            mii.fState = MFS_DEFAULT;

            if ( !SetMenuItemInfo(GetHmenu(), (unsigned)id, FALSE, &mii) )
            {
                wxLogLastError(wxT("SetMenuItemInfo"));
            }
        }
#endif // __WIN32__

        m_menuItems.Append(pItem);
        m_noItems++;
    }
}

void wxMenu::AppendSeparator()
{
    Append(new wxMenuItem(this, ID_SEPARATOR));
}

// Pullright item
void wxMenu::Append(int id,
                    const wxString& label,
                    wxMenu *SubMenu,
                    const wxString& helpString)
{
    Append(new wxMenuItem(this, id, label, helpString, FALSE, SubMenu));
}

// Ordinary menu item
void wxMenu::Append(int id,
                    const wxString& label,
                    const wxString& helpString,
                    bool checkable)
{
    // 'checkable' parameter is useless for Windows.
    Append(new wxMenuItem(this, id, label, helpString, checkable));
}

// delete item by id
void wxMenu::Delete(int id)
{
    wxMenuItem *item = NULL;
    int pos;
    wxNode *node;
    for (pos = 0, node = m_menuItems.First(); node; node = node->Next(), pos++)
    {
        item = (wxMenuItem *)node->Data();
        if ( item->GetId() == id )
            break;
    }

    wxCHECK_RET( node, wxT("wxMenu::Delete(): item doesn't exist") );

    HMENU menu = GetHmenu();

    wxMenu *pSubMenu = item->GetSubMenu();
    if ( pSubMenu != NULL ) {
        RemoveMenu(menu, (UINT)pos, MF_BYPOSITION);
        pSubMenu->m_hMenu = pSubMenu->m_savehMenu;
        pSubMenu->m_savehMenu = 0;
        pSubMenu->m_parent = NULL;
        //     RemoveChild(item->subMenu);
        pSubMenu->m_topLevelMenu = NULL;
        // TODO: Why isn't subMenu deleted here???
        // Will put this in for now. Assuming this is supposed
        // to delete the menu, not just remove it.
        item->DeleteSubMenu();
    }
    else {
        DeleteMenu(menu, (UINT)pos, MF_BYPOSITION);
    }

    m_menuItems.DeleteNode(node);
    delete item;
}

#if wxUSE_ACCEL

// ---------------------------------------------------------------------------
// accelerator helpers
// ---------------------------------------------------------------------------

// create the wxAcceleratorEntries for our accels and put them into provided
// array - return the number of accels we have
size_t wxMenu::CopyAccels(wxAcceleratorEntry *accels) const
{
    size_t count = GetAccelCount();
    for ( size_t n = 0; n < count; n++ )
    {
        *accels++ = *m_accels[n];
    }

    return count;
}

#endif // wxUSE_ACCEL

// ---------------------------------------------------------------------------
// wxMenu functions implemented in wxMenuItem
// ---------------------------------------------------------------------------

void wxMenu::Enable(int id, bool Flag)
{
    wxMenuItem *item = FindItemForId(id);
    wxCHECK_RET( item != NULL, wxT("can't enable non-existing menu item") );

    item->Enable(Flag);
}

bool wxMenu::IsEnabled(int id) const
{
    wxMenuItem *item = FindItemForId(id);
    wxCHECK_MSG( item != NULL, FALSE, wxT("invalid item id") );

    return item->IsEnabled();
}

void wxMenu::Check(int id, bool Flag)
{
    wxMenuItem *item = FindItemForId(id);
    wxCHECK_RET( item != NULL, wxT("can't get status of non-existing menu item") );

    item->Check(Flag);
}

bool wxMenu::IsChecked(int id) const
{
    wxMenuItem *item = FindItemForId(id);
    wxCHECK_MSG( item != NULL, FALSE, wxT("invalid item id") );

    return item->IsChecked();
}

void wxMenu::SetLabel(int id, const wxString& label)
{
    wxMenuItem *item = FindItemForId(id) ;
    wxCHECK_RET( item, wxT("wxMenu::SetLabel: no such item") );

    item->SetText(label);
}

wxString wxMenu::GetLabel(int id) const
{
    wxString label;
    wxMenuItem *pItem = FindItemForId(id) ;
    if (pItem)
        label = pItem->GetText() ;
    else
        wxFAIL_MSG(wxT("wxMenu::GetLabel: item doesn't exist"));

    return label;
}

void wxMenu::SetHelpString(int itemId, const wxString& helpString)
{
    wxMenuItem *item = FindItemForId (itemId);
    if (item)
        item->SetHelp(helpString);
    else
        wxFAIL_MSG(wxT("wxMenu::SetHelpString: item doesn't exist"));
}

wxString wxMenu::GetHelpString (int itemId) const
{
    wxString help;
    wxMenuItem *item = FindItemForId (itemId);
    if (item)
        help = item->GetHelp();
    else
        wxFAIL_MSG(wxT("wxMenu::GetHelpString: item doesn't exist"));

    return help;
}

// ---------------------------------------------------------------------------
// wxMenu title
// ---------------------------------------------------------------------------

void wxMenu::SetTitle(const wxString& label)
{
    bool hasNoTitle = m_title.IsEmpty();
    m_title = label;

    HMENU hMenu = GetHmenu();

    if ( hasNoTitle )
    {
        if ( !label.IsEmpty() )
        {
            if ( !InsertMenu(hMenu, 0u, MF_BYPOSITION | MF_STRING,
                             (unsigned)idMenuTitle, m_title) ||
                 !InsertMenu(hMenu, 1u, MF_BYPOSITION, (unsigned)-1, NULL) )
            {
                wxLogLastError(wxT("InsertMenu"));
            }
        }
    }
    else
    {
        if ( label.IsEmpty() )
        {
            // remove the title and the separator after it
            if ( !RemoveMenu(hMenu, 0, MF_BYPOSITION) ||
                 !RemoveMenu(hMenu, 0, MF_BYPOSITION) )
            {
                wxLogLastError("RemoveMenu");
            }
        }
        else
        {
            // modify the title
            if ( !ModifyMenu(hMenu, 0u,
                        MF_BYPOSITION | MF_STRING,
                        (unsigned)idMenuTitle, m_title) )
            {
                wxLogLastError("ModifyMenu");
            }
        }
    }

#ifdef __WIN32__
    // put the title string in bold face
    if ( !m_title.IsEmpty() )
    {
        MENUITEMINFO mii;
        mii.cbSize = sizeof(mii);
        mii.fMask = MIIM_STATE;
        mii.fState = MFS_DEFAULT;

        if ( !SetMenuItemInfo(hMenu, (unsigned)idMenuTitle, FALSE, &mii) )
        {
            wxLogLastError("SetMenuItemInfo");
        }
    }
#endif
}

const wxString wxMenu::GetTitle() const
{
    return m_title;
}

// ---------------------------------------------------------------------------
// event processing
// ---------------------------------------------------------------------------

bool wxMenu::MSWCommand(WXUINT WXUNUSED(param), WXWORD id)
{
    // ignore commands from the menu title

    // NB: VC++ generates wrong assembler for `if ( id != idMenuTitle )'!!
    if ( id != (WXWORD)idMenuTitle )
    {
        wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED);
        event.SetEventObject( this );
        event.SetId( id );
        event.SetInt( id );
        ProcessCommand(event);
    }

    return TRUE;
}

bool wxMenu::ProcessCommand(wxCommandEvent & event)
{
    bool processed = FALSE;

    // Try a callback
    if (m_callback)
    {
        (void)(*(m_callback))(*this, event);
        processed = TRUE;
    }

    // Try the menu's event handler
    if ( !processed && GetEventHandler())
    {
        processed = GetEventHandler()->ProcessEvent(event);
    }

    // Try the window the menu was popped up from (and up through the
    // hierarchy)
    wxWindow *win = GetInvokingWindow();
    if ( !processed && win )
        processed = win->GetEventHandler()->ProcessEvent(event);

    return processed;
}

// ---------------------------------------------------------------------------
// Item search
// ---------------------------------------------------------------------------

// Finds the item id matching the given string, -1 if not found.
int wxMenu::FindItem (const wxString& itemString) const
{
    wxString itemLabel = wxStripMenuCodes(itemString);
    for ( wxNode *node = m_menuItems.First(); node; node = node->Next() )
    {
        wxMenuItem *item = (wxMenuItem *)node->Data();
        if ( item->IsSubMenu() )
        {
            int ans = item->GetSubMenu()->FindItem(itemString);
            if ( ans != wxNOT_FOUND )
                return ans;
        }
        else if ( !item->IsSeparator() )
        {
            wxString label = wxStripMenuCodes(item->GetText());
            if ( itemLabel == label )
                return item->GetId();
        }
    }

    return wxNOT_FOUND;
}

wxMenuItem *wxMenu::FindItemForId(int itemId, wxMenu ** itemMenu) const
{
    if ( itemMenu )
        *itemMenu = NULL;

    wxMenuItem *item = NULL;
    for ( wxNode *node = m_menuItems.First(); node && !item; node = node->Next() )
    {
        item = (wxMenuItem *)node->Data();

        if ( item->GetId() == itemId )
        {
            if (itemMenu)
                *itemMenu = (wxMenu *)this;
        }
        else if ( item->IsSubMenu() )
        {
            item = item->GetSubMenu()->FindItemForId(itemId, itemMenu);
        }
        else
        {
            // don't exit the loop
            item = NULL;
        }
    }

    return item;
}

// ---------------------------------------------------------------------------
// other
// ---------------------------------------------------------------------------

void wxMenu::Attach(wxMenuBar *menubar)
{
    // menu can be in at most one menubar because otherwise they would both
    // delete the menu pointer
    wxASSERT_MSG( !m_menuBar, wxT("menu belongs to 2 menubars, expect a crash") );

    m_menuBar = menubar;
    m_savehMenu = m_hMenu;
    m_hMenu = 0;
}

void wxMenu::Detach()
{
    wxASSERT_MSG( m_menuBar, wxT("can't detach menu if it's not attached") );

    m_hMenu = m_savehMenu;
    m_savehMenu = 0;
}

// ---------------------------------------------------------------------------
// Menu Bar
// ---------------------------------------------------------------------------

void wxMenuBar::Init()
{
    m_eventHandler = this;
    m_titles = NULL;
    m_menuBarFrame = NULL;
    m_hMenu = 0;
}

wxMenuBar::wxMenuBar()
{
    Init();
}

wxMenuBar::wxMenuBar( long WXUNUSED(style) )
{
    Init();
}

wxMenuBar::wxMenuBar(int count, wxMenu *menus[], const wxString titles[])
{
    Init();

    m_titles.Alloc(count);

    for ( int i = 0; i < count; i++ )
    {
        m_menus.Append(menus[i]);
        m_titles.Add(titles[i]);

        menus[i]->Attach(this);
    }
}

wxMenuBar::~wxMenuBar()
{
}

// ---------------------------------------------------------------------------
// wxMenuBar helpers
// ---------------------------------------------------------------------------

void wxMenuBar::Refresh()
{
    wxCHECK_RET( m_menuBarFrame, wxT("can't refresh a menubar withotu a frame") );

    DrawMenuBar((HWND)m_menuBarFrame->GetHWND()) ;
}

WXHMENU wxMenuBar::Create()
{
    if (m_hMenu != 0 )
    return m_hMenu;

    wxCHECK_MSG( !m_hMenu, TRUE, wxT("menubar already created") );

    m_hMenu = (WXHMENU)::CreateMenu();

    if ( !m_hMenu )
    {
        wxLogLastError("CreateMenu");
    }
    else
    {
        size_t count = GetMenuCount();
        for ( size_t i = 0; i < count; i++ )
        {
            if ( !::AppendMenu((HMENU)m_hMenu, MF_POPUP | MF_STRING,
                               (UINT)m_menus[i]->GetHMenu(),
                               m_titles[i]) )
            {
                wxLogLastError("AppendMenu");
            }
        }
    }

    return m_hMenu;
}

// ---------------------------------------------------------------------------
// wxMenuBar functions to work with the top level submenus
// ---------------------------------------------------------------------------

// NB: we don't support owner drawn top level items for now, if we do these
//     functions would have to be changed to use wxMenuItem as well

void wxMenuBar::EnableTop(size_t pos, bool enable)
{
    int flag = enable ? MF_ENABLED : MF_GRAYED;;

    EnableMenuItem((HMENU)m_hMenu, pos, MF_BYPOSITION | flag);

    Refresh();
}

void wxMenuBar::SetLabelTop(size_t pos, const wxString& label)
{
    UINT id;
    UINT flagsOld = ::GetMenuState((HMENU)m_hMenu, pos, MF_BYPOSITION);
    if ( flagsOld == 0xFFFFFFFF )
    {
        wxLogLastError(wxT("GetMenuState"));

        return;
    }

    if ( flagsOld & MF_POPUP )
    {
        // HIBYTE contains the number of items in the submenu in this case
        flagsOld &= 0xff ;
        id = (UINT)::GetSubMenu((HMENU)m_hMenu, pos) ;
    }
    else
    {
        id = pos;
    }

    if ( ::ModifyMenu(GetHmenu(), pos, MF_BYPOSITION | MF_STRING | flagsOld,
                      id, label) == (int)0xFFFFFFFF )
    {
        wxLogLastError("ModifyMenu");
    }
}

wxString wxMenuBar::GetLabelTop(size_t pos) const
{
    int len = ::GetMenuString((HMENU)m_hMenu, pos, NULL, 0, MF_BYCOMMAND);

    len++;  // for the NUL character
    wxString label;
    ::GetMenuString(GetHmenu(), pos, label.GetWriteBuf(len), len, MF_BYCOMMAND);
    label.UngetWriteBuf();

    return label;
}

// ---------------------------------------------------------------------------
// wxMenuBar notifications
// ---------------------------------------------------------------------------

bool wxMenuBar::OnDelete(wxMenu *a_menu, int pos)
{
    if ( !m_menuBarFrame )
        return TRUE;

    if ( ::RemoveMenu((HMENU)m_hMenu, (UINT)pos, MF_BYPOSITION) )
    {
        // VZ: I'm not sure about what's going on here, so I leave an assert
        wxASSERT_MSG( m_menus[pos] == a_menu, wxT("what is this parameter for??") );

        a_menu->Detach();

        if ( m_menuBarFrame )
            Refresh();

        return TRUE;
    }
    else
    {
        wxLogLastError("RemoveMenu");
    }

    return FALSE;
}

bool wxMenuBar::OnAppend(wxMenu *a_menu, const wxChar *title)
{
    WXHMENU submenu = a_menu->GetHMenu();
    if ( !submenu )
        return FALSE;

    if ( !m_menuBarFrame )
        return TRUE;

    a_menu->Attach(this);

    if ( !::AppendMenu(GetHmenu(), MF_POPUP | MF_STRING,
                       (UINT)submenu, title) )
    {
        wxLogLastError(wxT("AppendMenu"));
    }

    Refresh();

    return TRUE;
}

// ---------------------------------------------------------------------------
// wxMenuBar construction
// ---------------------------------------------------------------------------

int wxMenuBar::FindMenu(const wxString& title)
{
    wxString menuTitle = wxStripMenuCodes(title);

    size_t count = GetMenuCount();
    for ( size_t i = 0; i < count; i++ )
    {
        wxString title = wxStripMenuCodes(m_titles[i]);
        if ( menuTitle == title )
            return i; 
    }

    return wxNOT_FOUND;

}

wxMenu *wxMenuBar::Replace(size_t pos, wxMenu *menu, const wxString& title)
{
    if ( m_menuBarFrame )
    {
        wxFAIL_MSG(wxT("not implemented"));

        return NULL;
    }
    else
    {
        wxMenu *menuOld = wxMenuBarBase::Replace(pos, menu, title);
        if ( menuOld )
        {
            m_titles[pos] = title;
        }

        return menuOld;
    }
}

bool wxMenuBar::Insert(size_t pos, wxMenu *menu, const wxString& title)
{
    if ( m_menuBarFrame )
    {
        wxFAIL_MSG(wxT("not implemented"));

        return FALSE;
    }
    else
    {
        if ( !wxMenuBarBase::Insert(pos, menu, title) )
            return FALSE;

        m_titles.Insert(title, pos);

        return TRUE;
    }
}

bool wxMenuBar::Append(wxMenu * menu, const wxString& title)
{
    if ( !wxMenuBarBase::Append(menu, title) )
        return FALSE;

    // menu is already appended, ignore errors
    (void)OnAppend(menu, title);

    m_titles.Add(title);

    menu->SetParent(this);

    return TRUE;
}

wxMenu *wxMenuBar::Remove(size_t pos)
{
    wxMenu *menu = wxMenuBarBase::Remove(pos);
    if ( !menu )
        return NULL;

    menu->SetParent(NULL);

    // the menu is deleted from the list anyhow, so we have to ignore all
    // possible errors here
    (void)OnDelete(menu, pos);

    m_titles.Remove(pos);

    return menu;
}

void wxMenuBar::Attach(wxFrame *frame)
{
    wxASSERT_MSG( !m_menuBarFrame, wxT("menubar already attached!") );

    m_menuBarFrame = frame;

#if wxUSE_ACCEL
    // create the accel table - we consider that the menubar construction is
    // finished
    size_t nAccelCount = 0;
    size_t i, count = GetMenuCount();
    for ( i = 0; i < count; i++ )
    {
        nAccelCount += m_menus[i]->GetAccelCount();
    }

    if ( nAccelCount )
    {
        wxAcceleratorEntry *accelEntries = new wxAcceleratorEntry[nAccelCount];

        nAccelCount = 0;
        for ( i = 0; i < count; i++ )
        {
            nAccelCount += m_menus[i]->CopyAccels(&accelEntries[nAccelCount]);
        }

        m_accelTable = wxAcceleratorTable(nAccelCount, accelEntries);

        delete [] accelEntries;
    }
#endif // wxUSE_ACCEL
}

void wxMenuBar::Detach()
{
//    ::DestroyMenu((HMENU)m_hMenu);
    m_hMenu = (WXHMENU)NULL;
    m_menuBarFrame = NULL;
}


// ---------------------------------------------------------------------------
// wxMenuBar searching for menu items
// ---------------------------------------------------------------------------

// Find the itemString in menuString, and return the item id or wxNOT_FOUND
int wxMenuBar::FindMenuItem(const wxString& menuString,
                            const wxString& itemString) const
{
    wxString menuLabel = wxStripMenuCodes(menuString);
    size_t count = GetMenuCount();
    for ( size_t i = 0; i < count; i++ )
    {
        wxString title = wxStripMenuCodes(m_titles[i]);
        if ( menuString == title )
            return m_menus[i]->FindItem(itemString);
    }

    return wxNOT_FOUND;
}

wxMenuItem *wxMenuBar::FindItem(int id, wxMenu **itemMenu) const
{
    if ( itemMenu )
        *itemMenu = NULL;

    wxMenuItem *item = NULL;
    size_t count = GetMenuCount();
    for ( size_t i = 0; !item && (i < count); i++ )
    {
        item = m_menus[i]->FindItemForId(id, itemMenu);
    }

    return item;
}


// ----------------------------------------------------------------------------
// helper functions
// ----------------------------------------------------------------------------

wxWindow *wxMenu::GetWindow() const
{
    if ( m_pInvokingWindow != NULL )
        return m_pInvokingWindow;
    else if ( m_menuBar != NULL)
        return m_menuBar->GetFrame();

    return NULL;
}

WXHMENU wxMenu::GetHMenu() const
{
    if ( m_hMenu != 0 )
        return m_hMenu;
    else if ( m_savehMenu != 0 )
        return m_savehMenu;

    wxFAIL_MSG(wxT("wxMenu without HMENU"));

    return 0;
}

// Update a menu and all submenus recursively. source is the object that has
// the update event handlers defined for it. If NULL, the menu or associated
// window will be used.
void wxMenu::UpdateUI(wxEvtHandler* source)
{
    if (!source && GetInvokingWindow())
        source = GetInvokingWindow()->GetEventHandler();
    if (!source)
        source = GetEventHandler();
    if (!source)
        source = this;

    wxNode* node = GetItems().First();
    while (node)
    {
        wxMenuItem* item = (wxMenuItem*) node->Data();
        if ( !item->IsSeparator() )
        {
            wxWindowID id = item->GetId();
            wxUpdateUIEvent event(id);
            event.SetEventObject( source );

            if (source->ProcessEvent(event))
            {
                if (event.GetSetText())
                    SetLabel(id, event.GetText());
                if (event.GetSetChecked())
                    Check(id, event.GetChecked());
                if (event.GetSetEnabled())
                    Enable(id, event.GetEnabled());
            }

            if (item->GetSubMenu())
                item->GetSubMenu()->UpdateUI(source);
        }
        node = node->Next();
    }
}
