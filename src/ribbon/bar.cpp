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
wxEND_EVENT_TABLE()

void wxRibbonBar::AddPage(wxRibbonPage *page)
{
    wxRibbonPageTabInfo info;

    info.m_page = page;
    info.m_active = false;
    info.m_hovered = false;
    info.m_highlight = false;
    info.m_shown = true;
    // info.rect not set (intentional)

    wxInfoDC dcTemp(this);
    wxString label;
    if ( m_flags & wxRIBBON_BAR_SHOW_PAGE_LABELS )
        label = page->GetLabel();
    wxBitmap icon;
    if ( m_flags & wxRIBBON_BAR_SHOW_PAGE_ICONS )
        icon = page->GetIcon();
    m_art->GetBarTabWidth(dcTemp, this, label, icon,
                          &info.m_idealWidth,
                          &info.m_smallBeginNeedSeparatorWidth,
                          &info.m_smallMustHaveSeparatorWidth,
                          &info.m_minimumWidth);

    if ( m_pages.IsEmpty() )
    {
        m_tabsTotalWidthIdeal = info.m_idealWidth;
        m_tabsTotalWidthMinimum = info.m_minimumWidth;
    }
    else
    {
        int sep = m_art->GetMetric(wxRIBBON_ART_TAB_SEPARATION_SIZE);
        m_tabsTotalWidthIdeal += sep + info.m_idealWidth;
        m_tabsTotalWidthMinimum += sep + info.m_minimumWidth;
    }
    m_pages.Add(info);

    page->Hide(); // Most likely case is that this new page is not the active tab
    page->SetArtProvider(m_art);

    if ( m_pages.GetCount() == 1 )
    {
        SetActivePage((size_t)0);
    }
}

bool wxRibbonBar::DismissExpandedPanel()
{
    if ( m_currentPage == wxNOT_FOUND )
        return false;
    return m_pages.Item(m_currentPage).m_page->DismissExpandedPanel();
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

    m_ribbonState = mode;
}


void wxRibbonBar::ShowPanels(bool show)
{
    ShowPanels( show ? wxRIBBON_BAR_PINNED : wxRIBBON_BAR_MINIMIZED );
}

void wxRibbonBar::SetWindowStyleFlag(long style)
{
    m_flags = style;
    if ( m_art )
        m_art->SetFlags(style);
}

long wxRibbonBar::GetWindowStyleFlag() const
{
    return m_flags;
}

bool wxRibbonBar::Realize()
{
    bool status = true;

    wxInfoDC dcTemp(this);
    int sep = m_art->GetMetric(wxRIBBON_ART_TAB_SEPARATION_SIZE);
    size_t numTabs = m_pages.GetCount();
    bool firstVisible = true;
    size_t i;
    for ( i = 0; i < numTabs; ++i )
    {
        wxRibbonPageTabInfo& info = m_pages.Item(i);
        if ( !info.m_shown )
            continue;
        RepositionPage(info.m_page);
        if ( !info.m_page->Realize() )
        {
            status = false;
        }
        wxString label;
        if ( m_flags & wxRIBBON_BAR_SHOW_PAGE_LABELS )
            label = info.m_page->GetLabel();
        wxBitmap icon;
        if ( m_flags & wxRIBBON_BAR_SHOW_PAGE_ICONS )
            icon = info.m_page->GetIcon();
        m_art->GetBarTabWidth(dcTemp, this, label, icon,
                              &info.m_idealWidth,
                              &info.m_smallBeginNeedSeparatorWidth,
                              &info.m_smallMustHaveSeparatorWidth,
                              &info.m_minimumWidth);

        if ( firstVisible )
        {
            firstVisible = false;

            m_tabsTotalWidthIdeal = info.m_idealWidth;
            m_tabsTotalWidthMinimum = info.m_minimumWidth;
        }
        else
        {
            m_tabsTotalWidthIdeal += sep + info.m_idealWidth;
            m_tabsTotalWidthMinimum += sep + info.m_minimumWidth;
        }
    }
    m_tabHeight = m_art->GetTabCtrlHeight(dcTemp, this, m_pages);

    RecalculateMinSize();
    RecalculateTabSizes();
    Refresh();

    return status;
}

