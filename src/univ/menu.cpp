/////////////////////////////////////////////////////////////////////////////
// Name:        univ/menu.cpp
// Purpose:     wxMenuItem, wxMenu and wxMenuBar implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     25.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "univmenuitem.h"
    #pragma implementation "univmenu.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/dynarray.h"
    #include "wx/controlh"      // for FindAccelIndex()
    #include "wx/menu.h"
#endif // WX_PRECOMP

#if wxUSE_MENUS

#include "wx/univ/renderer.h"

// ----------------------------------------------------------------------------
// wxMenuInfo contains all extra information about top level menus we need
// ----------------------------------------------------------------------------

struct WXDLLEXPORT wxMenuInfo
{
    wxMenuInfo(wxMenuBar *menubar, const wxString& title)
    {
        SetLabel(menubar, title);
    }

    void SetLabel(wxMenuBar *menubar, const wxString& title)
    {
        label = title;

        wxSize size;
        wxClientDC dc(menubar);
        dc.GetTextExtent(title, &size.x, &size.y);

        // adjust for the renderer we use and store the width
        width = menubar->GetRenderer()->GetMenuBarItemSize(size).x;
    }

    wxString label;
    wxCoord width;
    bool isEnabled;
};

#include "wx/arrimpl.cpp"

WX_DEFINE_OBJARRAY(wxMenuInfoArray);

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMenu, wxMenuBase)
IMPLEMENT_DYNAMIC_CLASS(wxMenuBar, wxMenuBarBase)
IMPLEMENT_DYNAMIC_CLASS(wxMenuItem, wxMenuItemBase)

BEGIN_EVENT_TABLE(wxMenuBar, wxMenuBarBase)
    EVT_SET_FOCUS(wxMenuBar::OnSetFocus)
    EVT_KEY_DOWN(wxMenuBar::OnKeyDown)
    EVT_LEFT_DOWN(wxMenuBar::OnLeftDown)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxMenu
// ----------------------------------------------------------------------------

bool wxMenu::DoAppend(wxMenuItem *item)
{
    if ( !wxMenuBase::DoAppend(item) )
        return FALSE;

    return TRUE;
}

bool wxMenu::DoInsert(size_t pos, wxMenuItem *item)
{
    if ( !wxMenuBase::DoInsert(pos, item) )
        return FALSE;

    return TRUE;
}

