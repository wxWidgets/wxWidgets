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
    #include "wx/settings.h"
#endif // WX_PRECOMP

#if wxUSE_MENUS

#include "wx/popupwin.h"

#include "wx/univ/renderer.h"

// ----------------------------------------------------------------------------
// wxMenuInfo contains all extra information about top level menus we need
// ----------------------------------------------------------------------------

struct WXDLLEXPORT wxMenuInfo
{
    wxMenuInfo(wxMenuBar *menubar, const wxString& text)
    {
        SetLabel(menubar, text);
    }

    void SetLabel(wxMenuBar *menubar, const wxString& text)
    {
        indexAccel = wxControl::FindAccelIndex(text, &label);

        wxSize size;
        wxClientDC dc(menubar);
        dc.SetFont(wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT));
        dc.GetTextExtent(label, &size.x, &size.y);

        // adjust for the renderer we use and store the width
        width = menubar->GetRenderer()->GetMenuBarItemSize(size).x;
    }

    wxString label;
    wxCoord width;
    int indexAccel;
    bool isEnabled;
};

#include "wx/arrimpl.cpp"

WX_DEFINE_OBJARRAY(wxMenuInfoArray);

// ----------------------------------------------------------------------------
// wxPopupMenuWindow: a popup window showing a menu
// ----------------------------------------------------------------------------

class wxPopupMenuWindow : public wxPopupTransientWindow
{
public:
    wxPopupMenuWindow(wxWindow *parent, wxMenu *menu)
        : wxPopupTransientWindow(parent)
        {
            m_menu = menu;

            (void)Create(parent, wxBORDER_RAISED);
        }

    virtual void OnDismiss() { m_menu->OnDismiss(); }

protected:
    // draw the menu inside this window
    virtual void DoDraw(wxControlRenderer *renderer)
    {
        m_menu->DoDraw(renderer);
    }

private:
    // the menu we show
    wxMenu *m_menu;
};

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMenu, wxEvtHandler)
IMPLEMENT_DYNAMIC_CLASS(wxMenuBar, wxWindow)
IMPLEMENT_DYNAMIC_CLASS(wxMenuItem, wxObject)

BEGIN_EVENT_TABLE(wxMenuBar, wxMenuBarBase)
    EVT_SET_FOCUS(wxMenuBar::OnSetFocus)

    EVT_KEY_DOWN(wxMenuBar::OnKeyDown)

    EVT_LEFT_DOWN(wxMenuBar::OnLeftDown)
    EVT_MOTION(wxMenuBar::OnMouseMove)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

wxWindow *wxMenu::GetParentWindow() const
{
    return m_menuBar ? m_menuBar : m_invokingWindow;
}

// ----------------------------------------------------------------------------
// wxMenu
// ----------------------------------------------------------------------------

void wxMenu::Init()
{
    m_geometry = NULL;

    m_popupMenu = NULL;
}

wxMenu::~wxMenu()
{
    delete m_geometry;
    delete m_popupMenu;
}

// ----------------------------------------------------------------------------
// wxMenu and wxMenuGeometryInfo
// ----------------------------------------------------------------------------

wxMenuGeometryInfo::~wxMenuGeometryInfo()
{
}

const wxMenuGeometryInfo& wxMenu::GetGeometryInfo() const
{
    if ( !m_geometry )
    {
        wxWindow *win = GetParentWindow();

        if ( win )
        {
            wxConstCast(this, wxMenu)->m_geometry =
                win->GetRenderer()->GetMenuGeometry(win, *this);
        }
        else
        {
            wxFAIL_MSG( _T("neither popup nor menubar menu?") );
        }
    }

    return *m_geometry;
}

void wxMenu::InvalidateGeometryInfo()
{
    if ( m_geometry )
    {
        delete m_geometry;
        m_geometry = NULL;
    }
}

// ----------------------------------------------------------------------------
// wxMenu adding/removing items
// ----------------------------------------------------------------------------

