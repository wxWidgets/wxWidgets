/////////////////////////////////////////////////////////////////////////////
// Name:        menu.cpp
// Purpose:     wxMenu, wxMenuBar, wxMenuItem
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


// ============================================================================
// headers & declarations
// ============================================================================

// wxWindows headers
// -----------------

#ifdef __GNUG__
#pragma implementation "menu.h"
#pragma implementation "menuitem.h"
#endif

#include "wx/app.h"
#include "wx/menu.h"
#include "wx/menuitem.h"
#include "wx/window.h"
#include "wx/log.h"
#include "wx/utils.h"
#include "wx/frame.h"

#include "wx/mac/uma.h"

// other standard headers
// ----------------------
#include <string.h>

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxMenu, wxEvtHandler)
IMPLEMENT_DYNAMIC_CLASS(wxMenuBar, wxEvtHandler)
#endif

// the (popup) menu title has this special id
static const int idMenuTitle = -2;
static MenuItemIndex firstUserHelpMenuItem = 0 ;

const short kwxMacMenuBarResource = 1 ;
const short kwxMacAppleMenuId = 1 ;

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

void wxMenu::Init()
{
    m_doBreak = FALSE;

    // create the menu
	Str255 	label;
	wxMenuItem::MacBuildMenuString( label, NULL , NULL , m_title , false );
	m_macMenuId = s_macNextMenuId++;
	m_hMenu = ::NewMenu(m_macMenuId, label);

    if ( !m_hMenu )
    {
        wxLogLastError("CreatePopupMenu");
    }

    // if we have a title, insert it in the beginning of the menu
    if ( !!m_title )
    {
        Append(idMenuTitle, m_title) ;
        AppendSeparator() ;
    }
}

wxMenu::~wxMenu()
{
	if (MAC_WXHMENU(m_hMenu))
		::DisposeMenu(MAC_WXHMENU(m_hMenu));

#if wxUSE_ACCEL
    // delete accels
    WX_CLEAR_ARRAY(m_accels);
#endif // wxUSE_ACCEL
}

void wxMenu::Break()
{
	// not available on the mac platform
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
            m_accels.RemoveAt(n);
    }

    if ( IsAttached() )
    {
        m_menuBar->RebuildAccelTable();
    }
}

#endif // wxUSE_ACCEL

// function appends a new item or submenu to the menu
// append a new item or submenu to the menu
bool wxMenu::DoInsertOrAppend(wxMenuItem *pItem, size_t pos)
{
    wxASSERT_MSG( pItem != NULL, "can't append NULL item to the menu" );
#if wxUSE_ACCEL
    UpdateAccel(pItem);
#endif // wxUSE_ACCEL

	if ( pItem->IsSeparator() )
	{
		if ( pos == (size_t)-1 )
		{
			MacAppendMenu(MAC_WXHMENU(m_hMenu), "\p-");
		}
		else
		{
			MacInsertMenuItem(MAC_WXHMENU(m_hMenu), "\p-" , pos);
		}
	}
	else
	{
		wxMenu *pSubMenu = pItem->GetSubMenu() ;
		if ( pSubMenu != NULL )
		{
 			Str255 label;
   			wxASSERT_MSG( pSubMenu->m_hMenu != NULL , "invalid submenu added");
		    pSubMenu->m_menuParent = this ;
			wxMenuItem::MacBuildMenuString( label , NULL , NULL , pItem->GetText() ,false);

			if (wxMenuBar::MacGetInstalledMenuBar() == m_menuBar)
			{
				::InsertMenu( MAC_WXHMENU( pSubMenu->m_hMenu ) , -1 ) ;
			}

			if ( pos == (size_t)-1 )
			{
				UMAAppendSubMenuItem(MAC_WXHMENU(m_hMenu), label, pSubMenu->m_macMenuId);
			}
			else
			{
				UMAInsertSubMenuItem(MAC_WXHMENU(m_hMenu), label , pos, pSubMenu->m_macMenuId);
			}

            // if adding a submenu to a menu already existing in the menu bar, we
            // must set invoking window to allow processing events from this
            // submenu
            if ( m_invokingWindow )
                wxMenubarSetInvokingWindow(pSubMenu, m_invokingWindow);
		}
		else
		{
			Str255 label ;
			UInt8 modifiers ;
			SInt16 key ;
			wxMenuItem::MacBuildMenuString( label, &key  , &modifiers , pItem->GetText(), pItem->GetId() == wxApp::s_macAboutMenuItemId);
			if ( label[0] == 0 )
			{
				// we cannot add empty menus on mac
				label[0] = 1 ;
				label[1] = ' ' ;
			}
			if ( pos == (size_t)-1 )
			{
				UMAAppendMenuItem(MAC_WXHMENU(m_hMenu), label,key,modifiers);
				SetMenuItemCommandID( MAC_WXHMENU(m_hMenu) , CountMenuItems(MAC_WXHMENU(m_hMenu))  , pItem->GetId() ) ;
				if ( pItem->GetBitmap().Ok() )
				{
					ControlButtonContentInfo info ;
					wxMacCreateBitmapButton( &info , pItem->GetBitmap() , kControlContentCIconHandle ) ;
					if ( info.contentType != kControlNoContent )
					{
						if ( info.contentType == kControlContentCIconHandle )
							SetMenuItemIconHandle( MAC_WXHMENU(m_hMenu) , CountMenuItems(MAC_WXHMENU(m_hMenu) ) ,
								kMenuColorIconType , (Handle) info.u.cIconHandle ) ;
				    }

				}
			}
			else
			{
				UMAInsertMenuItem(MAC_WXHMENU(m_hMenu), label , pos,key,modifiers);
				SetMenuItemCommandID( MAC_WXHMENU(m_hMenu) , pos+1 , pItem->GetId() ) ;
				if ( pItem->GetBitmap().Ok() )
				{
					ControlButtonContentInfo info ;
					wxMacCreateBitmapButton( &info , pItem->GetBitmap() , kControlContentCIconHandle ) ;
					if ( info.contentType != kControlNoContent )
					{
						if ( info.contentType == kControlContentCIconHandle )
							SetMenuItemIconHandle( MAC_WXHMENU(m_hMenu) , pos ,
								kMenuColorIconType , (Handle) info.u.cIconHandle ) ;
				    }

				}
			}
  			if ( pItem->GetId() == idMenuTitle )
  			{
  				if ( pos == (size_t)-1 )
				{
					UMADisableMenuItem(MAC_WXHMENU(m_hMenu) , CountMenuItems(MAC_WXHMENU(m_hMenu) ) ) ;
				}
				else
				{
					UMADisableMenuItem(MAC_WXHMENU(m_hMenu) , pos + 1 ) ;
				}
  			}
		}
	}
    // if we're already attached to the menubar, we must update it
    if ( IsAttached() )
    {
        m_menuBar->Refresh();
    }
	return TRUE ;
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

        m_accels.RemoveAt(n);
    }
    //else: this item doesn't have an accel, nothing to do
