/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/menu_osx.cpp
// Purpose:     wxMenu, wxMenuBar, wxMenuItem
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// headers & declarations
// ============================================================================

// wxWidgets headers
// -----------------

#include "wx/wxprec.h"

#if wxUSE_MENUS

#include "wx/menu.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/app.h"
    #include "wx/utils.h"
    #include "wx/frame.h"
    #include "wx/dialog.h"
    #include "wx/menuitem.h"
#endif

#include "wx/osx/private.h"
#include "wx/scopedptr.h"
#include "wx/private/menuradio.h" // for wxMenuRadioItemsData

// other standard headers
// ----------------------
#include <string.h>

wxIMPLEMENT_ABSTRACT_CLASS(wxMenuImpl, wxObject);

wxMenuImpl::~wxMenuImpl()
{
}

// the (popup) menu title has this special menuid
static const int idMenuTitle = -3;

wxScopedPtr<wxMenu> gs_emptyMenuBar;

// ============================================================================
// implementation
// ============================================================================

// Menus

// Construct a menu with optional title (then use append)

void wxMenu::Init()
{
    m_doBreak = false;
    m_allowRearrange = true;
    m_noEventsMode = false;
    m_radioData = NULL;

    m_peer = wxMenuImpl::Create( this, wxStripMenuCodes(m_title, wxStrip_Menu) );

#if wxOSX_USE_COCOA
    // under macOS there is no built-in title, so if we have a title, insert it in the beginning of the menu
    if ( !m_title.empty() )
    {
        Append(idMenuTitle, m_title) ;
        AppendSeparator() ;
    }
#endif
}

wxMenu::~wxMenu()
{
    delete m_radioData;
    delete m_peer;
}

WXHMENU wxMenu::GetHMenu() const
{
    if ( m_peer )
        return m_peer->GetHMenu();
    return NULL;
}

void wxMenu::SetAllowRearrange( bool allow )
{
    m_allowRearrange = allow;
}

void wxMenu::SetNoEventsMode( bool noEvents )
{
    m_noEventsMode = noEvents;
}

bool wxMenu::OSXGetRadioGroupRange(int pos, int *start, int *end) const
{
    return m_radioData && m_radioData->GetGroupRange(pos, start, end);
}

// function appends a new item or submenu to the menu
// append a new item or submenu to the menu
bool wxMenu::DoInsertOrAppend(wxMenuItem *item, size_t pos)
{
    wxASSERT_MSG( item != NULL, wxT("can't append NULL item to the menu") );
    GetPeer()->InsertOrAppend( item, pos );

    wxMenu *pSubMenu = item->GetSubMenu() ;
    if ( pSubMenu != NULL )
    {
        wxASSERT_MSG( pSubMenu->GetHMenu() != NULL , wxT("invalid submenu added"));
        pSubMenu->m_menuParent = this ;

        pSubMenu->DoRearrange();
    }
    else if ( item->GetId() == idMenuTitle )
    {
        item->GetMenu()->Enable( idMenuTitle, false );
    }

    if ( pos == (size_t)-1 )
    {
        pos = GetMenuItemCount() - 1;
    }

    // Update radio groups if we're inserting a new menu item.
    // Inserting radio and non-radio item has a different impact
    // on radio groups, so we have to handle each case separately.
    // (Inserting a radio item in the middle of existing groups extends this group,
    // but inserting a non-radio item breaks it into two subgroups.)
    bool check = false;
    if ( item->IsRadio() )
    {
        if ( !m_radioData )
            m_radioData = new wxMenuRadioItemsData;

        if ( m_radioData->UpdateOnInsertRadio(pos) )
            check = true; // ensure that we have a checked item in the radio group
    }
    else if ( m_radioData )
    {
        if ( m_radioData->UpdateOnInsertNonRadio(pos) )
        {
            // One of the existing groups has been split into two subgroups.
            wxFAIL_MSG(wxS("Inserting non-radio item inside a radio group?"));
        }
    }

#if wxUSE_MENUBAR
    // if we're already attached to the menubar, we must update it
    if ( IsAttached() && GetMenuBar()->IsAttached() )
    {
        if ( item->IsSubMenu() )
        {
            item->GetSubMenu()->SetupBitmaps();
        }
        if ( !item->IsSeparator() )
        {
            item->UpdateItemBitmap();
        }
        GetMenuBar()->Refresh();
    }
#endif // wxUSE_MENUBAR

    if ( check )
        item->Check(true);

    return true ;
}

