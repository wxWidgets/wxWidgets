///////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/bar.cpp
// Purpose:     Top-level component of the ribbon-bar-style interface
// Author:      Peter Cawley
// Created:     2009-05-23
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"


#if wxUSE_RIBBON

#include "wx/ribbon/bar.h"
#include "wx/ribbon/art.h"
#include "wx/dcbuffer.h"
#include "wx/app.h"
#include "wx/vector.h"

#if defined(WXWINUI_RIBBON_TOOLKIT)
    #include "ribboncontainerswinui.h"
#endif

#if defined(__WXWINUI__) && wxUSE_WINUI3
    #include "wx/winui/private/tlwhostmsw.h"
    #include "wx/weakref.h"
    #include "ribbonchromewinui.h"
    #include "ribboncommandwinui.h"
    #include <winrt/Microsoft.UI.Input.h>
#endif

#ifndef WX_PRECOMP
#endif

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

#include "wx/imaglist.h"

wxDEFINE_EVENT(wxEVT_RIBBONBAR_PAGE_CHANGED, wxRibbonBarEvent);
wxDEFINE_EVENT(wxEVT_RIBBONBAR_PAGE_CHANGING, wxRibbonBarEvent);
wxDEFINE_EVENT(wxEVT_RIBBONBAR_TAB_MIDDLE_DOWN, wxRibbonBarEvent);
wxDEFINE_EVENT(wxEVT_RIBBONBAR_TAB_MIDDLE_UP, wxRibbonBarEvent);
wxDEFINE_EVENT(wxEVT_RIBBONBAR_TAB_RIGHT_DOWN, wxRibbonBarEvent);
wxDEFINE_EVENT(wxEVT_RIBBONBAR_TAB_RIGHT_UP, wxRibbonBarEvent);
wxDEFINE_EVENT(wxEVT_RIBBONBAR_TAB_LEFT_DCLICK, wxRibbonBarEvent);
wxDEFINE_EVENT(wxEVT_RIBBONBAR_TOGGLED, wxRibbonBarEvent);
wxDEFINE_EVENT(wxEVT_RIBBONBAR_HELP_CLICK, wxRibbonBarEvent);

wxIMPLEMENT_CLASS(wxRibbonBar, wxRibbonControl);
wxIMPLEMENT_DYNAMIC_CLASS(wxRibbonBarEvent, wxNotifyEvent);

wxBEGIN_EVENT_TABLE(wxRibbonBar, wxRibbonControl)
  EVT_ERASE_BACKGROUND(wxRibbonBar::OnEraseBackground)
  EVT_LEAVE_WINDOW(wxRibbonBar::OnMouseLeave)
  EVT_LEFT_DOWN(wxRibbonBar::OnMouseLeftDown)
  EVT_LEFT_UP(wxRibbonBar::OnMouseLeftUp)
  EVT_MIDDLE_DOWN(wxRibbonBar::OnMouseMiddleDown)
  EVT_MIDDLE_UP(wxRibbonBar::OnMouseMiddleUp)
  EVT_MOTION(wxRibbonBar::OnMouseMove)
  EVT_PAINT(wxRibbonBar::OnPaint)
  EVT_RIGHT_DOWN(wxRibbonBar::OnMouseRightDown)
  EVT_RIGHT_UP(wxRibbonBar::OnMouseRightUp)
  EVT_LEFT_DCLICK(wxRibbonBar::OnMouseDoubleClick)
  EVT_SIZE(wxRibbonBar::OnSize)
  EVT_KILL_FOCUS(wxRibbonBar::OnKillFocus)
  EVT_DPI_CHANGED(wxRibbonBar::OnDPIChanged)
  EVT_SYS_COLOUR_CHANGED(wxRibbonBar::OnSysColourChanged)
wxEND_EVENT_TABLE()

void wxRibbonBar::AddPage(wxRibbonPage *page)
{
    wxRibbonPageTabInfo info;

    info.page = page;
    info.active = false;
    info.hovered = false;
    info.highlight = false;
    info.shown = true;
    // info.rect not set (intentional)

    wxInfoDC dcTemp(this);
    wxString label;
    if(m_flags & wxRIBBON_BAR_SHOW_PAGE_LABELS)
        label = page->GetLabel();
    wxBitmap icon;
    if(m_flags & wxRIBBON_BAR_SHOW_PAGE_ICONS)
        icon = page->GetIcon();
    m_art->GetBarTabWidth(dcTemp, this, label, icon,
                          &info.ideal_width,
                          &info.small_begin_need_separator_width,
                          &info.small_must_have_separator_width,
                          &info.minimum_width);

    if(m_pages.IsEmpty())
    {
        m_tabs_total_width_ideal = info.ideal_width;
        m_tabs_total_width_minimum = info.minimum_width;
    }
    else
    {
        int sep = m_art->GetMetric(wxRIBBON_ART_TAB_SEPARATION_SIZE);
        m_tabs_total_width_ideal += sep + info.ideal_width;
        m_tabs_total_width_minimum += sep + info.minimum_width;
    }
    m_pages.Add(info);

    page->Hide(); // Most likely case is that this new page is not the active tab
    page->SetArtProvider(m_art);

    if(m_pages.GetCount() == 1)
    {
        SetActivePage((size_t)0);
    }
#if defined(WXWINUI_RIBBON_TOOLKIT)
    if ( wxRibbonContainerWinUI::Native(this) )
        wxRibbonContainerWinUI::Sync(this);
#endif
}

bool wxRibbonBar::DismissExpandedPanel()
{
    if(m_current_page == wxNOT_FOUND)
        return false;
    return m_pages.Item(m_current_page).page->DismissExpandedPanel();
}


void wxRibbonBar::ShowPanels(wxRibbonDisplayMode mode)
{
    switch ( mode )
    {
        case wxRIBBON_BAR_PINNED:
        case wxRIBBON_BAR_EXPANDED:
            m_arePanelsShown = true;
            break;

        case wxRIBBON_BAR_MINIMIZED:
            m_arePanelsShown = false;
            break;
    }

#if defined(WXWINUI_RIBBON_TOOLKIT)
    if ( wxRibbonContainerWinUI::Native(this) )
    {
        m_ribbon_state = mode;
        InvalidateBestSize();
        wxRibbonContainerWinUI::Sync(this);
        return;
    }
#endif

    SetMinSize(wxSize(GetSize().GetWidth(), DoGetBestSize().GetHeight()));
    Realise();
    GetParent()->Layout();

    // See RepositionPage(): nothing above gives the bar of a ribbon whose
    // groups are stacked another size, which is what folds the others.
    if ( (m_flags & wxRIBBON_BAR_FLOW_VERTICAL) &&
         m_current_page != wxNOT_FOUND )
    {
        RepositionPage(m_pages.Item(m_current_page).page);
    }

    m_ribbon_state = mode;
}


void wxRibbonBar::ShowPanels(bool show)
{
    ShowPanels( show ? wxRIBBON_BAR_PINNED : wxRIBBON_BAR_MINIMIZED );
}

void wxRibbonBar::SetWindowStyleFlag(long style)
{
    m_flags = style;
    if(m_art)
        m_art->SetFlags(style);
#if defined(WXWINUI_RIBBON_TOOLKIT)
    if ( wxRibbonContainerWinUI::Native(this) )
        wxRibbonContainerWinUI::Sync(this);
#endif
}

long wxRibbonBar::GetWindowStyleFlag() const
{
    return m_flags;
}

bool wxRibbonBar::Realize()
{
#if defined(WXWINUI_RIBBON_TOOLKIT)
    if ( wxRibbonContainerWinUI::Native(this) )
        return wxRibbonContainerWinUI::Realize(this);
#endif

    bool status = true;

    wxInfoDC dcTemp(this);
    int sep = m_art->GetMetric(wxRIBBON_ART_TAB_SEPARATION_SIZE);
    size_t numtabs = m_pages.GetCount();
    bool firstVisible = true;
    size_t i;
    for(i = 0; i < numtabs; ++i)
    {
        wxRibbonPageTabInfo& info = m_pages.Item(i);
        if (!info.shown)
            continue;
        RepositionPage(info.page);
        if(!info.page->Realize())
        {
            status = false;
        }
        wxString label;
        if(m_flags & wxRIBBON_BAR_SHOW_PAGE_LABELS)
            label = info.page->GetLabel();
        wxBitmap icon;
        if(m_flags & wxRIBBON_BAR_SHOW_PAGE_ICONS)
            icon = info.page->GetIcon();
        m_art->GetBarTabWidth(dcTemp, this, label, icon,
                              &info.ideal_width,
                              &info.small_begin_need_separator_width,
                              &info.small_must_have_separator_width,
                              &info.minimum_width);

        if ( firstVisible )
        {
            firstVisible = false;

            m_tabs_total_width_ideal = info.ideal_width;
            m_tabs_total_width_minimum = info.minimum_width;
        }
        else
        {
            m_tabs_total_width_ideal += sep + info.ideal_width;
            m_tabs_total_width_minimum += sep + info.minimum_width;
        }
    }
    const int tabHeightBefore = m_tab_height;
    m_tab_height = m_art->GetTabCtrlHeight(dcTemp, this, m_pages);

    RecalculateMinSize();
    RecalculateTabSizes();

    // The page lies under the tabs: it follows them when their height
    // changes without the bar changing size -- another art provider with
    // other metrics, tabs shown or hidden. It used to keep its place until
    // the next resize of the bar.
    // (While pages are being removed the current one may not be among those
    // left yet.)
    if ( m_tab_height != tabHeightBefore && m_current_page != wxNOT_FOUND &&
         static_cast<size_t>(m_current_page) < m_pages.GetCount() )
    {
        RepositionPage(m_pages.Item(m_current_page).page);
    }

    Refresh();

    return status;
}

