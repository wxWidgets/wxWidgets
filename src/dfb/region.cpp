/////////////////////////////////////////////////////////////////////////////
// Name:      src/mgl/region.cpp
// Purpose:   Region handling for wxWidgets/DFB
// Author:    Vaclav Slavik
// Created:   2006-08-08
// RCS-ID:    $Id$
// Copyright: (c) 2006 REA Elektronik GmbH
// Licence:   wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/region.h"

IMPLEMENT_DYNAMIC_CLASS(wxRegion, wxGDIObject)
IMPLEMENT_DYNAMIC_CLASS(wxRegionIterator, wxObject)

//-----------------------------------------------------------------------------
// wxRegionRefData
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxRegionRefData : public wxGDIRefData
{
public:
    wxRegionRefData() {}
    wxRegionRefData(const wxRect& rect) : m_rect(rect) {}
    wxRegionRefData(const wxRegionRefData& data) : m_rect(data.m_rect) {}

    ~wxRegionRefData() {}

    // default assignment and comparision operators are OK

    wxRect m_rect;
};

#define M_REGION_OF(r) ((wxRegionRefData*)((r).m_refData))
#define M_REGION       M_REGION_OF(*this)

//-----------------------------------------------------------------------------
// wxRegion
//-----------------------------------------------------------------------------

wxObjectRefData *wxRegion::CreateRefData() const
{
    return new wxRegionRefData;
}

wxObjectRefData *wxRegion::CloneRefData(const wxObjectRefData *data) const
{
    return new wxRegionRefData(*(wxRegionRefData *)data);
}

wxRegion::wxRegion()
{
    m_refData = NULL;
}

wxRegion::wxRegion(wxCoord x, wxCoord y, wxCoord w, wxCoord h)
{
    m_refData = new wxRegionRefData(wxRect(x, y, w, h));
}

wxRegion::wxRegion(const wxPoint& topLeft, const wxPoint& bottomRight)
{
    m_refData = new wxRegionRefData(wxRect(topLeft, bottomRight));
}

wxRegion::wxRegion(const wxRect& r)
{
    m_refData = new wxRegionRefData(r);
}

wxRegion::~wxRegion()
{
    // m_refData unrefed in ~wxObject
}

bool wxRegion::operator==(const wxRegion& region) const
{
    if ( m_refData == region.m_refData )
        return true;

    if ( !Ok() )
    {
        // only equal if both are invalid, otherwise different
        return !region.Ok();
    }

    return M_REGION->m_rect == M_REGION_OF(region)->m_rect;
}

//-----------------------------------------------------------------------------
// Information about the region
//-----------------------------------------------------------------------------

void wxRegion::GetBox(wxCoord& x, wxCoord& y, wxCoord&w, wxCoord &h) const
{
    wxRect r = GetBox();
    x = r.GetX();
    y = r.GetY();
    w = r.GetWidth();
    h = r.GetHeight();
}

wxRect wxRegion::GetBox() const
{
    if (m_refData)
        return M_REGION->m_rect;
    else
        return wxRect();
}

bool wxRegion::Empty() const
{
    if (!m_refData)
        return true;

    return M_REGION->m_rect.IsEmpty();
}

//-----------------------------------------------------------------------------
// Modifications
//-----------------------------------------------------------------------------

void wxRegion::Clear()
{
    UnRef();
}

bool wxRegion::Offset(wxCoord x, wxCoord y)
{
    AllocExclusive();
    M_REGION->m_rect.Offset(x, y);
    return true;
}

bool wxRegion::Union(const wxRect& rect)
{
    AllocExclusive();

    if ( M_REGION->m_rect.Inside(rect) )
    {
        return true;
    }
    else if ( rect.Inside(M_REGION->m_rect) )
    {
        M_REGION->m_rect = rect;
        return true;
    }
    else
    {
        wxFAIL_MSG( _T("only rectangular regions are supported") );
        return false;
    }
}

bool wxRegion::Union(const wxRegion& region)
{
    wxCHECK_MSG( region.Ok(), false, _T("invalid region") );
    return Union(M_REGION_OF(region)->m_rect);
}

bool wxRegion::Intersect(const wxRect& rect)
{
    AllocExclusive();
    M_REGION->m_rect.Intersect(rect);
    return true;
}

bool wxRegion::Intersect(const wxRegion& region)
{
    wxCHECK_MSG( region.Ok(), false, _T("invalid region") );
    return Intersect(M_REGION_OF(region)->m_rect);
}

bool wxRegion::Subtract(const wxRect& rect)
{
    wxCHECK_MSG( Ok(), false, _T("invalid region") );

    if ( rect.Inside(M_REGION->m_rect) )
    {
        // subtracted rectangle contains this one, so the result is empty
        // rectangle
        M_REGION->m_rect = wxRect();
        return true;
    }
    else if ( !M_REGION->m_rect.Intersects(rect) )
    {
        // the rectangles are disjoint, so substracting has no effect
        return true;
    }
    else
    {
        wxFAIL_MSG( _T("only rectangular regions implemented") );
        return false;
    }
}

bool wxRegion::Subtract(const wxRegion& region)
{
    wxCHECK_MSG( region.Ok(), false, _T("invalid region") );
    return Subtract(M_REGION_OF(region)->m_rect);
}

bool wxRegion::Xor(const wxRect& rect)
{
    wxFAIL_MSG( _T("Xor not implemented") );
    return false;
}

bool wxRegion::Xor(const wxRegion& region)
{
    wxCHECK_MSG( region.Ok(), false, _T("invalid region") );
    return Xor(M_REGION_OF(region)->m_rect);
}


//-----------------------------------------------------------------------------
// Tests
//-----------------------------------------------------------------------------

wxRegionContain wxRegion::Contains(wxCoord x, wxCoord y) const
{
    wxCHECK_MSG( Ok(), wxOutRegion, _T("invalid region") );

    if (M_REGION->m_rect.Inside(x, y))
        return wxInRegion;
    else
        return wxOutRegion;
}

wxRegionContain wxRegion::Contains(const wxRect& rect) const
{
    wxCHECK_MSG( Ok(), wxOutRegion, _T("invalid region") );

    // 1) is the rectangle entirely covered by the region?
    if (M_REGION->m_rect.Inside(rect))
        return wxInRegion;

    // 2) is the rectangle completely outside the region?
    if (!M_REGION->m_rect.Intersects(rect))
        return wxOutRegion;

    // 3) neither case happened => it is partially covered:
    return wxPartRegion;
}

//-----------------------------------------------------------------------------
// wxRegionIterator
//-----------------------------------------------------------------------------

void wxRegionIterator::Reset(const wxRegion& region)
{
    wxRegionRefData *d = M_REGION_OF(region);
    m_rect = d ? d->m_rect : wxRect();
}

wxRegionIterator& wxRegionIterator::operator++()
{
    // there's only one rectangle in the iterator, so iterating always
    // reaches the end:
    Reset();
    return *this;
}

wxRegionIterator wxRegionIterator::operator++(int)
{
    wxRegionIterator tmp = *this;

    // there's only one rectangle in the iterator, so iterating always
    // reaches the end:
    Reset();

    return tmp;
}