bool wxMenu::DoAppend(wxMenuItem *item)
{
    if ( !wxMenuBase::DoAppend(item) )
        return FALSE;

    InvalidateGeometryInfo();

    return TRUE;
}

bool wxMenu::DoInsert(size_t pos, wxMenuItem *item)
{
    if ( !wxMenuBase::DoInsert(pos, item) )
        return FALSE;

    InvalidateGeometryInfo();

    return TRUE;
}

wxMenuItem *wxMenu::DoRemove(wxMenuItem *item)
{
    wxMenuItem *itemOld = wxMenuBase::DoRemove(item);

    if ( itemOld )
    {
        InvalidateGeometryInfo();
    }

    return itemOld;
}

// ----------------------------------------------------------------------------
// wxMenu geometry
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// wxMenu drawing
// ----------------------------------------------------------------------------

wxRenderer *wxMenu::GetRenderer() const
{
    wxWindow *win = GetParentWindow();

    // we're going to crash without renderer!
    wxCHECK_MSG( win, NULL, _T("neither popup nor menubar menu?") );

    return win->GetRenderer();
}

void wxMenu::RefreshItem(wxMenuItem *item)
{
    // the item geometry changed, so our might have changed as well
    InvalidateGeometryInfo();

    if ( IsShown() )
    {
        // this would eb a bug in IsShown()
        wxCHECK_RET( m_popupMenu, _T("must have popup window if shown!") );

        // refresh everything - not very efficient but this shouldn't happen
        // often
        m_popupMenu->Refresh();
    }
}

void wxMenu::DoDraw(wxControlRenderer *renderer)
{
    // no clipping so far - do we need it? I don't think so as the menu is
    // never partially covered as it is always on top of everything

    wxDC& dc = renderer->GetDC();
    dc.SetFont(wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT));

    // FIXME: this should be done in the renderer
    //dc.SetDeviceOrigin(1, 1);

    wxRenderer *rend = renderer->GetRenderer();

    wxCoord y = 0;
    const wxMenuGeometryInfo& gi = GetGeometryInfo();
    for ( wxMenuItemList::Node *node = m_items.GetFirst();
          node;
          node = node->GetNext() )
    {
        wxMenuItem *item = node->GetData();

        // height of this item
        wxCoord h;

        if ( item->IsSeparator() )
        {
            h = rend->DrawMenuSeparator(dc, y, gi);
        }
        else // not a separator
        {
            int flags = 0;
            if ( item->IsCheckable() )
            {
                flags |= wxCONTROL_CHECKABLE;

                if ( item->IsChecked() )
                {
                    flags |= wxCONTROL_CHECKED;
                }
            }

            if ( !item->IsEnabled() )
                flags |= wxCONTROL_DISABLED;

            if ( item->IsSubMenu() )
                flags |= wxCONTROL_ISSUBMENU;

            h = rend->DrawMenuItem
                      (
                        dc,
                        y,
                        gi,
                        item->GetLabel(),
                        item->GetAccelString(),
                        item->GetBitmap((flags & wxCONTROL_CHECKED) != 0),
                        flags,
                        item->GetAccelIndex()
                      );
        }

        y += h;
    }
}

// ----------------------------------------------------------------------------
// wxMenu showing and hiding
// ----------------------------------------------------------------------------

bool wxMenu::IsShown() const
{
    return m_isShown;
}

void wxMenu::OnDismiss()
{
    m_isShown = FALSE;

    // notify the menu bar if we're a top level menu
    if ( m_menuBar )
    {
        if ( !m_menuParent )
        {
            m_menuBar->OnDismiss();
        }
        //else: we're not a top level menu
    }
    else // popup window
    {
        wxCHECK_RET( m_invokingWindow, _T("what kind of menu is this?") );

        // TODO we need to return from DoPopupMenu()
    }
}