void wxRibbonBar::OnMouseMove(wxMouseEvent& evt)
{
    int x = evt.GetX();
    int y = evt.GetY();
    int hoveredPage = wxNOT_FOUND;
    bool refreshTabs = false;
    if ( y < m_tabHeight )
    {
        // It is quite likely that the mouse moved a small amount and is still over the same tab
        if ( m_currentHoveredPage != wxNOT_FOUND &&
            m_pages.Item((size_t)m_currentHoveredPage).m_rect.Contains(x, y) )
        {
            hoveredPage = m_currentHoveredPage;
            // But be careful, if tabs can be scrolled, then parts of the tab rect may not be valid
            if ( m_tabScrollButtonsShown )
            {
                if ( x >= m_tabScrollRightButtonRect.GetX() || x < m_tabScrollLeftButtonRect.GetRight() )
                {
                    hoveredPage = wxNOT_FOUND;
                }
            }
        }
        else
        {
            HitTestTabs(evt.GetPosition(), &hoveredPage);
        }
    }
    if ( hoveredPage != m_currentHoveredPage )
    {
        if ( m_currentHoveredPage != wxNOT_FOUND )
        {
            m_pages.Item((int)m_currentHoveredPage).m_hovered = false;
        }
        m_currentHoveredPage = hoveredPage;
        if ( m_currentHoveredPage != wxNOT_FOUND )
        {
            m_pages.Item((int)m_currentHoveredPage).m_hovered = true;
        }
        refreshTabs = true;
    }
    if ( m_tabScrollButtonsShown )
    {
#define SET_FLAG(variable, flag) \
    { if ( ((variable) & (flag)) != (flag) ) { variable |= (flag); refreshTabs = true; }}
#define UNSET_FLAG(variable, flag) \
    { if ( (variable) & (flag) ) { variable &= ~(flag); refreshTabs = true; }}

        if ( m_tabScrollLeftButtonRect.Contains(x, y) )
            SET_FLAG(m_tabScrollLeftButtonState, wxRIBBON_SCROLL_BTN_HOVERED)
        else
            UNSET_FLAG(m_tabScrollLeftButtonState, wxRIBBON_SCROLL_BTN_HOVERED)

        if ( m_tabScrollRightButtonRect.Contains(x, y) )
            SET_FLAG(m_tabScrollRightButtonState, wxRIBBON_SCROLL_BTN_HOVERED)
        else
            UNSET_FLAG(m_tabScrollRightButtonState, wxRIBBON_SCROLL_BTN_HOVERED)
#undef SET_FLAG
#undef UNSET_FLAG
    }
    if ( refreshTabs )
    {
        RefreshTabBar();
    }
    if ( m_flags & wxRIBBON_BAR_SHOW_TOGGLE_BUTTON )
        HitTestRibbonButton(m_toggleButtonRect, evt.GetPosition(), m_toggleButtonHovered);
    if ( m_flags & wxRIBBON_BAR_SHOW_HELP_BUTTON )
        HitTestRibbonButton(m_helpButtonRect, evt.GetPosition(), m_helpButtonHovered);
}

void wxRibbonBar::OnMouseLeave(wxMouseEvent& WXUNUSED(evt))
{
    // The ribbon bar is (usually) at the top of a window, and at least on MSW, the mouse
    // can leave the window quickly and leave a tab in the hovered state.
    bool refreshTabs = false;
    if ( m_currentHoveredPage != wxNOT_FOUND )
    {
        m_pages.Item((int)m_currentHoveredPage).m_hovered = false;
        m_currentHoveredPage = wxNOT_FOUND;
        refreshTabs = true;
    }
    if ( m_tabScrollLeftButtonState & wxRIBBON_SCROLL_BTN_HOVERED )
    {
        m_tabScrollLeftButtonState &= ~wxRIBBON_SCROLL_BTN_HOVERED;
        refreshTabs = true;
    }
    if ( m_tabScrollRightButtonState & wxRIBBON_SCROLL_BTN_HOVERED )
    {
        m_tabScrollRightButtonState &= ~wxRIBBON_SCROLL_BTN_HOVERED;
        refreshTabs = true;
    }
    if ( refreshTabs )
    {
        RefreshTabBar();
    }
    if ( m_toggleButtonHovered )
    {
        m_barHovered = false;
        m_toggleButtonHovered = false;
        Refresh(false);
    }
    if ( m_helpButtonHovered )
    {
        m_helpButtonHovered = false;
        m_barHovered = false;
        Refresh(false);
    }
}

wxRibbonPage* wxRibbonBar::GetPage(int n)
{
    if ( n < 0 || (size_t)n >= m_pages.GetCount() )
        return nullptr;
    return m_pages.Item(n).m_page;
}

wxRibbonPage* wxRibbonBar::GetPageById(wxWindowID id)
{
    for ( const auto& page : m_pages )
    {
        if ( page.m_page->GetId() == id )
            return page.m_page;
    }
    return nullptr;
}

size_t wxRibbonBar::GetPageCount() const
{
    return m_pages.GetCount();
}

bool wxRibbonBar::IsPageShown(size_t page) const
{
    if ( page >= m_pages.GetCount() )
        return false;
    return m_pages.Item(page).m_shown;
}

void wxRibbonBar::ShowPage(size_t page, bool show)
{
    if ( page >= m_pages.GetCount() )
        return;
    m_pages.Item(page).m_shown = show;
}

bool wxRibbonBar::IsPageHighlighted(size_t page) const
{
    if ( page >= m_pages.GetCount() )
        return false;
    return m_pages.Item(page).m_highlight;
}

void wxRibbonBar::AddPageHighlight(size_t page, bool highlight)
{
    if ( page >= m_pages.GetCount() )
        return;
    m_pages.Item(page).m_highlight = highlight;
}

void wxRibbonBar::DeletePage(size_t n)
{
    if ( n < m_pages.GetCount() )
    {
        wxRibbonPage *page = m_pages.Item(n).m_page;

        // Schedule page object for destruction and not destroying directly
        // as this function can be called in an event handler and page functions
        // can be called after removing.
        // Like in wxRibbonButtonBar::OnMouseUp
        if ( !wxTheApp->IsScheduledForDestruction(page) )
        {
            wxTheApp->ScheduleForDestruction(page);
        }

        m_pages.RemoveAt(n);

        if ( m_currentPage == static_cast<int>(n) )
        {
            m_currentPage = wxNOT_FOUND;

            if ( m_pages.GetCount() > 0 )
                SetActivePage(wxMin(n, m_pages.GetCount() - 1));
        }
        else if ( m_currentPage > static_cast<int>(n) )
        {
            m_currentPage--;
        }
    }
}

