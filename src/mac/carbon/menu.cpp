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

wxMenu::wxMenu(const wxString& title, const wxFunction func)
{
    m_title = title;
    m_parent = NULL;
    m_eventHandler = this;
    m_noItems = 0;
    m_menuBar = NULL;
    m_pInvokingWindow = NULL ;
    m_clientData = (void*) NULL;
    if (m_title != "")
    {
        Append(idMenuTitle, m_title) ;
        AppendSeparator() ;
    }

    Callback(func);

	Str255 	label;
	wxMacBuildMenuString( label, NULL , NULL , title , false );
	m_macMenuId = s_macNextMenuId++; 
    wxCHECK_RET( s_macNextMenuId < 236 , "menu ids > 235 cannot be used for submenus on mac" );
	m_macMenuHandle = ::NewMenu(m_macMenuId, label);
	m_macMenuEnabled = true ;
}

// The wxWindow destructor will take care of deleting the submenus.
wxMenu::~wxMenu()
{
    wxNode *node = m_menuItems.First();
    while (node)
    {
        wxMenuItem *item = (wxMenuItem *)node->Data();

        // Delete child menus.
        // Beware: they must not be appended to children list!!!
        // (because order of delete is significant)
        if (item->GetSubMenu())
            item->DeleteSubMenu();

        wxNode *next = node->Next();
        delete item;
        delete node;
        node = next;
    }
	if (m_macMenuHandle)
		::DisposeMenu(m_macMenuHandle);
}

void wxMenu::Break()
{
	// not available on the mac platform
}

// function appends a new item or submenu to the menu
void wxMenu::Append(wxMenuItem *pItem)
{
    wxCHECK_RET( pItem != NULL, "can't append NULL item to the menu" );

    m_menuItems.Append(pItem);

	if ( pItem->IsSeparator() )
	{
		MacAppendMenu(m_macMenuHandle, "\p-");
	}
	else 
	{
		wxMenu *pSubMenu = pItem->GetSubMenu() ;
		if ( pSubMenu != NULL )
		{
 			Str255 label;
   			wxCHECK_RET( pSubMenu->m_macMenuHandle != NULL , "invalid submenu added");
		    pSubMenu->m_parent = this ;
			wxMacBuildMenuString( label , NULL , NULL , pItem->GetName() ,false);
		
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

			if (wxMenuBar::s_macInstalledMenuBar == m_menuBar) 
			{
				::InsertMenu( pSubMenu->m_macMenuHandle , -1 ) ;
			}
			
			::AppendMenu(m_macMenuHandle, label);
		}
		else
		{
			Str255 label ;
			wxMacBuildMenuString( label , NULL , NULL , pItem->GetName(), pItem->GetId() == wxApp::s_macAboutMenuItemId);
			if ( label[0] == 0 )
			{
				// we cannot add empty menus on mac
				label[0] = 1 ;
				label[1] = ' ' ;
			}
			::AppendMenu(m_macMenuHandle, label );
  			if ( pItem->GetId() == idMenuTitle ) 
  			{
					UMADisableMenuItem( m_macMenuHandle , CountMItems( m_macMenuHandle ) ) ;
  			}
		}
	}

    m_noItems++;
}

void wxMenu::AppendSeparator()
{
    Append(new wxMenuItem(this, ID_SEPARATOR));
}

// Pullright item
void wxMenu::Append(int Id, const wxString& label, wxMenu *SubMenu, 
                    const wxString& helpString)
{
    Append(new wxMenuItem(this, Id, label, helpString, FALSE, SubMenu));
}

// Ordinary menu item
void wxMenu::Append(int Id, const wxString& label, 
                    const wxString& helpString, bool checkable)
{
  // 'checkable' parameter is useless for Windows.
    Append(new wxMenuItem(this, Id, label, helpString, checkable));
}

