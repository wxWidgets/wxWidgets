/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/menu.cpp
// Purpose:     wxMenu, wxMenuBar, wxMenuItem
// Author:      Julian Smart
// Modified by: Vadim Zeitlin
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_MENUS

#include "wx/menu.h"

#ifndef WX_PRECOMP
    #include "wx/frame.h"
    #include "wx/utils.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/image.h"
#endif

#if wxUSE_OWNER_DRAWN
    #include "wx/ownerdrw.h"
#endif

#include "wx/scopedarray.h"
#include "wx/msw/private.h"
#include "wx/msw/wrapcctl.h" // include <commctrl.h> "properly"
#include "wx/private/menuradio.h" // for wxMenuRadioItemsData

// other standard headers
#include <string.h>

#include "wx/dynlib.h"

// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// the (popup) menu title has this special id
static const int idMenuTitle = wxID_NONE;

// ----------------------------------------------------------------------------
// private helper classes and functions
// ----------------------------------------------------------------------------

namespace
{

// make the given menu item default
void SetDefaultMenuItem(HMENU hmenu, UINT id)
{
    WinStruct<MENUITEMINFO> mii;
    mii.fMask = MIIM_STATE;
    mii.fState = MFS_DEFAULT;

    if ( !::SetMenuItemInfo(hmenu, id, FALSE, &mii) )
    {
        wxLogLastError(wxT("SetMenuItemInfo"));
    }
}

// make the given menu item owner-drawn
void SetOwnerDrawnMenuItem(HMENU hmenu,
                           UINT id,
                           ULONG_PTR data,
                           BOOL byPositon = FALSE)
{
    WinStruct<MENUITEMINFO> mii;
    mii.fMask = MIIM_FTYPE | MIIM_DATA;
    mii.fType = MFT_OWNERDRAW;
    mii.dwItemData = data;

    if ( reinterpret_cast<wxMenuItem*>(data)->IsSeparator() )
        mii.fType |= MFT_SEPARATOR;

    if ( !::SetMenuItemInfo(hmenu, id, byPositon, &mii) )
    {
        wxLogLastError(wxT("SetMenuItemInfo"));
    }
}

} // anonymous namespace

// ============================================================================
// implementation
// ============================================================================

// ---------------------------------------------------------------------------
// wxMenu construction, adding and removing menu items
// ---------------------------------------------------------------------------

// Construct a menu with optional title (then use append)
void wxMenu::InitNoCreate()
{
    m_radioData = NULL;
    m_doBreak = false;

#if wxUSE_OWNER_DRAWN
    m_ownerDrawn = false;
    m_maxBitmapWidth = 0;
    m_maxAccelWidth = -1;
#endif // wxUSE_OWNER_DRAWN
}

void wxMenu::Init()
{
    InitNoCreate();

    // create the menu
    m_hMenu = (WXHMENU)CreatePopupMenu();
    if ( !m_hMenu )
    {
        wxLogLastError(wxT("CreatePopupMenu"));
    }

    // if we have a title, insert it in the beginning of the menu
    if ( !m_title.empty() )
    {
        const wxString title = m_title;
        m_title.clear(); // so that SetTitle() knows there was no title before
        SetTitle(title);
    }
}

wxMenu::wxMenu(WXHMENU hMenu)
{
    InitNoCreate();

    m_hMenu = hMenu;

    // Ensure that our internal idea of how many items we have corresponds to
    // the real number of items in the menu.
    //
    // We could also retrieve the real labels of the items here but it doesn't
    // seem to be worth the trouble.
    const int numExistingItems = ::GetMenuItemCount(m_hMenu);
    for ( int n = 0; n < numExistingItems; n++ )
    {
        wxMenuBase::DoAppend(wxMenuItem::New(this, wxID_SEPARATOR));
    }
}

// The wxWindow destructor will take care of deleting the submenus.
wxMenu::~wxMenu()
{
    // we should free Windows resources only if Windows doesn't do it for us
    // which happens if we're attached to a menubar or a submenu of another
    // menu
    if ( m_hMenu && !IsAttached() && !GetParent() )
    {
        if ( !::DestroyMenu(GetHmenu()) )
        {
            wxLogLastError(wxT("DestroyMenu"));
        }
    }

#if wxUSE_ACCEL
    // delete accels
    WX_CLEAR_ARRAY(m_accels);
#endif // wxUSE_ACCEL

    delete m_radioData;
}

void wxMenu::Break()
{
    // this will take effect during the next call to Append()
    m_doBreak = true;
}

#if wxUSE_ACCEL

