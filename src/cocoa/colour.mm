/////////////////////////////////////////////////////////////////////////////
// Name:        colour.mm
// Purpose:     wxColour class
// Author:      David Elliott
// Modified by:
// Created:     2003/06/17
// RCS-ID:      $Id$
// Copyright:   (c) 2003 David Elliott
// Licence:   	wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#endif //WX_PRECOMP

#include "wx/gdicmn.h"
#include "wx/colour.h"

#include "wx/cocoa/autorelease.h"

#import <AppKit/NSColor.h>

IMPLEMENT_DYNAMIC_CLASS(wxColour, wxObject)

void wxColour::Init()
{
    m_cocoaNSColor = NULL;
    m_red =
    m_blue =
    m_green = 0;
}

wxColour::wxColour (const wxColour& col)
:   m_cocoaNSColor(col.m_cocoaNSColor)
,   m_red(col.m_red)
,   m_green(col.m_green)
,   m_blue(col.m_blue)
{
    [m_cocoaNSColor retain];
}

wxColour::wxColour( WX_NSColor aColor )
:   m_cocoaNSColor(nil)
{
    Set(aColor);
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

void wxColour::InitFromName(const wxString& name)
{
    if ( wxTheColourDatabase )
    {
        wxColour col = wxTheColourDatabase->Find(name);
        if ( col.Ok() )
        {
            *this = col;
            return;
        }
    }

    // leave invalid
    Init();
}

wxColour::~wxColour ()
{
    [m_cocoaNSColor release];
}

void wxColour::Set (unsigned char r, unsigned char g, unsigned char b)
{
    wxAutoNSAutoreleasePool pool;
    [m_cocoaNSColor release];
    m_cocoaNSColor = [[NSColor colorWithCalibratedRed:r/255.0 green:g/255.0 blue:b/255.0 alpha:1.0] retain];
    m_red = r;
    m_green = g;
    m_blue = b;
}

void wxColour::Set( WX_NSColor aColor )
{
    [aColor retain];
    [m_cocoaNSColor release];
    m_cocoaNSColor = aColor;

    /* Make a temporary color in RGB format and get the values.  Note that
       unless the color was actually RGB to begin with it's likely that
       these will be fairly bogus. Particulary if the color is a pattern. */
    NSColor *rgbColor = [m_cocoaNSColor colorUsingColorSpaceName:NSCalibratedRGBColorSpace];
    m_red 	= (wxUint8) ([rgbColor redComponent]   * 255.0);
    m_green = (wxUint8) ([rgbColor greenComponent] * 255.0);
    m_blue 	= (wxUint8) ([rgbColor blueComponent]  * 255.0);
}