void wxRibbonBar::OnMouseMove(wxMouseEvent& evt)
{
    int x = evt.GetX();
    int y = evt.GetY();
    int hovered_page = wxNOT_FOUND;
    bool refresh_tabs = false;
    if(y < m_tab_height)
    {
        // It is quite likely that the mouse moved a small amount and is still over the same tab
        if(m_current_hovered_page != wxNOT_FOUND && m_pages.Item((size_t)m_current_hovered_page).rect.Contains(x, y))
        {
            hovered_page = m_current_hovered_page;
            // But be careful, if tabs can be scrolled, then parts of the tab rect may not be valid
            if(m_tab_scroll_buttons_shown)
            {
                if(x >= m_tab_scroll_right_button_rect.GetX() || x < m_tab_scroll_left_button_rect.GetRight())
                {
                    hovered_page = wxNOT_FOUND;
                }
            }
        }
        else
        {
            HitTestTabs(evt.GetPosition(), &hovered_page);
        }
    }
    if(hovered_page != m_current_hovered_page)
    {
        if(m_current_hovered_page != wxNOT_FOUND)
        {
            m_pages.Item((int)m_current_hovered_page).hovered = false;
        }
        m_current_hovered_page = hovered_page;
        if(m_current_hovered_page != wxNOT_FOUND)
        {
            m_pages.Item((int)m_current_hovered_page).hovered = true;
        }
        refresh_tabs = true;
    }
    if(m_tab_scroll_buttons_shown)
    {
#define SET_FLAG(variable, flag) \
    { if(((variable) & (flag)) != (flag)) { variable |= (flag); refresh_tabs = true; }}
#define UNSET_FLAG(variable, flag) \
    { if((variable) & (flag)) { variable &= ~(flag); refresh_tabs = true; }}

        if(m_tab_scroll_left_button_rect.Contains(x, y))
            SET_FLAG(m_tab_scroll_left_button_state, wxRIBBON_SCROLL_BTN_HOVERED)
        else
            UNSET_FLAG(m_tab_scroll_left_button_state, wxRIBBON_SCROLL_BTN_HOVERED)

        if(m_tab_scroll_right_button_rect.Contains(x, y))
            SET_FLAG(m_tab_scroll_right_button_state, wxRIBBON_SCROLL_BTN_HOVERED)
        else
            UNSET_FLAG(m_tab_scroll_right_button_state, wxRIBBON_SCROLL_BTN_HOVERED)
#undef SET_FLAG
#undef UNSET_FLAG
    }
    if(refresh_tabs)
    {
        RefreshTabBar();
    }
    if ( m_flags & wxRIBBON_BAR_SHOW_TOGGLE_BUTTON )
        HitTestRibbonButton(m_toggle_button_rect, evt.GetPosition(), m_toggle_button_hovered);
    if ( m_flags & wxRIBBON_BAR_SHOW_HELP_BUTTON )
        HitTestRibbonButton(m_help_button_rect, evt.GetPosition(), m_help_button_hovered);
}

void wxRibbonBar::OnMouseLeave(wxMouseEvent& WXUNUSED(evt))
{
    // The ribbon bar is (usually) at the top of a window, and at least on MSW, the mouse
    // can leave the window quickly and leave a tab in the hovered state.
    bool refresh_tabs = false;
    if(m_current_hovered_page != wxNOT_FOUND)
    {
        m_pages.Item((int)m_current_hovered_page).hovered = false;
        m_current_hovered_page = wxNOT_FOUND;
        refresh_tabs = true;
    }
    if(m_tab_scroll_left_button_state & wxRIBBON_SCROLL_BTN_HOVERED)
    {
        m_tab_scroll_left_button_state &= ~wxRIBBON_SCROLL_BTN_HOVERED;
        refresh_tabs = true;
    }
    if(m_tab_scroll_right_button_state & wxRIBBON_SCROLL_BTN_HOVERED)
    {
        m_tab_scroll_right_button_state &= ~wxRIBBON_SCROLL_BTN_HOVERED;
        refresh_tabs = true;
    }
    if(refresh_tabs)
    {
        RefreshTabBar();
    }
    if(m_toggle_button_hovered)
    {
        m_bar_hovered = false;
        m_toggle_button_hovered = false;
        Refresh(false);
    }
    if ( m_help_button_hovered )
    {
        m_help_button_hovered = false;
        m_bar_hovered = false;
        Refresh(false);
    }
}

wxRibbonPage* wxRibbonBar::GetPage(int n)
{
    if(n < 0 || (size_t)n >= m_pages.GetCount())
        return nullptr;
    return m_pages.Item(n).page;
}

wxRibbonPage* wxRibbonBar::GetPageById(wxWindowID id)
{
    for (const auto& page : m_pages)
    {
        if (page.page->GetId() == id)
            return page.page;
    }
    return nullptr;
}

size_t wxRibbonBar::GetPageCount() const
{
    return m_pages.GetCount();
}

bool wxRibbonBar::IsPageShown(size_t page) const
{
    if (page >= m_pages.GetCount())
        return false;
    return m_pages.Item(page).shown;
}

void wxRibbonBar::ShowPage(size_t page, bool show)
{
    if(page >= m_pages.GetCount())
        return;
    m_pages.Item(page).shown = show;
#if defined(WXWINUI_RIBBON_TOOLKIT)
    if ( wxRibbonContainerWinUI::Native(this) )
        wxRibbonContainerWinUI::Sync(this);
#endif
}

bool wxRibbonBar::IsPageHighlighted(size_t page) const
{
    if (page >= m_pages.GetCount())
        return false;
    return m_pages.Item(page).highlight;
}

void wxRibbonBar::AddPageHighlight(size_t page, bool highlight)
{
    if(page >= m_pages.GetCount())
        return;
    m_pages.Item(page).highlight = highlight;
#if defined(WXWINUI_RIBBON_TOOLKIT)
    if ( wxRibbonContainerWinUI::Native(this) )
        wxRibbonContainerWinUI::Sync(this);
#endif
}

void wxRibbonBar::DeletePage(size_t n)
{
    if(n < m_pages.GetCount())
    {
        wxRibbonPage *page = m_pages.Item(n).page;

        // Schedule page object for destruction and not destroying directly
        // as this function can be called in an event handler and page functions
        // can be called after removing.
        // Like in wxRibbonButtonBar::OnMouseUp
        if(!wxTheApp->IsScheduledForDestruction(page))
        {
            wxTheApp->ScheduleForDestruction(page);
        }

        m_pages.RemoveAt(n);

        if(m_current_page == static_cast<int>(n))
        {
            m_current_page = wxNOT_FOUND;

            if(m_pages.GetCount() > 0)
                SetActivePage(wxMin(n, m_pages.GetCount() - 1));
        }
        else if(m_current_page > static_cast<int>(n))
        {
            m_current_page--;
        }

        // If the current hovered page is the one getting deleted, then we
        // don't have a hovered page anymore.
        if ( m_current_hovered_page == static_cast<int>(n) )
        {
            m_current_hovered_page = wxNOT_FOUND;
        }
        // ...otherwise, the pages after it shifted down by one,
        // so adjust the index to keep referring to the same (still hovered) page.
        else if ( m_current_hovered_page > static_cast<int>(n) )
        {
            m_current_hovered_page--;
        }
#if defined(WXWINUI_RIBBON_TOOLKIT)
        if ( wxRibbonContainerWinUI::Native(this) )
            wxRibbonContainerWinUI::Sync(this);
#endif
    }
}

