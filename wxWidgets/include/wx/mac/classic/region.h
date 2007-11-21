/////////////////////////////////////////////////////////////////////////////
// Name:        region.h
// Purpose:     wxRegion class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_REGION_H_
#define _WX_REGION_H_

#include "wx/list.h"
#include "wx/gdiobj.h"
#include "wx/gdicmn.h"

class WXDLLEXPORT wxRect;
class WXDLLEXPORT wxPoint;

// So far, for internal use only
enum wxRegionOp {
wxRGN_AND,          // Creates the intersection of the two combined regions.
wxRGN_COPY,         // Creates a copy of the region identified by hrgnSrc1.
wxRGN_DIFF,         // Combines the parts of hrgnSrc1 that are not part of hrgnSrc2.
wxRGN_OR,           // Creates the union of two combined regions.
wxRGN_XOR           // Creates the union of two combined regions except for any overlapping areas.
};

class WXDLLEXPORT wxRegion : public wxGDIObject {
DECLARE_DYNAMIC_CLASS(wxRegion);
    friend class WXDLLEXPORT wxRegionIterator;
public:
    wxRegion(long x, long y, long w, long h);
    wxRegion(const wxPoint& topLeft, const wxPoint& bottomRight);
    wxRegion(const wxRect& rect);
    wxRegion( WXHRGN hRegion );
    wxRegion();
    wxRegion( const wxBitmap& bmp)
    {
        Union(bmp);
    }
    wxRegion( const wxBitmap& bmp,
              const wxColour& transColour, int tolerance = 0)
    {
        Union(bmp, transColour, tolerance);
    }

    virtual ~wxRegion();

    //# Modify region
    // Clear current region
    void Clear();

    // Union rectangle or region with this.
    bool Union(long x, long y, long width, long height)
        { return Combine(x, y, width, height, wxRGN_OR); }
    bool Union(const wxRect& rect)
        { return Combine(rect, wxRGN_OR); }
    bool Union(const wxRegion& region)
        { return Combine(region, wxRGN_OR); }

    // Intersect rectangle or region with this.
    bool Intersect(long x, long y, long width, long height)
        { return Combine(x, y, width, height, wxRGN_AND); }
    bool Intersect(const wxRect& rect)
        { return Combine(rect, wxRGN_AND); }
    bool Intersect(const wxRegion& region)
        { return Combine(region, wxRGN_AND); }

    // Subtract rectangle or region from this:
    // Combines the parts of 'this' that are not part of the second region.
    bool Subtract(long x, long y, long width, long height)
        { return Combine(x, y, width, height, wxRGN_DIFF); }
    bool Subtract(const wxRect& rect)
        { return Combine(rect, wxRGN_DIFF); }
    bool Subtract(const wxRegion& region)
        { return Combine(region, wxRGN_DIFF); }

    // XOR: the union of two combined regions except for any overlapping areas.
    bool Xor(long x, long y, long width, long height)
        { return Combine(x, y, width, height, wxRGN_XOR); }
    bool Xor(const wxRect& rect)
        { return Combine(rect, wxRGN_XOR); }
    bool Xor(const wxRegion& region)
        { return Combine(region, wxRGN_XOR); }

    //# Information on region
    // Outer bounds of region
    void GetBox(wxCoord& x, wxCoord& y, wxCoord&w, wxCoord &h) const;
    wxRect GetBox() const ;

    // Is region empty?
    bool Empty() const;
    inline bool IsEmpty() const { return Empty(); }

    //# Tests
    // Does the region contain the point (x,y)?
    wxRegionContain Contains(long x, long y) const;
    // Does the region contain the point pt?
    wxRegionContain Contains(const wxPoint& pt) const;
    // Does the region contain the rectangle (x, y, w, h)?
    wxRegionContain Contains(long x, long y, long w, long h) const;
    // Does the region contain the rectangle rect?
    wxRegionContain Contains(const wxRect& rect) const;

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

    // Internal
    bool Combine(long x, long y, long width, long height, wxRegionOp op);
    bool Combine(const wxRegion& region, wxRegionOp op);
    bool Combine(const wxRect& rect, wxRegionOp op);
    const WXHRGN GetWXHRGN() const ;
};

class WXDLLEXPORT wxRegionIterator : public wxObject
{
    DECLARE_DYNAMIC_CLASS(wxRegionIterator)

public:
    wxRegionIterator();
    wxRegionIterator(const wxRegion& region);
    wxRegionIterator(const wxRegionIterator& iterator);
    virtual ~wxRegionIterator();

    wxRegionIterator& operator=(const wxRegionIterator& iterator);

    void Reset() { m_current = 0; }
    void Reset(const wxRegion& region);

    operator bool () const { return m_current < m_numRects; }
    bool HaveRects() const { return m_current < m_numRects; }

    wxRegionIterator& operator++();
    wxRegionIterator operator++(int);

    long GetX() const;
    long GetY() const;
    long GetW() const;
    long GetWidth() const { return GetW(); }
    long GetH() const;
    long GetHeight() const { return GetH(); }
    wxRect GetRect() const { return wxRect(GetX(), GetY(), GetWidth(), GetHeight()); }
private:
    void SetRects(long numRects, wxRect *rects);

    long     m_current;
    long     m_numRects;
    wxRegion m_region;
    wxRect*  m_rects;
};

#endif
    // _WX_REGION_H_
