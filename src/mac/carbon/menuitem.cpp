///////////////////////////////////////////////////////////////////////////////
// Name:        menuitem.cpp
// Purpose:     wxMenuItem implementation
// Author:      AUTHOR
// Modified by: 
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// headers & declarations
// ============================================================================

#include "wx/menu.h"
#include "wx/menuitem.h"

#include <wx/mac/uma.h>
// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// dynamic classes implementation
// ----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARY
  IMPLEMENT_DYNAMIC_CLASS(wxMenuItem, wxObject)
#endif  //USE_SHARED_LIBRARY

// ----------------------------------------------------------------------------
// wxMenuItem
// ----------------------------------------------------------------------------

//
// Helper Functions to get Mac Menus the way they should be ;-)
//

void wxMacCtoPString(const char* theCString, Str255 thePString);

// remove inappropriate characters, if useShortcuts is false, the ampersand will not auto-generate a mac menu-shortcut

int wxMenuItem::MacBuildMenuString(StringPtr outMacItemText, SInt16 *outMacShortcutChar , UInt8 *outMacModifiers , const char *inItemText , bool useShortcuts )
{
	char *p = (char *) &outMacItemText[1] ;
	short macModifiers = 0 ;
	char macShortCut = 0 ;
	const char *inItemName ;
	wxString inItemTextMac ;
	
	if (wxApp::s_macDefaultEncodingIsPC)
	{
		inItemTextMac =  wxMacMakeMacStringFromPC( inItemText ) ;
		inItemName = inItemTextMac ;
	}
	else
	{
		inItemName = inItemText ;
	}
	
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
				*p++ = '[' ;
				break ;
			case '>' :
				*p++ = ']' ;
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
						if (strncmp("Ctrl", inItemName, 4) == 0) 
						{
							inItemName = inItemName + 5;
							macShortCut = *inItemName;
						}
						else if (strncmp("Cntrl", inItemName, 5) == 0) 
						{
							inItemName = inItemName + 6;
							macShortCut = *inItemName;
						}
						else if (strncmp("Alt", inItemName, 3) == 0) 
						{
							inItemName = inItemName + 4;
							macModifiers |= kMenuOptionModifier ;
							macShortCut = *inItemName ;
						}
						else if (strncmp("Shift", inItemName, 5) == 0) 
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
		
	return 0 ;
}

// ctor & dtor
// -----------

wxMenuItem::wxMenuItem(wxMenu *pParentMenu, int id,
                       const wxString& text, const wxString& strHelp,
                       bool bCheckable,
                       wxMenu *pSubMenu) 
{
    wxASSERT( pParentMenu != NULL );

    m_parentMenu  = pParentMenu;
    m_subMenu     = pSubMenu;
    m_isEnabled   = TRUE;
    m_isChecked   = FALSE;
    m_id          = id;
    m_text        = text;
    m_isCheckable = bCheckable;
    m_help        = strHelp;


    if ( m_text ==  "E&xit"  ||m_text == "Exit" )
    {
        m_text = "Quit\tCtrl+Q" ;
    }
}

wxMenuItem::~wxMenuItem() 
{
}

bool wxMenuItem::IsChecked() const
{
    return wxMenuItemBase::IsChecked() ;
}

wxString wxMenuItem::GetLabel() const
{
    return wxStripMenuCodes(m_text);
}

// accelerators
// ------------

#if wxUSE_ACCEL

wxAcceleratorEntry *wxMenuItem::GetAccel() const
{
    return wxGetAccelFromString(GetText());
}

#endif // wxUSE_ACCEL

// misc
// ----

/*

// delete the sub menu
void wxMenuItem::DeleteSubMenu()
{
  wxASSERT( m_subMenu != NULL );

  delete m_subMenu;
  m_subMenu = NULL;
}

*/

// change item state
// -----------------

void wxMenuItem::Enable(bool bDoEnable)
{
  if ( m_isEnabled != bDoEnable ) {
    if ( m_subMenu == NULL ) 
    {     
    	// normal menu item
	    if ( m_parentMenu->GetHMenu() )
	    {
	   	 	int index = m_parentMenu->MacGetIndexFromItem( this ) ;
	   	 	if ( index >= 1 )
	   	 	{
	   	 		if ( bDoEnable )
	   	 			UMAEnableMenuItem( m_parentMenu->GetHMenu() , index ) ;
	   	 		else
	   	 			UMADisableMenuItem( m_parentMenu->GetHMenu() , index ) ;
	   	 	}
	    }
    }
    else                            
    {
  		// submenu
	    if ( m_parentMenu->GetHMenu() )
	    {
	   	 	int index = m_parentMenu->MacGetIndexFromItem( this ) ;
	   	 	if ( index >= 1 )
	   	 	{
	   	 		if ( bDoEnable )
	   	 			UMAEnableMenuItem( m_parentMenu->GetHMenu() , index ) ;
	   	 		else
	   	 			UMADisableMenuItem( m_parentMenu->GetHMenu() , index ) ;
	   	 	}
	    }
    }

    m_isEnabled = bDoEnable;
  }
}

void wxMenuItem::Check(bool bDoCheck)
{
  wxCHECK_RET( IsCheckable(), "only checkable items may be checked" );

  if ( m_isChecked != bDoCheck ) 
  {
    m_isChecked = bDoCheck;
   	if ( m_parentMenu->GetHMenu() )
    {
   	 	int index = m_parentMenu->MacGetIndexFromItem( this ) ;
   	 	if ( index >= 1 )
   	 	{
   	 		if ( bDoCheck )
					::SetItemMark( m_parentMenu->GetHMenu() , index , 0x12 ) ; // checkmark
				else
 					::SetItemMark( m_parentMenu->GetHMenu() , index , 0 ) ; // no mark
  	 	}
  	}
  }
}

void wxMenuItem::SetText(const wxString& text)
{
    // don't do anything if label didn't change
    if ( m_text == text )
        return;

    wxMenuItemBase::SetText(text);
//    OWNER_DRAWN_ONLY( wxOwnerDrawn::SetName(text) );

    wxCHECK_RET( m_parentMenu && m_parentMenu->GetHMenu(), wxT("menuitem without menu") );
   	if ( m_parentMenu->GetHMenu() )
    {
   	 	int index = m_parentMenu->MacGetIndexFromItem( this ) ;
   	 	if ( index >= 1 )
   	 	{
 			Str255 label;
			MacBuildMenuString( label , NULL , NULL , text ,false);
   	 		UMASetMenuItemText( m_parentMenu->GetHMenu() , index , label ) ; // checkmark
  	 	}
  	}

#if wxUSE_ACCEL
    m_parentMenu->UpdateAccel(this);
#endif // wxUSE_ACCEL

}
void wxMenuItem::SetCheckable(bool checkable)
{
    wxMenuItemBase::SetCheckable(checkable);
   // OWNER_DRAWN_ONLY( wxOwnerDrawn::SetCheckable(checkable) );
}

// ----------------------------------------------------------------------------
// wxMenuItemBase
// ----------------------------------------------------------------------------

/* static */
wxString wxMenuItemBase::GetLabelFromText(const wxString& text)
{
    return wxStripMenuCodes(text);
}

wxMenuItem *wxMenuItemBase::New(wxMenu *parentMenu,
                                int id,
                                const wxString& name,
                                const wxString& help,
                                bool isCheckable,
                                wxMenu *subMenu)
{
    return new wxMenuItem(parentMenu, id, name, help, isCheckable, subMenu);
}
