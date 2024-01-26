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

#include <vector>

//-----------------------------------------------------------------------------
// wxPalette
//-----------------------------------------------------------------------------

struct wxPaletteEntry
{
    unsigned char red, green, blue;
};

struct wxPaletteRefData : public wxGDIRefData
{
    wxPaletteRefData() = default;
    wxPaletteRefData(const wxPaletteRefData& other);

    std::vector<wxPaletteEntry> m_entries;
};

// We need to define the copy ctor explicitly because the base class copy
// ctor is deleted.
wxPaletteRefData::wxPaletteRefData(const wxPaletteRefData& palette)
    : wxGDIRefData()
{
    m_entries = palette.m_entries;
}

//-----------------------------------------------------------------------------

#define M_PALETTEDATA ((wxPaletteRefData *)m_refData)

wxIMPLEMENT_DYNAMIC_CLASS(wxPalette,wxGDIObject);

wxPalette::wxPalette()
{
    m_refData = nullptr;
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
        return M_PALETTEDATA->m_entries.size();

    return 0;
}

bool wxPalette::Create(int n,
                       const unsigned char *red,
                       const unsigned char *green,
                       const unsigned char *blue)
{
    UnRef();
    m_refData = new wxPaletteRefData();

    M_PALETTEDATA->m_entries.resize(n);

    int i = 0;
    for ( wxPaletteEntry& e : M_PALETTEDATA->m_entries )
    {
        e.red = red[i];
        e.green = green[i];
        e.blue = blue[i];

        ++i;
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

    int i = 0;
    for ( const wxPaletteEntry& e : M_PALETTEDATA->m_entries )
    {
        if ((d = 0.299 * abs(red - e.red) +
                 0.587 * abs(green - e.green) +
                 0.114 * abs(blue - e.blue)) < distance) {
            distance = d;
            closest = i;
        }

        ++i;
    }
    return closest;
}

bool wxPalette::GetRGB(int pixel,
                       unsigned char *red,
                       unsigned char *green,
                       unsigned char *blue) const
{
    if ( !m_refData )
        return false;

    if ( pixel < 0 || (unsigned)pixel >= M_PALETTEDATA->m_entries.size() )
        return false;

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
