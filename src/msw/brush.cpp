/////////////////////////////////////////////////////////////////////////////
// Name:        brush.cpp
// Purpose:     wxBrush
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "brush.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx/setup.h"
#include "wx/list.h"
#include "wx/utils.h"
#include "wx/app.h"
#include "wx/brush.h"
#endif

#include "wx/msw/private.h"

#include "assert.h"

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxBrush, wxGDIObject)
#endif

wxBrushRefData::wxBrushRefData(void)
{
  m_style = wxSOLID;
  m_hBrush = 0;
}

wxBrushRefData::wxBrushRefData(const wxBrushRefData& data)
{
  m_style = data.m_style;
  m_stipple = data.m_stipple;
  m_colour = data.m_colour;
  m_hBrush = 0;
}

wxBrushRefData::~wxBrushRefData(void)
{
	if ( m_hBrush )
		::DeleteObject((HBRUSH) m_hBrush);
}

// Brushes
wxBrush::wxBrush(void)
{
  if ( wxTheBrushList )
    wxTheBrushList->AddBrush(this);
}

wxBrush::~wxBrush()
{
    if (wxTheBrushList)
        wxTheBrushList->RemoveBrush(this);
}

wxBrush::wxBrush(const wxColour& col, int Style)
{
  m_refData = new wxBrushRefData;

  M_BRUSHDATA->m_colour = col;
  M_BRUSHDATA->m_style = Style;
  M_BRUSHDATA->m_hBrush = 0;

  RealizeResource();

  if ( wxTheBrushList )
    wxTheBrushList->AddBrush(this);
}

wxBrush::wxBrush(const wxBitmap& stipple)
{
  m_refData = new wxBrushRefData;

  M_BRUSHDATA->m_style = wxSTIPPLE;
  M_BRUSHDATA->m_stipple = stipple;
  M_BRUSHDATA->m_hBrush = 0;

  RealizeResource();

  if ( wxTheBrushList )
    wxTheBrushList->AddBrush(this);
}

bool wxBrush::RealizeResource(void) 
{
  if (M_BRUSHDATA && (M_BRUSHDATA->m_hBrush == 0))
  {
    if (M_BRUSHDATA->m_style==wxTRANSPARENT)
    {
      M_BRUSHDATA->m_hBrush = (WXHBRUSH) ::GetStockObject(NULL_BRUSH);
      return TRUE;
    }
    COLORREF ms_colour = 0 ;

    ms_colour = M_BRUSHDATA->m_colour.GetPixel() ;

    switch (M_BRUSHDATA->m_style)
    {
/****
    // Don't reset cbrush, wxTRANSPARENT is handled by wxBrush::SelectBrush()
    // this could save (many) time if frequently switching from
    // wxSOLID to wxTRANSPARENT, because Create... is not always called!!
    //
    // NB August 95: now create and select a Null brush instead.
    // This could be optimized as above.
    case wxTRANSPARENT:
      M_BRUSHDATA->m_hBrush = NULL;  // Must always select a suitable background brush
                      // - could choose white always for a quick solution
      break;
***/
      case wxBDIAGONAL_HATCH:
        M_BRUSHDATA->m_hBrush = (WXHBRUSH) CreateHatchBrush(HS_BDIAGONAL,ms_colour) ;
        break ;
      case wxCROSSDIAG_HATCH:
        M_BRUSHDATA->m_hBrush = (WXHBRUSH) CreateHatchBrush(HS_DIAGCROSS,ms_colour) ;
        break ;
      case wxFDIAGONAL_HATCH:
        M_BRUSHDATA->m_hBrush = (WXHBRUSH) CreateHatchBrush(HS_FDIAGONAL,ms_colour) ;
        break ;
      case wxCROSS_HATCH:
        M_BRUSHDATA->m_hBrush = (WXHBRUSH) CreateHatchBrush(HS_CROSS,ms_colour) ;
        break ;
      case wxHORIZONTAL_HATCH:
        M_BRUSHDATA->m_hBrush = (WXHBRUSH) CreateHatchBrush(HS_HORIZONTAL,ms_colour) ;
        break ;
      case wxVERTICAL_HATCH:
        M_BRUSHDATA->m_hBrush = (WXHBRUSH) CreateHatchBrush(HS_VERTICAL,ms_colour) ;
        break ;
      case wxSTIPPLE:
        if (M_BRUSHDATA->m_stipple.Ok())
          M_BRUSHDATA->m_hBrush = (WXHBRUSH) CreatePatternBrush((HBITMAP) M_BRUSHDATA->m_stipple.GetHBITMAP()) ;
        else
          M_BRUSHDATA->m_hBrush = (WXHBRUSH) CreateSolidBrush(ms_colour) ;
        break ;
      case wxSOLID:
      default:
        M_BRUSHDATA->m_hBrush = (WXHBRUSH) CreateSolidBrush(ms_colour) ;
        break;
    }
#ifdef WXDEBUG_CREATE
    if (M_BRUSHDATA->m_hBrush==NULL) wxError("Cannot create brush","Internal error") ;
#endif
    return TRUE;
  }
  else
    return FALSE;
}

WXHANDLE wxBrush::GetResourceHandle(void)
{
  return (WXHANDLE) M_BRUSHDATA->m_hBrush;
}

bool wxBrush::FreeResource(bool WXUNUSED(force))
{
  if (M_BRUSHDATA && (M_BRUSHDATA->m_hBrush != 0))
  {
    DeleteObject((HBRUSH) M_BRUSHDATA->m_hBrush);
    M_BRUSHDATA->m_hBrush = 0;
    return TRUE;
  }
  else return FALSE;
}

bool wxBrush::IsFree() const
{
  return (M_BRUSHDATA && (M_BRUSHDATA->m_hBrush == 0));
}

void wxBrush::Unshare()
{
	// Don't change shared data
	if (!m_refData)
    {
		m_refData = new wxBrushRefData();
	}
    else
    {
		wxBrushRefData* ref = new wxBrushRefData(*(wxBrushRefData*)m_refData);
		UnRef();
		m_refData = ref;
	}
}


void wxBrush::SetColour(const wxColour& col)
{
    Unshare();

    M_BRUSHDATA->m_colour = col;

    RealizeResource();
}

void wxBrush::SetColour(unsigned char r, unsigned char g, unsigned char b)
{
    Unshare();

    M_BRUSHDATA->m_colour.Set(r, g, b);

    RealizeResource();
}

void wxBrush::SetStyle(int Style)
{
    Unshare();

    M_BRUSHDATA->m_style = Style;

    RealizeResource();
}

void wxBrush::SetStipple(const wxBitmap& Stipple)
{
    Unshare();

    M_BRUSHDATA->m_stipple = Stipple;

    RealizeResource();
}