void wxRibbonBar::ClearPages()
{
    size_t i;
    for ( i = 0; i <m_pages.GetCount(); i++ )
    {
        wxRibbonPage *page = m_pages.Item(i).m_page;
        // Schedule page object for destruction and not destroying directly
        // as this function can be called in an event handler and page functions
        // can be called after removing.
        // Like in wxRibbonButtonBar::OnMouseUp
        if ( !wxTheApp->IsScheduledForDestruction(page) )
        {
            wxTheApp->ScheduleForDestruction(page);
        }
    }
    m_pages.Empty();
    Realize();
    m_currentPage = wxNOT_FOUND;
    Refresh();
}

bool wxRibbonBar::SetActivePage(size_t page)
{
    if ( m_currentPage == (int)page )
    {
        return true;
    }

    if ( page >= m_pages.GetCount() )
    {
        return false;
    }

    if ( m_currentPage != wxNOT_FOUND )
    {
        m_pages.Item((size_t)m_currentPage).m_active = false;
        m_pages.Item((size_t)m_currentPage).m_page->Hide();
    }
    m_currentPage = (int)page;
    m_pages.Item(page).m_active = true;
    m_pages.Item(page).m_shown = true;
    {
        wxRibbonPage* wnd = m_pages.Item(page).m_page;
        RepositionPage(wnd);
        wnd->Layout();
        wnd->Show();
    }
    Refresh();

    return true;
}

bool wxRibbonBar::SetActivePage(wxRibbonPage* page)
{
    size_t numPages = m_pages.GetCount();
    size_t i;
    for ( i = 0; i < numPages; ++i )
    {
        if ( m_pages.Item(i).m_page == page )
        {
            return SetActivePage(i);
        }
    }
    return false;
}

int wxRibbonBar::GetPageNumber(wxRibbonPage* page) const
{
    size_t numPages = m_pages.GetCount();
    for ( size_t i = 0; i < numPages; ++i )
    {
        if ( m_pages.Item(i).m_page == page )
        {
            return i;
        }
    }
    return wxNOT_FOUND;
}


int wxRibbonBar::GetActivePage() const
{
    return m_currentPage;
}

void wxRibbonBar::SetTabCtrlMargins(int left, int right)
{
    m_tabMarginLeft = left;
    m_tabMarginRight = right;

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
        return m_page->m_smallMustHaveSeparatorWidth
                < other.m_page->m_smallMustHaveSeparatorWidth;
    }

    wxRibbonPageTabInfo *m_page;
};

