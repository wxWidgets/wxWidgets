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
    #include "wx/control.h"      // for FindAccelIndex()
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
    wxPopupMenuWindow(wxWindow *parent, wxMenu *menu);

    // override the base class version to select the first item initially
    virtual void Popup(wxWindow *focus = NULL);

    // notify the menu when the window disappears from screen
    virtual void OnDismiss();

    // get the currently selected item (may be NULL)
    wxMenuItem *GetCurrentItem() const
    {
        return m_nodeCurrent ? m_nodeCurrent->GetData() : NULL;
    }

    // find the menu item at given position
    wxMenuItemList::Node *GetMenuItemFromPoint(const wxPoint& pt) const;

    // refresh the given item
    void RefreshItem(wxMenuItem *item);

    // process the key event, return TRUE if done
    bool ProcessKeyDown(int key);

protected:
    // draw the menu inside this window
    virtual void DoDraw(wxControlRenderer *renderer);

    // event handlers
    void OnLeftUp(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnKeyDown(wxKeyEvent& event);

    // reset the current item and node
    void ResetCurrent();

    // set the current node and item withotu refreshing anything
    void SetCurrent(wxMenuItemList::Node *node);

    // change the current item refreshing the old and new items
    void ChangeCurrent(wxMenuItemList::Node *node);

    // send the event about the item click
    void ClickItem(wxMenuItem *item);

    // show the submenu for this item
    void OpenSubmenu(wxMenuItem *item);

    // hide the menu
    void HideMenu(bool dismissParent);

    // react to menu dismissing
    void DoDismiss(bool dismissParent);

private:
    // the menu we show
    wxMenu *m_menu;

    // the menu node corresponding to the current item
    wxMenuItemList::Node *m_nodeCurrent;

    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMenu, wxEvtHandler)
IMPLEMENT_DYNAMIC_CLASS(wxMenuBar, wxWindow)
IMPLEMENT_DYNAMIC_CLASS(wxMenuItem, wxObject)

BEGIN_EVENT_TABLE(wxPopupMenuWindow, wxPopupTransientWindow)
    EVT_KEY_DOWN(wxPopupMenuWindow::OnKeyDown)

    EVT_LEFT_UP(wxPopupMenuWindow::OnLeftUp)
    EVT_MOTION(wxPopupMenuWindow::OnMouseMove)
    EVT_LEAVE_WINDOW(wxPopupMenuWindow::OnMouseLeave)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxMenuBar, wxMenuBarBase)
    EVT_SET_FOCUS(wxMenuBar::OnSetFocus)

    EVT_KEY_DOWN(wxMenuBar::OnKeyDown)

    EVT_LEFT_DOWN(wxMenuBar::OnLeftDown)
    EVT_MOTION(wxMenuBar::OnMouseMove)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxPopupMenuWindow
// ----------------------------------------------------------------------------

wxPopupMenuWindow::wxPopupMenuWindow(wxWindow *parent, wxMenu *menu)
{
    m_menu = menu;
    ResetCurrent();

    (void)Create(parent, wxBORDER_RAISED);
}

// ----------------------------------------------------------------------------
// wxPopupMenuWindow current item/node handling
// ----------------------------------------------------------------------------

void wxPopupMenuWindow::ResetCurrent()
{
    SetCurrent(NULL);
}

void wxPopupMenuWindow::SetCurrent(wxMenuItemList::Node *node)
{
    m_nodeCurrent = node;
}

void wxPopupMenuWindow::ChangeCurrent(wxMenuItemList::Node *node)
{
    if ( node != m_nodeCurrent )
    {
        if ( m_nodeCurrent )
            RefreshItem(m_nodeCurrent->GetData());

        m_nodeCurrent = node;

        if ( m_nodeCurrent )
            RefreshItem(m_nodeCurrent->GetData());
    }
}

// ----------------------------------------------------------------------------
// wxPopupMenuWindow popup/dismiss
// ----------------------------------------------------------------------------

void wxPopupMenuWindow::Popup(wxWindow *focus)
{
    SetCurrent(m_menu->GetMenuItems().GetFirst());

    wxPopupTransientWindow::Popup(focus);
}

void wxPopupMenuWindow::OnDismiss()
{
    // when we are dismissed because the user clicked elsewhere or we lost
    // focus in any other way, hide the parent menu as well
    DoDismiss(TRUE);
}

