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

};

wxRegionRefData::wxRegionRefData(void)
{
};

wxRegionRefData::~wxRegionRefData(void)
{
};

//-----------------------------------------------------------------------------

#define M_REGIONDATA ((wxRegionRefData *)m_refData)

IMPLEMENT_DYNAMIC_CLASS(wxRegion,wxGDIObject);
  
wxRegion::wxRegion( long x, long y, long w, long h )
{
  m_refData = new wxRegionRefData();
};

wxRegion::wxRegion( const wxPoint& topLeft, const wxPoint& bottomRight )
{
  m_refData = new wxRegionRefData();
};

wxRegion::wxRegion( const wxRect& rect )
{
  m_refData = new wxRegionRefData();
};

wxRegion::wxRegion(void)
{
  m_refData = new wxRegionRefData();
};

wxRegion::~wxRegion(void)
{
};

void wxRegion::Clear(void)
{
  UnRef();
  m_refData = new wxRegionRefData();
};

bool wxRegion::Union( long x, long y, long width, long height )
{
  return TRUE;
};

bool wxRegion::Union( const wxRect& rect )
{
  return TRUE;
};

bool wxRegion::Union( const wxRegion& region )
{
  return TRUE;
};

bool wxRegion::Intersect( long x, long y, long width, long height )
{
  return TRUE;
};

bool wxRegion::Intersect( const wxRect& rect )
{
  return TRUE;
};

bool wxRegion::Intersect( const wxRegion& region )
{
  return TRUE;
};

bool wxRegion::Subtract( long x, long y, long width, long height )
{
  return TRUE;
};

bool wxRegion::Subtract( const wxRect& rect )
{
  return TRUE;
};

bool wxRegion::Subtract( const wxRegion& region )
{
  return TRUE;
};

bool wxRegion::Xor( long x, long y, long width, long height )
{
  return TRUE;
};

bool wxRegion::Xor( const wxRect& rect )
{
  return TRUE;
};

bool wxRegion::Xor( const wxRegion& region )
{
  return TRUE;
};

void wxRegion::GetBox( long& x, long& y, long&w, long &h ) const
{
  x = 0;
  y = 0;
  w = -1;
  h = -1;
};

wxRect wxRegion::GetBox(void) const
{
  return wxRect( 0, 0, -1, -1 );
};

bool wxRegion::Empty(void) const
{
};

wxRegionContain wxRegion::Contains( long x, long y ) const
{
  return wxOutRegion;
};

wxRegionContain wxRegion::Contains( long x, long y, long w, long h ) const
{
  return wxOutRegion;
};

