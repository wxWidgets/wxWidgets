/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/colour.cpp
// Purpose:     wxColour class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/colour.h"

#ifndef WX_PRECOMP
    #include "wx/gdicmn.h"
#endif

IMPLEMENT_DYNAMIC_CLASS(wxColour, wxObject)

// Colour

#include "wx/mac/private.h"

static void wxComposeRGBColor( WXCOLORREF* color , int red, int blue, int green ) ;
static void wxComposeRGBColor( WXCOLORREF* color , int red, int blue, int green )
{
    RGBColor* col = (RGBColor*) color ;
    col->red = (red << 8) + red;
    col->blue = (blue << 8) + blue;
    col->green = (green << 8) + green;
}

void wxColour::Init()
{
    m_isInit = false;
    m_red =
    m_blue =
    m_green = 0;

    wxComposeRGBColor( &m_pixel , m_red , m_blue , m_green ) ;
}

wxColour::wxColour (const wxColour& col)
    : wxObject()
{
    m_red = col.m_red;
    m_green = col.m_green;
    m_blue = col.m_blue;
    m_isInit = col.m_isInit;

    memcpy( &m_pixel , &col.m_pixel , 6 ) ;
}

wxColour::wxColour (const wxColour* col)
{
    m_red = col->m_red;
    m_green = col->m_green;
    m_blue = col->m_blue;
    m_isInit = col->m_isInit;

    memcpy( &m_pixel , &col->m_pixel , 6 ) ;
}

wxColour& wxColour::operator =(const wxColour& col)
{
    m_red = col.m_red;
    m_green = col.m_green;
    m_blue = col.m_blue;
    m_isInit = col.m_isInit;

    memcpy( &m_pixel , &col.m_pixel , 6 ) ;

    return *this;
}

wxColour::~wxColour ()
{
}

void wxColour::InitWith (unsigned char r, unsigned char g, unsigned char b)
{
    m_red = r;
    m_green = g;
    m_blue = b;
    m_isInit = true;

    wxComposeRGBColor( &m_pixel , m_red , m_blue , m_green ) ;
}

void wxColour::Set( const WXCOLORREF* color )
{
    RGBColor* col = (RGBColor*) color ;
    memcpy( &m_pixel , color , 6 ) ;
    m_red = col->red>>8 ;
    m_blue = col->blue>>8 ;
    m_green = col->green>>8 ;
}