int wxMenu::FindAccel(int id) const
{
    size_t n, count = m_accels.GetCount();
    for ( n = 0; n < count; n++ )
    {
        if ( m_accels[n]->m_command == id )
            return n;
    }

    return wxNOT_FOUND;
}

void wxMenu::UpdateAccel(wxMenuItem *item)
{
    if ( item->IsSubMenu() )
    {
        wxMenu *submenu = item->GetSubMenu();
        wxMenuItemList::compatibility_iterator node = submenu->GetMenuItems().GetFirst();
        while ( node )
        {
            UpdateAccel(node->GetData());

            node = node->GetNext();
        }
    }
    else if ( !item->IsSeparator() )
    {
        // recurse upwards: we should only modify m_accels of the top level
        // menus, not of the submenus as wxMenuBar doesn't look at them
        // (alternative and arguable cleaner solution would be to recurse
        // downwards in GetAccelCount() and CopyAccels())
        if ( GetParent() )
        {
            GetParent()->UpdateAccel(item);
            return;
        }

        // remove old accels
        int n = wxNOT_FOUND;
        while ( (n = FindAccel(item->GetId())) != wxNOT_FOUND )
        {
            delete m_accels[n];
            m_accels.RemoveAt(n);
        }

        // find the (new) accel for this item and add it if any
        wxAcceleratorEntry *accel = wxAcceleratorEntry::Create(item->GetItemLabel());
        if ( accel )
        {
            accel->m_command = item->GetId();
            m_accels.Add(accel);
        }

        // add extra accels
        const wxVector<wxAcceleratorEntry>& extraAccelsVector = item->GetExtraAccels();
        const int extraAccelsSize = extraAccelsVector.size();
        for (int i = 0; i < extraAccelsSize; ++i)
        {
            wxAcceleratorEntry *extraAccel = new wxAcceleratorEntry(extraAccelsVector[i]);
            extraAccel->m_command = item->GetId();
            m_accels.Add(extraAccel);
        }

        if ( IsAttached() )
        {
            GetMenuBar()->RebuildAccelTable();
        }

#if wxUSE_OWNER_DRAWN
        ResetMaxAccelWidth();
#endif
    }
    //else: it is a separator, they can't have accels, nothing to do
}

void wxMenu::RemoveAccel(wxMenuItem *item)
{
    // recurse upwards: we should only modify m_accels of the top level
    // menus, not of the submenus as wxMenuBar doesn't look at them
    // (alternative and arguable cleaner solution would be to recurse
    // downwards in GetAccelCount() and CopyAccels())
    if ( GetParent() )
    {
        GetParent()->RemoveAccel(item);
        return;
    }

    // remove the corresponding accels from the accel table
    int n = wxNOT_FOUND;
    bool accels_found = false;
    while ( (n = FindAccel(item->GetId())) != wxNOT_FOUND )
    {
        delete m_accels[n];
        m_accels.RemoveAt(n);
        accels_found = true;
    }

#if wxUSE_OWNER_DRAWN
    if ( accels_found )
    {
        ResetMaxAccelWidth();
    }
#endif
}

#endif // wxUSE_ACCEL

namespace
{

} // anonymous namespace

bool wxMenu::MSWGetRadioGroupRange(int pos, int *start, int *end) const
{
    return m_radioData && m_radioData->GetGroupRange(pos, start, end);
}

void wxMenu::SetupBitmaps()
{
    for ( wxMenuItemList::compatibility_iterator node = m_items.GetFirst();
          node;
          node = node->GetNext() )
    {
        wxMenuItem *item = node->GetData();
        if ( item->IsSubMenu() )
        {
            item->GetSubMenu()->SetupBitmaps();
        }

        if ( !item->IsSeparator() )
        {
            item->SetupBitmaps();
        }
    }
}

