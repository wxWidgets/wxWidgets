/////////////////////////////////////////////////////////////////////////////
// Name:        brush.cpp
// Purpose:     wxBrush
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "brush.h"
#endif

#include "wx/setup.h"
#include "wx/utils.h"
#include "wx/brush.h"

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxBrush, wxGDIObject)
#endif

#if 0 // WTF
wxBrushRefData::wxBrushRefData()
{
    m_style = wxSOLID;
// TODO: null data
}

wxBrushRefData::wxBrushRefData(const wxBrushRefData& data)
{
  m_style = data.m_style;
  m_stipple = data.m_stipple;
  m_colour = data.m_colour;
/* TODO: null data
  m_hBrush = 0;
*/
}

wxBrushRefData::~wxBrushRefData()
{
// TODO: delete data
}
#endif

// Brushes
wxBrush::wxBrush()
{
    if ( wxTheBrushList )
        wxTheBrushList->AddBrush(this);
}

wxBrush::~wxBrush()
{
    if ( wxTheBrushList )
        wxTheBrushList->RemoveBrush(this);
}

wxBrush::wxBrush(const wxColour& col, int Style)
{
    if ( wxTheBrushList )
        wxTheBrushList->AddBrush(this);
}

wxBrush::wxBrush(const wxBitmap& stipple)
{
        if ( wxTheBrushList )
        wxTheBrushList->AddBrush(this);
}

void wxBrush::Unshare()
{
}

void wxBrush::SetColour(const wxColour& col)
{
    Unshare();
    RealizeResource();
}

void wxBrush::SetColour(unsigned char r, unsigned char g, unsigned char b)
{
    Unshare();


    RealizeResource();
}

void wxBrush::SetStyle(int Style)
{
    Unshare();


    RealizeResource();
}

void wxBrush::SetStipple(const wxBitmap& Stipple)
{
    Unshare();


    RealizeResource();
}

bool wxBrush::RealizeResource()
{
// TODO: create the brush
    return FALSE;
}

int wxBrush::GetStyle() const
{
return 0;
}

wxColour& wxBrush::GetColour() const
{
    return *wxWHITE;
}

// vi:sts=4:sw=5:et