void wxRibbonBar::ClearPages()
{
    size_t i;
    for(i=0; i<m_pages.GetCount(); i++)
    {
        wxRibbonPage *page = m_pages.Item(i).page;
        // Schedule page object for destruction and not destroying directly
        // as this function can be called in an event handler and page functions
        // can be called after removing.
        // Like in wxRibbonButtonBar::OnMouseUp
        if(!wxTheApp->IsScheduledForDestruction(page))
        {
            wxTheApp->ScheduleForDestruction(page);
        }
    }
    m_pages.Empty();
    Realize();
    m_current_page = wxNOT_FOUND;
    m_current_hovered_page = wxNOT_FOUND;
    Refresh();
#if defined(WXWINUI_RIBBON_TOOLKIT)
    if ( wxRibbonContainerWinUI::Native(this) )
        wxRibbonContainerWinUI::Sync(this);
#endif
}

bool wxRibbonBar::SetActivePage(size_t page)
{
    if(m_current_page == (int)page)
    {
        return true;
    }

    if(page >= m_pages.GetCount())
    {
        return false;
    }

    if(m_current_page != wxNOT_FOUND)
    {
        m_pages.Item((size_t)m_current_page).active = false;
        m_pages.Item((size_t)m_current_page).page->Hide();
    }
    m_current_page = (int)page;
    m_pages.Item(page).active = true;
    m_pages.Item(page).shown = true;
#if defined(WXWINUI_RIBBON_TOOLKIT)
    if ( wxRibbonContainerWinUI::Native(this) )
    {
        // Programmatic selection remains silent: only the native user-input
        // adapter emits PAGE_CHANGING/PAGE_CHANGED.
        m_pages.Item(page).page->Show();
        Refresh();
        wxRibbonContainerWinUI::Sync(this);
        return true;
    }
#endif
    {
        wxRibbonPage* wnd = m_pages.Item(page).page;
        RepositionPage(wnd);
        wnd->Layout();
        wnd->Show();
    }
    Refresh();

    return true;
}

bool wxRibbonBar::SetActivePage(wxRibbonPage* page)
{
    size_t numpages = m_pages.GetCount();
    size_t i;
    for(i = 0; i < numpages; ++i)
    {
        if(m_pages.Item(i).page == page)
        {
            return SetActivePage(i);
        }
    }
    return false;
}

int wxRibbonBar::GetPageNumber(wxRibbonPage* page) const
{
    size_t numpages = m_pages.GetCount();
    for(size_t i = 0; i < numpages; ++i)
    {
        if(m_pages.Item(i).page == page)
        {
            return i;
        }
    }
    return wxNOT_FOUND;
}


int wxRibbonBar::GetActivePage() const
{
    return m_current_page;
}

void wxRibbonBar::SetTabCtrlMargins(int left, int right)
{
    m_tab_margin_left = left;
    m_tab_margin_right = right;

#if defined(WXWINUI_RIBBON_TOOLKIT)
    if ( wxRibbonContainerWinUI::Native(this) )
    {
        wxRibbonContainerWinUI::SetTabMargins(this, left, right);
        return;
    }
#endif

    RecalculateTabSizes();
}

struct PageComparedBySmallWidthAsc
{
    explicit PageComparedBySmallWidthAsc(wxRibbonPageTabInfo* page)
        : m_page(page)
    {
    }

    bool operator<(const PageComparedBySmallWidthAsc& other) const
    {
        return m_page->small_must_have_separator_width
                < other.m_page->small_must_have_separator_width;
    }

    wxRibbonPageTabInfo *m_page;
};

void wxRibbonBar::RecalculateTabSizes()
{
    size_t numtabs = m_pages.GetCount();

    if(numtabs == 0)
        return;

    int width = GetSize().GetWidth() - m_tab_margin_left - m_tab_margin_right;
    int tabsep = m_art->GetMetric(wxRIBBON_ART_TAB_SEPARATION_SIZE);
    int x = m_tab_margin_left;
    const int y = 0;

    if(width >= m_tabs_total_width_ideal)
    {
        // Simple case: everything at ideal width
        size_t i;
        for(i = 0; i < numtabs; ++i)
        {
            wxRibbonPageTabInfo& info = m_pages.Item(i);
            if (!info.shown)
                continue;
            info.rect.x = x;
            info.rect.y = y;
            info.rect.width = info.ideal_width;
            info.rect.height = m_tab_height;
            x += info.rect.width + tabsep;
        }
        m_tab_scroll_buttons_shown = false;
        m_tab_scroll_left_button_rect.SetWidth(0);
        m_tab_scroll_right_button_rect.SetWidth(0);
    }
    else if(width < m_tabs_total_width_minimum)
    {
        // Simple case: everything minimum with scrollbar
        size_t i;
        for(i = 0; i < numtabs; ++i)
        {
            wxRibbonPageTabInfo& info = m_pages.Item(i);
            if (!info.shown)
                continue;
            info.rect.x = x;
            info.rect.y = y;
            info.rect.width = info.minimum_width;
            info.rect.height = m_tab_height;
            x += info.rect.width + tabsep;
        }
        if(!m_tab_scroll_buttons_shown)
        {
            m_tab_scroll_left_button_state = wxRIBBON_SCROLL_BTN_NORMAL;
            m_tab_scroll_right_button_state = wxRIBBON_SCROLL_BTN_NORMAL;
            m_tab_scroll_buttons_shown = true;
        }
        {
            wxInfoDC temp_dc(this);
            int right_button_pos = GetClientSize().GetWidth() - m_tab_margin_right - m_tab_scroll_right_button_rect.GetWidth();
            if ( right_button_pos < m_tab_margin_left )
                right_button_pos = m_tab_margin_left;

            m_tab_scroll_left_button_rect.SetWidth(m_art->GetScrollButtonMinimumSize(temp_dc, this, wxRIBBON_SCROLL_BTN_LEFT | wxRIBBON_SCROLL_BTN_NORMAL | wxRIBBON_SCROLL_BTN_FOR_TABS).GetWidth());
            m_tab_scroll_left_button_rect.SetHeight(m_tab_height);
            m_tab_scroll_left_button_rect.SetX(m_tab_margin_left);
            m_tab_scroll_left_button_rect.SetY(0);
            m_tab_scroll_right_button_rect.SetWidth(m_art->GetScrollButtonMinimumSize(temp_dc, this, wxRIBBON_SCROLL_BTN_RIGHT | wxRIBBON_SCROLL_BTN_NORMAL | wxRIBBON_SCROLL_BTN_FOR_TABS).GetWidth());
            m_tab_scroll_right_button_rect.SetHeight(m_tab_height);
            m_tab_scroll_right_button_rect.SetX(right_button_pos);
            m_tab_scroll_right_button_rect.SetY(0);
        }
        if(m_tab_scroll_amount == 0)
        {
            m_tab_scroll_left_button_rect.SetWidth(0);
        }
        else if(m_tab_scroll_amount + width >= m_tabs_total_width_minimum)
        {
            m_tab_scroll_amount = m_tabs_total_width_minimum - width;
            m_tab_scroll_right_button_rect.SetX(m_tab_scroll_right_button_rect.GetX() + m_tab_scroll_right_button_rect.GetWidth());
            m_tab_scroll_right_button_rect.SetWidth(0);
        }
        for(i = 0; i < numtabs; ++i)
        {
            wxRibbonPageTabInfo& info = m_pages.Item(i);
            if (!info.shown)
                continue;
            info.rect.x -= m_tab_scroll_amount;
        }
    }
    else
    {
        m_tab_scroll_buttons_shown = false;
        m_tab_scroll_left_button_rect.SetWidth(0);
        m_tab_scroll_right_button_rect.SetWidth(0);
        // Complex case: everything sized such that: minimum <= width < ideal
        /*
           Strategy:
             1) Uniformly reduce all tab widths from ideal to small_must_have_separator_width
             2) Reduce the largest tab by 1 pixel, repeating until all tabs are same width (or at minimum)
             3) Uniformly reduce all tabs down to their minimum width
        */
        int smallest_tab_width = INT_MAX;
        int total_small_width = tabsep * (numtabs - 1);
        size_t i;
        for(i = 0; i < numtabs; ++i)
        {
            wxRibbonPageTabInfo& info = m_pages.Item(i);
            if (!info.shown)
                continue;
            if(info.small_must_have_separator_width < smallest_tab_width)
            {
                smallest_tab_width = info.small_must_have_separator_width;
            }
            total_small_width += info.small_must_have_separator_width;
        }
        if(width >= total_small_width)
        {
            // Do (1)
            int total_delta = m_tabs_total_width_ideal - total_small_width;
            total_small_width -= tabsep * (numtabs - 1);
            width -= tabsep * (numtabs - 1);
            for(i = 0; i < numtabs; ++i)
            {
                wxRibbonPageTabInfo& info = m_pages.Item(i);
                if (!info.shown)
                    continue;
                int delta = info.ideal_width - info.small_must_have_separator_width;
                info.rect.x = x;
                info.rect.y = y;
                info.rect.width = info.small_must_have_separator_width;
                if ( total_delta != 0 )
                    info.rect.width += delta * (width - total_small_width) / total_delta;
                info.rect.height = m_tab_height;

                x += info.rect.width + tabsep;
                total_delta -= delta;
                total_small_width -= info.small_must_have_separator_width;
                width -= info.rect.width;
            }
        }
        else
        {
            total_small_width = tabsep * (numtabs - 1);
            for(i = 0; i < numtabs; ++i)
            {
                wxRibbonPageTabInfo& info = m_pages.Item(i);
                if (!info.shown)
                    continue;
                if(info.minimum_width < smallest_tab_width)
                {
                    total_small_width += smallest_tab_width;
                }
                else
                {
                    total_small_width += info.minimum_width;
                }
            }
            if(width >= total_small_width)
            {
                // Do (2)
                wxVector<PageComparedBySmallWidthAsc> sorted_pages;
                sorted_pages.reserve(numtabs);
                for ( i = 0; i < numtabs; ++i )
                    sorted_pages.push_back(PageComparedBySmallWidthAsc(&m_pages.Item(i)));

                wxVectorSort(sorted_pages);
                width -= tabsep * (numtabs - 1);
                for(i = 0; i < numtabs; ++i)
                {
                    wxRibbonPageTabInfo* info = sorted_pages[i].m_page;
                    if (!info->shown)
                        continue;
                    if(info->small_must_have_separator_width * (int)(numtabs - i) <= width)
                    {
                        info->rect.width = info->small_must_have_separator_width;
                    }
                    else
                    {
                        info->rect.width = width / (numtabs - i);
                    }
                    width -= info->rect.width;
                }
                for(i = 0; i < numtabs; ++i)
                {
                    wxRibbonPageTabInfo& info = m_pages.Item(i);
                    if (!info.shown)
                        continue;
                    info.rect.x = x;
                    info.rect.y = y;
                    info.rect.height = m_tab_height;
                    x += info.rect.width + tabsep;
                }
            }
            else
            {
                // Do (3)
                total_small_width = (smallest_tab_width + tabsep) * numtabs - tabsep;
                int total_delta = total_small_width - m_tabs_total_width_minimum;
                total_small_width = m_tabs_total_width_minimum - tabsep * (numtabs - 1);
                width -= tabsep * (numtabs - 1);
                for(i = 0; i < numtabs; ++i)
                {
                    wxRibbonPageTabInfo& info = m_pages.Item(i);
                    if (!info.shown)
                        continue;
                    int delta = smallest_tab_width - info.minimum_width;
                    info.rect.x = x;
                    info.rect.y = y;
                    info.rect.width = info.minimum_width;
                    if( total_delta != 0 )
                        info.rect.width += delta * (width - total_small_width) / total_delta;
                    info.rect.height = m_tab_height;

                    x += info.rect.width + tabsep;
                    total_delta -= delta;
                    total_small_width -= info.minimum_width;
                    width -= info.rect.width;
                }
            }
        }
    }
}

