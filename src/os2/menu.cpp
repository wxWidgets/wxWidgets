/////////////////////////////////////////////////////////////////////////////
// Name:        menu.cpp
// Purpose:     wxMenu, wxMenuBar, wxMenuItem
// Author:      David Webster
// Modified by:
// Created:     10/10/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "menu.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/frame.h"
    #include "wx/menu.h"
    #include "wx/utils.h"
    #include "wx/intl.h"
    #include "wx/log.h"
#endif

#if wxUSE_OWNER_DRAWN
    #include "wx/ownerdrw.h"
#endif

#include "wx/os2/private.h"

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

    IMPLEMENT_DYNAMIC_CLASS(wxMenu, wxEvtHandler)
    IMPLEMENT_DYNAMIC_CLASS(wxMenuBar, wxEvtHandler)

// ============================================================================
// implementation
// ============================================================================

// ---------------------------------------------------------------------------
// wxMenu construction, adding and removing menu items
// ---------------------------------------------------------------------------

// Construct a menu with optional title (then use append)
void wxMenu::Init()
{
    m_doBreak = FALSE;

    // create the menu
    m_hMenu = (WXHMENU)0; // CreatePopupMenu();
    if ( !m_hMenu )
    {
        wxLogLastError("CreatePopupMenu");
    }

    // if we have a title, insert it in the beginning of the menu
    if ( !!m_title )
    {
        Append(idMenuTitle, m_title);
        AppendSeparator();
    }
}

// The wxWindow destructor will take care of deleting the submenus.
wxMenu::~wxMenu()
{
    // we should free Windows resources only if Windows doesn't do it for us
    // which happens if we're attached to a menubar or a submenu of another
    // menu
    if ( !IsAttached() && !GetParent() )
    {
/*
        if ( !::DestroyMenu(GetHmenu()) )
        {
            wxLogLastError("DestroyMenu");
        }
*/
    }

#if wxUSE_ACCEL
    // delete accels
    WX_CLEAR_ARRAY(m_accels);
#endif // wxUSE_ACCEL
}

void wxMenu::Break()
{
    // this will take effect during the next call to Append()
    m_doBreak = TRUE;
}

#if wxUSE_ACCEL

int wxMenu::FindAccel(int id) const
{
    size_t n, count = m_accels.GetCount();
    for ( n = 0; n < count; n++ )
    {
        if ( m_accels[n]->m_command == id )
            return n;
    }

    return wxNOT_FOUND;
}

void wxMenu::UpdateAccel(wxMenuItem *item)
{
    // find the (new) accel for this item
    wxAcceleratorEntry *accel = wxGetAccelFromString(item->GetText());
    if ( accel )
        accel->m_command = item->GetId();

    // find the old one
    int n = FindAccel(item->GetId());
    if ( n == wxNOT_FOUND )
    {
        // no old, add new if any
        if ( accel )
            m_accels.Add(accel);
        else
            return;     // skipping RebuildAccelTable() below
    }
    else
    {
        // replace old with new or just remove the old one if no new
        delete m_accels[n];
        if ( accel )
            m_accels[n] = accel;
        else
            m_accels.Remove(n);
    }

    if ( IsAttached() )
    {
        m_menuBar->RebuildAccelTable();
    }
}

#endif // wxUSE_ACCEL

// append a new item or submenu to the menu
bool wxMenu::DoInsertOrAppend(wxMenuItem *pItem, size_t pos)
{
#if wxUSE_ACCEL
    UpdateAccel(pItem);
#endif // wxUSE_ACCEL

    UINT flags = 0;
// TODO:
/*
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
        wxASSERT_MSG( submenu->GetHMenu(), wxT("invalid submenu") );

        submenu->SetParent(this);

        id = (UINT)submenu->GetHMenu();

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

    BOOL ok;
    if ( pos == (size_t)-1 )
    {
        ok = ::AppendMenu(GetHmenu(), flags, id, pData);
    }
    else
    {
        ok = ::InsertMenu(GetHmenu(), pos, flags | MF_BYPOSITION, id, pData);
    }

    if ( !ok )
    {
        wxLogLastError("Insert or AppendMenu");

        return FALSE;
    }
    else
    {
        // if we just appended the title, highlight it
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

        // if we're already attached to the menubar, we must update it
        if ( IsAttached() )
        {
            m_menuBar->Refresh();
        }

        return TRUE;
    }
*/
    return FALSE;
}

bool wxMenu::DoAppend(wxMenuItem *item)
{
    return wxMenuBase::DoAppend(item) && DoInsertOrAppend(item);
}

bool wxMenu::DoInsert(size_t pos, wxMenuItem *item)
{
    return wxMenuBase::DoInsert(pos, item) && DoInsertOrAppend(item, pos);
}

