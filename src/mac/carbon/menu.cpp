/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/menu.cpp
// Purpose:     wxMenu, wxMenuBar, wxMenuItem
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// headers & declarations
// ============================================================================

// wxWidgets headers
// -----------------

#include "wx/wxprec.h"

#include "wx/menu.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/app.h"
    #include "wx/utils.h"
    #include "wx/frame.h"
    #include "wx/menuitem.h"
#endif

#include "wx/mac/uma.h"

// other standard headers
// ----------------------
#include <string.h>

IMPLEMENT_DYNAMIC_CLASS(wxMenu, wxEvtHandler)
IMPLEMENT_DYNAMIC_CLASS(wxMenuBar, wxEvtHandler)

// the (popup) menu title has this special id
static const int idMenuTitle = -3;

static const short kwxMacAppleMenuId = 1 ;


// Find an item given the Macintosh Menu Reference

WX_DECLARE_HASH_MAP(MenuRef, wxMenu*, wxPointerHash, wxPointerEqual, MacMenuMap);

static MacMenuMap wxWinMacMenuList;

wxMenu *wxFindMenuFromMacMenu(MenuRef inMenuRef)
{
    MacMenuMap::iterator node = wxWinMacMenuList.find(inMenuRef);

    return (node == wxWinMacMenuList.end()) ? NULL : node->second;
}

void wxAssociateMenuWithMacMenu(MenuRef inMenuRef, wxMenu *menu) ;
void wxAssociateMenuWithMacMenu(MenuRef inMenuRef, wxMenu *menu)
{
    // adding NULL MenuRef is (first) surely a result of an error and
    // (secondly) breaks menu command processing
    wxCHECK_RET( inMenuRef != (MenuRef) NULL, wxT("attempt to add a NULL MenuRef to menu list") );

    wxWinMacMenuList[inMenuRef] = menu;
}

void wxRemoveMacMenuAssociation(wxMenu *menu) ;
void wxRemoveMacMenuAssociation(wxMenu *menu)
{
   // iterate over all the elements in the class
    MacMenuMap::iterator it;
    for ( it = wxWinMacMenuList.begin(); it != wxWinMacMenuList.end(); ++it )
    {
        if ( it->second == menu )
        {
            wxWinMacMenuList.erase(it);
            break;
        }
    }
}

// ============================================================================
// implementation
// ============================================================================
static void wxMenubarUnsetInvokingWindow( wxMenu *menu ) ;
static void wxMenubarSetInvokingWindow( wxMenu *menu, wxWindow *win );

// Menus

// Construct a menu with optional title (then use append)

#ifdef __DARWIN__
short wxMenu::s_macNextMenuId = 3 ;
#else
short wxMenu::s_macNextMenuId = 2 ;
#endif

static
wxMenu *
_wxMenuAt(const wxMenuList &menuList, size_t pos)
{
    wxMenuList::compatibility_iterator menuIter = menuList.GetFirst();

    while (pos-- > 0)
        menuIter = menuIter->GetNext();

    return menuIter->GetData() ;
}

void wxMenu::Init()
{
    m_doBreak = false;
    m_startRadioGroup = -1;

    // create the menu
    m_macMenuId = s_macNextMenuId++;
    m_hMenu = UMANewMenu(m_macMenuId, m_title, wxFont::GetDefaultEncoding() );

    if ( !m_hMenu )
    {
        wxLogLastError(wxT("UMANewMenu failed"));
    }

    wxAssociateMenuWithMacMenu( (MenuRef)m_hMenu , this ) ;

    // if we have a title, insert it in the beginning of the menu
    if ( !m_title.empty() )
    {
        Append(idMenuTitle, m_title) ;
        AppendSeparator() ;
    }
}

wxMenu::~wxMenu()
{
    wxRemoveMacMenuAssociation( this ) ;
    if (MAC_WXHMENU(m_hMenu))
        ::DisposeMenu(MAC_WXHMENU(m_hMenu));
}