wxRibbonBar::wxRibbonBar() = default;

wxRibbonBar::wxRibbonBar(wxWindow* parent,
                         wxWindowID id,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style)
    : wxRibbonControl(parent, id, pos, size, wxBORDER_NONE)
{
    CommonInit(style);
#if defined(WXWINUI_RIBBON_TOOLKIT)
    wxCHECK_RET(wxRibbonContainerWinUI::Create(this),
                "Failed to create native Toolkit Ribbon bar");
    wxRibbonContainerWinUI::SetTabMargins(this, m_tab_margin_left, m_tab_margin_right);
#endif
}

#if defined(__WXWINUI__) && wxUSE_WINUI3

// ----------------------------------------------------------------------------
// tabs shown by a SelectorBar
// ----------------------------------------------------------------------------

namespace
{

// One of the small buttons after the tabs: a subtle button holding a glyph
// of the icon font of Windows.
winrt::Microsoft::UI::Xaml::Controls::Button
wxRibbonBarMakeButton(const wchar_t *glyph, const wxString& name)
{
    namespace X = winrt::Microsoft::UI::Xaml;

    const auto button =
        wxRibbonWinUICommandDetail::LoadSubtle(L"Button", L"Button")
            .as<X::Controls::Button>();
    X::Controls::FontIcon icon;
    icon.Glyph(glyph);
    icon.FontSize(12);
    button.Content(icon);
    button.Width(32);
    button.Height(32);
    button.Padding(X::ThicknessHelper::FromUniformLength(0));
    X::Automation::AutomationProperties::SetName(
        button, wxWinUIToHString(name));
    return button;
}

} // anonymous namespace

void wxRibbonBar::WinUIChooseTab(size_t index)
{
    if ( index >= m_pages.GetCount() )
        return;

    // What a click on a tab does, see OnMouseLeftDown().
    wxRibbonPageTabInfo * const tab = &m_pages.Item(index);
    const bool current =
        m_current_page >= 0 && static_cast<size_t>(m_current_page) == index;
    if ( m_ribbon_state == wxRIBBON_BAR_MINIMIZED )
        ShowPanels(wxRIBBON_BAR_EXPANDED);
    else if ( current && m_ribbon_state == wxRIBBON_BAR_EXPANDED )
        HidePanels();

    if ( !current )
    {
        wxRibbonBarEvent query(wxEVT_RIBBONBAR_PAGE_CHANGING, GetId(), tab->page);
        query.SetEventObject(this);
        ProcessWindowEvent(query);
        if ( query.IsAllowed() )
        {
            SetActivePage(query.GetPage());

            wxRibbonBarEvent notification(wxEVT_RIBBONBAR_PAGE_CHANGED, GetId(),
                                          m_pages.Item(m_current_page).page);
            notification.SetEventObject(this);
            ProcessWindowEvent(notification);
        }
    }

    // The selector goes back to the page which is shown if the change was
    // refused.
    WinUISyncTabs();
}

void wxRibbonBar::WinUIToggle()
{
    // A ribbon which is folded and shown for a moment shows a pin there:
    // it is kept unfolded, not folded again.
    if ( m_ribbon_state == wxRIBBON_BAR_EXPANDED )
        ShowPanels(wxRIBBON_BAR_PINNED);
    else
        ShowPanels(ArePanelsShown() ? wxRIBBON_BAR_MINIMIZED : wxRIBBON_BAR_PINNED);
    wxRibbonBarEvent event(wxEVT_RIBBONBAR_TOGGLED, GetId());
    event.SetEventObject(this);
    ProcessWindowEvent(event);
    WinUISyncTabs();
}

void wxRibbonBar::WinUIHelp()
{
    wxRibbonBarEvent event(wxEVT_RIBBONBAR_HELP_CLICK, GetId());
    event.SetEventObject(this);
    ProcessWindowEvent(event);
}

