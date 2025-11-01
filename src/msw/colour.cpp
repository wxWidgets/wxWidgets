/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/colour.cpp
// Purpose:     wxColour class
// Author:      Julian Smart
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

#include "wx/msw/private.h"

#include <string.h>

// Colour

void wxColour::InitRGBA(unsigned char r, unsigned char g, unsigned char b,
                        unsigned char a)
{
    m_red = r;
    m_green = g;
    m_blue = b;
    m_alpha = a;
    m_isInit = true;
    m_pixel = PALETTERGB(m_red, m_green, m_blue);
}