void wxRibbonBar::RecalculateTabSizes()
{
    size_t numTabs = m_pages.GetCount();

    if ( numTabs == 0 )
        return;

    int width = GetSize().GetWidth() - m_tabMarginLeft - m_tabMarginRight;
    int tabSep = m_art->GetMetric(wxRIBBON_ART_TAB_SEPARATION_SIZE);
    int x = m_tabMarginLeft;
    const int y = 0;

    if ( width >= m_tabsTotalWidthIdeal )
    {
        // Simple case: everything at ideal width
        size_t i;
        for ( i = 0; i < numTabs; ++i )
        {
            wxRibbonPageTabInfo& info = m_pages.Item(i);
            if ( !info.m_shown )
                continue;
            info.m_rect.x = x;
            info.m_rect.y = y;
            info.m_rect.width = info.m_idealWidth;
            info.m_rect.height = m_tabHeight;
            x += info.m_rect.width + tabSep;
        }
        m_tabScrollButtonsShown = false;
        m_tabScrollLeftButtonRect.SetWidth(0);
        m_tabScrollRightButtonRect.SetWidth(0);
    }
    else if ( width < m_tabsTotalWidthMinimum )
    {
        // Simple case: everything minimum with scrollbar
        size_t i;
        for ( i = 0; i < numTabs; ++i )
        {
            wxRibbonPageTabInfo& info = m_pages.Item(i);
            if ( !info.m_shown )
                continue;
            info.m_rect.x = x;
            info.m_rect.y = y;
            info.m_rect.width = info.m_minimumWidth;
            info.m_rect.height = m_tabHeight;
            x += info.m_rect.width + tabSep;
        }
        if ( !m_tabScrollButtonsShown )
        {
            m_tabScrollLeftButtonState = wxRIBBON_SCROLL_BTN_NORMAL;
            m_tabScrollRightButtonState = wxRIBBON_SCROLL_BTN_NORMAL;
            m_tabScrollButtonsShown = true;
        }
        {
            wxInfoDC tempDc(this);
            int rightButtonPos = GetClientSize().GetWidth() - m_tabMarginRight - m_tabScrollRightButtonRect.GetWidth();
            if ( rightButtonPos < m_tabMarginLeft )
                rightButtonPos = m_tabMarginLeft;

            m_tabScrollLeftButtonRect.SetWidth(m_art->GetScrollButtonMinimumSize(tempDc, this, wxRIBBON_SCROLL_BTN_LEFT | wxRIBBON_SCROLL_BTN_NORMAL | wxRIBBON_SCROLL_BTN_FOR_TABS).GetWidth());
            m_tabScrollLeftButtonRect.SetHeight(m_tabHeight);
            m_tabScrollLeftButtonRect.SetX(m_tabMarginLeft);
            m_tabScrollLeftButtonRect.SetY(0);
            m_tabScrollRightButtonRect.SetWidth(m_art->GetScrollButtonMinimumSize(tempDc, this, wxRIBBON_SCROLL_BTN_RIGHT | wxRIBBON_SCROLL_BTN_NORMAL | wxRIBBON_SCROLL_BTN_FOR_TABS).GetWidth());
            m_tabScrollRightButtonRect.SetHeight(m_tabHeight);
            m_tabScrollRightButtonRect.SetX(rightButtonPos);
            m_tabScrollRightButtonRect.SetY(0);
        }
        if ( m_tabScrollAmount == 0 )
        {
            m_tabScrollLeftButtonRect.SetWidth(0);
        }
        else if ( m_tabScrollAmount + width >= m_tabsTotalWidthMinimum )
        {
            m_tabScrollAmount = m_tabsTotalWidthMinimum - width;
            m_tabScrollRightButtonRect.SetX(m_tabScrollRightButtonRect.GetX() + m_tabScrollRightButtonRect.GetWidth());
            m_tabScrollRightButtonRect.SetWidth(0);
        }
        for ( i = 0; i < numTabs; ++i )
        {
            wxRibbonPageTabInfo& info = m_pages.Item(i);
            if ( !info.m_shown )
                continue;
            info.m_rect.x -= m_tabScrollAmount;
        }
    }
    else
    {
        m_tabScrollButtonsShown = false;
        m_tabScrollLeftButtonRect.SetWidth(0);
        m_tabScrollRightButtonRect.SetWidth(0);
        // Complex case: everything sized such that: minimum <= width < ideal
        /*
           Strategy:
             1) Uniformly reduce all tab widths from ideal to smallMustHaveSeparatorWidth
             2) Reduce the largest tab by 1 pixel, repeating until all tabs are same width (or at minimum)
             3) Uniformly reduce all tabs down to their minimum width
        */
        int smallestTabWidth = INT_MAX;
        int totalSmallWidth = tabSep * (numTabs - 1);
        size_t i;
        for ( i = 0; i < numTabs; ++i )
        {
            wxRibbonPageTabInfo& info = m_pages.Item(i);
            if ( !info.m_shown )
                continue;
            if ( info.m_smallMustHaveSeparatorWidth < smallestTabWidth )
            {
                smallestTabWidth = info.m_smallMustHaveSeparatorWidth;
            }
            totalSmallWidth += info.m_smallMustHaveSeparatorWidth;
        }
        if ( width >= totalSmallWidth )
        {
            // Do (1)
            int totalDelta = m_tabsTotalWidthIdeal - totalSmallWidth;
            totalSmallWidth -= tabSep * (numTabs - 1);
            width -= tabSep * (numTabs - 1);
            for ( i = 0; i < numTabs; ++i )
            {
                wxRibbonPageTabInfo& info = m_pages.Item(i);
                if ( !info.m_shown )
                    continue;
                int delta = info.m_idealWidth - info.m_smallMustHaveSeparatorWidth;
                info.m_rect.x = x;
                info.m_rect.y = y;
                info.m_rect.width = info.m_smallMustHaveSeparatorWidth + delta * (width - totalSmallWidth) / totalDelta;
                info.m_rect.height = m_tabHeight;

                x += info.m_rect.width + tabSep;
                totalDelta -= delta;
                totalSmallWidth -= info.m_smallMustHaveSeparatorWidth;
                width -= info.m_rect.width;
            }
        }
        else
        {
            totalSmallWidth = tabSep * (numTabs - 1);
            for ( i = 0; i < numTabs; ++i )
            {
                wxRibbonPageTabInfo& info = m_pages.Item(i);
                if ( !info.m_shown )
                    continue;
                if ( info.m_minimumWidth < smallestTabWidth )
                {
                    totalSmallWidth += smallestTabWidth;
                }
                else
                {
                    totalSmallWidth += info.m_minimumWidth;
                }
            }
            if ( width >= totalSmallWidth )
            {
                // Do (2)
                wxVector<PageComparedBySmallWidthAsc> sortedPages;
                sortedPages.reserve(numTabs);
                for ( i = 0; i < numTabs; ++i )
                    sortedPages.push_back(PageComparedBySmallWidthAsc(&m_pages.Item(i)));

                wxVectorSort(sortedPages);
                width -= tabSep * (numTabs - 1);
                for ( i = 0; i < numTabs; ++i )
                {
                    wxRibbonPageTabInfo* info = sortedPages[i].m_page;
                    if ( !info->m_shown )
                        continue;
                    if ( info->m_smallMustHaveSeparatorWidth * (int)(numTabs - i) <= width )
                    {
                        info->m_rect.width = info->m_smallMustHaveSeparatorWidth;
                    }
                    else
                    {
                        info->m_rect.width = width / (numTabs - i);
                    }
                    width -= info->m_rect.width;
                }
                for ( i = 0; i < numTabs; ++i )
                {
                    wxRibbonPageTabInfo& info = m_pages.Item(i);
                    if ( !info.m_shown )
                        continue;
                    info.m_rect.x = x;
                    info.m_rect.y = y;
                    info.m_rect.height = m_tabHeight;
                    x += info.m_rect.width + tabSep;
                }
            }
            else
            {
                // Do (3)
                totalSmallWidth = (smallestTabWidth + tabSep) * numTabs - tabSep;
                int totalDelta = totalSmallWidth - m_tabsTotalWidthMinimum;
                totalSmallWidth = m_tabsTotalWidthMinimum - tabSep * (numTabs - 1);
                width -= tabSep * (numTabs - 1);
                for ( i = 0; i < numTabs; ++i )
                {
                    wxRibbonPageTabInfo& info = m_pages.Item(i);
                    if ( !info.m_shown )
                        continue;
                    int delta = smallestTabWidth - info.m_minimumWidth;
                    info.m_rect.x = x;
                    info.m_rect.y = y;
                    info.m_rect.width = info.m_minimumWidth + delta * (width - totalSmallWidth) / totalDelta;
                    info.m_rect.height = m_tabHeight;

                    x += info.m_rect.width + tabSep;
                    totalDelta -= delta;
                    totalSmallWidth -= info.m_minimumWidth;
                    width -= info.m_rect.width;
                }
            }
        }
    }
}