void wxMenu::Delete(int id)
{
    wxNode *node;
    wxMenuItem *item;
    int pos;

    for (pos = 0, node = m_menuItems.First(); node; node = node->Next(), pos++) {
	 item = (wxMenuItem *)node->Data();
	 if (item->GetId() == id)
		break;
    }

    if (!node)
		return;

    int index = pos + 1 ;

    if (index < 1)
		return;

	wxMenu *pSubMenu = item->GetSubMenu();
	if ( pSubMenu != NULL ) 
	{
		::DeleteMenuItem( m_macMenuHandle , index);
		pSubMenu->m_parent = NULL;
		// TODO: Why isn't subMenu deleted here???
		// Will put this in for now. Assuming this is supposed
		// to delete the menu, not just remove it.
		item->DeleteSubMenu();
	}
	else 
	{
		::DeleteMenuItem( m_macMenuHandle , index);
	}

    m_menuItems.DeleteNode(node);
   	// TODO shouldn't we do this ? Ž_m_noItems--; 
    delete item;
}

void wxMenu::Enable(int Id, bool Flag)
{
    wxMenuItem *item = FindItemForId(Id);
    wxCHECK_RET( item != NULL, "can't enable non-existing menu item" );

    item->Enable(Flag);
}

bool wxMenu::Enabled(int Id) const
{
    wxMenuItem *item = FindItemForId(Id);
    wxCHECK( item != NULL, FALSE );

    return item->IsEnabled();
}

void wxMenu::Check(int Id, bool Flag)
{
    wxMenuItem *item = FindItemForId(Id);
    wxCHECK_RET( item != NULL, "can't get status of non-existing menu item" );

    item->Check(Flag);
}

bool wxMenu::Checked(int Id) const
{
    wxMenuItem *item = FindItemForId(Id);
    wxCHECK( item != NULL, FALSE );

    return item->IsChecked();
}

void wxMenu::SetTitle(const wxString& label)
{
	Str255 title ;
    m_title = label ;
	wxMacBuildMenuString( title, NULL , NULL , label , false );
	UMASetMenuTitle( m_macMenuHandle , title ) ;
	if (wxMenuBar::s_macInstalledMenuBar == m_menuBar) // are we currently installed ?
	{
		::SetMenuBar( GetMenuBar() ) ;
		::InvalMenuBar() ;
	}
	// TODO:for submenus -> their parent item text must be corrected
}

