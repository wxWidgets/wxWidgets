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
#include "wx/clntdata.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3
    #include "../winui/private.h"
    #include "wx/winui/private/tlwhost.h"
    #include "wx/app.h"
    #include "wx/scopeguard.h"
    #include "wx/weakref.h"
    #include <winrt/Microsoft.UI.Xaml.Automation.h>
    #include <winrt/Microsoft.UI.Xaml.Markup.h>
    #include <algorithm>
    #include <array>
    #include <atomic>
    #include <cmath>
    #include <functional>
    #include <limits>
    #include <utility>
    #if defined(WXWINUI_RIBBON_TOOLKIT)
        #include "ribboncontainerswinui.h"
    #endif
namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = MUX::Controls;
namespace MUXA = MUX::Automation;
namespace WF = winrt::Windows::Foundation;
namespace
{
std::atomic<std::uint64_t> gs_ribbonGalleryKey{0};
}
#endif

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
#if defined(__WXWINUI__) && wxUSE_WINUI3
    const std::uint64_t key = ++gs_ribbonGalleryKey;
    int GetId() const { return m_id; }
#endif

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

#if defined(__WXWINUI__) && wxUSE_WINUI3
    void SetClientObject(wxClientData *data)
    {
        wxCHECK_RET(m_dataType != wxClientData_Void, "Cannot mix object and void client data");
        m_dataType = wxClientData_Object;
        if ( m_owned.get() == data ) return;
        // Publish before invoking the outgoing user destructor. It may clear
        // the gallery and delete this item, so nothing follows the deletion.
        std::unique_ptr<wxClientData> retired = std::move(m_owned);
        m_owned.reset(data);
    }
    wxClientData* GetClientObject() const
    {
        wxCHECK_MSG(m_dataType != wxClientData_Void, nullptr, "Item has void client data");
        return m_owned.get();
    }
    void SetClientData(void* data)
    {
        wxCHECK_RET(m_dataType != wxClientData_Object, "Cannot mix object and void client data");
        m_dataType = wxClientData_Void;
        m_untyped = data;
    }
    void* GetClientData() const
    {
        wxCHECK_MSG(m_dataType != wxClientData_Object, nullptr, "Item has object client data");
        return m_untyped;
    }
#else
    void SetClientObject(wxClientData *data) {m_client_data.SetClientObject(data);}
    wxClientData *GetClientObject() const {return m_client_data.GetClientObject();}
    void SetClientData(void *data) {m_client_data.SetClientData(data);}
    void *GetClientData() const {return m_client_data.GetClientData();}
#endif

protected:
    wxBitmapBundle m_bitmap;
#if defined(__WXWINUI__) && wxUSE_WINUI3
    std::unique_ptr<wxClientData> m_owned;
    void* m_untyped = nullptr;
    wxClientDataType m_dataType = wxClientData_None;
#else
    wxClientDataContainer m_client_data;
#endif
    wxRect m_position;
    int m_id = 0;
    bool m_is_visible = false;
};

#if defined(__WXWINUI__) && wxUSE_WINUI3
#include "gallerywinui.h"

bool wxRibbonGallery::InitializeWinUI()
{
    if ( m_winui ) return true;
    m_winui = std::make_shared<wxRibbonGalleryWinUI>(this);
    if ( m_winui->Initialize() ) return true;
    m_winui.reset();
    return false;
}

bool wxRibbonGallery::MSWOnEffectiveLayoutDirectionChanged()
{
    if ( auto peer = m_winui ) { peer->Changed(); return true; }
    return false;
}

void wxRibbonGallery::SetArtProvider(wxRibbonArtProvider* art)
{
    wxRibbonControl::SetArtProvider(art);
    if ( auto peer = m_winui ) peer->Changed();
}

wxSize wxRibbonGallery::GetMinSize() const
{
    return m_winui ? m_winui->Minimum() : wxRibbonControl::GetMinSize();
}
#endif

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
#if defined(__WXWINUI__) && wxUSE_WINUI3
    InitializeWinUI();
#endif
}

wxRibbonGallery::~wxRibbonGallery()
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    if ( auto peer = std::move(m_winui) ) peer->Close();
#endif
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
#if defined(__WXWINUI__) && wxUSE_WINUI3
    return InitializeWinUI();
#else
    return true;
#endif
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
#if defined(__WXWINUI__) && wxUSE_WINUI3
    evt.Skip();
#else
    m_hovered = true;
    if(m_mouse_active_rect != nullptr && !evt.LeftIsDown())
    {
        m_mouse_active_rect = nullptr;
        m_active_item = nullptr;
    }
    Refresh(false);
