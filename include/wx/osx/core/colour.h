/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/core/colour.h
// Purpose:     wxColour class
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
class WXDLLIMPEXP_CORE wxColour: public wxColourBase
{
public:
    // constructors
    // ------------
    DEFINE_STD_WXCOLOUR_CONSTRUCTORS

    // default copy ctor and dtor are ok

    // accessors
    virtual ChannelType Red() const override;
    virtual ChannelType Green() const override;
    virtual ChannelType Blue() const override;
    virtual ChannelType Alpha() const override;

    virtual bool IsSolid() const override;

    // comparison
    bool operator == (const wxColour& colour) const;
    bool operator != (const wxColour& colour) const { return !(*this == colour); }

    // CoreGraphics CGColor
    // --------------------

    // This ctor does take ownership of the color.
    wxColour( CGColorRef col );

    // don't take ownership of the returned value
    CGColorRef GetCGColor() const;

    // do take ownership of the returned value
    CGColorRef CreateCGColor() const { return wxCFRetain(GetCGColor()); }

#if wxOSX_USE_COCOA_OR_CARBON
    // Quickdraw RGBColor
    // ------------------
    wxColour(const RGBColor& col);
    void GetRGBColor( RGBColor *col ) const;
#endif

#if wxOSX_USE_COCOA
    // NSColor Cocoa
    // -------------

    // This ctor does not take ownership of the color.
    explicit wxColour(WX_NSColor color);
    WX_NSColor OSXGetNSColor() const;
    WX_NSImage OSXGetNSPatternImage() const;
#endif

protected :
    virtual void
    InitRGBA(ChannelType r, ChannelType g, ChannelType b, ChannelType a) override;

    virtual wxGDIRefData *CreateGDIRefData() const override;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const override;

private:

    wxDECLARE_DYNAMIC_CLASS(wxColour);
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

    virtual wxColourRefData* Clone() const = 0;

#if wxOSX_USE_COCOA
    virtual WX_NSColor GetNSColor() const;
    virtual WX_NSImage GetNSPatternImage() const;
#endif
};

#endif
  // _WX_COLOUR_H_