void wxMenu::Break()
{
    // not available on the mac platform
}

void wxMenu::Attach(wxMenuBarBase *menubar)
{
    wxMenuBase::Attach(menubar);

    EndRadioGroup();
}

// function appends a new item or submenu to the menu
// append a new item or submenu to the menu
bool wxMenu::DoInsertOrAppend(wxMenuItem *pItem, size_t pos)
{
    wxASSERT_MSG( pItem != NULL, wxT("can't append NULL item to the menu") );

    if ( pItem->IsSeparator() )
    {
        if ( pos == (size_t)-1 )
            AppendMenuItemTextWithCFString( MAC_WXHMENU(m_hMenu),
                CFSTR(""), kMenuItemAttrSeparator, 0,NULL); 
        else
            InsertMenuItemTextWithCFString( MAC_WXHMENU(m_hMenu),
                CFSTR(""), pos, kMenuItemAttrSeparator, 0); 
    }
    else
    {
        wxMenu *pSubMenu = pItem->GetSubMenu() ;
        if ( pSubMenu != NULL )
        {
            wxASSERT_MSG( pSubMenu->m_hMenu != NULL , wxT("invalid submenu added"));
            pSubMenu->m_menuParent = this ;

            if (wxMenuBar::MacGetInstalledMenuBar() == GetMenuBar())
                pSubMenu->MacBeforeDisplay( true ) ;

            if ( pos == (size_t)-1 )
                UMAAppendSubMenuItem(MAC_WXHMENU(m_hMenu), wxStripMenuCodes(pItem->GetText()), wxFont::GetDefaultEncoding(), pSubMenu->m_macMenuId);
            else
                UMAInsertSubMenuItem(MAC_WXHMENU(m_hMenu), wxStripMenuCodes(pItem->GetText()), wxFont::GetDefaultEncoding(), pos, pSubMenu->m_macMenuId);

            pItem->UpdateItemBitmap() ;
            pItem->UpdateItemStatus() ;
        }
        else
        {
            if ( pos == (size_t)-1 )
            {
                UMAAppendMenuItem(MAC_WXHMENU(m_hMenu), wxT("a") , wxFont::GetDefaultEncoding() );
                pos = CountMenuItems(MAC_WXHMENU(m_hMenu)) ;
            }
            else
            {
                // MacOS counts menu items from 1 and inserts after, therefore having the
                // same effect as wx 0 based and inserting before, we must correct pos
                // after however for updates to be correct
                UMAInsertMenuItem(MAC_WXHMENU(m_hMenu), wxT("a"), wxFont::GetDefaultEncoding(), pos);
                pos += 1 ;
            }

            SetMenuItemCommandID( MAC_WXHMENU(m_hMenu) , pos , wxIdToMacCommand ( pItem->GetId() ) ) ;
            SetMenuItemRefCon( MAC_WXHMENU(m_hMenu) , pos , (URefCon) pItem ) ;
            pItem->UpdateItemText() ;
            pItem->UpdateItemBitmap() ;
            pItem->UpdateItemStatus() ;

            if ( pItem->GetId() == idMenuTitle )
                UMAEnableMenuItem(MAC_WXHMENU(m_hMenu) , pos , false ) ;
        }
    }

    // if we're already attached to the menubar, we must update it
    if ( IsAttached() && GetMenuBar()->IsAttached() )
        GetMenuBar()->Refresh();

    return true ;
}

void wxMenu::EndRadioGroup()
{
    // we're not inside a radio group any longer
    m_startRadioGroup = -1;
}

