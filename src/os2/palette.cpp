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

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx/setup.h"
#include "wx/palette.h"
#endif

#define INCL_PM
#define INCL_GPI
#include <os2.h>

#include "assert.h"


/*
 * Palette
 *
 */

wxPaletteRefData::wxPaletteRefData()
{
    m_hPalette = 0;
}

wxPaletteRefData::~wxPaletteRefData()
{
    if ( m_hPalette )
        return;
        // TODO: ::DeleteObject((HPALETTE) m_hPalette);
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

bool wxPalette::FreeResource(bool force)
{
    if ( M_PALETTEDATA && M_PALETTEDATA->m_hPalette)
    {
// TODO:   DeleteObject((HPALETTE)M_PALETTEDATA->m_hPalette);
    }
    return TRUE;
}

bool wxPalette::Create(int n, const unsigned char *red, const unsigned char *green, const unsigned char *blue)
{
  UnRef();

  m_refData = new wxPaletteRefData;

  // TODO
/*
  NPLOGPALETTE npPal = (NPLOGPALETTE)LocalAlloc(LMEM_FIXED, sizeof(LOGPALETTE) +
                        (WORD)n * sizeof(PALETTEENTRY));
  if (!npPal)
    return(FALSE);

  npPal->palVersion = 0x300;
  npPal->palNumEntries = n;

  int i;
  for (i = 0; i < n; i ++)
  {
    npPal->palPalEntry[i].peRed = red[i];
    npPal->palPalEntry[i].peGreen = green[i];
    npPal->palPalEntry[i].peBlue = blue[i];
    npPal->palPalEntry[i].peFlags = 0;
  }
  M_PALETTEDATA->m_hPalette = (WXHPALETTE) CreatePalette((LPLOGPALETTE)npPal);
  LocalFree((HANDLE)npPal);
*/
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
/*
  PALETTEENTRY entry;
  if (::GetPaletteEntries((HPALETTE) M_PALETTEDATA->m_hPalette, index, 1, &entry))
  {
         *red = entry.peRed;
         *green = entry.peGreen;
         *blue = entry.peBlue;
         return TRUE;
  } else
         return FALSE;
*/
  return FALSE;
}

void wxPalette::SetHPALETTE(WXHPALETTE pal)
{
	if ( !m_refData )
		m_refData = new wxPaletteRefData;

    M_PALETTEDATA->m_hPalette = pal;
}


