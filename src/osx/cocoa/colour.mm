/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/colour.mm
// Purpose:     Conversions between NSColor and wxColour
// Author:      Vadim Zeitlin
// Created:     2015-11-26 (completely replacing the old version of the file)
// Copyright:   (c) 2015 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/colour.h"

#include "wx/osx/private.h"
#include "wx/osx/private/available.h"

class wxNSColorRefData : public wxColourRefData
{
public:
    wxNSColorRefData(WX_NSColor color);
    
    wxNSColorRefData(const wxNSColorRefData& other);

    virtual ~wxNSColorRefData();
    
    virtual CGFloat Red() const wxOVERRIDE;
    virtual CGFloat Green() const wxOVERRIDE;
    virtual CGFloat Blue() const wxOVERRIDE;
    virtual CGFloat Alpha() const wxOVERRIDE;
    
    virtual bool IsSolid() const wxOVERRIDE;

    CGColorRef GetCGColor() const wxOVERRIDE;
    
    virtual wxColourRefData* Clone() const wxOVERRIDE { return new wxNSColorRefData(*this); }
    
    virtual WX_NSColor GetNSColor() const wxOVERRIDE;
    virtual WX_NSImage GetNSPatternImage() const wxOVERRIDE;
private:
    WX_NSColor m_nsColour;
    
    wxDECLARE_NO_ASSIGN_CLASS(wxNSColorRefData);
};

wxNSColorRefData::wxNSColorRefData(WX_NSColor color)
{
    m_nsColour = [color retain];
}

wxNSColorRefData::wxNSColorRefData(const wxNSColorRefData& other)
{
    m_nsColour = [other.m_nsColour retain];
}

wxNSColorRefData::~wxNSColorRefData()
{
    [m_nsColour release];
}

WX_NSColor wxNSColorRefData::GetNSColor() const
{
    return m_nsColour;
}

CGFloat wxNSColorRefData::Red() const
{
    wxOSXEffectiveAppearanceSetter helper;
    if ( NSColor* colRGBA = [m_nsColour colorUsingColorSpaceName:NSCalibratedRGBColorSpace] )
        return [colRGBA redComponent];
    
    return 0.0;
}

CGFloat wxNSColorRefData::Green() const
{
    wxOSXEffectiveAppearanceSetter helper;
    if ( NSColor* colRGBA = [m_nsColour colorUsingColorSpaceName:NSCalibratedRGBColorSpace] )
        return [colRGBA greenComponent];
    
    return 0.0;
}

CGFloat wxNSColorRefData::Blue() const
{
    wxOSXEffectiveAppearanceSetter helper;
    if ( NSColor* colRGBA = [m_nsColour colorUsingColorSpaceName:NSCalibratedRGBColorSpace] )
        return [colRGBA blueComponent];
    
    return 0.0;
}

CGFloat wxNSColorRefData::Alpha() const
{
    wxOSXEffectiveAppearanceSetter helper;
    if ( NSColor* colRGBA = [m_nsColour colorUsingColorSpaceName:NSCalibratedRGBColorSpace] )
        return [colRGBA alphaComponent];
    
    return 0.0;
}

bool wxNSColorRefData::IsSolid() const
{
    return [m_nsColour colorUsingColorSpaceName:NSCalibratedRGBColorSpace] != nil;
}

CGColorRef wxNSColorRefData::GetCGColor() const
{
    wxOSXEffectiveAppearanceSetter helper;
    return [m_nsColour CGColor];
}

WX_NSImage wxNSColorRefData::GetNSPatternImage() const
{
    NSColor* colPat = [m_nsColour colorUsingColorSpaceName:NSPatternColorSpace];
    if ( colPat )
    {
        NSImage* nsimage = [colPat patternImage];
        if ( nsimage )
        {
            return nsimage;
        }
    }

    return NULL;
}

WX_NSColor wxColourRefData::GetNSColor() const
{
    wxOSXEffectiveAppearanceSetter helper;
    return [NSColor colorWithCalibratedRed:Red() green:Green() blue:Blue() alpha:Alpha() ];
}

WX_NSImage wxColourRefData::GetNSPatternImage() const
{
    return NULL;
}

wxColour::wxColour(WX_NSColor col)
{
    m_refData = new wxNSColorRefData(col);
}
