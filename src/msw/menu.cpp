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

// convenience macros
#define GetHMENU()          ((HMENU)GetHMenu())
#define GetHMenuOf(menu)    ((HMENU)menu->GetHMenu())

// ============================================================================
// implementation
// ============================================================================

// ---------------------------------------------------------------------------
// wxMenu construction, adding and removing menu items
// ---------------------------------------------------------------------------

// Construct a menu with optional title (then use append)
wxMenu::wxMenu(const wxString& title, const wxFunction func)
      : m_title(title)
{
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

#if WXWIN_COMPATIBILITY
    Callback(func);
#endif
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
    wxCHECK_RET( pItem != NULL, _T("can't append NULL item to the menu") );

#if wxUSE_ACCEL
    // check for accelerators: they are given after '\t'
    wxString label = pItem->GetName();
    int posTab = label.Find(_T('\t'));
    if ( posTab != wxNOT_FOUND ) {
        // parse the accelerator string
        int keyCode = 0;
        int accelFlags = wxACCEL_NORMAL;
        wxString current;
        for ( size_t n = (size_t)posTab + 1; n < label.Len(); n++ ) {
            if ( (label[n] == '+') || (label[n] == '-') ) {
                if ( current == _("ctrl") )
                    accelFlags |= wxACCEL_CTRL;
                else if ( current == _("alt") )
                    accelFlags |= wxACCEL_ALT;
                else if ( current == _("shift") )
                    accelFlags |= wxACCEL_SHIFT;
                else {
                    wxLogDebug(_T("Unknown accel modifier: '%s'"),
                               current.c_str());
                }

                current.Empty();
            }
            else {
                current += wxTolower(label[n]);
            }
        }

        if ( current.IsEmpty() ) {
            wxLogDebug(_T("No accel key found, accel string ignored."));
        }
        else {
            if ( current.Len() == 1 ) {
                // it's a letter
                keyCode = wxToupper(current[0U]);
            }
            else {
                // it should be a function key
                if ( current[0U] == 'f' && isdigit(current[1U]) &&
                     (current.Len() == 2 ||
                     (current.Len() == 3 && isdigit(current[2U]))) ) {
                    int n;
                    wxSscanf(current.c_str() + 1, _T("%d"), &n);

                    keyCode = VK_F1 + n - 1;
                }
                else {
                    wxLogDebug(_T("Unrecognized accel key '%s', accel "
                                  "string ignored."), current.c_str());
                }
            }
        }

        if ( keyCode ) {
            // do add an entry
            m_accelKeyCodes.Add(keyCode);
            m_accelFlags.Add(accelFlags);
            m_accelIds.Add(pItem->GetId());
        }
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
        pData = label;
    }

    if ( !::AppendMenu(GetHMENU(), flags, id, pData) )
    {
        wxLogLastError("AppendMenu");
    }
    else
    {
#ifdef __WIN32__
        if ( id == idMenuTitle )
        {
            // visually select the menu title
            MENUITEMINFO mii;
            mii.cbSize = sizeof(mii);
            mii.fMask = MIIM_STATE;
            mii.fState = MFS_DEFAULT;

            if ( !SetMenuItemInfo(GetHMENU(), (unsigned)id, FALSE, &mii) )
            {
                wxLogLastError(_T("SetMenuItemInfo"));
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

    wxCHECK_RET( node, _T("wxMenu::Delete(): item doesn't exist") );

    HMENU menu = GetHMENU();

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
        (*accels++).Set(m_accelFlags[n], m_accelKeyCodes[n], m_accelIds[n]);
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
    wxCHECK_RET( item != NULL, _T("can't enable non-existing menu item") );

    item->Enable(Flag);
}

bool wxMenu::IsEnabled(int id) const
{
    wxMenuItem *item = FindItemForId(id);
    wxCHECK_MSG( item != NULL, FALSE, _T("invalid item id") );

    return item->IsEnabled();
}

void wxMenu::Check(int id, bool Flag)
{
    wxMenuItem *item = FindItemForId(id);
    wxCHECK_RET( item != NULL, _T("can't get status of non-existing menu item") );

    item->Check(Flag);
}

bool wxMenu::IsChecked(int id) const
{
    wxMenuItem *item = FindItemForId(id);
    wxCHECK_MSG( item != NULL, FALSE, _T("invalid item id") );

    return item->IsChecked();
}

void wxMenu::SetLabel(int id, const wxString& label)
{
    wxMenuItem *item = FindItemForId(id) ;
    wxCHECK_RET( item, _T("wxMenu::SetLabel: no such item") );

    item->SetName(label);
}

wxString wxMenu::GetLabel(int id) const
{
    wxString label;
    wxMenuItem *pItem = FindItemForId(id) ;
    if (pItem)
        label = pItem->GetName() ;
    else
        wxFAIL_MSG(_T("wxMenu::GetLabel: item doesn't exist"));

    return label;
}

void wxMenu::SetHelpString(int itemId, const wxString& helpString)
{
    wxMenuItem *item = FindItemForId (itemId);
    if (item)
        item->SetHelp(helpString);
    else
        wxFAIL_MSG(_T("wxMenu::SetHelpString: item doesn't exist"));
}

wxString wxMenu::GetHelpString (int itemId) const
{
    wxString help;
    wxMenuItem *item = FindItemForId (itemId);
    if (item)
        help = item->GetHelp();
    else
        wxFAIL_MSG(_T("wxMenu::GetHelpString: item doesn't exist"));

    return help;
}

// ---------------------------------------------------------------------------
// wxMenu title
// ---------------------------------------------------------------------------

void wxMenu::SetTitle(const wxString& label)
{
    bool hasNoTitle = m_title.IsEmpty();
    m_title = label;

    HMENU hMenu = GetHMENU();

    if ( hasNoTitle )
    {
        if ( !label.IsEmpty() )
        {
            if ( !InsertMenu(hMenu, 0u, MF_BYPOSITION | MF_STRING,
                             (unsigned)idMenuTitle, m_title) ||
                 !InsertMenu(hMenu, 1u, MF_BYPOSITION, (unsigned)-1, NULL) )
            {
                wxLogLastError(_T("InsertMenu"));
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
            wxString label = wxStripMenuCodes(item->GetName());
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

bool wxWindow::PopupMenu(wxMenu *menu, int x, int y)
{
    menu->SetInvokingWindow(this);
    menu->UpdateUI();

    HWND hWnd = (HWND) GetHWND();
    HMENU hMenu = (HMENU)menu->GetHMenu();
    POINT point;
    point.x = x;
    point.y = y;
    ::ClientToScreen(hWnd, &point);
    wxCurrentPopupMenu = menu;
    ::TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, point.x, point.y, 0, hWnd, NULL);
    wxYield();
    wxCurrentPopupMenu = NULL;

    menu->SetInvokingWindow(NULL);

    return TRUE;
}

void wxMenu::Attach(wxMenuBar *menubar)
{
    // menu can be in at most one menubar because otherwise they would both
    // delete the menu pointer
    wxASSERT_MSG( !m_menuBar, _T("menu belongs to 2 menubars, expect a crash") );

    m_menuBar = menubar;
    m_savehMenu = m_hMenu;
    m_hMenu = 0;
}

void wxMenu::Detach()
{
    wxASSERT_MSG( m_menuBar, _T("can't detach menu if it's not attached") );

    m_hMenu = m_savehMenu;
    m_savehMenu = 0;
}

// ---------------------------------------------------------------------------
// Menu Bar
// ---------------------------------------------------------------------------

void wxMenuBar::Init()
{
    m_eventHandler = this;
    m_menuCount = 0;
    m_menus = NULL;
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

    m_menuCount = count;
    m_menus = menus;
    m_titles = new wxString[count];

    int i;
    for ( i = 0; i < count; i++ )
        m_titles[i] = titles[i];

    for ( i = 0; i < count; i++ )
        m_menus[i]->Attach(this);
}

wxMenuBar::~wxMenuBar()
{
    for ( int i = 0; i < m_menuCount; i++ )
    {
        delete m_menus[i];
    }

    delete[] m_menus;
    delete[] m_titles;
}

// ---------------------------------------------------------------------------
// wxMenuBar helpers
// ---------------------------------------------------------------------------

void wxMenuBar::Refresh()
{
    wxCHECK_RET( m_menuBarFrame, _T("can't refresh a menubar withotu a frame") );

    DrawMenuBar((HWND)m_menuBarFrame->GetHWND()) ;
}

WXHMENU wxMenuBar::Create()
{
    wxCHECK_MSG( !m_hMenu, TRUE, _T("menubar already created") );

    m_hMenu = (WXHMENU)::CreateMenu();

    if ( !m_hMenu )
    {
        wxLogLastError("CreateMenu");
    }
    else
    {
        for ( int i = 0; i < m_menuCount; i++ )
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
// wxMenuBar functions forwarded to wxMenuItem
// ---------------------------------------------------------------------------

// Must only be used AFTER menu has been attached to frame,
// otherwise use individual menus to enable/disable items
void wxMenuBar::Enable(int id, bool enable)
{
    wxMenu *itemMenu = NULL;
    wxMenuItem *item = FindItemForId(id, &itemMenu) ;

    wxCHECK_RET( item, _T("attempt to enable an item which doesn't exist") );

    item->Enable(enable);
}

void wxMenuBar::EnableTop(int pos, bool enable)
{
    int flag = enable ? MF_ENABLED : MF_GRAYED;;

    EnableMenuItem((HMENU)m_hMenu, pos, MF_BYPOSITION | flag);
}

// Must only be used AFTER menu has been attached to frame,
// otherwise use individual menus
void wxMenuBar::Check(int id, bool check)
{
    wxMenu *itemMenu = NULL;
    wxMenuItem *item = FindItemForId(id, &itemMenu) ;

    wxCHECK_RET( item, _T("attempt to check an item which doesn't exist") );
    wxCHECK_RET( item->IsCheckable(), _T("attempt to check an uncheckable item") );

    item->Check(check);
}

bool wxMenuBar::IsChecked(int id) const
{
    wxMenu *itemMenu = NULL;
    wxMenuItem *item = FindItemForId(id, &itemMenu) ;

    wxCHECK_MSG( item, FALSE, _T("wxMenuBar::IsChecked(): no such item") );

    int flag = ::GetMenuState(GetHMenuOf(itemMenu), id, MF_BYCOMMAND);

    return (flag & MF_CHECKED) != 0;
}

bool wxMenuBar::IsEnabled(int id) const
{
    wxMenu *itemMenu = NULL;
    wxMenuItem *item = FindItemForId(id, &itemMenu) ;

    wxCHECK_MSG( item, FALSE, _T("wxMenuBar::IsEnabled(): no such item") );

    int flag = ::GetMenuState(GetHMenuOf(itemMenu), id, MF_BYCOMMAND) ;

    return (flag & MF_ENABLED) != 0;
}

void wxMenuBar::SetLabel(int id, const wxString& label)
{
    wxMenu *itemMenu = NULL;
    wxMenuItem *item = FindItemForId(id, &itemMenu) ;

    wxCHECK_RET( item, _T("wxMenuBar::SetLabel(): no such item") );

    item->SetName(label);
}

wxString wxMenuBar::GetLabel(int id) const
{
    wxMenu *itemMenu = NULL;
    wxMenuItem *item = FindItemForId(id, &itemMenu) ;

    wxCHECK_MSG( item, _T(""), _T("wxMenuBar::GetLabel(): no such item") );

    return item->GetName();
}

void wxMenuBar::SetHelpString (int id, const wxString& helpString)
{
    wxMenu *itemMenu = NULL;
    wxMenuItem *item = FindItemForId(id, &itemMenu) ;

    wxCHECK_RET( item, _T("wxMenuBar::SetHelpString(): no such item") );

    item->SetHelp(helpString);
}

wxString wxMenuBar::GetHelpString (int id) const
{
    wxMenu *itemMenu = NULL;
    wxMenuItem *item = FindItemForId(id, &itemMenu) ;

    wxCHECK_MSG( item, _T(""), _T("wxMenuBar::GetHelpString(): no such item") );

    return item->GetHelp();
}

// ---------------------------------------------------------------------------
// wxMenuBar functions to work with the top level submenus
// ---------------------------------------------------------------------------

// NB: we don't support owner drawn top level items for now, if we do these
//     functions would have to be changed to use wxMenuItem as well

void wxMenuBar::SetLabelTop(int pos, const wxString& label)
{
    UINT id;
    UINT flagsOld = ::GetMenuState((HMENU)m_hMenu, pos, MF_BYPOSITION);
    if ( flagsOld == 0xFFFFFFFF )
    {
        wxLogLastError(_T("GetMenuState"));

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

    if ( ::ModifyMenu(GetHMENU(), pos, MF_BYPOSITION | MF_STRING | flagsOld,
                      id, label) == 0xFFFFFFFF )
    {
        wxLogLastError("ModifyMenu");
    }
}

wxString wxMenuBar::GetLabelTop(int pos) const
{
    int len = ::GetMenuString((HMENU)m_hMenu, pos, NULL, 0, MF_BYCOMMAND);

    len++;  // for the NUL character
    wxString label;
    ::GetMenuString(GetHMENU(), pos, label.GetWriteBuf(len), len, MF_BYCOMMAND);
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
        wxASSERT_MSG( m_menus[pos] == a_menu, _T("what is this parameter for??") );

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

    if ( !::AppendMenu(GetHMENU(), MF_POPUP | MF_STRING,
                       (UINT)submenu, title) )
    {
        wxLogLastError(_T("AppendMenu"));
    }

    Refresh();

    return TRUE;
}

// ---------------------------------------------------------------------------
// wxMenuBar construction
// ---------------------------------------------------------------------------

void wxMenuBar::Append (wxMenu * menu, const wxString& title)
{
    if (!OnAppend(menu, title))
        return;

    m_menuCount ++;
    wxMenu **new_menus = new wxMenu *[m_menuCount];
    wxString *new_titles = new wxString[m_menuCount];
    int i;

    for (i = 0; i < m_menuCount - 1; i++)
    {
        new_menus[i] = m_menus[i];
        m_menus[i] = NULL;
        new_titles[i] = m_titles[i];
        m_titles[i] = _T("");
    }
    if (m_menus)
    {
        delete[]m_menus;
        delete[]m_titles;
    }
    m_menus = new_menus;
    m_titles = new_titles;

    m_menus[m_menuCount - 1] = (wxMenu *)menu;
    m_titles[m_menuCount - 1] = title;

    menu->SetParent(this);
}

void wxMenuBar::Delete(wxMenu * menu, int i)
{
    int j;
    int ii = (int) i;

    if (menu != 0) {
        for (ii = 0; ii < m_menuCount; ii++) {
            if (m_menus[ii] == menu)
                break;
        }
        if (ii >= m_menuCount)
            return;
    } else {
        if (ii < 0 || ii >= m_menuCount)
            return;
        menu = m_menus[ii];
    }

    if (!OnDelete(menu, ii))
        return;

    menu->SetParent(NULL);

    -- m_menuCount;
    for (j = ii; j < m_menuCount; j++) {
        m_menus[j] = m_menus[j + 1];
        m_titles[j] = m_titles[j + 1];
    }
}

void wxMenuBar::Attach(wxFrame *frame)
{
    wxASSERT_MSG( !m_menuBarFrame, _T("menubar already attached!") );

    m_menuBarFrame = frame;

#if wxUSE_ACCEL
    // create the accel table - we consider that the menubar construction is
    // finished
    size_t nAccelCount = 0;
    int i;
    for ( i = 0; i < m_menuCount; i++ )
    {
        nAccelCount += m_menus[i]->GetAccelCount();
    }

    if ( nAccelCount )
    {
        wxAcceleratorEntry *accelEntries = new wxAcceleratorEntry[nAccelCount];

        nAccelCount = 0;
        for ( i = 0; i < m_menuCount; i++ )
        {
            nAccelCount += m_menus[i]->CopyAccels(&accelEntries[nAccelCount]);
        }

        m_accelTable = wxAcceleratorTable(nAccelCount, accelEntries);

        delete [] accelEntries;
    }
#endif // wxUSE_ACCEL
}

// ---------------------------------------------------------------------------
// wxMenuBar searching for menu items
// ---------------------------------------------------------------------------

// Find the itemString in menuString, and return the item id or wxNOT_FOUND
int wxMenuBar::FindMenuItem(const wxString& menuString,
                            const wxString& itemString) const
{
    wxString menuLabel = wxStripMenuCodes(menuString);
    for ( int i = 0; i < m_menuCount; i++ )
    {
        wxString title = wxStripMenuCodes(m_titles[i]);
        if ( menuString == title )
            return m_menus[i]->FindItem(itemString);
    }

    return wxNOT_FOUND;
}

wxMenuItem *wxMenuBar::FindItemForId (int id, wxMenu **itemMenu) const
{
    if ( itemMenu )
        *itemMenu = NULL;

    wxMenuItem *item = NULL;
    for ( int i = 0; !item && (i < m_menuCount); i++ )
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

    wxFAIL_MSG(_T("wxMenu without HMENU"));

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
