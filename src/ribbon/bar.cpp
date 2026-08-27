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
#include "wx/ribbon/panel.h"
#include "wx/ribbon/buttonbar.h"
#include "wx/ribbon/toolbar.h"
#include "wx/ribbon/gallery.h"
#include "wx/dcbuffer.h"
#include "wx/app.h"
#include "wx/vector.h"

#include <algorithm>
#include <vector>

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

    SetMinSize(wxSize(GetSize().GetWidth(), DoGetBestSize().GetHeight()));
    Realise();
    GetParent()->Layout();

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
}

long wxRibbonBar::GetWindowStyleFlag() const
{
    return m_flags;
}

bool wxRibbonBar::Realize()
{
    HideKeyTips();

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
    m_tab_height = m_art->GetTabCtrlHeight(dcTemp, this, m_pages);

    RecalculateMinSize();
    RecalculateTabSizes();
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
    HideKeyTips();
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
}

void wxRibbonBar::DeletePage(size_t n)
{
    if(n < m_pages.GetCount())
    {
        HideKeyTips();

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
}

wxRibbonBar::~wxRibbonBar()
{
    if ( m_keyTipsActive )
        HideKeyTips();

    if ( m_keyTipsTopLevelParent != nullptr && !m_keyTipsTopLevelParent->IsBeingDeleted() )
    {
        m_keyTipsTopLevelParent->Unbind(wxEVT_CHAR_HOOK, &wxRibbonBar::OnKeyTipsCharHook, this);
        m_keyTipsTopLevelParent->Unbind(wxEVT_ACTIVATE, &wxRibbonBar::OnKeyTipsActivate, this);
        m_keyTipsTopLevelParent->Unbind(wxEVT_DESTROY, &wxRibbonBar::OnKeyTipsWindowDestroy, this);
    }
    m_keyTipsTopLevelParent = nullptr;

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

    return true;
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

    if ( m_keyTipsTriggerKeys.empty() )
        m_keyTipsTriggerKeys.push_back({ WXK_F10, wxMOD_NONE });

    if ( m_keyTipsTopLevelParent == nullptr )
    {
        m_keyTipsTopLevelParent = wxGetTopLevelParent(this);
        if ( m_keyTipsTopLevelParent != nullptr )
        {
            m_keyTipsTopLevelParent->Bind(wxEVT_CHAR_HOOK, &wxRibbonBar::OnKeyTipsCharHook, this);
            m_keyTipsTopLevelParent->Bind(wxEVT_ACTIVATE, &wxRibbonBar::OnKeyTipsActivate, this);
            m_keyTipsTopLevelParent->Bind(wxEVT_DESTROY, &wxRibbonBar::OnKeyTipsWindowDestroy, this);
        }
    }
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
    wxAutoBufferedPaintDC dc(this);

    if(GetUpdateRegion().Contains(0, 0, GetClientSize().GetWidth(), m_tab_height) == wxOutRegion)
    {
        // Nothing to do in the tab area, and the page area is handled by the active page
        return;
    }

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

    if ( m_keyTipsActive )
    {
        // The toggle and help buttons leave a clipping region set.
        dc.DestroyClippingRegion();
        std::vector<KeyTipBadge> badges;
        GetKeyTipTargetsFor(this, &badges);
        for ( const auto& badge : badges )
            m_art->DrawKeyTip(dc, this, badge.rect, badge.text);
    }
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
    if ( m_keyTipsActive )
        HideKeyTips();

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
    int w, h;
    GetSize(&w, &h);
    page->SetSizeWithScrollButtonAdjustment(0, m_tab_height, w, h - m_tab_height);
}

wxRibbonPageTabInfo* wxRibbonBar::HitTestTabs(wxPoint position, int* index)
{
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
    if ( m_keyTipsActive )
        HideKeyTips();

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
    if ( m_keyTipsActive )
        HideKeyTips();

    DoMouseButtonCommon(evt, wxEVT_RIBBONBAR_TAB_MIDDLE_DOWN);
}

void wxRibbonBar::OnMouseMiddleUp(wxMouseEvent& evt)
{
    DoMouseButtonCommon(evt, wxEVT_RIBBONBAR_TAB_MIDDLE_UP);
}

void wxRibbonBar::OnMouseRightDown(wxMouseEvent& evt)
{
    if ( m_keyTipsActive )
        HideKeyTips();

    DoMouseButtonCommon(evt, wxEVT_RIBBONBAR_TAB_RIGHT_DOWN);
}

void wxRibbonBar::OnMouseRightUp(wxMouseEvent& evt)
{
    DoMouseButtonCommon(evt, wxEVT_RIBBONBAR_TAB_RIGHT_UP);
}

void wxRibbonBar::OnMouseDoubleClick(wxMouseEvent& evt)
{
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
    if ( m_keyTipsActive )
        HideKeyTips();

    HideIfExpanded();
}

// ----------------------------------------------------------------------------
// KeyTips (Office-style keyboard access mode)
// ----------------------------------------------------------------------------

void wxRibbonBar::SetPageKeyTip(size_t page, const wxString& keytip)
{
    if ( page >= m_pages.GetCount() )
        return;
    m_pages.Item(page).keytip = keytip.Upper();
}

void wxRibbonBar::SetPageKeyTip(wxRibbonPage* page, const wxString& keytip)
{
    int n = GetPageNumber(page);
    if ( n != wxNOT_FOUND )
        SetPageKeyTip((size_t)n, keytip);
}

void wxRibbonBar::SetToggleButtonKeyTip(const wxString& keytip)
{
    m_toggleButtonKeyTip = keytip.Upper();
}

void wxRibbonBar::SetHelpButtonKeyTip(const wxString& keytip)
{
    m_helpButtonKeyTip = keytip.Upper();
}

void wxRibbonBar::SetKeyTipsTriggerKey(int keyCode, int modifiers)
{
    m_keyTipsTriggerKeys.clear();
    m_keyTipsTriggerKeys.push_back({ keyCode, modifiers });
}

void wxRibbonBar::AddKeyTipsTriggerKey(int keyCode, int modifiers)
{
    m_keyTipsTriggerKeys.push_back({ keyCode, modifiers });
}

void wxRibbonBar::ClearKeyTipsTriggerKeys()
{
    m_keyTipsTriggerKeys.clear();
}

void wxRibbonBar::RefreshKeyTipTargetWindows()
{
    // Refresh each window directly. Refreshing only an ancestor doesn't
    // reliably repaint child windows.
    for ( wxWindow* w : m_keyTipsWindows )
        w->Refresh();
}

bool wxRibbonBar::ShowKeyTips()
{
    if ( m_keyTipsActive )
        return true;

    if ( !IsShownOnScreen() )
        return false;

    DoBuildKeyTipTargets();
    if ( m_keyTipsTargets.empty() )
    {
        m_keyTipsWindows.clear();
        return false;
    }

    for ( wxWindow* w : m_keyTipsWindows )
    {
        if ( w != this )
            w->Bind(wxEVT_DESTROY, &wxRibbonBar::OnKeyTipsWindowDestroy, this);
    }

    m_keyTipsActive = true;
    m_keyTipsTypedPrefix.clear();
    RefreshKeyTipTargetWindows();
    return true;
}

void wxRibbonBar::HideKeyTips()
{
    if ( !m_keyTipsActive )
        return;

    // Refresh before clearing state, so each window still knows to repaint.
    RefreshKeyTipTargetWindows();
    m_keyTipsActive = false;
    m_keyTipsTypedPrefix.clear();
    for ( wxWindow* w : m_keyTipsWindows )
    {
        if ( w != this && w != nullptr && !w->IsBeingDeleted() )
            w->Unbind(wxEVT_DESTROY, &wxRibbonBar::OnKeyTipsWindowDestroy, this);
    }
    m_keyTipsTargets.clear();
    m_keyTipsWindows.clear();
}

void wxRibbonBar::DoBuildKeyTipTargets()
{
    m_keyTipsTargets.clear();

    size_t numtabs = m_pages.GetCount();
    for ( size_t i = 0; i < numtabs; ++i )
    {
        wxRibbonPageTabInfo& tab = m_pages.Item(i);
        if ( !tab.shown || tab.keytip.empty() )
            continue;

        wxRibbonKeyTipInfo info;
        info.fullKeyTip = tab.keytip;
        info.rect = tab.rect;
        info.window = this;
        info.kind = wxRibbonKeyTipInfo::KeyTip_PageTab;
        info.pageIndex = i;
        m_keyTipsTargets.push_back(info);
    }

    if ( (m_flags & wxRIBBON_BAR_SHOW_TOGGLE_BUTTON) && !m_toggleButtonKeyTip.empty() )
    {
        wxRibbonKeyTipInfo info;
        info.fullKeyTip = m_toggleButtonKeyTip;
        info.rect = m_toggle_button_rect;
        info.window = this;
        info.kind = wxRibbonKeyTipInfo::KeyTip_ToggleButton;
        m_keyTipsTargets.push_back(info);
    }

    if ( (m_flags & wxRIBBON_BAR_SHOW_HELP_BUTTON) && !m_helpButtonKeyTip.empty() )
    {
        wxRibbonKeyTipInfo info;
        info.fullKeyTip = m_helpButtonKeyTip;
        info.rect = m_help_button_rect;
        info.window = this;
        info.kind = wxRibbonKeyTipInfo::KeyTip_HelpButton;
        m_keyTipsTargets.push_back(info);
    }

    if ( m_current_page != wxNOT_FOUND && m_arePanelsShown )
    {
        wxRibbonPage* page = m_pages.Item(m_current_page).page;
        for ( wxWindowList::compatibility_iterator node = page->GetChildren().GetFirst();
              node; node = node->GetNext() )
        {
            wxRibbonPanel* panel = wxDynamicCast(node->GetData(), wxRibbonPanel);
            if ( panel == nullptr || !panel->IsShown() )
                continue;

            if ( panel->IsMinimised() )
            {
                wxString keytip = panel->GetKeyTip();
                if ( !keytip.empty() )
                {
                    wxRibbonKeyTipInfo info;
                    info.fullKeyTip = keytip;
                    info.rect = wxRect(wxPoint(0, 0), panel->GetSize());
                    info.window = panel;
                    info.kind = wxRibbonKeyTipInfo::KeyTip_MinimisedPanel;
                    info.panel = panel;
                    m_keyTipsTargets.push_back(info);
                }
                continue;
            }

            if ( panel->HasExtButton() )
            {
                wxString keytip = panel->GetExtButtonKeyTip();
                if ( !keytip.empty() )
                {
                    wxRibbonKeyTipInfo info;
                    info.fullKeyTip = keytip;
                    info.rect = panel->GetExtButtonRect();
                    info.window = panel;
                    info.kind = wxRibbonKeyTipInfo::KeyTip_ExtButton;
                    info.panel = panel;
                    m_keyTipsTargets.push_back(info);
                }
            }

            for ( wxWindowList::compatibility_iterator pnode = panel->GetChildren().GetFirst();
                  pnode; pnode = pnode->GetNext() )
            {
                wxWindow* child = pnode->GetData();
                if ( !child->IsShown() )
                    continue;

                wxRibbonButtonBar* bb = wxDynamicCast(child, wxRibbonButtonBar);
                wxRibbonToolBar* tb = wxDynamicCast(child, wxRibbonToolBar);
                wxRibbonGallery* gallery = wxDynamicCast(child, wxRibbonGallery);
                if ( bb != nullptr )
                {
                    size_t count = bb->GetButtonCount();
                    for ( size_t b = 0; b < count; ++b )
                    {
                        wxRibbonButtonBarButtonBase* button = bb->GetItem(b);
                        int id = bb->GetItemId(button);
                        if ( !bb->GetButtonEnabled(id) )
                            continue;
                        wxString keytip = bb->GetKeyTip(id);
                        if ( keytip.empty() )
                            continue;

                        // Empty if the button isn't in the current layout.
                        wxRect rect = bb->GetItemRect(id);
                        if ( rect.IsEmpty() )
                            continue;

                        wxRibbonKeyTipInfo info;
                        info.fullKeyTip = keytip;
                        info.rect = rect;
                        info.window = bb;
                        info.kind = wxRibbonKeyTipInfo::KeyTip_ButtonBarItem;
                        info.buttonBar = bb;
                        info.buttonBarItemId = id;
                        m_keyTipsTargets.push_back(info);

                        wxString dropdownKeytip = bb->GetDropdownKeyTip(id);
                        wxRect dropdownRect = bb->GetItemDropdownRect(id);
                        if ( !dropdownKeytip.empty() && !dropdownRect.IsEmpty() )
                        {
                            wxRibbonKeyTipInfo dropdownInfo;
                            dropdownInfo.fullKeyTip = dropdownKeytip;
                            dropdownInfo.rect = dropdownRect;
                            dropdownInfo.window = bb;
                            dropdownInfo.kind = wxRibbonKeyTipInfo::KeyTip_ButtonBarItem;
                            dropdownInfo.buttonBar = bb;
                            dropdownInfo.buttonBarItemId = id;
                            dropdownInfo.dropdown = true;
                            m_keyTipsTargets.push_back(dropdownInfo);
                        }
                    }
                }
                else if ( tb != nullptr )
                {
                    size_t count = tb->GetToolCount();
                    for ( size_t t = 0; t < count; ++t )
                    {
                        wxRibbonToolBarToolBase* tool = tb->GetToolByPos(t);
                        if ( tool == nullptr )
                            continue; // separator
                        int id = tb->GetToolId(tool);
                        if ( !tb->GetToolEnabled(id) )
                            continue;
                        wxString keytip = tb->GetKeyTip(id);
                        if ( keytip.empty() )
                            continue;

                        wxRect rect = tb->GetToolRect(id);
                        if ( rect.IsEmpty() )
                            continue;

                        wxRibbonKeyTipInfo info;
                        info.fullKeyTip = keytip;
                        info.rect = rect;
                        info.window = tb;
                        info.kind = wxRibbonKeyTipInfo::KeyTip_ToolBarItem;
                        info.toolBar = tb;
                        info.toolBarItemId = id;
                        m_keyTipsTargets.push_back(info);

                        wxString dropdownKeytip = tb->GetDropdownKeyTip(id);
                        wxRect dropdownRect = tb->GetToolDropdownRect(id);
                        if ( !dropdownKeytip.empty() && !dropdownRect.IsEmpty() )
                        {
                            wxRibbonKeyTipInfo dropdownInfo;
                            dropdownInfo.fullKeyTip = dropdownKeytip;
                            dropdownInfo.rect = dropdownRect;
                            dropdownInfo.window = tb;
                            dropdownInfo.kind = wxRibbonKeyTipInfo::KeyTip_ToolBarItem;
                            dropdownInfo.toolBar = tb;
                            dropdownInfo.toolBarItemId = id;
                            dropdownInfo.dropdown = true;
                            m_keyTipsTargets.push_back(dropdownInfo);
                        }
                    }
                }
                else if ( gallery != nullptr )
                {
                    wxString keytip = gallery->GetKeyTip();
                    if ( !keytip.empty() )
                    {
                        wxRibbonKeyTipInfo info;
                        info.fullKeyTip = keytip;
                        info.rect = wxRect(wxPoint(0, 0), gallery->GetSize());
                        info.window = gallery;
                        info.kind = wxRibbonKeyTipInfo::KeyTip_Gallery;
                        m_keyTipsTargets.push_back(info);
                    }
                }
            }
        }
    }

    for ( auto& target : m_keyTipsTargets )
        target.remaining = target.fullKeyTip;

    std::vector<wxWindow*> oldWindows;
    if ( m_keyTipsActive )
        oldWindows = m_keyTipsWindows;

    std::vector<wxWindow*> newWindows;
    newWindows.push_back(this);
    for ( const auto& target : m_keyTipsTargets )
    {
        if ( std::find(newWindows.begin(), newWindows.end(), target.window) == newWindows.end() )
            newWindows.push_back(target.window);
    }

    if ( m_keyTipsActive )
    {
        for ( wxWindow* w : oldWindows )
        {
            if ( w != this && std::find(newWindows.begin(), newWindows.end(), w) == newWindows.end()
                 && w != nullptr && !w->IsBeingDeleted() )
                w->Unbind(wxEVT_DESTROY, &wxRibbonBar::OnKeyTipsWindowDestroy, this);
        }
        for ( wxWindow* w : newWindows )
        {
            if ( w != this && std::find(oldWindows.begin(), oldWindows.end(), w) == oldWindows.end() )
                w->Bind(wxEVT_DESTROY, &wxRibbonBar::OnKeyTipsWindowDestroy, this);
        }
    }
    m_keyTipsWindows = std::move(newWindows);
}

void wxRibbonBar::DoActivateKeyTipTarget(const wxRibbonKeyTipInfo& target)
{
    switch ( target.kind )
    {
        case wxRibbonKeyTipInfo::KeyTip_PageTab:
        {
            wxRibbonPageTabInfo& tab = m_pages.Item(target.pageIndex);
            if ( m_ribbon_state == wxRIBBON_BAR_MINIMIZED )
                ShowPanels(wxRIBBON_BAR_EXPANDED);
            if ( (int)target.pageIndex != m_current_page )
            {
                wxRibbonBarEvent query(wxEVT_RIBBONBAR_PAGE_CHANGING, GetId(), tab.page);
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
            // Switching pages isn't a command, so stay in keytip mode and
            // show the keytips of the page we just moved to.
            ShowKeyTips();
            break;
        }

        case wxRibbonKeyTipInfo::KeyTip_ToggleButton:
        {
            ShowPanels(ArePanelsShown() ? wxRIBBON_BAR_MINIMIZED : wxRIBBON_BAR_PINNED);
            wxRibbonBarEvent event(wxEVT_RIBBONBAR_TOGGLED, GetId());
            event.SetEventObject(this);
            ProcessWindowEvent(event);
            break;
        }

        case wxRibbonKeyTipInfo::KeyTip_HelpButton:
        {
            wxRibbonBarEvent event(wxEVT_RIBBONBAR_HELP_CLICK, GetId());
            event.SetEventObject(this);
            ProcessWindowEvent(event);
            break;
        }

        case wxRibbonKeyTipInfo::KeyTip_ExtButton:
        {
            wxRibbonPanelEvent notification(wxEVT_RIBBONPANEL_EXTBUTTON_ACTIVATED, target.panel->GetId());
            notification.SetEventObject(target.panel);
            notification.SetPanel(target.panel);
            target.panel->ProcessWindowEvent(notification);
            break;
        }

        case wxRibbonKeyTipInfo::KeyTip_MinimisedPanel:
            target.panel->ShowExpanded();
            break;

        case wxRibbonKeyTipInfo::KeyTip_ButtonBarItem:
        {
            wxRibbonButtonBarButtonBase* button = target.buttonBar->GetItemById(target.buttonBarItemId);
            if ( button != nullptr )
                target.buttonBar->ActivateButton(button, target.dropdown);
            break;
        }

        case wxRibbonKeyTipInfo::KeyTip_ToolBarItem:
        {
            wxRibbonToolBarToolBase* tool = target.toolBar->FindById(target.toolBarItemId);
            if ( tool != nullptr )
                target.toolBar->ActivateTool(tool, target.dropdown);
            break;
        }

        case wxRibbonKeyTipInfo::KeyTip_Gallery:
            target.window->SetFocus();
            break;
    }
}

void wxRibbonBar::GetKeyTipTargetsFor(wxWindow* window, std::vector<KeyTipBadge>* badges) const
{
    badges->clear();
    if ( !m_keyTipsActive )
        return;

    for ( const auto& target : m_keyTipsTargets )
    {
        if ( target.window == window )
        {
            KeyTipBadge badge;
            badge.rect = target.rect;
            badge.text = target.remaining;
            badges->push_back(badge);
        }
    }
}

void wxRibbonBar::OnKeyTipsWindowDestroy(wxWindowDestroyEvent& event)
{
    event.Skip();
    wxWindow* window = event.GetWindow();

    if ( window == m_keyTipsTopLevelParent )
    {
        m_keyTipsTopLevelParent = nullptr;
        if ( m_keyTipsActive )
            HideKeyTips();
        return;
    }

    if ( !m_keyTipsActive )
        return;

    auto it = std::find(m_keyTipsWindows.begin(), m_keyTipsWindows.end(), window);
    if ( it != m_keyTipsWindows.end() )
    {
        m_keyTipsWindows.erase(it);
        HideKeyTips();
    }
}

bool wxRibbonBar::Reparent(wxWindowBase* newParent)
{
    if ( m_keyTipsActive )
        HideKeyTips();

    wxWindow* oldTLW = m_keyTipsTopLevelParent;
    bool res = wxRibbonControl::Reparent(newParent);
    wxWindow* newTLW = wxGetTopLevelParent(this);
    if ( newTLW != oldTLW )
    {
        if ( oldTLW != nullptr && !oldTLW->IsBeingDeleted() )
        {
            oldTLW->Unbind(wxEVT_CHAR_HOOK, &wxRibbonBar::OnKeyTipsCharHook, this);
            oldTLW->Unbind(wxEVT_ACTIVATE, &wxRibbonBar::OnKeyTipsActivate, this);
            oldTLW->Unbind(wxEVT_DESTROY, &wxRibbonBar::OnKeyTipsWindowDestroy, this);
        }
        m_keyTipsTopLevelParent = newTLW;
        if ( m_keyTipsTopLevelParent != nullptr )
        {
            m_keyTipsTopLevelParent->Bind(wxEVT_CHAR_HOOK, &wxRibbonBar::OnKeyTipsCharHook, this);
            m_keyTipsTopLevelParent->Bind(wxEVT_ACTIVATE, &wxRibbonBar::OnKeyTipsActivate, this);
            m_keyTipsTopLevelParent->Bind(wxEVT_DESTROY, &wxRibbonBar::OnKeyTipsWindowDestroy, this);
        }
    }
    return res;
}

void wxRibbonBar::OnKeyTipsActivate(wxActivateEvent& event)
{
    event.Skip();
    if ( m_keyTipsActive && !event.GetActive() )
        HideKeyTips();
}

bool wxRibbonBar::IsKeyTipsTriggerKey(int keyCode, int modifiers) const
{
    for ( const auto& trigger : m_keyTipsTriggerKeys )
    {
        if ( keyCode == trigger.keyCode && modifiers == trigger.modifiers )
            return true;
    }
    return false;
}

void wxRibbonBar::OnKeyTipsCharHook(wxKeyEvent& event)
{
    if ( !m_keyTipsActive )
    {
        // Only steal the trigger key if there's something to show, so
        // ribbon bars with no keytips configured stay backward compatible.
        if ( IsKeyTipsTriggerKey(event.GetKeyCode(), event.GetModifiers()) &&
             ShowKeyTips() )
        {
            return;
        }
        event.Skip();
        return;
    }

    // KeyTips mode is active.
    int keycode = event.GetKeyCode();

    if ( keycode == WXK_ESCAPE )
    {
        if ( !m_keyTipsTypedPrefix.empty() )
        {
            m_keyTipsTypedPrefix.clear();
            DoBuildKeyTipTargets();
            RefreshKeyTipTargetWindows();
        }
        else
        {
            HideKeyTips();
        }
        return;
    }

    if ( IsKeyTipsTriggerKey(keycode, event.GetModifiers()) )
    {
        HideKeyTips();
        return;
    }

    // Accelerators aren't keytips: leave the mode and let the key through,
    // otherwise Ctrl+S would fire the "S" keytip and Alt+F4 would be eaten.
    if ( event.GetModifiers() & ~wxMOD_SHIFT )
    {
        HideKeyTips();
        event.Skip();
        return;
    }

    if ( keycode > WXK_SPACE && keycode <= 255 && wxIsalnum((wxChar)keycode) )
    {
        wxUniChar ch = wxToupper((wxChar)keycode);

        std::vector<wxRibbonKeyTipInfo> matches;
        for ( const auto& target : m_keyTipsTargets )
        {
            if ( target.remaining.empty() || target.remaining[0] != ch )
                continue;

            if ( target.window == nullptr || target.window->IsBeingDeleted() )
                continue;
            // The window may have been hidden since the targets were built.
            if ( !target.window->IsShownOnScreen() )
                continue;

            matches.push_back(target);
        }

        if ( matches.empty() )
        {
            // No match: consume the key, but otherwise do nothing.
            return;
        }

        for ( auto& target : matches )
            target.remaining = target.remaining.Mid(1);

        if ( matches.size() == 1 && matches[0].remaining.empty() )
        {
            wxRibbonKeyTipInfo activated = matches[0];
            HideKeyTips();
            DoActivateKeyTipTarget(activated);
        }
        else
        {
            m_keyTipsTargets = matches;
            m_keyTipsTypedPrefix += ch;
            RefreshKeyTipTargetWindows();
        }
        return;
    }

    // Any other key: swallow it while keytips are active.
}

#endif // wxUSE_RIBBON