// append a new item or submenu to the menu
bool wxMenu::DoInsertOrAppend(wxMenuItem *pItem, size_t pos)
{
#if wxUSE_ACCEL
    UpdateAccel(pItem);
#endif // wxUSE_ACCEL

    // we should support disabling the item even prior to adding it to the menu
    UINT flags = pItem->IsEnabled() ? MF_ENABLED : MF_GRAYED;

    // if "Break" has just been called, insert a menu break before this item
    // (and don't forget to reset the flag)
    if ( m_doBreak ) {
        flags |= MF_MENUBREAK;
        m_doBreak = false;
    }

    if ( pItem->IsSeparator() ) {
        flags |= MF_SEPARATOR;
    }

    // id is the numeric id for normal menu items and HMENU for submenus as
    // required by ::AppendMenu() API
    UINT_PTR id;
    wxMenu *submenu = pItem->GetSubMenu();
    if ( submenu != NULL ) {
        wxASSERT_MSG( submenu->GetHMenu(), wxT("invalid submenu") );

        submenu->SetParent(this);

        id = (UINT_PTR)submenu->GetHMenu();

        flags |= MF_POPUP;
    }
    else {
        id = pItem->GetMSWId();
    }


    // prepare to insert the item in the menu
    wxString itemText = pItem->GetItemLabel();
    LPCTSTR pData = NULL;
    if ( pos == (size_t)-1 )
    {
        // append at the end (note that the item is already appended to
        // internal data structures)
        pos = GetMenuItemCount() - 1;
    }

    // Update radio groups data if we're inserting a new menu item.
    // Inserting radio and non-radio item has a different impact
    // on radio groups so we have to handle each case separately.
    // (Inserting a radio item in the middle of existing group extends
    // the group, but inserting non-radio item breaks it into two subgroups.)
    //
    bool checkInitially = false;
    if ( pItem->IsRadio() )
    {
        if ( !m_radioData )
            m_radioData = new wxMenuRadioItemsData;

        if ( m_radioData->UpdateOnInsertRadio(pos) )
            checkInitially = true;
    }
    else if ( m_radioData )
    {
        if ( m_radioData->UpdateOnInsertNonRadio(pos) )
        {
            // One of the existing groups has been split into two subgroups.
            wxFAIL_MSG(wxS("Inserting non-radio item inside a radio group?"));
        }
    }

    // Also handle the case of check menu items that had been checked before
    // being attached to the menu: we don't need to actually call Check() on
    // them, so we don't use checkInitially in this case, but we do need to
    // make them checked at Windows level too. Notice that we shouldn't ask
    // Windows for the checked state here, as wxMenuItem::IsChecked() does, as
    // the item is not attached yet, so explicitly call the base class version.
    if ( pItem->IsCheck() && pItem->wxMenuItemBase::IsChecked() )
        flags |= MF_CHECKED;

    // adjust position to account for the title of a popup menu, if any
    if ( !GetMenuBar() && !m_title.empty() )
        pos += 2; // for the title itself and its separator

    BOOL ok = false;

#if wxUSE_OWNER_DRAWN
    // Under older systems mixing owner-drawn and non-owner-drawn items results
    // in inconsistent margins, so we force this one to be owner-drawn if any
    // other items already are.
    if ( m_ownerDrawn )
        pItem->SetOwnerDrawn(true);

    // check if we have something more than a simple text item
    bool makeItemOwnerDrawn = false;
#endif // wxUSE_OWNER_DRAWN

    if (
#if wxUSE_OWNER_DRAWN
            !pItem->IsOwnerDrawn() &&
#endif
        !pItem->IsSeparator() )
            {
                WinStruct<MENUITEMINFO> mii;
                mii.fMask = MIIM_STRING | MIIM_DATA;

                mii.cch = itemText.length();
                mii.dwTypeData = wxMSW_CONV_LPTSTR(itemText);

                if ( flags & MF_POPUP )
                {
                    mii.fMask |= MIIM_SUBMENU;
                    mii.hSubMenu = GetHmenuOf(pItem->GetSubMenu());
                }
                else
                {
                    mii.fMask |= MIIM_ID;
                    mii.wID = id;
                }

                if ( flags & MF_GRAYED )
                {
                    mii.fMask |= MIIM_STATE;
                    mii.fState = MFS_GRAYED;
                }

                if ( flags & MF_CHECKED )
                {
                    mii.fMask |= MIIM_STATE;
                    mii.fState = MFS_CHECKED;
                }

                if ( flags & MF_MENUBREAK )
                {
                    mii.fMask |= MIIM_FTYPE;
                    mii.fType = MFT_MENUBREAK;
                }

                mii.dwItemData = reinterpret_cast<ULONG_PTR>(pItem);

                ok = ::InsertMenuItem(GetHmenu(), pos, TRUE /* by pos */, &mii);
                if ( !ok )
                {
                    wxLogLastError(wxT("InsertMenuItem()"));
#if wxUSE_OWNER_DRAWN
            // In case of failure switch new item to the owner-drawn mode.
            makeItemOwnerDrawn = true;
#endif
                }
                else // InsertMenuItem() ok
                {
                    // we need to remove the extra indent which is reserved for
                    // the checkboxes by default as it looks ugly unless check
                    // boxes are used together with bitmaps and this is not the
                    // case in wx API
                    WinStruct<MENUINFO> mi;
                    mi.fMask = MIM_STYLE;
                    mi.dwStyle = MNS_CHECKORBMP;
                    if ( !::SetMenuInfo(GetHmenu(), &mi) )
                    {
                        wxLogLastError(wxT("SetMenuInfo(MNS_NOCHECK)"));
                    }
                }
        }

#if wxUSE_OWNER_DRAWN
    if ( pItem->IsOwnerDrawn() || makeItemOwnerDrawn )
        {
            // item draws itself, pass pointer to it in data parameter
            flags |= MF_OWNERDRAW;
            pData = (LPCTSTR)pItem;

            bool updateAllMargins = false;

            // get size of bitmap always return valid value (0 for invalid bitmap),
            // so we don't needed check if bitmap is valid ;)
            int uncheckedW = pItem->GetBitmap(false).GetWidth();
            int checkedW   = pItem->GetBitmap(true).GetWidth();

            if ( m_maxBitmapWidth < uncheckedW )
            {
                m_maxBitmapWidth = uncheckedW;
                updateAllMargins = true;
            }

            if ( m_maxBitmapWidth < checkedW )
            {
                m_maxBitmapWidth = checkedW;
                updateAllMargins = true;
            }

            // make other item ownerdrawn and update margin width for equals alignment
            if ( !m_ownerDrawn || updateAllMargins )
            {
                // we must use position in SetOwnerDrawnMenuItem because
                // all separators have the same id
                int position = 0;
                wxMenuItemList::compatibility_iterator node = GetMenuItems().GetFirst();
                while (node)
                {
                    wxMenuItem* item = node->GetData();

                    // Current item is already added to the list of items
                    // but is not yet physically attached to the menu
                    // so we have to skip setting it as an owner drawn.
                    // It will be done later on when the item will be created.
                    if ( !item->IsOwnerDrawn() && item != pItem )
                    {
                        item->SetOwnerDrawn(true);
                        SetOwnerDrawnMenuItem(GetHmenu(), position,
                                              reinterpret_cast<ULONG_PTR>(item), TRUE);
                    }

                    item->SetMarginWidth(m_maxBitmapWidth);

                    node = node->GetNext();
                    // Current item is already added to the list of items
                    // but is not yet physically attached to the menu
                    // so it cannot be counted while determining position
                    // in the menu.
                    if ( item != pItem )
                        position++;
                }

                // set menu as ownerdrawn
                m_ownerDrawn = true;

                ResetMaxAccelWidth();
            }
            // only update our margin for equals alignment to other item
            else if ( !updateAllMargins )
            {
                pItem->SetMarginWidth(m_maxBitmapWidth);
            }
        }
#endif // wxUSE_OWNER_DRAWN

    // item might have already been inserted by InsertMenuItem() above
    if ( !ok )
    {
        if ( !::InsertMenu(GetHmenu(), pos, flags | MF_BYPOSITION, id, pData) )
        {
            wxLogLastError(wxT("InsertMenu[Item]()"));

            return false;
        }

#if wxUSE_OWNER_DRAWN
        if ( makeItemOwnerDrawn )
        {
            pItem->SetOwnerDrawn(true);
            SetOwnerDrawnMenuItem(GetHmenu(), pos,
                                  reinterpret_cast<ULONG_PTR>(pItem), TRUE);
        }
#endif
    }


    // Check the item if it should be initially checked.
    if ( checkInitially )
        pItem->Check(true);

    // if we just appended the title, highlight it
    if ( id == (UINT_PTR)idMenuTitle )
    {
        // visually select the menu title
        SetDefaultMenuItem(GetHmenu(), id);
    }

    // if we're already attached to the menubar, we must update it
    if ( IsAttached() && GetMenuBar()->IsAttached() )
    {
        GetMenuBar()->Refresh();
    }

    return true;
}

