/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dfb/region.h
// Purpose:     wxRegion class
// Author:      Vaclav Slavik
// Created:     2006-08-08
// RCS-ID:      $Id$
// Copyright:   (c) 2006 REA Elektronik GmbH
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DFB_REGION_H_
#define _WX_DFB_REGION_H_

#include "wx/gdiobj.h"
#include "wx/gdicmn.h"

#warning "Move these to wx/region.h when common code is moved there"
class WXDLLIMPEXP_CORE wxBitmap;
class WXDLLIMPEXP_CORE wxColour;

class WXDLLIMPEXP_CORE wxRegion : public wxGDIObject
{

public:
    wxRegion(wxCoord x, wxCoord y, wxCoord w, wxCoord h);
    wxRegion(const wxPoint& topLeft, const wxPoint& bottomRight);
    wxRegion(const wxRect& rect);
    wxRegion(const wxBitmap& bmp)
    {
        Union(bmp);
    }
    wxRegion(const wxBitmap& bmp,
             const wxColour& transColour, int tolerance = 0)
    {
        Union(bmp, transColour, tolerance);
    }

    wxRegion();
    ~wxRegion();

    #warning "FIXME: move this to common code? at least Ok()"
    bool Ok() const { return m_refData != NULL; }

    bool operator==(const wxRegion& region) const;
    bool operator!=(const wxRegion& region) const { return !(*this == region); }

    // Clear current region
    void Clear();

    bool Offset(wxCoord x, wxCoord y);

    // Union rectangle or region with this.
    bool Union(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
        { return Union(wxRect(x, y, width, height)); }
    bool Union(const wxRect& rect);
    bool Union(const wxRegion& region);

    // Intersect rectangle or region with this.
    bool Intersect(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
        { return Intersect(wxRect(x, y, width, height)); }
    bool Intersect(const wxRect& rect);
    bool Intersect(const wxRegion& region);

    // Subtract rectangle or region from this:
    // Combines the parts of 'this' that are not part of the second region.
    bool Subtract(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
        { return Subtract(wxRect(x, y, width, height)); }
    bool Subtract(const wxRect& rect);
    bool Subtract(const wxRegion& region);

    // XOR: the union of two combined regions except for any overlapping areas.
    bool Xor(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
        { return Xor(wxRect(x, y, width, height)); }
    bool Xor(const wxRect& rect);
    bool Xor(const wxRegion& region);

    // Outer bounds of region
    void GetBox(wxCoord& x, wxCoord& y, wxCoord&w, wxCoord &h) const;
    wxRect GetBox() const;

    // Is region empty?
    bool Empty() const;
    bool IsEmpty() const { return Empty(); }

    // Does the region contain the point (x,y)?
    wxRegionContain Contains(wxCoord x, wxCoord y) const;
    // Does the region contain the point pt?
    wxRegionContain Contains(const wxPoint& pt) const
        { return Contains(pt.x, pt.y); }
    // Does the region contain the rectangle rect?
    wxRegionContain Contains(const wxRect& rect) const;
    // Does the region contain the rectangle (x, y, w, h)?
    wxRegionContain Contains(wxCoord x, wxCoord y, wxCoord w, wxCoord h) const
        { return Contains(wxRect(x, y, w, h)); }


    #warning "Move these union versions + ConvertToBitmap to wxRegionBase"
    // Convert the region to a B&W bitmap with the white pixels being inside
    // the region.
    wxBitmap ConvertToBitmap() const;

    // Use the non-transparent pixels of a wxBitmap for the region to combine
    // with this region.  First version takes transparency from bitmap's mask,
    // second lets the user specify the colour to be treated as transparent
    // along with an optional tolerance value.
    // NOTE: implemented in common/rgncmn.cpp
    bool Union(const wxBitmap& bmp);
    bool Union(const wxBitmap& bmp,
               const wxColour& transColour, int tolerance = 0);

    // NB: implementation detail of DirectFB, should be removed if full
    //     (i.e. not rect-only version is implemented) so that all code that
    //     assumes region==rect breaks
    wxRect AsRect() const { return GetBox(); }

protected:
    // ref counting code
    virtual wxObjectRefData *CreateRefData() const;
    virtual wxObjectRefData *CloneRefData(const wxObjectRefData *data) const;

    friend class WXDLLIMPEXP_CORE wxRegionIterator;

    DECLARE_DYNAMIC_CLASS(wxRegion);
};


class WXDLLIMPEXP_CORE wxRegionIterator : public wxObject
{
public:
    wxRegionIterator() {}
    wxRegionIterator(const wxRegion& region) { Reset(region); }

    void Reset() { m_rect = wxRect(); }
    void Reset(const wxRegion& region);

    bool HaveRects() const { return !m_rect.IsEmpty(); }
    operator bool() const { return HaveRects(); }

    wxRegionIterator& operator++();
    wxRegionIterator operator++(int);

    wxCoord GetX() const { return m_rect.GetX(); }
    wxCoord GetY() const { return m_rect.GetY(); }
    wxCoord GetW() const { return m_rect.GetWidth(); }
    wxCoord GetWidth() const { return GetW(); }
    wxCoord GetH() const { return m_rect.GetHeight(); }
    wxCoord GetHeight() const { return GetH(); }
    wxRect GetRect() const { return m_rect; }

private:
    wxRect m_rect;

    DECLARE_DYNAMIC_CLASS(wxRegionIterator);
};

#endif // _WX_DFB_REGION_H_