wxMenuItem* wxMenu::DoAppend(wxMenuItem *item)
{
    if (wxMenuBase::DoAppend(item) && DoInsertOrAppend(item) )
        return item;

    return NULL;
}

wxMenuItem* wxMenu::DoInsert(size_t pos, wxMenuItem *item)
{
    if (wxMenuBase::DoInsert(pos, item) && DoInsertOrAppend(item, pos))
        return item;

    return NULL;
}

wxMenuItem *wxMenu::DoRemove(wxMenuItem *item)
{
    // Update indices of radio groups.
    if ( m_radioData )
    {
        int pos = GetMenuItems().IndexOf(item);
        if ( m_radioData->UpdateOnRemoveItem(pos) )
        {
            wxASSERT_MSG( item->IsRadio(), wxS("Removing non radio button from radio group?") );
        }
        //else: item being removed is not in a radio group
    }

/*
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

    wxOSXMenuRemoveItem(m_hMenu , pos );
    */
#if wxUSE_ACCEL
    // we need to remove all hidden menu items related to this one
    item->RemoveHiddenItems();
#endif
    GetPeer()->Remove( item );
    // and from internal data structures
    return wxMenuBase::DoRemove(item);
}

void wxMenu::SetTitle(const wxString& label)
{
    m_title = label ;
    GetPeer()->SetTitle( wxStripMenuCodes( label, wxStrip_Menu ) );
}

bool wxMenu::ProcessCommand(wxCommandEvent & event)
{
    bool processed = false;

    // Try the menu's event handler
    if ( /* !processed && */ GetEventHandler())
        processed = GetEventHandler()->SafelyProcessEvent(event);

    // Try the window the menu was popped up from
    // (and up through the hierarchy)
    wxWindow *win = GetWindow();
    if ( !processed && win )
        processed = win->HandleWindowEvent(event);

    return processed;
}

// ---------------------------------------------------------------------------
// other
// ---------------------------------------------------------------------------

// MacOS needs to know about submenus somewhere within this menu
// before it can be displayed, also hide special menu items
// like preferences that are handled by the OS
void wxMenu::DoRearrange()
{
    if ( !AllowRearrange() )
        return;

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
            // already done
        }
        else // normal item
        {
            // what we do here is to hide the special items which are
            // shown in the application menu anyhow -- it doesn't make
            // sense to show them in their normal place as well
            if ( item->GetId() == wxApp::s_macAboutMenuItemId ||
                    item->GetId() == wxApp::s_macPreferencesMenuItemId ||
                    item->GetId() == wxApp::s_macExitMenuItemId )

            {
                item->GetPeer()->Hide( true );

                // also check for a separator which was used just to
                // separate this item from the others, so don't leave
                // separator at the menu start or end nor 2 consecutive
                // separators
                wxMenuItemList::compatibility_iterator nextNode = node->GetNext();
                wxMenuItem *next = nextNode ? nextNode->GetData() : NULL;

                wxMenuItem *sepToHide = 0;
                if ( !previousItem && next && next->IsSeparator() )
                {
                    // next (i.e. second as we must be first) item is
                    // the separator to hide
                    wxASSERT_MSG( pos == 0, wxT("should be the menu start") );
                    sepToHide = next;
                }
                else if ( GetMenuItems().GetCount() == pos + 1 &&
                            previousItem != NULL &&
                                previousItem->IsSeparator() )
                {
                    // prev item is a trailing separator we want to hide
                    sepToHide = previousItem;
                }
                else if ( previousItem && previousItem->IsSeparator() &&
                            next && next->IsSeparator() )
                {
                    // two consecutive separators, this is one too many
                    sepToHide = next;
                }

                if ( sepToHide )
                {
                    // hide the separator as well
                    sepToHide->GetPeer()->Hide( true );
                }
            }
        }

        previousItem = item ;
    }
}


bool wxMenu::HandleCommandUpdateStatus( wxMenuItem* item )
{
    int menuid = item ? item->GetId() : 0;
    wxUpdateUIEvent event(menuid);
    event.SetEventObject( this );

    if ( !item || !item->IsCheckable() )
        event.DisallowCheck();

    bool processed = DoProcessEvent(this, event, GetWindow());

    if ( processed )
    {
        // if anything changed, update the changed attribute
        if (event.GetSetText())
            SetLabel(menuid, event.GetText());
        if (event.GetSetChecked())
            Check(menuid, event.GetChecked());
        if (event.GetSetEnabled())
            Enable(menuid, event.GetEnabled());
    }

    return processed;
}