#endif
}

void wxRibbonGallery::OnMouseMove(wxMouseEvent& evt)
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    evt.Skip();
#else
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
#endif
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
#if !defined(__WXWINUI__) || !wxUSE_WINUI3
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
#endif
}

void wxRibbonGallery::OnMouseDown(wxMouseEvent& evt)
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    evt.Skip();
#else
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
#endif
}

void wxRibbonGallery::OnMouseUp(wxMouseEvent& evt)
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    evt.Skip();
#else
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
                wxCommandEvent notification(wxEVT_BUTTON,
                    GetId());
                notification.SetEventObject(this);
                ProcessWindowEvent(notification);
            }
            else if(m_active_item != nullptr)
            {
                if(m_selected_item != m_active_item)
                {
                    m_selected_item = m_active_item;
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
        }
        m_mouse_active_rect = nullptr;
        m_active_item = nullptr;
        Refresh(false);
    }
#endif
}

void wxRibbonGallery::OnMouseDClick(wxMouseEvent& evt)
{
    // The 2nd click of a double-click should be handled as a click in the
    // same way as the 1st click of the double-click. This is useful for
    // scrolling through the gallery.
    OnMouseDown(evt);
    OnMouseUp(evt);
}

void wxRibbonGallery::SetItemClientObject(wxRibbonGalleryItem* itm,
                                          wxClientData* data)
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    const auto peer = m_winui;
    wxCHECK_RET(peer && peer->Key(itm), "Invalid gallery item");
#endif
    itm->SetClientObject(data);
}

wxClientData* wxRibbonGallery::GetItemClientObject(const wxRibbonGalleryItem* itm) const
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    const auto peer = m_winui;
    wxCHECK_MSG(peer && peer->Key(itm), nullptr, "Invalid gallery item");
#endif
    return itm->GetClientObject();
}

void wxRibbonGallery::SetItemClientData(wxRibbonGalleryItem* itm, void* data)
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    const auto peer = m_winui;
    wxCHECK_RET(peer && peer->Key(itm), "Invalid gallery item");
#endif
    itm->SetClientData(data);
}

void* wxRibbonGallery::GetItemClientData(const wxRibbonGalleryItem* itm) const
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    const auto peer = m_winui;
    wxCHECK_MSG(peer && peer->Key(itm), nullptr, "Invalid gallery item");
#endif
    return itm->GetClientData();
}

bool wxRibbonGallery::ScrollLines(int lines)
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    const auto peer = m_winui;
    return peer && peer->Scroll(static_cast<double>(lines) * peer->LineSize());
#else
    if(m_scroll_limit == 0 || m_art == nullptr)
        return false;

    return ScrollPixels(lines * GetScrollLineSize());
#endif
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
#if defined(__WXWINUI__) && wxUSE_WINUI3
    const auto peer = m_winui;
    return peer && peer->Scroll(pixels);
#else
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
#endif
}

void wxRibbonGallery::EnsureVisible(const wxRibbonGalleryItem* item)
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    if ( const auto peer = m_winui ) peer->EnsureVisible(peer->Key(item));
#else
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
#endif
}

bool wxRibbonGallery::IsHovered() const
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    return m_winui && m_winui->IsHovered();
#else
    return m_hovered;
#endif
}

void wxRibbonGallery::OnEraseBackground(wxEraseEvent& WXUNUSED(evt))
{
    // All painting done in main paint handler to minimise flicker
}

void wxRibbonGallery::OnPaint(wxPaintEvent& WXUNUSED(evt))
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    // The native slot owns presentation. Validate the compatibility HWND
    // without drawing the legacy gallery over its WinUI subtree.
    wxPaintDC dc(this);
#else
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
    }
#endif
}

void wxRibbonGallery::OnSize(wxSizeEvent& WXUNUSED(evt))
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    if ( auto peer = m_winui ) peer->Resized();
#else
    Layout();
#endif
}

void wxRibbonGallery::OnDPIChanged(wxDPIChangedEvent& event)
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    if ( auto peer = m_winui ) peer->Changed();
#else
    Realize();
#endif
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
#if defined(__WXWINUI__) && wxUSE_WINUI3
    const auto peer = m_winui;
    return peer ? peer->Append(bitmap, id, nullptr, nullptr, false) : nullptr;
#else
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
#endif
}

wxRibbonGalleryItem* wxRibbonGallery::Append(const wxBitmapBundle& bitmap, int id,
                                             void* clientData)
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    wxASSERT(bitmap.IsOk());
    const auto peer = m_winui;
    return peer ? peer->Append(bitmap, id, clientData, nullptr, false, true) : nullptr;
