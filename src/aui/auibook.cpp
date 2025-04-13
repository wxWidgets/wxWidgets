///////////////////////////////////////////////////////////////////////////////
// Name:        src/aui/auibook.cpp
// Purpose:     wxaui: wx advanced user interface - notebook
// Author:      Benjamin I. Williams
// Modified by: Jens Lody
// Created:     2006-06-28
// Copyright:   (C) Copyright 2006, Kirix Corporation, All Rights Reserved
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"


#if wxUSE_AUI

#include "wx/aui/auibook.h"

#ifndef WX_PRECOMP
    #include "wx/settings.h"
    #include "wx/dcclient.h"
    #include "wx/dcmemory.h"
    #include "wx/frame.h"
#endif

#include "wx/aui/serializer.h"
#include "wx/aui/tabmdi.h"
#include "wx/wupdlock.h"

#include "wx/dcbuffer.h" // just for wxALWAYS_NATIVE_DOUBLE_BUFFER

#ifdef __WXMAC__
#include "wx/osx/private.h"
#endif

#include <algorithm>
#include <stack>
#include <unordered_set>

wxDEFINE_EVENT(wxEVT_AUINOTEBOOK_PAGE_CLOSE, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_AUINOTEBOOK_PAGE_CLOSED, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_AUINOTEBOOK_PAGE_CHANGING, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_AUINOTEBOOK_PAGE_CHANGED, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_AUINOTEBOOK_BUTTON, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_AUINOTEBOOK_BEGIN_DRAG, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_AUINOTEBOOK_END_DRAG, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_AUINOTEBOOK_CANCEL_DRAG, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_AUINOTEBOOK_DRAG_MOTION, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_AUINOTEBOOK_ALLOW_DND, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_AUINOTEBOOK_BG_DCLICK, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_AUINOTEBOOK_DRAG_DONE, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_AUINOTEBOOK_TAB_MIDDLE_UP, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_AUINOTEBOOK_TAB_MIDDLE_DOWN, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_AUINOTEBOOK_TAB_RIGHT_UP, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_AUINOTEBOOK_TAB_RIGHT_DOWN, wxAuiNotebookEvent);

wxIMPLEMENT_CLASS(wxAuiNotebook, wxBookCtrlBase);
wxIMPLEMENT_CLASS(wxAuiTabCtrl, wxControl);
wxIMPLEMENT_DYNAMIC_CLASS(wxAuiNotebookEvent, wxBookCtrlEvent);


// Local helper functions
namespace
{

// Build the vector of buttons for the page depending on the notebook flags.
std::vector<wxAuiTabContainerButton> MakePageButtons(unsigned int flags)
{
    std::vector<wxAuiTabContainerButton> buttons;

    // Pin/unpin button can be only shown if one of the styles enabling it is
    // used and depends on the current tab state, so make it hidden by default.
    if (flags & (wxAUI_NB_PIN_ON_ACTIVE_TAB | wxAUI_NB_UNPIN_ON_ALL_PINNED))
        buttons.push_back({wxAUI_BUTTON_PIN, wxRIGHT, wxAUI_BUTTON_STATE_HIDDEN});

    // Close button is hidden by default, it will be shown depending on the
    // exact style used and, for wxAUI_NB_CLOSE_ON_ACTIVE_TAB, on whether the
    // tab is current.
    if (flags & (wxAUI_NB_CLOSE_ON_ALL_TABS | wxAUI_NB_CLOSE_ON_ACTIVE_TAB))
        buttons.push_back({wxAUI_BUTTON_CLOSE, wxRIGHT, wxAUI_BUTTON_STATE_HIDDEN});

    return buttons;
}

} // anonymous namespace


// -- wxAuiTabContainer class implementation --


// wxAuiTabContainer is a class which contains information about each
// tab.  It also can render an entire tab control to a specified DC.
// It's not a window class itself, because this code will be used by
// the wxAuiManager, where it is disadvantageous to have separate
// windows for each tab control in the case of "docked tabs"

// A derived class, wxAuiTabCtrl, is an actual wxWindow-derived window
// which can be used as a tab control in the normal sense.


wxAuiTabContainer::wxAuiTabContainer()
{
    m_tabOffset = 0;
    m_flags = 0;
    m_art = new wxAuiDefaultTabArt;

    AddButton(wxAUI_BUTTON_LEFT, wxLEFT);
    AddButton(wxAUI_BUTTON_RIGHT, wxRIGHT);
    AddButton(wxAUI_BUTTON_WINDOWLIST, wxRIGHT);
    AddButton(wxAUI_BUTTON_CLOSE, wxRIGHT);
}

wxAuiTabContainer::~wxAuiTabContainer()
{
    delete m_art;
}

void wxAuiTabContainer::SetArtProvider(wxAuiTabArt* art)
{
    delete m_art;
    m_art = art;

    if (m_art)
    {
        m_art->SetFlags(m_flags);
    }
}

wxAuiTabArt* wxAuiTabContainer::GetArtProvider() const
{
    return m_art;
}

void wxAuiTabContainer::SetFlags(unsigned int flags)
{
    // check for new close button settings
    RemoveButton(wxAUI_BUTTON_LEFT);
    RemoveButton(wxAUI_BUTTON_RIGHT);
    RemoveButton(wxAUI_BUTTON_WINDOWLIST);
    RemoveButton(wxAUI_BUTTON_CLOSE);


    if ((flags & wxAUI_NB_SCROLL_BUTTONS) && !(flags & wxAUI_NB_MULTILINE))
    {
        AddButton(wxAUI_BUTTON_LEFT, wxLEFT);
        AddButton(wxAUI_BUTTON_RIGHT, wxRIGHT);
    }

    if (flags & wxAUI_NB_WINDOWLIST_BUTTON)
    {
        AddButton(wxAUI_BUTTON_WINDOWLIST, wxRIGHT);
    }

    if (flags & wxAUI_NB_CLOSE_BUTTON)
    {
        AddButton(wxAUI_BUTTON_CLOSE, wxRIGHT);
    }

    // Also synchronize the tabs buttons with the new settings if any of the
    // flags affecting them changed.
    const auto flagsAffectingButtons =
        wxAUI_NB_PIN_ON_ACTIVE_TAB |
        wxAUI_NB_UNPIN_ON_ALL_PINNED |
        wxAUI_NB_CLOSE_ON_ALL_TABS |
        wxAUI_NB_CLOSE_ON_ACTIVE_TAB;
    if ((m_flags & flagsAffectingButtons) != (flags & flagsAffectingButtons))
    {
        for (auto& page : m_pages)
        {
            page.buttons = MakePageButtons(flags);
        }
    }

    if (m_art)
    {
        m_art->SetFlags(flags);
    }

    m_flags = flags;
}

void wxAuiTabContainer::SetNormalFont(const wxFont& font)
{
    m_art->SetNormalFont(font);
}

void wxAuiTabContainer::SetSelectedFont(const wxFont& font)
{
    m_art->SetSelectedFont(font);
}

void wxAuiTabContainer::SetMeasuringFont(const wxFont& font)
{
    m_art->SetMeasuringFont(font);
}

void wxAuiTabContainer::SetColour(const wxColour& colour)
{
    m_art->SetColour(colour);
}

void wxAuiTabContainer::SetActiveColour(const wxColour& colour)
{
    m_art->SetActiveColour(colour);
}

void wxAuiTabContainer::SetRect(const wxRect& rect, wxWindow* wnd)
{
    m_rect = rect;

    if (m_art)
    {
        m_art->SetSizingInfo(rect.GetSize(), m_pages.GetCount(), wnd);
    }
}

void wxAuiTabContainer::SetRowHeight(int rowHeight)
{
    m_tabRowHeight = rowHeight;
}

bool wxAuiTabContainer::AddPage(const wxAuiNotebookPage& info)
{
    return InsertPage(info, m_pages.GetCount());
}

bool wxAuiTabContainer::InsertPage(const wxAuiNotebookPage& info,
                                   size_t idx)
{
    const auto it = m_pages.insert(m_pages.begin() + idx, info);

    it->buttons = MakePageButtons(m_flags);

    // let the art provider know how many pages we have
    if (m_art)
    {
        m_art->SetSizingInfo(m_rect.GetSize(), m_pages.GetCount(), info.window);
    }

    return true;
}

bool wxAuiTabContainer::MovePage(wxWindow* page,
                                 size_t new_idx)
{
    int idx = GetIdxFromWindow(page);
    if (idx == -1)
        return false;

    return MovePage(static_cast<size_t>(idx), new_idx);
}

bool wxAuiTabContainer::MovePage(size_t old_idx, size_t new_idx)
{
    const auto b = m_pages.begin();
    if (old_idx < new_idx)
        std::rotate(b + old_idx, b + old_idx + 1, b + new_idx + 1);
    else if (old_idx > new_idx)
        std::rotate(b + new_idx, b + old_idx, b + old_idx + 1);
    else // nothing to do
        return false;

    return true;
}

bool wxAuiTabContainer::RemovePage(wxWindow* page)
{
    int idx = GetIdxFromWindow(page);
    if (idx == -1)
        return false;

    RemovePageAt(idx);

    return true;
}

void wxAuiTabContainer::RemovePageAt(size_t idx)
{
    wxCHECK_RET( idx < m_pages.GetCount(), "invalid page index" );

    wxWindow* const wnd = m_pages[idx].window;

    m_pages.RemoveAt(idx);

    // let the art provider know how many pages we have
    if (m_art)
    {
        m_art->SetSizingInfo(m_rect.GetSize(), m_pages.GetCount(), wnd);
    }
}

void wxAuiTabContainer::RemoveAll()
{
    m_pages.Clear();
}

bool wxAuiTabContainer::SetActivePage(const wxWindow* wnd)
{
    bool found = false;

    for ( auto& page : m_pages )
    {
        if (page.window == wnd)
        {
            page.active = true;
            found = true;
        }
        else
        {
            page.active = false;
        }
    }

    return found;
}

void wxAuiTabContainer::SetNoneActive()
{
    for ( auto& page : m_pages )
    {
        page.active = false;
    }
}

bool wxAuiTabContainer::SetActivePage(size_t page)
{
    if (page >= m_pages.GetCount())
        return false;

    return SetActivePage(m_pages.Item(page).window);
}

int wxAuiTabContainer::GetActivePage() const
{
    size_t i, page_count = m_pages.GetCount();
    for (i = 0; i < page_count; ++i)
    {
        wxAuiNotebookPage& page = m_pages.Item(i);
        if (page.active)
            return i;
    }

    return -1;
}

wxWindow* wxAuiTabContainer::GetWindowFromIdx(size_t idx) const
{
    if (idx >= m_pages.GetCount())
        return nullptr;

    return m_pages[idx].window;
}

int wxAuiTabContainer::GetIdxFromWindow(const wxWindow* wnd) const
{
    const size_t page_count = m_pages.GetCount();
    for ( size_t i = 0; i < page_count; ++i )
    {
        wxAuiNotebookPage& page = m_pages.Item(i);
        if (page.window == wnd)
            return i;
    }
    return wxNOT_FOUND;
}

wxAuiNotebookPage& wxAuiTabContainer::GetPage(size_t idx)
{
    wxASSERT_MSG(idx < m_pages.GetCount(), wxT("Invalid Page index"));

    return m_pages[idx];
}

const wxAuiNotebookPage& wxAuiTabContainer::GetPage(size_t idx) const
{
    wxASSERT_MSG(idx < m_pages.GetCount(), wxT("Invalid Page index"));

    return m_pages[idx];
}

const wxAuiNotebookPageArray& wxAuiTabContainer::GetPages() const
{
    return m_pages;
}

size_t wxAuiTabContainer::GetPageCount() const
{
    return m_pages.GetCount();
}

void wxAuiTabContainer::AddButton(int id,
                                  int location,
                                  const wxBitmapBundle& WXUNUSED(normalBitmap),
                                  const wxBitmapBundle& WXUNUSED(disabledBitmap))
{
    // We ignore the bitmaps as they are never used currently.
    m_buttons.push_back({id, location});
}

void wxAuiTabContainer::RemoveButton(int id)
{
    for (auto it = m_buttons.begin(); it != m_buttons.end(); ++it)
    {
        if (it->id == id)
        {
            m_buttons.erase(it);
            return;
        }
    }
}



size_t wxAuiTabContainer::GetTabOffset() const
{
    return m_tabOffset;
}

void wxAuiTabContainer::SetTabOffset(size_t offset)
{
    // It doesn't make sense to set the offset when multiple lines of tabs are
    // used as it is not used in this case.
    wxASSERT( !IsFlagSet(wxAUI_NB_MULTILINE) );

    m_tabOffset = offset;
}


void
wxAuiTabContainer::UpdateButtonsState(wxAuiNotebookPage& page, bool forceActive)
{
    const bool isActive = forceActive || page.active;
    for (auto& button : page.buttons)
    {
        bool shown = false;
        switch ( page.kind )
        {
            case wxAuiTabKind::Normal:
            case wxAuiTabKind::Pinned:
                switch ( button.id )
                {
                    case wxAUI_BUTTON_CLOSE:
                        if (IsFlagSet(wxAUI_NB_CLOSE_ON_ALL_TABS))
                        {
                            shown = true;
                        }
                        else if (IsFlagSet(wxAUI_NB_CLOSE_ON_ACTIVE_TAB))
                        {
                            if (isActive)
                                shown = true;
                        }
                        break;

                    case wxAUI_BUTTON_PIN:
                        if (IsFlagSet(wxAUI_NB_PIN_ON_ACTIVE_TAB))
                        {
                            if (isActive)
                            {
                                shown = true;

                                if ( page.kind == wxAuiTabKind::Pinned )
                                    button.curState |= wxAUI_BUTTON_STATE_CHECKED;
                                else
                                    button.curState &= ~wxAUI_BUTTON_STATE_CHECKED;
                            }
                        }

                        // This is not an "else" of the above "if" as when both
                        // styles are used, we show "pin" button only on the
                        // active tab, but also show "unpin" on all the pinned
                        // tabs.
                        if (IsFlagSet(wxAUI_NB_UNPIN_ON_ALL_PINNED))
                        {
                            if ( page.kind == wxAuiTabKind::Pinned )
                            {
                                shown = true;

                                button.curState |= wxAUI_BUTTON_STATE_CHECKED;
                            }
                        }
                        break;
                }
                break;

            case wxAuiTabKind::Locked:
                // A locked page can't be closed or pinned, leave the button
                // in its default hidden state.
                break;
        }

        // Leave the other flags unchanged, this doesn't matter when hiding
        // the button but does when showing it as it could be pressed or
        // under the mouse.
        if ( shown )
            button.curState &= ~wxAUI_BUTTON_STATE_HIDDEN;
        else
            button.curState |= wxAUI_BUTTON_STATE_HIDDEN;
    }
}

// Combined border between the tabs and the buttons in DIPs.
static const int wxAUI_BUTTONS_BORDER = 2;

int wxAuiTabContainer::GetAvailableForTabs(const wxRect& rect,
                                           wxReadOnlyDC& dc,
                                           wxWindow* wnd)
{
    // This function is similar to RenderButtons() but is only used when
    // wxAUI_NB_MULTILINE is on, so we can simplify things here compared to
    // the other functions, notable we can assume that all tabs are visible.

    size_t i;
    const size_t button_count = m_buttons.size();

    int right_buttons_width = 0;

    // measure the buttons on the right side
    for (i = 0; i < button_count; ++i)
    {
        wxAuiTabContainerButton& button = m_buttons.at(button_count - i - 1);

        if (button.location != wxRIGHT)
            continue;
        if (button.curState & wxAUI_BUTTON_STATE_HIDDEN)
            continue;
        if (button.id == wxAUI_BUTTON_RIGHT) // Never used in multi-line mode.
            continue;

        wxRect button_rect = rect;
        button_rect.SetY(1);
        button_rect.SetWidth(rect.width - right_buttons_width);

        const int buttonWidth = m_art->GetButtonRect(dc,
                                                     wnd,
                                                     button_rect,
                                                     button.id,
                                                     button.curState,
                                                     wxRIGHT);

        right_buttons_width += buttonWidth;
    }


    int left_buttons_width = 0;

    // measure the buttons on the left side

    for (i = 0; i < button_count; ++i)
    {
        wxAuiTabContainerButton& button = m_buttons.at(button_count - i - 1);

        if (button.location != wxLEFT)
            continue;
        if (button.curState & wxAUI_BUTTON_STATE_HIDDEN)
            continue;
        if (button.id == wxAUI_BUTTON_LEFT) // Never used in multi-line mode.
            continue;

        wxRect button_rect(left_buttons_width, 1, 1000, rect.height);

        const int buttonWidth = m_art->GetButtonRect(dc,
                                                     wnd,
                                                     button_rect,
                                                     button.id,
                                                     button.curState,
                                                     wxLEFT);

        left_buttons_width += buttonWidth;
    }

    if (left_buttons_width == 0)
        left_buttons_width = m_art->GetIndentSize();

    return rect.width - left_buttons_width - right_buttons_width - wnd->FromDIP(wxAUI_BUTTONS_BORDER);
}

