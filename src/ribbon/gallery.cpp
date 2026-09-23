///////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/gallery.cpp
// Purpose:     Ribbon control which displays a gallery of items to choose from
// Author:      Peter Cawley
// Created:     2009-07-22
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"


#if wxUSE_RIBBON

#include "wx/ribbon/gallery.h"
#include "wx/ribbon/art.h"
#include "wx/ribbon/bar.h"
#include "wx/dcbuffer.h"
#include "wx/renderer.h"
#include "wx/clntdata.h"

#ifndef WX_PRECOMP
#endif

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

wxDEFINE_EVENT(wxEVT_RIBBONGALLERY_HOVER_CHANGED, wxRibbonGalleryEvent);
wxDEFINE_EVENT(wxEVT_RIBBONGALLERY_SELECTED, wxRibbonGalleryEvent);
wxDEFINE_EVENT(wxEVT_RIBBONGALLERY_CLICKED, wxRibbonGalleryEvent);

wxIMPLEMENT_DYNAMIC_CLASS(wxRibbonGalleryEvent, wxCommandEvent);
wxIMPLEMENT_CLASS(wxRibbonGallery, wxRibbonControl);

class wxRibbonGalleryItem
{
public:
    wxRibbonGalleryItem() = default;

    void SetId(int id) {m_id = id;}
    void SetBitmap(const wxBitmapBundle& bitmap) {m_bitmap = bitmap;}
    const wxBitmapBundle& GetBitmapBundle() const {return m_bitmap;}
    wxBitmap GetBitmap(wxWindow* wnd) const {return m_bitmap.GetBitmapFor(wnd);}
    void SetIsVisible(bool visible) {m_is_visible = visible;}
    void SetPosition(int x, int y, const wxSize& size)
    {
        m_position = wxRect(wxPoint(x, y), size);
    }
    bool IsVisible() const {return m_is_visible;}
    const wxRect& GetPosition() const {return m_position;}

    void SetClientObject(wxClientData *data) {m_client_data.SetClientObject(data);}
    wxClientData *GetClientObject() const {return m_client_data.GetClientObject();}
    void SetClientData(void *data) {m_client_data.SetClientData(data);}
    void *GetClientData() const {return m_client_data.GetClientData();}

protected:
    wxBitmapBundle m_bitmap;
    wxClientDataContainer m_client_data;
    wxRect m_position;
    int m_id = 0;
    bool m_is_visible = false;
};

wxBEGIN_EVENT_TABLE(wxRibbonGallery, wxRibbonControl)
    EVT_ENTER_WINDOW(wxRibbonGallery::OnMouseEnter)
    EVT_ERASE_BACKGROUND(wxRibbonGallery::OnEraseBackground)
    EVT_LEAVE_WINDOW(wxRibbonGallery::OnMouseLeave)
    EVT_LEFT_DOWN(wxRibbonGallery::OnMouseDown)
    EVT_LEFT_UP(wxRibbonGallery::OnMouseUp)
    EVT_LEFT_DCLICK(wxRibbonGallery::OnMouseDClick)
    EVT_MOTION(wxRibbonGallery::OnMouseMove)
    EVT_PAINT(wxRibbonGallery::OnPaint)
    EVT_SIZE(wxRibbonGallery::OnSize)
    EVT_DPI_CHANGED(wxRibbonGallery::OnDPIChanged)
    EVT_SYS_COLOUR_CHANGED(wxRibbonGallery::OnSysColourChanged)
wxEND_EVENT_TABLE()

wxRibbonGallery::wxRibbonGallery()
{
}

wxRibbonGallery::wxRibbonGallery(wxWindow* parent,
                  wxWindowID id,
                  const wxPoint& pos,
                  const wxSize& size,
                  long style)
    : wxRibbonControl(parent, id, pos, size, wxBORDER_NONE)
{
    CommonInit(style);
}

wxRibbonGallery::~wxRibbonGallery()
{
    Clear();
}

bool wxRibbonGallery::Create(wxWindow* parent,
                wxWindowID id,
                const wxPoint& pos,
                const wxSize& size,
                long style)
{
    if(!wxRibbonControl::Create(parent, id, pos, size, wxBORDER_NONE))
    {
        return false;
    }

    CommonInit(style);
    return true;
}

void wxRibbonGallery::CommonInit(long WXUNUSED(style))
{
    m_selected_item = nullptr;
    m_hovered_item = nullptr;
    m_active_item = nullptr;
    m_scroll_up_button_rect = wxRect(0, 0, 0, 0);
    m_scroll_down_button_rect = wxRect(0, 0, 0, 0);
    m_extension_button_rect = wxRect(0, 0, 0, 0);
    m_mouse_active_rect = nullptr;
    m_bitmap_size = wxSize(64, 32);
    m_bitmap_padded_size = m_bitmap_size;
    m_item_separation_x = 0;
    m_item_separation_y = 0;
    m_scroll_amount = 0;
    m_scroll_limit = 0;
    m_up_button_state = wxRIBBON_GALLERY_BUTTON_DISABLED;
    m_down_button_state = wxRIBBON_GALLERY_BUTTON_NORMAL;
    m_extension_button_state = wxRIBBON_GALLERY_BUTTON_NORMAL;
    m_hovered = false;

    SetBackgroundStyle(wxBG_STYLE_PAINT);
}

void wxRibbonGallery::OnMouseEnter(wxMouseEvent& evt)
{
    m_hovered = true;
    if(m_mouse_active_rect != nullptr && !evt.LeftIsDown())
    {
        m_mouse_active_rect = nullptr;
        m_active_item = nullptr;
    }
    Refresh(false);
}

