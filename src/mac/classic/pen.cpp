/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/pen.cpp
// Purpose:     wxPen
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/pen.h"

#ifndef WX_PRECOMP
    #include "wx/utils.h"
#endif

IMPLEMENT_DYNAMIC_CLASS(wxPen, wxGDIObject)

wxPenRefData::wxPenRefData()
{
    m_style = wxSOLID;
    m_width = 1;
    m_join = wxJOIN_ROUND ;
    m_cap = wxCAP_ROUND ;
    m_nbDash = 0 ;
    m_dash = 0 ;
}

wxPenRefData::wxPenRefData(const wxPenRefData& data)
: wxGDIRefData()
{
    m_style = data.m_style;
    m_width = data.m_width;
    m_join = data.m_join;
    m_cap = data.m_cap;
    m_nbDash = data.m_nbDash;
    m_dash = data.m_dash;
    m_colour = data.m_colour;
}

wxPenRefData::~wxPenRefData()
{
}

// Pens

wxPen::wxPen()
{
}

wxPen::~wxPen()
{
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

bool wxPen::RealizeResource()
{
    // nothing to do here for mac
    return true;
}
