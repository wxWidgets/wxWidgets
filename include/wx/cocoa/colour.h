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
    wxColour() { Init(); }

    // from RGB
    wxColour( unsigned char red, unsigned char green, unsigned char blue )
    :   m_cocoaNSColor(NULL)
    {   Set(red,green,blue); }
    wxColour( unsigned long colRGB )
    :   m_cocoaNSColor(NULL)
    {   Set(colRGB); }

    // implicit conversion from the colour name
    wxColour( const wxString &colourName )
    {   InitFromName(colourName); }
    wxColour( const char *colourName )
    {   InitFromName(wxString::FromAscii(colourName)); }
#if wxUSE_UNICODE
    wxColour( const wxChar *colourName ) { InitFromName( wxString(colourName) ); }
#endif

    // copy ctors and assignment operators
    wxColour( const wxColour& col );
    wxColour& operator = ( const wxColour& col );

    virtual ~wxColour();

    // accessors
    bool Ok() const { return m_cocoaNSColor; }
    WX_NSColor GetNSColor() { return m_cocoaNSColor; }

    unsigned char Red() const { return m_red; }
    unsigned char Green() const { return m_green; }
    unsigned char Blue() const { return m_blue; }

    // comparison
    bool operator == (const wxColour& colour) const
    {
        // VZ: sure we want to compare NSColor objects for equality here?
        return (m_cocoaNSColor == colour.m_cocoaNSColor
            && m_red == colour.m_red
            && m_green == colour.m_green
            && m_blue == colour.m_blue);
    }
    bool operator != (const wxColour& colour) const
    {   return !(*this == colour); }

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
    // puts the object in an invalid, uninitialized state
    void Init();

    // create the object from name, leaves it uninitialized if it failed
    void InitFromName(const wxString& col);

private:
    WX_NSColor m_cocoaNSColor;
    unsigned char m_red;
    unsigned char m_green;
    unsigned char m_blue;

    DECLARE_DYNAMIC_CLASS(wxColour)
};

#endif // __WX_COCOA_COLOUR_H__