void wxRibbonGallery::OnMouseMove(wxMouseEvent& evt)
{
    bool refresh = false;
    wxPoint pos = evt.GetPosition();

    if(TestButtonHover(m_scroll_up_button_rect, pos, &m_up_button_state))
        refresh = true;
    if(TestButtonHover(m_scroll_down_button_rect, pos, &m_down_button_state))
        refresh = true;
    if(TestButtonHover(m_extension_button_rect, pos, &m_extension_button_state))
        refresh = true;

    wxRibbonGalleryItem *hovered_item = nullptr;
    wxRibbonGalleryItem *active_item = nullptr;
    if(m_client_rect.Contains(pos))
    {
        if(m_art && m_art->GetFlags() & wxRIBBON_BAR_FLOW_VERTICAL)
            pos.x += m_scroll_amount;
        else
            pos.y += m_scroll_amount;

        size_t item_count = m_items.Count();
        size_t item_i;
        for(item_i = 0; item_i < item_count; ++item_i)
        {
            wxRibbonGalleryItem *item = m_items.Item(item_i);
            if(!item->IsVisible())
                continue;

            if(item->GetPosition().Contains(pos))
            {
                if(m_mouse_active_rect == &item->GetPosition())
                    active_item = item;
                hovered_item = item;
                break;
            }
        }
    }
    if(active_item != m_active_item)
    {
        m_active_item = active_item;
        refresh = true;
    }
    if(hovered_item != m_hovered_item)
    {
        m_hovered_item = hovered_item;
        wxRibbonGalleryEvent notification(
            wxEVT_RIBBONGALLERY_HOVER_CHANGED, GetId());
        notification.SetEventObject(this);
        notification.SetGallery(this);
        notification.SetGalleryItem(hovered_item);
        ProcessWindowEvent(notification);
        refresh = true;
    }

    if(refresh)
        Refresh(false);
}

bool wxRibbonGallery::TestButtonHover(const wxRect& rect, wxPoint pos,
        wxRibbonGalleryButtonState* state)
{
    if(*state == wxRIBBON_GALLERY_BUTTON_DISABLED)
        return false;

    wxRibbonGalleryButtonState new_state;
    if(rect.Contains(pos))
    {
        if(m_mouse_active_rect == &rect)
            new_state = wxRIBBON_GALLERY_BUTTON_ACTIVE;
        else
            new_state = wxRIBBON_GALLERY_BUTTON_HOVERED;
    }
    else
        new_state = wxRIBBON_GALLERY_BUTTON_NORMAL;

    if(new_state != *state)
    {
        *state = new_state;
        return true;
    }
    else
    {
        return false;
    }
}

void wxRibbonGallery::OnMouseLeave(wxMouseEvent& WXUNUSED(evt))
{
    m_hovered = false;
    m_active_item = nullptr;
    if(m_up_button_state != wxRIBBON_GALLERY_BUTTON_DISABLED)
        m_up_button_state = wxRIBBON_GALLERY_BUTTON_NORMAL;
    if(m_down_button_state != wxRIBBON_GALLERY_BUTTON_DISABLED)
        m_down_button_state = wxRIBBON_GALLERY_BUTTON_NORMAL;
    if(m_extension_button_state != wxRIBBON_GALLERY_BUTTON_DISABLED)
        m_extension_button_state = wxRIBBON_GALLERY_BUTTON_NORMAL;
    if(m_hovered_item != nullptr)
    {
        m_hovered_item = nullptr;
        wxRibbonGalleryEvent notification(
            wxEVT_RIBBONGALLERY_HOVER_CHANGED, GetId());
        notification.SetEventObject(this);
        notification.SetGallery(this);
        ProcessWindowEvent(notification);
    }
    Refresh(false);
}

void wxRibbonGallery::OnMouseDown(wxMouseEvent& evt)
{
    DismissKeyTips();

    wxPoint pos = evt.GetPosition();
    m_mouse_active_rect = nullptr;
    if(m_client_rect.Contains(pos))
    {
        if(m_art && m_art->GetFlags() & wxRIBBON_BAR_FLOW_VERTICAL)
            pos.x += m_scroll_amount;
        else
            pos.y += m_scroll_amount;
        size_t item_count = m_items.Count();
        size_t item_i;
        for(item_i = 0; item_i < item_count; ++item_i)
        {
            wxRibbonGalleryItem *item = m_items.Item(item_i);
            if(!item->IsVisible())
                continue;

            const wxRect& rect = item->GetPosition();
            if(rect.Contains(pos))
            {
                m_active_item = item;
                m_mouse_active_rect = &rect;
                break;
            }
        }
    }
    else if(m_scroll_up_button_rect.Contains(pos))
    {
        if(m_up_button_state != wxRIBBON_GALLERY_BUTTON_DISABLED)
        {
            m_mouse_active_rect = &m_scroll_up_button_rect;
            m_up_button_state = wxRIBBON_GALLERY_BUTTON_ACTIVE;
        }
    }
    else if(m_scroll_down_button_rect.Contains(pos))
    {
        if(m_down_button_state != wxRIBBON_GALLERY_BUTTON_DISABLED)
        {
            m_mouse_active_rect = &m_scroll_down_button_rect;
            m_down_button_state = wxRIBBON_GALLERY_BUTTON_ACTIVE;
        }
    }
    else if(m_extension_button_rect.Contains(pos))
    {
        if(m_extension_button_state != wxRIBBON_GALLERY_BUTTON_DISABLED)
        {
            m_mouse_active_rect = &m_extension_button_rect;
            m_extension_button_state = wxRIBBON_GALLERY_BUTTON_ACTIVE;
        }
    }
    if(m_mouse_active_rect != nullptr)
        Refresh(false);
}