void wxMenu::Popup(const wxPoint& pos, const wxSize& size)
{
    // create the popup window if not done yet
    if ( !m_popupMenu )
    {
        m_popupMenu = new wxPopupMenuWindow(GetParentWindow(), this);

        m_popupMenu->SetClientSize(GetGeometryInfo().GetSize());
    }

    // position it as specified
    m_popupMenu->Position(pos, size);

    // and show it
    m_popupMenu->Popup();

    m_isShown = TRUE;
}

void wxMenu::Dismiss()
{
    wxCHECK_RET( IsShown(), _T("can't dismiss hidden menu") );

    m_popupMenu->Dismiss();
}

// ----------------------------------------------------------------------------
// wxMenuItem construction
// ----------------------------------------------------------------------------

wxMenuItem::wxMenuItem(wxMenu *parentMenu,
                       int id,
                       const wxString& text,
                       const wxString& help,
                       bool isCheckable,
                       wxMenu *subMenu)
{
    m_id = id;
    m_parentMenu = parentMenu;
    m_subMenu = subMenu;

    m_text = text;
    m_help = help;

    m_isCheckable = isCheckable;
    m_isEnabled = TRUE;
    m_isChecked = FALSE;

    m_indexAccel = -1;
}

wxMenuItem::~wxMenuItem()
{
}

// ----------------------------------------------------------------------------
// wxMenuItemBase methods implemented here
// ----------------------------------------------------------------------------

/* static */
wxMenuItem *wxMenuItemBase::New(wxMenu *parentMenu,
                                int id,
                                const wxString& name,
                                const wxString& help,
                                bool isCheckable,
                                wxMenu *subMenu)
{
    return new wxMenuItem(parentMenu, id, name, help, isCheckable, subMenu);
}

/* static */
wxString wxMenuItemBase::GetLabelFromText(const wxString& text)
{
    return wxStripMenuCodes(text);
}

// ----------------------------------------------------------------------------
// wxMenuItem operations
// ----------------------------------------------------------------------------

void wxMenuItem::NotifyMenu()
{
    m_parentMenu->RefreshItem(this);
}

void wxMenuItem::SetText(const wxString& text)
{
    if ( text != m_text )
    {
        wxMenuItemBase::SetText(text);

        m_indexAccel = wxControl::FindAccelIndex(text);

        // will be empty if the text contains no TABs - ok
        m_strAccel = text.AfterFirst(_T('\t'));

        NotifyMenu();
    }
}

void wxMenuItem::SetCheckable(bool checkable)
{
    if ( checkable != m_isCheckable )
    {
        wxMenuItemBase::SetCheckable(checkable);

        NotifyMenu();
    }
}

void wxMenuItem::SetBitmaps(const wxBitmap& bmpChecked,
                            const wxBitmap& bmpUnchecked)
{
    m_bmpChecked = bmpChecked;
    m_bmpUnchecked = bmpUnchecked;

    NotifyMenu();
}

void wxMenuItem::Enable(bool enable)
{
    if ( enable != m_isEnabled )
    {
        wxMenuItemBase::Enable(enable);

        NotifyMenu();
    }
}

void wxMenuItem::Check(bool check)
{
    if ( check != m_isChecked )
    {
        wxMenuItemBase::Check(check);

        NotifyMenu();
    }
}

// ----------------------------------------------------------------------------
// wxMenuBar creation
// ----------------------------------------------------------------------------

void wxMenuBar::Init()
{
    m_frameLast = NULL;
    m_focusOld = NULL;

    m_current = -1;

    m_menuShown = NULL;
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

        SetFont(wxSystemSettings::GetSystemFont(wxSYS_SYSTEM_FONT));
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

wxMenuBar::~wxMenuBar()
{
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
    wxDC& dc = renderer->GetDC();
    dc.SetFont(wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT));

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

        GetRenderer()->DrawMenuBarItem
                       (
                            dc,
                            rect,
                            m_menuInfos[n].label,
                            flags,
                            m_menuInfos[n].indexAccel
                       );
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
        dc.SetFont(wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT));
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