wxRibbonBar::wxRibbonBar()
{
    m_flags = 0;
    m_tabsTotalWidthIdeal = 0;
    m_tabsTotalWidthMinimum = 0;
    m_tabMarginLeft = 0;
    m_tabMarginRight = 0;
    m_tabHeight = 0;
    m_tabScrollAmount = 0;
    m_currentPage = wxNOT_FOUND;
    m_currentHoveredPage = wxNOT_FOUND;
    m_tabScrollLeftButtonState = wxRIBBON_SCROLL_BTN_NORMAL;
    m_tabScrollRightButtonState = wxRIBBON_SCROLL_BTN_NORMAL;
    m_tabScrollButtonsShown = false;
    m_arePanelsShown = true;
    m_helpButtonHovered = false;

}

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
    SetArtProvider(nullptr);

    for ( size_t n = 0; n < m_imageLists.size(); ++n )
    {
        delete m_imageLists[n];
    }
}

bool wxRibbonBar::Create(wxWindow* parent,
                wxWindowID id,
                const wxPoint& pos,
                const wxSize& size,
                long style)
{
    if ( !wxRibbonControl::Create(parent, id, pos, size, wxBORDER_NONE) )
        return false;

    CommonInit(style);

    return true;
}

void wxRibbonBar::CommonInit(long style)
{
    SetName("wxRibbonBar");

    m_flags = style;
    m_tabsTotalWidthIdeal = 0;
    m_tabsTotalWidthMinimum = 0;
    m_tabMarginLeft = 50;
    m_tabMarginRight = 20;
    if ( m_flags & wxRIBBON_BAR_SHOW_TOGGLE_BUTTON )
        m_tabMarginRight += 20;
    if ( m_flags & wxRIBBON_BAR_SHOW_HELP_BUTTON )
        m_tabMarginRight += 20;
    m_tabHeight = 20; // initial guess
    m_tabScrollAmount = 0;
    m_currentPage = wxNOT_FOUND;
    m_currentHoveredPage = wxNOT_FOUND;
    m_tabScrollLeftButtonState = wxRIBBON_SCROLL_BTN_NORMAL;
    m_tabScrollRightButtonState = wxRIBBON_SCROLL_BTN_NORMAL;
    m_tabScrollButtonsShown = false;
    m_arePanelsShown = true;

    if ( m_art == nullptr )
    {
        SetArtProvider(new wxRibbonDefaultArtProvider);
    }
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    m_toggleButtonHovered = false;
    m_barHovered = false;

    m_ribbonState = wxRIBBON_BAR_PINNED;
}

wxImageList* wxRibbonBar::GetButtonImageList(wxSize size, int initialCount)
{
    for ( size_t n = 0; n < m_imageLists.size(); ++n )
    {
        if ( m_imageLists[n]->GetSize() == size )
            return m_imageLists[n];
    }

    wxImageList* const
        il = new wxImageList(size.GetWidth(), size.GetHeight(), /*mask*/false, initialCount);
    m_imageLists.push_back(il);

    return il;
}

void wxRibbonBar::SetArtProvider(wxRibbonArtProvider* art)
{
    wxRibbonArtProvider *old = m_art;
    m_art = art;

    if ( art )
    {
        art->SetFlags(m_flags);
    }
    size_t numPages = m_pages.GetCount();
    size_t i;
    for ( i = 0; i < numPages; ++i )
    {
        wxRibbonPage *page = m_pages.Item(i).m_page;
        if ( page->GetArtProvider() != art )
        {
            page->SetArtProvider(art);
        }
    }

    delete old;
}