void wxRibbonGallery::OnMouseUp(wxMouseEvent& evt)
{
    if(m_mouse_active_rect != nullptr)
    {
        wxPoint pos = evt.GetPosition();
        if(m_active_item)
        {
            if(m_art && m_art->GetFlags() & wxRIBBON_BAR_FLOW_VERTICAL)
                pos.x += m_scroll_amount;
            else
                pos.y += m_scroll_amount;
        }
        if(m_mouse_active_rect->Contains(pos))
        {
            if(m_mouse_active_rect == &m_scroll_up_button_rect)
            {
                m_up_button_state = wxRIBBON_GALLERY_BUTTON_HOVERED;
                ScrollLines(-1);
            }
            else if(m_mouse_active_rect == &m_scroll_down_button_rect)
            {
                m_down_button_state = wxRIBBON_GALLERY_BUTTON_HOVERED;
                ScrollLines(1);
            }
            else if(m_mouse_active_rect == &m_extension_button_rect)
            {
                m_extension_button_state = wxRIBBON_GALLERY_BUTTON_HOVERED;
                DoActivateExtensionButton();
            }
            else if(m_active_item != nullptr)
            {
                DoActivateItem(m_active_item);
            }
        }
        m_mouse_active_rect = nullptr;
        m_active_item = nullptr;
        Refresh(false);
    }
}

void wxRibbonGallery::DoActivateItem(wxRibbonGalleryItem* item)
{
    if(m_selected_item != item)
    {
        m_selected_item = item;
        wxRibbonGalleryEvent notification(
            wxEVT_RIBBONGALLERY_SELECTED, GetId());
        notification.SetEventObject(this);
        notification.SetGallery(this);
        notification.SetGalleryItem(m_selected_item);
        ProcessWindowEvent(notification);
    }

    wxRibbonGalleryEvent notification(
        wxEVT_RIBBONGALLERY_CLICKED, GetId());
    notification.SetEventObject(this);
    notification.SetGallery(this);
    notification.SetGalleryItem(m_selected_item);
    ProcessWindowEvent(notification);
}

void wxRibbonGallery::DoActivateExtensionButton()
{
    wxCommandEvent notification(wxEVT_BUTTON,
        GetId());
    notification.SetEventObject(this);
    ProcessWindowEvent(notification);
}

void wxRibbonGallery::OnMouseDClick(wxMouseEvent& evt)
{
    // The 2nd click of a double-click should be handled as a click in the
    // same way as the 1st click of the double-click. This is useful for
    // scrolling through the gallery.
    OnMouseDown(evt);
    OnMouseUp(evt);
}

bool wxRibbonGallery::DoFocusItemFrom(int pos, int step)
{
    const int count = static_cast<int>(m_items.Count());
    for ( int i = pos; i >= 0 && i < count; i += step )
    {
        wxRibbonGalleryItem* item = m_items.Item(i);
        if ( !item->IsVisible() )
            continue;

        DoClearExtensionFocus();
        m_focused_item = item;

        // Scroll the item into view, if needed.
        if ( m_art != nullptr )
        {
            const wxRect& rect = item->GetPosition();
            if ( m_art->GetFlags() & wxRIBBON_BAR_FLOW_VERTICAL )
            {
                const int left = rect.GetLeft() - m_scroll_amount;
                const int right = rect.GetRight() - m_scroll_amount;
                if ( left < m_client_rect.GetLeft() )
                    ScrollPixels(left - m_client_rect.GetLeft());
                else if ( right > m_client_rect.GetRight() )
                    ScrollPixels(right - m_client_rect.GetRight());
            }
            else
            {
                const int top = rect.GetTop() - m_scroll_amount;
                const int bottom = rect.GetBottom() - m_scroll_amount;
                if ( top < m_client_rect.GetTop() )
                    ScrollPixels(top - m_client_rect.GetTop());
                else if ( bottom > m_client_rect.GetBottom() )
                    ScrollPixels(bottom - m_client_rect.GetBottom());
            }
        }

        Refresh(false);
        return true;
    }
    return false;
}

int wxRibbonGallery::DoGetFocusedItemIndex() const
{
    if ( m_focused_item == nullptr )
        return wxNOT_FOUND;

    const int count = static_cast<int>(m_items.Count());
    for ( int i = 0; i < count; ++i )
    {
        if ( m_items.Item(i) == m_focused_item )
            return i;
    }
    return wxNOT_FOUND;
}

bool wxRibbonGallery::DoFocusExtensionButton()
{
    if ( m_extension_button_rect.IsEmpty() ||
         m_extension_button_state == wxRIBBON_GALLERY_BUTTON_DISABLED )
        return false;

    m_focused_item = nullptr;
    m_extension_focused = true;
    // The art providers draw the button from its state, so show it as hovered.
    m_extension_button_state = wxRIBBON_GALLERY_BUTTON_HOVERED;
    Refresh(false);
    return true;
}

void wxRibbonGallery::DoClearExtensionFocus()
{
    if ( m_extension_focused )
    {
        m_extension_focused = false;
        if ( m_extension_button_state == wxRIBBON_GALLERY_BUTTON_HOVERED )
            m_extension_button_state = wxRIBBON_GALLERY_BUTTON_NORMAL;
    }
}