wxMenuItem* wxMenu::DoAppend(wxMenuItem *item)
{
    return wxMenuBase::DoAppend(item) && DoInsertOrAppend(item) ? item : NULL;
}

wxMenuItem* wxMenu::DoInsert(size_t pos, wxMenuItem *item)
{
    if (wxMenuBase::DoInsert(pos, item) && DoInsertOrAppend(item, pos))
        return item;
    else
        return NULL;
}

wxMenuItem *wxMenu::DoRemove(wxMenuItem *item)
{
    // we need to find the item's position in the child list
    size_t pos;
    wxMenuItemList::compatibility_iterator node = GetMenuItems().GetFirst();
    for ( pos = 0; node; pos++ )
    {
        if ( node->GetData() == item )
            break;

        node = node->GetNext();
    }

#if wxUSE_ACCEL
    RemoveAccel(item);
#endif // wxUSE_ACCEL

    // Update indices of radio groups.
    if ( m_radioData )
    {
        if ( m_radioData->UpdateOnRemoveItem(pos) )
        {
            wxASSERT_MSG( item->IsRadio(),
                          wxT("Removing non radio button from radio group?") );
        }
        //else: item being removed is not in a radio group
    }

    // remove the item from the menu
    if ( !::RemoveMenu(GetHmenu(), (UINT)pos, MF_BYPOSITION) )
    {
        wxLogLastError(wxT("RemoveMenu"));
    }

    if ( IsAttached() && GetMenuBar()->IsAttached() )
    {
        // otherwise, the change won't be visible
        GetMenuBar()->Refresh();
    }

    // and from internal data structures
    return wxMenuBase::DoRemove(item);
}

