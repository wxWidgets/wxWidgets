/////////////////////////////////////////////////////////////////////////////
// Name:        pen.cpp
// Purpose:     wxPen
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "pen.h"
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
#include "wx/pen.h"
#endif

#include "wx/msw/private.h"
#include "assert.h"

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxPen, wxGDIObject)
#endif

wxPenRefData::wxPenRefData(void)
{
//  m_stipple = NULL ;
  m_style = wxSOLID;
  m_width = 1;
  m_join = wxJOIN_ROUND ;
  m_cap = wxCAP_ROUND ;
  m_nbDash = 0 ;
  m_dash = 0 ;
  m_hPen = 0;
}

wxPenRefData::~wxPenRefData(void)
{
	if ( m_hPen )
		::DeleteObject((HPEN) m_hPen);
}

// Pens

wxPen::wxPen(void)
{
  if ( wxThePenList )
    wxThePenList->AddPen(this);
}

wxPen::~wxPen()
{
    if (wxThePenList)
        wxThePenList->RemovePen(this);
}

// Should implement Create
wxPen::wxPen(const wxColour& col, const int Width, const int Style)
{
  m_refData = new wxPenRefData;

  M_PENDATA->m_colour = col;
//  M_PENDATA->m_stipple = NULL;
  M_PENDATA->m_width = Width;
  M_PENDATA->m_style = Style;
  M_PENDATA->m_join = wxJOIN_ROUND ;
  M_PENDATA->m_cap = wxCAP_ROUND ;
  M_PENDATA->m_nbDash = 0 ;
  M_PENDATA->m_dash = 0 ;
  M_PENDATA->m_hPen = 0 ;

#ifndef __WIN32__
  // In Windows, only a pen of width = 1 can be dotted or dashed!
  if ((Style == wxDOT) || (Style == wxLONG_DASH) ||
      (Style == wxSHORT_DASH) || (Style == wxDOT_DASH) ||
      (Style == wxUSER_DASH))
    M_PENDATA->m_width = 1;
#else
/***
  DWORD vers = GetVersion() ;
  WORD  high = HIWORD(vers) ; // high bit=0 for NT, 1 for Win32s
  // Win32s doesn't support wide dashed pens

  if ((high&0x8000)!=0)
***/
  if (wxGetOsVersion()==wxWIN32S)
  {
    // In Windows, only a pen of width = 1 can be dotted or dashed!
    if ((Style == wxDOT) || (Style == wxLONG_DASH) ||
        (Style == wxSHORT_DASH) || (Style == wxDOT_DASH) ||
        (Style == wxUSER_DASH))
      M_PENDATA->m_width = 1;
  }
#endif 
  RealizeResource();

  if ( wxThePenList )
    wxThePenList->AddPen(this);
}

wxPen::wxPen(const wxBitmap& stipple, const int Width)
{
  m_refData = new wxPenRefData;

//  M_PENDATA->m_colour = col;
  M_PENDATA->m_stipple = stipple;
  M_PENDATA->m_width = Width;
  M_PENDATA->m_style = wxSTIPPLE;
  M_PENDATA->m_join = wxJOIN_ROUND ;
  M_PENDATA->m_cap = wxCAP_ROUND ;
  M_PENDATA->m_nbDash = 0 ;
  M_PENDATA->m_dash = 0 ;
  M_PENDATA->m_hPen = 0 ;

  RealizeResource();

  if ( wxThePenList )
    wxThePenList->AddPen(this);
}

wxPen::wxPen(const wxString& col, const int Width, const int Style)
{
  m_refData = new wxPenRefData;

  M_PENDATA->m_colour = col;
//  M_PENDATA->m_stipple = NULL ;
  M_PENDATA->m_width = Width;
  M_PENDATA->m_style = Style;
  M_PENDATA->m_join = wxJOIN_ROUND ;
  M_PENDATA->m_cap = wxCAP_ROUND ;
  M_PENDATA->m_nbDash = 0 ;
  M_PENDATA->m_dash = 0 ;
  M_PENDATA->m_hPen = 0 ;

  RealizeResource();

  if ( wxThePenList )
    wxThePenList->AddPen(this);
}

