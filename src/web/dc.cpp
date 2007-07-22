/////////////////////////////////////////////////////////////////////////////
// Name:        src/web/dc.cpp
// Purpose:     wxDC class
// Author:      Julian Smart
// Modified by: 
// Created:     01/02/97
// RCS-ID:      $Id: dc.cpp 45752 2007-05-02 11:05:45Z RR $
// Copyright:   (c) Julian Smart, John Wilmes
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/dc.h"

IMPLEMENT_ABSTRACT_CLASS(wxDC, wxDCBase)

//-----------------------------------------------------------------------------
// wxDC
//-----------------------------------------------------------------------------

wxDC::wxDC()
{
    m_ok = false;

    m_pen = *wxBLACK_PEN;
    m_font = *wxNORMAL_FONT;
    m_brush = *wxWHITE_BRUSH;

    m_backgroundMode = wxTRANSPARENT;
}

wxDC::~wxDC() {
}

void wxDC::DoSetClippingRegion(wxCoord x, wxCoord y,
                                     wxCoord width, wxCoord height) {
}

