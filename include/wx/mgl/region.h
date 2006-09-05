/////////////////////////////////////////////////////////////////////////////
// Name:        region.h
// Purpose:     wxRegion class
// Author:      Vaclav Slavik
// Created:     2001/03/12
// RCS-ID:      $Id$
// Copyright:   (c) 2001-2002 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_REGION_H_
#define _WX_REGION_H_

#include "wx/list.h"
#include "wx/gdiobj.h"
#include "wx/gdicmn.h"

class WXDLLEXPORT wxRect;
class WXDLLEXPORT wxPoint;
class MGLRegion;

class WXDLLEXPORT wxRegion : public wxGDIObject
{
    DECLARE_DYNAMIC_CLASS(wxRegion);
    friend class WXDLLEXPORT wxRegionIterator;

public:
    wxRegion(wxCoord x, wxCoord y, wxCoord w, wxCoord h);
    wxRegion(const wxPoint& topLeft, const wxPoint& bottomRight);
    wxRegion(const wxRect& rect);
    wxRegion(const MGLRegion& region);
    wxRegion( const wxBitmap& bmp)
    {
        Union(bmp);
    }
    wxRegion( const wxBitmap& bmp,
              const wxColour& transColour, int tolerance = 0)
    {
        Union(bmp, transColour, tolerance);
    }

    wxRegion();
    virtual ~wxRegion();

    //# Modify region
    // Clear current region
    void Clear(void);

    bool Offset(wxCoord x, wxCoord y);

    // Union rectangle or region with this.
    bool Union(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
    bool Union(const wxRect& rect) { return Union(rect.x, rect.y, rect.width, rect.height); }
    bool Union(const wxRegion& region);

    // Intersect rectangle or region with this.
    bool Intersect(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
    bool Intersect(const wxRect& rect) { return Intersect(rect.x, rect.y, rect.width, rect.height); }
    bool Intersect(const wxRegion& region);

    // Subtract rectangle or region from this:
    // Combines the parts of 'this' that are not part of the second region.
    bool Subtract(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
    bool Subtract(const wxRect& rect) { return Subtract(rect.x, rect.y, rect.width, rect.height); }
    bool Subtract(const wxRegion& region);

    // XOR: the union of two combined regions except for any overlapping areas.
    bool Xor(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
    bool Xor(const wxRect& rect) { return Xor(rect.x, rect.y, rect.width, rect.height); }
    bool Xor(const wxRegion& region);

    //# Information on region
    // Outer bounds of region
    void GetBox(wxCoord& x, wxCoord& y, wxCoord&w, wxCoord &h) const;
    wxRect GetBox(void) const ;

    // Is region empty?
    bool Empty(void) const;
    inline bool IsEmpty(void) const { return Empty(); }

    //# Tests
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


    // implementation from now on:
    const MGLRegion& GetMGLRegion() const;

protected:
    // ref counting code
    virtual wxObjectRefData *CreateRefData() const;
    virtual wxObjectRefData *CloneRefData(const wxObjectRefData *data) const;
};


WX_DECLARE_EXPORTED_LIST(wxRect, wxRegionRectList);

class WXDLLEXPORT wxRegionIterator : public wxObject
{
    DECLARE_DYNAMIC_CLASS(wxRegionIterator);
public:
    wxRegionIterator(void);
    wxRegionIterator(const wxRegion& region);
    virtual ~wxRegionIterator(void);

    void Reset(void) { m_currentNode = NULL; }
    void Reset(const wxRegion& region);

#ifndef __SALFORDC__
    operator bool (void) const { return (m_currentNode != NULL); }
#endif

    bool HaveRects(void) const { return (m_currentNode != NULL); }

    void operator ++ (void);
    void operator ++ (int);

    wxCoord GetX(void) const;
    wxCoord GetY(void) const;
    wxCoord GetW(void) const;
    wxCoord GetWidth(void) const { return GetW(); }
    wxCoord GetH(void) const;
    wxCoord GetHeight(void) const { return GetH(); }
    wxRect GetRect() const { return wxRect(GetX(), GetY(), GetWidth(), GetHeight()); }

private:
    wxRegionRectList         m_rects;
    wxRegionRectList::Node  *m_currentNode;
};

#endif
    // _WX_REGION_H_
