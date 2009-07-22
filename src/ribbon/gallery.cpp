///////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/gallery.cpp
// Purpose:     Ribbon control which displays a gallery of items to choose from
// Author:      Peter Cawley
// Modified by:
// Created:     2009-07-22
// RCS-ID:      $Id$
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/ribbon/gallery.h"

#if wxUSE_RIBBON

#include "wx/ribbon/art.h"
#include "wx/dcbuffer.h"
#include "wx/clntdata.h"

#ifndef WX_PRECOMP
#endif

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

IMPLEMENT_CLASS(wxRibbonGallery, wxRibbonControl)

class wxRibbonGalleryItem
{
public:
    wxRibbonGalleryItem()
    {
        m_id = 0;
    }

    void SetId(int id) {m_id = id;}
    void SetBitmap(const wxBitmap& bitmap) {m_bitmap = bitmap;}
    const wxBitmap& GetBitmap() const {return m_bitmap;}

    void SetClientObject(wxClientData *data) {m_client_data.SetClientObject(data);}
    wxClientData *GetClientObject() const {return m_client_data.GetClientObject();}
    void SetClientData(void *data) {m_client_data.SetClientData(data);}
    void *GetClientData() const {return m_client_data.GetClientData();}

protected:
    wxBitmap m_bitmap;
    wxClientDataContainer m_client_data;
    int m_id;
};

BEGIN_EVENT_TABLE(wxRibbonGallery, wxRibbonControl)
    EVT_ERASE_BACKGROUND(wxRibbonGallery::OnEraseBackground)
    EVT_PAINT(wxRibbonGallery::OnPaint)
    EVT_SIZE(wxRibbonGallery::OnSize)
END_EVENT_TABLE()

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
    m_scroll_up_button_rect = wxRect(0, 0, 0, 0);
    m_scroll_down_button_rect = wxRect(0, 0, 0, 0);
    m_extension_button_rect = wxRect(0, 0, 0, 0);
    m_bitmap_size = wxSize(64, 32);
    m_bitmap_padded_size = m_bitmap_size;
    m_item_separation_x = 0;
    m_item_separation_y = 0;

    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}

void wxRibbonGallery::OnEraseBackground(wxEraseEvent& WXUNUSED(evt))
{
    // All painting done in main paint handler to minimise flicker
}

void wxRibbonGallery::OnPaint(wxPaintEvent& WXUNUSED(evt))
{
    wxAutoBufferedPaintDC dc(this);
    if(m_art == NULL)
        return;

    m_art->DrawGalleryBackground(dc, this, GetSize());

    wxPoint origin;
    wxSize client_size = m_art->GetGalleryClientSize(dc, this, GetSize(),
        &origin, NULL, NULL, NULL);
    int padding_top = m_art->GetMetric(wxRIBBON_ART_GALLERY_BITMAP_PADDING_TOP_SIZE);
    int padding_left = m_art->GetMetric(wxRIBBON_ART_GALLERY_BITMAP_PADDING_LEFT_SIZE);
    int padding_right = m_art->GetMetric(wxRIBBON_ART_GALLERY_BITMAP_PADDING_RIGHT_SIZE);
    int padding_bottom = m_art->GetMetric(wxRIBBON_ART_GALLERY_BITMAP_PADDING_BOTTOM_SIZE);
    int x_cursor = 0;
    int y_cursor = 0;

    dc.SetClippingRegion(origin.x, origin.y, client_size.x, client_size.y);

    size_t item_count = m_items.Count();
    size_t item_i;
    for(item_i = 0; item_i < item_count; ++item_i)
    {
        wxRibbonGalleryItem *item = m_items.Item(item_i);
        if(x_cursor + m_bitmap_padded_size.x > client_size.GetWidth())
        {
            if(x_cursor == 0)
                break;
            x_cursor = 0;
            y_cursor += m_bitmap_padded_size.y;
            if(y_cursor >= client_size.GetHeight())
                break;
        }
        dc.DrawBitmap(item->GetBitmap(), origin.x + x_cursor + padding_left,
            origin.y + y_cursor + padding_top);
        x_cursor += m_bitmap_padded_size.x;
    }
}

void wxRibbonGallery::OnSize(wxSizeEvent& evt)
{
    if(m_art == NULL)
        return;

    wxMemoryDC dc;
    wxSize client = m_art->GetGalleryClientSize(dc, this, evt.GetSize(), NULL,
        &m_scroll_up_button_rect, &m_scroll_down_button_rect,
        &m_extension_button_rect);
}

wxRibbonGalleryItem* wxRibbonGallery::Append(const wxBitmap& bitmap, int id)
{
    wxASSERT(bitmap.IsOk());
    if(m_items.IsEmpty())
    {
        m_bitmap_size = bitmap.GetSize();
        CalculateMinSize();
    }
    else
    {
        wxASSERT(bitmap.GetSize() == m_bitmap_size);
    }

    wxRibbonGalleryItem *item = new wxRibbonGalleryItem;
    item->SetId(id);
    item->SetBitmap(bitmap);
    m_items.Add(item);
    return item;
}

wxRibbonGalleryItem* wxRibbonGallery::Append(const wxBitmap& bitmap, int id,
                                             void* clientData)
{
    wxRibbonGalleryItem *item = Append(bitmap, id);
    item->SetClientData(clientData);
    return item;
}

wxRibbonGalleryItem* wxRibbonGallery::Append(const wxBitmap& bitmap, int id,
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
}

bool wxRibbonGallery::IsSizingContinuous() const
{
    return false;
}

void wxRibbonGallery::CalculateMinSize()
{
    if(m_art == NULL || !m_bitmap_size.IsFullySpecified())
    {
        SetMinSize(wxSize(20, 20));
    }
    else
    {
        m_bitmap_padded_size = m_bitmap_size;
        m_bitmap_padded_size.IncBy(
            m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_LEFT_SIZE) +
            m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_RIGHT_SIZE),
            m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_TOP_SIZE) +
            m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_BOTTOM_SIZE));

        wxMemoryDC dc;
        SetMinSize(m_art->GetGallerySize(dc, this, m_bitmap_padded_size));
    }
}

wxSize wxRibbonGallery::DoGetNextSmallerSize(wxOrientation direction,
                                        wxSize relative_to) const
{
    if(m_art == NULL)
        return relative_to;

    wxMemoryDC dc;

    wxSize client = m_art->GetGalleryClientSize(dc, this, relative_to, NULL,
        NULL, NULL, NULL);
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
    if(m_art == NULL)
        return relative_to;

    wxMemoryDC dc;

    wxSize client = m_art->GetGalleryClientSize(dc, this, relative_to, NULL,
        NULL, NULL, NULL);
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


#endif // wxUSE_RIBBON