wxMenuItem* wxMenu::DoAppend(wxMenuItem *item)
{
    wxCHECK_MSG( item, NULL, _T("NULL item in wxMenu::DoAppend") );

    bool check = false;

    if ( item->GetKind() == wxITEM_RADIO )
    {
        int count = GetMenuItemCount();

        if ( m_startRadioGroup == -1 )
        {
            // start a new radio group
            m_startRadioGroup = count;

            // for now it has just one element
            item->SetAsRadioGroupStart();
            item->SetRadioGroupEnd(m_startRadioGroup);

            // ensure that we have a checked item in the radio group
            check = true;
        }
        else // extend the current radio group
        {
            // we need to update its end item
            item->SetRadioGroupStart(m_startRadioGroup);
            wxMenuItemList::compatibility_iterator node = GetMenuItems().Item(m_startRadioGroup);

            if ( node )
            {
                node->GetData()->SetRadioGroupEnd(count);
            }
            else
            {
                wxFAIL_MSG( _T("where is the radio group start item?") );
            }
        }
    }
    else // not a radio item
    {
        EndRadioGroup();
    }

    if ( !wxMenuBase::DoAppend(item) || !DoInsertOrAppend(item) )
        return NULL;

    if ( check )
        // check the item initially
        item->Check(true);

    return item;
}

wxMenuItem* wxMenu::DoInsert(size_t pos, wxMenuItem *item)
{
    if (wxMenuBase::DoInsert(pos, item) && DoInsertOrAppend(item, pos))
        return item;

    return NULL;
}

wxMenuItem *wxMenu::DoRemove(wxMenuItem *item)
{
    // we need to find the items position in the child list
    size_t pos;
    wxMenuItemList::compatibility_iterator node = GetMenuItems().GetFirst();

    for ( pos = 0; node; pos++ )
    {
        if ( node->GetData() == item )
            break;

        node = node->GetNext();
    }

    // DoRemove() (unlike Remove) can only be called for existing item!
    wxCHECK_MSG( node, NULL, wxT("bug in wxMenu::Remove logic") );

    ::DeleteMenuItem(MAC_WXHMENU(m_hMenu) , pos + 1);

    if ( IsAttached() && GetMenuBar()->IsAttached() )
        // otherwise, the change won't be visible
        GetMenuBar()->Refresh();

    // and from internal data structures
    return wxMenuBase::DoRemove(item);
}

void wxMenu::SetTitle(const wxString& label)
{
    m_title = label ;
    UMASetMenuTitle(MAC_WXHMENU(m_hMenu) , label , wxFont::GetDefaultEncoding() ) ;
}

bool wxMenu::ProcessCommand(wxCommandEvent & event)
{
    bool processed = false;

    // Try the menu's event handler
    if ( /* !processed && */ GetEventHandler())
        processed = GetEventHandler()->ProcessEvent(event);

    // Try the window the menu was popped up from
    // (and up through the hierarchy)
    wxWindow *win = GetInvokingWindow();
    if ( !processed && win )
        processed = win->GetEventHandler()->ProcessEvent(event);

    return processed;
}

// ---------------------------------------------------------------------------
// other
// ---------------------------------------------------------------------------

wxWindow *wxMenu::GetWindow() const
{
    if ( m_invokingWindow != NULL )
        return m_invokingWindow;
    else if ( GetMenuBar() != NULL)
        return (wxWindow *) GetMenuBar()->GetFrame();

    return NULL;
}

// helper functions returning the mac menu position for a certain item, note that this is
// mac-wise 1 - based, i.e. the first item has index 1 whereas on MSWin it has pos 0

int wxMenu::MacGetIndexFromId( int id )
{
    size_t pos;
    wxMenuItemList::compatibility_iterator node = GetMenuItems().GetFirst();
    for ( pos = 0; node; pos++ )
    {
        if ( node->GetData()->GetId() == id )
            break;

        node = node->GetNext();
    }

    if (!node)
        return 0;

    return pos + 1 ;
}

int wxMenu::MacGetIndexFromItem( wxMenuItem *pItem )
{
    size_t pos;
    wxMenuItemList::compatibility_iterator node = GetMenuItems().GetFirst();
    for ( pos = 0; node; pos++ )
    {
        if ( node->GetData() == pItem )
            break;

        node = node->GetNext();
    }

    if (!node)
        return 0;

    return pos + 1 ;
}