void wxPopupMenuWindow::DoDismiss(bool dismissParent)
{
    ResetCurrent();

    m_menu->OnDismiss(dismissParent);
}

void wxPopupMenuWindow::HideMenu(bool dismissParent)
{
    Dismiss();
    DoDismiss(dismissParent);
}

// ----------------------------------------------------------------------------
// wxPopupMenuWindow geometry
// ----------------------------------------------------------------------------

wxMenuItemList::Node *
wxPopupMenuWindow::GetMenuItemFromPoint(const wxPoint& pt) const
{
    // we only use the y coord normally, but still check x in case the point is
    // outside the window completely
    if ( wxWindow::HitTest(pt) == wxHT_WINDOW_INSIDE )
    {
        wxCoord y = 0;
        for ( wxMenuItemList::Node *node = m_menu->GetMenuItems().GetFirst();
              node;
              node = node->GetNext() )
        {
            wxMenuItem *item = node->GetData();
            y += item->GetHeight();
            if ( y > pt.y )
            {
                // found
                return node;
            }
        }
    }

    return NULL;
}

// ----------------------------------------------------------------------------
// wxPopupMenuWindow drawing
// ----------------------------------------------------------------------------

void wxPopupMenuWindow::RefreshItem(wxMenuItem *item)
{
    wxCHECK_RET( item, _T("can't refresh NULL item") );

    wxASSERT_MSG( IsShown(), _T("can't refresh menu which is not shown") );

    // FIXME: -1 here because of SetLogicalOrigin(1, 1) in DoDraw()
    RefreshRect(wxRect(0, item->GetPosition() - 1,
                m_menu->GetGeometryInfo().GetSize().x, item->GetHeight()));
}

void wxPopupMenuWindow::DoDraw(wxControlRenderer *renderer)
{
    // no clipping so far - do we need it? I don't think so as the menu is
    // never partially covered as it is always on top of everything

    wxDC& dc = renderer->GetDC();
    dc.SetFont(wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT));

    // FIXME: this should be done in the renderer, however when it is fixed
    //        wxPopupMenuWindow::RefreshItem() should be changed too!
    dc.SetLogicalOrigin(1, 1);

    wxRenderer *rend = renderer->GetRenderer();

    wxCoord y = 0;
    const wxMenuGeometryInfo& gi = m_menu->GetGeometryInfo();
    for ( wxMenuItemList::Node *node = m_menu->GetMenuItems().GetFirst();
          node;
          node = node->GetNext() )
    {
        wxMenuItem *item = node->GetData();

        if ( item->IsSeparator() )
        {
            rend->DrawMenuSeparator(dc, y, gi);
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

            if ( item == GetCurrentItem() )
                flags |= wxCONTROL_SELECTED;

            rend->DrawMenuItem
                  (
                     dc,
                     y,
                     gi,
                     item->GetLabel(),
                     item->GetAccelString(),
                     // strangely enough, for unchecked item we use the
                     // "checked" bitmap because this is the default one - this
                     // explain this strange boolean expression
                     item->GetBitmap(!item->IsCheckable() || item->IsChecked()),
                     flags,
                     item->GetAccelIndex()
                  );
        }

        y += item->GetHeight();
    }
}

// ----------------------------------------------------------------------------
// wxPopupMenuWindow actions
// ----------------------------------------------------------------------------

void wxPopupMenuWindow::ClickItem(wxMenuItem *item)
{
    wxCHECK_RET( item, _T("can't click NULL item") );

    wxASSERT_MSG( !item->IsSeparator() && !item->IsSubMenu(),
                  _T("can't click this item") );

    m_menu->SendEvent(item->GetId(),
                      item->IsCheckable() ? item->IsChecked() : -1);

    // close all menus
    HideMenu(TRUE);
}

void wxPopupMenuWindow::OpenSubmenu(wxMenuItem *item)
{
    wxCHECK_RET( item, _T("can't open NULL submenu") );

    wxMenu *submenu = item->GetSubMenu();
    wxCHECK_RET( submenu, _T("can only open submenus!") );

    submenu->Popup(ClientToScreen(wxPoint(0, item->GetPosition())),
                   wxSize(m_menu->GetGeometryInfo().GetSize().x, 0));
}

// ----------------------------------------------------------------------------
// wxPopupMenuWindow input handling
// ----------------------------------------------------------------------------

