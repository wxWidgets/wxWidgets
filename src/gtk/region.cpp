/////////////////////////////////////////////////////////////////////////////
// Name:        gtk/region.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "region.h"
#endif

#include "wx/region.h"

#include <gdk/gdk.h>
#include <gtk/gtk.h>


//-----------------------------------------------------------------------------
// wxRegion
//-----------------------------------------------------------------------------

class wxRegionRefData: public wxObjectRefData
{
public:
    wxRegionRefData();
    ~wxRegionRefData();

    GdkRegion  *m_region;
    wxList      m_rects;
};

wxRegionRefData::wxRegionRefData()
{
    m_region = (GdkRegion *) NULL;
}

wxRegionRefData::~wxRegionRefData()
{
    if (m_region) gdk_region_destroy( m_region );

    wxNode *node = m_rects.First();
    while (node)
    {
        wxRect *r = (wxRect*)node->Data();
        delete r;
        node = node->Next();
    }
}

//-----------------------------------------------------------------------------

#define M_REGIONDATA ((wxRegionRefData *)m_refData)

IMPLEMENT_DYNAMIC_CLASS(wxRegion,wxGDIObject);

wxRegion::wxRegion( wxCoord x, wxCoord y, wxCoord w, wxCoord h )
{
    m_refData = new wxRegionRefData();
    GdkRegion *reg = gdk_region_new();
    GdkRectangle rect;
    rect.x = x;
    rect.y = y;
    rect.width = w;
    rect.height = h;
    M_REGIONDATA->m_region = gdk_region_union_with_rect( reg, &rect );
    gdk_region_destroy( reg );
    M_REGIONDATA->m_rects.Append( (wxObject*) new wxRect(x,y,w,h) );
}

wxRegion::wxRegion( const wxPoint& topLeft, const wxPoint& bottomRight )
{
    m_refData = new wxRegionRefData();
    GdkRegion *reg = gdk_region_new();
    GdkRectangle rect;
    rect.x = topLeft.x;
    rect.y = topLeft.y;
    rect.width = bottomRight.x - rect.x;
    rect.height = bottomRight.y - rect.y;
    M_REGIONDATA->m_region = gdk_region_union_with_rect( reg, &rect );
    gdk_region_destroy( reg );
    M_REGIONDATA->m_rects.Append( (wxObject*) new wxRect(topLeft,bottomRight) );
}

wxRegion::wxRegion( const wxRect& rect )
{
    m_refData = new wxRegionRefData();
    GdkRegion *reg = gdk_region_new();
    GdkRectangle g_rect;
    g_rect.x = rect.x;
    g_rect.y = rect.y;
    g_rect.width = rect.width;
    g_rect.height = rect.height;
    M_REGIONDATA->m_region = gdk_region_union_with_rect( reg, &g_rect );
    gdk_region_destroy( reg );

    wxNode *node = M_REGIONDATA->m_rects.First();
    while (node)
    {
        wxRect *r = (wxRect*)node->Data();
        M_REGIONDATA->m_rects.Append( (wxObject*) new wxRect(r->x,r->y,r->width,r->height) );
        node = node->Next();
    }
}

wxRegion::wxRegion()
{
    m_refData = new wxRegionRefData();
    M_REGIONDATA->m_region = gdk_region_new();
}

wxRegion::~wxRegion()
{
}

bool wxRegion::operator == ( const wxRegion& region )
{
    return m_refData == region.m_refData;
}

bool wxRegion::operator != ( const wxRegion& region )
{
    return m_refData != region.m_refData;
}

void wxRegion::Clear()
{
    UnRef();
    m_refData = new wxRegionRefData();
    M_REGIONDATA->m_region = gdk_region_new();
}

bool wxRegion::Union( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
    GdkRectangle rect;
    rect.x = x;
    rect.y = y;
    rect.width = width;
    rect.height = height;
    GdkRegion *reg = gdk_region_union_with_rect( M_REGIONDATA->m_region, &rect );
    gdk_region_destroy( M_REGIONDATA->m_region );
    M_REGIONDATA->m_region = reg;
    M_REGIONDATA->m_rects.Append( (wxObject*) new wxRect(x,y,width,height) );
    return TRUE;
}

bool wxRegion::Union( const wxRect& rect )
{
    GdkRectangle g_rect;
    g_rect.x = rect.x;
    g_rect.y = rect.y;
    g_rect.width = rect.width;
    g_rect.height = rect.height;
    GdkRegion *reg = gdk_region_union_with_rect( M_REGIONDATA->m_region, &g_rect );
    gdk_region_destroy( M_REGIONDATA->m_region );
    M_REGIONDATA->m_region = reg;
    M_REGIONDATA->m_rects.Append( (wxObject*) new wxRect(rect.x,rect.y,rect.width,rect.height) );
    return TRUE;
}

bool wxRegion::Union( const wxRegion& region )
{
    GdkRegion *reg = gdk_regions_union( M_REGIONDATA->m_region, region.GetRegion() );
    gdk_region_destroy( M_REGIONDATA->m_region );
    M_REGIONDATA->m_region = reg;

    wxNode *node = region.GetRectList()->First();
    while (node)
    {
        wxRect *r = (wxRect*)node->Data();
        M_REGIONDATA->m_rects.Append( (wxObject*) new wxRect(r->x,r->y,r->width,r->height) );
        node = node->Next();
    }

    return TRUE;
}

bool wxRegion::Intersect( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
    wxRegion reg( x, y, width, height );
    Intersect( reg );
    return TRUE;
}

bool wxRegion::Intersect( const wxRect& rect )
{
    wxRegion reg( rect );
    Intersect( reg );
    return TRUE;
}

