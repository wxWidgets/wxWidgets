/////////////////////////////////////////////////////////////////////////////
// Name:      palmos/region.cpp
// Purpose:   wxRegion implementation
// Author:    William Osborne
// Modified by:
// Created:   10/13/04
// RCS-ID:    $Id: 
// Copyright: (c) William Osborne
// Licence:   wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "region.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/region.h"
#include "wx/gdicmn.h"

IMPLEMENT_DYNAMIC_CLASS(wxRegion, wxGDIObject)
IMPLEMENT_DYNAMIC_CLASS(wxRegionIterator, wxObject)

// ----------------------------------------------------------------------------
// wxRegionRefData implementation
// ----------------------------------------------------------------------------

// ============================================================================
// wxRegion implementation
// ============================================================================

// ----------------------------------------------------------------------------
// ctors and dtor
// ----------------------------------------------------------------------------

wxRegion::wxRegion()
{
}

wxRegion::wxRegion(WXHRGN hRegion)
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

wxRegion::wxRegion(size_t n, const wxPoint *points, int fillStyle)
{
}

wxRegion::~wxRegion()
{
}

wxObjectRefData *wxRegion::CreateRefData() const
{
    return NULL;
}

wxObjectRefData *wxRegion::CloneRefData(const wxObjectRefData *data) const
{
    return NULL;
}

// ----------------------------------------------------------------------------
// wxRegion operations
// ----------------------------------------------------------------------------

// Clear current region
void wxRegion::Clear()
{
}

bool wxRegion::Offset(wxCoord x, wxCoord y)
{
    return false;
}

// combine another region with this one
bool wxRegion::Combine(const wxRegion& rgn, wxRegionOp op)
{
    return false;
}

// Combine rectangle (x, y, w, h) with this.
bool wxRegion::Combine(wxCoord x, wxCoord y,
                       wxCoord width, wxCoord height,
                       wxRegionOp op)
{
    return false;
}

bool wxRegion::Combine(const wxRect& rect, wxRegionOp op)
{
    return false;
}

// ----------------------------------------------------------------------------
// wxRegion bounding box
// ----------------------------------------------------------------------------

// Outer bounds of region
void wxRegion::GetBox(wxCoord& x, wxCoord& y, wxCoord&w, wxCoord &h) const
{
}

wxRect wxRegion::GetBox() const
{
    return wxRect(0, 0, 0, 0);
}

// Is region empty?
bool wxRegion::Empty() const
{
    return true;
}

// ----------------------------------------------------------------------------
// wxRegion hit testing
// ----------------------------------------------------------------------------

// Does the region contain the point (x,y)?
wxRegionContain wxRegion::Contains(wxCoord x, wxCoord y) const
{
    return wxOutRegion;
}

// Does the region contain the point pt?
wxRegionContain wxRegion::Contains(const wxPoint& pt) const
{
    return wxOutRegion;
}

// Does the region contain the rectangle (x, y, w, h)?
wxRegionContain wxRegion::Contains(wxCoord x, wxCoord y,
                                   wxCoord w, wxCoord h) const
{
    return wxOutRegion;
}

// Does the region contain the rectangle rect
wxRegionContain wxRegion::Contains(const wxRect& rect) const
{
    return wxOutRegion;
}

// Get internal region handle
WXHRGN wxRegion::GetHRGN() const
{
    return 0;
}

// ============================================================================
// wxRegionIterator implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxRegionIterator ctors/dtor
// ----------------------------------------------------------------------------

void wxRegionIterator::Init()
{
}

wxRegionIterator::~wxRegionIterator()
{
}

// Initialize iterator for region
wxRegionIterator::wxRegionIterator(const wxRegion& region)
{
}

wxRegionIterator& wxRegionIterator::operator=(const wxRegionIterator& ri)
{
    return *this;
}

// ----------------------------------------------------------------------------
// wxRegionIterator operations
// ----------------------------------------------------------------------------

// Reset iterator for a new region.
void wxRegionIterator::Reset(const wxRegion& region)
{
}

wxRegionIterator& wxRegionIterator::operator++()
{
    return *this;
}

wxRegionIterator wxRegionIterator::operator ++ (int)
{
    return *this;
}

// ----------------------------------------------------------------------------
// wxRegionIterator accessors
// ----------------------------------------------------------------------------

wxCoord wxRegionIterator::GetX() const
{
    return 0;
}

wxCoord wxRegionIterator::GetY() const
{
    return 0;
}

wxCoord wxRegionIterator::GetW() const
{
    return 0;
}

wxCoord wxRegionIterator::GetH() const
{
    return 0;
}

