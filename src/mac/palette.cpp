/////////////////////////////////////////////////////////////////////////////
// Name:        palette.cpp
// Purpose:     wxPalette
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "palette.h"
#endif

#include "wx/palette.h"

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxPalette, wxGDIObject)
#endif

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
	delete[] m_palette ;
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

  return FALSE;
}

int wxPalette::GetPixel(const unsigned char red, const unsigned char green, const unsigned char blue) const
{
    if ( !m_refData )
  	return -1;

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
	    return FALSE;

    if (index < 0 || index >= M_PALETTEDATA->m_count)
        return FALSE;

    const wxColour& col = &M_PALETTEDATA->m_palette[index] ;
    *red = col.Red() ;
    *green = col.Green() ;
    *blue = col.Blue() ;
    
    return TRUE;
}