const wxString wxMenu::GetTitle() const
{
    return m_title;
}

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
		::SetMenuItemText( m_macMenuHandle , index , maclabel ) ;
    }
    else
    {
		wxMacBuildMenuString( maclabel , NULL , NULL , label , false );
		::SetMenuItemText( m_macMenuHandle , index , maclabel ) ;
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

void wxMenu::ProcessCommand(wxCommandEvent & event)
{
    bool processed = FALSE;

    // Try a callback
    if (m_callback)
    {
            (void) (*(m_callback)) (*this, event);
            processed = TRUE;
    }

    // Try the menu's event handler
    if ( !processed && GetEventHandler())
    {
            processed = GetEventHandler()->ProcessEvent(event);
    }

  // Try the window the menu was popped up from (and up
  // through the hierarchy)
  if ( !processed && GetInvokingWindow())
    processed = GetInvokingWindow()->GetEventHandler()->ProcessEvent(event);
}

bool wxWindow::PopupMenu(wxMenu *menu, int x, int y)
{
	menu->SetInvokingWindow(this);
	ClientToScreen( &x , &y ) ;

	::InsertMenu( menu->m_macMenuHandle , -1 ) ;
  long menuResult = ::PopUpMenuSelect(menu->m_macMenuHandle ,y,x, 0) ;
  menu->MacMenuSelect( this , TickCount() , HiWord(menuResult) , LoWord(menuResult) ) ;
	::DeleteMenu( menu->m_macMenuId ) ;
  menu->SetInvokingWindow(NULL);

  return TRUE;
}

// helper functions returning the mac menu position for a certain item, note that this is 
// mac-wise 1 - based, i.e. the first item has index 1 whereas on MSWin it has pos 0

int wxMenu::MacGetIndexFromId( int id ) 
{
    wxNode *node;
    wxMenuItem *item;
    int pos;
	
    for (pos = 0, node = m_menuItems.First(); node; node = node->Next(), pos++) 
    {
	 	item = (wxMenuItem *)node->Data();
	 	if (item->GetId() == id)
			break;
    }

    if (!node)
		return 0;
		
	return pos + 1 ;
}

int wxMenu::MacGetIndexFromItem( wxMenuItem *pItem ) 
{
    wxNode *node;
    int pos;

    for (pos = 0, node = m_menuItems.First(); node; node = node->Next(), pos++) 
    {
	 	if ((wxMenuItem *)node->Data() == pItem)
			break;
    }

    if (!node)
		return 0;
		
	return pos + 1 ;
}

void wxMenu::MacEnableMenu( bool bDoEnable ) 
{
	m_macMenuEnabled = bDoEnable ;
	if ( bDoEnable )
		UMAEnableMenuItem( m_macMenuHandle , 0 ) ;
	else
		UMADisableMenuItem( m_macMenuHandle , 0 ) ;
		
	::DrawMenuBar() ;
}

bool wxMenu::MacMenuSelect( wxEvtHandler* handler, long when , int macMenuId, int macMenuItemNum )
{
  int pos;
  wxNode *node;

	if ( m_macMenuId == macMenuId )
	{
		node = m_menuItems.Nth(macMenuItemNum-1);
		if (node) 
		{
			wxMenuItem *pItem = (wxMenuItem*)node->Data();
	
			wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, pItem->GetId());
			event.m_timeStamp = when;
			event.SetEventObject(handler);
      event.SetInt( pItem->GetId() );

			{
				bool processed = false ;

		    // Try a callback
		    if (m_callback)
		    {
		            (void) (*(m_callback)) (*this, event);
		            processed = TRUE;
		    }
		
		    // Try the menu's event handler
		    if ( !processed && handler)
		    {
		            processed = handler->ProcessEvent(event);
		    }
		
		  	// Try the window the menu was popped up from (and up
		  	// through the hierarchy)
		  	if ( !processed && GetInvokingWindow())
		    	processed = GetInvokingWindow()->GetEventHandler()->ProcessEvent(event);
		  }
			return true ;
		}
	}
	else if ( macMenuId == kHMHelpMenuID )
	{
		int menuItem = formerHelpMenuItems ;
	  for (pos = 0, node = m_menuItems.First(); node; node = node->Next(), pos++) 
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
					
					{
						bool processed = false ;
				    // Try a callback
				    if (m_callback)
				    {
				            (void) (*(m_callback)) (*this, event);
				            processed = TRUE;
				    }
				
				    // Try the menu's event handler
				    if ( !processed && handler)
				    {
				            processed = handler->ProcessEvent(event);
				    }
				
				  	// Try the window the menu was popped up from (and up
				  	// through the hierarchy)
				  	if ( !processed && GetInvokingWindow())
				    	processed = GetInvokingWindow()->GetEventHandler()->ProcessEvent(event);
				  }
				  
					return true ;
				}
			}
	  }
	}

  for (pos = 0, node = m_menuItems.First(); node; node = node->Next(), pos++) 
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

wxMenuBar::wxMenuBar()
{
    m_eventHandler = this;
    m_menuCount = 0;
    m_menus = NULL;
    m_titles = NULL;
    m_menuBarFrame = NULL;
}

wxMenuBar::wxMenuBar(int n, wxMenu *menus[], const wxString titles[])
{
    m_eventHandler = this;
    m_menuCount = n;
    m_menus = menus;
    m_titles = new wxString[n];
    int i;
    for ( i = 0; i < n; i++ )
			m_titles[i] = titles[i];
    m_menuBarFrame = NULL;
}