int wxAuiTabContainer::LayoutMultiLineTabs(const wxRect& rect, wxWindow* wnd)
{
    wxInfoDC dc(wnd);

    const int availableWidth = GetAvailableForTabs(rect, dc, wnd);

    int extraHeight = 0;

    int widthRow = 0;
    bool firstTabInRow = true;
    bool* lastRowEnd = nullptr; // Pointer to the flag in the last tab.
    for ( auto& page : m_pages )
    {
        // When using wxAUI_NB_{CLOSE,PIN}_ON_ACTIVE_TAB, we need to reserve
        // enough space in each row to show the corresponding button on the
        // active tab if it's in this row to avoid redoing the layout when the
        // selection changes (as this would be very confusing). So we
        // always measure the first tab in the row as if it had the button(s).
        // This relies on the extra increment in the width being constant for
        // all tabs, but this seems to be a safe enough assumption.
        //
        // Note that for non-wxAUI_NB_CLOSE_ON_ACTIVE_TAB cases, it doesn't
        // matter, as "Close" is either never shown at all or always shown
        // regardless of the page active status.
        //
        // Also note that we don't need to update the buttons state for the
        // locked pages as they never change, so the layout doesn't depend on
        // whether they're active or not and if the entire row consists of only
        // locked tabs, we never have to add any extra space.
        if ( page.kind != wxAuiTabKind::Locked )
        {
            UpdateButtonsState(page, firstTabInRow);
            firstTabInRow = false;
        }

        const auto size = m_art->GetPageTabSize(dc, wnd, page);

        widthRow += size.x;

        // Reset it by default, it will be set during the next loop
        // iteration or after the loop if it's the last tab in the row.
        page.rowEnd = false;

        // Start a new row if this tab doesn't fit into the current one.
        if ( widthRow > availableWidth )
        {
            widthRow = size.x;

            // We assume that all tabs have the same height: if they don't,
            // things are not going to look well no matter what.
            extraHeight += size.y;

            firstTabInRow = true;

            if ( lastRowEnd )
                *lastRowEnd = true;
        }

        lastRowEnd = &page.rowEnd;
    }

    if ( lastRowEnd )
        *lastRowEnd = true;

    return extraHeight;
}

void wxAuiTabContainer::RenderButtons(wxDC& dc, wxWindow* wnd,
                                      int& left_buttons_width,
                                      int& right_buttons_width)
{
    size_t i;
    const size_t page_count = m_pages.GetCount();
    const size_t button_count = m_buttons.size();

    // ensure we show as many tabs as possible
    while (m_tabOffset > 0 && IsTabVisible(page_count-1, m_tabOffset-1, &dc, wnd))
        --m_tabOffset;

    // find out if size of tabs is larger than can be
    // afforded on screen
    int total_width = 0;
    int visible_width = 0;
    for (i = 0; i < page_count; ++i)
    {
        wxAuiNotebookPage& page = m_pages.Item(i);

        UpdateButtonsState(page);

        int x_extent = 0;
        const wxSize size = m_art->GetPageTabSize(dc, wnd, page, &x_extent);

        if (i+1 < page_count)
            total_width += x_extent;
        else
            total_width += size.x;

        if (i >= m_tabOffset)
        {
            if (i+1 < page_count)
                visible_width += x_extent;
            else
                visible_width += size.x;
        }
    }

    if (total_width > m_rect.GetWidth() || m_tabOffset != 0)
    {
        // show left/right buttons
        for (i = 0; i < button_count; ++i)
        {
            wxAuiTabContainerButton& button = m_buttons.at(i);
            if (button.id == wxAUI_BUTTON_LEFT ||
                button.id == wxAUI_BUTTON_RIGHT)
            {
                button.curState &= ~wxAUI_BUTTON_STATE_HIDDEN;
            }
        }
    }
    else
    {
        // hide left/right buttons
        for (i = 0; i < button_count; ++i)
        {
            wxAuiTabContainerButton& button = m_buttons.at(i);
            if (button.id == wxAUI_BUTTON_LEFT ||
                button.id == wxAUI_BUTTON_RIGHT)
            {
                button.curState |= wxAUI_BUTTON_STATE_HIDDEN;
            }
        }
    }

    // determine whether various buttons should be enabled
    for (i = 0; i < button_count; ++i)
    {
        wxAuiTabContainerButton& button = m_buttons.at(i);
        if (button.id == wxAUI_BUTTON_LEFT)
        {
            if (m_tabOffset == 0)
                button.curState |= wxAUI_BUTTON_STATE_DISABLED;
            else
                button.curState &= ~wxAUI_BUTTON_STATE_DISABLED;
        }
        else if (button.id == wxAUI_BUTTON_RIGHT)
        {
            int button_width = 0;
            for ( const auto& b : m_buttons )
                button_width += b.rect.GetWidth();

            if (visible_width < m_rect.GetWidth() - button_width)
                button.curState |= wxAUI_BUTTON_STATE_DISABLED;
            else
                button.curState &= ~wxAUI_BUTTON_STATE_DISABLED;
        }
        else if (button.id == wxAUI_BUTTON_CLOSE)
        {
            button.curState &= ~wxAUI_BUTTON_STATE_DISABLED;

            // Disable "Close" button if the current page is locked, as such
            // pages can't be closed.
            for (const auto& page : m_pages)
            {
                if (page.active)
                {
                    switch ( page.kind )
                    {
                        case wxAuiTabKind::Normal:
                        case wxAuiTabKind::Pinned:
                            break;

                        case wxAuiTabKind::Locked:
                            button.curState |= wxAUI_BUTTON_STATE_DISABLED;
                            break;
                    }
                    break;
                }
            }
        }
    }

    right_buttons_width = 0;

    // draw the buttons on the right side
    for (i = 0; i < button_count; ++i)
    {
        wxAuiTabContainerButton& button = m_buttons.at(button_count - i - 1);

        if (button.location != wxRIGHT)
            continue;
        if (button.curState & wxAUI_BUTTON_STATE_HIDDEN)
            continue;

        wxRect button_rect = m_rect;
        button_rect.SetY(1);
        button_rect.SetWidth(m_rect.width - right_buttons_width);

        m_art->DrawButton(dc,
                          wnd,
                          button_rect,
                          button.id,
                          button.curState,
                          wxRIGHT,
                          &button.rect);

        right_buttons_width += button.rect.GetWidth();
    }



    left_buttons_width = 0;

    // draw the buttons on the left side

    for (i = 0; i < button_count; ++i)
    {
        wxAuiTabContainerButton& button = m_buttons.at(button_count - i - 1);

        if (button.location != wxLEFT)
            continue;
        if (button.curState & wxAUI_BUTTON_STATE_HIDDEN)
            continue;

        wxRect button_rect(left_buttons_width, 1, 1000, m_rect.height);

        m_art->DrawButton(dc,
                          wnd,
                          button_rect,
                          button.id,
                          button.curState,
                          wxLEFT,
                          &button.rect);

        left_buttons_width += button.rect.GetWidth();
    }

    if (left_buttons_width == 0)
        left_buttons_width = m_art->GetIndentSize();


    // update the tab buttons visibility
    // make sure tab button entries which aren't used are marked as hidden
    for (i = page_count; i < m_pages.GetCount(); ++i)
    {
        for (auto& button : m_pages[i].buttons)
            button.curState |= wxAUI_BUTTON_STATE_HIDDEN;
    }

    // buttons before the tab offset must be set to hidden
    for (i = 0; i < m_tabOffset; ++i)
    {
        for (auto& button : m_pages[i].buttons)
            button.curState |= wxAUI_BUTTON_STATE_HIDDEN;
    }
}


// Render() renders the tab catalog to the specified DC
// It is a virtual function and can be overridden to
// provide custom drawing capabilities
void wxAuiTabContainer::Render(wxDC* raw_dc, wxWindow* wnd)
{
    if (!raw_dc || !raw_dc->IsOk())
        return;

    if (m_rect.IsEmpty())
        return;

    size_t i;
    size_t page_count = m_pages.GetCount();

#if wxALWAYS_NATIVE_DOUBLE_BUFFER
    wxDC& dc = *raw_dc;
#else
    wxMemoryDC dc;

    // use the same layout direction as the window DC uses to ensure that the
    // text is rendered correctly
    dc.SetLayoutDirection(raw_dc->GetLayoutDirection());

    wxBitmap bmp;
    // create off-screen bitmap
    bmp.Create(m_rect.GetWidth(), m_rect.GetHeight(),*raw_dc);
    dc.SelectObject(bmp);

    if (!dc.IsOk())
        return;
#endif

    // draw background
    m_art->DrawBackground(dc, wnd, m_rect);

    // draw buttons
    int left_buttons_width = 0;
    int right_buttons_width = 0;
    RenderButtons(dc, wnd, left_buttons_width, right_buttons_width);

    int offset = left_buttons_width;

    // draw the tabs

    size_t active = (size_t)-1;
    wxRect active_rect;

    wxRect rect = m_rect;
    rect.y = 0;

    if (IsFlagSet(wxAUI_NB_MULTILINE) && page_count)
    {
        // We assume vertical size of all tabs is the same, so it doesn't
        // matter which one we use for measuring.
        const wxAuiNotebookPage& page = m_pages.Item(0);

        const auto size = m_art->GetPageTabSize(dc, wnd, page);

        rect.height = size.y;
    }
    else
    {
        rect.height = m_tabRowHeight;
    }

    // Note that this must be consistent with GetAvailableForTabs().
    const int rightBorder = m_rect.width - right_buttons_width - wnd->FromDIP(wxAUI_BUTTONS_BORDER);

    for (i = m_tabOffset; i < page_count; ++i)
    {
        wxAuiNotebookPage& page = m_pages.Item(i);

        UpdateButtonsState(page);

        // Check if this tab is at least partially visible when using a single
        // row (otherwise all rows are visible).
        if (!IsFlagSet(wxAUI_NB_MULTILINE) && offset >= rightBorder)
        {
            // This (and, hence, all the subsequent) tab(s) would be completely
            // hidden, stop drawing.
            break;
        }

        rect.x = offset;
        rect.width = rightBorder - offset;

        offset += m_art->DrawPageTab(dc, wnd, page, rect);

        if (page.active)
        {
            active = i;
            active_rect = rect;
        }

        // Start a new row if necessary when using multiple rows.
        if (IsFlagSet(wxAUI_NB_MULTILINE) && page.rowEnd)
        {
            offset = left_buttons_width;

            rect.y += rect.height;
        }
    }


    // make sure to deactivate buttons which are off the screen to the right
    for (++i; i < m_pages.GetCount(); ++i)
    {
        for (auto& button : m_pages[i].buttons)
            button.curState |= wxAUI_BUTTON_STATE_HIDDEN;
    }


    // draw the active tab again so it stands in the foreground
    if (active >= m_tabOffset && active < m_pages.GetCount())
    {
        m_art->DrawPageTab(dc, wnd, m_pages.Item(active), active_rect);
    }


#if !wxALWAYS_NATIVE_DOUBLE_BUFFER
    raw_dc->Blit(m_rect.x, m_rect.y,
                 m_rect.GetWidth(), m_rect.GetHeight(),
                 &dc, 0, 0);
#endif
}

// Is the tab visible?
bool wxAuiTabContainer::IsTabVisible(int tabPage, int tabOffset, wxReadOnlyDC* dc, wxWindow* wnd)
{
    if (!dc || !dc->IsOk())
        return false;

    // All tabs are always visible when multiple lines are used.
    if (IsFlagSet(wxAUI_NB_MULTILINE))
        return true;

    size_t i;
    size_t page_count = m_pages.GetCount();
    size_t button_count = m_buttons.size();

    // First check if both buttons are disabled - if so, there's no need to
    // check further for visibility.
    int arrowButtonVisibleCount = 0;
    for (i = 0; i < button_count; ++i)
    {
        wxAuiTabContainerButton& button = m_buttons.at(i);
        if (button.id == wxAUI_BUTTON_LEFT ||
            button.id == wxAUI_BUTTON_RIGHT)
        {
            if ((button.curState & wxAUI_BUTTON_STATE_HIDDEN) == 0)
                arrowButtonVisibleCount ++;
        }
    }

    // Tab must be visible
    if (arrowButtonVisibleCount == 0)
        return true;

    // If tab is less than the given offset, it must be invisible by definition
    if (tabPage < tabOffset)
        return false;

    // draw buttons
    int left_buttons_width = 0;
    int right_buttons_width = 0;

    // calculate size of the buttons on the right side
    int offset = m_rect.x + m_rect.width;
    for (i = 0; i < button_count; ++i)
    {
        wxAuiTabContainerButton& button = m_buttons.at(button_count - i - 1);

        if (button.location != wxRIGHT)
            continue;
        if (button.curState & wxAUI_BUTTON_STATE_HIDDEN)
            continue;

        offset -= button.rect.GetWidth();
        right_buttons_width += button.rect.GetWidth();
    }

    offset = 0;

    // calculate size of the buttons on the left side
    for (i = 0; i < button_count; ++i)
    {
        wxAuiTabContainerButton& button = m_buttons.at(button_count - i - 1);

        if (button.location != wxLEFT)
            continue;
        if (button.curState & wxAUI_BUTTON_STATE_HIDDEN)
            continue;

        offset += button.rect.GetWidth();
        left_buttons_width += button.rect.GetWidth();
    }

    offset = left_buttons_width;

    if (offset == 0)
        offset += m_art->GetIndentSize();

    wxRect rect = m_rect;

    // See if the given page is visible at the given tab offset (effectively scroll position)
    for (i = tabOffset; i < page_count; ++i)
    {
        wxAuiNotebookPage& page = m_pages.Item(i);

        rect.width = m_rect.width - right_buttons_width - offset - wnd->FromDIP(wxAUI_BUTTONS_BORDER);

        if (rect.width <= 0)
            return false; // haven't found the tab, and we've run out of space, so return false

        UpdateButtonsState(page);

        int x_extent = 0;
        m_art->GetPageTabSize(*dc, wnd, page, &x_extent);

        offset += x_extent;

        if (i == (size_t) tabPage)
        {
            // If not all of the tab is visible, and supposing there's space to display it all,
            // we could do better so we return false.
            if (((m_rect.width - right_buttons_width - offset - wnd->FromDIP(wxAUI_BUTTONS_BORDER)) <= 0) && ((m_rect.width - right_buttons_width - left_buttons_width) > x_extent))
                return false;
            else
                return true;
        }
    }

    // Shouldn't really get here, but if it does, assume the tab is visible to prevent
    // further looping in calling code.
    return true;
}

// Make the tab visible if it wasn't already
void wxAuiTabContainer::MakeTabVisible(int tabPage, wxWindow* win)
{
    wxInfoDC dc(win);
    if (!IsTabVisible(tabPage, GetTabOffset(), & dc, win))
    {
        int i;
        for (i = 0; i < (int) m_pages.GetCount(); i++)
        {
            if (IsTabVisible(tabPage, i, & dc, win))
            {
                SetTabOffset(i);
                win->Refresh();
                return;
            }
        }
    }
}

// TabHitTest() tests if a tab was hit, returning the struct containing the
// window that was hit together with its position or null pointer otherwise.
wxAuiTabContainer::HitTestResult
wxAuiTabContainer::TabHitTest(const wxPoint& pt, int flags) const
{
    if (!m_rect.Contains(pt))
        return {};

    const wxAuiTabContainerButton* const btn = ButtonHitTest(pt);
    if (btn && !(btn->curState & wxAUI_BUTTON_STATE_DISABLED))
    {
        for ( const auto& button : m_buttons )
        {
            if ( btn == &button )
                return {};
        }
    }

    const size_t page_count = m_pages.GetCount();
    for (size_t i = m_tabOffset; i < page_count; ++i)
    {
        wxAuiNotebookPage& page = m_pages.Item(i);
        if (page.rect.Contains(pt))
        {
            return { page.window, static_cast<int>(i) };
        }

        // Also optionally check if the point lies over the blank space after
        // the last tab in the row if this should be allowed.
        if ((flags & HitTest_AllowAfterTab) && page.rowEnd)
        {
            if (pt.x >= page.rect.x &&
                    pt.y >= page.rect.y &&
                        pt.y < page.rect.y + page.rect.height)
            {
                return { page.window, static_cast<int>(i) };
            }
        }
    }

    return {};
}

// ButtonHitTest() tests if a button was hit. The function returns
// the button if one was hit and null pointer otherwise
wxAuiTabContainerButton*
wxAuiTabContainer::ButtonHitTest(const wxPoint& pt) const
{
    if (!m_rect.Contains(pt))
        return nullptr;

    for ( auto& button : m_buttons )
    {
        if (button.rect.Contains(pt) &&
            !(button.curState & wxAUI_BUTTON_STATE_HIDDEN ))
        {
            return const_cast<wxAuiTabContainerButton*>(&button);
        }
    }

    for ( const auto& page : m_pages )
    {
        for ( const auto& button : page.buttons )
        {
            if (button.rect.Contains(pt) &&
                !(button.curState & (wxAUI_BUTTON_STATE_HIDDEN |
                                       wxAUI_BUTTON_STATE_DISABLED)))
            {
                return const_cast<wxAuiTabContainerButton*>(&button);
            }
        }
    }

    return nullptr;
}



// the utility function ShowWnd() is the same as show,
// except it handles wxAuiMDIChildFrame windows as well,
// as the Show() method on this class is "unplugged"
static void ShowWnd(wxWindow* wnd, bool show)
{
#if wxUSE_MDI
    if (wxDynamicCast(wnd, wxAuiMDIChildFrame))
    {
        wxAuiMDIChildFrame* cf = (wxAuiMDIChildFrame*)wnd;
        cf->wxWindow::Show(show);
    }
    else
#endif
    {
        wnd->Show(show);
    }
}


// DoShowHide() this function shows the active window, then
// hides all of the other windows (in that order)
void wxAuiTabContainer::DoShowHide()
{
    const wxAuiNotebookPageArray& pages = GetPages();

    // show new active page first
    for ( const auto& page : pages )
    {
        if (page.active)
        {
            ShowWnd(page.window, true);
            break;
        }
    }

    // hide all other pages
    for ( const auto& page : pages )
    {
        if (!page.active)
            ShowWnd(page.window, false);
    }
}


int wxAuiTabContainer::GetFirstTabOfKind(wxAuiTabKind kind) const
{
    int pos = 0;
    for ( const auto& page : m_pages )
    {
        if ( page.kind == kind )
            break;

        pos++;
    }

    return pos;
}

int wxAuiTabContainer::GetFirstTabNotOfKind(wxAuiTabKind kind) const
{
    int pos = 0;
    for ( const auto& page : m_pages )
    {
        if ( page.kind != kind )
            break;

        pos++;
    }

    return pos;
}


// -- wxAuiTabCtrl class implementation --



