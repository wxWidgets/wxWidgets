/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/core/colour.h
// Purpose:     wxColourImpl class
// Author:      Stefan Csomor
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COLOUR_H_
#define _WX_COLOUR_H_

#include "wx/object.h"
#include "wx/string.h"

#include "wx/osx/core/cfref.h"

struct RGBColor;

// Colour
class WXDLLIMPEXP_CORE wxColourImpl: public wxColourBase
{
public:
    // constructors
    // ------------
    wxColourImpl() = default;

    // default copy ctor and dtor are ok

    // accessors
    virtual ChannelType Red() const override;
    virtual ChannelType Green() const override;
    virtual ChannelType Blue() const override;
    virtual ChannelType Alpha() const override;

    virtual bool IsSolid() const override;

    // comparison
    bool operator == (const wxColourImpl& colour) const;
    bool operator != (const wxColourImpl& colour) const { return !(*this == colour); }

    // CoreGraphics CGColor
    // --------------------

    // This ctor does take ownership of the color.
    wxColourImpl( CGColorRef col );

    // don't take ownership of the returned value
    CGColorRef GetCGColor() const;

    // do take ownership of the returned value
    CGColorRef CreateCGColor() const { return wxCFRetain(GetCGColor()); }

#if wxOSX_USE_COCOA_OR_CARBON
    // Quickdraw RGBColor
    // ------------------
    wxColourImpl(const RGBColor& col);
    void GetRGBColor( RGBColor *col ) const;
#endif

    // This ctor does not take ownership of the color.
    explicit wxColourImpl(WXColor color);

    WXColor OSXGetWXColor() const;
    WXImage OSXGetWXPatternImage() const;
#if WXWIN_COMPATIBILITY_3_2
#if wxOSX_USE_COCOA
    // NSColor Cocoa
    // -------------

    WX_NSColor OSXGetNSColor() const
    {
        return OSXGetWXColor();
    }
    WX_NSImage OSXGetNSPatternImage() const
    {
        return OSXGetWXPatternImage();
    }
#endif
#endif

    virtual void
    InitRGBA(ChannelType r, ChannelType g, ChannelType b, ChannelType a) override;

    virtual void
    InitRGBA(float r, float g, float b, float a);

    virtual wxGDIRefData *CreateGDIRefData() const override;
    wxNODISCARD virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const override;
};

class wxColourRefData : public wxGDIRefData
{
public:
    wxColourRefData() = default;
    virtual ~wxColourRefData() = default;

    virtual double Red() const = 0;
    virtual double Green() const = 0;
    virtual double Blue() const = 0;
    virtual double Alpha() const = 0;

    virtual bool IsSolid() const
        { return true; }

    virtual CGColorRef GetCGColor() const = 0;

    wxNODISCARD virtual wxColourRefData* Clone() const = 0;

    virtual WXColor GetWXColor() const = 0;
    virtual WXImage GetWXPatternImage() const = 0;

#if WXWIN_COMPATIBILITY_3_2
#if wxOSX_USE_COCOA
    virtual WX_NSColor GetNSColor() const
    {
        return GetWXColor();
    }
    virtual WX_NSImage GetNSPatternImage() const
    {
        return GetWXPatternImage();
    }
#endif
#endif
};

#endif
  // _WX_COLOUR_H_