wxMenuBar::~wxMenuBar()
{
	if (s_macInstalledMenuBar == this)
	{
		::ClearMenuBar();
		s_macInstalledMenuBar = NULL;
	}

    int i;
    for (i = 0; i < m_menuCount; i++)
    {
        delete m_menus[i];
    }
    delete[] m_menus;
    delete[] m_titles;

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
 
   	for (int i = 0; i < m_menuCount; i++)
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
					
		  	for (pos = 0 , node = menu->m_menuItems.First(); node; node = node->Next(), pos++) 
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
						wxMacBuildMenuString( label , NULL , NULL , item->GetName(), item->GetId() != wxApp::s_macAboutMenuItemId); // no shortcut in about menu
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
				UMASetMenuTitle( menu->m_macMenuHandle , label ) ;
		  	for (pos = 0, node = menu->m_menuItems.First(); node; node = node->Next(), pos++) 
	  		{
		 			item = (wxMenuItem *)node->Data();
		 			subMenu = item->GetSubMenu() ;
					if (subMenu)	 		
					{
						::InsertMenu( subMenu->m_macMenuHandle , -1 ) ;
					}
				}
				::InsertMenu(m_menus[i]->m_macMenuHandle, 0);
			}
		}
		::DrawMenuBar() ;

	s_macInstalledMenuBar = this;
}


// Must only be used AFTER menu has been attached to frame,
// otherwise use individual menus to enable/disable items
void wxMenuBar::Enable(int id, bool flag)
{
    wxMenu *itemMenu = NULL;
    wxMenuItem *item = FindItemForId(id, &itemMenu) ;
    if (!item)
        return;

	item->Enable( flag ) ;
}

void wxMenuBar::EnableTop(int pos, bool flag)
{
	m_menus[pos]->MacEnableMenu( flag ) ;
}

// Must only be used AFTER menu has been attached to frame,
// otherwise use individual menus
void wxMenuBar::Check(int id, bool flag)
{
    wxMenu *itemMenu = NULL;
    wxMenuItem *item = FindItemForId(id, &itemMenu) ;
    if (!item)
        return;

    if (!item->IsCheckable())
        return ;

	item->Check( flag ) ;
}

bool wxMenuBar::Checked(int id) const
{
    wxMenu *itemMenu = NULL;
    wxMenuItem *item = FindItemForId(id, &itemMenu) ;
    if (!item)
        return FALSE;

    if (!item->IsCheckable())
        return FALSE ;

    return item->IsChecked() ;
}

bool wxMenuBar::Enabled(int id) const
{
    wxMenu *itemMenu = NULL;
    wxMenuItem *item = FindItemForId(id, &itemMenu) ;
    if (!item)
        return FALSE;

	if ( !item->IsEnabled() )
		return FALSE ;
	
	if ( itemMenu->m_macMenuEnabled == false )
		return FALSE ;
		
	while( itemMenu->m_parent )
	{
		itemMenu = (wxMenu*) itemMenu->m_parent ;
		if ( itemMenu->IsKindOf( CLASSINFO( wxMenu ) ) )
		{
			if ( itemMenu->m_macMenuEnabled == false )
				return FALSE ;
		}
	}
	
    return TRUE ;
}


void wxMenuBar::SetLabel(int id, const wxString& label)
{
    wxMenu *itemMenu = NULL;
    wxMenuItem *item = FindItemForId(id, &itemMenu) ;

    if (!item)
        return;

    itemMenu->SetLabel( id , label ) ;
}

wxString wxMenuBar::GetLabel(int id) const
{
    wxMenu *itemMenu = NULL;
    wxMenuItem *item = FindItemForId(id, &itemMenu) ;

    if (!item)
        return wxString("");

    return itemMenu->GetLabel( id ) ;
}

void wxMenuBar::SetLabelTop(int pos, const wxString& label)
{
    m_menus[pos]->SetTitle( label ) ;
}

wxString wxMenuBar::GetLabelTop(int pos) const
{
   return m_menus[pos]->GetTitle() ;
}

