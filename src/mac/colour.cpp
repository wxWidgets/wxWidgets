/////////////////////////////////////////////////////////////////////////////
// Name:        colour.cpp
// Purpose:     wxColour class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "colour.h"
#endif

#include "wx/gdicmn.h"
#include "wx/colour.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxColour, wxObject)
#endif

// Colour

static void wxComposeRGBColor( RGBColor * col , int red, int blue, int green ) ;
static void wxComposeRGBColor( RGBColor * col , int red, int blue, int green ) 
{
	col->red = (red << 8) + red;
	col->blue = (blue << 8) + blue;
	col->green = (green << 8) + green;
}

wxColour::wxColour ()
{
  m_isInit = FALSE;
  m_red = m_blue = m_green = 0;

	wxComposeRGBColor( &m_pixel , m_red , m_blue , m_green ) ;
}

wxColour::wxColour (unsigned char r, unsigned char g, unsigned char b)
{
    m_red = r;
    m_green = g;
    m_blue = b;
    m_isInit = TRUE;

	wxComposeRGBColor( &m_pixel , m_red , m_blue , m_green ) ;
}

wxColour::wxColour (const wxColour& col)
{
    m_red = col.m_red;
    m_green = col.m_green;
    m_blue = col.m_blue;
    m_isInit = col.m_isInit;

    m_pixel = col.m_pixel;
}

wxColour::wxColour (const wxColour* col)
{
    m_red = col->m_red;
    m_green = col->m_green;
    m_blue = col->m_blue;
    m_isInit = col->m_isInit;

    m_pixel = col->m_pixel;
}

wxColour& wxColour::operator =(const wxColour& col)
{
  m_red = col.m_red;
  m_green = col.m_green;
  m_blue = col.m_blue;
  m_isInit = col.m_isInit;
 
  m_pixel = col.m_pixel;

  return *this;
}

void wxColour::InitFromName(const wxString& col)
{
    wxColour *the_colour = wxTheColourDatabase->FindColour (col);
    if (the_colour)
    {
        m_red = the_colour->Red ();
        m_green = the_colour->Green ();
        m_blue = the_colour->Blue ();
        m_isInit = TRUE;
    }
    else
    {
        m_red = 0;
        m_green = 0;
        m_blue = 0;
        m_isInit = FALSE;
    }

	wxComposeRGBColor( &m_pixel , m_red , m_blue , m_green ) ;
}

wxColour::~wxColour ()
{
}

void wxColour::Set (unsigned char r, unsigned char g, unsigned char b)
{
    m_red = r;
    m_green = g;
    m_blue = b;
    m_isInit = TRUE;

	wxComposeRGBColor( &m_pixel , m_red , m_blue , m_green ) ;
}