// ---------------------------------------------------------------------------
// accelerator helpers
// ---------------------------------------------------------------------------

#if wxUSE_ACCEL

// create the wxAcceleratorEntries for our accels and put them into the provided
// array - return the number of accels we have
size_t wxMenu::CopyAccels(wxAcceleratorEntry *accels) const
{
    size_t count = GetAccelCount();
    for ( size_t n = 0; n < count; n++ )
    {
        *accels++ = *m_accels[n];
    }

    return count;
}

wxAcceleratorTable *wxMenu::CreateAccelTable() const
{
    const size_t count = m_accels.size();
    wxScopedArray<wxAcceleratorEntry> accels(count);
    CopyAccels(accels.get());

    return new wxAcceleratorTable(count, accels.get());
}

#endif // wxUSE_ACCEL

// ---------------------------------------------------------------------------
// ownerdrawn helpers
// ---------------------------------------------------------------------------

#if wxUSE_OWNER_DRAWN

void wxMenu::CalculateMaxAccelWidth()
{
    wxASSERT_MSG( m_maxAccelWidth == -1, wxT("it's really needed?") );

    wxMenuItemList::compatibility_iterator node = GetMenuItems().GetFirst();
    while (node)
    {
        wxMenuItem* item = node->GetData();

        if ( item->IsOwnerDrawn() )
        {
            int width = item->MeasureAccelWidth();
            if (width > m_maxAccelWidth )
                m_maxAccelWidth = width;
        }

        node = node->GetNext();
    }
}

#endif // wxUSE_OWNER_DRAWN

// ---------------------------------------------------------------------------
// set wxMenu title
// ---------------------------------------------------------------------------

void wxMenu::SetTitle(const wxString& label)
{
    bool hasNoTitle = m_title.empty();
    m_title = label;

    HMENU hMenu = GetHmenu();

    if ( hasNoTitle )
    {
        if ( !label.empty() )
        {
            if ( !::InsertMenu(hMenu, 0u, MF_BYPOSITION | MF_STRING,
                               (UINT_PTR)idMenuTitle, m_title.t_str()) ||
                 !::InsertMenu(hMenu, 1u, MF_BYPOSITION, (unsigned)-1, NULL) )
            {
                wxLogLastError(wxT("InsertMenu"));
            }
        }
    }
    else
    {
        if ( label.empty() )
        {
            // remove the title and the separator after it
            if ( !RemoveMenu(hMenu, 0, MF_BYPOSITION) ||
                 !RemoveMenu(hMenu, 0, MF_BYPOSITION) )
            {
                wxLogLastError(wxT("RemoveMenu"));
            }
        }
        else
        {
            // modify the title
            if ( !ModifyMenu(hMenu, 0u,
                             MF_BYPOSITION | MF_STRING,
                             (UINT_PTR)idMenuTitle, m_title.t_str()) )
            {
                wxLogLastError(wxT("ModifyMenu"));
            }
        }
    }

    // put the title string in bold face
    if ( !m_title.empty() )
    {
        SetDefaultMenuItem(GetHmenu(), (UINT)idMenuTitle);
    }
}

// ---------------------------------------------------------------------------
// event processing
// ---------------------------------------------------------------------------

bool wxMenu::MSWCommand(WXUINT WXUNUSED(param), WXWORD id_)
{
    const int id = (signed short)id_;

    // ignore commands from the menu title
    if ( id != idMenuTitle )
    {
        // Default value for uncheckable items.
        int checked = -1;

        // update the check item when it's clicked
        wxMenuItem * const item = FindItem(id);
        if ( item )
        {
            if ( item->IsRadio() && item->IsChecked() )
                return true;

            if ( item->IsCheckable() )
            {
                item->Toggle();

                // Get the status of the menu item: note that it has been just changed
                // by Toggle() above so here we already get the new state of the item.
                //
                // Also notice that we must pass unsigned id_ and not sign-extended id
                // to ::GetMenuState() as this is what it expects.
                UINT menuState = ::GetMenuState(GetHmenu(), id_, MF_BYCOMMAND);
                checked = (menuState & MF_CHECKED) != 0;
            }

            item->GetMenu()->SendEvent(id, checked);
        }
        else
        {
            SendEvent(id, checked);
        }
    }

    return true;
}

