/////////////////////////////////////////////////////////////////////////////
// Name:        pen.cpp
// Purpose:     wxPen
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "pen.h"
#endif

#include "wx/setup.h"
#include "wx/utils.h"
#include "wx/pen.h"

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxPen, wxGDIObject)
#endif

wxPenRefData::wxPenRefData()
{
    m_style = wxSOLID;
    m_width = 1;
    m_join = wxJOIN_ROUND ;
    m_cap = wxCAP_ROUND ;
    m_nbDash = 0 ;
    m_dash = 0 ;
/* TODO: null data
    m_hPen = 0;
*/
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
    M_PENDATA->m_width = Width;
    M_PENDATA->m_style = Style;
    M_PENDATA->m_join = wxJOIN_ROUND ;
    M_PENDATA->m_cap = wxCAP_ROUND ;
    M_PENDATA->m_nbDash = 0 ;
    M_PENDATA->m_dash = 0 ;

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

    RealizeResource();

    if ( wxThePenList )
        wxThePenList->AddPen(this);
}

wxPen::wxPen(const wxString& col, int Width, int Style)
{
    m_refData = new wxPenRefData;

    M_PENDATA->m_colour = col;
    M_PENDATA->m_width = Width;
    M_PENDATA->m_style = Style;
    M_PENDATA->m_join = wxJOIN_ROUND ;
    M_PENDATA->m_cap = wxCAP_ROUND ;
    M_PENDATA->m_nbDash = 0 ;
    M_PENDATA->m_dash = 0 ;

    RealizeResource();

    if ( wxThePenList )
        wxThePenList->AddPen(this);
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

void wxPen::SetColour(const wxString& col)
{
    Unshare();

    M_PENDATA->m_colour = col;
  
    RealizeResource();
}

void wxPen::SetColour(const unsigned char r, const unsigned char g, const unsigned char b)
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

bool wxPen::RealizeResource()
{
    // TODO: create actual pen
    return FALSE;
}


