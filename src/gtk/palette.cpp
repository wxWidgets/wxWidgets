/////////////////////////////////////////////////////////////////////////////
// Name:        palette.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "palette.h"
#endif

#include "wx/palette.h"

#include <gdk/gdk.h>

//-----------------------------------------------------------------------------
// wxPalette
//-----------------------------------------------------------------------------

struct wxPaletteEntry
{
    unsigned char red, green, blue;
};

class wxPaletteRefData: public wxObjectRefData
{
  public:

    wxPaletteRefData(void);
    ~wxPaletteRefData(void);

    int m_count;
    wxPaletteEntry *m_entries;
#if 0
    GdkColormap  *m_colormap;
#endif
};

wxPaletteRefData::wxPaletteRefData()
{
    m_count = 0;
    m_entries = NULL;
#if 0
    m_colormap = (GdkColormap *) NULL;
#endif
}

wxPaletteRefData::~wxPaletteRefData()
{
    delete[] m_entries;
#if 0
    if (m_colormap) gdk_colormap_unref( m_colormap );
#endif
}

//-----------------------------------------------------------------------------

#define M_PALETTEDATA ((wxPaletteRefData *)m_refData)

IMPLEMENT_DYNAMIC_CLASS(wxPalette,wxGDIObject)

wxPalette::wxPalette()
{
    m_refData = NULL;
}

wxPalette::wxPalette(int n, const unsigned char *red, const unsigned char *green, const unsigned char *blue)
{
    Create(n, red, green, blue);
}

wxPalette::wxPalette(const wxPalette& palette)
{
    Ref(palette);
}

wxPalette::~wxPalette()
{
}

wxPalette& wxPalette::operator = (const wxPalette& palette)
{
    if (*this == palette) return (*this);
    Ref(palette);
    return *this;
}

bool wxPalette::operator == (const wxPalette& palette)
{
    return m_refData == palette.m_refData;
}

bool wxPalette::operator != (const wxPalette& palette)
{
    return m_refData != palette.m_refData;
}

bool wxPalette::Ok(void) const
{
    return (m_refData != NULL);
}

bool wxPalette::Create(int n,
                       const unsigned char *red,
                       const unsigned char *green, 
                       const unsigned char *blue)
{
    UnRef();
    m_refData = new wxPaletteRefData();
    
    M_PALETTEDATA->m_count = n;    
    M_PALETTEDATA->m_entries = new wxPaletteEntry[n];

    wxPaletteEntry *e = M_PALETTEDATA->m_entries;
    for (int i = 0; i < n; i++, e++)
    {
        e->red = red[i];
        e->green = green[i];
        e->blue = blue[i];
    }

    return TRUE;
}

int wxPalette::GetPixel( const unsigned char red,
                         const unsigned char green,
                         const unsigned char blue ) const
{
    if (!m_refData) return FALSE;

	int closest = 0;
	double d,distance = 1000.0; // max. dist is 256

    wxPaletteEntry *e = M_PALETTEDATA->m_entries;
    for (int i = 0; i < M_PALETTEDATA->m_count; i++, e++)
    {
        if ((d = 0.299 * abs(red - e->red) +
                 0.587 * abs(green - e->green) +
                 0.114 * abs(blue - e->blue)) < distance) {
            distance = d;
            closest = i;
        }
    }
	return closest;
}

bool wxPalette::GetRGB(int pixel, 
                       unsigned char *red,
                       unsigned char *green, 
                       unsigned char *blue) const
{
    if (!m_refData) return FALSE;
    if (pixel >= M_PALETTEDATA->m_count) return FALSE;
    
    wxPaletteEntry& p = M_PALETTEDATA->m_entries[pixel];
    if (red) *red = p.red;
    if (green) *green = p.green;
    if (blue) *blue = p.blue;
    return TRUE;
}