// get the menu with given handle (recursively)
wxMenu* wxMenu::MSWGetMenu(WXHMENU hMenu)
{
    // check self
    if ( GetHMenu() == hMenu )
        return this;

    // recursively query submenus
    for ( size_t n = 0 ; n < GetMenuItemCount(); ++n )
    {
        wxMenuItem* item = FindItemByPosition(n);
        wxMenu* submenu = item->GetSubMenu();
        if ( submenu )
        {
            submenu = submenu->MSWGetMenu(hMenu);
            if (submenu)
                return submenu;
        }
    }

    // unknown hMenu
    return NULL;
}

// ---------------------------------------------------------------------------
// Menu Bar
// ---------------------------------------------------------------------------

void wxMenuBar::Init()
{
    m_eventHandler = this;
    m_hMenu = 0;
}

wxMenuBar::wxMenuBar()
{
    Init();
}

wxMenuBar::wxMenuBar( long WXUNUSED(style) )
{
    Init();
}

wxMenuBar::wxMenuBar(size_t count, wxMenu *menus[], const wxString titles[], long WXUNUSED(style))
{
    Init();

    for ( size_t i = 0; i < count; i++ )
    {
        // We just want to store the menu title in the menu itself, not to
        // show it as a dummy item in the menu itself as we do with the popup
        // menu titles in overridden wxMenu::SetTitle().
        menus[i]->wxMenuBase::SetTitle(titles[i]);
        m_menus.Append(menus[i]);

        menus[i]->Attach(this);
    }
}

wxMenuBar::~wxMenuBar()
{
    // we should free Windows resources only if Windows doesn't do it for us
    // which happens if we're attached to a frame
    if (m_hMenu && !IsAttached())
    {
        ::DestroyMenu((HMENU)m_hMenu);
        m_hMenu = (WXHMENU)NULL;
    }
}

// ---------------------------------------------------------------------------
// wxMenuBar helpers
// ---------------------------------------------------------------------------

void wxMenuBar::Refresh()
{
    if ( IsFrozen() )
        return;

    wxCHECK_RET( IsAttached(), wxT("can't refresh unattached menubar") );

    DrawMenuBar(GetHwndOf(GetFrame()));
}

WXHMENU wxMenuBar::Create()
{
    if ( m_hMenu != 0 )
        return m_hMenu;

    m_hMenu = (WXHMENU)::CreateMenu();

    if ( !m_hMenu )
    {
        wxLogLastError(wxT("CreateMenu"));
    }
    else
    {
        for ( wxMenuList::iterator it = m_menus.begin();
              it != m_menus.end();
              ++it )
        {
            if ( !::AppendMenu((HMENU)m_hMenu, MF_POPUP | MF_STRING,
                               (UINT_PTR)(*it)->GetHMenu(),
                               (*it)->GetTitle().t_str()) )
            {
                wxLogLastError(wxT("AppendMenu"));
            }
        }
    }

    return m_hMenu;
}

int wxMenuBar::MSWPositionForWxMenu(wxMenu *menu, int wxpos)
{
    wxASSERT(menu);
    wxASSERT(menu->GetHMenu());
    wxASSERT(m_hMenu);

    int totalMSWItems = GetMenuItemCount((HMENU)m_hMenu);

    int i; // For old C++ compatibility
    for(i=wxpos; i<totalMSWItems; i++)
    {
        if(GetSubMenu((HMENU)m_hMenu,i)==(HMENU)menu->GetHMenu())
            return i;
    }
    for(i=0; i<wxpos; i++)
    {
        if(GetSubMenu((HMENU)m_hMenu,i)==(HMENU)menu->GetHMenu())
            return i;
    }
    wxFAIL;
    return -1;
}

// ---------------------------------------------------------------------------
// wxMenuBar functions to work with the top level submenus
// ---------------------------------------------------------------------------

// NB: we don't support owner drawn top level items for now, if we do these
//     functions would have to be changed to use wxMenuItem as well

void wxMenuBar::EnableTop(size_t pos, bool enable)
{
    wxCHECK_RET( IsAttached(), wxT("doesn't work with unattached menubars") );
    wxCHECK_RET( pos < GetMenuCount(), wxT("invalid menu index") );

    int flag = enable ? MF_ENABLED : MF_GRAYED;

    EnableMenuItem((HMENU)m_hMenu, MSWPositionForWxMenu(GetMenu(pos),pos), MF_BYPOSITION | flag);

    Refresh();
}

