/////////////////////////////////////////////////////////////////////////////
// File:      region.cpp
// Purpose:   Region class
// Author:    Markus Holzem, Julian Smart, Robert Roebling
// Created:   Fri Oct 24 10:46:34 MET 1997
// RCS-ID:      $Id$
// Copyright: (c) 1997 Markus Holzem, Julian Smart, Robert Roebling
// Licence:   wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "region.h"
#endif

#include "wx/region.h"
#include "wx/gdicmn.h"
#include "wx/log.h"

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxRegion, wxGDIObject);
IMPLEMENT_DYNAMIC_CLASS(wxRegionIterator,wxObject);

// ----------------------------------------------------------------------------
// wxRegion construction
// ----------------------------------------------------------------------------

wxRegion::~wxRegion()
{
    // m_refData unrefed in ~wxObject
}

// ----------------------------------------------------------------------------
// wxRegion comparison
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// wxRegion operations
// ----------------------------------------------------------------------------

void wxRegion::Clear()
{
    UnRef();
}

bool wxRegion::Combine(long x, long y, long width, long height, wxRegionOp op)
{
    return false;
}
bool wxRegion::Combine(const wxRegion& region, wxRegionOp op)
{
    return false;
}
bool wxRegion::Combine(const wxRect& rect, wxRegionOp op)
{
    return false;
}


// Does the region contain the point (x,y)?
wxRegionContain wxRegion::Contains(long x, long y) const
{
return wxOutRegion;
}
    
// Does the region contain the point pt?
wxRegionContain wxRegion::Contains(const wxPoint& pt) const
{
return wxOutRegion;
}
    
// Does the region contain the rectangle (x, y, w, h)?
wxRegionContain wxRegion::Contains(long x, long y, long w, long h) const
{
return wxOutRegion;
}
    
// Does the region contain the rectangle rect?
wxRegionContain wxRegion::Contains(const wxRect& rect) const
{
return wxOutRegion;
}
    
void wxRegion::GetBox(wxCoord& x, wxCoord& y, wxCoord&w, wxCoord &h) const
{
}

wxRect wxRegion::GetBox() const
{
    return wxRect();
}

wxRegion::wxRegion()
{
}

wxRegion::wxRegion(wxCoord x, wxCoord y, wxCoord w, wxCoord h)
{
}

wxRegion::wxRegion(const wxPoint& topLeft, const wxPoint& bottomRight)
{
}

wxRegionIterator::wxRegionIterator()
{
}

wxRegionIterator::~wxRegionIterator()
{
}

// vi:sts=4:sw=4:et
