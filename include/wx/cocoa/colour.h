/////////////////////////////////////////////////////////////////////////////
// Name:        colour.h
// Purpose:     wxColour class
// Author:      David Elliott
// Modified by:
// Created:     2003/06/17
// RCS-ID:      $Id$
// Copyright:   (c) 2003 David Elliott
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __WX_COCOA_COLOUR_H__
#define __WX_COCOA_COLOUR_H__

#include "wx/object.h"
#include "wx/string.h"

// ========================================================================
// wxColour
// ========================================================================
class WXDLLEXPORT wxColour: public wxObject
{
public:
    DECLARE_DYNAMIC_CLASS(wxColour)
// ------------------------------------------------------------------------
// initialization
// ------------------------------------------------------------------------
    wxColour();
    // from RGB
    wxColour( unsigned char red, unsigned char green, unsigned char blue )
    :   m_cocoaNSColor(NULL)
    {   Set(red,green,blue); }
    wxColour( unsigned long colRGB )
    :   m_cocoaNSColor(NULL)
    {   Set(colRGB); }
  
    // implicit conversion from the colour name
    wxColour( const wxString &colourName )
    :   m_cocoaNSColor(NULL)
    {   InitFromName(colourName); }
    wxColour( const char *colourName )
    :   m_cocoaNSColor(NULL)
    {   InitFromName(wxString::FromAscii(colourName)); }

    // copy ctors and assignment operators
    wxColour( const wxColour& col );
    wxColour& operator = ( const wxColour& col );

    ~wxColour();

// ------------------------------------------------------------------------
// Implementation
// ------------------------------------------------------------------------
    // accessors
    bool Ok() const { return m_cocoaNSColor; }

    unsigned char Red() const { return m_red; }
    unsigned char Green() const { return m_green; }
    unsigned char Blue() const { return m_blue; }

    // comparison
    bool operator == (const wxColour& colour) const
    {
        return (m_cocoaNSColor == colour.m_cocoaNSColor &&
            m_red == colour.m_red && 
            m_green == colour.m_green && 
            m_blue == colour.m_blue);
    }
    bool operator != (const wxColour& colour) const
    {   return !(*this == colour); }

//  const WXCOLORREF& GetPixel() const { return m_pixel; };

    // Set() functions
    void Set( unsigned char red, unsigned char green, unsigned char blue );
    void Set( unsigned long colRGB )
    {
        // we don't need to know sizeof(long) here because we assume that the three
        // least significant bytes contain the R, G and B values
        Set((unsigned char)colRGB,
            (unsigned char)(colRGB >> 8),
            (unsigned char)(colRGB >> 16));
    }

protected:
    void InitFromName(const wxString& col);

private:
    WX_NSColor m_cocoaNSColor;
    unsigned char m_red;
    unsigned char m_green;
    unsigned char m_blue;
};

#endif // __WX_COCOA_COLOUR_H__