wxMenuItem *wxMenu::DoRemove(wxMenuItem *item)
{
    wxMenuItem *itemOld = wxMenuBase::DoRemove(item);

    if ( itemOld )
    {
    }

    return itemOld;
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

// ----------------------------------------------------------------------------
// wxMenuBar creation
// ----------------------------------------------------------------------------

void wxMenuBar::Init()
{
    m_frameLast = NULL;
    m_focusOld = NULL;
}

void wxMenuBar::Attach(wxFrame *frame)
{
    // maybe you really wanted to call Detach()?
    wxCHECK_RET( frame, _T("wxMenuBar::Attach(NULL) called") );

    if ( IsCreated() )
    {
        // reparent if necessary
        if ( m_frameLast != frame )
        {
            Reparent(frame);
        }

        // show it back - was hidden by Detach()
        Show();
    }
    else // not created yet, do it now
    {
        // we have no way to return the error from here anyhow :-(
        (void)Create(frame, -1);
    }

    // remember the last frame which had us to avoid unnecessarily reparenting
    // above
    m_frameLast = frame;
}

void wxMenuBar::Detach()
{
    // don't delete the window because we may be reattached later, just hide it
    if ( m_frameLast )
    {
        Hide();
    }

    wxMenuBarBase::Detach();
}

// ----------------------------------------------------------------------------
// wxMenuBar adding/removing items
// ----------------------------------------------------------------------------

bool wxMenuBar::Append(wxMenu *menu, const wxString& title)
{
    return Insert(GetCount(), menu, title);
}

bool wxMenuBar::Insert(size_t pos, wxMenu *menu, const wxString& title)
{
    if ( !wxMenuBarBase::Insert(pos, menu, title) )
        return FALSE;

    wxMenuInfo *info = new wxMenuInfo(this, title);
    m_menuInfos.Insert(info, pos);

    RefreshAllItemsAfter(pos);

    return TRUE;
}

wxMenu *wxMenuBar::Replace(size_t pos, wxMenu *menu, const wxString& title)
{
    wxMenu *menuOld = wxMenuBarBase::Replace(pos, menu, title);

    if ( menuOld )
    {
        wxMenuInfo& info = m_menuInfos[pos];

        info.SetLabel(this, title);

        // even if the old menu was disabled, the new one is not any more
        info.isEnabled = TRUE;

        // even if we change only this one, the new label has different width,
        // so we need to refresh everything beyond this item as well
        RefreshAllItemsAfter(pos);
    }

    return menuOld;
}

wxMenu *wxMenuBar::Remove(size_t pos)
{
    wxMenu *menuOld = wxMenuBarBase::Remove(pos);

    if ( menuOld )
    {
        m_menuInfos.RemoveAt(pos);

        // this doesn't happen too often, so don't try to be too smart - just
        // refresh everything
        Refresh();
    }

    return menuOld;
}

// ----------------------------------------------------------------------------
// wxMenuBar top level menus access
// ----------------------------------------------------------------------------

wxCoord wxMenuBar::GetItemWidth(size_t pos) const
{
    return m_menuInfos[pos].width;
}

void wxMenuBar::EnableTop(size_t pos, bool enable)
{
    wxCHECK_RET( pos < GetCount(), _T("invalid index in EnableTop") );

    if ( enable != m_menuInfos[pos].isEnabled )
    {
        m_menuInfos[pos].isEnabled = enable;

        RefreshItem(pos);
    }
    //else: nothing to do
}

bool wxMenuBar::IsEnabledTop(size_t pos) const
{
    wxCHECK_MSG( pos < GetCount(), FALSE, _T("invalid index in IsEnabledTop") );

    return m_menuInfos[pos].isEnabled;
}

void wxMenuBar::SetLabelTop(size_t pos, const wxString& label)
{
    wxCHECK_RET( pos < GetCount(), _T("invalid index in EnableTop") );

    if ( label != m_menuInfos[pos].label )
    {
        m_menuInfos[pos].label = label;

        RefreshItem(pos);
    }
    //else: nothing to do
}

wxString wxMenuBar::GetLabelTop(size_t pos) const
{
    wxCHECK_MSG( pos < GetCount(), _T(""), _T("invalid index in GetLabelTop") );

    return m_menuInfos[pos].label;
}

// ----------------------------------------------------------------------------
// wxMenuBar drawing
// ----------------------------------------------------------------------------

void wxMenuBar::RefreshAllItemsAfter(size_t pos)
{
    wxRect rect = GetItemRect(pos);
    rect.width = GetClientSize().x - rect.x;
    RefreshRect(rect);
}

void wxMenuBar::RefreshItem(size_t pos)
{
    RefreshRect(GetItemRect(pos));
}

void wxMenuBar::DoDraw(wxControlRenderer *renderer)
{
    // redraw only the items which must be redrawn

    // we don't have to use GetUpdateClientRect() here because our client rect
    // is the same as total one
    wxRect rectUpdate = GetUpdateRegion().GetBox();

    int flagsMenubar = GetStateFlags();

    wxRect rect;
    rect.y = 0;
    rect.height = GetClientSize().y;

    wxCoord x = 0;
    size_t count = GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        if ( x > rectUpdate.GetRight() )
        {
            // all remaining items are to the right of rectUpdate
            break;
        }

        rect.x = x;
        rect.width = GetItemWidth(n);
        x += rect.width;
        if ( x < rectUpdate.x )
        {
            // this item is still to the left of rectUpdate
            continue;
        }

        int flags = flagsMenubar;
        if ( m_current != -1 && n == (size_t)m_current )
        {
            flags |= wxCONTROL_SELECTED;
        }

        wxString label = GetLabelTop(n);
        GetRenderer()->DrawMenuBarItem(renderer->GetDC(), rect, label, flags,
                                       wxControl::FindAccelIndex(label));
    }
}