wxBEGIN_EVENT_TABLE(wxAuiTabCtrl, wxControl)
    EVT_PAINT(wxAuiTabCtrl::OnPaint)
    EVT_ERASE_BACKGROUND(wxAuiTabCtrl::OnEraseBackground)
    EVT_SIZE(wxAuiTabCtrl::OnSize)
    EVT_LEFT_DOWN(wxAuiTabCtrl::OnLeftDown)
    EVT_LEFT_DCLICK(wxAuiTabCtrl::OnLeftDClick)
    EVT_LEFT_UP(wxAuiTabCtrl::OnLeftUp)
    EVT_MIDDLE_DOWN(wxAuiTabCtrl::OnMiddleDown)
    EVT_MIDDLE_UP(wxAuiTabCtrl::OnMiddleUp)
    EVT_RIGHT_DOWN(wxAuiTabCtrl::OnRightDown)
    EVT_RIGHT_UP(wxAuiTabCtrl::OnRightUp)
    EVT_MOTION(wxAuiTabCtrl::OnMotion)
    EVT_LEAVE_WINDOW(wxAuiTabCtrl::OnLeaveWindow)
    EVT_AUINOTEBOOK_BUTTON(wxID_ANY, wxAuiTabCtrl::OnButton)
    EVT_SET_FOCUS(wxAuiTabCtrl::OnSetFocus)
    EVT_KILL_FOCUS(wxAuiTabCtrl::OnKillFocus)
    EVT_CHAR(wxAuiTabCtrl::OnChar)
    EVT_MOUSE_CAPTURE_LOST(wxAuiTabCtrl::OnCaptureLost)
    EVT_SYS_COLOUR_CHANGED(wxAuiTabCtrl::OnSysColourChanged)
    EVT_DPI_CHANGED(wxAuiTabCtrl::OnDpiChanged)
wxEND_EVENT_TABLE()


wxAuiTabCtrl::wxAuiTabCtrl(wxWindow* parent,
                           wxWindowID id,
                           const wxPoint& pos,
                           const wxSize& size,
                           long style) : wxControl(parent, id, pos, size, style)
{
    SetName(wxT("wxAuiTabCtrl"));
}

wxAuiTabCtrl::~wxAuiTabCtrl()
{
}

void wxAuiTabCtrl::DoShowTab(int idx)
{
    DoShowHide();
    MakeTabVisible(idx, this);
}

void wxAuiTabCtrl::DoUpdateActive()
{
    DoShowHide();
    Refresh();
}

void wxAuiTabCtrl::DoEndDragging()
{
    m_clickPt = wxDefaultPosition;
    m_isDragging = false;
    m_clickTab = nullptr;
}

void wxAuiTabCtrl::DoApplyRect(const wxRect& rect, int tabCtrlHeight)
{
    // Save the full rectangle for GetHintScreenRect().
    m_fullRect = rect;

    // Save the height of a single tab row before possibly changing it below in
    // multi-line case.
    SetRowHeight(tabCtrlHeight);

    if (IsFlagSet(wxAUI_NB_MULTILINE))
    {
        tabCtrlHeight += LayoutMultiLineTabs(rect, this);
    }

    if (IsFlagSet(wxAUI_NB_BOTTOM))
    {
        SetSize(rect.x, rect.y + rect.height - tabCtrlHeight,
                rect.width, tabCtrlHeight);
        SetRect(wxRect(0, 0, rect.width, tabCtrlHeight));
    }
    else //TODO: if (IsFlagSet(wxAUI_NB_TOP))
    {
        SetSize(rect.x, rect.y, rect.width, tabCtrlHeight);
        SetRect(wxRect(0, 0, rect.width, tabCtrlHeight));
    }
    // TODO: else if (IsFlagSet(wxAUI_NB_LEFT)){}
    // TODO: else if (IsFlagSet(wxAUI_NB_RIGHT)){}

    Refresh();
    Update();
}

wxRect wxAuiTabCtrl::GetHintScreenRect() const
{
    wxRect rect = m_fullRect;
    GetParent()->ClientToScreen(&rect.x, &rect.y);
    return rect;
}

void wxAuiTabCtrl::OnPaint(wxPaintEvent&)
{
    wxPaintDC dc(this);

    if (GetPageCount() > 0)
        Render(&dc, this);
}

void wxAuiTabCtrl::OnSysColourChanged(wxSysColourChangedEvent &event)
{
    event.Skip();

    if (m_art)
    {
        m_art->UpdateColoursFromSystem();
    }
}

void wxAuiTabCtrl::OnEraseBackground(wxEraseEvent& WXUNUSED(evt))
{
}

void wxAuiTabCtrl::OnSize(wxSizeEvent& evt)
{
    wxSize s = evt.GetSize();
    wxRect r(0, 0, s.GetWidth(), s.GetHeight());
    SetRect(r);
}

void wxAuiTabCtrl::OnLeftDown(wxMouseEvent& evt)
{
    CaptureMouse();

    // Reset any previous values first.
    DoEndDragging();
    m_pressedButton = nullptr;


    if ( auto const tabInfo = TabHitTest(evt.GetPosition()) )
    {
        int new_selection = tabInfo.pos;

        // wxAuiNotebooks always want to receive this event
        // even if the tab is already active, because they may
        // have multiple tab controls
        if ((new_selection != GetActivePage() ||
            wxDynamicCast(GetParent(), wxAuiNotebook)) && !m_hoverButton)
        {
            wxAuiNotebookEvent e(wxEVT_AUINOTEBOOK_PAGE_CHANGING, m_windowId);
            e.SetSelection(new_selection);
            e.SetOldSelection(GetActivePage());
            e.SetEventObject(this);
            ProcessWindowEvent(e);
        }

        m_clickPt.x = evt.m_x;
        m_clickPt.y = evt.m_y;
        m_clickTab = tabInfo.window;
    }

    if (m_hoverButton)
    {
        m_pressedButton = m_hoverButton;
        m_pressedButton->curState |= wxAUI_BUTTON_STATE_PRESSED;
        Refresh();
        Update();
    }
}

void wxAuiTabCtrl::OnCaptureLost(wxMouseCaptureLostEvent& WXUNUSED(event))
{
    if (m_isDragging)
    {
        const auto clickTab = m_clickTab;

        DoEndDragging();

        wxAuiNotebookEvent evt(wxEVT_AUINOTEBOOK_CANCEL_DRAG, m_windowId);
        evt.SetSelection(GetIdxFromWindow(clickTab));
        evt.SetOldSelection(evt.GetSelection());
        evt.SetEventObject(this);
        ProcessWindowEvent(evt);
    }
}

void wxAuiTabCtrl::OnLeftUp(wxMouseEvent& evt)
{
    if (GetCapture() == this)
        ReleaseMouse();

    if (m_isDragging)
    {
        const auto clickTab = m_clickTab;

        DoEndDragging();

        wxAuiNotebookEvent e(wxEVT_AUINOTEBOOK_END_DRAG, m_windowId);
        e.SetSelection(GetIdxFromWindow(clickTab));
        e.SetOldSelection(e.GetSelection());
        e.SetEventObject(this);
        ProcessWindowEvent(e);

        return;
    }

    if (m_pressedButton)
    {
        m_pressedButton->curState &= ~wxAUI_BUTTON_STATE_PRESSED;

        // make sure we're still clicking the button
        const wxAuiTabContainerButton* const
            button = ButtonHitTest(evt.GetPosition());
        if (!button || button->curState & wxAUI_BUTTON_STATE_DISABLED)
            return;

        if (button != m_pressedButton)
        {
            m_pressedButton = nullptr;
            return;
        }

        Refresh();
        Update();

        if (!(m_pressedButton->curState & wxAUI_BUTTON_STATE_DISABLED))
        {
            wxAuiNotebookEvent e(wxEVT_AUINOTEBOOK_BUTTON, m_windowId);
            e.SetSelection(GetIdxFromWindow(m_clickTab));
            e.SetInt(m_pressedButton->id);
            e.SetEventObject(this);
            ProcessWindowEvent(e);
        }

        m_pressedButton = nullptr;
    }

    DoEndDragging();
}

void wxAuiTabCtrl::OnMiddleUp(wxMouseEvent& evt)
{
    auto const tabInfo = TabHitTest(evt.GetPosition());
    if (!tabInfo)
        return;

    wxAuiNotebookEvent e(wxEVT_AUINOTEBOOK_TAB_MIDDLE_UP, m_windowId);
    e.SetEventObject(this);
    e.SetSelection(tabInfo.pos);
    ProcessWindowEvent(e);
}

void wxAuiTabCtrl::OnMiddleDown(wxMouseEvent& evt)
{
    auto const tabInfo = TabHitTest(evt.GetPosition());
    if (!tabInfo)
        return;

    wxAuiNotebookEvent e(wxEVT_AUINOTEBOOK_TAB_MIDDLE_DOWN, m_windowId);
    e.SetEventObject(this);
    e.SetSelection(tabInfo.pos);
    ProcessWindowEvent(e);
}

void wxAuiTabCtrl::OnRightUp(wxMouseEvent& evt)
{
    auto const tabInfo = TabHitTest(evt.GetPosition());
    if (!tabInfo)
        return;

    wxAuiNotebookEvent e(wxEVT_AUINOTEBOOK_TAB_RIGHT_UP, m_windowId);
    e.SetEventObject(this);
    e.SetSelection(tabInfo.pos);
    ProcessWindowEvent(e);
}

void wxAuiTabCtrl::OnRightDown(wxMouseEvent& evt)
{
    auto const tabInfo = TabHitTest(evt.GetPosition());
    if (!tabInfo)
        return;

    wxAuiNotebookEvent e(wxEVT_AUINOTEBOOK_TAB_RIGHT_DOWN, m_windowId);
    e.SetEventObject(this);
    e.SetSelection(tabInfo.pos);
    ProcessWindowEvent(e);
}

void wxAuiTabCtrl::OnLeftDClick(wxMouseEvent& evt)
{
    wxPoint pos = evt.GetPosition();

    if (!TabHitTest(pos) && !ButtonHitTest(pos))
    {
        wxAuiNotebookEvent e(wxEVT_AUINOTEBOOK_BG_DCLICK, m_windowId);
        e.SetEventObject(this);
        ProcessWindowEvent(e);
    }
}

void wxAuiTabCtrl::OnMotion(wxMouseEvent& evt)
{
    wxPoint pos = evt.GetPosition();

    // check if the mouse is hovering above a button
    wxAuiTabContainerButton* const button = ButtonHitTest(pos);
    if (button && !(button->curState & wxAUI_BUTTON_STATE_DISABLED))
    {
        if (m_hoverButton && button != m_hoverButton)
        {
            m_hoverButton->curState &= ~wxAUI_BUTTON_STATE_HOVER;
            m_hoverButton = nullptr;
            Refresh();
            Update();
        }

        if (!(button->curState & wxAUI_BUTTON_STATE_HOVER))
        {
            button->curState |= wxAUI_BUTTON_STATE_HOVER;
            Refresh();
            Update();

            m_hoverButton = button;
            return;
        }
    }
    else
    {
        if (m_hoverButton)
        {
            m_hoverButton->curState &= ~wxAUI_BUTTON_STATE_HOVER;
            m_hoverButton = nullptr;
            Refresh();
            Update();
        }
    }

    bool hovering = false;
    if (evt.Moving())
    {
        if ( auto const tabInfo = TabHitTest(evt.GetPosition()) )
        {
            hovering = true;

            SetHoverTab(tabInfo.window);

#if wxUSE_TOOLTIPS
            wxString tooltip(m_pages[tabInfo.pos].tooltip);

            // If the text changes, set it else, keep old, to avoid
            // 'moving tooltip' effect
            if (GetToolTipText() != tooltip)
                SetToolTip(tooltip);
#endif // wxUSE_TOOLTIPS
        }
    }

    if (!hovering)
    {
        SetHoverTab(nullptr);

#if wxUSE_TOOLTIPS
        UnsetToolTip();
#endif // wxUSE_TOOLTIPS
    }

    if (!evt.LeftIsDown() || m_clickPt == wxDefaultPosition)
        return;

    if (m_isDragging)
    {
        wxAuiNotebookEvent e(wxEVT_AUINOTEBOOK_DRAG_MOTION, m_windowId);
        e.SetSelection(GetIdxFromWindow(m_clickTab));
        e.SetOldSelection(e.GetSelection());
        e.SetEventObject(this);
        ProcessWindowEvent(e);
        return;
    }


    int drag_x_threshold = wxSystemSettings::GetMetric(wxSYS_DRAG_X, this);
    int drag_y_threshold = wxSystemSettings::GetMetric(wxSYS_DRAG_Y, this);

    if (abs(pos.x - m_clickPt.x) > drag_x_threshold ||
        abs(pos.y - m_clickPt.y) > drag_y_threshold)
    {
        const int idx = GetIdxFromWindow(m_clickTab);
        if ( idx != wxNOT_FOUND )
        {
            switch ( GetPage(idx).kind )
            {
                case wxAuiTabKind::Normal:
                case wxAuiTabKind::Pinned:
                    break;

                case wxAuiTabKind::Locked:
                    // Don't allow dragging locked tabs.
                    return;
            }
        }

        wxAuiNotebookEvent e(wxEVT_AUINOTEBOOK_BEGIN_DRAG, m_windowId);
        e.SetSelection(idx);
        e.SetOldSelection(e.GetSelection());
        e.SetEventObject(this);
        ProcessWindowEvent(e);

        m_isDragging = true;
    }
}

void wxAuiTabCtrl::OnLeaveWindow(wxMouseEvent& WXUNUSED(event))
{
    if (m_hoverButton)
    {
        m_hoverButton->curState &= ~wxAUI_BUTTON_STATE_HOVER;
        m_hoverButton = nullptr;
        Refresh();
        Update();
    }

    SetHoverTab(nullptr);
}

void wxAuiTabCtrl::OnButton(wxAuiNotebookEvent& event)
{
    int button = event.GetInt();

    if (button == wxAUI_BUTTON_LEFT || button == wxAUI_BUTTON_RIGHT)
    {
        if (button == wxAUI_BUTTON_LEFT)
        {
            if (GetTabOffset() > 0)
            {
                SetTabOffset(GetTabOffset()-1);
                Refresh();
                Update();
            }
        }
        else
        {
            SetTabOffset(GetTabOffset()+1);
            Refresh();
            Update();
        }
    }
    else if (button == wxAUI_BUTTON_WINDOWLIST)
    {
        int idx = GetArtProvider()->ShowDropDown(this, m_pages, GetActivePage());

        if (idx != -1)
        {
            wxAuiNotebookEvent e(wxEVT_AUINOTEBOOK_PAGE_CHANGING, m_windowId);
            e.SetSelection(idx);
            e.SetOldSelection(GetActivePage());
            e.SetEventObject(this);
            ProcessWindowEvent(e);
        }
    }
    else
    {
        event.Skip();
    }
}

void wxAuiTabCtrl::OnSetFocus(wxFocusEvent& WXUNUSED(event))
{
    Refresh();
}

void wxAuiTabCtrl::OnKillFocus(wxFocusEvent& WXUNUSED(event))
{
    Refresh();
}

void wxAuiTabCtrl::OnChar(wxKeyEvent& event)
{
    if (GetActivePage() == -1)
    {
        event.Skip();
        return;
    }

    // We can't leave tab processing to the system; on Windows, tabs and keys
    // get eaten by the system and not processed properly if we specify both
    // wxTAB_TRAVERSAL and wxWANTS_CHARS. And if we specify just wxTAB_TRAVERSAL,
    // we don't key arrow key events.

    int key = event.GetKeyCode();

    if (key == WXK_NUMPAD_PAGEUP)
        key = WXK_PAGEUP;
    if (key == WXK_NUMPAD_PAGEDOWN)
        key = WXK_PAGEDOWN;
    if (key == WXK_NUMPAD_HOME)
        key = WXK_HOME;
    if (key == WXK_NUMPAD_END)
        key = WXK_END;
    if (key == WXK_NUMPAD_LEFT)
        key = WXK_LEFT;
    if (key == WXK_NUMPAD_RIGHT)
        key = WXK_RIGHT;

    if (key == WXK_TAB || key == WXK_PAGEUP || key == WXK_PAGEDOWN)
    {
        bool bCtrlDown = event.ControlDown();
        bool bShiftDown = event.ShiftDown();

        bool bForward = (key == WXK_TAB && !bShiftDown) || (key == WXK_PAGEDOWN);
        bool bWindowChange = (key == WXK_PAGEUP) || (key == WXK_PAGEDOWN) || bCtrlDown;
        bool bFromTab = (key == WXK_TAB);

        if (bFromTab && !bWindowChange)
        {
            // Handle ordinary tabs via Navigate. This is needed at least for wxGTK to tab properly.
            Navigate(bForward ? wxNavigationKeyEvent::IsForward : wxNavigationKeyEvent::IsBackward);
            return;
        }

        wxAuiNotebook* nb = wxDynamicCast(GetParent(), wxAuiNotebook);
        if (!nb)
        {
            event.Skip();
            return;
        }

        wxNavigationKeyEvent keyEvent;
        keyEvent.SetDirection(bForward);
        keyEvent.SetWindowChange(bWindowChange);
        keyEvent.SetFromTab(bFromTab);
        keyEvent.SetEventObject(nb);

        if (!nb->ProcessWindowEvent(keyEvent))
        {
            // Not processed? Do an explicit tab into the page.
            wxWindow* win = GetWindowFromIdx(GetActivePage());
            if (win)
                win->SetFocus();
        }
        return;
    }

    if (m_pages.GetCount() < 2)
    {
        event.Skip();
        return;
    }

    int newPage = -1;

    int forwardKey, backwardKey;
    if (GetLayoutDirection() == wxLayout_RightToLeft)
    {
        forwardKey = WXK_LEFT;
        backwardKey = WXK_RIGHT;
    }
    else
     {
        forwardKey = WXK_RIGHT;
        backwardKey = WXK_LEFT;
    }

    if (key == forwardKey)
    {
        if (m_pages.GetCount() > 1)
        {
            if (GetActivePage() == -1)
                newPage = 0;
            else if (GetActivePage() < (int) (m_pages.GetCount() - 1))
                newPage = GetActivePage() + 1;
        }
    }
    else if (key == backwardKey)
    {
        if (m_pages.GetCount() > 1)
        {
            if (GetActivePage() == -1)
                newPage = (int) (m_pages.GetCount() - 1);
            else if (GetActivePage() > 0)
                newPage = GetActivePage() - 1;
        }
    }
    else if (key == WXK_HOME)
    {
        newPage = 0;
    }
    else if (key == WXK_END)
    {
        newPage = (int) (m_pages.GetCount() - 1);
    }
    else
        event.Skip();

    if (newPage != -1)
    {
        wxAuiNotebookEvent e(wxEVT_AUINOTEBOOK_PAGE_CHANGING, m_windowId);
        e.SetSelection(newPage);
        e.SetOldSelection(newPage);
        e.SetEventObject(this);
        this->ProcessWindowEvent(e);
    }
    else
        event.Skip();
}