bool wxPen::RealizeResource(void)
{
  if (M_PENDATA && (M_PENDATA->m_hPen == 0))
  {
    if (M_PENDATA->m_style==wxTRANSPARENT)
    {
      M_PENDATA->m_hPen = (WXHPEN) ::GetStockObject(NULL_PEN);
      return TRUE;
    }

    COLORREF ms_colour = 0 ;
    ms_colour = M_PENDATA->m_colour.GetPixel() ;

    // Join style, Cap style, Pen Stippling only on Win32.
    // Currently no time to find equivalent on Win3.1, sorry
    // [if such equiv exist!!]
#ifdef __WIN32__
    if (M_PENDATA->m_join==wxJOIN_ROUND        &&
        M_PENDATA->m_cap==wxCAP_ROUND          &&
        M_PENDATA->m_style!=wxUSER_DASH        &&
        M_PENDATA->m_style!=wxSTIPPLE          &&
        M_PENDATA->m_width <= 1
       )
      M_PENDATA->m_hPen = (WXHPEN) CreatePen(wx2msPenStyle(M_PENDATA->m_style), M_PENDATA->m_width, ms_colour);
    else
    {
      DWORD ms_style = PS_GEOMETRIC|wx2msPenStyle(M_PENDATA->m_style) ;

      LOGBRUSH logb ;

      switch(M_PENDATA->m_join)
      {
        case wxJOIN_BEVEL: ms_style |= PS_JOIN_BEVEL ; break ;
        case wxJOIN_MITER: ms_style |= PS_JOIN_MITER ; break ;
        default:
        case wxJOIN_ROUND: ms_style |= PS_JOIN_ROUND ; break ;
      }

      switch(M_PENDATA->m_cap)
      {
        case wxCAP_PROJECTING: ms_style |= PS_ENDCAP_SQUARE ; break ;
        case wxCAP_BUTT:       ms_style |= PS_ENDCAP_FLAT ;   break ;
        default:
        case wxCAP_ROUND:      ms_style |= PS_ENDCAP_ROUND ;  break ;
      }

      switch(M_PENDATA->m_style)
      {
        case wxSTIPPLE:
          logb.lbStyle = BS_PATTERN ;
          if (M_PENDATA->m_stipple.Ok())
            logb.lbHatch = (LONG)M_PENDATA->m_stipple.GetHBITMAP() ;
          else
            logb.lbHatch = (LONG)0 ;
        break ;
        case wxBDIAGONAL_HATCH:
          logb.lbStyle = BS_HATCHED ;
          logb.lbHatch = HS_BDIAGONAL ;
        break ;
        case wxCROSSDIAG_HATCH:
          logb.lbStyle = BS_HATCHED ;
          logb.lbHatch = HS_DIAGCROSS ;
        break ;
        case wxFDIAGONAL_HATCH:
          logb.lbStyle = BS_HATCHED ;
          logb.lbHatch = HS_FDIAGONAL ;
        break ;
        case wxCROSS_HATCH:
          logb.lbStyle = BS_HATCHED ;
          logb.lbHatch = HS_CROSS ;
        break ;
        case wxHORIZONTAL_HATCH:
          logb.lbStyle = BS_HATCHED ;
          logb.lbHatch = HS_HORIZONTAL ;
        break ;
        case wxVERTICAL_HATCH:
          logb.lbStyle = BS_HATCHED ;
          logb.lbHatch = HS_VERTICAL ;
        break ;
        default:
          logb.lbStyle = BS_SOLID ;
        break ;
      }
      logb.lbColor = ms_colour ;
      wxDash *real_dash ;
      if (M_PENDATA->m_style==wxUSER_DASH && M_PENDATA->m_nbDash && M_PENDATA->m_dash)
      {
        real_dash = new wxDash[M_PENDATA->m_nbDash] ;
        int i;
        for (i=0;i<M_PENDATA->m_nbDash;i++)
          real_dash[i] = M_PENDATA->m_dash[i] * M_PENDATA->m_width ;
      }
      else
        real_dash = 0 ;

      // Win32s doesn't have ExtCreatePen function...
      if (wxGetOsVersion()==wxWINDOWS_NT || wxGetOsVersion()==wxWIN95)
        M_PENDATA->m_hPen = (WXHPEN) ExtCreatePen(ms_style,M_PENDATA->m_width,&logb,
                            M_PENDATA->m_style==wxUSER_DASH ? M_PENDATA->m_nbDash:0, (const DWORD *)real_dash);
      else
        M_PENDATA->m_hPen = (WXHPEN) CreatePen(wx2msPenStyle(M_PENDATA->m_style), M_PENDATA->m_width, ms_colour);

      if (real_dash)
        delete [] real_dash ;
    }
#else
    M_PENDATA->m_hPen = (WXHPEN) CreatePen(wx2msPenStyle(M_PENDATA->m_style), M_PENDATA->m_width, ms_colour);
#endif
#ifdef DEBUG_CREATE
    if (M_PENDATA->m_hPen==0)
      wxError("Cannot create pen","Internal error") ;
#endif
    return TRUE;
  }
  return FALSE;
}

