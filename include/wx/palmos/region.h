/////////////////////////////////////////////////////////////////////////////
// Name:        wx/palmos/region.h
// Purpose:     wxRegion class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) 1997-2002 wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_REGION_H_
#define _WX_REGION_H_

#include "wx/gdiobj.h"
#include "wx/gdicmn.h"

class WXDLLEXPORT wxRect;
class WXDLLEXPORT wxPoint;

// So far, for internal use only
enum wxRegionOp
{
    wxRGN_AND,          // Creates the intersection of the two combined regions.
    wxRGN_COPY,         // Creates a copy of the region identified by hrgnSrc1.
    wxRGN_DIFF,         // Combines the parts of hrgnSrc1 that are not part of hrgnSrc2.
    wxRGN_OR,           // Creates the union of two combined regions.
    wxRGN_XOR           // Creates the union of two combined regions except for any overlapping areas.
};

class WXDLLEXPORT wxRegion : public wxGDIObject
{
public:
    wxRegion();
    wxRegion(wxCoord x, wxCoord y, wxCoord w, wxCoord h);
    wxRegion(const wxPoint& topLeft, const wxPoint& bottomRight);
    wxRegion(const wxRect& rect);
    wxRegion(WXHRGN hRegion); // Hangs on to this region
    wxRegion(size_t n, const wxPoint *points, int fillStyle = wxODDEVEN_RULE );
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

    // Modify region
    // -------------

    // Clear current region
    void Clear();

    // Move the region
    bool Offset(wxCoord x, wxCoord y);

    // Union rectangle or region with this.
    bool Union(wxCoord x, wxCoord y, wxCoord width, wxCoord height) { return Combine(x, y, width, height, wxRGN_OR); }
    bool Union(const wxRect& rect) { return Combine(rect, wxRGN_OR); }
    bool Union(const wxRegion& region) { return Combine(region, wxRGN_OR); }

    // Intersect rectangle or region with this.
    bool Intersect(wxCoord x, wxCoord y, wxCoord width, wxCoord height) { return Combine(x, y, width, height, wxRGN_AND); }
    bool Intersect(const wxRect& rect)  { return Combine(rect, wxRGN_AND); }
    bool Intersect(const wxRegion& region)  { return Combine(region, wxRGN_AND); }

    // Subtract rectangle or region from this:
    // Combines the parts of 'this' that are not part of the second region.
    bool Subtract(wxCoord x, wxCoord y, wxCoord width, wxCoord height) { return Combine(x, y, width, height, wxRGN_DIFF); }
    bool Subtract(const wxRect& rect)  { return Combine(rect, wxRGN_DIFF); }
    bool Subtract(const wxRegion& region)  { return Combine(region, wxRGN_DIFF); }

    // XOR: the union of two combined regions except for any overlapping areas.
    bool Xor(wxCoord x, wxCoord y, wxCoord width, wxCoord height) { return Combine(x, y, width, height, wxRGN_XOR); }
    bool Xor(const wxRect& rect)  { return Combine(rect, wxRGN_XOR); }
    bool Xor(const wxRegion& region)  { return Combine(region, wxRGN_XOR); }

    // Information on region
    // ---------------------

    // Outer bounds of region
    void GetBox(wxCoord& x, wxCoord& y, wxCoord&w, wxCoord &h) const;
    wxRect GetBox() const ;

    // Is region empty?
    bool Empty() const;
    inline bool IsEmpty() const { return Empty(); }

    // Tests
    // Does the region contain the point (x,y)?
    wxRegionContain Contains(wxCoord x, wxCoord y) const;
    // Does the region contain the point pt?
    wxRegionContain Contains(const wxPoint& pt) const;
    // Does the region contain the rectangle (x, y, w, h)?
    wxRegionContain Contains(wxCoord x, wxCoord y, wxCoord w, wxCoord h) const;
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
    bool Combine(wxCoord x, wxCoord y, wxCoord width, wxCoord height, wxRegionOp op);
    bool Combine(const wxRegion& region, wxRegionOp op);
    bool Combine(const wxRect& rect, wxRegionOp op);

    // Get internal region handle
    WXHRGN GetHRGN() const;

protected:
    virtual wxObjectRefData *CreateRefData() const;
    virtual wxObjectRefData *CloneRefData(const wxObjectRefData *data) const;

    friend class WXDLLEXPORT wxRegionIterator;

    DECLARE_DYNAMIC_CLASS(wxRegion)
};

class WXDLLEXPORT wxRegionIterator : public wxObject
{
public:
    wxRegionIterator() { Init(); }
    wxRegionIterator(const wxRegion& region);
    wxRegionIterator(const wxRegionIterator& ri) : wxObject(ri) { Init(); *this = ri; }

    wxRegionIterator& operator=(const wxRegionIterator& ri);

    virtual ~wxRegionIterator();

    void Reset() { m_current = 0; }
    void Reset(const wxRegion& region);

    bool HaveRects() const { return (m_current < m_numRects); }

#ifndef __SALFORDC__
    operator bool () const { return HaveRects(); }
#endif

    wxRegionIterator& operator++();
    wxRegionIterator operator++(int);

    wxCoord GetX() const;
    wxCoord GetY() const;
    wxCoord GetW() const;
    wxCoord GetWidth() const { return GetW(); }
    wxCoord GetH() const;
    wxCoord GetHeight() const { return GetH(); }

    wxRect GetRect() const { return wxRect(GetX(), GetY(), GetW(), GetH()); }

private:
    // common part of all ctors
    void Init();

    long     m_current;
    long     m_numRects;
    wxRegion m_region;
    wxRect*  m_rects;

    DECLARE_DYNAMIC_CLASS(wxRegionIterator)
};

#endif
    // _WX_REGION_H_
