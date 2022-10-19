///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/menuitem_osx.cpp
// Purpose:     wxMenuItem implementation
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_MENUS

#include "wx/menuitem.h"
#include "wx/stockitem.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/menu.h"
#endif // WX_PRECOMP

#include "wx/osx/private.h"
#include "wx/osx/private/available.h"

wxIMPLEMENT_ABSTRACT_CLASS(wxMenuItemImpl, wxObject);

wxMenuItemImpl::~wxMenuItemImpl()
{
}

wxMenuItem::wxMenuItem(wxMenu *pParentMenu,
                       int id,
                       const wxString& t,
                       const wxString& strHelp,
                       wxItemKind kind,
                       wxMenu *pSubMenu)
           :wxMenuItemBase(pParentMenu, id, t, strHelp, kind, pSubMenu)
{
    wxASSERT_MSG( id != 0 || pSubMenu != nullptr , wxT("A MenuItem ID of Zero does not work under Mac") ) ;

    // In other languages there is no difference in naming the Exit/Quit menu item between MacOS and Windows guidelines
    // therefore these item must not be translated
    if (pParentMenu != nullptr && !pParentMenu->GetNoEventsMode())
        if ( wxStripMenuCodes(m_text, wxStrip_Menu).Upper() == wxT("EXIT") )
            m_text = wxT("Quit\tCtrl+Q") ;

    wxString text = wxStripMenuCodes(m_text, (pParentMenu != nullptr && pParentMenu->GetNoEventsMode()) ? wxStrip_Accel : wxStrip_Menu);
    if (text.IsEmpty() && !IsSeparator())
    {
        wxASSERT_MSG(wxIsStockID(GetId()), wxT("A non-stock menu item with an empty label?"));
        text = wxGetStockLabel(GetId(), wxSTOCK_WITH_ACCELERATOR|wxSTOCK_WITH_MNEMONIC);
    }

    // use accessors for ID and Kind because they might have been changed in the base constructor
#if wxUSE_ACCEL
    wxAcceleratorEntry *entry = wxAcceleratorEntry::Create( m_text ) ;
    m_peer = wxMenuItemImpl::Create( this, pParentMenu, GetId(), text, entry, strHelp, GetKind(), pSubMenu );
    delete entry;
#else
    m_peer = wxMenuItemImpl::Create( this, pParentMenu, GetId(), text, nullptr, strHelp, GetKind(), pSubMenu );
#endif // wxUSE_ACCEL/!wxUSE_ACCEL
}

wxMenuItem::~wxMenuItem()
{
    delete m_peer;
}

// change item state
// -----------------

void wxMenuItem::Enable(bool bDoEnable)
{
    if (( m_isEnabled != bDoEnable
      // avoid changing menuitem state when menu is disabled
      // eg. BeginAppModalStateForWindow() will disable menus and ignore this change
      // which in turn causes m_isEnabled to become out of sync with real menuitem state
         )
      // always update builtin menuitems
         || (   GetId() == wxApp::s_macPreferencesMenuItemId
             || GetId() == wxApp::s_macExitMenuItemId
             || GetId() == wxApp::s_macAboutMenuItemId
         ))
    {
        wxMenuItemBase::Enable( bDoEnable ) ;
        UpdateItemStatus() ;
    }
}

void wxMenuItem::UncheckRadio()
{
    if ( m_isChecked )
    {
        wxMenuItemBase::Check( false ) ;
        UpdateItemStatus() ;
    }
}

