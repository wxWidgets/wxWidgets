/////////////////////////////////////////////////////////////////////////////
// Name:        pen.cpp
// Purpose:     wxPen
// Author:      David Webster
// Modified by:
// Created:     10/10/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx/setup.h"
#include "wx/list.h"
#include "wx/utils.h"
#include "wx/app.h"
#include "wx/pen.h"
#endif

#include "wx/os2/private.h"
#include "assert.h"

wxPenRefData::wxPenRefData()
{
    m_style = wxSOLID;
    m_width = 1;
    m_join = wxJOIN_ROUND ;
    m_cap = wxCAP_ROUND ;
    m_nbDash = 0 ;
    m_dash = 0 ;
    m_hPen = 0;
}

wxPenRefData::wxPenRefData(const wxPenRefData& data)
{
    m_style = data.m_style;
    m_width = data.m_width;
    m_join = data.m_join;
    m_cap = data.m_cap;
    m_nbDash = data.m_nbDash;
    m_dash = data.m_dash;
    m_colour = data.m_colour;
/* TODO: null data
    m_hPen = 0;
*/
}

wxPenRefData::~wxPenRefData()
{
    // TODO: delete data
}

// Pens

wxPen::wxPen()
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
wxPen::wxPen(const wxColour& col, int Width, int Style)
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

// TODO:
/*
    if ((Style == wxDOT) || (Style == wxLONG_DASH) ||
        (Style == wxSHORT_DASH) || (Style == wxDOT_DASH) ||
        (Style == wxUSER_DASH))
        M_PENDATA->m_width = 1;
*/
    RealizeResource();

    if ( wxThePenList )
        wxThePenList->AddPen(this);
}

wxPen::wxPen(const wxBitmap& stipple, int Width)
{
    m_refData = new wxPenRefData;

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

bool wxPen::RealizeResource()
{
    // TODO: create actual pen
    return FALSE;
}

WXHANDLE wxPen::GetResourceHandle()
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
// TODO:        DeleteObject((HPEN) M_PENDATA->m_hPen);
        M_PENDATA->m_hPen = 0;
        return TRUE;
    }
    else return FALSE;
}

bool wxPen::IsFree() const
{
  return (M_PENDATA && M_PENDATA->m_hPen == 0);
}

void wxPen::Unshare()
{
    // Don't change shared data
    if (!m_refData)
    {
        m_refData = new wxPenRefData();
    }
    else
    {
        wxPenRefData* ref = new wxPenRefData(*(wxPenRefData*)m_refData);
        UnRef();
        m_refData = ref;
    }
}

void wxPen::SetColour(const wxColour& col)
{
    Unshare();

    M_PENDATA->m_colour = col;

    RealizeResource();
}

void wxPen::SetColour(unsigned char r, unsigned char g, unsigned char b)
{
    Unshare();

    M_PENDATA->m_colour.Set(r, g, b);

    RealizeResource();
}

void wxPen::SetWidth(int Width)
{
    Unshare();

    M_PENDATA->m_width = Width;

    RealizeResource();
}

void wxPen::SetStyle(int Style)
{
    Unshare();

    M_PENDATA->m_style = Style;

    RealizeResource();
}

void wxPen::SetStipple(const wxBitmap& Stipple)
{
    Unshare();

    M_PENDATA->m_stipple = Stipple;
    M_PENDATA->m_style = wxSTIPPLE;

    RealizeResource();
}

void wxPen::SetDashes(int nb_dashes, const wxDash *Dash)
{
    Unshare();

    M_PENDATA->m_nbDash = nb_dashes;
    M_PENDATA->m_dash = (wxDash *)Dash;

    RealizeResource();
}

void wxPen::SetJoin(int Join)
{
    Unshare();

    M_PENDATA->m_join = Join;

    RealizeResource();
}

void wxPen::SetCap(int Cap)
{
    Unshare();

    M_PENDATA->m_cap = Cap;

    RealizeResource();
}

int wx2os2PenStyle(int wx_style)
{
    int cstyle;
// TODO:
/*
    switch (wx_style)
    {
       case wxDOT:
           cstyle = PS_DOT;
       break;

       case wxDOT_DASH:
	   cstyle = PS_DASHDOT;
	   break;

       case wxSHORT_DASH:
       case wxLONG_DASH:
           cstyle = PS_DASH;
	   break;

       case wxTRANSPARENT:
           cstyle = PS_NULL;
	   break;

       case wxUSER_DASH:
#ifdef __WIN32__
           // Win32s doesn't have PS_USERSTYLE
	   if (wxGetOsVersion()==wxWINDOWS_NT || wxGetOsVersion()==wxWIN95)
	       cstyle = PS_USERSTYLE;
	   else
               cstyle = PS_DOT; // We must make a choice... This is mine!
#else
           cstyle = PS_DASH;
#endif
           break;
       case wxSOLID:
       default:
           cstyle = PS_SOLID;
           break;
   }
*/
   return cstyle;
}