void wxAuiTabCtrl::OnDpiChanged(wxDPIChangedEvent& event)
{
    m_art->UpdateDpi();
    event.Skip();
}

// wxAuiTabFrame is an interesting case.  It's important that all child pages
// of the multi-notebook control are all actually children of that control
// (and not grandchildren).  wxAuiTabFrame facilitates this.  There is one
// instance of wxAuiTabFrame for each tab control inside the multi-notebook.
// It's important to know that wxAuiTabFrame is not a real window, but it merely
// used to capture the dimensions/positioning of the internal tab control and
// it's managed page windows

class wxAuiTabFrame : public wxWindow
{
public:

    wxAuiTabFrame(wxAuiTabCtrl* tabs, const wxSize& size, int tabCtrlHeight) :
        m_rect(size),
        m_tabs(tabs),
        m_tabCtrlHeight(tabCtrlHeight)
    {
    }

    ~wxAuiTabFrame()
    {
        // use pending delete because sometimes during
        // window closing, refreshs are pending
        if (!wxPendingDelete.Member(m_tabs))
            wxPendingDelete.Append(m_tabs);
    }

    void SetTabCtrlHeight(int h)
    {
        m_tabCtrlHeight = h;
    }

    // As we don't have a valid HWND, the base class version doesn't work for
    // this window, so override it to return the appropriate DPI.
    wxSize GetDPI() const override
    {
        return m_tabs->GetDPI();
    }

protected:
    void DoSetSize(int x, int y,
                   int width, int height,
                   int WXUNUSED(sizeFlags = wxSIZE_AUTO)) override
    {
        m_rect = wxRect(x, y, width, height);
        DoSizing();
    }

    void DoGetClientSize(int* x, int* y) const override
    {
        *x = m_rect.width;
        *y = m_rect.height;
    }

public:
    bool Show( bool WXUNUSED(show = true) ) override { return false; }

    void DoSizing()
    {
        if (m_tabs->IsFrozen() || m_tabs->GetParent()->IsFrozen())
            return;

        m_tabs->DoApplyRect(m_rect, m_tabCtrlHeight);
        m_tab_rect = m_tabs->GetRect();

        const wxAuiNotebookPageArray& pages = m_tabs->GetPages();

        for ( auto& page : pages )
        {
            int border_space = m_tabs->GetArtProvider()->GetAdditionalBorderSpace(page.window);

            int height = m_rect.height - m_tab_rect.height - border_space;
            if ( height < 0 )
            {
                // avoid passing negative height to wxWindow::SetSize(), this
                // results in assert failures/GTK+ warnings
                height = 0;
            }
            int width = m_rect.width - 2 * border_space;
            if (width < 0)
                width = 0;

            if (m_tabs->IsFlagSet(wxAUI_NB_BOTTOM))
            {
                page.window->SetSize(m_rect.x + border_space,
                                     m_rect.y + border_space,
                                     width,
                                     height);
            }
            else //TODO: if (IsFlagSet(wxAUI_NB_TOP))
            {
                page.window->SetSize(m_rect.x + border_space,
                                     m_rect.y + m_tab_rect.height,
                                     width,
                                     height);
            }
            // TODO: else if (IsFlagSet(wxAUI_NB_LEFT)){}
            // TODO: else if (IsFlagSet(wxAUI_NB_RIGHT)){}
        }
    }

protected:
    void DoGetSize(int* x, int* y) const override
    {
        if (x)
            *x = m_rect.GetWidth();
        if (y)
            *y = m_rect.GetHeight();
    }

public:
    void Update() override
    {
        // does nothing
    }

    wxRect m_rect;
    wxRect m_tab_rect;
    wxAuiTabCtrl* const m_tabs;
    int m_tabCtrlHeight = 0;
};


const int wxAuiBaseTabCtrlId = 5380;


// -- wxAuiNotebook class implementation --

// More convenient version of FindTab(): returns all the results instead of
// requiring output parameters for returning some of them.
//
// Note that TabInfo returned by FindTab() is normally always valid.
struct wxAuiNotebook::TabInfo : wxAuiNotebookPosition
{
    TabInfo() = default;

    TabInfo(wxAuiTabCtrl* tabCtrl_, int tabIdx_, wxAuiNotebookPage* info)
        : wxAuiNotebookPosition{tabCtrl_, tabIdx_}, pageInfo(info)
    {
    }

    // Information about the page or nullptr if not found.
    wxAuiNotebookPage* pageInfo = nullptr;
};

#define EVT_AUI_RANGE(id1, id2, event, func) \
    wx__DECLARE_EVT2(event, id1, id2, wxAuiNotebookEventHandler(func))

wxBEGIN_EVENT_TABLE(wxAuiNotebook, wxBookCtrlBase)
    EVT_SIZE(wxAuiNotebook::OnSize)
    EVT_CHILD_FOCUS(wxAuiNotebook::OnChildFocusNotebook)
    EVT_AUI_RANGE(wxAuiBaseTabCtrlId, wxAuiBaseTabCtrlId+500,
                      wxEVT_AUINOTEBOOK_PAGE_CHANGING,
                      wxAuiNotebook::OnTabClicked)
    EVT_AUI_RANGE(wxAuiBaseTabCtrlId, wxAuiBaseTabCtrlId+500,
                      wxEVT_AUINOTEBOOK_BEGIN_DRAG,
                      wxAuiNotebook::OnTabBeginDrag)
    EVT_AUI_RANGE(wxAuiBaseTabCtrlId, wxAuiBaseTabCtrlId+500,
                      wxEVT_AUINOTEBOOK_END_DRAG,
                      wxAuiNotebook::OnTabEndDrag)
    EVT_AUI_RANGE(wxAuiBaseTabCtrlId, wxAuiBaseTabCtrlId+500,
                      wxEVT_AUINOTEBOOK_CANCEL_DRAG,
                      wxAuiNotebook::OnTabCancelDrag)
    EVT_AUI_RANGE(wxAuiBaseTabCtrlId, wxAuiBaseTabCtrlId+500,
                      wxEVT_AUINOTEBOOK_DRAG_MOTION,
                      wxAuiNotebook::OnTabDragMotion)
    EVT_AUI_RANGE(wxAuiBaseTabCtrlId, wxAuiBaseTabCtrlId+500,
                      wxEVT_AUINOTEBOOK_BUTTON,
                      wxAuiNotebook::OnTabButton)
    EVT_AUI_RANGE(wxAuiBaseTabCtrlId, wxAuiBaseTabCtrlId+500,
                      wxEVT_AUINOTEBOOK_TAB_MIDDLE_DOWN,
                      wxAuiNotebook::OnTabMiddleDown)
    EVT_AUI_RANGE(wxAuiBaseTabCtrlId, wxAuiBaseTabCtrlId+500,
                      wxEVT_AUINOTEBOOK_TAB_MIDDLE_UP,
                      wxAuiNotebook::OnTabMiddleUp)
    EVT_AUI_RANGE(wxAuiBaseTabCtrlId, wxAuiBaseTabCtrlId+500,
                      wxEVT_AUINOTEBOOK_TAB_RIGHT_DOWN,
                      wxAuiNotebook::OnTabRightDown)
    EVT_AUI_RANGE(wxAuiBaseTabCtrlId, wxAuiBaseTabCtrlId+500,
                      wxEVT_AUINOTEBOOK_TAB_RIGHT_UP,
                      wxAuiNotebook::OnTabRightUp)
    EVT_AUI_RANGE(wxAuiBaseTabCtrlId, wxAuiBaseTabCtrlId+500,
                      wxEVT_AUINOTEBOOK_BG_DCLICK,
                      wxAuiNotebook::OnTabBgDClick)
    EVT_NAVIGATION_KEY(wxAuiNotebook::OnNavigationKeyNotebook)
    EVT_SYS_COLOUR_CHANGED(wxAuiNotebook::OnSysColourChanged)
    EVT_DPI_CHANGED(wxAuiNotebook::OnDpiChanged)
wxEND_EVENT_TABLE()

namespace
{

// wxAuiNotebook always adds a special dummy pane with this name.
constexpr const char* const DUMMY_PANE_NAME = "dummy";

// Check if this is a dummy pane.
bool IsDummyPane(const wxAuiPaneInfo& pane)
{
    return pane.name == DUMMY_PANE_NAME;
}

} // anonymous namespace

void wxAuiNotebook::OnSysColourChanged(wxSysColourChangedEvent &event)
{
    event.Skip(true);
    wxAuiTabArt* art = m_tabs.GetArtProvider();
    art->UpdateColoursFromSystem();

    for ( const auto& pane : m_mgr.GetAllPanes() )
    {
        if ( IsDummyPane(pane) )
            continue;
        wxAuiTabFrame* tab_frame = (wxAuiTabFrame*)pane.window;
        wxAuiTabCtrl* tabctrl = tab_frame->m_tabs;
        tabctrl->GetArtProvider()->UpdateColoursFromSystem();
        tabctrl->Refresh();
    }
    Refresh();
}

void wxAuiNotebook::OnDpiChanged(wxDPIChangedEvent& event)
{
    UpdateTabCtrlHeight();
    event.Skip();
}

void wxAuiNotebook::Init()
{
    m_curPage = wxNOT_FOUND;
    m_tabIdCounter = wxAuiBaseTabCtrlId;
    m_dummyWnd = nullptr;
    m_requestedBmpSize = wxDefaultSize;
    m_requestedTabCtrlHeight = -1;
}

bool wxAuiNotebook::Create(wxWindow* parent,
                           wxWindowID id,
                           const wxPoint& pos,
                           const wxSize& size,
                           long style)
{
    if (!wxControl::Create(parent, id, pos, size, style))
        return false;

    InitNotebook(style);

    return true;
}

// InitNotebook() contains common initialization
// code called by all constructors
void wxAuiNotebook::InitNotebook(long style)
{
    SetName(wxT("wxAuiNotebook"));
    m_curPage = wxNOT_FOUND;
    m_tabIdCounter = wxAuiBaseTabCtrlId;
    m_dummyWnd = nullptr;
    m_flags = (unsigned int)style;
    m_tabCtrlHeight = FromDIP(20);

    m_normalFont = *wxNORMAL_FONT;
    m_selectedFont = *wxNORMAL_FONT;
    m_selectedFont.SetWeight(wxFONTWEIGHT_BOLD);

    SetArtProvider(new wxAuiDefaultTabArt);

    m_dummyWnd = new wxWindow(this, wxID_ANY, wxPoint(0,0), wxSize(0,0));
    m_dummyWnd->SetSize(FromDIP(wxSize(200, 200)));
    m_dummyWnd->Show(false);

    m_mgr.SetManagedWindow(this);
    m_mgr.SetFlags(wxAUI_MGR_DEFAULT);
    m_mgr.SetDockSizeConstraint(1.0, 1.0); // no dock size constraint

    m_mgr.AddPane(m_dummyWnd,
              wxAuiPaneInfo().Name(DUMMY_PANE_NAME).Bottom().CaptionVisible(false).Show(false));

    m_mgr.Update();
}

wxAuiTabFrame* wxAuiNotebook::CreateTabFrame(wxSize size)
{
    auto* const tabs = new wxAuiTabCtrl(this,
                                        m_tabIdCounter++,
                                        wxDefaultPosition,
                                        wxDefaultSize,
                                        wxNO_BORDER|wxWANTS_CHARS);
    tabs->SetFlags(m_flags);
    tabs->SetArtProvider(m_tabs.GetArtProvider()->Clone());

    return new wxAuiTabFrame(tabs, size, m_tabCtrlHeight);
}

wxAuiNotebook::~wxAuiNotebook()
{
    // Indicate we're deleting pages
    SendDestroyEvent();

    while ( GetPageCount() > 0 )
        DeletePage(0);

    m_mgr.UnInit();
}

void wxAuiNotebook::SetArtProvider(wxAuiTabArt* art)
{
    m_tabs.SetArtProvider(art);

    // If the art provider implements GetXXXFont() functions, use them.
    //
    // Otherwise keep using our own fonts.
    wxFont font = art->GetNormalFont();
    if ( font.IsOk() )
        m_normalFont = font;
    font = art->GetSelectedFont();
    if ( font.IsOk() )
        m_selectedFont = font;

    // Update the height and do nothing else if it did something but otherwise
    // (i.e. if the new art provider uses the same height as the old one) we
    // need to manually set the art provider for all tabs ourselves.
    if ( !UpdateTabCtrlHeight() )
    {
        for ( const auto& pane : m_mgr.GetAllPanes() )
        {
            if ( IsDummyPane(pane) )
                continue;
            wxAuiTabFrame* tab_frame = (wxAuiTabFrame*)pane.window;
            wxAuiTabCtrl* tabctrl = tab_frame->m_tabs;
            tabctrl->SetArtProvider(art->Clone());
        }
    }
}

// SetTabCtrlHeight() is the highest-level override of the
// tab height.  A call to this function effectively enforces a
// specified tab ctrl height, overriding all other considerations,
// such as text or bitmap height.  It overrides any call to
// SetUniformBitmapSize().  Specifying a height of -1 reverts
// any previous call and returns to the default behaviour

void wxAuiNotebook::SetTabCtrlHeight(int height)
{
    m_requestedTabCtrlHeight = height;

    // if window is already initialized, recalculate the tab height
    if (m_dummyWnd)
    {
        UpdateTabCtrlHeight();
    }
}


// SetUniformBitmapSize() ensures that all tabs will have
// the same height, even if some tabs don't have bitmaps
// Passing wxDefaultSize to this function will instruct
// the control to use dynamic tab height-- so when a tab
// with a large bitmap is added, the tab ctrl's height will
// automatically increase to accommodate the bitmap

void wxAuiNotebook::SetUniformBitmapSize(const wxSize& size)
{
    m_requestedBmpSize = size;

    // if window is already initialized, recalculate the tab height
    if (m_dummyWnd)
    {
        UpdateTabCtrlHeight();
    }
}

// UpdateTabCtrlHeight() does the actual tab resizing. It's meant
// to be used internally
bool wxAuiNotebook::UpdateTabCtrlHeight()
{
    // get the tab ctrl height we will use
    int height = CalculateTabCtrlHeight();

    // if the tab control height needs to change, update
    // all of our tab controls with the new height
    if (m_tabCtrlHeight == height)
        return false;

    wxAuiTabArt* art = m_tabs.GetArtProvider();

    m_tabCtrlHeight = height;

    for ( const auto& pane : m_mgr.GetAllPanes() )
    {
        if ( IsDummyPane(pane) )
            continue;
        wxAuiTabFrame* tab_frame = (wxAuiTabFrame*)pane.window;
        wxAuiTabCtrl* tabctrl = tab_frame->m_tabs;
        tab_frame->SetTabCtrlHeight(m_tabCtrlHeight);
        tabctrl->SetArtProvider(art->Clone());
        tab_frame->DoSizing();
    }

    return true;
}

void wxAuiNotebook::UpdateHintWindowSize()
{
    wxSize size = CalculateNewSplitSize();

    // the placeholder hint window should be set to this size
    wxAuiPaneInfo& info = m_mgr.GetPane(DUMMY_PANE_NAME);
    if (info.IsOk())
    {
        info.MinSize(size);
        info.BestSize(size);
        m_dummyWnd->SetSize(size);
    }
}


// calculates the size of the new split
wxSize wxAuiNotebook::CalculateNewSplitSize()
{
    // One of the panes corresponds to the dummy window, the rest are tabs.
    const int tab_ctrl_count = m_mgr.GetAllPanes().size() - 1;

    wxSize new_split_size;

    // if there is only one tab control, the first split
    // should happen around the middle
    if (tab_ctrl_count < 2)
    {
        new_split_size = GetClientSize();
        new_split_size.x /= 2;
        new_split_size.y /= 2;
    }
    else
    {
        // this is in place of a more complicated calculation
        // that needs to be implemented
        new_split_size = FromDIP(wxSize(180,180));
    }

    return new_split_size;
}

int wxAuiNotebook::CalculateTabCtrlHeight()
{
    // if a fixed tab ctrl height is specified,
    // just return that instead of calculating a
    // tab height
    if (m_requestedTabCtrlHeight != -1)
        return m_requestedTabCtrlHeight;

    // find out new best tab height
    wxAuiTabArt* art = m_tabs.GetArtProvider();

    return art->GetBestTabCtrlSize(this,
                                   m_tabs.GetPages(),
                                   m_requestedBmpSize);
}


wxAuiTabArt* wxAuiNotebook::GetArtProvider() const
{
    return m_tabs.GetArtProvider();
}

void wxAuiNotebook::SetWindowStyleFlag(long style)
{
    wxControl::SetWindowStyleFlag(style);

    m_flags = (unsigned int)style;

    // if the control is already initialized
    if (m_mgr.GetManagedWindow() == (wxWindow*)this)
    {
        // let all of the tab children know about the new style
        for ( const auto& pane : m_mgr.GetAllPanes() )
        {
            if ( IsDummyPane(pane) )
                continue;
            wxAuiTabFrame* tabframe = (wxAuiTabFrame*)pane.window;
            wxAuiTabCtrl* tabctrl = tabframe->m_tabs;
            tabctrl->SetFlags(m_flags);
            tabframe->DoSizing();
            tabctrl->Refresh();
            tabctrl->Update();
        }
    }
}


