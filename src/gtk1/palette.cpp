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

#include "gdk/gdk.h"

//-----------------------------------------------------------------------------
// wxPalette
//-----------------------------------------------------------------------------

class wxPaletteRefData: public wxObjectRefData
{
  public:

    wxPaletteRefData(void);
    ~wxPaletteRefData(void);

    GdkColormap  *m_colormap;
};

wxPaletteRefData::wxPaletteRefData()
{
    m_colormap = (GdkColormap *) NULL;
}

wxPaletteRefData::~wxPaletteRefData()
{
    if (m_colormap) gdk_colormap_unref( m_colormap );
}

//-----------------------------------------------------------------------------

#define M_PALETTEDATA ((wxPaletteRefData *)m_refData)

IMPLEMENT_DYNAMIC_CLASS(wxPalette,wxGDIObject)

wxPalette::wxPalette()
{
}

wxPalette::wxPalette( int n, const unsigned char *red, const unsigned char *green, const unsigned char *blue )
{
    m_refData = new wxPaletteRefData();
    Create( n, red, green, blue );
}

wxPalette::wxPalette( const wxPalette& palette )
{
    Ref( palette );
}

wxPalette::~wxPalette()
{
}

wxPalette& wxPalette::operator = ( const wxPalette& palette )
{
    if (*this == palette) return (*this);
    Ref( palette );
    return *this;
}

bool wxPalette::operator == ( const wxPalette& palette )
{
    return m_refData == palette.m_refData;
}

bool wxPalette::operator != ( const wxPalette& palette )
{
    return m_refData != palette.m_refData;
}

bool wxPalette::Ok(void) const
{
    return (m_refData != NULL);
}

bool wxPalette::Create( int WXUNUSED(n), 
                        const unsigned char *WXUNUSED(red), 
			const unsigned char *WXUNUSED(green), 
			const unsigned char *WXUNUSED(blue) )
{
    wxFAIL_MSG(_T("not implemented"));

    return FALSE;
}

int wxPalette::GetPixel( const unsigned char WXUNUSED(red), 
                         const unsigned char WXUNUSED(green), 
			 const unsigned char WXUNUSED(blue) ) const
{
    wxFAIL_MSG(_T("not implemented"));

    return 0;
}

bool wxPalette::GetRGB( int WXUNUSED(pixel), 
                        unsigned char *WXUNUSED(red), 
			unsigned char *WXUNUSED(green), 
			unsigned char *WXUNUSED(blue) ) const
{
    wxFAIL_MSG(_T("not implemented"));
 
    return 0;
}