bool wxMenuBar::OnDelete(wxMenu *a_menu, int pos)
{
	if (s_macInstalledMenuBar == this)
	{
		::DeleteMenu( a_menu->m_macMenuId /* m_menus[pos]->m_macMenuId */ ) ;
		::InvalMenuBar() ;
		return TRUE ;
	}
	else
	{
		return TRUE ;
	}
}

bool wxMenuBar::OnAppend(wxMenu *a_menu, const char *title)
{
	if (!a_menu->m_macMenuHandle)
	 	return FALSE;

	if (s_macInstalledMenuBar == this)
	{
		Str255 	label;
		wxMacBuildMenuString( label, NULL , NULL , title , false );
		UMASetMenuTitle( a_menu->m_macMenuHandle , label ) ;
		::InsertMenu( a_menu->m_macMenuHandle , 0 ) ;
		::InvalMenuBar() ;
		return TRUE ;
	}
	else
	{
		return TRUE ;
	}
}

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
        m_titles[i] = "";
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

  ((wxMenu *)menu)->m_menuBar = (wxMenuBar *) this;
  ((wxMenu *)menu)->SetParent(this);
}

void wxMenuBar::Delete(wxMenu * menu, int i)
{
    int j;
    int ii = (int) i;

    if (menu != 0)
    {
	    for (ii = 0; ii < m_menuCount; ii++)
        {
            if (m_menus[ii] == menu)
            break;
	    }
        if (ii >= m_menuCount)
            return;
    } else
    {
        if (ii < 0 || ii >= m_menuCount)
            return;
        menu = m_menus[ii];
    }

    if (!OnDelete(menu, ii))
        return;

    menu->SetParent(NULL);

    -- m_menuCount;
    for (j = ii; j < m_menuCount; j++)
    {
        m_menus[j] = m_menus[j + 1];
        m_titles[j] = m_titles[j + 1];
    }
}

// Find the menu menuString, item itemString, and return the item id.
// Returns -1 if none found.
int wxMenuBar::FindMenuItem (const wxString& menuString, const wxString& itemString) const
{
    char buf1[200];
    char buf2[200];
    wxStripMenuCodes ((char *)(const char *)menuString, buf1);
    int i;
    for (i = 0; i < m_menuCount; i++)
    {
        wxStripMenuCodes ((char *)(const char *)m_titles[i], buf2);
        if (strcmp (buf1, buf2) == 0)
            return m_menus[i]->FindItem (itemString);
    }
    return -1;
}

wxMenuItem *wxMenuBar::FindItemForId (int Id, wxMenu ** itemMenu) const
{
    if (itemMenu)
        *itemMenu = NULL;

    wxMenuItem *item = NULL;
    int i;
    for (i = 0; i < m_menuCount; i++)
        if ((item = m_menus[i]->FindItemForId (Id, itemMenu)))
            return item;
    return NULL;
}

void wxMenuBar::SetHelpString (int Id, const wxString& helpString)
{
    int i;
    for (i = 0; i < m_menuCount; i++)
    {
        if (m_menus[i]->FindItemForId (Id))
        {
            m_menus[i]->SetHelpString (Id, helpString);
            return;
        }
    }
}

wxString wxMenuBar::GetHelpString (int Id) const
{
    int i;
    for (i = 0; i < m_menuCount; i++)
    {
        if (m_menus[i]->FindItemForId (Id))
            return wxString(m_menus[i]->GetHelpString (Id));
    }
    return wxString("");
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
	  for (int i = 0; i < m_menuCount; i++)
	  {
	  	if ( m_menus[i]->m_macMenuId == macMenuId || 
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
	
	  for (int i = 0; i < m_menuCount; i++)
	  {
	  	if ( m_menus[i]->MacMenuSelect( handler , when , macMenuId , macMenuItemNum ) )
	  	{
	  		break ;
	  	}
		}
	}
}

