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

wxColour::wxColour ()
{
    m_isInit = FALSE;
    m_red = m_blue = m_green = 0;
/* TODO
    m_pixel = 0;
*/
}

wxColour::wxColour (unsigned char r, unsigned char g, unsigned char b)
{
    m_red = r;
    m_green = g;
    m_blue = b;
    m_isInit = TRUE;
/* TODO
    m_pixel = PALETTERGB (m_red, m_green, m_blue);
*/
}

wxColour::wxColour (const wxColour& col)
{
    m_red = col.m_red;
    m_green = col.m_green;
    m_blue = col.m_blue;
    m_isInit = col.m_isInit;
/* TODO
    m_pixel = col.m_pixel;
*/
}

wxColour& wxColour::operator =(const wxColour& col)
{
  m_red = col.m_red;
  m_green = col.m_green;
  m_blue = col.m_blue;
  m_isInit = col.m_isInit;
/* TODO
  m_pixel = col.m_pixel;
*/
  return *this;
}

wxColour::wxColour (const wxString& col)
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
/* TODO
    m_pixel = PALETTERGB (m_red, m_green, m_blue);
*/
}

wxColour::~wxColour ()
{
}

wxColour& wxColour::operator = (const wxString& col)
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
/* TODO
    m_pixel = PALETTERGB (m_red, m_green, m_blue);
*/
    return (*this);
}

void wxColour::Set (unsigned char r, unsigned char g, unsigned char b)
{
    m_red = r;
    m_green = g;
    m_blue = b;
    m_isInit = TRUE;
/* TODO
    m_pixel = PALETTERGB (m_red, m_green, m_blue);
*/
}