void wxRibbonBar::WinUISyncTabs()
{
    if ( !m_winuiChrome )
        return;

    namespace X = winrt::Microsoft::UI::Xaml;
    namespace C = X::Controls;

    wxRibbonChromeWinUI& chrome = *m_winuiChrome;
    if ( chrome.syncing )
        return;
    chrome.syncing = true;
    struct Done
    {
        wxRibbonChromeWinUI& chrome;
        ~Done() { chrome.syncing = false; }
    } done{ chrome };

    try
    {
        const auto strip = chrome.Find<C::Grid>(L"Strip");
        const auto tabs = chrome.Find<C::SelectorBar>(L"Tabs");
        const auto buttons = chrome.Find<C::StackPanel>(L"Buttons");
        if ( !strip || !tabs || !buttons )
            return;

        // The room the layout of the bar keeps for the tabs: none when they
        // are not shown at all.
        const int height = m_tab_height > 2 ? ToDIP(m_tab_height) : 0;
        if ( height != chrome.extent )
        {
            chrome.extent = height;
            strip.Visibility(height ? X::Visibility::Visible
                                    : X::Visibility::Collapsed);
            if ( height )
                strip.Height(height);
        }

        // The tabs and the buttons are made again when what they are
        // changes, which is rare; choosing a tab is not such a change.
        wxString signature = wxString::Format("%ld", m_flags);
        for ( size_t n = 0; n < m_pages.GetCount(); ++n )
        {
            const wxRibbonPageTabInfo& info = m_pages.Item(n);
            signature << '\x1f' << (info.shown ? '1' : '0')
                      << (info.page ? info.page->GetLabel() : wxString());
        }
        if ( signature != chrome.label )
        {
            chrome.label = signature;
            chrome.state = -1;

            const wxWeakRef<wxRibbonBar> weak(this);

            // A click on the tab which is already chosen changes no
            // selection, and still means something to a ribbon which is
            // folded: it is told apart from the click which has just
            // chosen the tab by the time between the two.
            const auto tapped = [weak](size_t n)
            {
                wxRibbonBar * const bar = weak.get();
                if ( !bar || !bar->m_winuiChrome ||
                     bar->m_winuiChrome->syncing ||
                     ::GetTickCount64() - bar->m_winuiChrome->chosen < 400 )
                {
                    return;
                }
                if ( bar->m_current_page >= 0 &&
                     static_cast<size_t>(bar->m_current_page) == n )
                {
                    bar->WinUIChooseTab(n);
                }
            };
            // A selection is shown only if the item had its template when
            // it was made: it is made again when the item is loaded.
            const auto loaded = [weak](size_t n)
            {
                wxRibbonBar * const bar = weak.get();
                if ( bar && bar->m_winuiChrome && bar->m_current_page >= 0 &&
                     static_cast<size_t>(bar->m_current_page) == n )
                {
                    bar->m_winuiChrome->state = -1;
                    bar->WinUISyncTabs();
                }
            };

            // The tabs are those of a SelectorBar, or of a TabView if the
            // application asked for one (wxRIBBON_BAR_TAB_VIEW).
            const bool asTabView = (m_flags & wxRIBBON_BAR_TAB_VIEW) != 0;
            C::TabView tabView{ nullptr };
            for ( const auto& child : strip.Children() )
            {
                if ( (tabView = child.try_as<C::TabView>()) )
                    break;
            }
            if ( asTabView && !tabView )
            {
                tabView = C::TabView();
                tabView.IsAddTabButtonVisible(false);
                tabView.CanDragTabs(false);
                tabView.CanReorderTabs(false);
                tabView.TabWidthMode(C::TabViewWidthMode::SizeToContent);
                tabView.VerticalAlignment(X::VerticalAlignment::Bottom);
                tabView.SelectionChanged([weak](const auto& sender, const auto&)
                {
                    wxRibbonBar * const bar = weak.get();
                    const auto view = sender.try_as<C::TabView>();
                    if ( !bar || !view || !bar->m_winuiChrome ||
                         bar->m_winuiChrome->syncing ||
                         view.SelectedIndex() < 0 )
                    {
                        return;
                    }
                    bar->m_winuiChrome->chosen = ::GetTickCount64();
                    bar->WinUIChooseTab(static_cast<size_t>(view.SelectedIndex()));
                });
                strip.Children().Append(tabView);
            }
            if ( tabView )
            {
                tabView.TabItems().Clear();
                tabView.Visibility(asTabView ? X::Visibility::Visible
                                             : X::Visibility::Collapsed);
            }
            if ( const auto scroll = chrome.Find<C::ScrollViewer>(L"TabScroll") )
            {
                scroll.Visibility(asTabView ? X::Visibility::Collapsed
                                            : X::Visibility::Visible);
            }

            tabs.Items().Clear();
            for ( size_t n = 0; n < m_pages.GetCount(); ++n )
            {
                const wxRibbonPageTabInfo& info = m_pages.Item(n);

                wxString label;
                wxBitmap icon;
                if ( info.page )
                {
                    if ( m_flags & wxRIBBON_BAR_SHOW_PAGE_LABELS )
                        label = info.page->GetLabel();
                    if ( m_flags & wxRIBBON_BAR_SHOW_PAGE_ICONS )
                        icon = info.page->GetIcon();
                }

                X::FrameworkElement element{ nullptr };
                if ( asTabView )
                {
                    C::TabViewItem item;
                    item.Header(winrt::box_value(wxWinUIToHString(label)));
                    item.IsClosable(false);
                    if ( icon.IsOk() )
                    {
                        C::ImageIconSource source;
                        source.ImageSource(wxWinUIWriteableBitmapFromBitmap(icon));
                        item.IconSource(source);
                    }
                    tabView.TabItems().Append(item);
                    element = item;
                }
                else
                {
                    C::SelectorBarItem item;
                    item.Text(wxWinUIToHString(label));
                    if ( icon.IsOk() )
                    {
                        C::ImageIcon image;
                        image.Source(wxWinUIWriteableBitmapFromBitmap(icon));
                        item.Icon(image);
                    }
                    tabs.Items().Append(item);
                    element = item;
                }

                if ( info.page )
                {
                    X::Automation::AutomationProperties::SetName(
                        element, wxWinUIToHString(info.page->GetLabel()));
                }
                if ( !info.shown )
                    element.Visibility(X::Visibility::Collapsed);
                element.Tapped([tapped, n](const auto&, const auto&) { tapped(n); });
                element.Loaded([loaded, n](const auto&, const auto&) { loaded(n); });
            }

            buttons.Children().Clear();
            if ( m_flags & wxRIBBON_BAR_SHOW_TOGGLE_BUTTON )
            {
                const auto toggle =
                    wxRibbonBarMakeButton(L"\uE70E", _("Show or hide the ribbon"));
                toggle.Name(L"Toggle");
                toggle.Click([weak](const auto&, const auto&)
                {
                    if ( wxRibbonBar * const bar = weak.get() )
                        bar->WinUIToggle();
                });
                buttons.Children().Append(toggle);
            }
            if ( m_flags & wxRIBBON_BAR_SHOW_HELP_BUTTON )
            {
                const auto help = wxRibbonBarMakeButton(L"\uE897", _("Help"));
                help.Click([weak](const auto&, const auto&)
                {
                    if ( wxRibbonBar * const bar = weak.get() )
                        bar->WinUIHelp();
                });
                buttons.Children().Append(help);
            }
        }

        const int state = (m_current_page + 1) * 4 + static_cast<int>(m_ribbon_state);
        if ( state != chrome.state )
        {
            chrome.state = state;

            C::TabView shownTabView{ nullptr };
            if ( m_flags & wxRIBBON_BAR_TAB_VIEW )
            {
                for ( const auto& child : strip.Children() )
                {
                    if ( (shownTabView = child.try_as<C::TabView>()) )
                        break;
                }
            }
            if ( shownTabView )
            {
                if ( m_current_page >= 0 &&
                     static_cast<uint32_t>(m_current_page) <
                        shownTabView.TabItems().Size() )
                {
                    shownTabView.SelectedIndex(m_current_page);
                }
            }
            else if ( m_current_page >= 0 &&
                 static_cast<uint32_t>(m_current_page) < tabs.Items().Size() )
            {
                // The selector shows the mark of its selection only if the
                // item had its template when it was selected: one which was
                // selected too early is selected again when it is loaded,
                // see where the items are made.
                const auto item = tabs.Items().GetAt(m_current_page);
                if ( tabs.SelectedItem() == item && item.IsLoaded() )
                    tabs.SelectedItem(nullptr);
                tabs.SelectedItem(item);

                // Among more tabs than fit, the chosen one is in view.
                if ( item.IsLoaded() )
                    item.StartBringIntoView();
            }
            else
            {
                tabs.SelectedItem(nullptr);
            }

            // Fold the ribbon, unfold it, or keep it unfolded.
            for ( const auto& child : buttons.Children() )
            {
                const auto toggle = child.try_as<C::Button>();
                if ( !toggle || toggle.Name() != L"Toggle" )
                    continue;

                if ( const auto icon = toggle.Content().try_as<C::FontIcon>() )
                {
                    icon.Glyph(m_ribbon_state == wxRIBBON_BAR_MINIMIZED ? L"\uE70D"
                             : m_ribbon_state == wxRIBBON_BAR_EXPANDED ? L"\uE718"
                             : L"\uE70E");
                }
            }
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("ribbon tabs", e);
    }
}

#endif // __WXWINUI__ && wxUSE_WINUI3

wxRibbonBar::~wxRibbonBar()
{
#if defined(WXWINUI_RIBBON_TOOLKIT)
    wxRibbonContainerWinUI::Close(this);
#endif
#if defined(__WXWINUI__) && wxUSE_WINUI3
    if ( m_winuiChrome )
        m_winuiChrome->Close();
#endif

    SetArtProvider(nullptr);

    for ( auto* list : m_image_lists )
    {
        delete list;
    }
}

bool wxRibbonBar::Create(wxWindow* parent,
                wxWindowID id,
                const wxPoint& pos,
                const wxSize& size,
                long style)
{
    if(!wxRibbonControl::Create(parent, id, pos, size, wxBORDER_NONE))
        return false;

    CommonInit(style);

#if defined(WXWINUI_RIBBON_TOOLKIT)
    if ( !wxRibbonContainerWinUI::Create(this) ) return false;
    wxRibbonContainerWinUI::SetTabMargins(this, m_tab_margin_left, m_tab_margin_right);
    return true;
#else
    return true;
#endif
}

void wxRibbonBar::CommonInit(long style)
{
    SetName("wxRibbonBar");

    m_flags = style;
    m_tab_margin_left = 50;
    m_tab_margin_right = 20;
    if ( m_flags & wxRIBBON_BAR_SHOW_TOGGLE_BUTTON )
        m_tab_margin_right += 20;
    if ( m_flags & wxRIBBON_BAR_SHOW_HELP_BUTTON )
        m_tab_margin_right += 20;
    m_tab_height = 20; // initial guess

    if(m_art == nullptr)
    {
        SetArtProvider(new wxRibbonDefaultArtProvider);
    }
    SetBackgroundStyle(wxBG_STYLE_PAINT);

#if defined(__WXWINUI__) && wxUSE_WINUI3 && !defined(WXWINUI_RIBBON_TOOLKIT)
    // The tabs are a SelectorBar, XAML content of this window; if that
    // cannot be, the art provider draws them.
    if ( !m_winuiChrome && GetHandle() )
    {
        auto chrome = std::make_shared<wxRibbonChromeWinUI>();
        if ( chrome->Create(this, wxRibbonBarChromeMarkup()) )
        {
            m_winuiChrome = chrome;

            if ( const auto tabs =
                    chrome->Find<winrt::Microsoft::UI::Xaml::Controls::SelectorBar>(L"Tabs") )
            {
                const wxWeakRef<wxRibbonBar> weak(this);

                // More tabs than fit can be scrolled, with the wheel too.
                if ( const auto scroll =
                        chrome->Find<winrt::Microsoft::UI::Xaml::Controls::ScrollViewer>(L"TabScroll") )
                {
                    scroll.PointerWheelChanged([scroll](const auto&, const auto& args)
                    {
                        if ( scroll.ScrollableWidth() <= 0 )
                            return;

                        const int delta = args.GetCurrentPoint(scroll)
                                              .Properties().MouseWheelDelta();
                        scroll.ChangeView(scroll.HorizontalOffset() - delta,
                                          nullptr, nullptr);
                        args.Handled(true);
                    });
                }

                // A selection made before the selector has its template is
                // not shown: it is made again once it is loaded.
                tabs.Loaded([weak](const auto&, const auto&)
                {
                    wxRibbonBar * const bar = weak.get();
                    if ( bar && bar->m_winuiChrome )
                    {
                        bar->m_winuiChrome->state = -1;
                        bar->WinUISyncTabs();
                    }
                });

                tabs.SelectionChanged([weak](const auto& sender, const auto&)
                {
                    wxRibbonBar * const bar = weak.get();
                    if ( !bar || !bar->m_winuiChrome ||
                         bar->m_winuiChrome->syncing )
                    {
                        return;
                    }

                    uint32_t index = 0;
                    const auto selected = sender.SelectedItem();
                    if ( !selected ||
                         !sender.Items().IndexOf(selected, index) )
                    {
                        return;
                    }
                    bar->m_winuiChrome->chosen = ::GetTickCount64();
                    bar->WinUIChooseTab(index);
                });
            }
        }
    }
#endif
}

wxImageList* wxRibbonBar::GetButtonImageList(wxSize size, int initialCount)
{
    for ( auto* list : m_image_lists )
    {
        if ( list->GetSize() == size )
            return list;
    }

    wxImageList* const
        il = new wxImageList(size.GetWidth(), size.GetHeight(), /*mask*/false, initialCount);
    m_image_lists.push_back(il);

    return il;
}

void wxRibbonBar::SetArtProvider(wxRibbonArtProvider* art)
{
    if ( art == m_art )
        return;

    wxRibbonArtProvider *old = m_art;
    m_art = art;

    if(art)
    {
        art->SetFlags(m_flags);
    }
    size_t numpages = m_pages.GetCount();
    size_t i;
    for(i = 0; i < numpages; ++i)
    {
        wxRibbonPage *page = m_pages.Item(i).page;
        if(page->GetArtProvider() != art)
        {
            page->SetArtProvider(art);
        }
    }

    delete old;
}

void wxRibbonBar::OnPaint(wxPaintEvent& WXUNUSED(evt))
{
#if defined(WXWINUI_RIBBON_TOOLKIT)
    if ( wxRibbonContainerWinUI::Native(this) )
    {
        wxPaintDC dc(this);
        return;
    }
#endif

#if defined(__WXWINUI__) && wxUSE_WINUI3
    if ( m_winuiChrome )
    {
        // Every change of the tabs leads here, as it used to redraw them.
        wxPaintDC dc(this);
        WinUISyncTabs();
        return;
    }

    // What is painted goes to the surface of the window, which XAML shows
    // with its transparency: no buffer in between, which would make every
    // pixel of the window opaque. The tabs lie on the material of the
    // window, so what was painted where they are painted again goes first.
    wxPaintDC dc(this);
#else
    wxAutoBufferedPaintDC dc(this);
#endif

    if(GetUpdateRegion().Contains(0, 0, GetClientSize().GetWidth(), m_tab_height) == wxOutRegion)
    {
        // Nothing to do in the tab area, and the page area is handled by the active page
        return;
    }

#if defined(__WXWINUI__) && wxUSE_WINUI3
    {
        wxRect repainted = GetUpdateRegion().GetBox();
        repainted.Intersect(wxRect(0, 0, GetClientSize().GetWidth(), m_tab_height));
        if ( !repainted.IsEmpty() )
        {
            wxWinUIPaintSurfaceClearRect(dc.GetHDC(), repainted.x, repainted.y,
                                         repainted.width, repainted.height);
        }
    }
#endif

    DoEraseBackground(dc);

    if ( m_flags & wxRIBBON_BAR_SHOW_HELP_BUTTON  )
        m_help_button_rect = m_art->GetRibbonHelpButtonArea(GetSize());
    if ( m_flags & wxRIBBON_BAR_SHOW_TOGGLE_BUTTON  )
        m_toggle_button_rect = m_art->GetBarToggleButtonArea(GetSize());

    size_t numtabs = m_pages.GetCount();
    double sep_visibility = 0.0;
    bool draw_sep = false;
    wxRect tabs_rect(m_tab_margin_left, 0, GetClientSize().GetWidth() - m_tab_margin_left - m_tab_margin_right, m_tab_height);
    if(m_tab_scroll_buttons_shown)
    {
        tabs_rect.x += m_tab_scroll_left_button_rect.GetWidth();
        tabs_rect.width -= m_tab_scroll_left_button_rect.GetWidth() + m_tab_scroll_right_button_rect.GetWidth();
    }
    size_t i;
    for(i = 0; i < numtabs; ++i)
    {
        wxRibbonPageTabInfo& info = m_pages.Item(i);
        if (!info.shown)
            continue;

        dc.DestroyClippingRegion();
        if(m_tab_scroll_buttons_shown)
        {
            if(!tabs_rect.Intersects(info.rect))
                continue;
            dc.SetClippingRegion(tabs_rect);
        }
        dc.SetClippingRegion(info.rect);
        m_art->DrawTab(dc, this, info);

        if(info.rect.width < info.small_begin_need_separator_width)
        {
            draw_sep = true;
            if(info.rect.width < info.small_must_have_separator_width)
            {
                sep_visibility += 1.0;
            }
            else
            {
                sep_visibility += (double)(info.small_begin_need_separator_width - info.rect.width) / (double)(info.small_begin_need_separator_width - info.small_must_have_separator_width);
            }
        }
    }
    if(draw_sep)
    {
        wxRect rect = m_pages.Item(0).rect;
        rect.width = m_art->GetMetric(wxRIBBON_ART_TAB_SEPARATION_SIZE);
        sep_visibility /= (double)numtabs;
        for(i = 0; i < numtabs - 1; ++i)
        {
            wxRibbonPageTabInfo& info = m_pages.Item(i);
            if (!info.shown)
                continue;
            rect.x = info.rect.x + info.rect.width;

            if(m_tab_scroll_buttons_shown && !tabs_rect.Intersects(rect))
            {
                continue;
            }

            dc.DestroyClippingRegion();
            dc.SetClippingRegion(rect);
            m_art->DrawTabSeparator(dc, this, rect, sep_visibility);
        }
    }
    if(m_tab_scroll_buttons_shown)
    {
        if(m_tab_scroll_left_button_rect.GetWidth() != 0)
        {
            dc.DestroyClippingRegion();
            dc.SetClippingRegion(m_tab_scroll_left_button_rect);
            m_art->DrawScrollButton(dc, this, m_tab_scroll_left_button_rect, wxRIBBON_SCROLL_BTN_LEFT | m_tab_scroll_left_button_state | wxRIBBON_SCROLL_BTN_FOR_TABS);
        }
        if(m_tab_scroll_right_button_rect.GetWidth() != 0)
        {
            dc.DestroyClippingRegion();
            dc.SetClippingRegion(m_tab_scroll_right_button_rect);
            m_art->DrawScrollButton(dc, this, m_tab_scroll_right_button_rect, wxRIBBON_SCROLL_BTN_RIGHT | m_tab_scroll_right_button_state | wxRIBBON_SCROLL_BTN_FOR_TABS);
        }
    }

    if ( m_flags & wxRIBBON_BAR_SHOW_HELP_BUTTON  )
        m_art->DrawHelpButton(dc, this, m_help_button_rect);
    if ( m_flags & wxRIBBON_BAR_SHOW_TOGGLE_BUTTON  )
        m_art->DrawToggleButton(dc, this, m_toggle_button_rect, m_ribbon_state);

}

void wxRibbonBar::OnEraseBackground(wxEraseEvent& WXUNUSED(evt))
{
    // Background painting done in main paint handler to reduce screen flicker
}

void wxRibbonBar::DoEraseBackground(wxDC& dc)
{
    wxRect tabs(GetSize());
    tabs.height = m_tab_height;
    m_art->DrawTabCtrlBackground(dc, this, tabs);
}

void wxRibbonBar::OnSize(wxSizeEvent& evt)
{
#if defined(WXWINUI_RIBBON_TOOLKIT)
    if ( wxRibbonContainerWinUI::Native(this) )
    {
        evt.Skip();
        wxRibbonContainerWinUI::Sync(this);
        return;
    }
#endif

    RecalculateTabSizes();
    if(m_current_page != wxNOT_FOUND)
    {
        RepositionPage(m_pages.Item(m_current_page).page);
    }
    RefreshTabBar();

    evt.Skip();
}

void wxRibbonBar::OnDPIChanged(wxDPIChangedEvent& event)
{
#if defined(WXWINUI_RIBBON_TOOLKIT)
    if ( wxRibbonContainerWinUI::Native(this) )
    {
        event.Skip();
        wxRibbonContainerWinUI::Sync(this);
        return;
    }
#endif

    // Recalculate tab sizes for new DPI
    RecalculateTabSizes();

    // Realize all pages to update their layouts
    size_t page_count = m_pages.GetCount();
    for(size_t i = 0; i < page_count; ++i)
    {
        m_pages.Item(i).page->Realize();
    }

    // Reposition current page
    if(m_current_page != wxNOT_FOUND)
    {
        RepositionPage(m_pages.Item(m_current_page).page);
    }

    Refresh();
    event.Skip();
}

void wxRibbonBar::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    event.Skip();
    if ( m_art )
        m_art->UpdateColoursFromSystem();
}

