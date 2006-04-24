/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/colour.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/colour.h"

#include "wx/gtk/private.h"

#include <gdk/gdk.h>

//-----------------------------------------------------------------------------
// wxColour
//-----------------------------------------------------------------------------

class wxColourRefData: public wxObjectRefData
{
public:
    wxColourRefData()
    {
        m_color.red = 0;
        m_color.green = 0;
        m_color.blue = 0;
        m_color.pixel = 0;
        m_colormap = NULL;
        m_hasPixel = false;
    }

    wxColourRefData(const wxColourRefData& data)
        : wxObjectRefData()
    {
        m_color = data.m_color;
        m_colormap = data.m_colormap;
        m_hasPixel = data.m_hasPixel;
    }

    ~wxColourRefData()
    {
        FreeColour();
    }

    void FreeColour();
    void AllocColour( GdkColormap* cmap );

    GdkColor     m_color;
    GdkColormap *m_colormap;
    bool         m_hasPixel;
};

void wxColourRefData::FreeColour()
{
    if (m_hasPixel)
    {
        gdk_colormap_free_colors(m_colormap, &m_color, 1);
    }
}

void wxColourRefData::AllocColour( GdkColormap *cmap )
{
    if (m_hasPixel && (m_colormap == cmap))
        return;

    FreeColour();

    m_hasPixel = gdk_colormap_alloc_color(cmap, &m_color, FALSE, TRUE);
    m_colormap = cmap;
}

//-----------------------------------------------------------------------------

#define M_COLDATA wx_static_cast(wxColourRefData*, m_refData)

// GDK's values are in 0..65535 range, our are in 0..255
#define SHIFT  8

IMPLEMENT_DYNAMIC_CLASS(wxColour,wxGDIObject)

wxColour::~wxColour()
{
}

bool wxColour::operator == ( const wxColour& col ) const
{
    if (m_refData == col.m_refData)
        return true;

    if (!m_refData || !col.m_refData)
        return false;

    const GdkColor& own = M_COLDATA->m_color;
    const GdkColor& other = wx_static_cast(wxColourRefData*, col.m_refData)->m_color;
    return own.red == other.red &&
           own.blue == other.blue &&
           own.green == other.green;
}

wxObjectRefData *wxColour::CreateRefData() const
{
    return new wxColourRefData;
}

wxObjectRefData *wxColour::CloneRefData(const wxObjectRefData *data) const
{
    return new wxColourRefData(*(wxColourRefData *)data);
}

void wxColour::InitWith( unsigned char red, unsigned char green, unsigned char blue )
{
    AllocExclusive();

    M_COLDATA->m_color.red = ((unsigned short)red) << SHIFT;
    M_COLDATA->m_color.green = ((unsigned short)green) << SHIFT;
    M_COLDATA->m_color.blue = ((unsigned short)blue) << SHIFT;
    M_COLDATA->m_color.pixel = 0;

    M_COLDATA->m_colormap = NULL;
    M_COLDATA->m_hasPixel = false;
}

unsigned char wxColour::Red() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid colour") );

    return (unsigned char)(M_COLDATA->m_color.red >> SHIFT);
}

unsigned char wxColour::Green() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid colour") );

    return (unsigned char)(M_COLDATA->m_color.green >> SHIFT);
}

unsigned char wxColour::Blue() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid colour") );

    return (unsigned char)(M_COLDATA->m_color.blue >> SHIFT);
}

void wxColour::CalcPixel( GdkColormap *cmap )
{
    if (!Ok()) return;

    M_COLDATA->AllocColour( cmap );
}

int wxColour::GetPixel() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid colour") );

    return M_COLDATA->m_color.pixel;
}

GdkColor *wxColour::GetColor() const
{
    wxCHECK_MSG( Ok(), NULL, wxT("invalid colour") );

    return &M_COLDATA->m_color;
}

bool wxColour::FromString(const wxChar *str)
{
    GdkColor colGDK;
    if ( gdk_color_parse( wxGTK_CONV_SYS( str ), &colGDK ) )
    {
        UnRef();

        m_refData = new wxColourRefData;
        M_COLDATA->m_color = colGDK;
        return true;
    }

    return wxColourBase::FromString(str);
}
