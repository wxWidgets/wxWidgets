/////////////////////////////////////////////////////////////////////////////
// Name:        colour.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "colour.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/gdicmn.h"
#include "wx/colour.h"
#include "wx/gtk/private.h"

#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <gdk/gdkprivate.h>

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
        m_colormap = (GdkColormap *) NULL;
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

    bool operator == (const wxColourRefData& data) const
    {
        return (m_colormap == data.m_colormap &&
                m_hasPixel == data.m_hasPixel &&
                m_color.red == data.m_color.red &&
                m_color.green == data.m_color.green &&
                m_color.blue == data.m_color.blue &&
                m_color.pixel == data.m_color.pixel);
    }

    void FreeColour();
    void AllocColour( GdkColormap* cmap );

    GdkColor     m_color;
    GdkColormap *m_colormap;
    bool         m_hasPixel;

    friend class wxColour;

    // reference counter for systems with <= 8-Bit display
    static gushort colMapAllocCounter[ 256 ];
};

gushort wxColourRefData::colMapAllocCounter[ 256 ] =
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void wxColourRefData::FreeColour()
{
    if (m_colormap)
    {
#ifdef __WXGTK20__
        if ((m_colormap->visual->type == GDK_VISUAL_GRAYSCALE) ||
            (m_colormap->visual->type == GDK_VISUAL_PSEUDO_COLOR))
#else
        GdkColormapPrivate *private_colormap = (GdkColormapPrivate*) m_colormap;
        if ((private_colormap->visual->type == GDK_VISUAL_GRAYSCALE) ||
            (private_colormap->visual->type == GDK_VISUAL_PSEUDO_COLOR))
#endif
        {
            int idx = m_color.pixel;
            colMapAllocCounter[ idx ] = colMapAllocCounter[ idx ] - 1;

            if (colMapAllocCounter[ idx ] == 0)
            gdk_colormap_free_colors( m_colormap, &m_color, 1 );
        }
    }
}

void wxColourRefData::AllocColour( GdkColormap *cmap )
{
    if (m_hasPixel && (m_colormap == cmap))
        return;

    FreeColour();

#ifdef __WXGTK20__
    if ( (cmap->visual->type == GDK_VISUAL_GRAYSCALE) ||
	     (cmap->visual->type == GDK_VISUAL_PSEUDO_COLOR) )
#else
    GdkColormapPrivate *private_colormap = (GdkColormapPrivate*) cmap;
    if ((private_colormap->visual->type == GDK_VISUAL_GRAYSCALE) ||
        (private_colormap->visual->type == GDK_VISUAL_PSEUDO_COLOR))
#endif
    {
        m_hasPixel = gdk_colormap_alloc_color( cmap, &m_color, FALSE, TRUE );
        int idx = m_color.pixel;
        colMapAllocCounter[ idx ] = colMapAllocCounter[ idx ] + 1;
    }
    else
    {
        m_hasPixel = gdk_color_alloc( cmap, &m_color );
    }
    m_colormap = cmap;
}

//-----------------------------------------------------------------------------

#define M_COLDATA ((wxColourRefData *)m_refData)

// GDK's values are in 0..65535 range, our are in 0..255
#define SHIFT  8

IMPLEMENT_DYNAMIC_CLASS(wxColour,wxGDIObject)

wxColour::wxColour( unsigned char red, unsigned char green, unsigned char blue )
{
    m_refData = new wxColourRefData();
    M_COLDATA->m_color.red = ((unsigned short)red) << SHIFT;
    M_COLDATA->m_color.green = ((unsigned short)green) << SHIFT;
    M_COLDATA->m_color.blue = ((unsigned short)blue) << SHIFT;
    M_COLDATA->m_color.pixel = 0;
}

/* static */
wxColour wxColour::CreateByName(const wxString& name)
{
    wxColour col;

    GdkColor colGDK;
    if ( gdk_color_parse( wxGTK_CONV( name ), &colGDK ) )
    {
        wxColourRefData *refData = new wxColourRefData;
        refData->m_color = colGDK;
        col.m_refData = refData;
    }

    return col;
}


void wxColour::InitFromName( const wxString &colourName )
{
    // check the cache first
    wxColour col;
    if ( wxTheColourDatabase )
    {
        col = wxTheColourDatabase->Find(colourName);
    }

    if ( !col.Ok() )
    {
        col = CreateByName(colourName);
    }

    if ( col.Ok() )
    {
        *this = col;
    }
    else
    {
        wxFAIL_MSG( wxT("wxColour: couldn't find colour") );
    }
}

wxColour::~wxColour()
{
}

bool wxColour::operator == ( const wxColour& col ) const
{
    if (m_refData == col.m_refData)
        return true;

    if (!m_refData || !col.m_refData)
        return false;

    GdkColor *own = &(((wxColourRefData*)m_refData)->m_color);
    GdkColor *other = &(((wxColourRefData*)col.m_refData)->m_color);
    return own->red == other->red &&
                own->blue == other->blue &&
                     own->green == other->green;
}

wxObjectRefData *wxColour::CreateRefData() const
{
    return new wxColourRefData;
}

wxObjectRefData *wxColour::CloneRefData(const wxObjectRefData *data) const
{
    return new wxColourRefData(*(wxColourRefData *)data);
}

void wxColour::Set( unsigned char red, unsigned char green, unsigned char blue )
{
    AllocExclusive();

    M_COLDATA->m_color.red = ((unsigned short)red) << SHIFT;
    M_COLDATA->m_color.green = ((unsigned short)green) << SHIFT;
    M_COLDATA->m_color.blue = ((unsigned short)blue) << SHIFT;
    M_COLDATA->m_color.pixel = 0;

    M_COLDATA->m_colormap = (GdkColormap*) NULL;
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
    wxCHECK_MSG( Ok(), (GdkColor *) NULL, wxT("invalid colour") );

    return &M_COLDATA->m_color;
}


