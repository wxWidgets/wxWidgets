/////////////////////////////////////////////////////////////////////////////
// Name:        palette.cpp
// Purpose:     wxPalette
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
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
    m_colormap = (WXColormap) 0;
}

wxPaletteRefData::~wxPaletteRefData()
{
    // TODO
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

  // TODO

  return FALSE;
}

int wxPalette::GetPixel(const unsigned char red, const unsigned char green, const unsigned char blue) const
{
    if ( !m_refData )
  	return FALSE;

    // TODO
    return FALSE;
}

bool wxPalette::GetRGB(int index, unsigned char *red, unsigned char *green, unsigned char *blue) const
{
    if ( !m_refData )
	    return FALSE;

    if (index < 0 || index > 255)
        return FALSE;

    // TODO
    return FALSE;
}


