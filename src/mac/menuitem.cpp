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

// ctor & dtor
// -----------

wxMenuItem::wxMenuItem(wxMenu *pParentMenu, int id,
                       const wxString& strName, const wxString& strHelp,
                       bool bCheckable,
                       wxMenu *pSubMenu) :
                        m_bCheckable(bCheckable),
                        m_strName(strName),
                        m_strHelp(strHelp)
{
  wxASSERT( pParentMenu != NULL );

  m_pParentMenu = pParentMenu;
  m_pSubMenu    = pSubMenu;
  m_idItem      = id;
  m_bEnabled    = TRUE;

  if ( m_strName ==  "E&xit"  ||m_strName == "Exit" )
  {
  	m_strName = "Quit\tCtrl+Q" ;
  }
}

wxMenuItem::~wxMenuItem() 
{
}

// misc
// ----

// delete the sub menu
void wxMenuItem::DeleteSubMenu()
{
  wxASSERT( m_pSubMenu != NULL );

  delete m_pSubMenu;
  m_pSubMenu = NULL;
}

// change item state
// -----------------

void wxMenuItem::Enable(bool bDoEnable)
{
  if ( m_bEnabled != bDoEnable ) {
    if ( m_pSubMenu == NULL ) 
    {     
    	// normal menu item
	    if ( m_pParentMenu->m_macMenuHandle )
	    {
	   	 	int index = m_pParentMenu->MacGetIndexFromItem( this ) ;
	   	 	if ( index >= 1 )
	   	 	{
	   	 		if ( bDoEnable )
	   	 			UMAEnableMenuItem( m_pParentMenu->m_macMenuHandle , index ) ;
	   	 		else
	   	 			UMADisableMenuItem( m_pParentMenu->m_macMenuHandle , index ) ;
	   	 	}
	    }
    }
    else                            
    {
  		// submenu
	    if ( m_pParentMenu->m_macMenuHandle )
	    {
	   	 	int index = m_pParentMenu->MacGetIndexFromItem( this ) ;
	   	 	if ( index >= 1 )
	   	 	{
	   	 		if ( bDoEnable )
	   	 			UMAEnableMenuItem( m_pParentMenu->m_macMenuHandle , index ) ;
	   	 		else
	   	 			UMADisableMenuItem( m_pParentMenu->m_macMenuHandle , index ) ;
	   	 	}
	    }
    }

    m_bEnabled = bDoEnable;
  }
}

void wxMenuItem::Check(bool bDoCheck)
{
  wxCHECK_RET( IsCheckable(), "only checkable items may be checked" );

  if ( m_bChecked != bDoCheck ) 
  {
    m_bChecked = bDoCheck;
   	if ( m_pParentMenu->m_macMenuHandle )
    {
   	 	int index = m_pParentMenu->MacGetIndexFromItem( this ) ;
   	 	if ( index >= 1 )
   	 	{
   	 		if ( bDoCheck )
					::SetItemMark( m_pParentMenu->m_macMenuHandle , index , 0x12 ) ; // checkmark
				else
 					::SetItemMark( m_pParentMenu->m_macMenuHandle , index , 0 ) ; // no mark
  	 	}
  	}
  }
}