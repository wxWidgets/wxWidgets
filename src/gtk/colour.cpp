/////////////////////////////////////////////////////////////////////////////
// Name:        colour.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "colour.h"
#endif

#include "wx/gdicmn.h"

#include "gdk/gdk.h"
#include "gdk/gdkprivate.h"

//-----------------------------------------------------------------------------
// wxColour
//-----------------------------------------------------------------------------

class wxColourRefData: public wxObjectRefData
{
  public:

    wxColourRefData();
    ~wxColourRefData();
    void FreeColour();

    GdkColor     m_color;
    GdkColormap *m_colormap;
    bool         m_hasPixel;

    friend wxColour;
};

wxColourRefData::wxColourRefData()
{
    m_color.red = 0;
    m_color.green = 0;
    m_color.blue = 0;
    m_color.pixel = 0;
    m_colormap = (GdkColormap *) NULL;
    m_hasPixel = FALSE;
}

wxColourRefData::~wxColourRefData()
{
    FreeColour();
}

void wxColourRefData::FreeColour()
{
//  if (m_hasPixel) gdk_colors_free( m_colormap, &m_color, 1, 0 );
}

//-----------------------------------------------------------------------------

#define M_COLDATA ((wxColourRefData *)m_refData)

#define SHIFT (8*(sizeof(short int)-sizeof(char)))

IMPLEMENT_DYNAMIC_CLASS(wxColour,wxGDIObject)

wxColour::wxColour()
{
}

wxColour::wxColour( unsigned char red, unsigned char green, unsigned char blue )
{
    m_refData = new wxColourRefData();
    M_COLDATA->m_color.red = ((unsigned short)red) << SHIFT;
    M_COLDATA->m_color.green = ((unsigned short)green) << SHIFT;
    M_COLDATA->m_color.blue = ((unsigned short)blue) << SHIFT;
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
        if (!gdk_color_parse( colourName, &M_COLDATA->m_color ))
        {
            wxFAIL_MSG( "wxColour: couldn't find colour" );
	    printf( "Colourname %s.\n", WXSTRINGCAST colourName );
      
            delete m_refData;
            m_refData = (wxObjectRefData *) NULL;
        }
    }
}

wxColour::wxColour( const wxColour& col )
{
    Ref( col );
}

wxColour::~wxColour()
{
}

wxColour& wxColour::operator = ( const wxColour& col )
{
    if (*this == col) return (*this);
    Ref( col );
    return *this;
}

bool wxColour::operator == ( const wxColour& col ) const
{
    return m_refData == col.m_refData;
}

bool wxColour::operator != ( const wxColour& col) const
{
    return m_refData != col.m_refData;
}

void wxColour::Set( unsigned char red, unsigned char green, unsigned char blue )
{
    UnRef();
    m_refData = new wxColourRefData();
    M_COLDATA->m_color.red = ((unsigned short)red) << SHIFT;
    M_COLDATA->m_color.green = ((unsigned short)green) << SHIFT;
    M_COLDATA->m_color.blue = ((unsigned short)blue) << SHIFT;
    M_COLDATA->m_color.pixel = 0;
}

unsigned char wxColour::Red() const
{
    wxCHECK_MSG( Ok(), 0, "invalid colour" );

    return (unsigned char)(M_COLDATA->m_color.red >> SHIFT);
}

unsigned char wxColour::Green() const
{
    wxCHECK_MSG( Ok(), 0, "invalid colour" );

    return (unsigned char)(M_COLDATA->m_color.green >> SHIFT);
}

unsigned char wxColour::Blue() const
{
    wxCHECK_MSG( Ok(), 0, "invalid colour" );

    return (unsigned char)(M_COLDATA->m_color.blue >> SHIFT);
}

bool wxColour::Ok() const
{
    return (m_refData != NULL);
}

void wxColour::CalcPixel( GdkColormap *cmap )
{
    if (!Ok()) return;

    if ((M_COLDATA->m_hasPixel) && (M_COLDATA->m_colormap == cmap)) return;
    
    M_COLDATA->FreeColour();

    GdkColormapPrivate *private_colormap = (GdkColormapPrivate*) cmap;
    if ((private_colormap->visual->type == GDK_VISUAL_GRAYSCALE) ||
        (private_colormap->visual->type == GDK_VISUAL_PSEUDO_COLOR))
    {
        GdkColor *colors = cmap->colors;
        int max = 3 * (65536);
        int index = -1;

        for (int i = 0; i < cmap->size; i++)
        {
            int rdiff = (M_COLDATA->m_color.red - colors[i].red);
            int gdiff = (M_COLDATA->m_color.green - colors[i].green);
            int bdiff = (M_COLDATA->m_color.blue - colors[i].blue);
            int sum = ABS (rdiff) + ABS (gdiff) + ABS (bdiff);
            if (sum < max) { index = i; max = sum; }
        }

        M_COLDATA->m_hasPixel = TRUE;
        M_COLDATA->m_color.pixel = index;
    }
    else
    { 
        M_COLDATA->m_hasPixel = gdk_color_alloc( cmap, &M_COLDATA->m_color );
    }

    M_COLDATA->m_colormap = cmap;
}

int wxColour::GetPixel() const
{
    wxCHECK_MSG( Ok(), 0, "invalid colour" );

    return M_COLDATA->m_color.pixel;
}

GdkColor *wxColour::GetColor() const
{
    wxCHECK_MSG( Ok(), (GdkColor *) NULL, "invalid colour" );

    return &M_COLDATA->m_color;
}


