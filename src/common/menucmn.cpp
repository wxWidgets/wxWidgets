///////////////////////////////////////////////////////////////////////////////
// Name:        common/menucmn.cpp
// Purpose:     wxMenu and wxMenuBar methods common to all ports
// Author:      Vadim Zeitlin
// Modified by:
// Created:     26.10.99
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "menubase.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/menu.h"
#endif

// ----------------------------------------------------------------------------
// template lists
// ----------------------------------------------------------------------------

#include "wx/listimpl.cpp"
WX_DEFINE_LIST(wxMenuList);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// ctor and dtor
// ----------------------------------------------------------------------------

wxMenuBarBase::wxMenuBarBase()
{
    // we own the menus when we get them
    m_menus.DeleteContents(TRUE);
}

wxMenuBarBase::~wxMenuBarBase()
{
    // nothing to do, the list will delete the menus because of the call to
    // DeleteContents() above
}

// ----------------------------------------------------------------------------
// wxMenuBar item access: the base class versions manage m_menus list, the
// derived class should reflect the changes in the real menubar
// ----------------------------------------------------------------------------

wxMenu *wxMenuBarBase::GetMenu(size_t pos) const
{
    wxMenuList::Node *node = m_menus.Item(pos);
    wxCHECK_MSG( node, NULL, wxT("bad index in wxMenuBar::GetMenu()") );

    return node->GetData();
}

bool wxMenuBarBase::Append(wxMenu *menu, const wxString& WXUNUSED(title))
{
    wxCHECK_MSG( menu, FALSE, wxT("can't append NULL menu") );

    m_menus.Append(menu);

    return TRUE;
}

bool wxMenuBarBase::Insert(size_t pos, wxMenu *menu,
                           const wxString& WXUNUSED(title))
{
    wxCHECK_MSG( menu, FALSE, wxT("can't insert NULL menu") );

    wxMenuList::Node *node = m_menus.Item(pos);
    wxCHECK_MSG( node, FALSE, wxT("bad index in wxMenuBar::Insert()") );

    m_menus.Insert(node, menu);

    return TRUE;
}

wxMenu *wxMenuBarBase::Replace(size_t pos, wxMenu *menu,
                               const wxString& WXUNUSED(title))
{
    wxCHECK_MSG( menu, NULL, wxT("can't insert NULL menu") );

    wxMenuList::Node *node = m_menus.Item(pos);
    wxCHECK_MSG( node, NULL, wxT("bad index in wxMenuBar::Replace()") );

    wxMenu *menuOld = node->GetData();
    node->SetData(menu);

    return menuOld;
}

wxMenu *wxMenuBarBase::Remove(size_t pos)
{
    wxMenuList::Node *node = m_menus.Item(pos);
    wxCHECK_MSG( node, NULL, wxT("bad index in wxMenuBar::Remove()") );

    node = m_menus.DetachNode(node);
    wxCHECK( node, NULL );  // unexpected
    wxMenu *menu = node->GetData();

    delete node;

    return menu;
}

// ---------------------------------------------------------------------------
// wxMenuBar functions forwarded to wxMenuItem
// ---------------------------------------------------------------------------

void wxMenuBarBase::Enable(int id, bool enable)
{
    wxMenuItem *item = FindItem(id);

    wxCHECK_RET( item, wxT("attempt to enable an item which doesn't exist") );

    item->Enable(enable);
}

void wxMenuBarBase::Check(int id, bool check)
{
    wxMenuItem *item = FindItem(id);

    wxCHECK_RET( item, wxT("attempt to check an item which doesn't exist") );
    wxCHECK_RET( item->IsCheckable(), wxT("attempt to check an uncheckable item") );

    item->Check(check);
}

bool wxMenuBarBase::IsChecked(int id) const
{
    wxMenuItem *item = FindItem(id);

    wxCHECK_MSG( item, FALSE, wxT("wxMenuBar::IsChecked(): no such item") );

    return item->IsChecked();
}

bool wxMenuBarBase::IsEnabled(int id) const
{
    wxMenuItem *item = FindItem(id);

    wxCHECK_MSG( item, FALSE, wxT("wxMenuBar::IsEnabled(): no such item") );

    return item->IsEnabled();
}

void wxMenuBarBase::SetLabel(int id, const wxString& label)
{
    wxMenuItem *item = FindItem(id);

    wxCHECK_RET( item, wxT("wxMenuBar::SetLabel(): no such item") );

    item->SetText(label);
}

wxString wxMenuBarBase::GetLabel(int id) const
{
    wxMenuItem *item = FindItem(id);

    wxCHECK_MSG( item, wxEmptyString,
                 wxT("wxMenuBar::GetLabel(): no such item") );

    return item->GetText();
}

void wxMenuBarBase::SetHelpString(int id, const wxString& helpString)
{
    wxMenuItem *item = FindItem(id);

    wxCHECK_RET( item, wxT("wxMenuBar::SetHelpString(): no such item") );

    item->SetHelp(helpString);
}

wxString wxMenuBarBase::GetHelpString(int id) const
{
    wxMenuItem *item = FindItem(id);

    wxCHECK_MSG( item, wxEmptyString,
                 wxT("wxMenuBar::GetHelpString(): no such item") );

    return item->GetHelp();
}

