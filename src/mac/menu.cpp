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

#include "wx/menu.h"
#include "wx/menuitem.h"
#include "wx/log.h"
#include "wx/utils.h"

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
static int formerHelpMenuItems = 0 ;

const short kwxMacMenuBarResource = 1 ;
const short kwxMacAppleMenuId = 1 ;

// ============================================================================
// implementation
// ============================================================================

//
// Helper Functions to get Mac Menus the way they should be ;-)
//

void wxMacCtoPString(const char* theCString, Str255 thePString);

// remove inappropriate characters, if useShortcuts is false, the ampersand will not auto-generate a mac menu-shortcut

static void wxMacBuildMenuString(StringPtr outMacItemText, char *outMacShortcutChar , short *outMacModifiers , const char *inItemName , bool useShortcuts )
{
	char *p = (char *) &outMacItemText[1] ;
	short macModifiers = 0 ;
	char macShortCut = 0 ;
	
	if ( useShortcuts && !wxApp::s_macSupportPCMenuShortcuts )
		useShortcuts = false ;
	
	// we have problems with a leading hypen - it will be taken as a separator
	
	while ( *inItemName == '-' )
		inItemName++ ;
		
	while( *inItemName )
	{
		switch ( *inItemName )
		{
			// special characters for macintosh menus -> use some replacement
			case ';' :
				*p++ = ',' ;
				break ;
			case '^' :
				*p++ = ' ' ;
				break ;
			case '!' :
				*p++ = ' ' ;
				break ;
			case '<' :
				*p++ = ' ' ;
				break ;
			case '/' :
				*p++ = '|' ;
				break ;
			case '(' :
				*p++ = '[' ;
				break ;
			case ')' :	
				*p++ = ']' ;
				break ;
			// shortcuts
			case '&' :
				{
					++inItemName ;
					if ( *inItemName )
					{
						*p++ = *inItemName ;
						if ( useShortcuts )
							macShortCut = *inItemName ;
					}
					else
						--inItemName ;
				}
				break ;
			// win-like accelerators
			case '\t' :
				{
					++inItemName ;
					while( *inItemName )
					{
						if (strncmp("Ctrl+", inItemName, 5) == 0) 
						{
							inItemName = inItemName + 5;
							macShortCut = *inItemName;
						}
						else if (strncmp("Alt+", inItemName, 4) == 0) 
						{
							inItemName = inItemName + 4;
							macModifiers |= kMenuOptionModifier ;
							macShortCut = *inItemName ;
						}
						else if (strncmp("Shift+", inItemName, 6) == 0) 
						{
							inItemName = inItemName + 6;
							macModifiers |= kMenuShiftModifier ;
							macShortCut = *inItemName ;
						}
						else if (strncmp("F", inItemName, 1) == 0) 
						{
							inItemName += strlen( inItemName ) ;
							// no function keys at the moment
							// macModifiers |= kMenuShiftModifier ;
							// macShortCut = *inItemName ;
						}
						else
						{
							break ;
						}
					}

					if ( *inItemName == 0 )
						--inItemName ;
						
				}
				break ;
			default :
				*p++ = *inItemName ;
		}
		++inItemName ;
	}

	outMacItemText[0] = (p - (char *)outMacItemText) - 1;
	if ( outMacShortcutChar )
		*outMacShortcutChar = macShortCut ;
	if ( outMacModifiers )
		*outMacModifiers = macModifiers ;
	if ( macShortCut )
	{
			int pos = outMacItemText[0] ;
			outMacItemText[++pos] = '/';
			outMacItemText[++pos] = toupper( macShortCut );
			outMacItemText[0] = pos ;
	}
}

// Menus

// Construct a menu with optional title (then use append)

short wxMenu::s_macNextMenuId = 2 ;