void wxMenuItem::Check(bool bDoCheck)
{
    wxCHECK_RET( IsCheckable() && !IsSeparator(), wxT("only checkable items may be checked") );

    if ( m_isChecked != bDoCheck )
    {
        if ( GetKind() == wxITEM_RADIO )
        {
            if ( bDoCheck )
            {
                wxMenuItemBase::Check( bDoCheck ) ;
                UpdateItemStatus() ;

                // get the index of this item in the menu
                const wxMenuItemList& items = m_parentMenu->GetMenuItems();
                int pos = items.IndexOf(this);
                wxCHECK_RET( pos != wxNOT_FOUND,
                             wxT("menuitem not found in the menu items list?") );

                // get the radio group range
                int start, end;
                if ( !m_parentMenu->OSXGetRadioGroupRange(pos, &start, &end) )
                {
                    wxFAIL_MSG( wxS("Menu radio item not part of radio group?") );
                }

                // also uncheck all the other items in this radio group
                wxMenuItemList::compatibility_iterator node = items.Item(start);
                for ( int n = start; n <= end && node; n++ )
                {
                    if ( n != pos )
                        ((wxMenuItem*)node->GetData())->UncheckRadio();

                    node = node->GetNext();
                }
            }
        }
        else
        {
            wxMenuItemBase::Check( bDoCheck ) ;
            UpdateItemStatus() ;
        }
    }
}

void wxMenuItem::SetItemLabel(const wxString& text)
{
    // don't do anything if label didn't change
    if ( m_text == text )
        return;

    wxMenuItemBase::SetItemLabel(text);

    UpdateItemText() ;
}


void wxMenuItem::UpdateItemBitmap()
{
    if ( !m_parentMenu )
        return;

    if ( m_bitmap.IsOk() )
    {
        GetPeer()->SetBitmap(GetBitmap());
    }
}

void wxMenuItem::UpdateItemStatus()
{
    if ( !m_parentMenu )
        return ;

    if ( IsSeparator() )
        return ;

    if ( IsCheckable() && IsChecked() )
        GetPeer()->Check( true );
    else
        GetPeer()->Check( false );

    GetPeer()->Enable( IsEnabled() );
}

void wxMenuItem::UpdateItemText()
{
    if ( !m_parentMenu )
        return ;

    wxString text = wxStripMenuCodes(m_text, m_parentMenu != nullptr && m_parentMenu->GetNoEventsMode() ? wxStrip_Accel : wxStrip_Menu);
    if (text.IsEmpty() && !IsSeparator())
    {
        wxASSERT_MSG(wxIsStockID(GetId()), wxT("A non-stock menu item with an empty label?"));
        text = wxGetStockLabel(GetId(), wxSTOCK_WITH_ACCELERATOR|wxSTOCK_WITH_MNEMONIC);
    }

#if wxUSE_ACCEL
    wxAcceleratorEntry *entry = wxAcceleratorEntry::Create( m_text ) ;
    GetPeer()->SetLabel( text, entry );
    delete entry ;
#else
    GetPeer()->SetLabel( text, nullptr );
#endif // wxUSE_ACCEL/!wxUSE_ACCEL
}

#if wxUSE_ACCEL

void wxMenuItem::AddExtraAccel(const wxAcceleratorEntry& accel)
{
    if (WX_IS_MACOS_AVAILABLE(10, 13))
    {
        wxMenuItemBase::AddExtraAccel(accel);

        // create the same wxMenuItem but hidden and with different accelerator.
        wxMenuItem* hiddenMenuItem = new wxMenuItem(m_parentMenu, GetId(), m_text, m_help, GetKind(), m_subMenu);
        hiddenMenuItem->SetAccel(&(m_extraAccels.back()));
        hiddenMenuItem->GetPeer()->Hide(true);
        hiddenMenuItem->GetPeer()->SetAllowsKeyEquivalentWhenHidden(true);
        m_parentMenu->GetPeer()->InsertOrAppend( hiddenMenuItem, -1 );
        hiddenMenuItem->SetMenu(m_parentMenu);
        m_hiddenMenuItems.push_back(hiddenMenuItem);
    }
    else
    {
        wxLogDebug("Extra accelerators not being supported under macOS < 10.13");
    }
}

void wxMenuItem::ClearExtraAccels()
{
    wxMenuItemBase::ClearExtraAccels();
    RemoveHiddenItems();
}

void wxMenuItem::RemoveHiddenItems()
{
    for (size_t i = 0; i < m_hiddenMenuItems.size(); ++i)
    {
        m_parentMenu->GetPeer()->Remove( m_hiddenMenuItems[i] );
    }
}

#endif // wxUSE_ACCEL

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

#endif