int wxMenuBar::GetMenuFromPoint(const wxPoint& pos) const
{
    if ( pos.x < 0 || pos.y < 0 || pos.y > GetClientSize().y )
        return -1;

    // do find it
    wxCoord x = 0;
    size_t count = GetCount();
    for ( size_t item = 0; item < count; item++ )
    {
        x += GetItemWidth(item);

        if ( x > pos.x )
        {
            return item;
        }
    }

    // to the right of the last menu item
    return -1;
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
    if ( HasCapture() )
    {
        if ( m_current != -1 )
        {
            RefreshItem(m_current);
            m_current = -1;
        }

        ReleaseCapture();
    }
    else // we didn't have mosue capture, capture it now
    {
        m_current = GetMenuFromPoint(event.GetPosition());
        if ( m_current == -1 )
        {
            // don't skip the event so we won't get the focus
            return;
        }

        // show it as selected
        RefreshItem(m_current);

        CaptureMouse();
    }
}

void wxMenuBar::OnMouseMove(wxMouseEvent& event)
{
    if ( HasCapture() )
    {
        int currentNew = GetMenuFromPoint(event.GetPosition());
        if ( currentNew != m_current )
        {
            if ( m_current != -1 )
                RefreshItem(m_current);

            m_current = currentNew;

            if ( m_current != -1 )
                RefreshItem(m_current);
        }
    }
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
            if ( m_current != -1 )
            {
                if ( IsShowingMenu() )
                {
                    DismissMenu();
                }

                size_t current = (size_t)m_current;
                m_current = -1;
                RefreshItem(current);
            }

            if ( !m_focusOld )
            {
                wxCHECK_RET( m_menuBarFrame, _T("menubar without frame?") );

                m_focusOld = m_menuBarFrame;
            }

            m_focusOld->SetFocus();
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

                // remember if we were showing a menu - if we did, we should
                // show the new menu after changing the item
                bool wasMenuOpened = IsShowingMenu();
                if ( wasMenuOpened )
                {
                    DismissMenu();
                }

                // and so the old current item loses its selected status
                RefreshItem(m_current);

                if ( key == WXK_LEFT )
                {
                    if ( m_current-- == 0 )
                        m_current = count - 1;
                }
                else // right
                {
                    if ( ++m_current == (int)count )
                        m_current = 0;
                }

                // this one should be shown as selected
                RefreshItem(m_current);

                if ( wasMenuOpened )
                {
                    PopupMenu();
                }
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

// ----------------------------------------------------------------------------
// wxMenuBar menus showing
// ----------------------------------------------------------------------------

void wxMenuBar::PopupMenu()
{
    wxCHECK_RET( m_current != -1, _T("no menu to popup") );

    // forgot to call DismissMenu()?
    wxASSERT_MSG( !m_menuShown, _T("shouldn't show two menu at once!") );

    // remember the menu we show
    m_menuShown = GetMenu(m_current);

    // position it correctly: note that we must use screen coords and that we
    // pass 0 as width to position the menu exactly below the item, not to the
    // right of it
    wxRect rectItem = GetItemRect(m_current);
    m_menuShown->Popup(ClientToScreen(rectItem.GetPosition()),
                       wxSize(0, rectItem.GetHeight()));
}

void wxMenuBar::DismissMenu()
{
    wxCHECK_RET( m_menuShown, _T("can't dismiss menu if none is shown") );

    m_menuShown->Dismiss();
    OnDismiss();
}

void wxMenuBar::OnDismiss()
{
    m_menuShown = NULL;
}

// ----------------------------------------------------------------------------
// popup menu support
// ----------------------------------------------------------------------------

bool wxWindow::DoPopupMenu(wxMenu *menu, int x, int y)
{
    // TODO
    wxFAIL_MSG( _T("not implemented") );

    return FALSE;
}

#endif // wxUSE_MENUS