bool wxAuiNotebook::AddPage(wxWindow* page,
                            const wxString& caption,
                            bool select,
                            const wxBitmapBundle& bitmap)
{
    return InsertPage(GetPageCount(), page, caption, select, bitmap);
}

bool wxAuiNotebook::InsertPage(size_t page_idx,
                               wxWindow* page,
                               const wxString& caption,
                               bool select,
                               const wxBitmapBundle& bitmap)
{
    // Note that index may be equal to GetPageCount() here to append the page.
    wxCHECK_MSG(page_idx <= GetPageCount(), false, wxT("invalid page index"));

    wxCHECK_MSG(page, false, wxT("page pointer must be non-null"));

    wxAuiNotebookPage info;
    info.window = page;
    info.caption = caption;
    info.bitmap = bitmap;
    info.active = false;

    wxAuiTabCtrl* active_tabctrl = GetActiveTabCtrl();

    // The usual case is appending a new page, when we can just add it page at
    // the end of the currently active tab control.
    int tab_page_idx = -1;

    // But when inserting, we may need to do it at a different place.
    if ( page_idx != m_tabs.GetPageCount() )
    {
        const auto tabInfo = FindTab(m_tabs.GetWindowFromIdx(page_idx));
        if ( tabInfo.tabCtrl == active_tabctrl )
        {
            // Use this index for insertion, as it's in the correct control.
            tab_page_idx = tabInfo.tabIdx;
        }
        //else: Do nothing, we'll append it to the active tab control.
    }

    InsertPageAt(info, page_idx, active_tabctrl, tab_page_idx, select);

    return true;
}

void wxAuiNotebook::InsertPageAt(wxAuiNotebookPage& info,
                                 size_t page_idx,
                                 wxAuiTabCtrl* tabctrl,
                                 int tab_page_idx,
                                 bool select)
{
    wxWindow* const page = info.window;

    page->Reparent(this);

    // if there are currently no tabs, the first added
    // tab must be selected, even if "select" is false
    if (m_tabs.GetPageCount() == 0)
    {
        select = true;
    }

    m_tabs.InsertPage(info, page_idx);

    if ( tab_page_idx == -1 )
        tab_page_idx = tabctrl->GetPageCount();
    tabctrl->InsertPage(info, tab_page_idx);

    // Note that we don't need to call DoSizing() if the height has changed, as
    // it's already called from UpdateTabCtrlHeight() itself in this case.
    if ( !UpdateTabCtrlHeight() )
        DoSizing();

    tabctrl->DoShowHide();

    // adjust selected index
    if(m_curPage >= (int) page_idx)
        m_curPage++;

    if (select)
    {
        SetSelectionToWindow(page);
    }
}

int wxAuiNotebook::GetNextPage(bool forward) const
{
    if ( m_curPage == wxNOT_FOUND )
        return wxNOT_FOUND;

    const auto tabInfo = FindTab(m_tabs.GetWindowFromIdx(m_curPage));
    if ( !tabInfo )
        return wxNOT_FOUND;

    // Find the next or previous position in the active tab control, with
    // wraparound.
    const int lastPos = tabInfo.tabCtrl->GetPageCount() - 1;
    const int nextPos = forward
        ? tabInfo.tabIdx < lastPos ? tabInfo.tabIdx + 1 : 0
        : tabInfo.tabIdx > 0 ? tabInfo.tabIdx - 1 : lastPos;

    // Now find the corresponding page index.
    return m_tabs.GetIdxFromWindow(tabInfo.tabCtrl->GetWindowFromIdx(nextPos));
}

wxWindow* wxAuiNotebook::DoRemovePage(size_t page_idx)
{
    wxCHECK_MSG(page_idx < GetPageCount(), nullptr, "invalid page index");

    // Lock the window for changes to avoid flicker when
    // removing the active page (there is a noticeable 
    // flicker from the active tab is closed and until a
    // new one is selected) - this is noticeable on MSW
    wxWindowUpdateLocker locker { this };

    // save active window pointer
    wxWindow* active_wnd = nullptr;
    if (m_curPage >= 0)
        active_wnd = m_tabs.GetWindowFromIdx(m_curPage);

    // save pointer of window being deleted
    wxWindow* wnd = m_tabs.GetWindowFromIdx(page_idx);
    wxWindow* new_active = nullptr;

    ShowWnd(wnd, false);

    // find out which onscreen tab ctrl owns this tab
    const auto tabInfo = FindTab(wnd);
    if ( !tabInfo )
        return nullptr;

    wxAuiTabCtrl* const ctrl = tabInfo.tabCtrl;
    int ctrl_idx = tabInfo.tabIdx;

    bool is_curpage = (m_curPage == (int)page_idx);
    bool is_active_in_split = ctrl->GetPage(ctrl_idx).active;


    // remove the tab from main catalog
    m_tabs.RemovePageAt(page_idx);

    // remove the tab from the onscreen tab ctrl
    ctrl->RemovePageAt(ctrl_idx);

    if (is_active_in_split)
    {
        const int ctrl_new_page_count = (int)ctrl->GetPageCount();

        if (ctrl_idx >= ctrl_new_page_count)
            ctrl_idx = ctrl_new_page_count-1;

        if (ctrl_idx >= 0 && ctrl_idx < ctrl_new_page_count)
        {
            // set new page as active in the tab split
            ctrl->SetActivePage(ctrl_idx);

            // if the page deleted was the current page for the
            // entire tab control, then record the window
            // pointer of the new active page for activation
            if (is_curpage)
            {
                new_active = ctrl->GetWindowFromIdx(ctrl_idx);
            }
            else // not deleting the globally active page
            {
                // so don't change the current one
                new_active = active_wnd;

                // but we still need to show the new active page in this
                // control, otherwise it wouldn't be updated
                ctrl->DoShowTab(ctrl_idx);
            }
        }
    }
    else
    {
        // we are not deleting the active page, so keep it the same
        new_active = active_wnd;
    }


    if (!new_active)
    {
        // we haven't yet found a new page to active,
        // so select the next page from the main tab
        // catalogue

        if (page_idx < m_tabs.GetPageCount())
        {
            new_active = m_tabs.GetPage(page_idx).window;
        }

        if (!new_active && m_tabs.GetPageCount() > 0)
        {
            new_active = m_tabs.GetPage(0).window;
        }
    }


    RemoveEmptyTabFrames();

    m_curPage = wxNOT_FOUND;

    // set new active pane unless we're being destroyed anyhow
    if (new_active && !m_isBeingDeleted)
        SetSelectionToWindow(new_active);

    return wnd;
}

int wxAuiNotebook::FindPage(const wxWindow* page) const
{
    return m_tabs.GetIdxFromWindow(page);
}


// SetPageText() changes the tab caption of the specified page
bool wxAuiNotebook::SetPageText(size_t page_idx, const wxString& text)
{
    wxCHECK_MSG(page_idx < GetPageCount(), false, "invalid page index");

    // update our own tab catalog
    wxAuiNotebookPage& page_info = m_tabs.GetPage(page_idx);
    page_info.caption = text;

    // update what's on screen
    if ( const auto tabInfo = FindTab(page_info.window) )
    {
        tabInfo.pageInfo->caption = text;
        tabInfo.tabCtrl->Refresh();
        tabInfo.tabCtrl->Update();
    }

    return true;
}

// returns the page caption
wxString wxAuiNotebook::GetPageText(size_t page_idx) const
{
    wxCHECK_MSG(page_idx < GetPageCount(), wxString(), "invalid page index");

    // update our own tab catalog
    const wxAuiNotebookPage& page_info = m_tabs.GetPage(page_idx);
    return page_info.caption;
}

bool wxAuiNotebook::SetPageToolTip(size_t page_idx, const wxString& text)
{
    wxCHECK_MSG(page_idx < GetPageCount(), false, "invalid page index");

    // update our own tab catalog
    wxAuiNotebookPage& page_info = m_tabs.GetPage(page_idx);
    page_info.tooltip = text;

    const auto tabInfo = FindTab(page_info.window);
    if ( !tabInfo )
        return false;

    tabInfo.pageInfo->tooltip = text;

    // NB: we don't update the tooltip if it is already being displayed, it
    //     typically never happens, no need to code that
    return true;
}

wxString wxAuiNotebook::GetPageToolTip(size_t page_idx) const
{
    wxCHECK_MSG(page_idx < GetPageCount(), wxString(), "invalid page index");

    const wxAuiNotebookPage& page_info = m_tabs.GetPage(page_idx);
    return page_info.tooltip;
}

bool wxAuiNotebook::SetPageBitmap(size_t page_idx, const wxBitmapBundle& bitmap)
{
    wxCHECK_MSG(page_idx < GetPageCount(), false, "invalid page index");

    // update our own tab catalog
    wxAuiNotebookPage& page_info = m_tabs.GetPage(page_idx);
    page_info.bitmap = bitmap;

    // tab height might have changed
    UpdateTabCtrlHeight();

    // update what's on screen
    const auto tabInfo = FindTab(page_info.window);
    if ( !tabInfo )
        return false;

    tabInfo.pageInfo->bitmap = bitmap;
    tabInfo.tabCtrl->Refresh();
    tabInfo.tabCtrl->Update();

    return true;
}

// returns the page bitmap
wxBitmap wxAuiNotebook::GetPageBitmap(size_t page_idx) const
{
    wxCHECK_MSG(page_idx < GetPageCount(), wxBitmap(), "invalid page index");

    // update our own tab catalog
    const wxAuiNotebookPage& page_info = m_tabs.GetPage(page_idx);
    return page_info.bitmap.GetBitmap(page_info.bitmap.GetDefaultSize());
}

wxAuiTabKind wxAuiNotebook::GetPageKind(size_t page_idx) const
{
    wxCHECK_MSG(page_idx < GetPageCount(), wxAuiTabKind::Normal, "invalid page index");

    return m_tabs.GetPage(page_idx).kind;
}

bool wxAuiNotebook::SetPageKind(size_t page_idx, wxAuiTabKind kind)
{
    wxCHECK_MSG(page_idx < GetPageCount(), false, "invalid page index");

    wxAuiNotebookPage& page_info = m_tabs.GetPage(page_idx);
    if ( page_info.kind == kind )
        return false;

    const auto tabInfo = FindTab(page_info.window);
    if ( !tabInfo )
        return false;

    auto& tabCtrl = *tabInfo.tabCtrl;

    // Update the tab position as changing its kind moves it: if it becomes
    // locked/pinned, it needs to be moved to the end of the group of the tabs
    // of this kind and if it becomes normal, it needs to be moved to the
    // beginning of the normal tabs (and not the end, to minimize the distance
    // by which it moves).
    size_t newIdx = 0;
    switch ( kind )
    {
        case wxAuiTabKind::Normal:
            newIdx = tabCtrl.GetFirstTabOfKind(wxAuiTabKind::Normal);

            // There must be at least the locked/pinned tab which is changing
            // its kind now before the first normal one.
            wxASSERT( newIdx > 0 );

            // Move it before the first normal tab or before the end.
            newIdx--;
            break;

        case wxAuiTabKind::Pinned:
            // When a normal tab becomes pinned, it should be moved to the end
            // of the pinned tabs, but when a locked tab becomes pinned, it
            // makes more sense to put it at the beginning of this group, for
            // symmetry with what happens when a pinned tab becomes normal.
            switch ( tabInfo.pageInfo->kind )
            {
                case wxAuiTabKind::Normal:
                    newIdx = tabCtrl.GetFirstTabOfKind(wxAuiTabKind::Normal);
                    break;

                case wxAuiTabKind::Pinned:
                    // This tab is switching to pinned state, so it couldn't
                    // have already been pinned before.
                    wxFAIL_MSG("unreachable");
                    break;

                case wxAuiTabKind::Locked:
                    newIdx = tabCtrl.GetFirstTabNotOfKind(wxAuiTabKind::Locked);

                    // There must be at least one locked tab, which is
                    // switching to pinned state right now.
                    wxASSERT( newIdx > 0 );

                    newIdx--;
                    break;
            }
            break;

        case wxAuiTabKind::Locked:
            newIdx = tabCtrl.GetFirstTabNotOfKind(wxAuiTabKind::Locked);
            break;
    }

    page_info.kind = kind;

    // And also update kind in this copy of the page info before moving it.
    tabInfo.pageInfo->kind = kind;

    tabCtrl.MovePage(tabInfo.tabIdx, newIdx);
    tabCtrl.Refresh();

    return true;
}

// GetSelection() returns the index of the currently active page
int wxAuiNotebook::GetSelection() const
{
    return m_curPage;
}

// SetSelection() sets the currently active page
int wxAuiNotebook::SetSelection(size_t new_page)
{
    return DoModifySelection(new_page, true);
}

void wxAuiNotebook::SetSelectionToWindow(wxWindow *win)
{
    const int idx = m_tabs.GetIdxFromWindow(win);
    wxCHECK_RET( idx != wxNOT_FOUND, wxT("invalid notebook page") );


    // since a tab was clicked, let the parent know that we received
    // the focus, even if we will assign that focus immediately
    // to the child tab in the SetSelection call below
    // (the child focus event will also let wxAuiManager, if any,
    // know that the notebook control has been activated)

    wxWindow* parent = GetParent();
    if (parent)
    {
        wxChildFocusEvent eventFocus(this);
        parent->ProcessWindowEvent(eventFocus);
    }


    SetSelection(idx);
}

// GetPageCount() returns the total number of
// pages managed by the multi-notebook
size_t wxAuiNotebook::GetPageCount() const
{
    return m_tabs.GetPageCount();
}

// GetPage() returns the wxWindow pointer of the
// specified page
wxWindow* wxAuiNotebook::GetPage(size_t page_idx) const
{
    wxASSERT(page_idx < m_tabs.GetPageCount());

    return m_tabs.GetWindowFromIdx(page_idx);
}

// DoSizing() performs all sizing operations in each tab control
void wxAuiNotebook::DoSizing()
{
    for ( const auto& pane : m_mgr.GetAllPanes() )
    {
        if ( IsDummyPane(pane) )
            continue;

        wxAuiTabFrame* tabframe = (wxAuiTabFrame*)pane.window;
        tabframe->DoSizing();
    }
}

// GetActiveTabCtrl() returns the active tab control.  It is
// called to determine which control gets new windows being added
wxAuiTabCtrl* wxAuiNotebook::GetActiveTabCtrl()
{
    if (m_curPage >= 0 && m_curPage < (int)m_tabs.GetPageCount())
    {
        // find the tab ctrl with the current page
        return FindTab(m_tabs.GetPage(m_curPage).window).tabCtrl;
    }

    // no current page, just return the main tab control
    return GetMainTabCtrl();
}

wxAuiTabCtrl* wxAuiNotebook::GetMainTabCtrl()
{
    wxAuiTabCtrl* tabMain = nullptr;
    for ( const auto& pane : m_mgr.GetAllPanes() )
    {
        if ( IsDummyPane(pane) )
            continue;

        if ( pane.dock_direction == wxAUI_DOCK_CENTER )
        {
            wxASSERT_MSG( !tabMain, "Multiple main tab controls?" );

            tabMain = static_cast<wxAuiTabFrame*>(pane.window)->m_tabs;
        }
    }

    if ( !tabMain )
    {
        wxAuiTabFrame* tabframe = CreateTabFrame();
        m_mgr.AddPane(tabframe,
                      wxAuiPaneInfo().Center().CaptionVisible(false));
        m_mgr.Update();

        tabMain = tabframe->m_tabs;
    }

    return tabMain;
}

std::vector<wxAuiTabCtrl*> wxAuiNotebook::GetAllTabCtrls()
{
    std::vector<wxAuiTabCtrl*> tabCtrls;
    for ( const auto& pane : m_mgr.GetAllPanes() )
    {
        if ( IsDummyPane(pane) )
            continue;

        wxAuiTabFrame* tabframe = (wxAuiTabFrame*)pane.window;
        tabCtrls.push_back(tabframe->m_tabs);
    }

    if ( tabCtrls.empty() )
    {
        tabCtrls.push_back(GetMainTabCtrl());
    }

    return tabCtrls;
}

wxAuiNotebook::TabInfo wxAuiNotebook::FindTab(wxWindow* wnd) const
{
    for ( const auto& pane : m_mgr.GetAllPanes() )
    {
        if ( IsDummyPane(pane) )
            continue;

        wxAuiTabFrame* tabframe = (wxAuiTabFrame*)pane.window;
        wxAuiTabCtrl* const tabCtrl = tabframe->m_tabs;

        const size_t page_count = tabCtrl->GetPageCount();
        for ( size_t i = 0; i < page_count; ++i )
        {
            wxAuiNotebookPage& page = tabCtrl->GetPage(i);
            if ( page.window == wnd )
                return { tabCtrl, static_cast<int>(i), &page };
        }
    }

    wxFAIL_MSG( "Window unexpectedly not found in any tab control" );

    return {};
}

// This overload is deprecated and kept only for compatibility.
bool wxAuiNotebook::FindTab(wxWindow* page, wxAuiTabCtrl** ctrl, int* idx) const
{
    const auto tabInfo = FindTab(page);
    if ( !tabInfo )
        return false;

    *ctrl = tabInfo.tabCtrl;
    *idx = tabInfo.tabIdx;

    return true;
}