#else
    wxRibbonGalleryItem *item = Append(bitmap, id);
    item->SetClientData(clientData);
    return item;
#endif
}

wxRibbonGalleryItem* wxRibbonGallery::Append(const wxBitmapBundle& bitmap, int id,
                                             wxClientData* clientData)
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    wxASSERT(bitmap.IsOk());
    const auto peer = m_winui;
    return peer ? peer->Append(bitmap, id, nullptr, clientData, true) : nullptr;
#else
    wxRibbonGalleryItem *item = Append(bitmap, id);
    item->SetClientObject(clientData);
    return item;
#endif
}

void wxRibbonGallery::Clear()
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    if ( const auto peer = m_winui )
    {
        peer->Clear();
        return;
    }
    // Destruction has already retired the adapter. Detach the entire batch
    // before running user client-data destructors, which can call Clear again.
    wxArrayRibbonGalleryItem retired;
    retired.swap(m_items);
    m_selected_item = m_hovered_item = m_active_item = nullptr;
    m_mouse_active_rect = nullptr;
    for ( auto* item : retired ) delete item;
#else
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

    // This points either to one of the button rectangles, which are still
    // valid, or to the rectangle of one of the items deleted above.
    if( m_mouse_active_rect != &m_scroll_up_button_rect &&
        m_mouse_active_rect != &m_scroll_down_button_rect &&
        m_mouse_active_rect != &m_extension_button_rect )
    {
        m_mouse_active_rect = nullptr;
    }
#endif
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
#if defined(__WXWINUI__) && wxUSE_WINUI3
    const auto peer = m_winui;
    return peer && peer->Rebuild();
#else
    CalculateMinSize();
    return Layout();
#endif
}

bool wxRibbonGallery::Layout()
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    const auto peer = m_winui;
    return peer && peer->Rebuild();
#else
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
#endif
}

wxSize wxRibbonGallery::DoGetBestSize() const
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    return m_winui ? m_winui->Best() : wxSize(20, 20);
#else
    return m_best_size;
#endif
}

wxSize wxRibbonGallery::DoGetNextSmallerSize(wxOrientation direction,
                                        wxSize relative_to) const
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    return m_winui ? m_winui->NextSize(direction, relative_to, false) : relative_to;
#else
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
#endif
}

wxSize wxRibbonGallery::DoGetNextLargerSize(wxOrientation direction,
                                       wxSize relative_to) const
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    return m_winui ? m_winui->NextSize(direction, relative_to, true) : relative_to;
#else
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
#endif
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
#if defined(__WXWINUI__) && wxUSE_WINUI3
    const auto peer = m_winui;
    if ( !peer ) return;
    const auto key = peer->Key(item);
    wxCHECK_RET(!item || key, "Invalid gallery item");
    peer->Select(key, false);
#else
    if(item != m_selected_item)
    {
        m_selected_item = item;
        Refresh(false);
    }
#endif
}

wxRibbonGalleryItem* wxRibbonGallery::GetSelection() const
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    return m_winui ? m_winui->Selection() : nullptr;
#else
    return m_selected_item;
#endif
}

wxRibbonGalleryItem* wxRibbonGallery::GetHoveredItem() const
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    return m_winui ? m_winui->Hovered() : nullptr;
#else
    return m_hovered_item;
#endif
}

wxRibbonGalleryItem* wxRibbonGallery::GetActiveItem() const
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    return m_winui ? m_winui->Active() : nullptr;
#else
    return m_active_item;
#endif
}

wxRibbonGalleryButtonState wxRibbonGallery::GetUpButtonState() const
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    const auto peer = m_winui;
    return peer ? peer->ButtonState(0) : wxRIBBON_GALLERY_BUTTON_DISABLED;
#else
    return m_up_button_state;
#endif
}

wxRibbonGalleryButtonState wxRibbonGallery::GetDownButtonState() const
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    const auto peer = m_winui;
    return peer ? peer->ButtonState(1) : wxRIBBON_GALLERY_BUTTON_DISABLED;
#else
    return m_down_button_state;
#endif
}

wxRibbonGalleryButtonState wxRibbonGallery::GetExtensionButtonState() const
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    const auto peer = m_winui;
    return peer ? peer->ButtonState(2) : wxRIBBON_GALLERY_BUTTON_DISABLED;
#else
    return m_extension_button_state;
#endif
}

#endif // wxUSE_RIBBON