bool wxRibbonGallery::HasFocusableItems() const
{
    if ( !m_extension_button_rect.IsEmpty() &&
         m_extension_button_state != wxRIBBON_GALLERY_BUTTON_DISABLED )
        return true;

    for ( size_t i = 0; i < m_items.Count(); ++i )
    {
        if ( m_items.Item(i)->IsVisible() )
            return true;
    }
    return false;
}

bool wxRibbonGallery::FocusFirstItem()
{
    return DoFocusItemFrom(0, 1) || DoFocusExtensionButton();
}

bool wxRibbonGallery::FocusLastItem()
{
    return DoFocusExtensionButton() ||
            DoFocusItemFrom(static_cast<int>(m_items.Count()) - 1, -1);
}

bool wxRibbonGallery::FocusNextItem(bool forward)
{
    const int lastItem = static_cast<int>(m_items.Count()) - 1;

    if ( m_extension_focused )
        return !forward && DoFocusItemFrom(lastItem, -1);

    const int current = DoGetFocusedItemIndex();
    if ( current == wxNOT_FOUND )
        return forward ? FocusFirstItem() : FocusLastItem();

    const int step = forward ? 1 : -1;
    if ( DoFocusItemFrom(current + step, step) )
        return true;

    // The extension button comes after the last item.
    return forward && DoFocusExtensionButton();
}

bool wxRibbonGallery::FocusItemInDirection(wxDirection direction)
{
    if ( direction != wxUP && direction != wxDOWN )
        return false;

    if ( m_extension_focused )
        return direction == wxUP &&
                DoFocusItemFrom(static_cast<int>(m_items.Count()) - 1, -1);

    const int current = DoGetFocusedItemIndex();
    if ( current == wxNOT_FOUND )
        return false;

    // Find the closest item in the row above or below, in the same column if
    // there is one there.
    const wxRect& from = m_items.Item(current)->GetPosition();
    const int row_y = from.GetTop() +
        (direction == wxUP ? -m_bitmap_padded_size.GetHeight()
                           : m_bitmap_padded_size.GetHeight());

    int best{ wxNOT_FOUND };
    int best_distance{ 0 };
    for ( int i = 0; i < static_cast<int>(m_items.Count()); ++i )
    {
        const wxRibbonGalleryItem* item = m_items.Item(i);
        if ( !item->IsVisible() || item->GetPosition().GetTop() != row_y )
            continue;

        int distance = item->GetPosition().GetLeft() - from.GetLeft();
        if ( distance < 0 )
            distance = -distance;
        if ( best == wxNOT_FOUND || distance < best_distance )
        {
            best = i;
            best_distance = distance;
        }
    }

    return best != wxNOT_FOUND && DoFocusItemFrom(best, 1);
}

void wxRibbonGallery::ClearFocusedItem()
{
    if ( m_focused_item != nullptr || m_extension_focused )
    {
        m_focused_item = nullptr;
        DoClearExtensionFocus();
        Refresh(false);
    }
}

void wxRibbonGallery::ActivateFocusedItem(bool dropdown)
{
    // There is no dropdown, so there is nothing to open.
    if ( dropdown )
        return;

    if ( m_extension_focused )
        DoActivateExtensionButton();
    else if ( m_focused_item != nullptr )
        DoActivateItem(m_focused_item);
    else
        return;

    Refresh(false);
}

void wxRibbonGallery::SetItemClientObject(wxRibbonGalleryItem* itm,
                                          wxClientData* data)
{
    itm->SetClientObject(data);
}

wxClientData* wxRibbonGallery::GetItemClientObject(const wxRibbonGalleryItem* itm) const
{
    return itm->GetClientObject();
}

void wxRibbonGallery::SetItemClientData(wxRibbonGalleryItem* itm, void* data)
{
    itm->SetClientData(data);
}

void* wxRibbonGallery::GetItemClientData(const wxRibbonGalleryItem* itm) const
{
    return itm->GetClientData();
}

bool wxRibbonGallery::ScrollLines(int lines)
{
    if(m_scroll_limit == 0 || m_art == nullptr)
        return false;

    return ScrollPixels(lines * GetScrollLineSize());
}

int wxRibbonGallery::GetScrollLineSize() const
{
    if(m_art == nullptr)
        return 32;

    int line_size = m_bitmap_padded_size.GetHeight();
    if(m_art->GetFlags() & wxRIBBON_BAR_FLOW_VERTICAL)
        line_size = m_bitmap_padded_size.GetWidth();

    return line_size;
}

bool wxRibbonGallery::ScrollPixels(int pixels)
{
    if(m_scroll_limit == 0 || m_art == nullptr)
        return false;

    if(pixels < 0)
    {
        if(m_scroll_amount > 0)
        {
            m_scroll_amount += pixels;
            if(m_scroll_amount <= 0)
            {
                m_scroll_amount = 0;
                m_up_button_state = wxRIBBON_GALLERY_BUTTON_DISABLED;
            }
            else if(m_up_button_state == wxRIBBON_GALLERY_BUTTON_DISABLED)
                m_up_button_state = wxRIBBON_GALLERY_BUTTON_NORMAL;
            if(m_down_button_state == wxRIBBON_GALLERY_BUTTON_DISABLED)
                m_down_button_state = wxRIBBON_GALLERY_BUTTON_NORMAL;
            return true;
        }
    }
    else if(pixels > 0)
    {
        if(m_scroll_amount < m_scroll_limit)
        {
            m_scroll_amount += pixels;
            if(m_scroll_amount >= m_scroll_limit)
            {
                m_scroll_amount = m_scroll_limit;
                m_down_button_state = wxRIBBON_GALLERY_BUTTON_DISABLED;
            }
            else if(m_down_button_state == wxRIBBON_GALLERY_BUTTON_DISABLED)
                m_down_button_state = wxRIBBON_GALLERY_BUTTON_NORMAL;
            if(m_up_button_state == wxRIBBON_GALLERY_BUTTON_DISABLED)
                m_up_button_state = wxRIBBON_GALLERY_BUTTON_NORMAL;
            return true;
        }
    }
    return false;
}

