/////////////////////////////////////////////////////////////////////////////
// Name:        brush.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
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
  
    int        m_style;
    wxBitmap   m_stipple;
    wxColour   m_colour;
};

wxBrushRefData::wxBrushRefData(void)
{
  m_style = 0;
};

//-----------------------------------------------------------------------------

#define M_BRUSHDATA ((wxBrushRefData *)m_refData)

IMPLEMENT_DYNAMIC_CLASS(wxBrush,wxGDIObject)

wxBrush::wxBrush(void)
{
  if (wxTheBrushList) wxTheBrushList->AddBrush( this );
};

wxBrush::wxBrush( const wxColour &colour, const int style )
{
  m_refData = new wxBrushRefData();
  M_BRUSHDATA->m_style = style;
  M_BRUSHDATA->m_colour = colour;
  
  if (wxTheBrushList) wxTheBrushList->AddBrush( this );
};

wxBrush::wxBrush( const wxString &colourName, const int style )
{
  m_refData = new wxBrushRefData();
  M_BRUSHDATA->m_style = style;
  M_BRUSHDATA->m_colour = colourName;
  
  if (wxTheBrushList) wxTheBrushList->AddBrush( this );
};

wxBrush::wxBrush( const wxBitmap &stippleBitmap )
{
  m_refData = new wxBrushRefData();
  M_BRUSHDATA->m_style = wxSTIPPLE;
  M_BRUSHDATA->m_colour = *wxBLACK;
  M_BRUSHDATA->m_stipple = stippleBitmap;
  
  if (wxTheBrushList) wxTheBrushList->AddBrush( this );
};

wxBrush::wxBrush( const wxBrush &brush )
{
  Ref( brush );
  
  if (wxTheBrushList) wxTheBrushList->AddBrush( this );
};

wxBrush::wxBrush( const wxBrush *brush )
{
  if (brush) Ref( *brush );
  
  if (wxTheBrushList) wxTheBrushList->Append( this );
};

wxBrush::~wxBrush(void)
{
  if (wxTheBrushList) wxTheBrushList->RemoveBrush( this );
};

wxBrush& wxBrush::operator = ( const wxBrush& brush )
{
  if (*this == brush) return (*this); 
  Ref( brush ); 
  return *this; 
};
  
bool wxBrush::operator == ( const wxBrush& brush )
{
  return m_refData == brush.m_refData; 
};

bool wxBrush::operator != ( const wxBrush& brush )
{
  return m_refData != brush.m_refData; 
};

bool wxBrush::Ok(void) const
{
  return ((m_refData) && M_BRUSHDATA->m_colour.Ok());
};

int wxBrush::GetStyle(void) const
{
  return M_BRUSHDATA->m_style;
};

wxColour &wxBrush::GetColour(void) const
{
  return M_BRUSHDATA->m_colour;
};

wxBitmap *wxBrush::GetStipple(void) const
{
  return &M_BRUSHDATA->m_stipple;
};


