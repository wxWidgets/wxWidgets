///////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/menuitem.cpp
// Purpose:     wxMenuItem implementation
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/menuitem.h"
#include "wx/stockitem.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/menu.h"
#endif // WX_PRECOMP

#include "wx/mac/uma.h"

IMPLEMENT_DYNAMIC_CLASS(wxMenuItem, wxObject)


wxMenuItem::wxMenuItem(wxMenu *pParentMenu,
                       int id,
                       const wxString& text,
                       const wxString& strHelp,
                       wxItemKind kind,
                       wxMenu *pSubMenu)
           :wxMenuItemBase(pParentMenu, id, text, strHelp, kind, pSubMenu)
{
    wxASSERT_MSG( id != 0 || pSubMenu != NULL , wxT("A MenuItem ID of Zero does not work under Mac") ) ;

    // In other languages there is no difference in naming the Exit/Quit menu item between MacOS and Windows guidelines
    // therefore these item must not be translated
    if ( wxStripMenuCodes(m_text).Upper() == wxT("EXIT") )
        m_text = wxT("Quit\tCtrl+Q") ;

    m_radioGroup.start = -1;
    m_isRadioGroupStart = false;
}

wxMenuItem::~wxMenuItem()
{
}

// change item state
// -----------------

void wxMenuItem::SetBitmap(const wxBitmap& bitmap)
{
      m_bitmap = bitmap;
      UpdateItemBitmap();
}

void wxMenuItem::UpdateItemBitmap()
{
    if ( !m_parentMenu )
        return ;

    MenuHandle mhandle = MAC_WXHMENU(m_parentMenu->GetHMenu()) ;
    MenuItemIndex index = m_parentMenu->MacGetIndexFromItem( this ) ;
    if ( mhandle == NULL || index == 0)
        return ;

    if ( m_bitmap.Ok() )
    {
#if wxUSE_BMPBUTTON
        ControlButtonContentInfo info ;
        wxMacCreateBitmapButton( &info , m_bitmap ) ;
        if ( info.contentType != kControlNoContent )
        {
            if ( info.contentType == kControlContentIconRef )
                SetMenuItemIconHandle( mhandle , index ,
                    kMenuIconRefType , (Handle) info.u.iconRef ) ;
        }
        wxMacReleaseBitmapButton( &info ) ;
#endif
    }
}

void wxMenuItem::UpdateItemStatus()
{
    if ( !m_parentMenu )
        return ;

    if ( IsSeparator() )
        return ;

#if TARGET_CARBON
    if ( UMAGetSystemVersion() >= 0x1000 && GetId() == wxApp::s_macPreferencesMenuItemId)
    {
        if ( !IsEnabled() )
            DisableMenuCommand( NULL , kHICommandPreferences ) ;
        else
            EnableMenuCommand( NULL , kHICommandPreferences ) ;
    }

    if ( UMAGetSystemVersion() >= 0x1000 && GetId() == wxApp::s_macExitMenuItemId)
    {
        if ( !IsEnabled() )
            DisableMenuCommand( NULL , kHICommandQuit ) ;
        else
            EnableMenuCommand( NULL , kHICommandQuit ) ;
    }
#endif

    {
        MenuHandle mhandle = MAC_WXHMENU(m_parentMenu->GetHMenu()) ;
        MenuItemIndex index = m_parentMenu->MacGetIndexFromItem( this ) ;
        if ( mhandle == NULL || index == 0)
            return ;

        UMAEnableMenuItem( mhandle , index , m_isEnabled ) ;
        if ( IsCheckable() && IsChecked() )
            ::SetItemMark( mhandle , index , 0x12 ) ; // checkmark
        else
            ::SetItemMark( mhandle , index , 0 ) ; // no mark

        UMASetMenuItemText( mhandle , index , wxStripMenuCodes(m_text) , wxFont::GetDefaultEncoding() ) ;
        wxAcceleratorEntry *entry = wxAcceleratorEntry::Create( m_text ) ;
        UMASetMenuItemShortcut( mhandle , index , entry ) ;
        delete entry ;
    }
}

void wxMenuItem::UpdateItemText()
{
    if ( !m_parentMenu )
        return ;

    MenuHandle mhandle = MAC_WXHMENU(m_parentMenu->GetHMenu()) ;
    MenuItemIndex index = m_parentMenu->MacGetIndexFromItem( this ) ;
    if (mhandle == NULL || index == 0)
        return ;

    wxString text = m_text;
    if (text.IsEmpty() && !IsSeparator())
    {
        wxASSERT_MSG(wxIsStockID(GetId()), wxT("A non-stock menu item with an empty label?"));
        text = wxGetStockLabel(GetId(), wxSTOCK_WITH_ACCELERATOR|wxSTOCK_WITH_MNEMONIC);
    }

    UMASetMenuItemText( mhandle , index , wxStripMenuCodes(text) , wxFont::GetDefaultEncoding() ) ;
    wxAcceleratorEntry *entry = wxAcceleratorEntry::Create( text ) ;
    UMASetMenuItemShortcut( mhandle , index , entry ) ;
    delete entry ;
}

void wxMenuItem::Enable(bool bDoEnable)
{
    if (( m_isEnabled != bDoEnable
#if TARGET_CARBON
      // avoid changing menuitem state when menu is disabled
      // eg. BeginAppModalStateForWindow() will disable menus and ignore this change
      // which in turn causes m_isEnabled to become out of sync with real menuitem state
         && !(m_parentMenu && !IsMenuItemEnabled(MAC_WXHMENU(m_parentMenu->GetHMenu()), 0)) )
      // always update builtin menuitems
         || (   GetId() == wxApp::s_macPreferencesMenuItemId
             || GetId() == wxApp::s_macExitMenuItemId
             || GetId() == wxApp::s_macAboutMenuItemId
#endif
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
                             _T("menuitem not found in the menu items list?") );

                // get the radio group range
                int start, end;

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

void wxMenuItem::SetText(const wxString& text)
{
    // don't do anything if label didn't change
    if ( m_text == text )
        return;

    wxMenuItemBase::SetText(text);

    UpdateItemText() ;
}

// radio group stuff
// -----------------

void wxMenuItem::SetAsRadioGroupStart()
{
    m_isRadioGroupStart = true;
}

void wxMenuItem::SetRadioGroupStart(int start)
{
    wxASSERT_MSG( !m_isRadioGroupStart,
                  wxT("should only be called for the next radio items") );

    m_radioGroup.start = start;
}

void wxMenuItem::SetRadioGroupEnd(int end)
{
    wxASSERT_MSG( m_isRadioGroupStart,
                  wxT("should only be called for the first radio item") );

    m_radioGroup.end = end;
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
                                wxItemKind kind,
                                wxMenu *subMenu)
{
    return new wxMenuItem(parentMenu, id, name, help, kind, subMenu);
}