bool wxMenu::HandleCommandProcess( wxMenuItem* item )
{
    wxCHECK_MSG( item, false, "must have a valid item" );

    int menuid = item->GetId();
    bool processed = false;
    if (item->IsCheckable())
        item->Check( !item->IsChecked() ) ;

    // A bit counterintuitively, we call OSXAfterMenuEvent() _before_ calling
    // the user defined handler. This is done to account for the case when this
    // handler deletes the window, as it can possibly do.
    if (wxWindow* const w = GetInvokingWindow())
    {
        // Let the invoking window update itself if necessary.
        w->OSXAfterMenuEvent();
    }

    if ( SendEvent( menuid , item->IsCheckable() ? item->IsChecked() : -1 ) )
        processed = true ;

    if(!processed)
    {
        processed = item->GetPeer()->DoDefault();  
    }

    return processed;
}

void wxMenu::HandleMenuItemHighlighted( wxMenuItem* item )
{
    int menuid = item ? item->GetId() : wxID_NONE;
    wxMenuEvent wxevent(wxEVT_MENU_HIGHLIGHT, menuid, this);
    ProcessMenuEvent(this, wxevent, GetWindow());
}

void wxMenu::DoHandleMenuOpenedOrClosed(wxEventType evtType)
{
    // Popup menu being currently shown or NULL, defined in wincmn.cpp.
    extern wxMenu *wxCurrentPopupMenu;

    // Set the id to allow wxMenuEvent::IsPopup() to work correctly.
    int menuid = this == wxCurrentPopupMenu ? wxID_ANY : 0;
    wxMenuEvent wxevent(evtType, menuid, this);
    ProcessMenuEvent(this, wxevent, GetWindow());
}

void wxMenu::HandleMenuOpened()
{
    DoHandleMenuOpenedOrClosed(wxEVT_MENU_OPEN);
}

void wxMenu::HandleMenuClosed()
{
    DoHandleMenuOpenedOrClosed(wxEVT_MENU_CLOSE);
}

#if wxUSE_MENUBAR
void wxMenu::Attach(wxMenuBarBase *menubar)
{
    wxMenuBase::Attach(menubar);

    if (menubar->IsAttached())
    {
        SetupBitmaps();
    }
}
#endif

void wxMenu::SetInvokingWindow(wxWindow* win)
{
    wxMenuBase::SetInvokingWindow(win);

    if ( win )
        SetupBitmaps();
}

void wxMenu::SetupBitmaps()
{
    for ( wxMenuItemList::compatibility_iterator node = m_items.GetFirst();
          node;
          node = node->GetNext() )
    {
        wxMenuItem *item = node->GetData();
        if ( item->IsSubMenu() )
        {
            item->GetSubMenu()->SetupBitmaps();
        }
        if ( !item->IsSeparator() )
        {
            item->UpdateItemBitmap();
        }
    }
}

#if wxUSE_MENUBAR

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


const int firstMenuPos = 1; // to account for the 0th application menu on mac