void wxMenu::Init()
{
    m_doBreak = FALSE;

    // create the menu
	Str255 	label;
	wxMacBuildMenuString( label, NULL , NULL , m_title , false );
	m_macMenuId = s_macNextMenuId++; 
    wxCHECK_RET( s_macNextMenuId < 236 , "menu ids > 235 cannot be used for submenus on mac" );
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
	if (m_hMenu)
		::DisposeMenu(m_hMenu);

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
            m_accels.Remove(n);
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
			MacAppendMenu(m_hMenu, "\p-");
		}
		else
		{
			MacInsertMenuItem(m_hMenu, "\p-" , pos);
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
			wxMacBuildMenuString( label , NULL , NULL , pItem->GetText() ,false);
		
			// hardcoded adding of the submenu combination for mac
		
			int theEnd = label[0] + 1; 
			if (theEnd > 251) 
				theEnd = 251; // mac allows only 255 characters
			label[theEnd++] = '/';
			label[theEnd++] = hMenuCmd; 
			label[theEnd++] = '!';
			label[theEnd++] = pSubMenu->m_macMenuId; 
			label[theEnd] = 0x00;
			label[0] = theEnd;

			if (wxMenuBar::MacGetInstalledMenuBar() == m_menuBar) 
			{
				::InsertMenu( pSubMenu->m_hMenu , -1 ) ;
			}
			
			if ( pos == (size_t)-1 )
			{
				MacAppendMenu(m_hMenu, label);
			}
			else
			{
				MacInsertMenuItem(m_hMenu, label , pos);
			}
		}
		else
		{
			Str255 label ;
			wxMacBuildMenuString( label , NULL , NULL , pItem->GetText(), pItem->GetId() == wxApp::s_macAboutMenuItemId);
			if ( label[0] == 0 )
			{
				// we cannot add empty menus on mac
				label[0] = 1 ;
				label[1] = ' ' ;
			}
			if ( pos == (size_t)-1 )
			{
				MacAppendMenu(m_hMenu, label);
			}
			else
			{
				MacInsertMenuItem(m_hMenu, label , pos);
			}
  			if ( pItem->GetId() == idMenuTitle ) 
  			{
  				if ( pos == (size_t)-1 )
				{
					UMADisableMenuItem( m_hMenu , CountMItems( m_hMenu ) ) ;
				}
				else
				{
					UMADisableMenuItem( m_hMenu , pos + 1 ) ;
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

        m_accels.Remove(n);
    }
    //else: this item doesn't have an accel, nothing to do
#endif // wxUSE_ACCEL

	::DeleteMenuItem( m_hMenu , pos + 1);

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
	wxMacBuildMenuString( title, NULL , NULL , label , false );
	UMASetMenuTitle( m_hMenu , title ) ;
}

/*

void wxMenu::SetLabel(int id, const wxString& label)
{
    Str255 maclabel ;
   int index ;
    wxMenuItem *item = FindItemForId(id) ;
    if (item==NULL)
        return;

    index = MacGetIndexFromItem( item ) ;
    if (index < 1)
		return;

    if (item->GetSubMenu()==NULL)
    {
		wxMacBuildMenuString( maclabel , NULL , NULL , label , false );
		::SetMenuItemText( m_hMenu , index , maclabel ) ;
    }
    else
    {
		wxMacBuildMenuString( maclabel , NULL , NULL , label , false );
		::SetMenuItemText( m_hMenu , index , maclabel ) ;
    }
    item->SetName(label);
}

wxString wxMenu::GetLabel(int Id) const
{
    wxMenuItem *pItem = FindItemForId(Id) ;
    return pItem->GetName() ;
}

// Finds the item id matching the given string, -1 if not found.
int wxMenu::FindItem (const wxString& itemString) const
{
    char buf1[200];
    char buf2[200];
    wxStripMenuCodes ((char *)(const char *)itemString, buf1);

    for (wxNode * node = m_menuItems.First (); node; node = node->Next ())
    {
      wxMenuItem *item = (wxMenuItem *) node->Data ();
      if (item->GetSubMenu())
      {
        int ans = item->GetSubMenu()->FindItem(itemString);
        if (ans > -1)
          return ans;
      }
      if ( !item->IsSeparator() )
      {
        wxStripMenuCodes((char *)item->GetName().c_str(), buf2);
        if (strcmp(buf1, buf2) == 0)
          return item->GetId();
      }
    }

    return -1;
}

wxMenuItem *wxMenu::FindItemForId(int itemId, wxMenu ** itemMenu) const
{
    if (itemMenu)
        *itemMenu = NULL;
    for (wxNode * node = m_menuItems.First (); node; node = node->Next ())
    {
        wxMenuItem *item = (wxMenuItem *) node->Data ();

        if (item->GetId() == itemId)
        {
            if (itemMenu)
                *itemMenu = (wxMenu *) this;
            return item;
        }

        if (item->GetSubMenu())
        {
            wxMenuItem *ans = item->GetSubMenu()->FindItemForId (itemId, itemMenu);
            if (ans)
                return ans;
        }
    }

    if (itemMenu)
        *itemMenu = NULL;
    return NULL;
}

void wxMenu::SetHelpString(int itemId, const wxString& helpString)
{
    wxMenuItem *item = FindItemForId (itemId);
    if (item)
        item->SetHelp(helpString);
}

wxString wxMenu::GetHelpString (int itemId) const
{
    wxMenuItem *item = FindItemForId (itemId);
    wxString str("");
    return (item == NULL) ? str : item->GetHelp();
}
*/

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
/*
bool wxWindow::PopupMenu(wxMenu *menu, int x, int y)
{
	menu->SetInvokingWindow(this);
	ClientToScreen( &x , &y ) ;

	::InsertMenu( menu->m_hMenu , -1 ) ;
  long menuResult = ::PopUpMenuSelect(menu->m_hMenu ,y,x, 0) ;
  menu->MacMenuSelect( this , TickCount() , HiWord(menuResult) , LoWord(menuResult) ) ;
	::DeleteMenu( menu->m_macMenuId ) ;
  menu->SetInvokingWindow(NULL);

  return TRUE;
}
*/
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
		UMAEnableMenuItem( m_hMenu , 0 ) ;
	else
		UMADisableMenuItem( m_hMenu , 0 ) ;
		
	::DrawMenuBar() ;
}

