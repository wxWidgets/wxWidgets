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
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_8
    if (wxPlatformInfo::Get().CheckOSVersion(10, 8)) {
        wxOSXEffectiveAppearanceSetter helper;
        return [m_nsColour CGColor];
    }
#endif
    CGColorRef cgcolor = NULL;

    // Simplest case is when we can directly get the RGBA components:
    if (NSColor* colRGBA = [m_nsColour colorUsingColorSpaceName:NSCalibratedRGBColorSpace])
    {
        CGFloat components[4];
        [colRGBA getRed:&components[0] green:&components[1] blue:&components[2] alpha:&components[3]];

        cgcolor = CGColorCreate(wxMacGetGenericRGBColorSpace(), components);
    }
    // Some colours use patterns, we can handle them with the help of CGColorRef
    else if (NSColor* colPat = [m_nsColour colorUsingColorSpaceName:NSPatternColorSpace])
    {
        NSImage* const nsimage = [colPat patternImage];
        if (nsimage)
        {
            NSSize size = [nsimage size];
            NSRect r = NSMakeRect(0, 0, size.width, size.height);
            CGImageRef cgimage = [nsimage CGImageForProposedRect:&r context:nil hints:nil];
            if (cgimage)
            {
                // Callbacks for CGPatternCreate()
                struct PatternCreateCallbacks
                {
                    static void Draw(void* info, CGContextRef ctx)
                    {
                        CGImageRef image = (CGImageRef)info;
                        CGContextDrawImage(
                            ctx,
                            CGRectMake(0, 0, CGImageGetWidth(image), CGImageGetHeight(image)),
                            image);
                    }

                    static void Release(void* WXUNUSED(info))
                    {
                        // Do not release the image here, we don't own it as it
                        // comes from NSImage.
                    }
                };

                const CGPatternCallbacks callbacks = {
                    /* version: */ 0,
                    &PatternCreateCallbacks::Draw,
                    &PatternCreateCallbacks::Release
                };

                CGPatternRef pattern = CGPatternCreate(
                                            cgimage,
                                            CGRectMake(0, 0, size.width, size.height),
                                            CGAffineTransformMake(1, 0, 0, 1, 0, 0),
                                            size.width,
                                            size.height,
                                            kCGPatternTilingConstantSpacing,
                                            /* isColored: */ true,
                                            &callbacks
                                       );
                CGColorSpaceRef space = CGColorSpaceCreatePattern(NULL);
                CGFloat components[1] = { 1.0 };
                cgcolor = CGColorCreateWithPattern(space, pattern, components);
                CGColorSpaceRelease(space);
                CGPatternRelease(pattern);
            }
        }
    }

    if (cgcolor == NULL)
    {
        // Don't assert here, this will more likely than not result in a crash as
        // colours are often created in drawing code which will be called again
        // when the assert dialog is shown, resulting in a recursive assertion
        // failure and, hence, a crash.
        NSLog(@"Failed to convert NSColor \"%@\" to CGColorRef.", m_nsColour);
    }
    return cgcolor;
}

WX_NSColor wxColourRefData::GetNSColor() const
{
    wxOSXEffectiveAppearanceSetter helper;
    return [NSColor colorWithCalibratedRed:Red() green:Green() blue:Blue() alpha:Alpha() ];
}

wxColour::wxColour(WX_NSColor col)
{
    m_refData = new wxNSColorRefData(col);
}


