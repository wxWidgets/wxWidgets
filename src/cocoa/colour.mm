/////////////////////////////////////////////////////////////////////////////
// Name:        colour.mm
// Purpose:     wxColour class
// Author:      David Elliott
// Modified by:
// Created:     2003/06/17
// RCS-ID:      $Id$
// Copyright:   (c) 2003 David Elliott
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#import <AppKit/NSColor.h>

#include "wx/gdicmn.h"
#include "wx/colour.h"

IMPLEMENT_DYNAMIC_CLASS(wxColour, wxObject)

wxColour::wxColour ()
:   m_cocoaNSColor(NULL)
{
    m_red = m_blue = m_green = 0;
}

wxColour::wxColour (const wxColour& col)
:   m_cocoaNSColor(col.m_cocoaNSColor)
,   m_red(col.m_red)
,   m_green(col.m_green)
,   m_blue(col.m_blue)
{
    [m_cocoaNSColor retain];
}

wxColour& wxColour::operator =(const wxColour& col)
{
    m_cocoaNSColor = col.m_cocoaNSColor;
    m_red = col.m_red;
    m_green = col.m_green;
    m_blue = col.m_blue;
    [m_cocoaNSColor retain];
    return *this;
}

void wxColour::InitFromName(const wxString& col)
{
    wxColour *the_colour = wxTheColourDatabase->FindColour (col);
    if (the_colour)
    {
        *this = *the_colour;
    }
    else
    {
        [m_cocoaNSColor release];
        m_cocoaNSColor = NULL;
        m_red = 0;
        m_green = 0;
        m_blue = 0;
    }
}

wxColour::~wxColour ()
{
    [m_cocoaNSColor release];
}

void wxColour::Set (unsigned char r, unsigned char g, unsigned char b)
{
    [m_cocoaNSColor release];
    m_cocoaNSColor = [[NSColor colorWithCalibratedRed:r/255.0 green:g/255.0 blue:b/255.0 alpha:1.0] retain];
    m_red = r;
    m_green = g;
    m_blue = b;
}