void wxRibbonBar::OnPaint(wxPaintEvent& WXUNUSED(evt))
{
    wxAutoBufferedPaintDC dc(this);

    if ( GetUpdateRegion().Contains(0, 0, GetClientSize().GetWidth(), m_tabHeight) == wxOutRegion )
    {
        // Nothing to do in the tab area, and the page area is handled by the active page
        return;
    }

    DoEraseBackground(dc);

    if ( m_flags & wxRIBBON_BAR_SHOW_HELP_BUTTON )
        m_helpButtonRect = m_art->GetRibbonHelpButtonArea(GetSize());
    if ( m_flags & wxRIBBON_BAR_SHOW_TOGGLE_BUTTON )
        m_toggleButtonRect = m_art->GetBarToggleButtonArea(GetSize());

    size_t numTabs = m_pages.GetCount();
    double sepVisibility = 0.0;
    bool drawSep = false;
    wxRect tabsRect(m_tabMarginLeft, 0, GetClientSize().GetWidth() - m_tabMarginLeft - m_tabMarginRight, m_tabHeight);
    if ( m_tabScrollButtonsShown )
    {
        tabsRect.x += m_tabScrollLeftButtonRect.GetWidth();
        tabsRect.width -= m_tabScrollLeftButtonRect.GetWidth() + m_tabScrollRightButtonRect.GetWidth();
    }
    size_t i;
    for ( i = 0; i < numTabs; ++i )
    {
        wxRibbonPageTabInfo& info = m_pages.Item(i);
        if ( !info.m_shown )
            continue;

        dc.DestroyClippingRegion();
        if ( m_tabScrollButtonsShown )
        {
            if ( !tabsRect.Intersects(info.m_rect) )
                continue;
            dc.SetClippingRegion(tabsRect);
        }
        dc.SetClippingRegion(info.m_rect);
        m_art->DrawTab(dc, this, info);

        if ( info.m_rect.width < info.m_smallBeginNeedSeparatorWidth )
        {
            drawSep = true;
            if ( info.m_rect.width < info.m_smallMustHaveSeparatorWidth )
            {
                sepVisibility += 1.0;
            }
            else
            {
                sepVisibility += (double)(info.m_smallBeginNeedSeparatorWidth - info.m_rect.width) /
                                 (double)(info.m_smallBeginNeedSeparatorWidth - info.m_smallMustHaveSeparatorWidth);
            }
        }
    }
    if ( drawSep )
    {
        wxRect rect = m_pages.Item(0).m_rect;
        rect.width = m_art->GetMetric(wxRIBBON_ART_TAB_SEPARATION_SIZE);
        sepVisibility /= (double)numTabs;
        for ( i = 0; i < numTabs - 1; ++i )
        {
            wxRibbonPageTabInfo& info = m_pages.Item(i);
            if ( !info.m_shown )
                continue;
            rect.x = info.m_rect.x + info.m_rect.width;

            if ( m_tabScrollButtonsShown && !tabsRect.Intersects(rect) )
            {
                continue;
            }

            dc.DestroyClippingRegion();
            dc.SetClippingRegion(rect);
            m_art->DrawTabSeparator(dc, this, rect, sepVisibility);
        }
    }
    if ( m_tabScrollButtonsShown )
    {
        if ( m_tabScrollLeftButtonRect.GetWidth() != 0 )
        {
            dc.DestroyClippingRegion();
            dc.SetClippingRegion(m_tabScrollLeftButtonRect);
            m_art->DrawScrollButton(dc, this, m_tabScrollLeftButtonRect, wxRIBBON_SCROLL_BTN_LEFT | m_tabScrollLeftButtonState | wxRIBBON_SCROLL_BTN_FOR_TABS);
        }
        if ( m_tabScrollRightButtonRect.GetWidth() != 0 )
        {
            dc.DestroyClippingRegion();
            dc.SetClippingRegion(m_tabScrollRightButtonRect);
            m_art->DrawScrollButton(dc, this, m_tabScrollRightButtonRect, wxRIBBON_SCROLL_BTN_RIGHT | m_tabScrollRightButtonState | wxRIBBON_SCROLL_BTN_FOR_TABS);
        }
    }

    if ( m_flags & wxRIBBON_BAR_SHOW_HELP_BUTTON )
        m_art->DrawHelpButton(dc, this, m_helpButtonRect);
    if ( m_flags & wxRIBBON_BAR_SHOW_TOGGLE_BUTTON )
        m_art->DrawToggleButton(dc, this, m_toggleButtonRect, m_ribbonState);

}

void wxRibbonBar::OnEraseBackground(wxEraseEvent& WXUNUSED(evt))
{
    // Background painting done in main paint handler to reduce screen flicker
}

void wxRibbonBar::DoEraseBackground(wxDC& dc)
{
    wxRect tabs(GetSize());
    tabs.height = m_tabHeight;
    m_art->DrawTabCtrlBackground(dc, this, tabs);
}

void wxRibbonBar::OnSize(wxSizeEvent& evt)
{
    RecalculateTabSizes();
    if ( m_currentPage != wxNOT_FOUND )
    {
        RepositionPage(m_pages.Item(m_currentPage).m_page);
    }
    RefreshTabBar();

    evt.Skip();
}

void wxRibbonBar::OnDPIChanged(wxDPIChangedEvent& event)
{
    // Recalculate tab sizes for new DPI
    RecalculateTabSizes();

    // Realize all pages to update their layouts
    size_t pageCount = m_pages.GetCount();
    for ( size_t i = 0; i < pageCount; ++i )
    {
        m_pages.Item(i).m_page->Realize();
    }

    // Reposition current page
    if ( m_currentPage != wxNOT_FOUND )
    {
        RepositionPage(m_pages.Item(m_currentPage).m_page);
    }

    Refresh();
    event.Skip();
}

void wxRibbonBar::RepositionPage(wxRibbonPage *page)
{
    int w, h;
    GetSize(&w, &h);
    page->SetSizeWithScrollButtonAdjustment(0, m_tabHeight, w, h - m_tabHeight);
}