void wxMenu::MacEnableMenu( bool bDoEnable )
{
    UMAEnableMenuItem(MAC_WXHMENU(m_hMenu) , 0 , bDoEnable ) ;

    ::DrawMenuBar() ;
}

// MacOS needs to know about submenus somewhere within this menu
// before it can be displayed, also hide special menu items
// like preferences that are handled by the OS
void wxMenu::MacBeforeDisplay( bool isSubMenu )
{
    wxMenuItem* previousItem = NULL ;
    size_t pos ;
    wxMenuItemList::compatibility_iterator node;
    wxMenuItem *item;

    for (pos = 0, node = GetMenuItems().GetFirst(); node; node = node->GetNext(), pos++)
    {
        item = (wxMenuItem *)node->GetData();
        wxMenu* subMenu = item->GetSubMenu() ;
        if (subMenu)
        {
            subMenu->MacBeforeDisplay( true ) ;
        }
        else // normal item
        {
#if TARGET_CARBON
            // what we do here is to hide the special items which are
            // shown in the application menu anyhow -- it doesn't make
            // sense to show them in their normal place as well
            if ( item->GetId() == wxApp::s_macAboutMenuItemId ||
                ( UMAGetSystemVersion() >= 0x1000 && (
                    item->GetId() == wxApp::s_macPreferencesMenuItemId ||
                    item->GetId() == wxApp::s_macExitMenuItemId ) ) )

            {
                ChangeMenuItemAttributes( MAC_WXHMENU( GetHMenu() ),
                                          pos + 1, kMenuItemAttrHidden, 0 );

                // also check for a separator which was used just to
                // separate this item from the others, so don't leave
                // separator at the menu start or end nor 2 consecutive
                // separators
                wxMenuItemList::compatibility_iterator nextNode = node->GetNext();
                wxMenuItem *next = nextNode ? nextNode->GetData() : NULL;

                size_t posSeptoHide;
                if ( !previousItem && next && next->IsSeparator() )
                {
                    // next (i.e. second as we must be first) item is
                    // the separator to hide
                    wxASSERT_MSG( pos == 0, _T("should be the menu start") );
                    posSeptoHide = 2;
                }
                else if ( GetMenuItems().GetCount() == pos + 1 &&
                            previousItem != NULL &&
                                previousItem->IsSeparator() )
                {
                    // prev item is a trailing separator we want to hide
                    posSeptoHide = pos;
                }
                else if ( previousItem && previousItem->IsSeparator() &&
                            next && next->IsSeparator() )
                {
                    // two consecutive separators, this is one too many
                    posSeptoHide = pos;
                }
                else // no separators to hide
                {
                    posSeptoHide = 0;
                }

                if ( posSeptoHide )
                {
                    // hide the separator as well
                    ChangeMenuItemAttributes( MAC_WXHMENU( GetHMenu() ),
                                              posSeptoHide,
                                              kMenuItemAttrHidden,
                                              0 );
                }
            }
#endif // TARGET_CARBON
        }

        previousItem = item ;
    }

    if ( isSubMenu )
        ::InsertMenu(MAC_WXHMENU( GetHMenu()), -1);
}

// undo all changes from the MacBeforeDisplay call
void wxMenu::MacAfterDisplay( bool isSubMenu )
{
    if ( isSubMenu )
        ::DeleteMenu(MacGetMenuId());

    wxMenuItemList::compatibility_iterator node;
    wxMenuItem *item;

    for (node = GetMenuItems().GetFirst(); node; node = node->GetNext())
    {
        item = (wxMenuItem *)node->GetData();
        wxMenu* subMenu = item->GetSubMenu() ;
        if (subMenu)
        {
            subMenu->MacAfterDisplay( true ) ;
        }
        else
        {
            // no need to undo hidings
        }
    }
}

// Menu Bar