wxMenuItem *wxMenu::DoRemove(wxMenuItem *item)
{
    // we need to find the items position in the child list
    size_t pos;
    wxMenuItemList::Node *node = GetMenuItems().GetFirst();
    for ( pos = 0; node; pos++ )
    {
        if ( node->GetData() == item )
            break;

        node = node->GetNext();
    }

    // DoRemove() (unlike Remove) can only be called for existing item!
    wxCHECK_MSG( node, NULL, wxT("bug in wxMenu::Remove logic") );

#if wxUSE_ACCEL
    // remove the corresponding accel from the accel table
    int n = FindAccel(item->GetId());
    if ( n != wxNOT_FOUND )
    {
        delete m_accels[n];

        m_accels.Remove(n);
    }
    //else: this item doesn't have an accel, nothing to do
#endif // wxUSE_ACCEL
/*
    // remove the item from the menu
    if ( !::RemoveMenu(GetHmenu(), (UINT)pos, MF_BYPOSITION) )
    {
        wxLogLastError("RemoveMenu");
    }
*/
    if ( IsAttached() )
    {
        // otherwise, the chane won't be visible
        m_menuBar->Refresh();
    }

    // and from internal data structures
    return wxMenuBase::DoRemove(item);
}

// ---------------------------------------------------------------------------
// accelerator helpers
// ---------------------------------------------------------------------------

#if wxUSE_ACCEL

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
// set wxMenu title
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
/*
            if ( !::InsertMenu(hMenu, 0u, MF_BYPOSITION | MF_STRING,
                               (unsigned)idMenuTitle, m_title) ||
                 !::InsertMenu(hMenu, 1u, MF_BYPOSITION, (unsigned)-1, NULL) )
            {
                wxLogLastError("InsertMenu");
            }
*/
        }
    }
    else
    {
        if ( label.IsEmpty() )
        {
/*
            // remove the title and the separator after it
            if ( !RemoveMenu(hMenu, 0, MF_BYPOSITION) ||
                 !RemoveMenu(hMenu, 0, MF_BYPOSITION) )
            {
                wxLogLastError("RemoveMenu");
            }
*/
        }
        else
        {
/*
            // modify the title
            if ( !ModifyMenu(hMenu, 0u,
                             MF_BYPOSITION | MF_STRING,
                             (unsigned)idMenuTitle, m_title) )
            {
                wxLogLastError("ModifyMenu");
            }
*/
        }
    }
/*
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
#endif // Win32
*/
}

// ---------------------------------------------------------------------------
// event processing
// ---------------------------------------------------------------------------

bool wxMenu::OS2Command(WXUINT WXUNUSED(param), WXWORD id)
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

#if WXWIN_COMPATIBILITY
    // Try a callback
    if (m_callback)
    {
        (void)(*(m_callback))(*this, event);
        processed = TRUE;
    }
#endif // WXWIN_COMPATIBILITY

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
// other
// ---------------------------------------------------------------------------

void wxMenu::Attach(wxMenuBar *menubar)
{
    // menu can be in at most one menubar because otherwise they would both
    // delete the menu pointer
    wxASSERT_MSG( !m_menuBar, wxT("menu belongs to 2 menubars, expect a crash") );

    m_menuBar = menubar;
}

void wxMenu::Detach()
{
    wxASSERT_MSG( m_menuBar, wxT("can't detach menu if it's not attached") );

    m_menuBar = NULL;
}

wxWindow *wxMenu::GetWindow() const
{
    if ( m_invokingWindow != NULL )
        return m_invokingWindow;
    else if ( m_menuBar != NULL)
        return m_menuBar->GetFrame();

    return NULL;
}

// ---------------------------------------------------------------------------
// Menu Bar
// ---------------------------------------------------------------------------