void wxRibbonBar::RepositionPage(wxRibbonPage *page)
{
#if defined(WXWINUI_RIBBON_TOOLKIT)
    if ( wxRibbonContainerWinUI::Native(this) )
        return;
#endif

    int w, h;
    GetSize(&w, &h);

    // Folding a ribbon is giving its bar no more height than its tabs. A
    // ribbon whose groups are stacked along an edge of the window is as high
    // as the window whatever its own best size: its page is folded here.
    if ( !m_arePanelsShown && (m_flags & wxRIBBON_BAR_FLOW_VERTICAL) )
        h = m_tab_height;

    page->SetSizeWithScrollButtonAdjustment(0, m_tab_height, w, h - m_tab_height);
}

wxRibbonPageTabInfo* wxRibbonBar::HitTestTabs(wxPoint position, int* index)
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    // The tabs which are shown are those of the SelectorBar, which takes
    // the pointer itself; the rectangles wxRibbon works out for tabs it
    // would draw are not where they are.
    if ( m_winuiChrome )
        return nullptr;
#endif

    wxRect tabs_rect(m_tab_margin_left, 0, GetClientSize().GetWidth() - m_tab_margin_left - m_tab_margin_right, m_tab_height);
    if(m_tab_scroll_buttons_shown)
    {
        tabs_rect.SetX(tabs_rect.GetX() + m_tab_scroll_left_button_rect.GetWidth());
        tabs_rect.SetWidth(tabs_rect.GetWidth() - m_tab_scroll_left_button_rect.GetWidth() - m_tab_scroll_right_button_rect.GetWidth());
    }
    if(tabs_rect.Contains(position))
    {
        size_t numtabs = m_pages.GetCount();
        size_t i;
        for(i = 0; i < numtabs; ++i)
        {
            wxRibbonPageTabInfo& info = m_pages.Item(i);
            if (!info.shown)
                continue;
            if(info.rect.Contains(position))
            {
                if(index != nullptr)
                {
                    *index = (int)i;
                }
                return &info;
            }
        }
    }
    if(index != nullptr)
    {
        *index = -1;
    }
    return nullptr;
}

