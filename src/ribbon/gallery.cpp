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

    void SetId(int id) { m_id = id; }
    void SetBitmap(const wxBitmapBundle& bitmap) { m_bitmap = bitmap; }
    const wxBitmapBundle& GetBitmapBundle() const { return m_bitmap; }
    wxBitmap GetBitmap(wxWindow* wnd) const { return m_bitmap.GetBitmapFor(wnd); }
    void SetIsVisible(bool visible) { m_isVisible = visible; }
    void SetPosition(int x, int y, const wxSize& size)
    {
        m_position = wxRect(wxPoint(x, y), size);
    }
    bool IsVisible() const { return m_isVisible; }
    const wxRect& GetPosition() const { return m_position; }

    void SetClientObject(wxClientData *data) { m_clientData.SetClientObject(data); }
    wxClientData *GetClientObject() const { return m_clientData.GetClientObject(); }
    void SetClientData(void *data) { m_clientData.SetClientData(data); }
    void *GetClientData() const { return m_clientData.GetClientData(); }

protected:
    wxBitmapBundle m_bitmap;
    wxClientDataContainer m_clientData;
    wxRect m_position;
    int m_id = 0;
    bool m_isVisible = false;
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
    if ( !wxRibbonControl::Create(parent, id, pos, size, wxBORDER_NONE) )
    {
        return false;
    }

    CommonInit(style);
    return true;
}

void wxRibbonGallery::CommonInit(long WXUNUSED(style))
{
    m_selectedItem = nullptr;
    m_hoveredItem = nullptr;
    m_activeItem = nullptr;
    m_scrollUpButtonRect = wxRect(0, 0, 0, 0);
    m_scrollDownButtonRect = wxRect(0, 0, 0, 0);
    m_extensionButtonRect = wxRect(0, 0, 0, 0);
    m_mouseActiveRect = nullptr;
    m_bitmapSize = wxSize(64, 32);
    m_bitmapPaddedSize = m_bitmapSize;
    m_itemSeparationX = 0;
    m_itemSeparationY = 0;
    m_scrollAmount = 0;
    m_scrollLimit = 0;
    m_upButtonState = wxRIBBON_GALLERY_BUTTON_DISABLED;
    m_downButtonState = wxRIBBON_GALLERY_BUTTON_NORMAL;
    m_extensionButtonState = wxRIBBON_GALLERY_BUTTON_NORMAL;
    m_hovered = false;

    SetBackgroundStyle(wxBG_STYLE_PAINT);
}

void wxRibbonGallery::OnMouseEnter(wxMouseEvent& evt)
{
    m_hovered = true;
    if ( m_mouseActiveRect != nullptr && !evt.LeftIsDown() )
    {
        m_mouseActiveRect = nullptr;
        m_activeItem = nullptr;
    }
    Refresh(false);
}

void wxRibbonGallery::OnMouseMove(wxMouseEvent& evt)
{
    bool refresh = false;
    wxPoint pos = evt.GetPosition();

    if ( TestButtonHover(m_scrollUpButtonRect, pos, &m_upButtonState) )
        refresh = true;
    if ( TestButtonHover(m_scrollDownButtonRect, pos, &m_downButtonState) )
        refresh = true;
    if ( TestButtonHover(m_extensionButtonRect, pos, &m_extensionButtonState) )
        refresh = true;

    wxRibbonGalleryItem *hoveredItem = nullptr;
    wxRibbonGalleryItem *activeItem = nullptr;
    if ( m_clientRect.Contains(pos) )
    {
        if ( m_art && m_art->GetFlags() & wxRIBBON_BAR_FLOW_VERTICAL )
            pos.x += m_scrollAmount;
        else
            pos.y += m_scrollAmount;

        size_t itemCount = m_items.Count();
        size_t i;
        for ( i = 0; i < itemCount; ++i )
        {
            wxRibbonGalleryItem *item = m_items.Item(i);
            if ( !item->IsVisible() )
                continue;

            if ( item->GetPosition().Contains(pos) )
            {
                if ( m_mouseActiveRect == &item->GetPosition() )
                    activeItem = item;
                hoveredItem = item;
                break;
            }
        }
    }
    if ( activeItem != m_activeItem )
    {
        m_activeItem = activeItem;
        refresh = true;
    }
    if ( hoveredItem != m_hoveredItem )
    {
        m_hoveredItem = hoveredItem;
        wxRibbonGalleryEvent notification(
            wxEVT_RIBBONGALLERY_HOVER_CHANGED, GetId());
        notification.SetEventObject(this);
        notification.SetGallery(this);
        notification.SetGalleryItem(hoveredItem);
        ProcessWindowEvent(notification);
        refresh = true;
    }

    if ( refresh )
        Refresh(false);
}