WXHANDLE wxPen::GetResourceHandle(void)
{
	if ( !M_PENDATA )
		return 0;
	else
  		return (WXHANDLE)M_PENDATA->m_hPen;
}

bool wxPen::FreeResource(bool force)
{
  if (M_PENDATA && (M_PENDATA->m_hPen != 0))
  {
    DeleteObject((HPEN) M_PENDATA->m_hPen);
    M_PENDATA->m_hPen = 0;
    return TRUE;
  }
  else return FALSE;
}

/*
bool wxPen::UseResource(void)
{
  IncrementResourceUsage();
  return TRUE;
}

bool wxPen::ReleaseResource(void)
{
  DecrementResourceUsage();
  return TRUE;
}
*/

bool wxPen::IsFree(void)
{
  return (M_PENDATA && M_PENDATA->m_hPen == 0);
}

void wxPen::SetColour(const wxColour& col)
{
  if ( !M_PENDATA )
	m_refData = new wxPenRefData;

  M_PENDATA->m_colour = col;
  
  if (FreeResource())
    RealizeResource();
}

void wxPen::SetColour(const wxString& col)
{
  if ( !M_PENDATA )
	m_refData = new wxPenRefData;

  M_PENDATA->m_colour = col;
  
  if (FreeResource())
    RealizeResource();
}

void wxPen::SetColour(const unsigned char r, const unsigned char g, const unsigned char b)
{
  if ( !M_PENDATA )
	m_refData = new wxPenRefData;

  M_PENDATA->m_colour.Set(r, g, b);
  
  if (FreeResource())
    RealizeResource();
}

void wxPen::SetWidth(const int Width)
{
  if ( !M_PENDATA )
	m_refData = new wxPenRefData;

  M_PENDATA->m_width = Width;

  if (FreeResource())
    RealizeResource();
}

void wxPen::SetStyle(const int Style)
{
  if ( !M_PENDATA )
	m_refData = new wxPenRefData;

  M_PENDATA->m_style = Style;

  if (FreeResource())
    RealizeResource();
}

void wxPen::SetStipple(const wxBitmap& Stipple)
{
  if ( !M_PENDATA )
	m_refData = new wxPenRefData;

  M_PENDATA->m_stipple = Stipple;
  M_PENDATA->m_style = wxSTIPPLE;
  
  if (FreeResource())
    RealizeResource();
}

void wxPen::SetDashes(const int nb_dashes, const wxDash *Dash)
{
  if ( !M_PENDATA )
	m_refData = new wxPenRefData;

  M_PENDATA->m_nbDash = nb_dashes;
  M_PENDATA->m_dash = (wxDash *)Dash;
  
  if (FreeResource())
    RealizeResource();
}

void wxPen::SetJoin(const int Join)
{
  if ( !M_PENDATA )
	m_refData = new wxPenRefData;

  M_PENDATA->m_join = Join;

  if (FreeResource())
    RealizeResource();
}

void wxPen::SetCap(const int Cap)
{
  if ( !M_PENDATA )
	m_refData = new wxPenRefData;

  M_PENDATA->m_cap = Cap;

  if (FreeResource())
    RealizeResource();
}

int wx2msPenStyle(int wx_style)
{
  int cstyle;
/***
#ifdef __WIN32__
  DWORD vers = GetVersion() ;
  WORD  high = HIWORD(vers) ; // high bit=0 for NT, 1 for Win32s
#endif
***/
  switch (wx_style)
  {
    case wxDOT:
      cstyle = PS_DOT;
      break;
    case wxSHORT_DASH:
    case wxLONG_DASH:
      cstyle = PS_DASH;
      break;
    case wxTRANSPARENT:
      cstyle = PS_NULL;
      break;
    case wxUSER_DASH:
      // User dash style not supported on Win3.1, sorry...
#ifdef __WIN32__
      // Win32s doesn't have PS_USERSTYLE
/***
      if ((high&0x8000)==0)
***/
      if (wxGetOsVersion()==wxWINDOWS_NT)
        cstyle = PS_USERSTYLE ;
      else
        cstyle = PS_DOT ; // We must make a choice... This is mine!
#else
      cstyle = PS_DASH ;
#endif
      break ;
    case wxSOLID:
    default:
      cstyle = PS_SOLID;
      break;
  }
  return cstyle;
}