// ----------------------------------------------------------------------------
// wxMenuBar geometry
// ----------------------------------------------------------------------------

wxRect wxMenuBar::GetItemRect(size_t pos) const
{
    wxASSERT_MSG( pos < GetCount(), _T("invalid menu bar item index") );

    wxRect rect;
    rect.x =
    rect.y = 0;
    rect.height = GetClientSize().y;

    for ( size_t n = 0; n < pos; n++ )
    {
        rect.x += GetItemWidth(n);
    }

    rect.width = GetItemWidth(pos);

    return rect;
}

wxSize wxMenuBar::DoGetBestClientSize() const
{
    wxSize size;
    if ( GetMenuCount() > 0 )
    {
        wxClientDC dc(wxConstCast(this, wxMenuBar));
        dc.GetTextExtent(GetLabelTop(0), &size.x, &size.y);

        // adjust for the renderer we use
        size = GetRenderer()->GetMenuBarItemSize(size);
    }
    else // empty menubar
    {
        size.x =
        size.y = 0;
    }

    // the width is arbitrary, of course, for horizontal menubar
    size.x = 100;

    return size;
}

// ----------------------------------------------------------------------------
// wxMenuBar input handing
// ----------------------------------------------------------------------------

/*
   Note that wxMenuBar doesn't use wxInputHandler but handles keyboard and
   mouse in the same way under all platforms. This is because it doesn't derive
   from wxControl (which works with input handlers) but directly from wxWindow.

   Also, menu bar input handling is rather simple, so maybe it's not really
   worth making it themeable - at least I've decided against doing it now as it
   would merging the changes back into trunk more difficult. But it still could
   be done later if really needed.
 */

void wxMenuBar::OnSetFocus(wxFocusEvent& event)
{
    // remember the window which had the focus before
    m_focusOld = event.GetWindow();
}

void wxMenuBar::OnLeftDown(wxMouseEvent& event)
{
    m_current = HitTest(event.GetPosition());
    if ( m_current == -1 )
    {
        // don't skip the event so we won't get the focus
        return;
    }

    // show it as selected
    RefreshItem(m_current);

    // and let the menu bar to have focus
    event.Skip();
}

void wxMenuBar::OnKeyDown(wxKeyEvent& event)
{
    // the current item must have been set before
    wxCHECK_RET( m_current != -1, _T("where is current item?") );

    // cycle through the menu items when left/right arrows are pressed and open
    // the menu when up/down one is
    int key = event.GetKeyCode();
    switch ( key )
    {
        case WXK_ESCAPE:
            // remove the selection and give the focus away
            {
                size_t current = (size_t)m_current;
                m_current = -1;
                RefreshItem(current);

                if ( !m_focusOld )
                {
                    wxCHECK_RET( m_menuBarFrame, _T("menubar without frame?") );

                    m_focusOld = m_menuBarFrame;
                }

                m_focusOld->SetFocus();
            }
            break;

        case WXK_LEFT:
        case WXK_RIGHT:
            {
                size_t count = GetCount();
                if ( count == 1 )
                {
                    // the item won't change anyhow
                    break;
                }
                //else: otherwise, it will

                // and so the old current item loses its selected status
                RefreshItem(m_current);

                if ( key == WXK_LEFT )
                {
                    if ( m_current-- == 0 )
                        m_current = count;
                }
                else // right
                {
                    if ( m_current++ == (int)count )
                        m_current = 0;
                }

                // this one should be shown as selected
                RefreshItem(m_current);
            }
            break;

        case WXK_DOWN:
        case WXK_UP:
            // TODO: open the menu
            break;

        default:
            event.Skip();
    }
}

#endif // wxUSE_MENUS