void wxPopupMenuWindow::OnLeftUp(wxMouseEvent& event)
{
    wxMenuItemList::Node *node = GetMenuItemFromPoint(event.GetPosition());
    if ( node )
    {
        wxMenuItem *item = node->GetData();

        // clicking disabled items doesn't work
        if ( item->IsEnabled() )
        {
            // normal menu items generate commands, submenus can be opened and the
            // separators don't do anything
            if ( item->IsSubMenu() )
            {
                OpenSubmenu(item);
            }
            else if ( !item->IsSeparator() )
            {
                ClickItem(item);
            }
        }
    }
}

void wxPopupMenuWindow::OnMouseMove(wxMouseEvent& event)
{
    wxMenuBar *menubar = m_menu->GetMenuBar();
    if ( menubar )
    {
        // we need to translate coords to the client coords of the menubar
        wxPoint pt = ClientToScreen(event.GetPosition());
        if ( menubar->ProcessMouseEvent(menubar->ScreenToClient(pt)) )
        {
            // menubar has closed this menu and opened another one, probably
            return;
        }
    }

    wxMenuItemList::Node *node = GetMenuItemFromPoint(event.GetPosition());

    // don't reset current to NULL here, we only do it when the mouse leaves
    // the window (see below)
    if ( node )
    {
        ChangeCurrent(node);

        wxMenuItem *item = GetCurrentItem();
        if ( item->IsEnabled() && item->IsSubMenu() )
        {
            OpenSubmenu(item);
        }
    }

    event.Skip();
}

void wxPopupMenuWindow::OnMouseLeave(wxMouseEvent& event)
{
    // due to the artefact of mouse events generation under MSW, we actually
    // may get the mouse leave event after the menu had been already dismissed
    // and calling ChangeCurrent() would then assert, so don't do it
    if ( IsShown() )
    {
        ChangeCurrent(NULL);
    }

    event.Skip();
}

void wxPopupMenuWindow::OnKeyDown(wxKeyEvent& event)
{
    if ( !ProcessKeyDown(event.GetKeyCode()) )
    {
        event.Skip();
    }
}

bool wxPopupMenuWindow::ProcessKeyDown(int key)
{
    wxMenuItem *item = GetCurrentItem();

    // first let the opened submenu to have it (no test for IsEnabled() here,
    // the keys navigate even in a disabled submenu if we had somehow managed
    // to open it inspit of this)
    if ( item && item->IsSubMenu() && item->GetSubMenu()->ProcessKeyDown(key) )
    {
        return TRUE;
    }

    bool processed = TRUE;

    // handle the up/down arrows, home, end, esc and return here, pass the
    // left/right arrows to the menu bar except when the right arrow can be
    // used to open a submenu
    switch ( key )
    {
        case WXK_ESCAPE:
            // close just this menu
            HideMenu(FALSE);
            break;

        case WXK_RETURN:
            if ( item && item->IsEnabled() )
            {
                if ( item->IsSubMenu() )
                {
                    OpenSubmenu(item);
                }
                else if ( !item->IsSeparator() )
                {
                    ClickItem(item);
                }
                else // separator, nothing to do
                {
                    processed = FALSE;
                }
            }
            else // no item or item is disabled, nothing to do
            {
                processed = FALSE;
            }
            break;

        case WXK_HOME:
            ChangeCurrent(m_menu->GetMenuItems().GetFirst());
            break;

        case WXK_END:
            ChangeCurrent(m_menu->GetMenuItems().GetLast());
            break;

        case WXK_UP:
            {
                wxMenuItemList::Node *node;
                if ( m_nodeCurrent )
                {
                    node = m_nodeCurrent->GetPrevious();
                }
                else // no current item
                {
                    node = NULL;
                }

                if ( !node )
                {
                    // if no current item or if it is the first one, select the
                    // last one as Windows does it
                    node = m_menu->GetMenuItems().GetLast();
                }

                ChangeCurrent(node);
            }
            break;

        case WXK_DOWN:
            {
                wxMenuItemList::Node *node;
                if ( m_nodeCurrent )
                {
                    node = m_nodeCurrent->GetNext();
                }
                else // no current item
                {
                    node = NULL;
                }

                if ( !node )
                {
                    // if no current item or if it is the last one, select the
                    // first one as Windows does it
                    node = m_menu->GetMenuItems().GetFirst();
                }

                ChangeCurrent(node);
            }
            break;

        case WXK_RIGHT:
            if ( item && item->IsEnabled() && item->IsSubMenu() )
            {
                OpenSubmenu(item);
            }
            else
            {
                processed = FALSE;
            }
            break;

        default:
            // TODO: look for the menu item starting with this letter
            processed = FALSE;
    }

    return processed;
}

