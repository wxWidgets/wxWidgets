/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/paletteg.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Copyright:   (c) 1998 Robert Roebling and Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_PALETTE

#include "wx/palette.h"

//-----------------------------------------------------------------------------
// wxPalette
//-----------------------------------------------------------------------------

struct wxPaletteEntry
{
    unsigned char red, green, blue;
};

class wxPaletteRefData : public wxGDIRefData
{
public:
    wxPaletteRefData();
    wxPaletteRefData(const wxPaletteRefData& palette);
    virtual ~wxPaletteRefData();

    int m_count;
    wxPaletteEntry *m_entries;
};

wxPaletteRefData::wxPaletteRefData()
{
    m_count = 0;
    m_entries = NULL;
}

wxPaletteRefData::wxPaletteRefData(const wxPaletteRefData& palette)
{
    m_count = palette.m_count;
    m_entries = new wxPaletteEntry[m_count];
    for ( int i = 0; i < m_count; i++ )
        m_entries[i] = palette.m_entries[i];
}

wxPaletteRefData::~wxPaletteRefData()
{
    delete[] m_entries;
}

//-----------------------------------------------------------------------------

#define M_PALETTEDATA ((wxPaletteRefData *)m_refData)

wxIMPLEMENT_DYNAMIC_CLASS(wxPalette,wxGDIObject);

wxPalette::wxPalette()
{
    m_refData = NULL;
}

wxPalette::wxPalette(int n, const unsigned char *red, const unsigned char *green, const unsigned char *blue)
{
    Create(n, red, green, blue);
}

wxPalette::~wxPalette()
{
}

int wxPalette::GetColoursCount() const
{
    if (m_refData)
        return M_PALETTEDATA->m_count;

    return 0;
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

    return true;
}

int wxPalette::GetPixel( unsigned char red,
                         unsigned char green,
                         unsigned char blue ) const
{
    if (!m_refData) return wxNOT_FOUND;

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
    if (!m_refData) return false;
    if (pixel >= M_PALETTEDATA->m_count) return false;

    wxPaletteEntry& p = M_PALETTEDATA->m_entries[pixel];
    if (red) *red = p.red;
    if (green) *green = p.green;
    if (blue) *blue = p.blue;
    return true;
}

wxGDIRefData *wxPalette::CreateGDIRefData() const
{
    return new wxPaletteRefData;
}

wxGDIRefData *wxPalette::CloneGDIRefData(const wxGDIRefData *data) const
{
    return new wxPaletteRefData(*static_cast<const wxPaletteRefData *>(data));
}

#endif // wxUSE_PALETTE