bool wxMenu::MacMenuSelect( wxEvtHandler* handler, long when , int macMenuId, int macMenuItemNum )
{
  int pos;
  wxNode *node;

	if ( m_macMenuId == macMenuId )
	{
		node = GetMenuItems().Nth(macMenuItemNum-1);
		if (node) 
		{
			wxMenuItem *pItem = (wxMenuItem*)node->Data();
	
			wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, pItem->GetId());
			event.m_timeStamp = when;
			event.SetEventObject(handler);
     		event.SetInt( pItem->GetId() );
     		ProcessCommand( event ) ;
			return true ;
		}
	}
	else if ( macMenuId == kHMHelpMenuID )
	{
		int menuItem = formerHelpMenuItems ;
	  for (pos = 0, node = GetMenuItems().First(); node; node = node->Next(), pos++) 
	  {	
	  	wxMenuItem * pItem = (wxMenuItem *)  node->Data() ;
	  	
	  	wxMenu *pSubMenu = pItem->GetSubMenu() ;
			if ( pSubMenu != NULL )
			{
			}
			else
			{
				if ( pItem->GetId() != wxApp::s_macAboutMenuItemId )
					++menuItem ;
					
				if ( menuItem == macMenuItemNum )
				{
					wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, pItem->GetId());
					event.m_timeStamp = when;
					event.SetEventObject(handler);
       				event.SetInt( pItem->GetId() );
    				ProcessCommand( event ) ;
					return true ;
				}
			}
	  }
	}

  for (pos = 0, node = GetMenuItems().First(); node; node = node->Next(), pos++) 
  {	
  		wxMenuItem * pItem = (wxMenuItem *)  node->Data() ;
  	
  		wxMenu *pSubMenu = pItem->GetSubMenu() ;
		if ( pSubMenu != NULL )
		{
			if ( pSubMenu->MacMenuSelect( handler , when , macMenuId , macMenuItemNum ) )
				return true ;
		}
  }

	return false ;
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

void wxMenuBar::Refresh()
{
    wxCHECK_RET( IsAttached(), wxT("can't refresh unatteched menubar") );

    DrawMenuBar();
}

