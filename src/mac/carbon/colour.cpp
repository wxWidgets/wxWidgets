/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/colour.cpp
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

#include "wx/mac/private.h"

IMPLEMENT_DYNAMIC_CLASS(wxColour, wxObject)

wxColour::wxColour(const RGBColor& col)
{
    InitRGBColor(col);
}

wxColour::wxColour(CGColorRef col)
{
    InitCGColorRef(col);
}

void wxColour::GetRGBColor( RGBColor *col ) const
{
    col->red = (m_red << 8) + m_red;
    col->blue = (m_blue << 8) + m_blue;
    col->green = (m_green << 8) + m_green;
}

wxColour::~wxColour ()
{
}

wxColour& wxColour::operator=(const RGBColor& col)
{
    InitRGBColor(col);
    return *this;
}

wxColour& wxColour::operator=(CGColorRef col)
{
    InitCGColorRef(col);
    return *this;
}

void wxColour::InitRGBA (ChannelType r, ChannelType g, ChannelType b, ChannelType a)
{
    m_red = r;
    m_green = g;
    m_blue = b;
    m_alpha = a ;

    CGColorRef col = 0 ;
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5
    if ( CGColorCreateGenericRGB )
        col = CGColorCreateGenericRGB( r / 255.0, g / 255.0, b / 255.0, a / 255.0 );
    else
#endif
    {
        CGFloat components[4] = { r / 255.0, g / 255.0, b / 255.0, a / 255.0 } ;    
        col = CGColorCreate( wxMacGetGenericRGBColorSpace() , components ) ;
    }
    m_cgColour.reset( col );
}

void wxColour::InitRGBColor( const RGBColor& col )
{
    m_red = col.red >> 8;
    m_blue = col.blue >> 8;
    m_green = col.green >> 8;
    m_alpha = wxALPHA_OPAQUE;
    CGColorRef cfcol;
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5
    if ( CGColorCreateGenericRGB )
        cfcol = CGColorCreateGenericRGB( col.red / 65535.0, col.green / 65535.0, col.blue / 65535.0, 1.0 );
    else
#endif
    {
        CGFloat components[4] = { col.red / 65535.0, col.green / 65535.0, col.blue / 65535.0, 1.0 } ;    
        cfcol = CGColorCreate( wxMacGetGenericRGBColorSpace() , components ) ;
    }
    m_cgColour.reset( cfcol );
}

void wxColour::InitCGColorRef( CGColorRef col )
{
    m_cgColour.reset( col );
    size_t noComp = CGColorGetNumberOfComponents( col );
    if ( noComp >=3 && noComp <= 4 )
    {
        // TODO verify whether we really are on a RGB color space
        const CGFloat *components = CGColorGetComponents( col );
        m_red = (int)(components[0]*255+0.5);
        m_green = (int)(components[1]*255+0.5);
        m_blue = (int)(components[2]*255+0.5);
        if ( noComp == 4 )
            m_alpha =  (int)(components[3]*255+0.5);
        else
            m_alpha = wxALPHA_OPAQUE;
    }
    else
    {
        m_alpha = wxALPHA_OPAQUE;
        m_red = m_green = m_blue = 0;
    }
}

bool wxColour::operator == (const wxColour& colour) const
{
    return ( (IsOk() == colour.IsOk()) && (!IsOk() ||
                                           CGColorEqualToColor( m_cgColour, colour.m_cgColour ) ) );
}