void wxRibbonBar::OnMouseLeftDown(wxMouseEvent& evt)
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    // The tabs and the buttons of the bar are XAML controls, which take the
    // pointer themselves; the same press is also reported to this window,
    // where nothing of wxRibbon's own is drawn any more. Acting on it --
    // taking the focus, above all -- would take the press away from the
    // control it was meant for.
    if ( m_winuiChrome )
    {
        evt.Skip();
        return;
    }
#endif

    wxRibbonPageTabInfo *tab = HitTestTabs(evt.GetPosition());
    SetFocus();
    if ( tab )
    {
        if ( m_ribbon_state == wxRIBBON_BAR_MINIMIZED )
        {
            ShowPanels(wxRIBBON_BAR_EXPANDED);
        }
        else if ( (tab == &m_pages.Item(m_current_page)) && (m_ribbon_state == wxRIBBON_BAR_EXPANDED) )
        {
            HidePanels();
        }
    }
    else
    {
        if ( m_ribbon_state == wxRIBBON_BAR_EXPANDED )
        {
            HidePanels();
        }
    }
    if(tab && tab != &m_pages.Item(m_current_page))
    {
        wxRibbonBarEvent query(wxEVT_RIBBONBAR_PAGE_CHANGING, GetId(), tab->page);
        query.SetEventObject(this);
        ProcessWindowEvent(query);
        if(query.IsAllowed())
        {
            SetActivePage(query.GetPage());

            wxRibbonBarEvent notification(wxEVT_RIBBONBAR_PAGE_CHANGED, GetId(), m_pages.Item(m_current_page).page);
            notification.SetEventObject(this);
            ProcessWindowEvent(notification);
        }
    }
    else if(tab == nullptr)
    {
        if(m_tab_scroll_left_button_rect.Contains(evt.GetPosition()))
        {
            m_tab_scroll_left_button_state |= wxRIBBON_SCROLL_BTN_ACTIVE | wxRIBBON_SCROLL_BTN_HOVERED;
            RefreshTabBar();
        }
        else if(m_tab_scroll_right_button_rect.Contains(evt.GetPosition()))
        {
            m_tab_scroll_right_button_state |= wxRIBBON_SCROLL_BTN_ACTIVE | wxRIBBON_SCROLL_BTN_HOVERED;
            RefreshTabBar();
        }
    }

    wxPoint position = evt.GetPosition();

    if(position.x >= 0 && position.y >= 0)
    {
        wxSize size = GetSize();
        if(position.x < size.GetWidth() && position.y < size.GetHeight())
        {
            if(m_toggle_button_rect.Contains(position))
            {
                ShowPanels(ArePanelsShown() ? wxRIBBON_BAR_MINIMIZED : wxRIBBON_BAR_PINNED);
                wxRibbonBarEvent event(wxEVT_RIBBONBAR_TOGGLED, GetId());
                event.SetEventObject(this);
                ProcessWindowEvent(event);
            }
            if ( m_help_button_rect.Contains(position) )
            {
                wxRibbonBarEvent event(wxEVT_RIBBONBAR_HELP_CLICK, GetId());
                event.SetEventObject(this);
                ProcessWindowEvent(event);
            }
        }
    }
}

