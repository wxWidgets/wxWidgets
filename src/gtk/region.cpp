/////////////////////////////////////////////////////////////////////////////
// Name:        region.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/98
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "region.h"
#endif

#include "wx/region.h"

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
};

wxRegionRefData::wxRegionRefData(void)
{
  m_region = (GdkRegion *) NULL;
}

wxRegionRefData::~wxRegionRefData(void)
{
  if (m_region) gdk_region_destroy( m_region );
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
}

wxRegion::wxRegion(void)
{
  m_refData = new wxRegionRefData();
  M_REGIONDATA->m_region = gdk_region_new();
}

wxRegion::~wxRegion(void)
{
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
  return TRUE;
}

bool wxRegion::Union( const wxRegion& region )
{
  GdkRegion *reg = gdk_regions_union( M_REGIONDATA->m_region, region.GetRegion() );
  gdk_region_destroy( M_REGIONDATA->m_region );
  M_REGIONDATA->m_region = reg;
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
  return TRUE;
}

void wxRegion::GetBox( long& x, long& y, long&w, long &h ) const
{
  x = 0;
  y = 0;
  w = -1;
  h = -1;
}

wxRect wxRegion::GetBox(void) const
{
  return wxRect( 0, 0, -1, -1 );
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

GdkRegion *wxRegion::GetRegion(void) const
{
  return M_REGIONDATA->m_region;
}

