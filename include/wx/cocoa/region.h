/////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/region.h
// Purpose:     wxRegion class
// Author:      David Elliott
// Modified by: 
// Created:     2004/04/12
// RCS-ID:      $Id$
// Copyright:   (c) 2004 David Elliott
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COCOA_REGION_H__
#define _WX_COCOA_REGION_H__

#include "wx/generic/region.h"

typedef struct _NSRect NSRect;

class WXDLLEXPORT wxRegion : public wxRegionGeneric
{
    DECLARE_DYNAMIC_CLASS(wxRegion);
public:
    wxRegion(wxCoord x, wxCoord y, wxCoord w, wxCoord h)
    :   wxRegionGeneric(x,y,w,h)
    {}
    wxRegion(const wxPoint& topLeft, const wxPoint& bottomRight)
    :   wxRegionGeneric(topLeft, bottomRight)
    {}
    wxRegion(const wxRect& rect)
    :   wxRegionGeneric(rect)
    {}
    wxRegion() {}
    wxRegion(const wxBitmap& bmp)
    :   wxRegionGeneric()
        { Union(bmp); }
    wxRegion(const wxBitmap& bmp,
             const wxColour& transColour, int tolerance = 0)
    :   wxRegionGeneric()
        { Union(bmp, transColour, tolerance); }
    virtual ~wxRegion() {}
    wxRegion(const wxRegion& r)
    :   wxRegionGeneric(r)
    {}
    wxRegion& operator= (const wxRegion& r)
    {   return *(wxRegion*)&(this->wxRegionGeneric::operator=(r)); }

    // Cocoa-specific creation
    wxRegion(const NSRect& rect);
    wxRegion(const NSRect *rects, int count);

    // Use the non-transparent pixels of a wxBitmap for the region to combine
    // with this region.  First version takes transparency from bitmap's mask,
    // second lets the user specify the colour to be treated as transparent
    // along with an optional tolerance value.
    // NOTE: implemented in common/rgncmn.cpp
    bool Union(const wxBitmap& bmp);
    bool Union(const wxBitmap& bmp,
               const wxColour& transColour, int tolerance = 0);
    /* And because of function hiding: */
    bool Union(long x, long y, long width, long height)
    {   return wxRegionGeneric::Union(x,y,width,height); }
    bool Union(const wxRect& rect)
    {   return wxRegionGeneric::Union(rect); }
    bool Union(const wxRegion& region)
    {   return wxRegionGeneric::Union(region); }

    // Convert the region to a B&W bitmap with the black pixels being inside
    // the region.
    // NOTE: implemented in common/rgncmn.cpp
    wxBitmap ConvertToBitmap() const;

};

class WXDLLEXPORT wxRegionIterator : public wxRegionIteratorGeneric
{
//    DECLARE_DYNAMIC_CLASS(wxRegionIteratorGeneric);
public:
    wxRegionIterator() {}
    wxRegionIterator(const wxRegion& region)
    :   wxRegionIteratorGeneric(region)
    {}
    wxRegionIterator(const wxRegionIterator& iterator)
    :   wxRegionIteratorGeneric(iterator)
    {}
    virtual ~wxRegionIterator() {}

    wxRegionIterator& operator=(const wxRegionIterator& iter)
    {   return *(wxRegionIterator*)&(this->wxRegionIteratorGeneric::operator=(iter)); }
};

#endif //ndef _WX_COCOA_REGION_H__