bool wxRibbonGallery::TestButtonHover(const wxRect& rect, wxPoint pos,
        wxRibbonGalleryButtonState* state)
{
    if ( *state == wxRIBBON_GALLERY_BUTTON_DISABLED )
        return false;

    wxRibbonGalleryButtonState newState;
    if ( rect.Contains(pos) )
    {
        if ( m_mouseActiveRect == &rect )
            newState = wxRIBBON_GALLERY_BUTTON_ACTIVE;
        else
            newState = wxRIBBON_GALLERY_BUTTON_HOVERED;
    }
    else
        newState = wxRIBBON_GALLERY_BUTTON_NORMAL;

    if ( newState != *state )
    {
        *state = newState;
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
    m_activeItem = nullptr;
    if ( m_upButtonState != wxRIBBON_GALLERY_BUTTON_DISABLED )
        m_upButtonState = wxRIBBON_GALLERY_BUTTON_NORMAL;
    if ( m_downButtonState != wxRIBBON_GALLERY_BUTTON_DISABLED )
        m_downButtonState = wxRIBBON_GALLERY_BUTTON_NORMAL;
    if ( m_extensionButtonState != wxRIBBON_GALLERY_BUTTON_DISABLED )
        m_extensionButtonState = wxRIBBON_GALLERY_BUTTON_NORMAL;
    if ( m_hoveredItem != nullptr )
    {
        m_hoveredItem = nullptr;
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
    wxPoint pos = evt.GetPosition();
    m_mouseActiveRect = nullptr;
    if ( m_clientRect.Contains(pos) )
    {
        if ( m_art && m_art->GetFlags() & wxRIBBON_BAR_FLOW_VERTICAL )
            pos.x += m_scrollAmount;
        else
            pos.y += m_scrollAmount;
        size_t itemCount = m_items.Count();
        size_t i;
        for ( i = 0; i < itemCount; ++i )
        {
            wxRibbonGalleryItem *item = m_items.Item(i);
            if ( !item->IsVisible() )
                continue;

            const wxRect& rect = item->GetPosition();
            if ( rect.Contains(pos) )
            {
                m_activeItem = item;
                m_mouseActiveRect = &rect;
                break;
            }
        }
    }
    else if ( m_scrollUpButtonRect.Contains(pos) )
    {
        if ( m_upButtonState != wxRIBBON_GALLERY_BUTTON_DISABLED )
        {
            m_mouseActiveRect = &m_scrollUpButtonRect;
            m_upButtonState = wxRIBBON_GALLERY_BUTTON_ACTIVE;
        }
    }
    else if ( m_scrollDownButtonRect.Contains(pos) )
    {
        if ( m_downButtonState != wxRIBBON_GALLERY_BUTTON_DISABLED )
        {
            m_mouseActiveRect = &m_scrollDownButtonRect;
            m_downButtonState = wxRIBBON_GALLERY_BUTTON_ACTIVE;
        }
    }
    else if ( m_extensionButtonRect.Contains(pos) )
    {
        if ( m_extensionButtonState != wxRIBBON_GALLERY_BUTTON_DISABLED )
        {
            m_mouseActiveRect = &m_extensionButtonRect;
            m_extensionButtonState = wxRIBBON_GALLERY_BUTTON_ACTIVE;
        }
    }
    if ( m_mouseActiveRect != nullptr )
        Refresh(false);
}

void wxRibbonGallery::OnMouseUp(wxMouseEvent& evt)
{
    if ( m_mouseActiveRect != nullptr )
    {
        wxPoint pos = evt.GetPosition();
        if ( m_activeItem )
        {
            if ( m_art && m_art->GetFlags() & wxRIBBON_BAR_FLOW_VERTICAL )
                pos.x += m_scrollAmount;
            else
                pos.y += m_scrollAmount;
        }
        if ( m_mouseActiveRect->Contains(pos) )
        {
            if ( m_mouseActiveRect == &m_scrollUpButtonRect )
            {
                m_upButtonState = wxRIBBON_GALLERY_BUTTON_HOVERED;
                ScrollLines(-1);
            }
            else if ( m_mouseActiveRect == &m_scrollDownButtonRect )
            {
                m_downButtonState = wxRIBBON_GALLERY_BUTTON_HOVERED;
                ScrollLines(1);
            }
            else if ( m_mouseActiveRect == &m_extensionButtonRect )
            {
                m_extensionButtonState = wxRIBBON_GALLERY_BUTTON_HOVERED;
                wxCommandEvent notification(wxEVT_BUTTON,
                    GetId());
                notification.SetEventObject(this);
                ProcessWindowEvent(notification);
            }
            else if ( m_activeItem != nullptr )
            {
                if ( m_selectedItem != m_activeItem )
                {
                    m_selectedItem = m_activeItem;
                    wxRibbonGalleryEvent notification(
                        wxEVT_RIBBONGALLERY_SELECTED, GetId());
                    notification.SetEventObject(this);
                    notification.SetGallery(this);
                    notification.SetGalleryItem(m_selectedItem);
                    ProcessWindowEvent(notification);
                }

                wxRibbonGalleryEvent notification(
                    wxEVT_RIBBONGALLERY_CLICKED, GetId());
                notification.SetEventObject(this);
                notification.SetGallery(this);
                notification.SetGalleryItem(m_selectedItem);
                ProcessWindowEvent(notification);
            }
        }
        m_mouseActiveRect = nullptr;
        m_activeItem = nullptr;
        Refresh(false);
    }
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
    if ( m_scrollLimit == 0 || m_art == nullptr )
        return false;

    return ScrollPixels(lines * GetScrollLineSize());
}

int wxRibbonGallery::GetScrollLineSize() const
{
    if ( m_art == nullptr )
        return 32;

    int lineSize = m_bitmapPaddedSize.GetHeight();
    if ( m_art->GetFlags() & wxRIBBON_BAR_FLOW_VERTICAL )
        lineSize = m_bitmapPaddedSize.GetWidth();

    return lineSize;
}

bool wxRibbonGallery::ScrollPixels(int pixels)
{
    if ( m_scrollLimit == 0 || m_art == nullptr )
        return false;

    if ( pixels < 0 )
    {
        if ( m_scrollAmount > 0 )
        {
            m_scrollAmount += pixels;
            if ( m_scrollAmount <= 0 )
            {
                m_scrollAmount = 0;
                m_upButtonState = wxRIBBON_GALLERY_BUTTON_DISABLED;
            }
            else if ( m_upButtonState == wxRIBBON_GALLERY_BUTTON_DISABLED )
                m_upButtonState = wxRIBBON_GALLERY_BUTTON_NORMAL;
            if ( m_downButtonState == wxRIBBON_GALLERY_BUTTON_DISABLED )
                m_downButtonState = wxRIBBON_GALLERY_BUTTON_NORMAL;
            return true;
        }
    }
    else if ( pixels > 0 )
    {
        if ( m_scrollAmount < m_scrollLimit )
        {
            m_scrollAmount += pixels;
            if ( m_scrollAmount >= m_scrollLimit )
            {
                m_scrollAmount = m_scrollLimit;
                m_downButtonState = wxRIBBON_GALLERY_BUTTON_DISABLED;
            }
            else if ( m_downButtonState == wxRIBBON_GALLERY_BUTTON_DISABLED )
                m_downButtonState = wxRIBBON_GALLERY_BUTTON_NORMAL;
            if ( m_upButtonState == wxRIBBON_GALLERY_BUTTON_DISABLED )
                m_upButtonState = wxRIBBON_GALLERY_BUTTON_NORMAL;
            return true;
        }
    }
    return false;
}

void wxRibbonGallery::EnsureVisible(const wxRibbonGalleryItem* item)
{
    if ( item == nullptr || !item->IsVisible() || IsEmpty() || m_art == nullptr )
        return;

    if ( m_art->GetFlags() & wxRIBBON_BAR_FLOW_VERTICAL )
    {
        int x = item->GetPosition().GetLeft();
        int baseX = m_items.Item(0)->GetPosition().GetLeft();
        int delta = x - baseX - m_scrollAmount;
        ScrollLines(delta / m_bitmapPaddedSize.GetWidth());
    }
    else
    {
        int y = item->GetPosition().GetTop();
        int baseY = m_items.Item(0)->GetPosition().GetTop();
        int delta = y - baseY - m_scrollAmount;
        ScrollLines(delta / m_bitmapPaddedSize.GetHeight());
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
    if ( m_art == nullptr )
        return;

    m_art->DrawGalleryBackground(dc, this, GetSize());

    int padding_top = m_art->GetMetric(wxRIBBON_ART_GALLERY_BITMAP_PADDING_TOP_SIZE);
    int padding_left = m_art->GetMetric(wxRIBBON_ART_GALLERY_BITMAP_PADDING_LEFT_SIZE);

    dc.SetClippingRegion(m_clientRect);

    bool offsetVertical = true;
    if ( m_art->GetFlags() & wxRIBBON_BAR_FLOW_VERTICAL )
        offsetVertical = false;
    size_t itemCount = m_items.Count();
    size_t i;
    for ( i = 0; i < itemCount; ++i )
    {
        wxRibbonGalleryItem *item = m_items.Item(i);
        if ( !item->IsVisible() )
            continue;

        const wxRect& pos = item->GetPosition();
        wxRect offset_pos(pos);
        if ( offsetVertical )
            offset_pos.SetTop(offset_pos.GetTop() - m_scrollAmount);
        else
            offset_pos.SetLeft(offset_pos.GetLeft() - m_scrollAmount);
        m_art->DrawGalleryItemBackground(dc, this, offset_pos, item);
        // Resolve bitmap bundle for current DPI
        wxBitmap bmp = item->GetBitmap(this);
        if ( bmp.IsOk() )
            dc.DrawBitmap(bmp, offset_pos.GetLeft() + padding_left,
                offset_pos.GetTop() + padding_top);
    }
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

wxRibbonGalleryItem* wxRibbonGallery::Append(const wxBitmapBundle& bitmap, int id)
{
    wxASSERT(bitmap.IsOk());
    if ( m_items.IsEmpty() )
    {
        m_bitmapSize = bitmap.GetDefaultSize();
        CalculateMinSize();
    }
    else
    {
        wxASSERT(bitmap.GetDefaultSize() == m_bitmapSize);
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
    size_t itemCount = m_items.Count();
    for ( size_t  i = 0; i < itemCount; ++i )
    {
        wxRibbonGalleryItem *item = m_items.Item(i);
        delete item;
    }
    m_items.Clear();

    m_selectedItem = nullptr;
    m_hoveredItem = nullptr;
    m_activeItem = nullptr;
}

bool wxRibbonGallery::IsSizingContinuous() const
{
    return false;
}

void wxRibbonGallery::CalculateMinSize()
{
    if ( m_art == nullptr || !m_bitmapSize.IsFullySpecified() )
    {
        SetMinSize(wxSize(20, 20));
    }
    else
    {
        m_bitmapPaddedSize = m_bitmapSize;
        m_bitmapPaddedSize.IncBy(
            m_art->GetMetric(wxRIBBON_ART_GALLERY_BITMAP_PADDING_LEFT_SIZE) +
            m_art->GetMetric(wxRIBBON_ART_GALLERY_BITMAP_PADDING_RIGHT_SIZE),
            m_art->GetMetric(wxRIBBON_ART_GALLERY_BITMAP_PADDING_TOP_SIZE) +
            m_art->GetMetric(wxRIBBON_ART_GALLERY_BITMAP_PADDING_BOTTOM_SIZE));

        wxMemoryDC dc;
        SetMinSize(m_art->GetGallerySize(dc, this, m_bitmapPaddedSize));

        // The best size is displaying several items
        m_bestSize = m_bitmapPaddedSize;
        m_bestSize.x *= 3;
        m_bestSize = m_art->GetGallerySize(dc, this, m_bestSize);
    }
}

bool wxRibbonGallery::Realize()
{
    CalculateMinSize();
    return Layout();
}

bool wxRibbonGallery::Layout()
{
    if ( m_art == nullptr )
        return false;

    wxMemoryDC dc;
    wxPoint origin;
    wxSize clientSize = m_art->GetGalleryClientSize(dc, this, GetSize(),
        &origin, &m_scrollUpButtonRect, &m_scrollDownButtonRect,
        &m_extensionButtonRect);
    m_clientRect = wxRect(origin, clientSize);

    int x_cursor = 0;
    int y_cursor = 0;

    size_t itemCount = m_items.Count();
    size_t i;
    long artFlags = m_art->GetFlags();
    for ( i = 0; i < itemCount; ++i )
    {
        wxRibbonGalleryItem *item = m_items.Item(i);
        item->SetIsVisible(true);
        if ( artFlags & wxRIBBON_BAR_FLOW_VERTICAL )
        {
            if ( y_cursor + m_bitmapPaddedSize.y > clientSize.GetHeight() )
            {
                if ( y_cursor == 0 )
                    break;
                y_cursor = 0;
                x_cursor += m_bitmapPaddedSize.x;
            }
            item->SetPosition(origin.x + x_cursor, origin.y + y_cursor,
                m_bitmapPaddedSize);
            y_cursor += m_bitmapPaddedSize.y;
        }
        else
        {
            if ( x_cursor + m_bitmapPaddedSize.x > clientSize.GetWidth() )
            {
                if ( x_cursor == 0 )
                    break;
                x_cursor = 0;
                y_cursor += m_bitmapPaddedSize.y;
            }
            item->SetPosition(origin.x + x_cursor, origin.y + y_cursor,
                m_bitmapPaddedSize);
            x_cursor += m_bitmapPaddedSize.x;
        }
    }
    for ( ; i < itemCount; ++i )
    {
        wxRibbonGalleryItem *item = m_items.Item(i);
        item->SetIsVisible(false);
    }
    if ( artFlags & wxRIBBON_BAR_FLOW_VERTICAL )
        m_scrollLimit = x_cursor;
    else
        m_scrollLimit = y_cursor;
    if ( m_scrollAmount >= m_scrollLimit )
    {
        m_scrollAmount = m_scrollLimit;
        m_downButtonState = wxRIBBON_GALLERY_BUTTON_DISABLED;
    }
    else if ( m_downButtonState == wxRIBBON_GALLERY_BUTTON_DISABLED )
        m_downButtonState = wxRIBBON_GALLERY_BUTTON_NORMAL;

    if ( m_scrollAmount <= 0 )
    {
        m_scrollAmount = 0;
        m_upButtonState = wxRIBBON_GALLERY_BUTTON_DISABLED;
    }
    else if ( m_upButtonState == wxRIBBON_GALLERY_BUTTON_DISABLED )
        m_upButtonState = wxRIBBON_GALLERY_BUTTON_NORMAL;

    return true;
}

wxSize wxRibbonGallery::DoGetBestSize() const
{
    return m_bestSize;
}

wxSize wxRibbonGallery::DoGetNextSmallerSize(wxOrientation direction,
                                        wxSize relativeTo) const
{
    if ( m_art == nullptr )
        return relativeTo;

    wxMemoryDC dc;

    wxSize client = m_art->GetGalleryClientSize(dc, this, relativeTo, nullptr,
        nullptr, nullptr, nullptr);
    switch ( direction )
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
    if ( client.GetWidth() < 0 || client.GetHeight() < 0 )
        return relativeTo;

    client.x = (client.x / m_bitmapPaddedSize.x) * m_bitmapPaddedSize.x;
    client.y = (client.y / m_bitmapPaddedSize.y) * m_bitmapPaddedSize.y;

    wxSize size = m_art->GetGallerySize(dc, this, client);
    wxSize minimum = GetMinSize();

    if ( size.GetWidth() < minimum.GetWidth() ||
         size.GetHeight() < minimum.GetHeight() )
    {
        return relativeTo;
    }

    switch ( direction )
    {
    case wxHORIZONTAL:
        size.SetHeight(relativeTo.GetHeight());
        break;
    case wxVERTICAL:
        size.SetWidth(relativeTo.GetWidth());
        break;
    default:
        break;
    }

    return size;
}

wxSize wxRibbonGallery::DoGetNextLargerSize(wxOrientation direction,
                                       wxSize relativeTo) const
{
    if ( m_art == nullptr )
        return relativeTo;

    wxMemoryDC dc;

    wxSize client = m_art->GetGalleryClientSize(dc, this, relativeTo, nullptr,
        nullptr, nullptr, nullptr);

    // No need to grow if the given size can already display every item
    int nItems = (client.GetWidth() / m_bitmapPaddedSize.x) *
        (client.GetHeight() / m_bitmapPaddedSize.y);
    if ( nItems >= (int)m_items.GetCount() )
        return relativeTo;

    switch ( direction )
    {
    case wxHORIZONTAL:
        client.IncBy(m_bitmapPaddedSize.x, 0);
        break;
    case wxVERTICAL:
        client.IncBy(0, m_bitmapPaddedSize.y);
        break;
    case wxBOTH:
        client.IncBy(m_bitmapPaddedSize);
        break;
    }

    client.x = (client.x / m_bitmapPaddedSize.x) * m_bitmapPaddedSize.x;
    client.y = (client.y / m_bitmapPaddedSize.y) * m_bitmapPaddedSize.y;

    wxSize size = m_art->GetGallerySize(dc, this, client);
    wxSize minimum = GetMinSize();

    if ( size.GetWidth() < minimum.GetWidth() ||
         size.GetHeight() < minimum.GetHeight() )
    {
        return relativeTo;
    }

    switch ( direction )
    {
    case wxHORIZONTAL:
        size.SetHeight(relativeTo.GetHeight());
        break;
    case wxVERTICAL:
        size.SetWidth(relativeTo.GetWidth());
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
    if ( n >= GetCount() )
        return nullptr;
    return m_items.Item(n);
}

void wxRibbonGallery::SetSelection(wxRibbonGalleryItem* item)
{
    if ( item != m_selectedItem )
    {
        m_selectedItem = item;
        Refresh(false);
    }
}

wxRibbonGalleryItem* wxRibbonGallery::GetSelection() const
{
    return m_selectedItem;
}

wxRibbonGalleryItem* wxRibbonGallery::GetHoveredItem() const
{
    return m_hoveredItem;
}

wxRibbonGalleryItem* wxRibbonGallery::GetActiveItem() const
{
    return m_activeItem;
}

wxRibbonGalleryButtonState wxRibbonGallery::GetUpButtonState() const
{
    return m_upButtonState;
}

wxRibbonGalleryButtonState wxRibbonGallery::GetDownButtonState() const
{
    return m_downButtonState;
}

wxRibbonGalleryButtonState wxRibbonGallery::GetExtensionButtonState() const
{
    return m_extensionButtonState;
}

#endif // wxUSE_RIBBON