bool wxMenuBar::IsEnabledTop(size_t pos) const
{
    wxCHECK_MSG( pos < GetMenuCount(), false, wxS("invalid menu index") );
    WinStruct<MENUITEMINFO> mii;
    mii.fMask = MIIM_STATE;
    if ( !::GetMenuItemInfo(GetHmenu(), pos, TRUE, &mii) )
    {
        wxLogLastError(wxS("GetMenuItemInfo(menubar)"));
    }

    return !(mii.fState & MFS_GRAYED);
}

void wxMenuBar::SetMenuLabel(size_t pos, const wxString& label)
{
    wxCHECK_RET( pos < GetMenuCount(), wxT("invalid menu index") );

    m_menus[pos]->wxMenuBase::SetTitle(label);

    if ( !IsAttached() )
    {
        return;
    }
    //else: have to modify the existing menu

    int mswpos = MSWPositionForWxMenu(GetMenu(pos),pos);

    UINT_PTR id;
    UINT flagsOld = ::GetMenuState((HMENU)m_hMenu, mswpos, MF_BYPOSITION);
    if ( flagsOld == 0xFFFFFFFF )
    {
        wxLogLastError(wxT("GetMenuState"));

        return;
    }

    if ( flagsOld & MF_POPUP )
    {
        // HIBYTE contains the number of items in the submenu in this case
        flagsOld &= 0xff;
        id = (UINT_PTR)::GetSubMenu((HMENU)m_hMenu, mswpos);
    }
    else
    {
        id = pos;
    }

    if ( ::ModifyMenu(GetHmenu(), mswpos, MF_BYPOSITION | MF_STRING | flagsOld,
                      id, label.t_str()) == (int)0xFFFFFFFF )
    {
        wxLogLastError(wxT("ModifyMenu"));
    }

    Refresh();
}

wxString wxMenuBar::GetMenuLabel(size_t pos) const
{
    wxCHECK_MSG( pos < GetMenuCount(), wxEmptyString,
                 wxT("invalid menu index in wxMenuBar::GetMenuLabel") );

    return m_menus[pos]->GetTitle();
}

// ---------------------------------------------------------------------------
// wxMenuBar construction
// ---------------------------------------------------------------------------

wxMenu *wxMenuBar::Replace(size_t pos, wxMenu *menu, const wxString& title)
{
    wxMenu *menuOld = wxMenuBarBase::Replace(pos, menu, title);
    if ( !menuOld )
        return NULL;

    menu->wxMenuBase::SetTitle(title);

    if (GetHmenu())
    {
        int mswpos = MSWPositionForWxMenu(menuOld,pos);

        // can't use ModifyMenu() because it deletes the submenu it replaces
        if ( !::RemoveMenu(GetHmenu(), (UINT)mswpos, MF_BYPOSITION) )
        {
            wxLogLastError(wxT("RemoveMenu"));
        }

        if ( !::InsertMenu(GetHmenu(), (UINT)mswpos,
                           MF_BYPOSITION | MF_POPUP | MF_STRING,
                           (UINT_PTR)GetHmenuOf(menu), title.t_str()) )
        {
            wxLogLastError(wxT("InsertMenu"));
        }

#if wxUSE_ACCEL
        if ( menuOld->HasAccels() || menu->HasAccels() )
        {
            // need to rebuild accell table
            RebuildAccelTable();
        }
#endif // wxUSE_ACCEL

        if (IsAttached())
            Refresh();
    }

    return menuOld;
}

bool wxMenuBar::Insert(size_t pos, wxMenu *menu, const wxString& title)
{
    // Find out which MSW item before which we'll be inserting before
    // wxMenuBarBase::Insert is called and GetMenu(pos) is the new menu.
    // If IsAttached() is false this won't be used anyway
    bool isAttached =
        (GetHmenu() != 0);

    if ( !wxMenuBarBase::Insert(pos, menu, title) )
        return false;

    menu->wxMenuBase::SetTitle(title);

    if ( isAttached )
    {
        // We have a problem with the index if there is an extra "Window" menu
        // in this menu bar, which is added by wxMDIParentFrame to it directly
        // using Windows API (so that it remains invisible to the user code),
        // but which does affect the indices of the items we insert after it.
        // So we check if any of the menus before the insertion position is a
        // foreign one and adjust the insertion index accordingly.
        int mswExtra = 0;

        // Skip all this if the total number of menus matches (notice that the
        // internal menu count has already been incremented by wxMenuBarBase::
        // Insert() call above, hence -1).
        int mswCount = ::GetMenuItemCount(GetHmenu());
        if ( mswCount != -1 &&
                static_cast<unsigned>(mswCount) != GetMenuCount() - 1 )
        {
            wxMenuList::compatibility_iterator node = m_menus.GetFirst();
            for ( size_t n = 0; n < pos; n++ )
            {
                if ( ::GetSubMenu(GetHmenu(), n) != GetHmenuOf(node->GetData()) )
                    mswExtra++;
                else
                    node = node->GetNext();
            }
        }

        if ( !::InsertMenu(GetHmenu(), pos + mswExtra,
                           MF_BYPOSITION | MF_POPUP | MF_STRING,
                           (UINT_PTR)GetHmenuOf(menu), title.t_str()) )
        {
            wxLogLastError(wxT("InsertMenu"));
        }
#if wxUSE_ACCEL
        if ( menu->HasAccels() )
        {
            // need to rebuild accell table
            RebuildAccelTable();
        }
#endif // wxUSE_ACCEL

        if (IsAttached())
            Refresh();
    }

    return true;
}