static wxMenu *CreateAppleMenu()
{
    wxMenu *appleMenu = new wxMenu();
    appleMenu->SetAllowRearrange(false);

    // Create standard items unless the application explicitly disabled this by
    // setting the corresponding ids to wxID_NONE: although this is not
    // recommended, sometimes these items really don't make sense.
    if ( wxApp::s_macAboutMenuItemId != wxID_NONE )
    {
        wxString aboutLabel;
        if ( wxTheApp )
            aboutLabel.Printf(wxGETTEXT_IN_CONTEXT("macOS menu item", "About %s"),
                              wxTheApp->GetAppDisplayName());
        else
            aboutLabel = wxGETTEXT_IN_CONTEXT("macOS menu item", "About...");
        appleMenu->Append( wxApp::s_macAboutMenuItemId, aboutLabel);
        appleMenu->AppendSeparator();
    }

    if ( wxApp::s_macPreferencesMenuItemId != wxID_NONE )
    {
        appleMenu->Append( wxApp::s_macPreferencesMenuItemId,
                           wxGETTEXT_IN_CONTEXT("macOS menu item", "Preferences...")
                           + "\tCtrl+," );
        appleMenu->AppendSeparator();
    }

    appleMenu->Append(wxID_OSX_SERVICES, wxGETTEXT_IN_CONTEXT("macOS menu item", "Services"),
                      new wxMenu());
    appleMenu->AppendSeparator();

    // standard menu items, handled in wxMenu::HandleCommandProcess(), see above:
    wxString hideLabel;
    if ( wxTheApp )
        hideLabel = wxString::Format(wxGETTEXT_IN_CONTEXT("macOS menu item", "Hide %s"),
                                     wxTheApp->GetAppDisplayName());
    else
        hideLabel = wxGETTEXT_IN_CONTEXT("macOS menu item", "Hide Application");
    appleMenu->Append( wxID_OSX_HIDE, hideLabel + "\tCtrl+H" );
    appleMenu->Append( wxID_OSX_HIDEOTHERS,
                       wxGETTEXT_IN_CONTEXT("macOS menu item", "Hide Others")+"\tAlt+Ctrl+H" );
    appleMenu->Append( wxID_OSX_SHOWALL,
                       wxGETTEXT_IN_CONTEXT("macOS menu item", "Show All") );
    appleMenu->AppendSeparator();
    
    // Do always add "Quit" item unconditionally however, it can't be disabled.
    wxString quitLabel;
    if ( wxTheApp )
        quitLabel = wxString::Format(wxGETTEXT_IN_CONTEXT("macOS menu item", "Quit %s"),
                                     wxTheApp->GetAppDisplayName());
    else
        quitLabel = wxGETTEXT_IN_CONTEXT("macOS menu item", "Quit Application");
    appleMenu->Append( wxApp::s_macExitMenuItemId, quitLabel + "\tCtrl+Q" );

    return appleMenu;
}

void wxMenuBar::Init()
{
    if ( !gs_emptyMenuBar )
    {
        gs_emptyMenuBar.reset( new wxMenu() );
        gs_emptyMenuBar->AppendSubMenu(CreateAppleMenu(), "\x14") ;
    }
    
    m_eventHandler = this;
    m_menuBarFrame = NULL;
    m_rootMenu = new wxMenu();
    m_rootMenu->Attach(this);

    m_appleMenu = CreateAppleMenu();
    m_rootMenu->AppendSubMenu(m_appleMenu, "\x14") ;
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

    for ( size_t i = 0; i < count; i++ )
    {
        m_menus.Append(menus[i]);

        menus[i]->Attach(this);
        Append( menus[i], titles[i] );
    }
}

wxMenuBar::~wxMenuBar()
{
    if (s_macCommonMenuBar == this)
        s_macCommonMenuBar = NULL;

    MacUninstallMenuBar();
    wxDELETE( m_rootMenu );
    // apple menu is a submenu, therefore we don't have to delete it
    m_appleMenu = NULL;

    // deleting the root menu also removes all its wxMenu* submenus, therefore
    // we must avoid double deleting them in the superclass destructor
    m_menus.clear();
}

void wxMenuBar::Refresh(bool WXUNUSED(eraseBackground), const wxRect *WXUNUSED(rect))
{
    wxCHECK_RET( IsAttached(), wxT("can't refresh unattached menubar") );
}

void wxMenuBar::MacUninstallMenuBar()
{
  if (s_macInstalledMenuBar == this)
  {
    gs_emptyMenuBar->GetPeer()->MakeRoot();
    s_macInstalledMenuBar = NULL;
  }
}

void wxMenuBar::MacInstallMenuBar()
{
    if ( s_macInstalledMenuBar == this )
        return ;

    m_rootMenu->GetPeer()->MakeRoot();
    
    // hide items in the apple menu that don't exist in the wx menubar
    
    wxMenuItem* appleItem = NULL;
    wxMenuItem* wxItem = NULL;

    int menuid = wxApp::s_macAboutMenuItemId;
    appleItem = m_appleMenu->FindItem(menuid);
    wxItem = FindItem(menuid);
    if ( appleItem != NULL )
    {
        if ( wxItem == NULL )
            appleItem->GetPeer()->Hide();
        else 
            appleItem->SetItemLabel(wxItem->GetItemLabel());
    }
    
    menuid = wxApp::s_macPreferencesMenuItemId;
    appleItem = m_appleMenu->FindItem(menuid);
    wxItem = FindItem(menuid);
    if ( appleItem != NULL )
    {
        if ( wxItem == NULL )
            appleItem->GetPeer()->Hide();
        else 
            appleItem->SetItemLabel(wxItem->GetItemLabel());
    }

    s_macInstalledMenuBar = this;
}