#endif // wxUSE_ACCEL

	::DeleteMenuItem(MAC_WXHMENU(m_hMenu) , pos + 1);

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

void wxMenu::SetTitle(const wxString& label)
{
	Str255 title ;
    m_title = label ;
	wxMenuItem::MacBuildMenuString( title, NULL , NULL , label , false );
	UMASetMenuTitle(MAC_WXHMENU(m_hMenu) , title ) ;
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
#endif WXWIN_COMPATIBILITY

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

wxWindow *wxMenu::GetWindow() const
{
    if ( m_invokingWindow != NULL )
        return m_invokingWindow;
    else if ( m_menuBar != NULL)
        return (wxWindow *) m_menuBar->GetFrame();

    return NULL;
}

// helper functions returning the mac menu position for a certain item, note that this is
// mac-wise 1 - based, i.e. the first item has index 1 whereas on MSWin it has pos 0

int wxMenu::MacGetIndexFromId( int id )
{
    size_t pos;
    wxMenuItemList::Node *node = GetMenuItems().GetFirst();
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
    wxMenuItemList::Node *node = GetMenuItems().GetFirst();
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
	if ( bDoEnable )
		UMAEnableMenuItem(MAC_WXHMENU(m_hMenu) , 0 ) ;
	else
		UMADisableMenuItem(MAC_WXHMENU(m_hMenu) , 0 ) ;

	::DrawMenuBar() ;
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

void wxMenuBar::MacInstallMenuBar()
{
    if ( s_macInstalledMenuBar == this )
        return ;

    wxStAppResource resload ;

    Handle menubar = ::GetNewMBar( kwxMacMenuBarResource ) ;
    wxString message ;
    wxCHECK_RET( menubar != NULL, "can't read MBAR resource" );
    ::SetMenuBar( menubar ) ;
#if TARGET_API_MAC_CARBON
    ::DisposeMenuBar( menubar ) ;
#else
    ::DisposeHandle( menubar ) ;
#endif

#if TARGET_API_MAC_OS8
    MenuHandle menu = ::GetMenuHandle( kwxMacAppleMenuId ) ;
    if ( CountMenuItems( menu ) == 2 )
    {
        ::AppendResMenu(menu, 'DRVR');
    }
#endif

    // clean-up the help menu before adding new items
	MenuHandle mh = NULL ;
	if ( UMAGetHelpMenu( &mh , &firstUserHelpMenuItem) == noErr )
	{
	    for ( int i = CountMenuItems( mh ) ; i >= firstUserHelpMenuItem ; --i )
	    {
	        DeleteMenuItem( mh , i ) ;
	    }
	}
	else
	{
	    mh = NULL ;
	}


   	for (size_t i = 0; i < m_menus.GetCount(); i++)
  	{
        Str255 	label;
        wxNode *node;
        wxMenuItem *item;
        int pos ;
    	wxMenu* menu = m_menus[i] , *subMenu = NULL ;

    	if( m_titles[i] == "?" || m_titles[i] == "&?"  || m_titles[i] == wxApp::s_macHelpMenuTitleName )
    	{
			if ( mh == NULL )
			{
			    continue ;
			}

		  	for (pos = 0 , node = menu->GetMenuItems().First(); node; node = node->Next(), pos++)
	  		{
	 			item = (wxMenuItem *)node->Data();
	 			subMenu = item->GetSubMenu() ;
				if (subMenu)
				{
					// we don't support hierarchical menus in the help menu yet
				}
				else
				{
					if ( item->IsSeparator() )
					{
						if ( mh )
							MacAppendMenu(mh, "\p-" );
					}
					else
					{
						Str255 label ;
						UInt8 modifiers ;
						SInt16 key ;
						wxMenuItem::MacBuildMenuString( label, &key , &modifiers  , item->GetText(), item->GetId() != wxApp::s_macAboutMenuItemId); // no shortcut in about menu
						if ( label[0] == 0 )
						{
							// we cannot add empty menus on mac
							label[0] = 1 ;
							label[1] = ' ' ;
						}
						if ( item->GetId() == wxApp::s_macAboutMenuItemId )
						{
								::SetMenuItemText( GetMenuHandle( kwxMacAppleMenuId ) , 1 , label );
								UMAEnableMenuItem( GetMenuHandle( kwxMacAppleMenuId ) , 1 );
								SetMenuItemCommandID( GetMenuHandle( kwxMacAppleMenuId ) , 1 , item->GetId() ) ;
 						}
						else
						{
							if ( mh )
							{
								UMAAppendMenuItem(mh, label , key , modifiers );
								SetMenuItemCommandID( mh , CountMenuItems(mh) , item->GetId() ) ;
							}
						}
					}
				}
			}
		}
		else
		{
			wxMenuItem::MacBuildMenuString( label, NULL , NULL , m_titles[i] , false );
			UMASetMenuTitle( MAC_WXHMENU(menu->GetHMenu()) , label ) ;
				wxArrayPtrVoid submenus ;

	  		for (pos = 0, node = menu->GetMenuItems().First(); node; node = node->Next(), pos++)
  			{
	 			item = (wxMenuItem *)node->Data();
	 			subMenu = item->GetSubMenu() ;
				if (subMenu)
				{
				  submenus.Add(subMenu) ;
				}
			}
			::InsertMenu(MAC_WXHMENU(m_menus[i]->GetHMenu()), 0);
			for ( size_t i = 0 ; i < submenus.GetCount() ; ++i )
			{
			    wxMenu* submenu = (wxMenu*) submenus[i] ;
    	        wxNode *subnode;
    	        wxMenuItem *subitem;
    	        int subpos ;
                for ( subpos = 0 , subnode = submenu->GetMenuItems().First(); subnode; subnode = subnode->Next(), subpos++)
		  		{
  		 			subitem = (wxMenuItem *)subnode->Data();
  		 			wxMenu* itsSubMenu = subitem->GetSubMenu() ;
  					if (itsSubMenu)
  					{
  						submenus.Add(itsSubMenu) ;
  					}
  				}
				::InsertMenu( MAC_WXHMENU(submenu->GetHMenu()) , -1 ) ;
		    }
		}
	}
	::DrawMenuBar() ;
	s_macInstalledMenuBar = this;
}

void wxMenuBar::EnableTop(size_t pos, bool enable)
{
    wxCHECK_RET( IsAttached(), wxT("doesn't work with unattached menubars") );
	m_menus[pos]->MacEnableMenu( enable ) ;
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

    m_menus[pos]->SetTitle( label ) ;
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

// ---------------------------------------------------------------------------
// wxMenuBar construction
// ---------------------------------------------------------------------------

// This allows Replace to work with menus that have submenus.  Is it needed
// anywhere else?  (Like in Insert and Append?)
static void wxMenuBarInsertSubmenus(wxMenu* menu)
{
    int pos ;
    wxMenuItemList::Node *node;
    wxMenuItem *item;
    for (pos = 0, node = menu->GetMenuItems().GetFirst(); node; node = node->GetNext(), pos++)
    {
        item = (wxMenuItem *)node->GetData();
        wxMenu* subMenu = item->GetSubMenu() ;
        if (subMenu)
        {
            ::InsertMenu(MAC_WXHMENU( subMenu->GetHMenu()), -1);
            wxMenuBarInsertSubmenus(subMenu);
        }
    }
}

wxMenu *wxMenuBar::Replace(size_t pos, wxMenu *menu, const wxString& title)
{
    wxMenu *menuOld = wxMenuBarBase::Replace(pos, menu, title);
    if ( !menuOld )
        return FALSE;
    m_titles[pos] = title;

    if ( IsAttached() )
    {
		if (s_macInstalledMenuBar == this)
		{
			::DeleteMenu( menuOld->MacGetMenuId() /* m_menus[pos]->MacGetMenuId() */ ) ;
			{
				Str255 	label;
				wxMenuItem::MacBuildMenuString( label, NULL , NULL , title , false );
				UMASetMenuTitle( MAC_WXHMENU(menu->GetHMenu()) , label ) ;
				if ( pos == m_menus.GetCount() - 1)
				{
					::InsertMenu( MAC_WXHMENU(menu->GetHMenu()) , 0 ) ;
				}
				else
				{
					::InsertMenu( MAC_WXHMENU(menu->GetHMenu()) , m_menus[pos+1]->MacGetMenuId() ) ;
				}
                                wxMenuBarInsertSubmenus(menu);
			}
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

    return menuOld;
}

bool wxMenuBar::Insert(size_t pos, wxMenu *menu, const wxString& title)
{
    if ( !wxMenuBarBase::Insert(pos, menu, title) )
        return FALSE;

    m_titles.Insert(title, pos);

    Str255 label ;
		wxMenuItem::MacBuildMenuString( label, NULL , NULL , title , false );
		UMASetMenuTitle( MAC_WXHMENU(menu->GetHMenu()) , label ) ;

    if ( IsAttached() )
    {
    	if ( pos == (size_t) -1  || pos + 1 == m_menus.GetCount() )
		{
			::InsertMenu( MAC_WXHMENU(menu->GetHMenu()) , 0 ) ;
		}
		else
		{
			::InsertMenu( MAC_WXHMENU(menu->GetHMenu()) , m_menus[pos+1]->MacGetMenuId() ) ;
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

    return TRUE;
}

wxMenu *wxMenuBar::Remove(size_t pos)
{
    wxMenu *menu = wxMenuBarBase::Remove(pos);
    if ( !menu )
        return NULL;

    if ( IsAttached() )
    {
		if (s_macInstalledMenuBar == this)
		{
			::DeleteMenu( menu->MacGetMenuId() /* m_menus[pos]->MacGetMenuId() */ ) ;
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

    return menu;
}

bool wxMenuBar::Append(wxMenu *menu, const wxString& title)
{
    WXHMENU submenu = menu ? menu->GetHMenu() : 0;
    wxCHECK_MSG( submenu, FALSE, wxT("can't append invalid menu to menubar") );

    if ( !wxMenuBarBase::Append(menu, title) )
        return FALSE;

    m_titles.Add(title);

    Str255 label ;
		wxMenuItem::MacBuildMenuString( label, NULL , NULL , title , false );
		UMASetMenuTitle( MAC_WXHMENU(menu->GetHMenu()) , label ) ;

    if ( IsAttached() )
    {
    		if (s_macInstalledMenuBar == this)
    		{
    			::InsertMenu( MAC_WXHMENU(menu->GetHMenu()) , 0 ) ;
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

   // m_invokingWindow is set after wxFrame::SetMenuBar(). This call enables
    // adding menu later on.
    if (m_invokingWindow)
        wxMenubarSetInvokingWindow( menu, m_invokingWindow );

    return TRUE;
}

static void wxMenubarUnsetInvokingWindow( wxMenu *menu )
{
    menu->SetInvokingWindow( (wxWindow*) NULL );

    wxMenuItemList::Node *node = menu->GetMenuItems().GetFirst();
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

    wxMenuItemList::Node *node = menu->GetMenuItems().GetFirst();
    while (node)
    {
        wxMenuItem *menuitem = node->GetData();
        if (menuitem->IsSubMenu())
            wxMenubarSetInvokingWindow( menuitem->GetSubMenu() , win );
        node = node->GetNext();
    }
}

void wxMenuBar::UnsetInvokingWindow()
{
    m_invokingWindow = (wxWindow*) NULL;
    wxMenuList::Node *node = m_menus.GetFirst();
    while (node)
    {
        wxMenu *menu = node->GetData();
        wxMenubarUnsetInvokingWindow( menu );
        node = node->GetNext();
    }
}

void wxMenuBar::SetInvokingWindow(wxFrame *frame)
{
    m_invokingWindow = frame;
    wxMenuList::Node *node = m_menus.GetFirst();
    while (node)
    {
        wxMenu *menu = node->GetData();
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
#if wxUSE_ACCEL
    RebuildAccelTable();
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


