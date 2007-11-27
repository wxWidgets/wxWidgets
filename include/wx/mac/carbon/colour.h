/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/carbon/colour.h
// Purpose:     wxColour class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COLOUR_H_
#define _WX_COLOUR_H_

#include "wx/object.h"
#include "wx/string.h"

#include "wx/mac/corefoundation/cfref.h"

struct RGBColor;

// Colour
class WXDLLEXPORT wxColour: public wxColourBase
{
public:
    // constructors
    // ------------

    // default
    wxColour() { }
    DEFINE_STD_WXCOLOUR_CONSTRUCTORS

    // dtor
    virtual ~wxColour();

    // accessors
    bool IsOk() const;

    ChannelType Red() const { return m_red; }
    ChannelType Green() const { return m_green; }
    ChannelType Blue() const { return m_blue; }
    ChannelType Alpha() const { return m_alpha; }

    // comparison
    bool operator == (const wxColour& colour) const;
    
    bool operator != (const wxColour& colour) const { return !(*this == colour); }

    CGColorRef GetPixel() const { return m_cgColour; };
    
    CGColorRef GetCGColor() const { return m_cgColour; };
    CGColorRef CreateCGColor() const { return wxCFRetain( (CGColorRef)m_cgColour ); };
    
    void GetRGBColor( RGBColor *col ) const;

    // Mac-specific ctor and assignment operator from the native colour
    // assumes ownership of CGColorRef
    wxColour( CGColorRef col );
    wxColour(const RGBColor& col);
    wxColour& operator=(const RGBColor& col);
    wxColour& operator=(CGColorRef col);

protected :


    virtual void
    InitRGBA(ChannelType r, ChannelType g, ChannelType b, ChannelType a);
    void InitRGBColor( const RGBColor& col );
    void InitCGColorRef( CGColorRef col );
private:
    wxCFRef<CGColorRef>     m_cgColour;

    ChannelType             m_red;
    ChannelType             m_blue;
    ChannelType             m_green;
    ChannelType             m_alpha;

    DECLARE_DYNAMIC_CLASS(wxColour)
};

#endif
  // _WX_COLOUR_H_