bool wxMenuBar::Append(wxMenu *menu, const wxString& title)
{
    WXHMENU submenu = menu ? menu->GetHMenu() : 0;
    wxCHECK_MSG( submenu, false, wxT("can't append invalid menu to menubar") );

    if ( !wxMenuBarBase::Append(menu, title) )
        return false;

    menu->wxMenuBase::SetTitle(title);

    if (GetHmenu())
    {
        if ( !::AppendMenu(GetHmenu(), MF_POPUP | MF_STRING,
                           (UINT_PTR)submenu, title.t_str()) )
        {
            wxLogLastError(wxT("AppendMenu"));
        }

#if wxUSE_ACCEL
        if ( menu->HasAccels() )
        {
            // need to rebuild accelerator table
            RebuildAccelTable();
        }
#endif // wxUSE_ACCEL

        if (IsAttached())
            Refresh();
    }

    return true;
}

wxMenu *wxMenuBar::Remove(size_t pos)
{
    wxMenu *menu = wxMenuBarBase::Remove(pos);
    if ( !menu )
        return NULL;

    if (GetHmenu())
    {
        if ( !::RemoveMenu(GetHmenu(), (UINT)MSWPositionForWxMenu(menu,pos), MF_BYPOSITION) )
        {
            wxLogLastError(wxT("RemoveMenu"));
        }

#if wxUSE_ACCEL
        if ( menu->HasAccels() )
        {
            // need to rebuild accell table
            RebuildAccelTable();
        }
#endif // wxUSE_ACCEL

        if (IsAttached())
            Refresh();
    }

    return menu;
}

#if wxUSE_ACCEL

void wxMenuBar::RebuildAccelTable()
{
    // merge the accelerators of all menus into one accel table
    size_t nAccelCount = 0;
    size_t i, count = GetMenuCount();
    wxMenuList::iterator it;
    for ( i = 0, it = m_menus.begin(); i < count; i++, ++it )
    {
        nAccelCount += (*it)->GetAccelCount();
    }

    if ( nAccelCount )
    {
        wxAcceleratorEntry *accelEntries = new wxAcceleratorEntry[nAccelCount];

        nAccelCount = 0;
        for ( i = 0, it = m_menus.begin(); i < count; i++, ++it )
        {
            nAccelCount += (*it)->CopyAccels(&accelEntries[nAccelCount]);
        }

        SetAcceleratorTable(wxAcceleratorTable(nAccelCount, accelEntries));

        delete [] accelEntries;
    }
    else // No (more) accelerators.
    {
        SetAcceleratorTable(wxAcceleratorTable());
    }
}

#endif // wxUSE_ACCEL

void wxMenuBar::Attach(wxFrame *frame)
{
    wxMenuBarBase::Attach(frame);

#if wxUSE_ACCEL
    RebuildAccelTable();
#endif // wxUSE_ACCEL
}

void wxMenuBar::Detach()
{
    wxMenuBarBase::Detach();
}

int wxMenuBar::MSWGetTopMenuPos(WXHMENU hMenu) const
{
    for ( size_t n = 0 ; n < GetMenuCount(); ++n )
    {
        wxMenu* menu = GetMenu(n)->MSWGetMenu(hMenu);
        if ( menu )
            return n;
    }

    return wxNOT_FOUND;
}

wxMenu* wxMenuBar::MSWGetMenu(WXHMENU hMenu) const
{
    // If we're called with the handle of the menu bar itself, we can return
    // immediately as it certainly can't be the handle of one of our menus.
    if ( hMenu == GetHMenu() )
        return NULL;

    // query all menus
    for ( size_t n = 0 ; n < GetMenuCount(); ++n )
    {
        wxMenu* menu = GetMenu(n)->MSWGetMenu(hMenu);
        if ( menu )
            return menu;
    }

    // unknown hMenu
    return NULL;
}

#endif // wxUSE_MENUS