/*

Mac Implementation note :

The Mac has only one global menubar, so we attempt to install the currently
active menubar from a frame, we currently don't take into account mdi-frames
which would ask for menu-merging

Secondly there is no mac api for changing a menubar that is not the current
menubar, so we have to wait for preparing the actual menubar until the
wxMenubar is to be used

We can in subsequent versions use MacInstallMenuBar to provide some sort of
auto-merge for MDI in case this will be necessary

*/

wxMenuBar* wxMenuBar::s_macInstalledMenuBar = NULL ;
wxMenuBar* wxMenuBar::s_macCommonMenuBar = NULL ;
bool     wxMenuBar::s_macAutoWindowMenu = true ;
WXHMENU  wxMenuBar::s_macWindowMenuHandle = NULL ;

void wxMenuBar::Init()
{
    m_eventHandler = this;
    m_menuBarFrame = NULL;
    m_invokingWindow = (wxWindow*) NULL;
}

wxMenuBar::wxMenuBar()
{
    Init();
}

wxMenuBar::wxMenuBar( long WXUNUSED(style) )
{
    Init();
}

wxMenuBar::wxMenuBar(size_t count, wxMenu *menus[], const wxString titles[], long WXUNUSED(style))
{
    Init();

    m_titles.Alloc(count);

    for ( size_t i = 0; i < count; i++ )
    {
        m_menus.Append(menus[i]);
        m_titles.Add(titles[i]);

        menus[i]->Attach(this);
    }
}

wxMenuBar::~wxMenuBar()
{
    if (s_macCommonMenuBar == this)
        s_macCommonMenuBar = NULL;

    if (s_macInstalledMenuBar == this)
    {
        ::ClearMenuBar();
        s_macInstalledMenuBar = NULL;
    }
}

void wxMenuBar::Refresh(bool WXUNUSED(eraseBackground), const wxRect *WXUNUSED(rect))
{
    wxCHECK_RET( IsAttached(), wxT("can't refresh unatteched menubar") );

    DrawMenuBar();
}

