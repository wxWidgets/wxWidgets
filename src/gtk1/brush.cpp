/////////////////////////////////////////////////////////////////////////////
// Name:        brush.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "brush.h"
#endif

#include "wx/brush.h"

//-----------------------------------------------------------------------------
// wxBrush
//-----------------------------------------------------------------------------

class wxBrushRefData: public wxObjectRefData
{
  public:
  
    wxBrushRefData(void);
    wxBrushRefData( const wxBrushRefData& data );
 
    int        m_style;
    wxBitmap   m_stipple;
    wxColour   m_colour;
};

wxBrushRefData::wxBrushRefData(void)
{
  m_style = 0;
}

wxBrushRefData::wxBrushRefData( const wxBrushRefData& data )
{
  m_style = data.m_style;
  m_stipple = data.m_stipple;
  m_colour = data.m_colour;
}

//-----------------------------------------------------------------------------

#define M_BRUSHDATA ((wxBrushRefData *)m_refData)

IMPLEMENT_DYNAMIC_CLASS(wxBrush,wxGDIObject)

wxBrush::wxBrush(void)
{
  if (wxTheBrushList) wxTheBrushList->AddBrush( this );
}

wxBrush::wxBrush( const wxColour &colour, int style )
{
  m_refData = new wxBrushRefData();
  M_BRUSHDATA->m_style = style;
  M_BRUSHDATA->m_colour = colour;
  
  if (wxTheBrushList) wxTheBrushList->AddBrush( this );
}

wxBrush::wxBrush( const wxBitmap &stippleBitmap )
{
  m_refData = new wxBrushRefData();
  M_BRUSHDATA->m_style = wxSTIPPLE;
  M_BRUSHDATA->m_colour = *wxBLACK;
  M_BRUSHDATA->m_stipple = stippleBitmap;
  
  if (wxTheBrushList) wxTheBrushList->AddBrush( this );
}

wxBrush::wxBrush( const wxBrush &brush )
{
  Ref( brush );
  
  if (wxTheBrushList) wxTheBrushList->AddBrush( this );
}

wxBrush::wxBrush( const wxBrush *brush )
{
  if (brush) Ref( *brush );
  
  if (wxTheBrushList) wxTheBrushList->Append( this );
}

wxBrush::~wxBrush(void)
{
  if (wxTheBrushList) wxTheBrushList->RemoveBrush( this );
}

wxBrush& wxBrush::operator = ( const wxBrush& brush )
{
  if (*this == brush) return (*this); 
  Ref( brush ); 
  return *this; 
}
  
bool wxBrush::operator == ( const wxBrush& brush )
{
  return m_refData == brush.m_refData; 
}

bool wxBrush::operator != ( const wxBrush& brush )
{
  return m_refData != brush.m_refData; 
}

bool wxBrush::Ok(void) const
{
  return ((m_refData) && M_BRUSHDATA->m_colour.Ok());
}

int wxBrush::GetStyle(void) const
{
  if (m_refData == NULL)
  {
    wxFAIL_MSG( "invalid brush" );
    return 0;
  }

  return M_BRUSHDATA->m_style;
}

wxColour &wxBrush::GetColour(void) const
{
  if (m_refData == NULL)
  {
    wxFAIL_MSG( "invalid brush" );
    return wxNullColour;
  }
  
  return M_BRUSHDATA->m_colour;
}

wxBitmap *wxBrush::GetStipple(void) const
{
  if (m_refData == NULL)
  {
    wxFAIL_MSG( "invalid brush" );
    return &wxNullBitmap;
  }
  
  return &M_BRUSHDATA->m_stipple;
}

void wxBrush::SetColour( const wxColour& col )
{
  Unshare();
  M_BRUSHDATA->m_colour = col;
}

void wxBrush::SetColour( unsigned char r, unsigned char g, unsigned char b )
{
  Unshare();
  M_BRUSHDATA->m_colour.Set( r, g, b );
}

void wxBrush::SetStyle( int style )
{
  Unshare();
  M_BRUSHDATA->m_style = style;
}

void wxBrush::SetStipple( const wxBitmap& stipple )
{
  Unshare();
  M_BRUSHDATA->m_stipple = stipple;
}

void wxBrush::Unshare(void)
{
  if (!m_refData)
  {
    m_refData = new wxBrushRefData();
  }
  else
  {
    wxBrushRefData* ref = new wxBrushRefData( *(wxBrushRefData*)m_refData );
    UnRef();
    m_refData = ref;
  }
}