void wxRibbonGallery::EnsureVisible(const wxRibbonGalleryItem* item)
{
    if(item == nullptr || !item->IsVisible() || IsEmpty() || m_art == nullptr )
        return;

    if(m_art->GetFlags() & wxRIBBON_BAR_FLOW_VERTICAL)
    {
        int x = item->GetPosition().GetLeft();
        int base_x = m_items.Item(0)->GetPosition().GetLeft();
        int delta = x - base_x - m_scroll_amount;
        ScrollLines(delta / m_bitmap_padded_size.GetWidth());
    }
    else
    {
        int y = item->GetPosition().GetTop();
        int base_y = m_items.Item(0)->GetPosition().GetTop();
        int delta = y - base_y - m_scroll_amount;
        ScrollLines(delta / m_bitmap_padded_size.GetHeight());
    }
}

bool wxRibbonGallery::IsHovered() const
{
    return m_hovered;
}

void wxRibbonGallery::OnEraseBackground(wxEraseEvent& WXUNUSED(evt))
{
    // All painting done in main paint handler to minimise flicker
}

void wxRibbonGallery::OnPaint(wxPaintEvent& WXUNUSED(evt))
{
    wxAutoBufferedPaintDC dc(this);
    if(m_art == nullptr)
        return;

    m_art->DrawGalleryBackground(dc, this, GetSize());

    int padding_top = m_art->GetMetric(wxRIBBON_ART_GALLERY_BITMAP_PADDING_TOP_SIZE);
    int padding_left = m_art->GetMetric(wxRIBBON_ART_GALLERY_BITMAP_PADDING_LEFT_SIZE);

    dc.SetClippingRegion(m_client_rect);

    bool offset_vertical = true;
    if(m_art->GetFlags() & wxRIBBON_BAR_FLOW_VERTICAL)
        offset_vertical = false;
    size_t item_count = m_items.Count();
    size_t item_i;
    for(item_i = 0; item_i < item_count; ++item_i)
    {
        wxRibbonGalleryItem *item = m_items.Item(item_i);
        if(!item->IsVisible())
            continue;

        const wxRect& pos = item->GetPosition();
        wxRect offset_pos(pos);
        if(offset_vertical)
            offset_pos.SetTop(offset_pos.GetTop() - m_scroll_amount);
        else
            offset_pos.SetLeft(offset_pos.GetLeft() - m_scroll_amount);
        m_art->DrawGalleryItemBackground(dc, this, offset_pos, item);
        // Resolve bitmap bundle for current DPI
        wxBitmap bmp = item->GetBitmap(this);
        if (bmp.IsOk())
            dc.DrawBitmap(bmp, offset_pos.GetLeft() + padding_left,
                offset_pos.GetTop() + padding_top);

        if (item == m_focused_item)
        {
            offset_pos.Deflate(FromDIP(2));
            wxRendererNative::Get().DrawFocusRect(this, dc, offset_pos);
        }
    }

    dc.DestroyClippingRegion();

    if(m_extension_focused)
    {
        wxRect ext_rect{ m_extension_button_rect };
        ext_rect.Deflate(FromDIP(1));
        wxRendererNative::Get().DrawFocusRect(this, dc, ext_rect);
    }

    wxRibbonBar* bar = GetAncestorRibbonBar();
    if ( bar != nullptr )
        bar->DrawKeyTipsFor(dc, this, m_art);
}

void wxRibbonGallery::OnSize(wxSizeEvent& WXUNUSED(evt))
{
    Layout();
}

void wxRibbonGallery::OnDPIChanged(wxDPIChangedEvent& event)
{
    Realize();
    event.Skip();
}

void wxRibbonGallery::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    event.Skip();
    if ( m_art )
        m_art->UpdateColoursFromSystem();
}

wxRibbonGalleryItem* wxRibbonGallery::Append(const wxBitmapBundle& bitmap, int id)
{
    wxASSERT(bitmap.IsOk());
    if(m_items.IsEmpty())
    {
        m_bitmap_size = bitmap.GetDefaultSize();
        CalculateMinSize();
    }
    else
    {
        wxASSERT(bitmap.GetDefaultSize() == m_bitmap_size);
    }

    wxRibbonGalleryItem *item = new wxRibbonGalleryItem;
    item->SetId(id);
    item->SetBitmap(bitmap);
    m_items.Add(item);
    return item;
}

wxRibbonGalleryItem* wxRibbonGallery::Append(const wxBitmapBundle& bitmap, int id,
                                             void* clientData)
{
    wxRibbonGalleryItem *item = Append(bitmap, id);
    item->SetClientData(clientData);
    return item;
}

wxRibbonGalleryItem* wxRibbonGallery::Append(const wxBitmapBundle& bitmap, int id,
                                             wxClientData* clientData)
{
    wxRibbonGalleryItem *item = Append(bitmap, id);
    item->SetClientObject(clientData);
    return item;
}