void wxMenuBar::MacInstallMenuBar()
{
    if ( s_macInstalledMenuBar == this )
        return ;

    MenuBarHandle menubar = NULL ;

#if TARGET_API_MAC_OSX
    menubar = NewHandleClear( 6 /* sizeof( MenuBarHeader ) */ ) ;
#else
    menubar = NewHandleClear( 12 ) ;
    (*menubar)[3] = 0x0a ;
#endif

    ::SetMenuBar( menubar ) ;
    DisposeMenuBar( menubar ) ;
    MenuHandle appleMenu = NULL ;

    verify_noerr( CreateNewMenu( kwxMacAppleMenuId , 0 , &appleMenu ) ) ;
    verify_noerr( SetMenuTitleWithCFString( appleMenu , CFSTR( "\x14" ) ) );

    // Add About/Preferences separator only on OS X
    // KH/RN: Separator is always present on 10.3 but not on 10.2
    // However, the change from 10.2 to 10.3 suggests it is preferred
#if TARGET_API_MAC_OSX
    InsertMenuItemTextWithCFString( appleMenu,
                CFSTR(""), 0, kMenuItemAttrSeparator, 0); 
#endif
    InsertMenuItemTextWithCFString( appleMenu,
                CFSTR("About..."), 0, kMenuItemAttrSeparator, 0); 
    MacInsertMenu( appleMenu , 0 ) ;

    // clean-up the help menu before adding new items
    static MenuHandle mh = NULL ;

    if ( mh != NULL )
    {
        MenuItemIndex firstUserHelpMenuItem ;
        if ( UMAGetHelpMenu( &mh , &firstUserHelpMenuItem) == noErr )
        {
            for ( int i = CountMenuItems( mh ) ; i >= firstUserHelpMenuItem ; --i )
                DeleteMenuItem( mh , i ) ;
        }
        else
        {
            mh = NULL ;
        }
    }

#if TARGET_CARBON
    if ( UMAGetSystemVersion() >= 0x1000 && wxApp::s_macPreferencesMenuItemId)
    {
        wxMenuItem *item = FindItem( wxApp::s_macPreferencesMenuItemId , NULL ) ;
        if ( item == NULL || !(item->IsEnabled()) )
            DisableMenuCommand( NULL , kHICommandPreferences ) ;
        else
            EnableMenuCommand( NULL , kHICommandPreferences ) ;
    }

    // Unlike preferences which may or may not exist, the Quit item should be always
    // enabled unless it is added by the application and then disabled, otherwise
    // a program would be required to add an item with wxID_EXIT in order to get the
    // Quit menu item to be enabled, which seems a bit burdensome.
    if ( UMAGetSystemVersion() >= 0x1000 && wxApp::s_macExitMenuItemId)
    {
        wxMenuItem *item = FindItem( wxApp::s_macExitMenuItemId , NULL ) ;
        if ( item != NULL && !(item->IsEnabled()) )
            DisableMenuCommand( NULL , kHICommandQuit ) ;
        else
            EnableMenuCommand( NULL , kHICommandQuit ) ;
    }
#endif

    wxMenuList::compatibility_iterator menuIter = m_menus.GetFirst();
    for (size_t i = 0; i < m_menus.GetCount(); i++, menuIter = menuIter->GetNext())
    {
        wxMenuItemList::compatibility_iterator node;
        wxMenuItem *item;
        wxMenu* menu = menuIter->GetData() , *subMenu = NULL ;

        if ( m_titles[i] == wxT("?") || m_titles[i] == wxT("&?")  || m_titles[i] == wxApp::s_macHelpMenuTitleName )
        {
            for (node = menu->GetMenuItems().GetFirst(); node; node = node->GetNext())
            {
                item = (wxMenuItem *)node->GetData();
                subMenu = item->GetSubMenu() ;
                if (subMenu)
                {
                    // we don't support hierarchical menus in the help menu yet
                }
                else
                {
                    if ( item->GetId() != wxApp::s_macAboutMenuItemId )
                    {
                        if ( mh == NULL )
                        {
                            MenuItemIndex firstUserHelpMenuItem ;
                            if ( UMAGetHelpMenu( &mh , &firstUserHelpMenuItem) != noErr )
                            {
                                mh = NULL ;
                                break ;
                            }
                        }
                    }

                    if ( item->IsSeparator() )
                    {
                        if ( mh )
                            AppendMenuItemTextWithCFString( mh,
                                CFSTR(""), kMenuItemAttrSeparator, 0,NULL); 
                    }
                    else
                    {
                        wxAcceleratorEntry*
                            entry = wxAcceleratorEntry::Create( item->GetText() ) ;

                        if ( item->GetId() == wxApp::s_macAboutMenuItemId )
                        {
                            // this will be taken care of below
                        }
                        else
                        {
                            if ( mh )
                            {
                                UMAAppendMenuItem(mh, wxStripMenuCodes(item->GetText()) , wxFont::GetDefaultEncoding(), entry);
                                SetMenuItemCommandID( mh , CountMenuItems(mh) , wxIdToMacCommand ( item->GetId() ) ) ;
                                SetMenuItemRefCon( mh , CountMenuItems(mh) , (URefCon) item ) ;
                            }
                        }

                        delete entry ;
                    }
                }
            }
        }
        else
        {
            UMASetMenuTitle( MAC_WXHMENU(menu->GetHMenu()) , m_titles[i], m_font.GetEncoding()  ) ;
            menu->MacBeforeDisplay(false) ;
            ::InsertMenu(MAC_WXHMENU(_wxMenuAt(m_menus, i)->GetHMenu()), 0);
        }
    }

    // take care of the about menu item wherever it is
    {
        wxMenu* aboutMenu ;
        wxMenuItem *aboutMenuItem = FindItem(wxApp::s_macAboutMenuItemId , &aboutMenu) ;
        if ( aboutMenuItem )
        {
            wxAcceleratorEntry*
                entry = wxAcceleratorEntry::Create( aboutMenuItem->GetText() ) ;
            UMASetMenuItemText( GetMenuHandle( kwxMacAppleMenuId ) , 1 , wxStripMenuCodes ( aboutMenuItem->GetText() ) , wxFont::GetDefaultEncoding() );
            UMAEnableMenuItem( GetMenuHandle( kwxMacAppleMenuId ) , 1 , true );
            SetMenuItemCommandID( GetMenuHandle( kwxMacAppleMenuId ) , 1 , kHICommandAbout ) ;
            SetMenuItemRefCon(GetMenuHandle( kwxMacAppleMenuId ) , 1 , (URefCon)aboutMenuItem ) ;
            UMASetMenuItemShortcut( GetMenuHandle( kwxMacAppleMenuId ) , 1 , entry ) ;
        }
    }

    if ( GetAutoWindowMenu() )
    {
        if ( MacGetWindowMenuHMenu() == NULL )
            CreateStandardWindowMenu( 0 , (MenuHandle*) &s_macWindowMenuHandle ) ;

        InsertMenu( (MenuHandle) MacGetWindowMenuHMenu() , 0 ) ;
    }

    ::DrawMenuBar() ;
    s_macInstalledMenuBar = this;
}