void wxMenuBar::MacInstallMenuBar() 
{
  	Handle menubar = ::GetNewMBar( kwxMacMenuBarResource ) ;
  	wxString message ;
  	wxCHECK_RET( menubar != NULL, "can't read MBAR resource" );
  	::SetMenuBar( menubar ) ;
  	::DisposeHandle( menubar ) ;

		MenuHandle menu = ::GetMenuHandle( kwxMacAppleMenuId ) ;
		::AppendResMenu(menu, 'DRVR');
 
   	for (int i = 0; i < m_menus.GetCount(); i++)
  	{
			Str255 	label;
    	wxNode *node;
    	wxMenuItem *item;
    	int pos ;
			wxMenu* menu = m_menus[i] , *subMenu = NULL ;
		
			
			if( m_titles[i] == "?" || m_titles[i] == wxApp::s_macHelpMenuTitleName )
			{
				MenuHandle mh = NULL ;
				if ( HMGetHelpMenuHandle( &mh ) != noErr )
				{
					continue ;
				}
				if ( formerHelpMenuItems == 0 )
				{
					if( mh )
						formerHelpMenuItems = CountMenuItems( mh ) ;
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
						Str255 label ;
						wxMacBuildMenuString( label , NULL , NULL , item->GetText(), item->GetId() != wxApp::s_macAboutMenuItemId); // no shortcut in about menu
						if ( label[0] == 0 )
						{
							// we cannot add empty menus on mac
							label[0] = 1 ;
							label[1] = ' ' ;
						}
						if ( item->GetId() == wxApp::s_macAboutMenuItemId )
						{ 
								::SetMenuItemText( GetMenuHandle( kwxMacAppleMenuId ) , 1 , label );
		//					::EnableMenuItem( GetMenuHandle( kwxMacAppleMenuId ) , 1 );
								::EnableItem( GetMenuHandle( kwxMacAppleMenuId ) , 1 );
 						}
						else
						{
							if ( mh )
								::AppendMenu(mh, label );
						}
					}
				}
			}
			else
			{
				wxMacBuildMenuString( label, NULL , NULL , m_titles[i] , false );
				UMASetMenuTitle( menu->GetHMenu() , label ) ;
		  	for (pos = 0, node = menu->GetMenuItems().First(); node; node = node->Next(), pos++) 
	  		{
		 			item = (wxMenuItem *)node->Data();
		 			subMenu = item->GetSubMenu() ;
					if (subMenu)	 		
					{
						::InsertMenu( subMenu->GetHMenu() , -1 ) ;
					}
				}
				::InsertMenu(m_menus[i]->GetHMenu(), 0);
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
				wxMacBuildMenuString( label, NULL , NULL , title , false );
				UMASetMenuTitle( menu->GetHMenu() , label ) ;
				if ( pos == m_menus.GetCount() - 1)
				{
					::InsertMenu( menu->GetHMenu() , 0 ) ;
				}
				else
				{
					::InsertMenu( menu->GetHMenu() , m_menus[pos+1]->MacGetMenuId() ) ;
				}
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

    menu->Attach(this);

    if ( IsAttached() )
    {
    	if ( pos == (size_t) -1 )
		{
			::InsertMenu( menu->GetHMenu() , 0 ) ;
		}
		else
		{
			::InsertMenu( menu->GetHMenu() , m_menus[pos+1]->MacGetMenuId() ) ;
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

void wxMenuBar::MacMenuSelect(wxEvtHandler* handler, long when , int macMenuId, int macMenuItemNum)
{
	// first scan fast for direct commands, i.e. menus which have these commands directly in their own list

	if ( macMenuId == kwxMacAppleMenuId && macMenuItemNum == 1 )
	{
			wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, wxApp::s_macAboutMenuItemId );
			event.m_timeStamp = when;
			event.SetEventObject(handler);
     	event.SetInt( wxApp::s_macAboutMenuItemId );
			handler->ProcessEvent(event);
	}
	else
	{		
	  for (int i = 0; i < m_menus.GetCount() ; i++)
	  {
	  	if ( m_menus[i]->MacGetMenuId() == macMenuId || 
	  		( macMenuId == kHMHelpMenuID && ( m_titles[i] == "?" || m_titles[i] == wxApp::s_macHelpMenuTitleName ) )
	  		)
	  	{
	  		if ( m_menus[i]->MacMenuSelect( handler , when , macMenuId , macMenuItemNum ) )
	  			return ;
	  		else
	  		{
	  			//TODO flag this as an error since it must contain the item
	  			return ;
	  		}
	  	}
		}
	
	  for (int i = 0; i < m_menus.GetCount(); i++)
	  {
	  	if ( m_menus[i]->MacMenuSelect( handler , when , macMenuId , macMenuItemNum ) )
	  	{
	  		break ;
	  	}
		}
	}
}