void wxRibbonGallery::Clear()
{
    size_t item_count = m_items.Count();
    size_t item_i;
    for(item_i = 0; item_i < item_count; ++item_i)
    {
        wxRibbonGalleryItem *item = m_items.Item(item_i);
        delete item;
    }
    m_items.Clear();

    m_selected_item = nullptr;
    m_hovered_item = nullptr;
    m_active_item = nullptr;
    m_focused_item = nullptr;
    DoClearExtensionFocus();

    // This points either to one of the button rectangles, which are still
    // valid, or to the rectangle of one of the items deleted above.
    if( m_mouse_active_rect != &m_scroll_up_button_rect &&
        m_mouse_active_rect != &m_scroll_down_button_rect &&
        m_mouse_active_rect != &m_extension_button_rect )
    {
        m_mouse_active_rect = nullptr;
    }
}

bool wxRibbonGallery::IsSizingContinuous() const
{
    return false;
}

void wxRibbonGallery::CalculateMinSize()
{
    if(m_art == nullptr || !m_bitmap_size.IsFullySpecified())
    {
        SetMinSize(wxSize(20, 20));
    }
    else
    {
        m_bitmap_padded_size = m_bitmap_size;
        m_bitmap_padded_size.IncBy(
            m_art->GetMetric(wxRIBBON_ART_GALLERY_BITMAP_PADDING_LEFT_SIZE) +
            m_art->GetMetric(wxRIBBON_ART_GALLERY_BITMAP_PADDING_RIGHT_SIZE),
            m_art->GetMetric(wxRIBBON_ART_GALLERY_BITMAP_PADDING_TOP_SIZE) +
            m_art->GetMetric(wxRIBBON_ART_GALLERY_BITMAP_PADDING_BOTTOM_SIZE));

        wxMemoryDC dc;
        SetMinSize(m_art->GetGallerySize(dc, this, m_bitmap_padded_size));

        // The best size is displaying several items
        m_best_size = m_bitmap_padded_size;
        m_best_size.x *= 3;
        m_best_size = m_art->GetGallerySize(dc, this, m_best_size);
    }
}

bool wxRibbonGallery::Realize()
{
    CalculateMinSize();
    return Layout();
}

bool wxRibbonGallery::Layout()
{
    if(m_art == nullptr)
        return false;

    wxMemoryDC dc;
    wxPoint origin;
    wxSize client_size = m_art->GetGalleryClientSize(dc, this, GetSize(),
        &origin, &m_scroll_up_button_rect, &m_scroll_down_button_rect,
        &m_extension_button_rect);
    m_client_rect = wxRect(origin, client_size);

    int x_cursor = 0;
    int y_cursor = 0;

    size_t item_count = m_items.Count();
    size_t item_i;
    long art_flags = m_art->GetFlags();
    for(item_i = 0; item_i < item_count; ++item_i)
    {
        wxRibbonGalleryItem *item = m_items.Item(item_i);
        item->SetIsVisible(true);
        if(art_flags & wxRIBBON_BAR_FLOW_VERTICAL)
        {
            if(y_cursor + m_bitmap_padded_size.y > client_size.GetHeight())
            {
                if(y_cursor == 0)
                    break;
                y_cursor = 0;
                x_cursor += m_bitmap_padded_size.x;
            }
            item->SetPosition(origin.x + x_cursor, origin.y + y_cursor,
                m_bitmap_padded_size);
            y_cursor += m_bitmap_padded_size.y;
        }
        else
        {
            if(x_cursor + m_bitmap_padded_size.x > client_size.GetWidth())
            {
                if(x_cursor == 0)
                    break;
                x_cursor = 0;
                y_cursor += m_bitmap_padded_size.y;
            }
            item->SetPosition(origin.x + x_cursor, origin.y + y_cursor,
                m_bitmap_padded_size);
            x_cursor += m_bitmap_padded_size.x;
        }
    }
    for(; item_i < item_count; ++item_i)
    {
        wxRibbonGalleryItem *item = m_items.Item(item_i);
        item->SetIsVisible(false);
    }
    if(art_flags & wxRIBBON_BAR_FLOW_VERTICAL)
        m_scroll_limit = x_cursor;
    else
        m_scroll_limit = y_cursor;
    if(m_scroll_amount >= m_scroll_limit)
    {
        m_scroll_amount = m_scroll_limit;
        m_down_button_state = wxRIBBON_GALLERY_BUTTON_DISABLED;
    }
    else if(m_down_button_state == wxRIBBON_GALLERY_BUTTON_DISABLED)
        m_down_button_state = wxRIBBON_GALLERY_BUTTON_NORMAL;

    if(m_scroll_amount <= 0)
    {
        m_scroll_amount = 0;
        m_up_button_state = wxRIBBON_GALLERY_BUTTON_DISABLED;
    }
    else if(m_up_button_state == wxRIBBON_GALLERY_BUTTON_DISABLED)
        m_up_button_state = wxRIBBON_GALLERY_BUTTON_NORMAL;

    return true;
}

wxSize wxRibbonGallery::DoGetBestSize() const
{
    return m_best_size;
}

