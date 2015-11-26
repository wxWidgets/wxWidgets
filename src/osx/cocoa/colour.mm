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

// Helper function to avoid writing too many casts in wxColour ctor.
static inline wxColour::ChannelType NSColorChannelToWX(CGFloat c)
{
    return static_cast<wxColour::ChannelType>(c * 255 + 0.5);
}

wxColour::wxColour(WX_NSColor col)
{
    // Simplest case is when we can directly get the RGBA components:
    if ( NSColor* colRGBA = [col colorUsingColorSpaceName:NSCalibratedRGBColorSpace] )
    {
        InitRGBA
        (
             NSColorChannelToWX([colRGBA redComponent]),
             NSColorChannelToWX([colRGBA greenComponent]),
             NSColorChannelToWX([colRGBA blueComponent]),
             NSColorChannelToWX([colRGBA alphaComponent])
        );
        return;
    }

    // Don't assert here, this will more likely than not result in a crash as
    // colours are often created in drawing code which will be called again
    // when the assert dialog is shown, resulting in a recursive assertion
    // failure and, hence, a crash.
    NSLog(@"Failed to convert NSColor \"%@\" to wxColour.", col);
}

WX_NSColor wxColour::OSXGetNSColor() const
{
    return [NSColor colorWithCalibratedRed:m_red / 255.0 green:m_green / 255.0 blue:m_blue / 255.0 alpha:m_alpha / 255.0];
}