void wxMenuBar::EnableTop(size_t pos, bool enable)
{
    wxCHECK_RET( IsAttached(), wxT("doesn't work with unattached menubars") );

    _wxMenuAt(m_menus, pos)->MacEnableMenu( enable ) ;
    Refresh();
}

bool wxMenuBar::Enable(bool enable)
{
    wxCHECK_MSG( IsAttached(), false, wxT("doesn't work with unattached menubars") );

    size_t i;
    for (i = 0; i < GetMenuCount(); i++)
        EnableTop(i, enable);

    return true;
}

void wxMenuBar::SetLabelTop(size_t pos, const wxString& label)
{
    wxCHECK_RET( pos < GetMenuCount(), wxT("invalid menu index") );

    m_titles[pos] = label;

    if ( !IsAttached() )
        return;

    _wxMenuAt(m_menus, pos)->SetTitle( label ) ;

    if (wxMenuBar::s_macInstalledMenuBar == this) // are we currently installed ?
    {
        ::SetMenuBar( GetMenuBar() ) ;
        ::InvalMenuBar() ;
    }
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
        return NULL;

    m_titles[pos] = title;

    if ( IsAttached() )
    {
        if (s_macInstalledMenuBar == this)
        {
            menuOld->MacAfterDisplay( false ) ;
            ::DeleteMenu( menuOld->MacGetMenuId() /* m_menus[pos]->MacGetMenuId() */ ) ;

            menu->MacBeforeDisplay( false ) ;
            UMASetMenuTitle( MAC_WXHMENU(menu->GetHMenu()) , title , m_font.GetEncoding() ) ;
            if ( pos == m_menus.GetCount() - 1)
                ::InsertMenu( MAC_WXHMENU(menu->GetHMenu()) , 0 ) ;
            else
                ::InsertMenu( MAC_WXHMENU(menu->GetHMenu()) , _wxMenuAt(m_menus, pos + 1)->MacGetMenuId() ) ;
        }

        Refresh();
    }

    if (m_invokingWindow)
        wxMenubarSetInvokingWindow( menu, m_invokingWindow );

    return menuOld;
}

bool wxMenuBar::Insert(size_t pos, wxMenu *menu, const wxString& title)
{
    if ( !wxMenuBarBase::Insert(pos, menu, title) )
        return false;

    m_titles.Insert(title, pos);

    UMASetMenuTitle( MAC_WXHMENU(menu->GetHMenu()) , title , m_font.GetEncoding() ) ;

    if ( IsAttached() && s_macInstalledMenuBar == this )
    {
        if (s_macInstalledMenuBar == this)
        {
            menu->MacBeforeDisplay( false ) ;

            if ( pos == (size_t) -1  || pos + 1 == m_menus.GetCount() )
                ::InsertMenu( MAC_WXHMENU(menu->GetHMenu()) , 0 ) ;
            else
                ::InsertMenu( MAC_WXHMENU(menu->GetHMenu()) , _wxMenuAt(m_menus, pos+1)->MacGetMenuId() ) ;
        }

        Refresh();
    }

    if (m_invokingWindow)
        wxMenubarSetInvokingWindow( menu, m_invokingWindow );

    return true;
}