wxSize wxRibbonGallery::DoGetNextSmallerSize(wxOrientation direction,
                                        wxSize relative_to) const
{
    if(m_art == nullptr)
        return relative_to;

    wxMemoryDC dc;

    wxSize client = m_art->GetGalleryClientSize(dc, this, relative_to, nullptr,
        nullptr, nullptr, nullptr);
    switch(direction)
    {
    case wxHORIZONTAL:
        client.DecBy(1, 0);
        break;
    case wxVERTICAL:
        client.DecBy(0, 1);
        break;
    case wxBOTH:
        client.DecBy(1, 1);
        break;
    }
    if(client.GetWidth() < 0 || client.GetHeight() < 0)
        return relative_to;

    client.x = (client.x / m_bitmap_padded_size.x) * m_bitmap_padded_size.x;
    client.y = (client.y / m_bitmap_padded_size.y) * m_bitmap_padded_size.y;

    wxSize size = m_art->GetGallerySize(dc, this, client);
    wxSize minimum = GetMinSize();

    if(size.GetWidth() < minimum.GetWidth() ||
        size.GetHeight() < minimum.GetHeight())
    {
        return relative_to;
    }

    switch(direction)
    {
    case wxHORIZONTAL:
        size.SetHeight(relative_to.GetHeight());
        break;
    case wxVERTICAL:
        size.SetWidth(relative_to.GetWidth());
        break;
    default:
        break;
    }

    return size;
}

wxSize wxRibbonGallery::DoGetNextLargerSize(wxOrientation direction,
                                       wxSize relative_to) const
{
    if(m_art == nullptr)
        return relative_to;

    wxMemoryDC dc;

    wxSize client = m_art->GetGalleryClientSize(dc, this, relative_to, nullptr,
        nullptr, nullptr, nullptr);

    // No need to grow if the given size can already display every item
    int nitems = (client.GetWidth() / m_bitmap_padded_size.x) *
        (client.GetHeight() / m_bitmap_padded_size.y);
    if(nitems >= (int)m_items.GetCount())
        return relative_to;

    switch(direction)
    {
    case wxHORIZONTAL:
        client.IncBy(m_bitmap_padded_size.x, 0);
        break;
    case wxVERTICAL:
        client.IncBy(0, m_bitmap_padded_size.y);
        break;
    case wxBOTH:
        client.IncBy(m_bitmap_padded_size);
        break;
    }

    client.x = (client.x / m_bitmap_padded_size.x) * m_bitmap_padded_size.x;
    client.y = (client.y / m_bitmap_padded_size.y) * m_bitmap_padded_size.y;

    wxSize size = m_art->GetGallerySize(dc, this, client);
    wxSize minimum = GetMinSize();

    if(size.GetWidth() < minimum.GetWidth() ||
        size.GetHeight() < minimum.GetHeight())
    {
        return relative_to;
    }

    switch(direction)
    {
    case wxHORIZONTAL:
        size.SetHeight(relative_to.GetHeight());
        break;
    case wxVERTICAL:
        size.SetWidth(relative_to.GetWidth());
        break;
    default:
        break;
    }

    return size;
}

bool wxRibbonGallery::IsEmpty() const
{
    return m_items.IsEmpty();
}

unsigned int wxRibbonGallery::GetCount() const
{
    return (unsigned int)m_items.GetCount();
}

wxRibbonGalleryItem* wxRibbonGallery::GetItem(unsigned int n)
{
    if(n >= GetCount())
        return nullptr;
    return m_items.Item(n);
}

void wxRibbonGallery::SetSelection(wxRibbonGalleryItem* item)
{
    if(item != m_selected_item)
    {
        m_selected_item = item;
        Refresh(false);
    }
}

wxRibbonGalleryItem* wxRibbonGallery::GetSelection() const
{
    return m_selected_item;
}

wxRibbonGalleryItem* wxRibbonGallery::GetHoveredItem() const
{
    return m_hovered_item;
}

wxRibbonGalleryItem* wxRibbonGallery::GetActiveItem() const
{
    return m_active_item;
}

wxRibbonGalleryButtonState wxRibbonGallery::GetUpButtonState() const
{
    return m_up_button_state;
}

wxRibbonGalleryButtonState wxRibbonGallery::GetDownButtonState() const
{
    return m_down_button_state;
}

wxRibbonGalleryButtonState wxRibbonGallery::GetExtensionButtonState() const
{
    return m_extension_button_state;
}

#if wxUSE_ACCESSIBILITY

class wxRibbonGalleryAccessible : public wxWindowAccessible
{
public:
    explicit wxRibbonGalleryAccessible(wxRibbonGallery* gallery) : wxWindowAccessible(gallery) { }

    wxAccStatus GetChildCount(int* childCount) override
    {
        wxRibbonGallery* gallery = wxDynamicCast(GetWindow(), wxRibbonGallery);
        wxCHECK(gallery, wxACC_FAIL);

        *childCount = static_cast<int>(gallery->GetCount()) + (HasExtensionButton(gallery) ? 1 : 0);
        return wxACC_OK;
    }

    wxAccStatus GetChild(int childId, wxAccessible** child) override
    {
        *child = (childId == wxACC_SELF) ? this : nullptr;
        return wxACC_OK;
    }

    wxAccStatus GetRole(int childId, wxAccRole* role) override
    {
        if ( childId == wxACC_SELF )
        {
            *role = wxROLE_SYSTEM_LIST;
            return wxACC_OK;
        }

        wxRibbonGallery* gallery = wxDynamicCast(GetWindow(), wxRibbonGallery);
        wxCHECK(gallery, wxACC_FAIL);

        *role = static_cast<unsigned>(childId) <= gallery->GetCount()
            ? wxROLE_SYSTEM_LISTITEM : wxROLE_SYSTEM_PUSHBUTTON;
        return wxACC_OK;
    }

