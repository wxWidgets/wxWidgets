///////////////////////////////////////////////////////////////////////////////
// Name:        wx/ribbon/gallery.h
// Purpose:     Ribbon control which displays a gallery of items to choose from
// Author:      Peter Cawley
// Modified by:
// Created:     2009-07-22
// RCS-ID:      $Id$
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////
#ifndef _WX_RIBBON_GALLERY_H_
#define _WX_RIBBON_GALLERY_H_

#include "wx/defs.h"

#if wxUSE_RIBBON

#include "wx/ribbon/control.h"

class wxRibbonGalleryItem;

WX_DEFINE_USER_EXPORTED_ARRAY(wxRibbonGalleryItem*, wxArrayRibbonGalleryItem, class WXDLLIMPEXP_RIBBON);

class WXDLLIMPEXP_RIBBON wxRibbonGallery : public wxRibbonControl
{
public:
    wxRibbonGallery();

    wxRibbonGallery(wxWindow* parent,
                  wxWindowID id = wxID_ANY,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = 0);

    virtual ~wxRibbonGallery();

    bool Create(wxWindow* parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0);

    void Clear();

    wxRibbonGalleryItem* Append(const wxBitmap& bitmap, int id);
    wxRibbonGalleryItem* Append(const wxBitmap& bitmap, int id, void* clientData);
    wxRibbonGalleryItem* Append(const wxBitmap& bitmap, int id, wxClientData* clientData);

    wxRibbonGalleryItem* GetSelection() const;
    wxRibbonGalleryItem* GetHoveredItem() const;

    bool IsHovered() const;
    virtual bool IsSizingContinuous() const;
    virtual bool Realize();
    virtual bool Layout();

protected:
    wxBorder GetDefaultBorder() const { return wxBORDER_NONE; }
    void CommonInit(long style);
    void CalculateMinSize();

    void OnEraseBackground(wxEraseEvent& evt);
    void OnMouseEnter(wxMouseEvent& evt);
    void OnMouseLeave(wxMouseEvent& evt);
    void OnPaint(wxPaintEvent& evt);
    void OnSize(wxSizeEvent& evt);

    virtual wxSize DoGetBestSize() const;
    virtual wxSize DoGetNextSmallerSize(wxOrientation direction,
                                        wxSize relative_to) const;
    virtual wxSize DoGetNextLargerSize(wxOrientation direction,
                                       wxSize relative_to) const;

    wxArrayRibbonGalleryItem m_items;
    wxRibbonGalleryItem* m_selected_item;
    wxRibbonGalleryItem* m_hovered_item;
    wxSize m_bitmap_size;
    wxSize m_bitmap_padded_size;
    wxSize m_best_size;
    wxRect m_scroll_up_button_rect;
    wxRect m_scroll_down_button_rect;
    wxRect m_extension_button_rect;
    int m_item_separation_x;
    int m_item_separation_y;
    bool m_hovered;

#ifndef SWIG
    DECLARE_CLASS(wxRibbonGallery)
    DECLARE_EVENT_TABLE()
#endif
};

#endif // wxUSE_RIBBON

#endif // _WX_RIBBON_GALLERY_H_
