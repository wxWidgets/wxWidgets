/////////////////////////////////////////////////////////////////////////////
// Name:        colour.cpp
// Purpose:     wxColour class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "colour.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/gdicmn.h"

IMPLEMENT_DYNAMIC_CLASS(wxColour, wxObject)

// Colour

wxColour::wxColour ()
{
  m_red = m_blue = m_green = 0;
  m_isInit = FALSE;
}

wxColour::wxColour (unsigned char r, unsigned char g, unsigned char b)
{
  m_red = r;
  m_green = g;
  m_blue = b;
  m_isInit = TRUE;
}

wxColour::wxColour (const wxColour& col)
{
  m_red = col.m_red;
  m_green = col.m_green;
  m_blue = col.m_blue;
  m_isInit = col.m_isInit;
}

wxColour& wxColour::operator =(const wxColour& col)
{
  m_red = col.m_red;
  m_green = col.m_green;
  m_blue = col.m_blue;
  m_isInit = col.m_isInit;
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
}

wxColour::~wxColour()
{
}

void wxColour::Set (unsigned char r, unsigned char g, unsigned char b)
{
  m_red = r;
  m_green = g;
  m_blue = b;
  m_isInit = TRUE;
}

// Obsolete
#if WXWIN_COMPATIBILITY
void wxColour::Get (unsigned char *r, unsigned char *g, unsigned char *b) const
{
  *r = m_red;
  *g = m_green;
  *b = m_blue;
}
#endif

