#ifndef __WX_REGION_H__
#define __WX_REGION_H__

#include "wx/generic/region.h"

class WXDLLEXPORT wxRegion : public wxRegionGeneric {
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

private:
    DECLARE_DYNAMIC_CLASS(wxRegion);
};

class WXDLLEXPORT wxRegionIterator : public wxRegionIteratorGeneric
{
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

#endif // __WX_REGION_H__
