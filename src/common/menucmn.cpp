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
WX_DEFINE_LIST(wxMenuItemList);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxMenuItem
// ----------------------------------------------------------------------------

wxMenuItemBase::~wxMenuItemBase()
{
    delete m_subMenu;
}

#if wxUSE_ACCEL

void wxMenuItemBase::SetAccel(wxAcceleratorEntry *accel)
{
    wxString text = m_text.BeforeFirst(wxT('\t'));
    if ( accel )
    {
        text += wxT('\t');

        int flags = accel->GetFlags();
        if ( flags & wxACCEL_ALT )
            text += wxT("Alt-");
        if ( flags & wxACCEL_CTRL )
            text += wxT("Ctrl-");
        if ( flags & wxACCEL_SHIFT )
            text += wxT("Shift-");

        int code = accel->GetKeyCode();
        switch ( code )
        {
            case WXK_F1:
            case WXK_F2:
            case WXK_F3:
            case WXK_F4:
            case WXK_F5:
            case WXK_F6:
            case WXK_F7:
            case WXK_F8:
            case WXK_F9:
            case WXK_F10:
            case WXK_F11:
            case WXK_F12:
                text << wxT('F') << code - WXK_F1 + 1;
                break;

            // if there are any other keys wxGetAccelFromString() may return,
            // we should process them here

            default:
                if ( wxIsalnum(code) )
                {
                    text << (wxChar)code;

                    break;
                }

                wxFAIL_MSG( wxT("unknown keyboard accel") );
        }
    }

    SetText(text);
}

#endif // wxUSE_ACCEL

// ----------------------------------------------------------------------------
// wxMenu ctor and dtor
// ----------------------------------------------------------------------------

void wxMenuBase::Init(long style)
{
    m_items.DeleteContents(TRUE);

    m_menuBar = (wxMenuBar *)NULL;
    m_menuParent = (wxMenu *)NULL;

    m_invokingWindow = (wxWindow *)NULL;
    m_style = style;
    m_clientData = (void *)NULL;
    m_eventHandler = this;
}

wxMenuBase::~wxMenuBase()
{
    // nothing to do, wxMenuItemList dtor will delete the menu items
}

// ----------------------------------------------------------------------------
// wxMenu item adding/removing
// ----------------------------------------------------------------------------

bool wxMenuBase::DoAppend(wxMenuItem *item)
{
    wxCHECK_MSG( item, FALSE, wxT("invalid item in wxMenu::Append()") );

    m_items.Append(item);

    return TRUE;
}

bool wxMenuBase::Insert(size_t pos, wxMenuItem *item)
{
    wxCHECK_MSG( item, FALSE, wxT("invalid item in wxMenu::Insert") );
    wxCHECK_MSG( pos < GetMenuItemCount(), FALSE,
                 wxT("invalid index in wxMenu::Insert") );

    return DoInsert(pos, item);
}

bool wxMenuBase::DoInsert(size_t pos, wxMenuItem *item)
{
    wxCHECK_MSG( item, FALSE, wxT("invalid item in wxMenu::Insert()") );

    wxMenuItemList::Node *node = m_items.Item(pos);
    wxCHECK_MSG( node, FALSE, wxT("invalid index in wxMenu::Insert()") );

    m_items.Insert(node, item);

    return TRUE;
}

wxMenuItem *wxMenuBase::Remove(wxMenuItem *item)
{
    wxCHECK_MSG( item, NULL, wxT("invalid item in wxMenu::Remove") );

    return DoRemove(item);
}

wxMenuItem *wxMenuBase::DoRemove(wxMenuItem *item)
{
    wxMenuItemList::Node *node = m_items.Find(item);

    // if we get here, the item is valid or one of Remove() functions is broken
    wxCHECK_MSG( node, NULL, wxT("bug in wxMenu::Remove logic") );

    // we detach the item, but we do delete the list node (i.e. don't call
    // DetachNode() here!)
    node->SetData((wxMenuItem *)NULL);  // to prevent it from deleting the item
    m_items.DeleteNode(node);

    // item isn't attached to anything any more
    wxMenu *submenu = item->GetSubMenu();
    if ( submenu )
    {
        submenu->SetParent((wxMenu *)NULL);
    }

    return item;
}

bool wxMenuBase::Delete(wxMenuItem *item)
{
    wxCHECK_MSG( item, NULL, wxT("invalid item in wxMenu::Delete") );

    return DoDelete(item);
}

bool wxMenuBase::DoDelete(wxMenuItem *item)
{
    wxMenuItem *item2 = DoRemove(item);
    wxCHECK_MSG( item2, FALSE, wxT("failed to delete menu item") );

    // don't delete the submenu
    item2->SetSubMenu((wxMenu *)NULL);

    delete item2;

    return TRUE;
}

bool wxMenuBase::Destroy(wxMenuItem *item)
{
    wxCHECK_MSG( item, NULL, wxT("invalid item in wxMenu::Destroy") );

    return DoDestroy(item);
}

bool wxMenuBase::DoDestroy(wxMenuItem *item)
{
    wxMenuItem *item2 = DoRemove(item);
    wxCHECK_MSG( item2, FALSE, wxT("failed to delete menu item") );

    delete item2;

    return TRUE;
}

// ----------------------------------------------------------------------------
// wxMenu searching for items
// ----------------------------------------------------------------------------

