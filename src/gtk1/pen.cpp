/////////////////////////////////////////////////////////////////////////////
// Name:        pen.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "pen.h"
#endif

#include "wx/pen.h"

//-----------------------------------------------------------------------------
// wxPen
//-----------------------------------------------------------------------------

class wxPenRefData: public wxObjectRefData
{
  public:
  
    wxPenRefData(void);
  
    int        m_width;
    int        m_style;
    int        m_joinStyle;
    int        m_capStyle;
    wxColour   m_colour;
};

wxPenRefData::wxPenRefData(void)
{
  m_width = 1;
  m_style = wxSOLID;
  m_joinStyle = wxJOIN_ROUND;
  m_capStyle = wxCAP_ROUND;
}

//-----------------------------------------------------------------------------

#define M_PENDATA ((wxPenRefData *)m_refData)

IMPLEMENT_DYNAMIC_CLASS(wxPen,wxGDIObject)

wxPen::wxPen(void)
{
  if (wxThePenList) wxThePenList->AddPen( this );
}

wxPen::wxPen( const wxColour &colour, int width, int style )
{
  m_refData = new wxPenRefData();
  M_PENDATA->m_width = width;
  M_PENDATA->m_style = style;
  M_PENDATA->m_colour = colour;
  if (wxThePenList) wxThePenList->AddPen( this );
}

wxPen::wxPen( const wxString &colourName, int width, int style )
{
  m_refData = new wxPenRefData();
  M_PENDATA->m_width = width;
  M_PENDATA->m_style = style;
  M_PENDATA->m_colour = colourName;
  if (wxThePenList) wxThePenList->AddPen( this );
}

wxPen::wxPen( const wxPen& pen )
{
  Ref( pen );
  if (wxThePenList) wxThePenList->AddPen( this );
}

wxPen::wxPen( const wxPen* pen )
{
  UnRef();
  if (pen) Ref( *pen ); 
  if (wxThePenList) wxThePenList->AddPen( this );
}

wxPen::~wxPen(void)
{
  if (wxThePenList) wxThePenList->RemovePen( this );
}

wxPen& wxPen::operator = ( const wxPen& pen )
{
  if (*this == pen) return (*this); 
  Ref( pen ); 
  return *this; 
}

bool wxPen::operator == ( const wxPen& pen )
{
  return m_refData == pen.m_refData; 
}

bool wxPen::operator != ( const wxPen& pen )
{
  return m_refData != pen.m_refData; 
}

void wxPen::SetColour( const wxColour &colour )
{
  if (!m_refData)
    m_refData = new wxPenRefData();

  M_PENDATA->m_colour = colour;
}

void wxPen::SetColour( const wxString &colourName )
{
  if (!m_refData)
    m_refData = new wxPenRefData();

  M_PENDATA->m_colour = colourName;
}

void wxPen::SetColour( int red, int green, int blue )
{
  if (!m_refData)
    m_refData = new wxPenRefData();

  M_PENDATA->m_colour.Set( red, green, blue );
}

void wxPen::SetCap( int capStyle )
{
  if (!m_refData)
    m_refData = new wxPenRefData();

  M_PENDATA->m_capStyle = capStyle;
}

void wxPen::SetJoin( int joinStyle )
{
  if (!m_refData)
    m_refData = new wxPenRefData();

  M_PENDATA->m_joinStyle = joinStyle;
}

void wxPen::SetStyle( int style )
{
  if (!m_refData)
    m_refData = new wxPenRefData();

  M_PENDATA->m_style = style;
}

void wxPen::SetWidth( int width )
{
  if (!m_refData)
    m_refData = new wxPenRefData();

  M_PENDATA->m_width = width;
}

int wxPen::GetCap(void) const
{
  return M_PENDATA->m_capStyle;
}

int wxPen::GetJoin(void) const
{
  if (!m_refData)
    return 0;
  else
    return M_PENDATA->m_joinStyle;
}

int wxPen::GetStyle(void) const
{
  if (!m_refData)
    return 0;
  else
    return M_PENDATA->m_style;
}

int wxPen::GetWidth(void) const
{
  if (!m_refData)
    return 0;
  else
    return M_PENDATA->m_width;
}

wxColour &wxPen::GetColour(void) const
{
  if (!m_refData)
    return wxNullColour;
  else
    return M_PENDATA->m_colour;
}

bool wxPen::Ok(void) const
{
  return (m_refData);
}