void wxRibbonBar::OnMouseLeftUp(wxMouseEvent& WXUNUSED(evt))
{
    if(!m_tab_scroll_buttons_shown)
    {
        return;
    }

    int amount = 0;
    if(m_tab_scroll_left_button_state & wxRIBBON_SCROLL_BTN_ACTIVE)
    {
        amount = -1;
    }
    else if(m_tab_scroll_right_button_state & wxRIBBON_SCROLL_BTN_ACTIVE)
    {
        amount = 1;
    }
    if(amount != 0)
    {
        m_tab_scroll_left_button_state &= ~wxRIBBON_SCROLL_BTN_ACTIVE;
        m_tab_scroll_right_button_state &= ~wxRIBBON_SCROLL_BTN_ACTIVE;
        ScrollTabBar(amount * 8);
    }
}

void wxRibbonBar::ScrollTabBar(int amount)
{
    bool show_left = true;
    bool show_right = true;
    if(m_tab_scroll_amount + amount <= 0)
    {
        amount = -m_tab_scroll_amount;
        show_left = false;
    }
    else if(m_tab_scroll_amount + amount + (GetClientSize().GetWidth() - m_tab_margin_left - m_tab_margin_right) >= m_tabs_total_width_minimum)
    {
        amount = m_tabs_total_width_minimum - m_tab_scroll_amount - (GetClientSize().GetWidth() - m_tab_margin_left - m_tab_margin_right);
        show_right = false;
    }
    if(amount == 0)
    {
        return;
    }
    m_tab_scroll_amount += amount;
    size_t numtabs = m_pages.GetCount();
    size_t i;
    for(i = 0; i < numtabs; ++i)
    {
        wxRibbonPageTabInfo& info = m_pages.Item(i);
        if (!info.shown)
            continue;
        info.rect.SetX(info.rect.GetX() - amount);
    }
    if(show_right != (m_tab_scroll_right_button_rect.GetWidth() != 0) ||
        show_left != (m_tab_scroll_left_button_rect.GetWidth() != 0))
    {
        wxInfoDC temp_dc(this);
        if(show_left)
        {
            m_tab_scroll_left_button_rect.SetWidth(m_art->GetScrollButtonMinimumSize(temp_dc, this, wxRIBBON_SCROLL_BTN_LEFT | wxRIBBON_SCROLL_BTN_NORMAL | wxRIBBON_SCROLL_BTN_FOR_TABS).GetWidth());
        }
        else
        {
            m_tab_scroll_left_button_rect.SetWidth(0);
        }

        if(show_right)
        {
            if(m_tab_scroll_right_button_rect.GetWidth() == 0)
            {
                m_tab_scroll_right_button_rect.SetWidth(m_art->GetScrollButtonMinimumSize(temp_dc, this, wxRIBBON_SCROLL_BTN_RIGHT | wxRIBBON_SCROLL_BTN_NORMAL | wxRIBBON_SCROLL_BTN_FOR_TABS).GetWidth());
                m_tab_scroll_right_button_rect.SetX(m_tab_scroll_right_button_rect.GetX() - m_tab_scroll_right_button_rect.GetWidth());
            }
        }
        else
        {
            if(m_tab_scroll_right_button_rect.GetWidth() != 0)
            {
                m_tab_scroll_right_button_rect.SetX(m_tab_scroll_right_button_rect.GetX() + m_tab_scroll_right_button_rect.GetWidth());
                m_tab_scroll_right_button_rect.SetWidth(0);
            }
        }
    }

    RefreshTabBar();
}

void wxRibbonBar::RefreshTabBar()
{
    wxRect tab_rect(0, 0, GetClientSize().GetWidth(), m_tab_height);
    Refresh(false, &tab_rect);
}

void wxRibbonBar::OnMouseMiddleDown(wxMouseEvent& evt)
{
    DoMouseButtonCommon(evt, wxEVT_RIBBONBAR_TAB_MIDDLE_DOWN);
}

void wxRibbonBar::OnMouseMiddleUp(wxMouseEvent& evt)
{
    DoMouseButtonCommon(evt, wxEVT_RIBBONBAR_TAB_MIDDLE_UP);
}

void wxRibbonBar::OnMouseRightDown(wxMouseEvent& evt)
{
    DoMouseButtonCommon(evt, wxEVT_RIBBONBAR_TAB_RIGHT_DOWN);
}

void wxRibbonBar::OnMouseRightUp(wxMouseEvent& evt)
{
    DoMouseButtonCommon(evt, wxEVT_RIBBONBAR_TAB_RIGHT_UP);
}

void wxRibbonBar::OnMouseDoubleClick(wxMouseEvent& evt)
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    // The tabs and the buttons of the bar are XAML controls, which take the
    // pointer themselves; the same press is also reported to this window,
    // where nothing of wxRibbon's own is drawn any more. Acting on it --
    // taking the focus, above all -- would take the press away from the
    // control it was meant for.
    if ( m_winuiChrome )
    {
        evt.Skip();
        return;
    }
#endif

    wxRibbonPageTabInfo *tab = HitTestTabs(evt.GetPosition());
    SetFocus();
    if ( tab && tab == &m_pages.Item(m_current_page) )
    {
        if ( m_ribbon_state == wxRIBBON_BAR_PINNED )
        {
            HidePanels();
        }
        else
        {
            ShowPanels(wxRIBBON_BAR_PINNED);
        }
    }
}

void wxRibbonBar::DoMouseButtonCommon(wxMouseEvent& evt, wxEventType tab_event_type)
{
    wxRibbonPageTabInfo *tab = HitTestTabs(evt.GetPosition());
    if(tab)
    {
        wxRibbonBarEvent notification(tab_event_type, GetId(), tab->page);
        notification.SetEventObject(this);
        ProcessWindowEvent(notification);
    }
}

void wxRibbonBar::RecalculateMinSize()
{
    wxSize min_size(wxDefaultCoord, wxDefaultCoord);
    size_t numtabs = m_pages.GetCount();
    if(numtabs != 0)
    {
        min_size = m_pages.Item(0).page->GetMinSize();

        size_t i;
        for(i = 1; i < numtabs; ++i)
        {
            wxRibbonPageTabInfo& info = m_pages.Item(i);
            if (!info.shown)
                continue;
            wxSize page_min = info.page->GetMinSize();

            min_size.x = wxMax(min_size.x, page_min.x);
            min_size.y = wxMax(min_size.y, page_min.y);
        }
    }
    if(min_size.y != wxDefaultCoord)
    {
        // TODO: Decide on best course of action when min height is unspecified
        // - should we specify it to the tab minimum, or leave it unspecified?
        min_size.IncBy(0, m_tab_height);
    }

    m_minWidth = min_size.GetWidth();
    m_minHeight = m_arePanelsShown ? min_size.GetHeight() : m_tab_height;
}

wxSize wxRibbonBar::DoGetBestSize() const
{
#if defined(WXWINUI_RIBBON_TOOLKIT)
    if ( wxRibbonContainerWinUI::Native(this) )
        return wxRibbonContainerWinUI::BestSize(const_cast<wxRibbonBar*>(this));
#endif

    wxSize best(0, 0);
    if(m_current_page != wxNOT_FOUND)
    {
        best = m_pages.Item(m_current_page).page->GetBestSize();
    }
    if(best.GetHeight() == wxDefaultCoord)
    {
        best.SetHeight(m_tab_height);
    }
    else
    {
        best.IncBy(0, m_tab_height);
    }
    if(!m_arePanelsShown)
    {
        best.SetHeight(m_tab_height);
    }
    return best;
}

void wxRibbonBar::HitTestRibbonButton(const wxRect& rect, const wxPoint& position, bool &hover_flag)
{
    bool hovered = false;
    if(position.x >= 0 && position.y >= 0)
    {
        wxSize size = GetSize();
        if(position.x < size.GetWidth() && position.y < size.GetHeight())
        {
            hovered = true;
        }
    }
    if(hovered)
    {
        bool toggle_button_hovered;
        toggle_button_hovered = rect.Contains(position);

        if ( hovered != m_bar_hovered || toggle_button_hovered != hover_flag )
        {
            m_bar_hovered = hovered;
            hover_flag = toggle_button_hovered;
            Refresh(false);
        }
    }
}

void wxRibbonBar::HideIfExpanded()
{
    if ( m_ribbon_state == wxRIBBON_BAR_EXPANDED)
        HidePanels();
}

void wxRibbonBar::OnKillFocus(wxFocusEvent& WXUNUSED(evt))
{
#if defined(WXWINUI_RIBBON_TOOLKIT)
    if ( wxRibbonContainerWinUI::Native(this) )
    {
        // Losing the tab slot to one of our command slots is not leaving the
        // Ribbon. The adapter also observes later exits from these children.
        wxRibbonContainerWinUI::NotifyFocusChange(this);
        return;
    }
#endif
    HideIfExpanded();
}

#endif // wxUSE_RIBBON