int
wxAuiNotebook::GetDropIndex(const wxAuiNotebookPage& page_info,
                            wxAuiTabCtrl* dest_tabs,
                            const wxPoint& mouse_screen_pt) const
{
    int insert_idx = -1;

    wxPoint pt = dest_tabs->ScreenToClient(mouse_screen_pt);
    if (auto const targetInfo = dest_tabs->TabHitTest(pt))
    {
        insert_idx = targetInfo.pos;

        // Check that we don't try to insert a tab between tabs of
        // inappropriate kind, kinds must always remain in a (non-strict)
        // decreasing order.
        if ( insert_idx > 0 )
        {
            if ( dest_tabs->GetPage(insert_idx - 1).kind < page_info.kind )
                return wxNOT_FOUND;
        }

        if ( page_info.kind < dest_tabs->GetPage(insert_idx).kind )
            return wxNOT_FOUND;
    }
    else // Append the tab if there is no existing tab under it.
    {
        insert_idx = dest_tabs->GetPageCount();

        // When appending we need just a single test.
        if ( dest_tabs->GetPage(insert_idx - 1).kind < page_info.kind )
            return wxNOT_FOUND;
    }

    return insert_idx;
}

wxAuiNotebookPosition wxAuiNotebook::GetPagePosition(size_t page) const
{
    return FindTab(GetPage(page));
}

std::vector<size_t>
wxAuiNotebook::GetPagesInDisplayOrder(wxAuiTabCtrl* tabCtrl) const
{
    wxCHECK_MSG( tabCtrl, std::vector<size_t>(), "invalid tab control" );

    const size_t count = tabCtrl->GetPageCount();
    std::vector<size_t> pages(count);
    for ( size_t i = 0; i < count; ++i )
    {
        pages[i] = m_tabs.GetIdxFromWindow(tabCtrl->GetWindowFromIdx(i));
    }

    return pages;
}

void wxAuiNotebook::Split(size_t page, int direction)
{
    wxSize cli_size = GetClientSize();

    // get the page's window pointer
    wxWindow* wnd = GetPage(page);
    if (!wnd)
        return;

    // notebooks with 1 or less pages can't be split
    if (GetPageCount() < 2)
        return;

    // find out which tab control the page currently belongs to
    const auto srcTabInfo = FindTab(wnd);
    if ( !srcTabInfo )
        return;

    wxAuiTabCtrl* const src_tabs = srcTabInfo.tabCtrl;

    // create a new tab frame
    wxAuiTabFrame* new_tabs = CreateTabFrame(CalculateNewSplitSize());
    wxAuiTabCtrl* const dest_tabs = new_tabs->m_tabs;

    // create a pane info structure with the information
    // about where the pane should be added
    wxAuiPaneInfo paneInfo = wxAuiPaneInfo().Bottom().CaptionVisible(false);
    wxPoint mouse_pt;

    if (direction == wxLEFT)
    {
        paneInfo.Left();
        mouse_pt = wxPoint(0, cli_size.y/2);
    }
    else if (direction == wxRIGHT)
    {
        paneInfo.Right();
        mouse_pt = wxPoint(cli_size.x, cli_size.y/2);
    }
    else if (direction == wxTOP)
    {
        paneInfo.Top();
        mouse_pt = wxPoint(cli_size.x/2, 0);
    }
    else if (direction == wxBOTTOM)
    {
        paneInfo.Bottom();
        mouse_pt = wxPoint(cli_size.x/2, cli_size.y);
    }

    m_mgr.AddPane(new_tabs, paneInfo, mouse_pt);
    m_mgr.Update();

    // remove the page from the source tabs
    wxAuiNotebookPage page_info = *srcTabInfo.pageInfo;
    page_info.active = false;
    src_tabs->RemovePageAt(srcTabInfo.tabIdx);
    if (src_tabs->GetPageCount() > 0)
    {
        src_tabs->SetActivePage((size_t)0);
        src_tabs->DoUpdateActive();
    }


    // add the page to the destination tabs
    dest_tabs->InsertPage(page_info, 0);

    if (src_tabs->GetPageCount() == 0)
    {
        RemoveEmptyTabFrames();
    }

    DoSizing();
    dest_tabs->DoUpdateActive();

    // force the set selection function reset the selection
    m_curPage = wxNOT_FOUND;

    // set the active page to the one we just split off
    SetSelectionToPage(page_info);

    UpdateHintWindowSize();
}

void wxAuiNotebook::UnsplitAll()
{
    auto* const tabMain = GetMainTabCtrl();

    bool changed = false;
    for ( const auto& pane : m_mgr.GetAllPanes() )
    {
        if ( IsDummyPane(pane) )
            continue;

        auto* const tab = static_cast<wxAuiTabFrame*>(pane.window)->m_tabs;

        // Don't move tabs from the main tab control to itself.
        if ( tab == tabMain )
            continue;

        while ( tab->GetPageCount() )
        {
            wxAuiNotebookPage info = tab->GetPage(0);
            info.active = false;

            tab->RemovePageAt(0);

            tabMain->AddPage(info);

            changed = true;
        }
    }

    if ( changed )
    {
        RemoveEmptyTabFrames();

        DoSizing();
    }
}

void wxAuiNotebook::OnSize(wxSizeEvent& evt)
{
    UpdateHintWindowSize();

    evt.Skip();
}

void wxAuiNotebook::OnTabClicked(wxAuiNotebookEvent& evt)
{
    wxAuiTabCtrl* ctrl = (wxAuiTabCtrl*)evt.GetEventObject();
    wxASSERT(ctrl != nullptr);

    wxWindow* wnd = ctrl->GetWindowFromIdx(evt.GetSelection());
    wxASSERT(wnd != nullptr);

    SetSelectionToWindow(wnd);
}

void wxAuiNotebook::OnTabBgDClick(wxAuiNotebookEvent& evt)
{
    // select the tab ctrl which received the db click
    int selection;
    wxWindow* wnd;
    wxAuiTabCtrl* ctrl = (wxAuiTabCtrl*)evt.GetEventObject();
    if (   (ctrl != nullptr)
        && ((selection = ctrl->GetActivePage()) != wxNOT_FOUND)
        && ((wnd = ctrl->GetWindowFromIdx(selection)) != nullptr))
    {
        SetSelectionToWindow(wnd);
    }

    // notify owner that the tabbar background has been double-clicked
    wxAuiNotebookEvent e(wxEVT_AUINOTEBOOK_BG_DCLICK, m_windowId);
    e.SetEventObject(this);
    ProcessWindowEvent(e);
}

void wxAuiNotebook::OnTabBeginDrag(wxAuiNotebookEvent&)
{
    m_lastDropMovePos = -1;
}

void wxAuiNotebook::OnTabDragMotion(wxAuiNotebookEvent& evt)
{
    wxPoint screen_pt = ::wxGetMousePosition();
    wxPoint client_pt = ScreenToClient(screen_pt);

    wxAuiTabCtrl* const src_tabs = (wxAuiTabCtrl*)evt.GetEventObject();
    wxAuiTabCtrl* const dest_tabs = GetTabCtrlFromPoint(client_pt);

    if (dest_tabs == src_tabs)
    {
        if (src_tabs)
        {
            src_tabs->SetCursor(wxCursor(wxCURSOR_ARROW));
        }

        // always hide the hint for inner-tabctrl drag
        m_mgr.HideHint();

        // if tab moving is not allowed, leave
        if (!(m_flags & wxAUI_NB_TAB_MOVE))
        {
            return;
        }

        wxPoint pt = dest_tabs->ScreenToClient(screen_pt);

        // When using multiple rows, allow dragging the tab to the space after
        // the last tab of the row too.
        int flags = wxAuiTabContainer::HitTest_Default;
        if (dest_tabs->IsFlagSet(wxAUI_NB_MULTILINE))
            flags |= wxAuiTabContainer::HitTest_AllowAfterTab;

        // this is an inner-tab drag/reposition
        if (auto const destTabInfo = dest_tabs->TabHitTest(pt, flags))
        {
            int src_idx = evt.GetSelection();
            wxCHECK_RET( src_idx != -1, "Invalid source tab?" );

            int dest_idx = destTabInfo.pos;

            if ( src_idx == dest_idx )
            {
                // Reset the last drop position, the mouse has moved away from
                // the other tab, so now the user should be able to drag this
                // one there again.
                m_lastDropMovePos = -1;
                return;
            }

            const auto& src_page = src_tabs->GetPage(src_idx);

            // A tab can only be moved inside the group of tabs of the same
            // kind, as otherwise the tabs of the same kind wouldn't be grouped
            // together any longer.
            if ( dest_tabs->GetPage(dest_idx).kind != src_page.kind )
                return;

            // When dragging a smaller tab over the larger one, after moving
            // the tab into the new position, the same point can be now over a
            // different tab, which would result in another move of this tab as
            // soon as the mouse moves even by a single pixel. And then, of
            // course, it would move again, and so on. To avoid this, we need
            // to check if the mouse has moved from the tab over which it was
            // when the last drop occurred.
            if ( dest_idx == m_lastDropMovePos )
                return;

            if (dest_tabs->MovePage(src_page.window, dest_idx))
            {
                // Update the layout when using multiline tabs as it can change
                // depending on the tab order.
                if (dest_tabs->IsFlagSet(wxAUI_NB_MULTILINE))
                    dest_tabs->LayoutMultiLineTabs(dest_tabs);

                dest_tabs->SetActivePage((size_t)dest_idx);
                dest_tabs->DoUpdateActive();
            }

            m_lastDropMovePos = dest_tabs->TabHitTest(pt).pos;
        }

        return;
    }


    // if external drag is allowed, check if the tab is being dragged
    // over a different wxAuiNotebook control
    if (m_flags & wxAUI_NB_TAB_EXTERNAL_MOVE)
    {
        wxWindow* tab_ctrl = ::wxFindWindowAtPoint(screen_pt);

        // if we aren't over any window, stop here
        if (!tab_ctrl)
            return;

        // make sure we are not over the hint window
        if (!wxDynamicCast(tab_ctrl, wxFrame))
        {
            wxAuiTabCtrl* other_tabs = nullptr;
            while (tab_ctrl)
            {
                other_tabs = wxDynamicCast(tab_ctrl, wxAuiTabCtrl);
                if (other_tabs)
                    break;
                tab_ctrl = tab_ctrl->GetParent();
            }

            if (other_tabs)
            {
                wxAuiNotebook* nb = (wxAuiNotebook*)tab_ctrl->GetParent();

                if (nb != this)
                {
                    m_mgr.UpdateHint(other_tabs->GetHintScreenRect());
                    return;
                }
            }
        }
        else
        {
            if (!dest_tabs)
            {
                // we are either over a hint window, or not over a tab
                // window, and there is no where to drag to, so exit
                return;
            }
        }
    }


    // if there are less than two panes, split can't happen, so leave
    if (m_tabs.GetPageCount() < 2)
        return;

    // if tab moving is not allowed, leave
    if (!(m_flags & wxAUI_NB_TAB_SPLIT))
        return;


    if (src_tabs)
    {
        src_tabs->SetCursor(wxCursor(wxCURSOR_SIZING));
    }


    wxRect hintRect;
    if (dest_tabs)
    {
        if (src_tabs)
        {
            const auto& src_page = src_tabs->GetPage(evt.GetSelection());
            if (GetDropIndex(src_page, dest_tabs, screen_pt) == wxNOT_FOUND)
            {
                m_mgr.HideHint();
                return;
            }
        }

        hintRect = dest_tabs->GetHintScreenRect();
    }
    else
    {
        hintRect = m_mgr.CalculateHintRect(m_dummyWnd, client_pt);
    }

    m_mgr.UpdateHint(hintRect);
}



void wxAuiNotebook::OnTabEndDrag(wxAuiNotebookEvent& evt)
{
    m_mgr.HideHint();


    wxAuiTabCtrl* src_tabs = (wxAuiTabCtrl*)evt.GetEventObject();
    wxCHECK_RET( src_tabs, wxT("no source object?") );

    src_tabs->SetCursor(wxCursor(wxCURSOR_ARROW));

    // get the mouse position, which will be used to determine the drop point
    wxPoint mouse_screen_pt = ::wxGetMousePosition();
    wxPoint mouse_client_pt = ScreenToClient(mouse_screen_pt);



    // check for an external move
    if (m_flags & wxAUI_NB_TAB_EXTERNAL_MOVE)
    {
        wxWindow* tab_ctrl = ::wxFindWindowAtPoint(mouse_screen_pt);

        while (tab_ctrl)
        {
            if (wxDynamicCast(tab_ctrl, wxAuiTabCtrl))
                break;
            tab_ctrl = tab_ctrl->GetParent();
        }

        if (tab_ctrl)
        {
            wxAuiNotebook* nb = (wxAuiNotebook*)tab_ctrl->GetParent();

            if (nb != this)
            {
                // find out from the destination control
                // if it's ok to drop this tab here
                wxAuiNotebookEvent e(wxEVT_AUINOTEBOOK_ALLOW_DND, m_windowId);
                e.SetSelection(evt.GetSelection());
                e.SetOldSelection(evt.GetSelection());
                e.SetEventObject(this);
                e.SetDragSource(this);
                e.Veto(); // dropping must be explicitly approved by control owner

                nb->ProcessWindowEvent(e);

                if (!e.IsAllowed())
                {
                    // no answer or negative answer
                    m_mgr.HideHint();
                    return;
                }

                // drop was allowed
                int src_idx = evt.GetSelection();
                wxWindow* src_page = src_tabs->GetWindowFromIdx(src_idx);

                // Check that it's not an impossible parent relationship
                wxWindow* p = nb;
                while (p && !p->IsTopLevel())
                {
                    if (p == src_page)
                    {
                        return;
                    }
                    p = p->GetParent();
                }

                // get main index of the page
                int main_idx = m_tabs.GetIdxFromWindow(src_page);
                wxCHECK_RET( main_idx != wxNOT_FOUND, wxT("no source page?") );


                // make a copy of the page info
                wxAuiNotebookPage page_info = m_tabs.GetPage(main_idx);
                page_info.hover = false;

                // remove the page from the source notebook
                RemovePage(main_idx);


                // found out the insert idx
                wxAuiTabCtrl* dest_tabs = (wxAuiTabCtrl*)tab_ctrl;
                wxPoint pt = dest_tabs->ScreenToClient(mouse_screen_pt);

                int insert_idx = -1;
                if (auto const targetInfo = dest_tabs->TabHitTest(pt))
                {
                    insert_idx = targetInfo.pos;
                }


                // add the page to the new notebook: note that we always append
                // it to the end in logical order, as it's not clear what
                // should its position be (the alternative would be to insert
                // it before the target page, but it's not really clear if this
                // is really what we want)
                nb->InsertPageAt(page_info, nb->GetPageCount(),
                                 dest_tabs, insert_idx,
                                 true /* select */);

                // notify owner that the tab has been dragged
                wxAuiNotebookEvent e2(wxEVT_AUINOTEBOOK_DRAG_DONE, m_windowId);
                e2.SetSelection(evt.GetSelection());
                e2.SetOldSelection(evt.GetSelection());
                e2.SetEventObject(this);
                ProcessWindowEvent(e2);

                return;
            }
        }
    }




    // only perform a tab split if it's allowed
    wxAuiTabCtrl* dest_tabs = nullptr;

    if ((m_flags & wxAUI_NB_TAB_SPLIT) && m_tabs.GetPageCount() >= 2)
    {
        wxAuiNotebookPage page_info = src_tabs->GetPage(evt.GetSelection());

        // If the pointer is in an existing tab frame, do a tab insert
        wxWindow* hit_wnd = ::wxFindWindowAtPoint(mouse_screen_pt);
        wxAuiTabFrame* tab_frame = (wxAuiTabFrame*)GetTabFrameFromTabCtrl(hit_wnd);
        int insert_idx = -1;
        if (tab_frame)
        {
            dest_tabs = tab_frame->m_tabs;

            if (dest_tabs == src_tabs)
                return;

            insert_idx = GetDropIndex(page_info, dest_tabs, mouse_screen_pt);
            if ( insert_idx == wxNOT_FOUND )
                return;
        }
        else
        {
            wxRect rect = m_mgr.CalculateHintRect(m_dummyWnd,
                                                  mouse_client_pt);
            if (rect.IsEmpty())
            {
                // there is no suitable drop location here, exit out
                return;
            }

            // If there is no tabframe at all, create one
            wxAuiTabFrame* new_tabs = CreateTabFrame(CalculateNewSplitSize());

            m_mgr.AddPane(new_tabs,
                          wxAuiPaneInfo().Bottom().CaptionVisible(false),
                          mouse_client_pt);
            m_mgr.Update();
            dest_tabs = new_tabs->m_tabs;
            insert_idx = 0;
        }



        // remove the page from the source tabs
        page_info.active = false;
        src_tabs->RemovePage(page_info.window);
        if (src_tabs->GetPageCount() > 0)
        {
            src_tabs->SetActivePage((size_t)0);
            src_tabs->DoUpdateActive();
        }



        // add the page to the destination tabs
        dest_tabs->InsertPage(page_info, insert_idx);

        if (src_tabs->GetPageCount() == 0)
        {
            RemoveEmptyTabFrames();
        }

        DoSizing();
        dest_tabs->DoUpdateActive();

        // force the set selection function reset the selection
        m_curPage = wxNOT_FOUND;

        // set the active page to the one we just split off
        SetSelectionToPage(page_info);

        UpdateHintWindowSize();
    }

    // notify owner that the tab has been dragged
    wxAuiNotebookEvent e(wxEVT_AUINOTEBOOK_DRAG_DONE, m_windowId);
    e.SetSelection(evt.GetSelection());
    e.SetOldSelection(evt.GetSelection());
    e.SetEventObject(this);
    ProcessWindowEvent(e);
}



void wxAuiNotebook::OnTabCancelDrag(wxAuiNotebookEvent& command_evt)
{
    wxAuiNotebookEvent& evt = (wxAuiNotebookEvent&)command_evt;

    m_mgr.HideHint();

    wxAuiTabCtrl* src_tabs = (wxAuiTabCtrl*)evt.GetEventObject();
    wxCHECK_RET( src_tabs, wxT("no source object?") );

    src_tabs->SetCursor(wxCursor(wxCURSOR_ARROW));
}