    wxAccStatus GetState(int childId, long* state) override
    {
        wxRibbonGallery* gallery = wxDynamicCast(GetWindow(), wxRibbonGallery);
        wxCHECK(gallery, wxACC_FAIL);

        if ( childId == wxACC_SELF )
        {
            long st{ 0 };
            if ( !gallery->IsEnabled() )
                st |= wxACC_STATE_SYSTEM_UNAVAILABLE;
            if ( !gallery->IsShownOnScreen() )
                st |= wxACC_STATE_SYSTEM_INVISIBLE;
            *state = st;
            return wxACC_OK;
        }

        wxRibbonBar* ribbonBar = gallery->GetAncestorRibbonBar();
        const bool barFocused = ribbonBar && ribbonBar->HasFocus();

        if ( childId >= 0 && static_cast<unsigned>(childId) <= gallery->GetCount() )
        {
            wxRibbonGalleryItem* item = gallery->GetItem(childId - 1);
            wxCHECK(item, wxACC_FAIL);

            long st = wxACC_STATE_SYSTEM_SELECTABLE;
            if ( !item->IsVisible() )
                st |= wxACC_STATE_SYSTEM_INVISIBLE;
            else
                st |= wxACC_STATE_SYSTEM_FOCUSABLE;
            if ( item == gallery->GetSelection() )
                st |= wxACC_STATE_SYSTEM_SELECTED;
            if ( item == gallery->m_focused_item && barFocused )
                st |= wxACC_STATE_SYSTEM_FOCUSED;
            *state = st;
            return wxACC_OK;
        }

        long st = wxACC_STATE_SYSTEM_FOCUSABLE;
        if ( gallery->GetExtensionButtonState() == wxRIBBON_GALLERY_BUTTON_DISABLED )
            st |= wxACC_STATE_SYSTEM_UNAVAILABLE;
        if ( gallery->m_extension_focused && barFocused )
            st |= wxACC_STATE_SYSTEM_FOCUSED;
        *state = st;
        return wxACC_OK;
    }

    wxAccStatus GetName(int childId, wxString* name) override
    {
        if ( childId == wxACC_SELF )
            return wxWindowAccessible::GetName(childId, name);

        wxRibbonGallery* gallery = wxDynamicCast(GetWindow(), wxRibbonGallery);
        wxCHECK(gallery, wxACC_FAIL);

        if ( childId >= 0 && static_cast<unsigned>(childId) < gallery->GetCount() )
            return wxACC_NOT_IMPLEMENTED;

        *name = _("More");
        return wxACC_OK;
    }

    wxAccStatus GetLocation(wxRect& rect, int elementId) override
    {
        if ( elementId == wxACC_SELF )
            return wxWindowAccessible::GetLocation(rect, elementId);

        wxRibbonGallery* gallery = wxDynamicCast(GetWindow(), wxRibbonGallery);
        wxCHECK(gallery, wxACC_FAIL);

        if ( elementId >= 0 && static_cast<unsigned>(elementId) <= gallery->GetCount() )
        {
            wxRibbonGalleryItem* item = gallery->GetItem(elementId - 1);
            wxCHECK(item, wxACC_FAIL);
            if ( !item->IsVisible() )
                return wxACC_FAIL;

            rect = GetOnScreenItemRect(gallery, item);
        }
        else
            rect = gallery->m_extension_button_rect;

        rect.SetPosition(gallery->ClientToScreen(rect.GetPosition()));
        return wxACC_OK;
    }

    wxAccStatus GetDefaultAction(int childId, wxString* actionName) override
    {
        if ( childId == wxACC_SELF )
            return wxACC_NOT_IMPLEMENTED;

        *actionName = _("Press");
        return wxACC_OK;
    }

    wxAccStatus DoDefaultAction(int childId) override
    {
        wxRibbonGallery* gallery = wxDynamicCast(GetWindow(), wxRibbonGallery);
        wxCHECK(gallery, wxACC_FAIL);

        if ( childId == wxACC_SELF )
            return wxACC_NOT_IMPLEMENTED;

        if ( childId >= 0 && static_cast<unsigned>(childId) <= gallery->GetCount() )
            gallery->DoActivateItem(gallery->GetItem(childId - 1));
        else
            gallery->DoActivateExtensionButton();
        return wxACC_OK;
    }

    wxAccStatus GetFocus(int* childId, wxAccessible** child) override
    {
        wxRibbonGallery* gallery = wxDynamicCast(GetWindow(), wxRibbonGallery);
        wxCHECK(gallery, wxACC_FAIL);

        const int index = gallery->DoGetFocusedItemIndex();
        if ( gallery->m_extension_focused )
        {
            *childId = static_cast<int>(gallery->GetCount()) + 1;
            *child = nullptr;
        }
        else if ( index != wxNOT_FOUND )
        {
            *childId = index + 1;
            *child = nullptr;
        }
        else
        {
            *childId = wxACC_SELF;
            *child = this;
        }
        return wxACC_OK;
    }

private:
    static bool HasExtensionButton(wxRibbonGallery* gallery)
    {
        return !gallery->m_extension_button_rect.IsEmpty();
    }

    static wxRect GetOnScreenItemRect(wxRibbonGallery* gallery, wxRibbonGalleryItem* item)
    {
        wxRect rect{ item->GetPosition() };
        const bool vertical = gallery->m_art &&
            (gallery->m_art->GetFlags() & wxRIBBON_BAR_FLOW_VERTICAL);
        if ( vertical )
            rect.SetLeft(rect.GetLeft() - gallery->m_scroll_amount);
        else
            rect.SetTop(rect.GetTop() - gallery->m_scroll_amount);
        return rect;
    }
};

wxAccessible* wxRibbonGallery::CreateAccessible()
{
    return new wxRibbonGalleryAccessible(this);
}

#endif // wxUSE_ACCESSIBILITY

#endif // wxUSE_RIBBON
