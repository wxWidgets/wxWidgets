/////////////////////////////////////////////////////////////////////////////
// Name:        region.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "region.h"
#endif

#include "wx/region.h"

#include "gdk/gdk.h"
#include "gtk/gtk.h"

#ifdef __WXDEBUG__
#ifdef NULL
#undef NULL
#endif
#define NULL ((void*)0L)
#endif

//-----------------------------------------------------------------------------
// wxRegion
//-----------------------------------------------------------------------------

class wxRegionRefData: public wxObjectRefData
{
  public:
  
    wxRegionRefData(void);
    ~wxRegionRefData(void);
  
  public:    

    GdkRegion  *m_region;
    wxList      m_rects;
};

wxRegionRefData::wxRegionRefData(void)
{
    m_region = (GdkRegion *) NULL;
}

wxRegionRefData::~wxRegionRefData(void)
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
  
wxRegion::wxRegion( long x, long y, long w, long h )
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

wxRegion::wxRegion(void)
{
    m_refData = new wxRegionRefData();
    M_REGIONDATA->m_region = gdk_region_new();
}

wxRegion::~wxRegion(void)
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

void wxRegion::Clear(void)
{
    UnRef();
    m_refData = new wxRegionRefData();
    M_REGIONDATA->m_region = gdk_region_new();
}

bool wxRegion::Union( long x, long y, long width, long height )
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

bool wxRegion::Intersect( long x, long y, long width, long height )
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

bool wxRegion::Subtract( long x, long y, long width, long height )
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

bool wxRegion::Xor( long x, long y, long width, long height )
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

void wxRegion::GetBox( long& x, long& y, long&w, long &h ) const
{
    x = 0;
    y = 0;
    w = -1;
    h = -1;
    wxNode *node = GetRectList()->First();
    while (node)
    {
        wxRect *r = (wxRect*)node->Data();
        if (node == GetRectList()->First())
        {
            x = r->x;
            y = r->y;
            w = r->width;
            h = r->height;
        }
        else
        {
            if (r->x < x)
            {
                x = r->x;
                w += x - r->x;
            }
            if (r->y < y) 
            {
                y = r->y;
                h += y - r->y;
            }
            if (r->width+r->x > x+w) 
            {
                w = r->x + r->width - x;
            }
            if (r->height+r->y > y+h) 
            {
                h = r->y + r->height - y;
            }
        }
        node = node->Next();
    }
}

wxRect wxRegion::GetBox(void) const
{
    long x = 0;
    long y = 0;
    long w = -1;
    long h = -1;
    GetBox( x, y, w, h );
    return wxRect( x, y, w, h );
}

bool wxRegion::Empty(void) const
{
    return gdk_region_empty( M_REGIONDATA->m_region );
}

wxRegionContain wxRegion::Contains( long x, long y ) const
{
    if (gdk_region_point_in( M_REGIONDATA->m_region, x, y ))
        return wxInRegion;
    else
        return wxOutRegion;
}

wxRegionContain wxRegion::Contains( long x, long y, long w, long h ) const
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

GdkRegion *wxRegion::GetRegion(void) const
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
  
wxRegionIterator::wxRegionIterator(void)
{
    m_current = 0;
}

wxRegionIterator::wxRegionIterator( const wxRegion& region )
{
    m_region = region;
    m_current = 0;
}

void wxRegionIterator::Reset( const wxRegion& region )
{
    m_region = region;
    m_current = 0;
}

wxRegionIterator::operator bool (void) const 
{ 
    return m_current < m_region.GetRectList()->Number(); 
}

bool wxRegionIterator::HaveRects(void) const 
{ 
    return m_current < m_region.GetRectList()->Number(); 
}

void wxRegionIterator::operator ++ (void)
{
    if (m_current < m_region.GetRectList()->Number()) ++m_current;
}

void wxRegionIterator::operator ++ (int)
{
    if (m_current < m_region.GetRectList()->Number()) ++m_current;
}

long wxRegionIterator::GetX(void) const
{
    wxNode *node = m_region.GetRectList()->Nth( m_current );
    if (!node) return 0;
    wxRect *r = (wxRect*)node->Data();
    return r->x;
}

long wxRegionIterator::GetY(void) const
{
    wxNode *node = m_region.GetRectList()->Nth( m_current );
    if (!node) return 0;
    wxRect *r = (wxRect*)node->Data();
    return r->y;
}

long wxRegionIterator::GetW(void) const
{
    wxNode *node = m_region.GetRectList()->Nth( m_current );
    if (!node) return 0;
    wxRect *r = (wxRect*)node->Data();
    return r->width;
}

long wxRegionIterator::GetH(void) const
{
    wxNode *node = m_region.GetRectList()->Nth( m_current );
    if (!node) return 0;
    wxRect *r = (wxRect*)node->Data();
    return r->height;
}


