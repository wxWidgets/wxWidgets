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

  IMPLEMENT_DYNAMIC_CLASS(wxMenuItem, wxObject)

void wxMacBuildMenuString(StringPtr outMacItemText, char *outMacShortcutChar , short *outMacModifiers , const char *inItemName , bool useShortcuts ) ;

wxString wxMenuItemBase::GetLabelFromText(const wxString& text)
{
    return wxStripMenuCodes(text);
}

// ----------------------------------------------------------------------------
// wxMenuItem
// ----------------------------------------------------------------------------

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
			wxMacBuildMenuString( label , NULL , NULL , text ,false);
   	 		::SetMenuItemText( m_parentMenu->GetHMenu() , index , label ) ; // checkmark
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

wxMenuItem *wxMenuItemBase::New(wxMenu *parentMenu,
                                int id,
                                const wxString& name,
                                const wxString& help,
                                bool isCheckable,
                                wxMenu *subMenu)
{
    return new wxMenuItem(parentMenu, id, name, help, isCheckable, subMenu);
}
