/////////////////////////////////////////////////////////////////////////////
// Name:        colour.cpp
// Purpose:     wxColour class
// Author:      Julian Smart, Robert Roebling
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "colour.h"
#endif

#include "wx/gdicmn.h"
#include "wx/app.h"

#include "wx/x11/private.h"

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
        m_colormap = (WXColormap *) NULL;
        m_hasPixel = FALSE;
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
    void AllocColour( WXColormap cmap );

    XColor       m_color;
    WXColormap   m_colormap;
    bool         m_hasPixel;

    friend class wxColour;

    // reference counter for systems with <= 8-Bit display
    static unsigned short colMapAllocCounter[ 256 ];
};

unsigned short wxColourRefData::colMapAllocCounter[ 256 ] = 
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
#if 0        
    if (m_colormap)
    {
        Colormap cm = (Colormap)m_colormap;

        GdkColormapPrivate *private_colormap = (GdkColormapPrivate*) m_colormap;
        if ((private_colormap->visual->type == GDK_VISUAL_GRAYSCALE) ||
            (private_colormap->visual->type == GDK_VISUAL_PSEUDO_COLOR))
        {
            int idx = m_color.pixel;
            colMapAllocCounter[ idx ] = colMapAllocCounter[ idx ] - 1;
        
            if (colMapAllocCounter[ idx ] == 0)
            gdk_colormap_free_colors( m_colormap, &m_color, 1 );
        }
    }
#endif
}

void wxColourRefData::AllocColour( WXColormap cmap )
{
    if (m_hasPixel && (m_colormap == cmap))
        return;

    FreeColour();

#if 0    
    GdkColormapPrivate *private_colormap = (GdkColormapPrivate*) cmap;
    if ((private_colormap->visual->type == GDK_VISUAL_GRAYSCALE) ||
        (private_colormap->visual->type == GDK_VISUAL_PSEUDO_COLOR))
    {
        m_hasPixel = gdk_colormap_alloc_color( cmap, &m_color, FALSE, TRUE );
        int idx = m_color.pixel;
        colMapAllocCounter[ idx ] = colMapAllocCounter[ idx ] + 1;
    }
    else
#endif
    {
        m_hasPixel = XAllocColor( wxGlobalDisplay(), (Colormap) cmap, &m_color );
    }
    m_colormap = cmap;
}

//-----------------------------------------------------------------------------

#define M_COLDATA ((wxColourRefData *)m_refData)

#define SHIFT (8*(sizeof(short int)-sizeof(char)))

IMPLEMENT_DYNAMIC_CLASS(wxColour,wxGDIObject)

wxColour::wxColour( unsigned char red, unsigned char green, unsigned char blue )
{
    m_refData = new wxColourRefData();
#if wxUSE_NANOX
    M_COLDATA->m_color.red = ((unsigned short)red) ;
    M_COLDATA->m_color.green = ((unsigned short)green) ;
    M_COLDATA->m_color.blue = ((unsigned short)blue) ;
#else    
    M_COLDATA->m_color.red = ((unsigned short)red) << SHIFT;
    M_COLDATA->m_color.green = ((unsigned short)green) << SHIFT;
    M_COLDATA->m_color.blue = ((unsigned short)blue) << SHIFT;
#endif
    M_COLDATA->m_color.pixel = 0;
}

void wxColour::InitFromName( const wxString &colourName )
{
    wxNode *node = (wxNode *) NULL;
    if ( (wxTheColourDatabase) && (node = wxTheColourDatabase->Find(colourName)) )
    {
        wxColour *col = (wxColour*)node->Data();
        UnRef();
        if (col) Ref( *col );
    }
    else
    {
        m_refData = new wxColourRefData();
        
        M_COLDATA->m_colormap = wxTheApp->GetMainColormap( wxGlobalDisplay() );
        
        if (!XParseColor( wxGlobalDisplay(), (Colormap) M_COLDATA->m_colormap, colourName.mb_str(), &M_COLDATA->m_color ))
        {
            // VZ: asserts are good in general but this one is triggered by
            //     calling wxColourDatabase::FindColour() with an
            //     unrecognized colour name and this can't be avoided from the
            //     user code, so don't give it here
            //
            //     a better solution would be to changed code in FindColour()

            //wxFAIL_MSG( wxT("wxColour: couldn't find colour") );

            delete m_refData;
            m_refData = (wxObjectRefData *) NULL;
        }
    }
}

wxColour::~wxColour()
{
}

bool wxColour::operator == ( const wxColour& col ) const
{
    if (m_refData == col.m_refData) return TRUE;

    if (!m_refData || !col.m_refData) return FALSE;

    XColor *own = &(((wxColourRefData*)m_refData)->m_color);
    XColor *other = &(((wxColourRefData*)col.m_refData)->m_color);
    if (own->red != other->red) return FALSE;
    if (own->blue != other->blue) return FALSE;
    if (own->green != other->green) return FALSE;

    return TRUE;
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
    
    m_refData = new wxColourRefData();
#if wxUSE_NANOX
    M_COLDATA->m_color.red = ((unsigned short)red) ;
    M_COLDATA->m_color.green = ((unsigned short)green) ;
    M_COLDATA->m_color.blue = ((unsigned short)blue) ;
#else
    M_COLDATA->m_color.red = ((unsigned short)red) << SHIFT;
    M_COLDATA->m_color.green = ((unsigned short)green) << SHIFT;
    M_COLDATA->m_color.blue = ((unsigned short)blue) << SHIFT;
#endif
    M_COLDATA->m_color.pixel = 0;
}

unsigned char wxColour::Red() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid colour") );

#if wxUSE_NANOX
    return (unsigned char) M_COLDATA->m_color.red ;
#else
    return (unsigned char)(M_COLDATA->m_color.red >> SHIFT);
#endif
}

unsigned char wxColour::Green() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid colour") );

#if wxUSE_NANOX
    return (unsigned char) M_COLDATA->m_color.green ;
#else
    return (unsigned char)(M_COLDATA->m_color.green >> SHIFT);
#endif
}

unsigned char wxColour::Blue() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid colour") );

#if wxUSE_NANOX
    return (unsigned char) M_COLDATA->m_color.blue ;
#else
    return (unsigned char)(M_COLDATA->m_color.blue >> SHIFT);
#endif
}

void wxColour::CalcPixel( WXColormap cmap )
{
    wxCHECK_RET( Ok(), wxT("invalid colour") );

    wxCHECK_RET( cmap, wxT("invalid colormap") );

    M_COLDATA->AllocColour( cmap );
}

unsigned long wxColour::GetPixel() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid colour") );

    return M_COLDATA->m_color.pixel;
}

WXColor *wxColour::GetColor() const
{
    wxCHECK_MSG( Ok(), (WXColor *) NULL, wxT("invalid colour") );

    return (WXColor*) &M_COLDATA->m_color;
}
