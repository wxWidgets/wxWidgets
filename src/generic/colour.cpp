/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/colour.cpp
// Purpose:     wxColour class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/colour.h"

#ifndef WX_PRECOMP
    #include "wx/gdicmn.h"
#endif

// Colour

void wxColour::Init()
{
    m_red =
    m_blue =
    m_green = 0;
    m_alpha = wxALPHA_OPAQUE;
    m_isInit = false;
}

void wxColour::InitRGBA(unsigned char r,
                        unsigned char g,
                        unsigned char b,
                        unsigned char a)
{
    m_red = r;
    m_green = g;
    m_blue = b;
    m_alpha = a;
    m_isInit = true;
}

wxColour& wxColour::operator=(const wxColour& col)
{
    m_red = col.m_red;
    m_green = col.m_green;
    m_blue = col.m_blue;
    m_alpha = col.m_alpha;
    m_isInit = col.m_isInit;
    return *this;
}