// ----------------------------------------------------------------------------
// wxMenu
// ----------------------------------------------------------------------------

void wxMenu::Init()
{
    m_geometry = NULL;

    m_popupMenu = NULL;
    m_isShown = FALSE;
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
        if ( m_popupMenu )
        {
            wxConstCast(this, wxMenu)->m_geometry =
                m_popupMenu->GetRenderer()->GetMenuGeometry(m_popupMenu, *this);
        }
        else
        {
            wxFAIL_MSG( _T("can't get geometry without window") );
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
// wxMenu attaching/detaching
// ----------------------------------------------------------------------------

void wxMenu::Attach(wxMenuBarBase *menubar)
{
    wxMenuBase::Attach(menubar);

    wxCHECK_RET( m_menuBar, _T("menubar can't be NULL after attaching") );

    SetNextHandler(m_menuBar->GetEventHandler());
}

void wxMenu::Detach()
{
    wxMenuBase::Detach();
}

// ----------------------------------------------------------------------------
// wxMenu drawing
// ----------------------------------------------------------------------------

wxRenderer *wxMenu::GetRenderer() const
{
    // we're going to crash without renderer!
    wxCHECK_MSG( m_popupMenu, NULL, _T("neither popup nor menubar menu?") );

    return m_popupMenu->GetRenderer();
}

void wxMenu::RefreshItem(wxMenuItem *item)
{
    // the item geometry changed, so our might have changed as well
    InvalidateGeometryInfo();

    if ( IsShown() )
    {
        // this would be a bug in IsShown()
        wxCHECK_RET( m_popupMenu, _T("must have popup window if shown!") );

        // recalc geometry to update the item height and such
        (void)GetGeometryInfo();

        m_popupMenu->RefreshItem(item);
    }
}

// ----------------------------------------------------------------------------
// wxMenu showing and hiding
// ----------------------------------------------------------------------------

bool wxMenu::IsShown() const
{
    return m_isShown;
}

void wxMenu::OnDismiss(bool dismissParent)
{
    m_isShown = FALSE;

    if ( dismissParent && m_menuParent )
    {
        // dismissParent is recursive
        m_menuParent->Dismiss();
        m_menuParent->OnDismiss(TRUE);
    }

    // notify the menu bar if we're a top level menu
    if ( m_menuBar )
    {
        m_menuBar->OnDismissMenu(dismissParent);
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
        // find the parent window
        wxWindow *parent = m_menuParent ? m_menuParent->m_popupMenu : NULL;
        if ( !parent )
        {
            parent = GetRootWindow();
        }

        wxCHECK_RET( parent, _T("no parent menu, no menu bar, no window??") );

        m_popupMenu = new wxPopupMenuWindow(parent, this);
    }

    // the geometry might have changed since the last time we were shown, so
    // always resize
    m_popupMenu->SetClientSize(GetGeometryInfo().GetSize());

    // position it as specified
    m_popupMenu->Position(pos, size);

    // the menu can't have the focus itself (it is a Windows limitation), so
    // always keep the focus at the originating window
    wxWindow *focus = GetRootWindow();

    wxASSERT_MSG( focus, _T("no window to keep focus on?") );

    // and show it
    m_popupMenu->Popup(focus);

    m_isShown = TRUE;
}

void wxMenu::Dismiss()
{
    wxCHECK_RET( IsShown(), _T("can't dismiss hidden menu") );

    m_popupMenu->Dismiss();
}

// ----------------------------------------------------------------------------
// wxMenu event processing
// ----------------------------------------------------------------------------

bool wxMenu::ProcessKeyDown(int key)
{
    wxCHECK_MSG( m_popupMenu, FALSE,
                 _T("can't process key events if not shown") );

    return m_popupMenu->ProcessKeyDown(key);
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

    m_posY =
    m_height = -1;

    UpdateAccelInfo();
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

void wxMenuItem::UpdateAccelInfo()
{
    m_indexAccel = wxControl::FindAccelIndex(m_text);

    // will be empty if the text contains no TABs - ok
    m_strAccel = m_text.AfterFirst(_T('\t'));
}

void wxMenuItem::SetText(const wxString& text)
{
    if ( text != m_text )
    {
        // first call the base class version to change m_text
        wxMenuItemBase::SetText(text);

        UpdateAccelInfo();

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

    m_shouldShowMenu = FALSE;
}

void wxMenuBar::Attach(wxFrame *frame)
{
    // maybe you really wanted to call Detach()?
    wxCHECK_RET( frame, _T("wxMenuBar::Attach(NULL) called") );

    wxMenuBarBase::Attach(frame);

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

        if ( !IsEnabledTop(n) )
        {
            flags |= wxCONTROL_DISABLED;
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

        event.Skip();
    }
    else // we didn't have mouse capture, capture it now
    {
        m_current = GetMenuFromPoint(event.GetPosition());
        if ( m_current == -1 )
        {
            // don't skip the event so we won't get the focus
            return;
        }

        CaptureMouse();

        // show it as selected
        RefreshItem(m_current);

        // show the menu
        PopupMenu();
    }
}

void wxMenuBar::OnMouseMove(wxMouseEvent& event)
{
    if ( HasCapture() )
    {
        (void)ProcessMouseEvent(event.GetPosition());
    }
    else
    {
        event.Skip();
    }
}

bool wxMenuBar::ProcessMouseEvent(const wxPoint& pt)
{
    // a hack to ignore the extra mouse events MSW sends us: this is similar to
    // wxUSE_MOUSEEVENT_HACK in wxWin itself but it isn't enough for us here as
    // we get the messages from different windows (old and new popup menus for
    // example)
#ifdef __WXMSW__
    static wxPoint s_ptLast;
    if ( pt == s_ptLast )
    {
        return FALSE;
    }

    s_ptLast = pt;
#endif // __WXMSW__

    int currentNew = GetMenuFromPoint(pt);
    if ( (currentNew == -1) || (currentNew == m_current) )
    {
        return FALSE;
    }

    if ( m_current != -1 )
    {
        // close the previous menu
        if ( IsShowingMenu() )
        {
            // restore m_shouldShowMenu flag after DismissMenu() which resets
            // it to FALSE
            bool old = m_shouldShowMenu;

            DismissMenu();

            m_shouldShowMenu = old;
        }

        RefreshItem(m_current);
    }

    m_current = currentNew;

    RefreshItem(m_current);

    // show the menu if we know that we should, even if we hadn't been showing
    // it before (this may happen if the previous menu was disabled)
    if ( m_shouldShowMenu )
    {
        // open the new menu if the old one we closed had been opened
        PopupMenu();
    }

    return TRUE;
}

void wxMenuBar::OnKeyDown(wxKeyEvent& event)
{
    // the current item must have been set before
    wxCHECK_RET( m_current != -1, _T("where is current item?") );

    int key = event.GetKeyCode();

    // first let the menu have it
    if ( IsShowingMenu() && m_menuShown->ProcessKeyDown(key) )
    {
        return;
    }

    // cycle through the menu items when left/right arrows are pressed and open
    // the menu when up/down one is
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

                OnDismiss();
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
        case WXK_RETURN:
            // open the menu
            PopupMenu();
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

    // in any case, we should show it - even if we won't
    m_shouldShowMenu = TRUE;

    if ( IsEnabledTop(m_current) )
    {
        // remember the menu we show
        m_menuShown = GetMenu(m_current);

        // position it correctly: note that we must use screen coords and that we
        // pass 0 as width to position the menu exactly below the item, not to the
        // right of it
        wxRect rectItem = GetItemRect(m_current);
        m_menuShown->Popup(ClientToScreen(rectItem.GetPosition()),
                           wxSize(0, rectItem.GetHeight()));
    }
    //else: don't show disabled menu
}

void wxMenuBar::DismissMenu()
{
    wxCHECK_RET( m_menuShown, _T("can't dismiss menu if none is shown") );

    m_menuShown->Dismiss();
    OnDismissMenu();
}

void wxMenuBar::OnDismissMenu(bool dismissMenuBar)
{
    m_shouldShowMenu = FALSE;
    m_menuShown = NULL;
    if ( dismissMenuBar )
    {
        OnDismiss();
    }
}

void wxMenuBar::OnDismiss()
{
    ReleaseCapture();

    if ( m_current != -1 )
    {
        RefreshItem(m_current);

        m_current = -1;
    }

    if ( !m_focusOld )
    {
        wxCHECK_RET( m_menuBarFrame, _T("menubar without frame?") );

        m_focusOld = m_menuBarFrame;
    }

    m_focusOld->SetFocus();
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