bool wxRegion::Intersect( const wxRegion& region )
{
    GdkRegion *reg = gdk_regions_intersect( M_REGIONDATA->m_region, region.GetRegion() );
    gdk_region_destroy( M_REGIONDATA->m_region );
    M_REGIONDATA->m_region = reg;
    return TRUE;
}

bool wxRegion::Subtract( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
    wxRegion reg( x, y, width, height );
    Subtract( reg );
    return TRUE;
}

bool wxRegion::Subtract( const wxRect& rect )
{
    wxRegion reg( rect );
    Subtract( reg );
    return TRUE;
}

bool wxRegion::Subtract( const wxRegion& region )
{
    GdkRegion *reg = gdk_regions_subtract( M_REGIONDATA->m_region, region.GetRegion() );
    gdk_region_destroy( M_REGIONDATA->m_region );
    M_REGIONDATA->m_region = reg;
    return TRUE;
}

bool wxRegion::Xor( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
    wxRegion reg( x, y, width, height );
    Xor( reg );
    return TRUE;
}

bool wxRegion::Xor( const wxRect& rect )
{
    wxRegion reg( rect );
    Xor( reg );
    return TRUE;
}

bool wxRegion::Xor( const wxRegion& region )
{
    GdkRegion *reg = gdk_regions_xor( M_REGIONDATA->m_region, region.GetRegion() );
    gdk_region_destroy( M_REGIONDATA->m_region );
    M_REGIONDATA->m_region = reg;

    wxNode *node = region.GetRectList()->First();
    while (node)
    {
        wxRect *r = (wxRect*)node->Data();
        M_REGIONDATA->m_rects.Append( (wxObject*) new wxRect(r->x,r->y,r->width,r->height) );
        node = node->Next();
    }

    return TRUE;
}

void wxRegion::GetBox( wxCoord& x, wxCoord& y, wxCoord&w, wxCoord &h ) const
{
    x = 0;
    y = 0;
    w = -1;
    h = -1;
    GdkRectangle rect;
    gdk_region_get_clipbox( M_REGIONDATA->m_region, &rect );
    x = rect.x;
    y = rect.y;
    w = rect.width;
    h = rect.height;
}

wxRect wxRegion::GetBox() const
{
    wxCoord x = 0;
    wxCoord y = 0;
    wxCoord w = -1;
    wxCoord h = -1;
    GetBox( x, y, w, h );
    return wxRect( x, y, w, h );
}

bool wxRegion::Empty() const
{
    return gdk_region_empty( M_REGIONDATA->m_region );
}

wxRegionContain wxRegion::Contains( wxCoord x, wxCoord y ) const
{
    if (gdk_region_point_in( M_REGIONDATA->m_region, x, y ))
        return wxInRegion;
    else
        return wxOutRegion;
}

wxRegionContain wxRegion::Contains( wxCoord x, wxCoord y, wxCoord w, wxCoord h ) const
{
    GdkRectangle rect;
    rect.x = x;
    rect.y = y;
    rect.width = w;
    rect.height = h;
    GdkOverlapType res = gdk_region_rect_in( M_REGIONDATA->m_region, &rect );
    switch (res)
    {
        case GDK_OVERLAP_RECTANGLE_IN:   return wxInRegion;
        case GDK_OVERLAP_RECTANGLE_OUT:  return wxOutRegion;
        case GDK_OVERLAP_RECTANGLE_PART: return wxPartRegion;
    }
    return wxOutRegion;
}

wxRegionContain wxRegion::Contains(const wxPoint& pt) const
{
    return Contains( pt.x, pt.y );
}

wxRegionContain wxRegion::Contains(const wxRect& rect) const
{
    return Contains( rect.x, rect.y, rect.width, rect.height );
}

GdkRegion *wxRegion::GetRegion() const
{
    return M_REGIONDATA->m_region;
}

wxList *wxRegion::GetRectList() const
{
    return &(M_REGIONDATA->m_rects);
}

//-----------------------------------------------------------------------------
// wxRegion
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxRegionIterator,wxObject);

wxRegionIterator::wxRegionIterator()
{
    Reset();
}

wxRegionIterator::wxRegionIterator( const wxRegion& region )
{
    Reset(region);
}

void wxRegionIterator::Reset( const wxRegion& region )
{
    m_region = region;
    Reset();
}

wxRegionIterator::operator bool () const
{
    return m_current < (size_t)m_region.GetRectList()->Number();
}

bool wxRegionIterator::HaveRects() const
{
    return m_current < (size_t)m_region.GetRectList()->Number();
}

void wxRegionIterator::operator ++ ()
{
    if (m_current < (size_t)m_region.GetRectList()->Number()) ++m_current;
}

void wxRegionIterator::operator ++ (int)
{
    if (m_current < (size_t)m_region.GetRectList()->Number()) ++m_current;
}

wxCoord wxRegionIterator::GetX() const
{
    wxNode *node = m_region.GetRectList()->Nth( m_current );
    if (!node) return 0;
    wxRect *r = (wxRect*)node->Data();
    return r->x;
}

wxCoord wxRegionIterator::GetY() const
{
    wxNode *node = m_region.GetRectList()->Nth( m_current );
    if (!node) return 0;
    wxRect *r = (wxRect*)node->Data();
    return r->y;
}

wxCoord wxRegionIterator::GetW() const
{
    wxNode *node = m_region.GetRectList()->Nth( m_current );
    if (!node) return 0;
    wxRect *r = (wxRect*)node->Data();
    return r->width;
}

wxCoord wxRegionIterator::GetH() const
{
    wxNode *node = m_region.GetRectList()->Nth( m_current );
    if (!node) return 0;
    wxRect *r = (wxRect*)node->Data();
    return r->height;
}