void wxMenuBar::Init()
{
    m_eventHandler = this;
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

void wxMenuBar::Refresh(
  bool                              WXUNUSED(bEraseBackground)
, const wxRect*                     WXUNUSED(pRect)
)
{
    wxCHECK_RET( IsAttached(), wxT("can't refresh unatteched menubar") );

//    DrawMenuBar(GetHwndOf(m_menuBarFrame));
}

WXHMENU wxMenuBar::Create()
{
    if (m_hMenu != 0 )
        return m_hMenu;

    wxCHECK_MSG( !m_hMenu, TRUE, wxT("menubar already created") );

// TODO:
/*

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
*/
    return (WXHMENU)0;
}

// ---------------------------------------------------------------------------
// wxMenuBar functions to work with the top level submenus
// ---------------------------------------------------------------------------

// NB: we don't support owner drawn top level items for now, if we do these
//     functions would have to be changed to use wxMenuItem as well

void wxMenuBar::EnableTop(size_t pos, bool enable)
{
    wxCHECK_RET( IsAttached(), wxT("doesn't work with unattached menubars") );

//    int flag = enable ? MF_ENABLED : MF_GRAYED;

//    EnableMenuItem((HMENU)m_hMenu, pos, MF_BYPOSITION | flag);

    Refresh();
}

void wxMenuBar::SetLabelTop(size_t pos, const wxString& label)
{
    wxCHECK_RET( pos < GetMenuCount(), wxT("invalid menu index") );

    m_titles[pos] = label;

    if ( !IsAttached() )
    {
        return;
    }
    //else: have to modify the existing menu

// TODO:
/*
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
        flagsOld &= 0xff;
        id = (UINT)::GetSubMenu((HMENU)m_hMenu, pos);
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
*/
    Refresh();
}

wxString wxMenuBar::GetLabelTop(size_t pos) const
{
    wxCHECK_MSG( pos < GetMenuCount(), wxEmptyString,
                 wxT("invalid menu index in wxMenuBar::GetLabelTop") );

    return m_titles[pos];
}

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

// ---------------------------------------------------------------------------
// wxMenuBar construction
// ---------------------------------------------------------------------------

wxMenu *wxMenuBar::Replace(size_t pos, wxMenu *menu, const wxString& title)
{
    wxMenu *menuOld = wxMenuBarBase::Replace(pos, menu, title);
    if ( !menuOld )
        return FALSE;
    m_titles[pos] = title;
// TODO:
/*
    if ( IsAttached() )
    {
        // can't use ModifyMenu() because it deletes the submenu it replaces
        if ( !::RemoveMenu(GetHmenu(), (UINT)pos, MF_BYPOSITION) )
        {
            wxLogLastError("RemoveMenu");
        }

        if ( !::InsertMenu(GetHmenu(), (UINT)pos,
                           MF_BYPOSITION | MF_POPUP | MF_STRING,
                           (UINT)GetHmenuOf(menu), title) )
        {
            wxLogLastError("InsertMenu");
        }

#if wxUSE_ACCEL
        if ( menuOld->HasAccels() || menu->HasAccels() )
        {
            // need to rebuild accell table
            RebuildAccelTable();
        }
#endif // wxUSE_ACCEL

        Refresh();
    }
*/
    return menuOld;
}

bool wxMenuBar::Insert(size_t pos, wxMenu *menu, const wxString& title)
{
    if ( !wxMenuBarBase::Insert(pos, menu, title) )
        return FALSE;

    m_titles.Insert(title, pos);

    menu->Attach(this);
// TODO:
/*
    if ( IsAttached() )
    {
        if ( !::InsertMenu(GetHmenu(), pos,
                           MF_BYPOSITION | MF_POPUP | MF_STRING,
                           (UINT)GetHmenuOf(menu), title) )
        {
            wxLogLastError("InsertMenu");
        }

#if wxUSE_ACCEL
        if ( menu->HasAccels() )
        {
            // need to rebuild accell table
            RebuildAccelTable();
        }
#endif // wxUSE_ACCEL

        Refresh();
    }
*/
    return TRUE;
}

bool wxMenuBar::Append(wxMenu *menu, const wxString& title)
{
    WXHMENU submenu = menu ? menu->GetHMenu() : 0;
    wxCHECK_MSG( submenu, FALSE, wxT("can't append invalid menu to menubar") );

    if ( !wxMenuBarBase::Append(menu, title) )
        return FALSE;

    menu->Attach(this);

    m_titles.Add(title);
// TODO:
/*
    if ( IsAttached() )
    {
        if ( !::AppendMenu(GetHmenu(), MF_POPUP | MF_STRING,
                           (UINT)submenu, title) )
        {
            wxLogLastError(wxT("AppendMenu"));
        }

#if wxUSE_ACCEL
        if ( menu->HasAccels() )
        {
            // need to rebuild accell table
            RebuildAccelTable();
        }
#endif // wxUSE_ACCEL

        Refresh();
    }
*/
    return TRUE;
}

wxMenu *wxMenuBar::Remove(size_t pos)
{
    wxMenu *menu = wxMenuBarBase::Remove(pos);
    if ( !menu )
        return NULL;
// TODO:
/*
    if ( IsAttached() )
    {
        if ( !::RemoveMenu(GetHmenu(), (UINT)pos, MF_BYPOSITION) )
        {
            wxLogLastError("RemoveMenu");
        }

        menu->Detach();

#if wxUSE_ACCEL
        if ( menu->HasAccels() )
        {
            // need to rebuild accell table
            RebuildAccelTable();
        }
#endif // wxUSE_ACCEL

        Refresh();
    }

    m_titles.Remove(pos);
*/
    return menu;
}

#if wxUSE_ACCEL

void wxMenuBar::RebuildAccelTable()
{
    // merge the accelerators of all menus into one accel table
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
}

#endif // wxUSE_ACCEL

void wxMenuBar::Attach(wxFrame *frame)
{
    wxASSERT_MSG( !IsAttached(), wxT("menubar already attached!") );

    m_menuBarFrame = frame;

#if wxUSE_ACCEL
    RebuildAccelTable();
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
        item = m_menus[i]->FindItem(id, itemMenu);
    }

    return item;
}

