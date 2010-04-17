/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/region.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/region.h"

wxRegion::wxRegion()
{
}

wxRegion::wxRegion(wxCoord x, wxCoord y, wxCoord w, wxCoord h)
{
}

wxRegion::wxRegion(const wxPoint& topLeft, const wxPoint& bottomRight)
{
}

wxRegion::wxRegion(const wxRect& rect)
{
}

wxRegion::wxRegion(size_t n, const wxPoint *points, wxPolygonFillMode fillStyle)
{
}

wxRegion::wxRegion(const wxBitmap& bmp)
{
}

wxRegion::wxRegion(const wxBitmap& bmp, const wxColour& transp, int tolerance)
{
}

bool wxRegion::IsEmpty() const
{
    return false;
}

void wxRegion::Clear()
{
}

wxGDIRefData *wxRegion::CreateGDIRefData() const
{
    return NULL;
}

wxGDIRefData *wxRegion::CloneGDIRefData(const wxGDIRefData *data) const
{
    return NULL;
}

bool wxRegion::DoIsEqual(const wxRegion& region) const
{
    return false;
}

bool wxRegion::DoGetBox(wxCoord& x, wxCoord& y, wxCoord& w, wxCoord& h) const
{
    return false;
}

wxRegionContain wxRegion::DoContainsPoint(wxCoord x, wxCoord y) const
{
    return wxRegionContain();
}

wxRegionContain wxRegion::DoContainsRect(const wxRect& rect) const
{
    return wxRegionContain();
}

bool wxRegion::DoOffset(wxCoord x, wxCoord y)
{
    return false;
}

bool wxRegion::DoUnionWithRect(const wxRect& rect)
{
    return false;
}

bool wxRegion::DoUnionWithRegion(const wxRegion& region)
{
    return false;
}

bool wxRegion::DoIntersect(const wxRegion& region)
{
    return false;
}

bool wxRegion::DoSubtract(const wxRegion& region)
{
    return false;
}

bool wxRegion::DoXor(const wxRegion& region)
{
    return false;
}

//##############################################################################

wxRegionIterator::wxRegionIterator()
{
}

wxRegionIterator::wxRegionIterator(const wxRegion& region)
{
}

wxRegionIterator::wxRegionIterator(const wxRegionIterator& ri)
{
}

wxRegionIterator::~wxRegionIterator()
{
}

wxRegionIterator& wxRegionIterator::operator=(const wxRegionIterator& ri)
{
    return *this;
}

void wxRegionIterator::Reset()
{
}

void wxRegionIterator::Reset(const wxRegion& region)
{
}

bool wxRegionIterator::HaveRects() const
{
    return false;
}

wxRegionIterator::operator bool () const
{
    return false;
}

wxRegionIterator& wxRegionIterator::operator ++ ()
{
    return *this;
}

wxRegionIterator wxRegionIterator::operator ++ (int)
{
    return *this;
}

wxCoord wxRegionIterator::GetX() const
{
    return wxCoord();
}

wxCoord wxRegionIterator::GetY() const
{
    return wxCoord();
}

wxCoord wxRegionIterator::GetW() const
{
    return wxCoord();
}

wxCoord wxRegionIterator::GetWidth() const
{
    return wxCoord();
}

wxCoord wxRegionIterator::GetH() const
{
    return wxCoord();
}

wxCoord wxRegionIterator::GetHeight() const
{
    return wxCoord();
}

wxRect wxRegionIterator::GetRect() const
{
    return wxRect();
}

