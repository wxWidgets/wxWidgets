/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/palette.cpp
// Purpose:     wxPalette
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_PALETTE

#include "wx/palette.h"

IMPLEMENT_DYNAMIC_CLASS(wxPalette, wxGDIObject)

/*
 * Palette
 *
 */

wxPaletteRefData::wxPaletteRefData()
{
    m_palette = NULL ;
    m_count = 0 ;
}

wxPaletteRefData::~wxPaletteRefData()
{
    if (m_palette != NULL) {
        delete[] m_palette ;
        m_palette = NULL;
    }
}

wxPalette::wxPalette()
{
}

wxPalette::wxPalette(int n, const unsigned char *red, const unsigned char *green, const unsigned char *blue)
{
    Create(n, red, green, blue);
}

wxPalette::~wxPalette()
{
}

bool wxPalette::Create(int n, const unsigned char *red, const unsigned char *green, const unsigned char *blue)
{
    UnRef();

    m_refData = new wxPaletteRefData;

    M_PALETTEDATA->m_count = n ;
    M_PALETTEDATA->m_palette = new wxColour[n] ;

    for ( int i = 0 ; i < n ; ++i)
    {
        M_PALETTEDATA->m_palette[i].Set( red[i] , green[i] , blue[i] ) ;
    }

    return false;
}

int wxPalette::GetPixel(unsigned char red, unsigned char green, unsigned char blue) const
{
    if ( !m_refData )
        return wxNOT_FOUND;

    long bestdiff = 3 * 256 ;
    long bestpos = 0 ;
    long currentdiff ;

    for ( int i = 0  ; i < M_PALETTEDATA->m_count ; ++i )
    {
        const wxColour& col = &M_PALETTEDATA->m_palette[i] ;
        currentdiff = abs ( col.Red() - red ) + abs( col.Green() - green ) + abs ( col.Blue() - blue )  ;
        if ( currentdiff < bestdiff )
        {
            bestdiff = currentdiff ;
            bestpos = i ;
            if ( bestdiff == 0 )
                break ;
        }
    }

    return bestpos;
}

bool wxPalette::GetRGB(int index, unsigned char *red, unsigned char *green, unsigned char *blue) const
{
    if ( !m_refData )
        return false;

    if (index < 0 || index >= M_PALETTEDATA->m_count)
        return false;

    const wxColour& col = &M_PALETTEDATA->m_palette[index] ;
    *red = col.Red() ;
    *green = col.Green() ;
    *blue = col.Blue() ;

    return true;
}

#endif
// wxUSE_PALETTE