void wxMenuBar::EnableTop(size_t pos, bool enable)
{
    wxCHECK_RET( IsAttached(), wxT("doesn't work with unattached menubars") );

    m_rootMenu->FindItemByPosition(pos+firstMenuPos)->Enable(enable);

    Refresh();
}

bool wxMenuBar::IsEnabledTop(size_t pos) const
{
    wxCHECK_MSG( IsAttached(), true,
                 wxT("doesn't work with unattached menubars") );

    wxMenuItem* const item = m_rootMenu->FindItemByPosition(pos+firstMenuPos);
    wxCHECK_MSG( item, false, wxT("invalid menu index") );

    return item->IsEnabled();
}

bool wxMenuBar::Enable(bool enable)
{
    wxCHECK_MSG( IsAttached(), false, wxT("doesn't work with unattached menubars") );

    size_t i;
    for (i = 0; i < GetMenuCount(); i++)
        EnableTop(i, enable);

    return true;
}

void wxMenuBar::SetMenuLabel(size_t pos, const wxString& label)
{
    wxCHECK_RET( pos < GetMenuCount(), wxT("invalid menu index") );

    GetMenu(pos)->SetTitle( label ) ;
}

wxString wxMenuBar::GetMenuLabel(size_t pos) const
{
    wxCHECK_MSG( pos < GetMenuCount(), wxEmptyString,
                 wxT("invalid menu index in wxMenuBar::GetMenuLabel") );

    return GetMenu(pos)->GetTitle();
}

void wxMenuBar::SetupBitmaps()
{
    for ( wxMenuList::const_iterator it = m_menus.begin(); it != m_menus.end(); ++it )
    {
        (*it)->SetupBitmaps();
    }
}

void wxMenuBar::Attach(wxFrame *frame)
{
    wxMenuBarBase::Attach(frame);

    SetupBitmaps();
}

// ---------------------------------------------------------------------------
// wxMenuBar construction
// ---------------------------------------------------------------------------

wxMenu *wxMenuBar::Replace(size_t pos, wxMenu *menu, const wxString& title)
{
    wxMenu *menuOld = wxMenuBarBase::Replace(pos, menu, title);
    if ( !menuOld )
        return NULL;

    wxMenuItem* item = m_rootMenu->FindItemByPosition(pos+firstMenuPos);
    m_rootMenu->Remove(item);
    m_rootMenu->Insert( pos+firstMenuPos, wxMenuItem::New( m_rootMenu, wxID_ANY, title, "", wxITEM_NORMAL, menu ) );

    return menuOld;
}

bool wxMenuBar::Insert(size_t pos, wxMenu *menu, const wxString& title)
{
    if ( !wxMenuBarBase::Insert(pos, menu, title) )
        return false;

    m_rootMenu->Insert( pos+firstMenuPos, wxMenuItem::New( m_rootMenu, wxID_ANY, title, "", wxITEM_NORMAL, menu ) );
    menu->SetTitle(title);

    return true;
}

wxMenu *wxMenuBar::Remove(size_t pos)
{
    wxMenu *menu = wxMenuBarBase::Remove(pos);
    if ( !menu )
        return NULL;

    wxMenuItem* item = m_rootMenu->FindItemByPosition(pos+firstMenuPos);
    m_rootMenu->Remove(item);

    return menu;
}

bool wxMenuBar::Append(wxMenu *menu, const wxString& title)
{
    WXHMENU submenu = menu ? menu->GetHMenu() : 0;
        wxCHECK_MSG( submenu, false, wxT("can't append invalid menu to menubar") );

    if ( !wxMenuBarBase::Append(menu, title) )
        return false;

    m_rootMenu->AppendSubMenu(menu, title);
    menu->SetTitle(title);

    return true;
}

void wxMenuBar::DoGetPosition(int *x, int *y) const
{
    int _x,_y,_width,_height;
    
    m_rootMenu->GetPeer()->GetMenuBarDimensions(_x, _y, _width, _height);

    if (x)
        *x = _x;
    if (y)
        *y = _y;
}

void wxMenuBar::DoGetSize(int *width, int *height) const
{
    int _x,_y,_width,_height;
    
    m_rootMenu->GetPeer()->GetMenuBarDimensions(_x, _y, _width, _height);

    if (width)
        *width = _width;
    if (height)
        *height = _height;
}

void wxMenuBar::DoGetClientSize(int *width, int *height) const
{
    DoGetSize(width, height);
}

#endif // wxUSE_MENUBAR

#endif // wxUSE_MENUS
