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
    if ( m_pSubMenu == NULL ) {     // normal menu item
        // TODO
    }
    else                            // submenu
    {
        // TODO
    }

    m_bEnabled = bDoEnable;
  }
}

void wxMenuItem::Check(bool bDoCheck)
{
  wxCHECK_RET( IsCheckable(), "only checkable items may be checked" );

  if ( m_bChecked != bDoCheck ) {
    // TODO
    m_bChecked = bDoCheck;
  }