// Finds the item id matching the given string, -1 if not found.
int wxMenuBase::FindItem(const wxString& text) const
{
    wxString label = wxMenuItem(NULL, wxID_SEPARATOR, text).GetLabel();
    for ( wxMenuItemList::Node *node = m_items.GetFirst();
          node;
          node = node->GetNext() )
    {
        wxMenuItem *item = node->GetData();
        if ( item->IsSubMenu() )
        {
            int rc = item->GetSubMenu()->FindItem(label);
            if ( rc != wxNOT_FOUND )
                return rc;
        }
        else if ( !item->IsSeparator() )
        {
            if ( item->GetLabel() == label )
                return item->GetId();
        }
    }

    return wxNOT_FOUND;
}

// recursive search for item by id
wxMenuItem *wxMenuBase::FindItem(int itemId, wxMenu **itemMenu) const
{
    if ( itemMenu )
        *itemMenu = NULL;

    wxMenuItem *item = NULL;
    for ( wxMenuItemList::Node *node = m_items.GetFirst();
          node && !item;
          node = node->GetNext() )
    {
        item = node->GetData();

        if ( item->GetId() == itemId )
        {
            if ( itemMenu )
                *itemMenu = (wxMenu *)this;
        }
        else if ( item->IsSubMenu() )
        {
            item = item->GetSubMenu()->FindItem(itemId, itemMenu);
        }
        else
        {
            // don't exit the loop
            item = NULL;
        }
    }

    return item;
}

// non recursive search
wxMenuItem *wxMenuBase::FindChildItem(int id, size_t *ppos) const
{
    wxMenuItem *item = (wxMenuItem *)NULL;
    wxMenuItemList::Node *node = GetMenuItems().GetFirst();

    size_t pos;
    for ( pos = 0; node; pos++ )
    {
        item = node->GetData();
        if ( item->GetId() == id )
            break;

        node = node->GetNext();
    }

    if ( ppos )
    {
        *ppos = item ? pos : (size_t)wxNOT_FOUND;
    }

    return item;
}

// ----------------------------------------------------------------------------
// wxMenu helpers
// ----------------------------------------------------------------------------

// Update a menu and all submenus recursively. source is the object that has
// the update event handlers defined for it. If NULL, the menu or associated
// window will be used.
void wxMenuBase::UpdateUI(wxEvtHandler* source)
{
    if ( !source && GetInvokingWindow() )
        source = GetInvokingWindow()->GetEventHandler();
    if ( !source )
        source = GetEventHandler();
    if ( !source )
        source = this;

    wxMenuItemList::Node* node = GetMenuItems().GetFirst();
    while ( node )
    {
        wxMenuItem* item = node->GetData();
        if ( !item->IsSeparator() )
        {
            wxWindowID id = item->GetId();
            wxUpdateUIEvent event(id);
            event.SetEventObject( source );

            if ( source->ProcessEvent(event) )
            {
                // if anything changed, update the chanegd attribute
                if (event.GetSetText())
                    SetLabel(id, event.GetText());
                if (event.GetSetChecked())
                    Check(id, event.GetChecked());
                if (event.GetSetEnabled())
                    Enable(id, event.GetEnabled());
            }

            // recurse to the submenus
            if ( item->GetSubMenu() )
                item->GetSubMenu()->UpdateUI(source);
        }
        //else: item is a separator (which don't process update UI events)

        node = node->GetNext();
    }
}

// ----------------------------------------------------------------------------
// wxMenu functions forwarded to wxMenuItem
// ----------------------------------------------------------------------------

void wxMenuBase::Enable( int id, bool enable )
{
    wxMenuItem *item = FindItem(id);

    wxCHECK_RET( item, wxT("wxMenu::Enable: no such item") );

    item->Enable(enable);
}

bool wxMenuBase::IsEnabled( int id ) const
{
    wxMenuItem *item = FindItem(id);

    wxCHECK_MSG( item, FALSE, wxT("wxMenu::IsEnabled: no such item") );

    return item->IsEnabled();
}

void wxMenuBase::Check( int id, bool enable )
{
    wxMenuItem *item = FindItem(id);

    wxCHECK_RET( item, wxT("wxMenu::Check: no such item") );

    item->Check(enable);
}

bool wxMenuBase::IsChecked( int id ) const
{
    wxMenuItem *item = FindItem(id);

    wxCHECK_MSG( item, FALSE, wxT("wxMenu::IsChecked: no such item") );

    return item->IsChecked();
}

void wxMenuBase::SetLabel( int id, const wxString &label )
{
    wxMenuItem *item = FindItem(id);

    wxCHECK_RET( item, wxT("wxMenu::SetLabel: no such item") );

    item->SetText(label);
}

wxString wxMenuBase::GetLabel( int id ) const
{
    wxMenuItem *item = FindItem(id);

    wxCHECK_MSG( item, wxT(""), wxT("wxMenu::GetLabel: no such item") );

    return item->GetText();
}

void wxMenuBase::SetHelpString( int id, const wxString& helpString )
{
    wxMenuItem *item = FindItem(id);

    wxCHECK_RET( item, wxT("wxMenu::SetHelpString: no such item") );

    item->SetHelp( helpString );
}

wxString wxMenuBase::GetHelpString( int id ) const
{
    wxMenuItem *item = FindItem(id);

    wxCHECK_MSG( item, wxT(""), wxT("wxMenu::GetHelpString: no such item") );

    return item->GetHelp();
}

// ----------------------------------------------------------------------------
// wxMenuBarBase ctor and dtor
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