wxRibbonPageTabInfo* wxRibbonBar::HitTestTabs(wxPoint position, int* index)
{
    wxRect tabsRect(m_tabMarginLeft, 0, GetClientSize().GetWidth() - m_tabMarginLeft - m_tabMarginRight, m_tabHeight);
    if ( m_tabScrollButtonsShown )
    {
        tabsRect.SetX(tabsRect.GetX() + m_tabScrollLeftButtonRect.GetWidth());
        tabsRect.SetWidth(tabsRect.GetWidth() - m_tabScrollLeftButtonRect.GetWidth() - m_tabScrollRightButtonRect.GetWidth());
    }
    if ( tabsRect.Contains(position) )
    {
        size_t numTabs = m_pages.GetCount();
        size_t i;
        for ( i = 0; i < numTabs; ++i )
        {
            wxRibbonPageTabInfo& info = m_pages.Item(i);
            if ( !info.m_shown )
                continue;
            if ( info.m_rect.Contains(position) )
            {
                if ( index != nullptr )
                {
                    *index = (int)i;
                }
                return &info;
            }
        }
    }
    if ( index != nullptr )
    {
        *index = -1;
    }
    return nullptr;
}

void wxRibbonBar::OnMouseLeftDown(wxMouseEvent& evt)
{
    wxRibbonPageTabInfo *tab = HitTestTabs(evt.GetPosition());
    SetFocus();
    if ( tab )
    {
        if ( m_ribbonState == wxRIBBON_BAR_MINIMIZED )
        {
            ShowPanels(wxRIBBON_BAR_EXPANDED);
        }
        else if ( (tab == &m_pages.Item(m_currentPage)) && (m_ribbonState == wxRIBBON_BAR_EXPANDED) )
        {
            HidePanels();
        }
    }
    else
    {
        if ( m_ribbonState == wxRIBBON_BAR_EXPANDED )
        {
            HidePanels();
        }
    }
    if ( tab && tab != &m_pages.Item(m_currentPage) )
    {
        wxRibbonBarEvent query(wxEVT_RIBBONBAR_PAGE_CHANGING, GetId(), tab->m_page);
        query.SetEventObject(this);
        ProcessWindowEvent(query);
        if ( query.IsAllowed() )
        {
            SetActivePage(query.GetPage());

            wxRibbonBarEvent notification(wxEVT_RIBBONBAR_PAGE_CHANGED, GetId(), m_pages.Item(m_currentPage).m_page);
            notification.SetEventObject(this);
            ProcessWindowEvent(notification);
        }
    }
    else if ( tab == nullptr )
    {
        if ( m_tabScrollLeftButtonRect.Contains(evt.GetPosition()) )
        {
            m_tabScrollLeftButtonState |= wxRIBBON_SCROLL_BTN_ACTIVE | wxRIBBON_SCROLL_BTN_HOVERED;
            RefreshTabBar();
        }
        else if ( m_tabScrollRightButtonRect.Contains(evt.GetPosition()) )
        {
            m_tabScrollRightButtonState |= wxRIBBON_SCROLL_BTN_ACTIVE | wxRIBBON_SCROLL_BTN_HOVERED;
            RefreshTabBar();
        }
    }

    wxPoint position = evt.GetPosition();

    if ( position.x >= 0 && position.y >= 0 )
    {
        wxSize size = GetSize();
        if ( position.x < size.GetWidth() && position.y < size.GetHeight() )
        {
            if ( m_toggleButtonRect.Contains(position) )
            {
                ShowPanels(ArePanelsShown() ? wxRIBBON_BAR_MINIMIZED : wxRIBBON_BAR_PINNED);
                wxRibbonBarEvent event(wxEVT_RIBBONBAR_TOGGLED, GetId());
                event.SetEventObject(this);
                ProcessWindowEvent(event);
            }
            if ( m_helpButtonRect.Contains(position) )
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
    if ( !m_tabScrollButtonsShown )
    {
        return;
    }

    int amount = 0;
    if ( m_tabScrollLeftButtonState & wxRIBBON_SCROLL_BTN_ACTIVE )
    {
        amount = -1;
    }
    else if ( m_tabScrollRightButtonState & wxRIBBON_SCROLL_BTN_ACTIVE )
    {
        amount = 1;
    }
    if ( amount != 0 )
    {
        m_tabScrollLeftButtonState &= ~wxRIBBON_SCROLL_BTN_ACTIVE;
        m_tabScrollRightButtonState &= ~wxRIBBON_SCROLL_BTN_ACTIVE;
        ScrollTabBar(amount * 8);
    }
}

void wxRibbonBar::ScrollTabBar(int amount)
{
    bool showLeft = true;
    bool showRight = true;
    if ( m_tabScrollAmount + amount <= 0 )
    {
        amount = -m_tabScrollAmount;
        showLeft = false;
    }
    else if ( m_tabScrollAmount + amount + (GetClientSize().GetWidth() - m_tabMarginLeft - m_tabMarginRight) >= m_tabsTotalWidthMinimum )
    {
        amount = m_tabsTotalWidthMinimum - m_tabScrollAmount - (GetClientSize().GetWidth() - m_tabMarginLeft - m_tabMarginRight);
        showRight = false;
    }
    if ( amount == 0 )
    {
        return;
    }
    m_tabScrollAmount += amount;
    size_t numTabs = m_pages.GetCount();
    size_t i;
    for ( i = 0; i < numTabs; ++i )
    {
        wxRibbonPageTabInfo& info = m_pages.Item(i);
        if ( !info.m_shown )
            continue;
        info.m_rect.SetX(info.m_rect.GetX() - amount);
    }
    if ( showRight != (m_tabScrollRightButtonRect.GetWidth() != 0) ||
        showLeft != (m_tabScrollLeftButtonRect.GetWidth() != 0) )
    {
        wxInfoDC tempDc(this);
        if ( showLeft )
        {
            m_tabScrollLeftButtonRect.SetWidth(m_art->GetScrollButtonMinimumSize(tempDc, this, wxRIBBON_SCROLL_BTN_LEFT | wxRIBBON_SCROLL_BTN_NORMAL | wxRIBBON_SCROLL_BTN_FOR_TABS).GetWidth());
        }
        else
        {
            m_tabScrollLeftButtonRect.SetWidth(0);
        }

        if ( showRight )
        {
            if ( m_tabScrollRightButtonRect.GetWidth() == 0 )
            {
                m_tabScrollRightButtonRect.SetWidth(m_art->GetScrollButtonMinimumSize(tempDc, this, wxRIBBON_SCROLL_BTN_RIGHT | wxRIBBON_SCROLL_BTN_NORMAL | wxRIBBON_SCROLL_BTN_FOR_TABS).GetWidth());
                m_tabScrollRightButtonRect.SetX(m_tabScrollRightButtonRect.GetX() - m_tabScrollRightButtonRect.GetWidth());
            }
        }
        else
        {
            if ( m_tabScrollRightButtonRect.GetWidth() != 0 )
            {
                m_tabScrollRightButtonRect.SetX(m_tabScrollRightButtonRect.GetX() + m_tabScrollRightButtonRect.GetWidth());
                m_tabScrollRightButtonRect.SetWidth(0);
            }
        }
    }

    RefreshTabBar();
}

void wxRibbonBar::RefreshTabBar()
{
    wxRect tabRect(0, 0, GetClientSize().GetWidth(), m_tabHeight);
    Refresh(false, &tabRect);
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
    wxRibbonPageTabInfo *tab = HitTestTabs(evt.GetPosition());
    SetFocus();
    if ( tab && tab == &m_pages.Item(m_currentPage) )
    {
        if ( m_ribbonState == wxRIBBON_BAR_PINNED )
        {
            HidePanels();
        }
        else
        {
            ShowPanels(wxRIBBON_BAR_PINNED);
        }
    }
}

void wxRibbonBar::DoMouseButtonCommon(wxMouseEvent& evt, wxEventType tabEventType)
{
    wxRibbonPageTabInfo *tab = HitTestTabs(evt.GetPosition());
    if ( tab )
    {
        wxRibbonBarEvent notification(tabEventType, GetId(), tab->m_page);
        notification.SetEventObject(this);
        ProcessWindowEvent(notification);
    }
}

void wxRibbonBar::RecalculateMinSize()
{
    wxSize minSize(wxDefaultCoord, wxDefaultCoord);
    size_t numTabs = m_pages.GetCount();
    if ( numTabs != 0 )
    {
        minSize = m_pages.Item(0).m_page->GetMinSize();

        size_t i;
        for ( i = 1; i < numTabs; ++i )
        {
            wxRibbonPageTabInfo& info = m_pages.Item(i);
            if ( !info.m_shown )
                continue;
            wxSize pageMin = info.m_page->GetMinSize();

            minSize.x = wxMax(minSize.x, pageMin.x);
            minSize.y = wxMax(minSize.y, pageMin.y);
        }
    }
    if ( minSize.y != wxDefaultCoord )
    {
        // TODO: Decide on best course of action when min height is unspecified
        // - should we specify it to the tab minimum, or leave it unspecified?
        minSize.IncBy(0, m_tabHeight);
    }

    m_minWidth = minSize.GetWidth();
    m_minHeight = m_arePanelsShown ? minSize.GetHeight() : m_tabHeight;
}

wxSize wxRibbonBar::DoGetBestSize() const
{
    wxSize best(0, 0);
    if ( m_currentPage != wxNOT_FOUND )
    {
        best = m_pages.Item(m_currentPage).m_page->GetBestSize();
    }
    if ( best.GetHeight() == wxDefaultCoord )
    {
        best.SetHeight(m_tabHeight);
    }
    else
    {
        best.IncBy(0, m_tabHeight);
    }
    if ( !m_arePanelsShown )
    {
        best.SetHeight(m_tabHeight);
    }
    return best;
}

void wxRibbonBar::HitTestRibbonButton(const wxRect& rect, const wxPoint& position, bool &hoverFlag)
{
    bool hovered = false;
    if ( position.x >= 0 && position.y >= 0 )
    {
        wxSize size = GetSize();
        if ( position.x < size.GetWidth() && position.y < size.GetHeight() )
        {
            hovered = true;
        }
    }
    if ( hovered )
    {
        bool toggleButtonHovered;
        toggleButtonHovered = rect.Contains(position);

        if ( hovered != m_barHovered || toggleButtonHovered != hoverFlag )
        {
            m_barHovered = hovered;
            hoverFlag = toggleButtonHovered;
            Refresh(false);
        }
    }
}

void wxRibbonBar::HideIfExpanded()
{
    if ( m_ribbonState == wxRIBBON_BAR_EXPANDED )
        HidePanels();
}

void wxRibbonBar::OnKillFocus(wxFocusEvent& WXUNUSED(evt))
{
    HideIfExpanded();
}

#endif // wxUSE_RIBBON