wxAuiTabCtrl* wxAuiNotebook::GetTabCtrlFromPoint(const wxPoint& pt)
{
    // if we've just removed the last tab from the source
    // tab set, the remove the tab control completely
    for ( const auto& pane : m_mgr.GetAllPanes() )
    {
        if ( IsDummyPane(pane) )
            continue;

        wxAuiTabFrame* tabframe = (wxAuiTabFrame*)pane.window;
        if (tabframe->m_tab_rect.Contains(pt))
            return tabframe->m_tabs;
    }

    return nullptr;
}

wxWindow* wxAuiNotebook::GetTabFrameFromTabCtrl(wxWindow* tab_ctrl)
{
    // if we've just removed the last tab from the source
    // tab set, the remove the tab control completely
    for ( const auto& pane : m_mgr.GetAllPanes() )
    {
        if ( IsDummyPane(pane) )
            continue;

        wxAuiTabFrame* tabframe = (wxAuiTabFrame*)pane.window;
        if (tabframe->m_tabs == tab_ctrl)
        {
            return tabframe;
        }
    }

    return nullptr;
}

void wxAuiNotebook::RemoveEmptyTabFrames()
{
    // if we've just removed the last tab from the source
    // tab set, then remove the tab control completely

    // Make a copy of all panes because the original is modified when detaching
    // the pane in the loop.
    wxAuiPaneInfoArray all_panes = m_mgr.GetAllPanes();
    for ( const auto& pane : all_panes )
    {
        if ( IsDummyPane(pane) )
            continue;

        wxAuiTabFrame* tab_frame = (wxAuiTabFrame*)pane.window;
        if (tab_frame->m_tabs->GetPageCount() == 0)
        {
            m_mgr.DetachPane(tab_frame);

            delete tab_frame;
        }
    }


    // check to see if there is still a center pane;
    // if there isn't, make a frame the center pane
    wxWindow* first_good = nullptr;
    bool center_found = false;
    for ( const auto& pane : m_mgr.GetAllPanes() )
    {
        if ( IsDummyPane(pane) )
            continue;
        if (pane.dock_direction == wxAUI_DOCK_CENTRE)
            center_found = true;
        if (!first_good)
            first_good = pane.window;
    }

    if (!center_found && first_good)
    {
        m_mgr.GetPane(first_good).Centre();
    }

    if (!m_isBeingDeleted)
        m_mgr.Update();
}

void wxAuiNotebook::OnChildFocusNotebook(wxChildFocusEvent& evt)
{
    evt.Skip();

    // if we're dragging a tab, don't change the current selection.
    // This code prevents a bug that used to happen when the hint window
    // was hidden.  In the bug, the focus would return to the notebook
    // child, which would then enter this handler and call
    // SetSelection, which is not desired turn tab dragging.

    for ( const auto& pane : m_mgr.GetAllPanes() )
    {
        if ( IsDummyPane(pane) )
            continue;
        wxAuiTabFrame* tabframe = (wxAuiTabFrame*)pane.window;
        if (tabframe->m_tabs->IsDragging())
            return;
    }


    // find the page containing the focused child
    wxWindow* win = evt.GetWindow();
    while ( win )
    {
        // pages have the notebook as the parent, so stop when we reach one
        // (and also stop in the impossible case of no parent at all)
        wxWindow* const parent = win->GetParent();
        if ( !parent || parent == this )
            break;

        win = parent;
    }

    // change the tab selection to this page
    int idx = m_tabs.GetIdxFromWindow(win);
    if (idx != -1 && idx != m_curPage)
    {
        SetSelection(idx);
    }
}

void wxAuiNotebook::OnNavigationKeyNotebook(wxNavigationKeyEvent& event)
{
    if ( event.IsWindowChange() ) {
        // change pages
        // FIXME: the problem with this is that if we have a split notebook,
        // we selection may go all over the place.
        AdvanceSelection(event.GetDirection());
    }
    else {
        // we get this event in 3 cases
        //
        // a) one of our pages might have generated it because the user TABbed
        // out from it in which case we should propagate the event upwards and
        // our parent will take care of setting the focus to prev/next sibling
        //
        // or
        //
        // b) the parent panel wants to give the focus to us so that we
        // forward it to our selected page. We can't deal with this in
        // OnSetFocus() because we don't know which direction the focus came
        // from in this case and so can't choose between setting the focus to
        // first or last panel child
        //
        // or
        //
        // c) we ourselves (see MSWTranslateMessage) generated the event
        //
        wxWindow * const parent = GetParent();

        // the wxObject* casts are required to avoid MinGW GCC 2.95.3 ICE
        const bool isFromParent = event.GetEventObject() == (wxObject*) parent;
        const bool isFromSelf = event.GetEventObject() == (wxObject*) this;

        if ( isFromParent || isFromSelf )
        {
            // no, it doesn't come from child, case (b) or (c): forward to a
            // page but only if direction is backwards (TAB) or from ourselves,
            if ( GetSelection() != wxNOT_FOUND &&
                    (!event.GetDirection() || isFromSelf) )
            {
                // so that the page knows that the event comes from it's parent
                // and is being propagated downwards
                event.SetEventObject(this);

                wxWindow *page = GetPage(GetSelection());
                if ( !page->ProcessWindowEvent(event) )
                {
                    page->SetFocus();
                }
                //else: page manages focus inside it itself
            }
            else // otherwise set the focus to the notebook itself
            {
                SetFocus();
            }
        }
        else
        {
            // it comes from our child, case (a), pass to the parent, but only
            // if the direction is forwards. Otherwise set the focus to the
            // notebook itself. The notebook is always the 'first' control of a
            // page.
            if ( !event.GetDirection() )
            {
                SetFocus();
            }
            else if ( parent )
            {
                event.SetCurrentFocus(this);
                parent->ProcessWindowEvent(event);
            }
        }
    }
}

void wxAuiNotebook::OnTabButton(wxAuiNotebookEvent& evt)
{
    wxAuiTabCtrl* tabs = (wxAuiTabCtrl*)evt.GetEventObject();

    int button_id = evt.GetInt();

    if (button_id == wxAUI_BUTTON_CLOSE)
    {
        int selection = evt.GetSelection();

        if (selection == -1)
        {
            // if the close button is to the right, use the active
            // page selection to determine which page to close
            selection = tabs->GetActivePage();
        }

        if (selection != -1)
        {
            wxWindow* close_wnd = tabs->GetWindowFromIdx(selection);

            // ask owner if it's ok to close the tab
            wxAuiNotebookEvent e(wxEVT_AUINOTEBOOK_PAGE_CLOSE, m_windowId);
            e.SetSelection(m_tabs.GetIdxFromWindow(close_wnd));
            const int idx = m_tabs.GetIdxFromWindow(close_wnd);
            e.SetSelection(idx);
            e.SetOldSelection(evt.GetSelection());
            e.SetEventObject(this);
            ProcessWindowEvent(e);
            if (!e.IsAllowed())
                return;


#if wxUSE_MDI
            if (wxDynamicCast(close_wnd, wxAuiMDIChildFrame))
            {
                close_wnd->Close();
            }
            else
#endif
            {
                int main_idx = m_tabs.GetIdxFromWindow(close_wnd);
                wxCHECK_RET( main_idx != wxNOT_FOUND, wxT("no page to delete?") );

                DeletePage(main_idx);
            }

            // notify owner that the tab has been closed
            wxAuiNotebookEvent e2(wxEVT_AUINOTEBOOK_PAGE_CLOSED, m_windowId);
            e2.SetSelection(idx);
            e2.SetEventObject(this);
            ProcessWindowEvent(e2);
        }
    }
    else if (button_id == wxAUI_BUTTON_PIN)
    {
        // For now we don't send any event, this can be always added later if
        // necessary.
        wxWindow* const wnd = tabs->GetWindowFromIdx(evt.GetSelection());

        const auto idx = m_tabs.GetIdxFromWindow(wnd);

        wxAuiTabKind newKind = wxAuiTabKind::Locked;
        switch ( GetPageKind(idx) )
        {
            case wxAuiTabKind::Normal:
                newKind = wxAuiTabKind::Pinned;
                break;

            case wxAuiTabKind::Pinned:
                newKind = wxAuiTabKind::Normal;
                break;

            case wxAuiTabKind::Locked:
                // Locked tabs can't be pinned or unpinned.
                break;
        }

        wxCHECK_RET(newKind != wxAuiTabKind::Locked,
                    "locked pages shouldn't have pin button");

        SetPageKind(idx, newKind);
    }
}


void wxAuiNotebook::OnTabMiddleDown(wxAuiNotebookEvent& evt)
{
    // patch event through to owner
    wxAuiTabCtrl* tabs = (wxAuiTabCtrl*)evt.GetEventObject();
    wxWindow* wnd = tabs->GetWindowFromIdx(evt.GetSelection());

    wxAuiNotebookEvent e(wxEVT_AUINOTEBOOK_TAB_MIDDLE_DOWN, m_windowId);
    e.SetSelection(m_tabs.GetIdxFromWindow(wnd));
    e.SetEventObject(this);
    ProcessWindowEvent(e);
}

void wxAuiNotebook::OnTabMiddleUp(wxAuiNotebookEvent& evt)
{
    // if the wxAUI_NB_MIDDLE_CLICK_CLOSE is specified, middle
    // click should act like a tab close action.  However, first
    // give the owner an opportunity to handle the middle up event
    // for custom action

    wxAuiTabCtrl* tabs = (wxAuiTabCtrl*)evt.GetEventObject();
    wxWindow* wnd = tabs->GetWindowFromIdx(evt.GetSelection());

    wxAuiNotebookEvent e(wxEVT_AUINOTEBOOK_TAB_MIDDLE_UP, m_windowId);
    e.SetSelection(m_tabs.GetIdxFromWindow(wnd));
    e.SetEventObject(this);
    if (ProcessWindowEvent(e))
        return;
    if (!e.IsAllowed())
        return;

    // check if we are supposed to close on middle-up
    if ((m_flags & wxAUI_NB_MIDDLE_CLICK_CLOSE) == 0)
        return;

    // simulate the user pressing the close button on the tab
    evt.SetInt(wxAUI_BUTTON_CLOSE);
    OnTabButton(evt);
}

void wxAuiNotebook::OnTabRightDown(wxAuiNotebookEvent& evt)
{
    // patch event through to owner
    wxAuiTabCtrl* tabs = (wxAuiTabCtrl*)evt.GetEventObject();
    wxWindow* wnd = tabs->GetWindowFromIdx(evt.GetSelection());

    wxAuiNotebookEvent e(wxEVT_AUINOTEBOOK_TAB_RIGHT_DOWN, m_windowId);
    e.SetSelection(m_tabs.GetIdxFromWindow(wnd));
    e.SetEventObject(this);
    ProcessWindowEvent(e);
}

void wxAuiNotebook::OnTabRightUp(wxAuiNotebookEvent& evt)
{
    // patch event through to owner
    wxAuiTabCtrl* tabs = (wxAuiTabCtrl*)evt.GetEventObject();
    wxWindow* wnd = tabs->GetWindowFromIdx(evt.GetSelection());

    wxAuiNotebookEvent e(wxEVT_AUINOTEBOOK_TAB_RIGHT_UP, m_windowId);
    e.SetSelection(m_tabs.GetIdxFromWindow(wnd));
    e.SetEventObject(this);
    ProcessWindowEvent(e);
}

// Sets the normal font
void wxAuiNotebook::SetNormalFont(const wxFont& font)
{
    m_normalFont = font;
    GetArtProvider()->SetNormalFont(font);
}

// Sets the selected tab font
void wxAuiNotebook::SetSelectedFont(const wxFont& font)
{
    m_selectedFont = font;
    GetArtProvider()->SetSelectedFont(font);
}

// Sets the measuring font
void wxAuiNotebook::SetMeasuringFont(const wxFont& font)
{
    GetArtProvider()->SetMeasuringFont(font);
}

// Sets the tab font
bool wxAuiNotebook::SetFont(const wxFont& font)
{
    wxControl::SetFont(font);

    wxFont normalFont(font);
    wxFont selectedFont(normalFont);
    selectedFont.SetWeight(wxFONTWEIGHT_BOLD);

    SetNormalFont(normalFont);
    SetSelectedFont(selectedFont);
    SetMeasuringFont(selectedFont);

    return true;
}

// Gets the tab control height
int wxAuiNotebook::GetTabCtrlHeight() const
{
    return m_tabCtrlHeight;
}

// Gets the height of the notebook for a given page height
int wxAuiNotebook::GetHeightForPageHeight(int pageHeight)
{
    UpdateTabCtrlHeight();

    int tabCtrlHeight = GetTabCtrlHeight();
    int decorHeight = 2;
    return tabCtrlHeight + pageHeight + decorHeight;
}

// Shows the window menu
bool wxAuiNotebook::ShowWindowMenu()
{
    wxAuiTabCtrl* tabCtrl = GetActiveTabCtrl();

    int idx = tabCtrl->GetArtProvider()->ShowDropDown(tabCtrl, tabCtrl->GetPages(), tabCtrl->GetActivePage());

    if (idx != -1)
    {
        wxAuiNotebookEvent e(wxEVT_AUINOTEBOOK_PAGE_CHANGING, tabCtrl->GetId());
        e.SetSelection(idx);
        e.SetOldSelection(tabCtrl->GetActivePage());
        e.SetEventObject(tabCtrl);
        ProcessWindowEvent(e);

        return true;
    }
    else
        return false;
}

void wxAuiNotebook::DoThaw()
{
    DoSizing();

    wxBookCtrlBase::DoThaw();
}

void wxAuiNotebook::SetPageSize (const wxSize& WXUNUSED(size))
{
    wxFAIL_MSG("Not implemented for wxAuiNotebook");
}

int wxAuiNotebook::HitTest (const wxPoint &pt, long *flags) const
{
    wxWindow *w = nullptr;
    long position = wxBK_HITTEST_NOWHERE;
    for ( const auto& pane : m_mgr.GetAllPanes() )
    {
        if ( IsDummyPane(pane) )
            continue;

        wxAuiTabFrame* tabframe = (wxAuiTabFrame*) pane.window;
        if (tabframe->m_tab_rect.Contains(pt))
        {
            wxPoint tabpos = tabframe->m_tabs->ScreenToClient(ClientToScreen(pt));
            w = tabframe->m_tabs->TabHitTest(tabpos).window;
            if (w)
                position = wxBK_HITTEST_ONITEM;
            break;
        }
        else if (tabframe->m_rect.Contains(pt))
        {
            w = tabframe->m_tabs->GetWindowFromIdx(tabframe->m_tabs->GetActivePage());
            if (w)
                position = wxBK_HITTEST_ONPAGE;
            break;
        }
    }

    if (flags)
        *flags = position;
    return w ? GetPageIndex(w) : wxNOT_FOUND;
}

int wxAuiNotebook::GetPageImage(size_t WXUNUSED(n)) const
{
    wxFAIL_MSG("Not implemented for wxAuiNotebook");
    return -1;
}

bool wxAuiNotebook::SetPageImage(size_t n, int imageId)
{
    return SetPageBitmap(n, GetBitmapBundle(imageId));
}

int wxAuiNotebook::ChangeSelection(size_t n)
{
    return DoModifySelection(n, false);
}

bool wxAuiNotebook::AddPage(wxWindow *page, const wxString &text, bool select,
                            int imageId)
{
    return AddPage(page, text, select, GetBitmapBundle(imageId));
}

bool wxAuiNotebook::DeleteAllPages()
{
    size_t count = GetPageCount();
    for(size_t i = 0; i < count; i++)
    {
        DeletePage(0);
    }
    return true;
}

bool wxAuiNotebook::InsertPage(size_t index, wxWindow *page,
                               const wxString &text, bool select,
                               int imageId)
{
    return InsertPage(index, page, text, select, GetBitmapBundle(imageId));
}

namespace
{

// Helper class to calculate the best size of a wxAuiNotebook
class wxAuiLayoutObject
{
public:
    enum
    {
        DockDir_Center,
        DockDir_Left,
        DockDir_Right,
        DockDir_Vertical,   // Merge elements from here vertically
        DockDir_Top,
        DockDir_Bottom,
        DockDir_None
    };

    wxAuiLayoutObject(const wxSize &size, const wxAuiPaneInfo &pInfo)
        : m_size(size)
    {
        m_pInfo = &pInfo;
        /*
            To speed up the sorting of the panes, the direction is mapped to a
            useful increasing value. This avoids complicated comparison of the
            enum values during the sort. The size calculation is done from the
            inner to the outermost direction. Therefore CENTER < LEFT/RIGHT <
            TOP/BOTTOM (It doesn't matter it LEFT or RIGHT is done first, as
            both extend the best size horizontally; the same applies for
            TOP/BOTTOM in vertical direction)
         */
        switch ( pInfo.dock_direction )
        {
            case wxAUI_DOCK_CENTER: m_dir = DockDir_Center; break;
            case wxAUI_DOCK_LEFT:   m_dir = DockDir_Left; break;
            case wxAUI_DOCK_RIGHT:  m_dir = DockDir_Right; break;
            case wxAUI_DOCK_TOP:    m_dir = DockDir_Top; break;
            case wxAUI_DOCK_BOTTOM: m_dir = DockDir_Bottom; break;
            default:                m_dir = DockDir_None;
        }
    }
    void MergeLayout(const wxAuiLayoutObject &lo2)
    {
        if ( this == &lo2 )
            return;

        bool mergeHorizontal;
        if ( m_pInfo->dock_layer != lo2.m_pInfo->dock_layer || m_dir != lo2.m_dir )
            mergeHorizontal = lo2.m_dir < DockDir_Vertical;
        else if ( m_pInfo->dock_row != lo2.m_pInfo->dock_row )
            mergeHorizontal = true;
        else
            mergeHorizontal = lo2.m_dir >= DockDir_Vertical;

        if ( mergeHorizontal )
        {
            m_size.x += lo2.m_size.x;
            if ( lo2.m_size.y > m_size.y )
                m_size.y = lo2.m_size.y;
        }
        else
        {
            if ( lo2.m_size.x > m_size.x )
                m_size.x = lo2.m_size.x;
            m_size.y += lo2.m_size.y;
        }
    }

