/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/region.h
// Purpose:     generic wxRegion class
// Author:      David Elliott
// Modified by:
// Created:     2004/04/12
// RCS-ID:      $Id$
// Copyright:   (c) 2004 David Elliott
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_REGION_H__
#define _WX_GENERIC_REGION_H__

#include "wx/gdiobj.h"
#include "wx/gdicmn.h"

class WXDLLEXPORT wxRect;
class WXDLLEXPORT wxPoint;

class WXDLLEXPORT wxRegionGeneric : public wxGDIObject
{
//    DECLARE_DYNAMIC_CLASS(wxRegionGeneric);
    friend class WXDLLEXPORT wxRegionIteratorGeneric;
public:
    wxRegionGeneric(wxCoord x, wxCoord y, wxCoord w, wxCoord h);
    wxRegionGeneric(const wxPoint& topLeft, const wxPoint& bottomRight);
    wxRegionGeneric(const wxRect& rect);
    wxRegionGeneric();
    ~wxRegionGeneric();

    bool Ok() const { return m_refData != NULL; }

    bool operator == ( const wxRegionGeneric& region ) const;
    bool operator != ( const wxRegionGeneric& region ) const { return !(*this == region); }

    //# Modify region
    // Clear current region
    void Clear();

    // Move the region
    bool Offset(wxCoord x, wxCoord y);

    // Union rectangle or region with this.
    bool Union(long x, long y, long width, long height)
    {   return Union(wxRect(x,y,width,height)); }
    bool Union(const wxRect& rect);
    bool Union(const wxRegionGeneric& region);

    // Intersect rectangle or region with this.
    bool Intersect(long x, long y, long width, long height)
    {   return Intersect(wxRect(x,y,width,height)); }
    bool Intersect(const wxRect& rect);
    bool Intersect(const wxRegionGeneric& region);

    // Subtract rectangle or region from this:
    // Combines the parts of 'this' that are not part of the second region.
    bool Subtract(long x, long y, long width, long height)
    {   return Subtract(wxRect(x,y,width,height)); }
    bool Subtract(const wxRect& rect);
    bool Subtract(const wxRegionGeneric& region);

    // XOR: the union of two combined regions except for any overlapping areas.
    bool Xor(long x, long y, long width, long height);
    bool Xor(const wxRect& rect);
    bool Xor(const wxRegionGeneric& region);

    //# Information on region
    // Outer bounds of region
    void GetBox(wxCoord& x, wxCoord& y, wxCoord&w, wxCoord &h) const;
    wxRect GetBox() const;

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

    // Use the non-transparent pixels of a wxBitmap for the region to combine
    // with this region.  First version takes transparency from bitmap's mask,
    // second lets the user specify the colour to be treated as transparent
    // along with an optional tolerance value.
    // NOTE: implemented in common/rgncmn.cpp
    bool Union(const wxBitmap& bmp);
    bool Union(const wxBitmap& bmp,
               const wxColour& transColour, int tolerance = 0);

    // Convert the region to a B&W bitmap with the white pixels being inside
    // the region.
    wxBitmap ConvertToBitmap() const;

protected:
    virtual wxObjectRefData *CreateRefData() const;
    virtual wxObjectRefData *CloneRefData(const wxObjectRefData *data) const;
};

class WXDLLEXPORT wxRegionIteratorGeneric : public wxObject
{
//    DECLARE_DYNAMIC_CLASS(wxRegionIteratorGeneric);
public:
    wxRegionIteratorGeneric();
    wxRegionIteratorGeneric(const wxRegionGeneric& region);
    wxRegionIteratorGeneric(const wxRegionIteratorGeneric& iterator);
    ~wxRegionIteratorGeneric();

    wxRegionIteratorGeneric& operator=(const wxRegionIteratorGeneric& iterator);

    void Reset() { m_current = 0; }
    void Reset(const wxRegionGeneric& region);

    operator bool () const { return HaveRects(); }
    bool HaveRects() const;

    wxRegionIteratorGeneric& operator++();
    wxRegionIteratorGeneric operator++(int);

    long GetX() const;
    long GetY() const;
    long GetW() const;
    long GetWidth() const { return GetW(); }
    long GetH() const;
    long GetHeight() const { return GetH(); }
    wxRect GetRect() const;
private:
    long     m_current;
    wxRegionGeneric m_region;
};

#endif //ndef _WX_GENERIC_REGION_H__
