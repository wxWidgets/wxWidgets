///////////////////////////////////////////////////////////////////////////////
// Name:        menuitem.cpp
// Purpose:     wxMenuItem implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     11.11.97
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "menuitem.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/font.h"
    #include "wx/bitmap.h"
    #include "wx/settings.h"
    #include "wx/font.h"
    #include "wx/window.h"
    #include "wx/accel.h"
    #include "wx/menu.h"
    #include "wx/string.h"
#endif

#include "wx/ownerdrw.h"
#include "wx/menuitem.h"
#include "wx/log.h"

#include "wx/msw/private.h"

// ---------------------------------------------------------------------------
// convenience macro
// ---------------------------------------------------------------------------

#define GetHMenuOf(menu)    ((HMENU)menu->GetHMenu())

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// dynamic classes implementation
// ----------------------------------------------------------------------------

#if     !defined(USE_SHARED_LIBRARY) || !USE_SHARED_LIBRARY
#if wxUSE_OWNER_DRAWN
  IMPLEMENT_DYNAMIC_CLASS2(wxMenuItem, wxObject, wxOwnerDrawn)
#else   //!USE_OWNER_DRAWN
  IMPLEMENT_DYNAMIC_CLASS(wxMenuItem, wxObject)
#endif  //USE_OWNER_DRAWN

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
#if wxUSE_OWNER_DRAWN
                        wxOwnerDrawn(strName, bCheckable),
#else   //no owner drawn support
                        m_bCheckable(bCheckable),
                        m_strName(strName),
#endif  //owner drawn
                        m_strHelp(strHelp)
{
    wxASSERT_MSG( pParentMenu != NULL, _T("a menu item should have a parent") );

#if  wxUSE_OWNER_DRAWN
    // set default menu colors
    #define SYS_COLOR(c) (wxSystemSettings::GetSystemColour(wxSYS_COLOUR_##c))

    SetTextColour(SYS_COLOR(MENUTEXT));
    SetBackgroundColour(SYS_COLOR(MENU));

    // we don't want normal items be owner-drawn
    ResetOwnerDrawn();

    #undef  SYS_COLOR
#endif

    m_pParentMenu = pParentMenu;
    m_pSubMenu    = pSubMenu;
    m_bEnabled    = TRUE;
    m_bChecked    = FALSE;
    m_idItem      = id;
}

wxMenuItem::~wxMenuItem()
{
}

// misc
// ----

// return the id for calling Win32 API functions
int wxMenuItem::GetRealId() const
{
    return m_pSubMenu ? (int)m_pSubMenu->GetHMenu() : GetId();
}

// delete the sub menu
// -------------------
void wxMenuItem::DeleteSubMenu()
{
    delete m_pSubMenu;
    m_pSubMenu = NULL;
}

// change item state
// -----------------

void wxMenuItem::Enable(bool bDoEnable)
{
    if ( m_bEnabled != bDoEnable ) {
        long rc = EnableMenuItem(GetHMenuOf(m_pParentMenu),
                                 GetRealId(),
                                 MF_BYCOMMAND |
                                 (bDoEnable ? MF_ENABLED : MF_GRAYED));

        if ( rc == -1 ) {
            wxLogLastError("EnableMenuItem");
        }

        m_bEnabled = bDoEnable;
    }
}

void wxMenuItem::Check(bool bDoCheck)
{
    wxCHECK_RET( IsCheckable(), _T("only checkable items may be checked") );

    if ( m_bChecked != bDoCheck ) {
        long rc = CheckMenuItem(GetHMenuOf(m_pParentMenu),
                                GetId(),
                                MF_BYCOMMAND |
                                (bDoCheck ? MF_CHECKED : MF_UNCHECKED));

        if ( rc == -1 ) {
            wxLogLastError("CheckMenuItem");
        }

        m_bChecked = bDoCheck;
    }
}

void wxMenuItem::SetName(const wxString& strName)
{
    // don't do anything if label didn't change
    if ( m_strName == strName )
        return;

    m_strName = strName;

    HMENU hMenu = GetHMenuOf(m_pParentMenu);

    UINT id = GetRealId();
    UINT flagsOld = ::GetMenuState(hMenu, id, MF_BYCOMMAND);
    if ( flagsOld == 0xFFFFFFFF )
    {
        wxLogLastError("GetMenuState");
    }
    else
    {
        if ( IsSubMenu() )
        {
            // high byte contains the number of items in a submenu for submenus
            flagsOld &= 0xFF;
            flagsOld |= MF_POPUP;
        }

        LPCTSTR data;
#if wxUSE_OWNER_DRAWN
        if ( IsOwnerDrawn() )
        {
            flagsOld |= MF_OWNERDRAW;
            data = (LPCTSTR)this;
        }
        else
#endif  //owner drawn
        {
            flagsOld |= MF_STRING;
            data = strName;
        }

        if ( ::ModifyMenu(hMenu, id,
                          MF_BYCOMMAND | flagsOld,
                          id, data) == 0xFFFFFFFF )
        {
            wxLogLastError(_T("ModifyMenu"));
        }
    }
}