wxMenu *wxMenuBar::Remove(size_t pos)
{
    wxMenu *menu = wxMenuBarBase::Remove(pos);
    if ( !menu )
        return NULL;

    if ( IsAttached() )
    {
        if (s_macInstalledMenuBar == this)
            ::DeleteMenu( menu->MacGetMenuId() /* m_menus[pos]->MacGetMenuId() */ ) ;

        Refresh();
    }

    m_titles.RemoveAt(pos);

    return menu;
}

bool wxMenuBar::Append(wxMenu *menu, const wxString& title)
{
    WXHMENU submenu = menu ? menu->GetHMenu() : 0;
        wxCHECK_MSG( submenu, false, wxT("can't append invalid menu to menubar") );

    if ( !wxMenuBarBase::Append(menu, title) )
        return false;

    m_titles.Add(title);

    UMASetMenuTitle( MAC_WXHMENU(menu->GetHMenu()) , title , m_font.GetEncoding() ) ;

    if ( IsAttached() )
    {
        if (s_macInstalledMenuBar == this)
        {
            menu->MacBeforeDisplay( false ) ;
            ::InsertMenu( MAC_WXHMENU(menu->GetHMenu()) , 0 ) ;
        }

        Refresh();
    }

    // m_invokingWindow is set after wxFrame::SetMenuBar(). This call enables
    // adding menu later on.
    if (m_invokingWindow)
        wxMenubarSetInvokingWindow( menu, m_invokingWindow );

    return true;
}

static void wxMenubarUnsetInvokingWindow( wxMenu *menu )
{
    menu->SetInvokingWindow( (wxWindow*) NULL );
    wxMenuItemList::compatibility_iterator node = menu->GetMenuItems().GetFirst();

    while (node)
    {
        wxMenuItem *menuitem = node->GetData();
        if (menuitem->IsSubMenu())
            wxMenubarUnsetInvokingWindow( menuitem->GetSubMenu() );

        node = node->GetNext();
    }
}

static void wxMenubarSetInvokingWindow( wxMenu *menu, wxWindow *win )
{
    menu->SetInvokingWindow( win );
    wxMenuItem *menuitem;
    wxMenuItemList::compatibility_iterator node = menu->GetMenuItems().GetFirst();

    while (node)
    {
        menuitem = node->GetData();
        if (menuitem->IsSubMenu())
            wxMenubarSetInvokingWindow( menuitem->GetSubMenu() , win );

        node = node->GetNext();
    }
}

void wxMenuBar::UnsetInvokingWindow()
{
    m_invokingWindow = (wxWindow*) NULL;
    wxMenu *menu;
    wxMenuList::compatibility_iterator node = m_menus.GetFirst();

    while (node)
    {
        menu = node->GetData();
        wxMenubarUnsetInvokingWindow( menu );

        node = node->GetNext();
    }
}

void wxMenuBar::SetInvokingWindow(wxFrame *frame)
{
    m_invokingWindow = frame;
    wxMenu *menu;
    wxMenuList::compatibility_iterator node = m_menus.GetFirst();

    while (node)
    {
        menu = node->GetData();
        wxMenubarSetInvokingWindow( menu, frame );

        node = node->GetNext();
    }
}

void wxMenuBar::Detach()
{
    wxMenuBarBase::Detach() ;
}

void wxMenuBar::Attach(wxFrame *frame)
{
    wxMenuBarBase::Attach( frame ) ;
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
        if ( menuLabel == title )
            return _wxMenuAt(m_menus, i)->FindItem(itemString);
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
        item = _wxMenuAt(m_menus, i)->FindItem(id, itemMenu);

    return item;
}