    wxSize m_size;
    const wxAuiPaneInfo *m_pInfo;
    unsigned char m_dir;

    /*
        As the caulculation is done from the inner to the outermost pane, the
        panes are sorted in the following order: layer, direction, row,
        position.
     */
    bool operator<(const wxAuiLayoutObject& lo2) const
    {
        int diff = m_pInfo->dock_layer - lo2.m_pInfo->dock_layer;
        if ( diff )
            return diff < 0;
        diff = m_dir - lo2.m_dir;
        if ( diff )
            return diff < 0;
        diff = m_pInfo->dock_row - lo2.m_pInfo->dock_row;
        if ( diff )
            return diff < 0;
        return m_pInfo->dock_pos < lo2.m_pInfo->dock_pos;
    }
};

} // anonymous namespace

wxSize wxAuiNotebook::DoGetBestSize() const
{
    /*
        The best size of the wxAuiNotebook is a combination of all panes inside
        the object. To be able to efficiently  calculate the dimensions (i.e.
        without iterating over the panes multiple times) the panes need to be
        processed in a specific order. Therefore we need to collect them in the
        following variable which is sorted later on.
     */
    wxVector<wxAuiLayoutObject> layouts;
    const int tabHeight = GetTabCtrlHeight();
    for ( const auto& pane : m_mgr.GetAllPanes() )
    {
        if ( IsDummyPane(pane) || pane.IsFloating() )
            continue;

        const wxAuiTabFrame* tabframe = (wxAuiTabFrame*) pane.window;
        const wxAuiNotebookPageArray &pages = tabframe->m_tabs->GetPages();

        wxSize bestPageSize;
        for ( size_t pIdx = 0; pIdx < pages.GetCount(); pIdx++ )
            bestPageSize.IncTo(pages[pIdx].window->GetBestSize());

        bestPageSize.y += tabHeight;
        // Store the current pane with its largest window dimensions
        layouts.push_back(wxAuiLayoutObject(bestPageSize, pane));
    }

    if ( layouts.empty() )
        return wxSize(0, 0);

    wxVectorSort(layouts);

    /*
        The sizes of the panes are merged here. As the center pane is always at
        position 0 all sizes are merged there. As panes can be stacked using
        the dock_pos property, different positions are merged at the first
        (i.e. dock_pos = 0) element before being merged with the center pane.
     */
    size_t pos = 0;
    for ( size_t n = 1; n < layouts.size(); n++ )
    {
        if ( layouts[n].m_pInfo->dock_layer == layouts[pos].m_pInfo->dock_layer &&
             layouts[n].m_dir == layouts[pos].m_dir &&
             layouts[n].m_pInfo->dock_row == layouts[pos].m_pInfo->dock_row )
        {
            layouts[pos].MergeLayout(layouts[n]);
        }
        else
        {
            layouts[0].MergeLayout(layouts[pos]);
            pos = n;
        }
    }
    layouts[0].MergeLayout(layouts[pos]);

    return layouts[0].m_size;
}

int wxAuiNotebook::DoModifySelection(size_t n, bool events)
{
    wxWindow* wnd = m_tabs.GetWindowFromIdx(n);
    if (!wnd)
        return m_curPage;

    // don't change the page unless necessary;
    // however, clicking again on a tab should give it the focus.
    if ((int)n == m_curPage)
    {
        wxAuiTabCtrl* const ctrl = FindTab(wnd).tabCtrl;
        if ( ctrl && FindFocus() != ctrl )
        {
            ctrl->SetFocus();
        }
        return m_curPage;
    }

    bool vetoed = false;

    wxAuiNotebookEvent evt(wxEVT_AUINOTEBOOK_PAGE_CHANGING, m_windowId);

    if(events)
    {
        evt.SetSelection(n);
        evt.SetOldSelection(m_curPage);
        evt.SetEventObject(this);
        ProcessWindowEvent(evt);
        vetoed = !evt.IsAllowed();
    }

    if (!vetoed)
    {
        int old_curpage = m_curPage;
        m_curPage = n;

        if ( const auto tabInfo = FindTab(wnd) )
        {
            wxAuiTabCtrl* const ctrl = tabInfo.tabCtrl;

            ctrl->SetActivePage(tabInfo.tabIdx);
            DoSizing();
            ctrl->DoShowTab(tabInfo.tabIdx);

            // set fonts
            for ( const auto& pane : m_mgr.GetAllPanes() )
            {
                if ( IsDummyPane(pane) )
                    continue;
                wxAuiTabCtrl* tabctrl = ((wxAuiTabFrame*)pane.window)->m_tabs;
                if (tabctrl != ctrl)
                    tabctrl->SetSelectedFont(m_normalFont);
                else
                    tabctrl->SetSelectedFont(m_selectedFont);
                tabctrl->Refresh();
            }

            // Set the focus to the page if we're not currently focused on the tab.
            // This is Firefox-like behaviour.
            if (wnd->IsShownOnScreen() && FindFocus() != ctrl)
                wnd->SetFocus();

            // program allows the page change
            if(events)
            {
                evt.SetEventType(wxEVT_AUINOTEBOOK_PAGE_CHANGED);
                (void)ProcessWindowEvent(evt);
            }

            return old_curpage;
        }
    }

    return m_curPage;
}

void wxAuiTabCtrl::SetHoverTab(wxWindow* wnd)
{
    bool hoverChanged = false;

    for ( auto& page : m_pages )
    {
        bool oldHover = page.hover;
        page.hover = (page.window == wnd);
        if ( oldHover != page.hover )
            hoverChanged = true;
    }

    if ( hoverChanged )
    {
        Refresh();
        Update();
    }
}

// ----------------------------------------------------------------------------
// Layout serialization
// ----------------------------------------------------------------------------

void
wxAuiNotebook::SaveLayout(const wxString& name,
                          wxAuiBookSerializer& serializer) const
{
    serializer.BeforeSaveNotebook(name);

    for ( const auto& pane : m_mgr.GetAllPanes() )
    {
        if ( IsDummyPane(pane) )
            continue;

        wxAuiTabLayoutInfo tab;
        m_mgr.CopyDockLayoutFrom(tab, pane);

        const wxAuiTabCtrl* const
            tabCtrl = static_cast<wxAuiTabFrame*>(pane.window)->m_tabs;

        tab.active = tabCtrl->GetActivePage();

        // As an optimization, don't bother with saving the pages order for the
        // main control if it hasn't been changed from the default.
        bool mustSavePages = false;
        if ( tab.dock_direction != wxAUI_DOCK_CENTER )
        {
            // The non-main tab controls can't possibly have all the pages, so
            // we always have to save the indices of the ones they contain.
            mustSavePages = true;
        }
        else if ( tabCtrl->GetPageCount() != GetPageCount() )
        {
            // If the main control doesn't have all the pages, we need to save
            // them for it as well.
            mustSavePages = true;
        }

        std::vector<int> pages;
        int n = 0;
        for ( const auto& page : tabCtrl->GetPages() )
        {
            // This is inefficient for many pages, we should consider using a
            // hash map indexed by the window pointer if this proves to be a
            // problem in practice.
            const int idx = m_tabs.GetIdxFromWindow(page.window);
            if ( idx != n++ )
            {
                // And if the pages are not in order, we must save them too.
                mustSavePages = true;
            }

            pages.push_back(idx);

            // Also remember if this page is pinned.
            switch ( page.kind )
            {
                case wxAuiTabKind::Normal:
                    // Nothing special to do.
                    break;

                case wxAuiTabKind::Pinned:
                    tab.pinned.push_back(idx);
                    break;

                case wxAuiTabKind::Locked:
                    // We don't store locked pages because their status can't
                    // be changed by the user, so it would be pointless to save
                    // and restore them.
                    break;
            }
        }

        // But if none of the conditions above is true, we can avoid saving
        // them, which also allows us not to remove and re-add them when
        // restoring later, see LoadLayout() below.
        if ( mustSavePages )
            tab.pages = std::move(pages);

        serializer.SaveNotebookTabControl(tab);
    }

    serializer.AfterSaveNotebook();
}

void
wxAuiNotebook::LoadLayout(const wxString& name,
                          wxAuiBookDeserializer& deserializer)
{
    const auto tabs = deserializer.LoadNotebookTabs(name);

    if ( tabs.empty() )
        return;

    // Remove any existing tabs before adding new ones.
    UnsplitAll();

    // Get the only remaining tab control.
    wxAuiTabCtrl* const tabMain = GetMainTabCtrl();

    // If we don't have anything saved and the pages are in the default order
    // we may not have to do anything at all.
    bool useExistingPages = false;

    // Reset the state of all pinned pages to normal, as this state will be
    // restored from the deserialized data below.
    //
    // Note that we do not do this for the locked pages, as their state can't
    // be changed by the user and so it's not necessarily to save nor restore
    // it.
    for ( auto& page : m_tabs.GetPages() )
    {
        if ( page.kind == wxAuiTabKind::Pinned )
            const_cast<wxAuiNotebookPage&>(page).kind = wxAuiTabKind::Normal;
    }

    // This set will contain all the pages that should be pinned.
    std::unordered_set<int> pinned;

    // Remember the active page in the main tab control if we change it.
    const wxWindow* activeInMainTab = nullptr;

    // Keep track of pages we've already added to some tab control: even if the
    // deserialized data is somehow incorrect and duplicates the page indices,
    // we don't want to try to have the same page in more than one tab control.
    std::unordered_set<int> addedPages;
    const int pageCount = m_tabs.GetPageCount();

    for ( const auto& tab : tabs )
    {
        wxAuiTabCtrl* tabCtrl = nullptr;

        // We'll deal with the pinned pages outside of this loop below, after
        // all pages are in their correct places, for now just remember them.
        pinned.insert(tab.pinned.begin(), tab.pinned.end());

        // The pages pointer will be set to point to either tab.pages or
        // pageDefault in which case it will also be filled.
        std::vector<int> pagesDefault;
        const std::vector<int>* pages = nullptr;

        // We don't need to create a new pane for the main, central tab control,
        // but we still need to order its tabs correctly.
        if ( tab.dock_direction == wxAUI_DOCK_CENTER )
        {
            // Special case: if the pages vector is empty check if we're using
            // the default order, in which case we can avoid doing anything if
            // the pages are already in the same order (which is a common case).
            if ( tab.pages.empty() )
            {
                useExistingPages = true;

                for ( int i = 0; i < pageCount; ++i )
                {
                    if ( tabMain->GetWindowFromIdx(i) != m_tabs.GetWindowFromIdx(i) )
                    {
                        useExistingPages = false;
                        break;
                    }
                }

                if ( useExistingPages )
                {
                    // All pages are in the main tab in the default order
                    // already, so we don't have anything to do except
                    // restoring the active page -- which is in this case the
                    // same as selection (tab indices == notebook indices).
                    SetSelection(tab.active);
                    break;
                }

                // The pages had been reordered, restore the default order when
                // re-adding them below.
                for ( int i = 0; i < pageCount; ++i )
                    pagesDefault.push_back(i);

                pages = &pagesDefault;
            }
            else
            {
                // Just use the specified pages.
                pages = &tab.pages;
            }

            // In any case, we must remove all pages currently in this tab as
            // they will be re-added.
            tabMain->RemoveAll();

            tabCtrl = tabMain;
        }
        else // Non-central tab control.
        {
            // In this case, we must have some pages, but if we somehow don't,
            // just skip adding the tab control entirely.
            if ( tab.pages.empty() )
                continue;

            // Re-add using the saved layout.
            wxAuiPaneInfo pane;
            m_mgr.CopyDockLayoutTo(tab, pane);

            // Our panes never show caption.
            pane.CaptionVisible(false);

            wxAuiTabFrame* tabframe = CreateTabFrame();
            m_mgr.AddPane(tabframe, pane);

            tabCtrl = tabframe->m_tabs;
            pages = &tab.pages;
        }

        // In any case, add the pages that this tab control had before to it.
        //
        // There is one extra complication: we must ensure that any locked
        // pages still come first, even if the deserializer positioned them
        // wrongly, because this is an invariant of wxAuiNotebook and has to be
        // respected. So instead of adding the pages directly, collect them in
        // this vector first.
        std::vector<int> pagesToAdd;
        pagesToAdd.reserve(pages->size());

        int lockedCount = 0;
        for ( auto page : *pages )
        {
            // We just silently ignore invalid or duplicate page indices
            // here, because it doesn't seem right for them to result in a
            // fatal error and any warnings would be just annoying.
            if ( page >= pageCount )
                continue;

            if ( !addedPages.insert(page).second )
                continue;

            if ( m_tabs.GetPage(page).kind == wxAuiTabKind::Locked )
            {
                // Locked pages must always come first, so insert them at the
                // beginning (but keep their original order).
                pagesToAdd.insert(pagesToAdd.begin() + lockedCount++, page);
            }
            else
            {
                pagesToAdd.push_back(page);
            }
        }

        // In the degenerate case when all pages are invalid, pagesToAdd may be
        // empty here, but we can still execute the rest of the code below, it
        // just won't do anything.

        const wxWindow* activeWindow = nullptr;
        for ( auto page : pagesToAdd )
        {
            const auto& info = m_tabs.GetPage(page);

            if ( page == tab.active )
                activeWindow = info.window;

            tabCtrl->AddPage(info);
        }

        if ( !activeWindow )
        {
            // We must have some active page, so make the first one active if
            // the deserialized data didn't define a valid active page.
            activeWindow = tabCtrl->GetWindowFromIdx(0);
        }

        tabCtrl->SetActivePage(activeWindow);

        if ( tabCtrl == tabMain )
        {
            // Remember it to set the selection to it below.
            activeInMainTab = activeWindow;
        }
    }

    // Check if there were any existing pages not added to any tab control.
    if ( !useExistingPages && wxSsize(addedPages) < pageCount )
    {
        // Use a stack here to remove the pages from the end below.
        std::stack<int> toRemove;

        for ( int i = 0; i < pageCount; ++i )
        {
            if ( addedPages.count(i) == 0 )
            {
                wxAuiTabCtrl* tabCtrl = tabMain;
                int tabIndex = wxNOT_FOUND;
                if ( deserializer.HandleOrphanedPage(*this, i,
                                                     &tabCtrl, &tabIndex) )
                {
                    // Try not to crash even if the deserializer implements
                    // HandleOrphanedPage() incorrectly.
                    if ( !tabCtrl )
                    {
                        wxFAIL_MSG
                        (
                            "HandleOrphanedPage() can't return null tab control"
                        );

                        tabCtrl = tabMain;
                    }

                    const int tabCount = tabCtrl->GetPageCount();
                    if ( tabIndex != wxNOT_FOUND && tabIndex >= tabCount )
                    {
                        wxFAIL_MSG
                        (
                            "HandleOrphanedPage() must return valid tab index"
                        );

                        tabIndex = wxNOT_FOUND;
                    }

                    if ( tabIndex == wxNOT_FOUND )
                        tabIndex = tabCount;

                    tabCtrl->InsertPage(m_tabs.GetPage(i), tabIndex);
                }
                else // Remove this page.
                {
                    toRemove.push(i);
                }
            }
        }

        while ( !toRemove.empty() )
        {
            // Note that we shouldn't call DoRemovePage() because it supposes
            // that the page is in some tab control and does nothing if this is
            // not the case.
            m_tabs.RemovePageAt(toRemove.top());

            toRemove.pop();
        }

        RemoveEmptyTabFrames();
    }

    // Now deal with the pinned pages: we must ensure that their positions are
    // consistent, i.e. there are no non-pinned pages before a pinned one in
    // any tab control.
    if ( !pinned.empty() )
    {
        for ( auto tabCtrl : GetAllTabCtrls() )
        {
            bool stop = false;
            for ( auto n : GetPagesInDisplayOrder(tabCtrl) )
            {
                auto& page = m_tabs.GetPage(n);
                switch ( page.kind )
                {
                    case wxAuiTabKind::Normal:
                        if ( pinned.count(n) )
                        {
                            // Make this page pinned. We don't need to call
                            // SetPageKind() for this, as it's already in the
                            // right place, just update its kind directly.
                            page.kind = wxAuiTabKind::Pinned;

                            int idx = tabCtrl->GetIdxFromWindow(page.window);
                            if ( idx == wxNOT_FOUND )
                            {
                                // This would be a logic error in this code.
                                wxFAIL_MSG("page not found in tab control");
                                break;
                            }

                            tabCtrl->GetPage(idx).kind = wxAuiTabKind::Pinned;
                        }
                        else
                        {
                            // There can be no pinned pages after the first
                            // normal one, so there is no point in continuing:
                            // even if any other pages were marked as pinned in
                            // the deserialized data, we would just ignore them
                            // anyhow.
                            stop = true;
                        }
                        break;

                    case wxAuiTabKind::Pinned:
                        // This would indicate a logic error in this function,
                        // as we unpinned all the pages at the start of it.
                        wxFAIL_MSG("all pages should be unpinned");
                        break;

                    case wxAuiTabKind::Locked:
                        // We can't change the kind of a locked page, so either
                        // it is before the pinned pages or data is invalid,
                        // leave it as is in any case.
                        break;
                }

                if ( stop )
                    break;
            }
        }
    }

    // Update the active pages visibility in all tab controls after adding and
    // removing all the pages.
    for ( auto tabCtrl : GetAllTabCtrls() )
    {
        tabCtrl->DoUpdateActive();
    }

    // We don't save information about the currently focused tab control, so
    // always make the main one active after loading the layout by setting the
    // selected page to the page active in it (if there is no such page, it
    // means we're reusing the existing pages and so don't need to do anything).
    if ( activeInMainTab )
        m_curPage = m_tabs.GetIdxFromWindow(activeInMainTab);

    m_mgr.Update();
}

#endif // wxUSE_AUI
