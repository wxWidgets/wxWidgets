///////////////////////////////////////////////////////////////////////////////
// Name:        menuitem.cpp
// Purpose:     wxMenuItem implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     11.11.97
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
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

#if wxUSE_MENUS

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

#include "wx/menuitem.h"
#include "wx/log.h"

#if wxUSE_ACCEL
    #include "wx/accel.h"
#endif // wxUSE_ACCEL

#include "wx/msw/private.h"

// ---------------------------------------------------------------------------
// macro
// ---------------------------------------------------------------------------

// hide the ugly cast
#define GetHMenuOf(menu)    ((HMENU)menu->GetHMenu())

// conditional compilation
#if wxUSE_OWNER_DRAWN
    #define OWNER_DRAWN_ONLY( code ) if ( IsOwnerDrawn() ) code
#else // !wxUSE_OWNER_DRAWN
    #define OWNER_DRAWN_ONLY( code )
#endif // wxUSE_OWNER_DRAWN/!wxUSE_OWNER_DRAWN

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// dynamic classes implementation
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMenuItem, wxObject)

// ----------------------------------------------------------------------------
// wxMenuItem
// ----------------------------------------------------------------------------

// ctor & dtor
// -----------

wxMenuItem::wxMenuItem(wxMenu *pParentMenu,
                       int id,
                       const wxString& text,
                       const wxString& strHelp,
                       wxItemKind kind,
                       wxMenu *pSubMenu)
          : wxMenuItemBase(pParentMenu, id, text, strHelp, kind, pSubMenu)
#if wxUSE_OWNER_DRAWN
            , wxOwnerDrawn(text, kind == wxITEM_CHECK)
#endif // owner drawn
{
    Init();
}

wxMenuItem::wxMenuItem(wxMenu *parentMenu,
                       int id,
                       const wxString& text,
                       const wxString& help,
                       bool isCheckable,
                       wxMenu *subMenu)
          : wxMenuItemBase(parentMenu, id, text, help,
                           isCheckable ? wxITEM_CHECK : wxITEM_NORMAL, subMenu)
#if wxUSE_OWNER_DRAWN
           , wxOwnerDrawn(text, isCheckable, true)
#endif // owner drawn
{
    Init();
}

void wxMenuItem::Init()
{
    m_radioGroup.start = -1;
    m_isRadioGroupStart = FALSE;

#if  wxUSE_OWNER_DRAWN
    // set default menu colors
    #define SYS_COLOR(c) (wxSystemSettings::GetColour(wxSYS_COLOUR_##c))

    SetTextColour(SYS_COLOR(MENUTEXT));
    SetBackgroundColour(SYS_COLOR(MENU));

    #undef  SYS_COLOR

    // we don't want normal items be owner-drawn
    ResetOwnerDrawn();

    // tell the owner drawing code to to show the accel string as well
    SetAccelString(m_text.AfterFirst(_T('\t')));
#endif // wxUSE_OWNER_DRAWN
}

wxMenuItem::~wxMenuItem()
{
}

// misc
// ----

// return the id for calling Win32 API functions
int wxMenuItem::GetRealId() const
{
    return m_subMenu ? (int)m_subMenu->GetHMenu() : GetId();
}

// get item state
// --------------

bool wxMenuItem::IsChecked() const
{
    int flag = ::GetMenuState(GetHMenuOf(m_parentMenu), GetId(), MF_BYCOMMAND);

    return (flag & MF_CHECKED) != 0;
}

/* static */
wxString wxMenuItemBase::GetLabelFromText(const wxString& text)
{
    return wxStripMenuCodes(text);
}

// radio group stuff
// -----------------

void wxMenuItem::SetAsRadioGroupStart()
{
    m_isRadioGroupStart = TRUE;
}

void wxMenuItem::SetRadioGroupStart(int start)
{
    wxASSERT_MSG( !m_isRadioGroupStart,
                  _T("should only be called for the next radio items") );

    m_radioGroup.start = start;
}

void wxMenuItem::SetRadioGroupEnd(int end)
{
    wxASSERT_MSG( m_isRadioGroupStart,
                  _T("should only be called for the first radio item") );

    m_radioGroup.end = end;
}

// change item state
// -----------------

void wxMenuItem::Enable(bool enable)
{
    if ( m_isEnabled == enable )
        return;

    long rc = EnableMenuItem(GetHMenuOf(m_parentMenu),
                             GetRealId(),
                             MF_BYCOMMAND |
                             (enable ? MF_ENABLED : MF_GRAYED));

    if ( rc == -1 ) {
        wxLogLastError(wxT("EnableMenuItem"));
    }

    wxMenuItemBase::Enable(enable);
}

void wxMenuItem::Check(bool check)
{
    wxCHECK_RET( IsCheckable(), wxT("only checkable items may be checked") );

    if ( m_isChecked == check )
        return;

    int flags = check ? MF_CHECKED : MF_UNCHECKED;
    HMENU hmenu = GetHMenuOf(m_parentMenu);

    if ( GetKind() == wxITEM_RADIO )
    {
        // it doesn't make sense to uncheck a radio item - what would this do?
        if ( !check )
            return;

        // get the index of this item in the menu
        const wxMenuItemList& items = m_parentMenu->GetMenuItems();
        int pos = items.IndexOf(this);
        wxCHECK_RET( pos != wxNOT_FOUND,
                     _T("menuitem not found in the menu items list?") );

        // get the radio group range
        int start,
            end;

        if ( m_isRadioGroupStart )
        {
            // we already have all information we need
            start = pos;
            end = m_radioGroup.end;
        }
        else // next radio group item
        {
            // get the radio group end from the start item
            start = m_radioGroup.start;
            end = items.Item(start)->GetData()->m_radioGroup.end;
        }

#ifdef __WIN32__
        // calling CheckMenuRadioItem() with such parameters hangs my system
        // (NT4 SP6) and I suspect this could happen to the others as well - so
        // don't do it!
        wxCHECK_RET( start != -1 && end != -1,
                     _T("invalid ::CheckMenuRadioItem() parameter(s)") );

        if ( !::CheckMenuRadioItem(hmenu,
                                   start,   // the first radio group item
                                   end,     // the last one
                                   pos,     // the one to check
                                   MF_BYPOSITION) )
        {
            wxLogLastError(_T("CheckMenuRadioItem"));
        }
#endif // __WIN32__

        // also uncheck all the other items in this radio group
        wxMenuItemList::Node *node = items.Item(start);
        for ( int n = start; n <= end && node; n++ )
        {
            if ( n != pos )
            {
                node->GetData()->m_isChecked = FALSE;
            }

            // we also have to do it in the menu for Win16 (under Win32
            // CheckMenuRadioItem() does it for us)
#ifndef __WIN32__
            ::CheckMenuItem(hmenu, n, n == pos ? MF_CHECKED : MF_UNCHECKED);
#endif // Win16

            node = node->GetNext();
        }
    }
    else // check item
    {
        if ( ::CheckMenuItem(hmenu,
                             GetRealId(),
                             MF_BYCOMMAND | flags) == (DWORD)-1 )
        {
            wxLogLastError(wxT("CheckMenuItem"));
        }
    }

    wxMenuItemBase::Check(check);
}

void wxMenuItem::SetText(const wxString& text)
{
    // don't do anything if label didn't change
    if ( m_text == text )
        return;

    wxMenuItemBase::SetText(text);
    OWNER_DRAWN_ONLY( wxOwnerDrawn::SetName(text) );
#if wxUSE_OWNER_DRAWN
    // tell the owner drawing code to to show the accel string as well
    SetAccelString(text.AfterFirst(_T('\t')));
#endif

    HMENU hMenu = GetHMenuOf(m_parentMenu);
    wxCHECK_RET( hMenu, wxT("menuitem without menu") );

#if wxUSE_ACCEL
    m_parentMenu->UpdateAccel(this);
#endif // wxUSE_ACCEL

    UINT id = GetRealId();
    UINT flagsOld = ::GetMenuState(hMenu, id, MF_BYCOMMAND);
    if ( flagsOld == 0xFFFFFFFF )
    {
        wxLogLastError(wxT("GetMenuState"));
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
            data = (wxChar*) text.c_str();
        }

        if ( ::ModifyMenu(hMenu, id,
                          MF_BYCOMMAND | flagsOld,
                          id, data) == (int)0xFFFFFFFF )
        {
            wxLogLastError(wxT("ModifyMenu"));
        }
    }
}

void wxMenuItem::SetCheckable(bool checkable)
{
    wxMenuItemBase::SetCheckable(checkable);
    OWNER_DRAWN_ONLY( wxOwnerDrawn::SetCheckable(checkable) );
}

// ----------------------------------------------------------------------------
// wxMenuItemBase
// ----------------------------------------------------------------------------

wxMenuItem *wxMenuItemBase::New(wxMenu *parentMenu,
                                int id,
                                const wxString& name,
                                const wxString& help,
                                wxItemKind kind,
                                wxMenu *subMenu)
{
    return new wxMenuItem(parentMenu, id, name, help, kind, subMenu);
}

#endif // wxUSE_MENUS
