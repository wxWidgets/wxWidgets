/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/colour.mm
// Purpose:     Conversions between UIColor and wxColour
// Author:      Stefan Csomor
// Created:     2025-10-02
// Copyright:   (c) 2025 Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/colour.h"

#include "wx/osx/private.h"
#include "wx/osx/private/available.h"

class wxUIColorRefData : public wxColourRefData
{
public:
    wxUIColorRefData(CGFloat r, CGFloat g, CGFloat b, CGFloat a);
    wxUIColorRefData(WXColor color);

    wxUIColorRefData(const wxUIColorRefData& other);

    virtual ~wxUIColorRefData();

    virtual CGFloat Red() const override;
    virtual CGFloat Green() const override;
    virtual CGFloat Blue() const override;
    virtual CGFloat Alpha() const override;

    virtual bool IsSolid() const override;

    CGColorRef GetCGColor() const override;

    virtual wxColourRefData* Clone() const override { return new wxUIColorRefData(*this); }

    virtual WXColor GetWXColor() const override;
    virtual WXImage GetWXPatternImage() const override;
private:
    static CGFloat GetCGColorComponent(CGColorRef col, int rgbaIndex);
    WX_UIColor m_uiColour;

    wxDECLARE_NO_ASSIGN_CLASS(wxUIColorRefData);
};


wxUIColorRefData::wxUIColorRefData(CGFloat r, CGFloat g, CGFloat b, CGFloat a)
{
    m_uiColour = [[UIColor colorWithRed:r green:g blue:b alpha:a] retain];
}

wxUIColorRefData::wxUIColorRefData(WXColor color)
{
    m_uiColour = [color retain];
}

wxUIColorRefData::wxUIColorRefData(const wxUIColorRefData& other)
{
    m_uiColour = [other.m_uiColour retain];
}

wxUIColorRefData::~wxUIColorRefData()
{
    [m_uiColour release];
}

WXColor wxUIColorRefData::GetWXColor() const
{
    return m_uiColour;
}

CGFloat wxUIColorRefData::GetCGColorComponent(CGColorRef col, int rgbaIndex)
{
    CGFloat value = 0.0;

    if ( col )
    {
        wxCFRef<CGColorRef> rgbacol;
        CGColorSpaceModel model = CGColorSpaceGetModel(CGColorGetColorSpace(col));
        size_t noComp = CGColorGetNumberOfComponents(col);
        const CGFloat* components = CGColorGetComponents(col);

        bool isRGB = true;

        if (model == kCGColorSpaceModelMonochrome)
        {
            wxASSERT_MSG(1 <= noComp && noComp <= 2, "Monochrome Color unexpected components");
            // is alpha is requested look into component if available
            if ( rgbaIndex == 3 )
                value = noComp > 1 ? components[1] : 1.0;
            else
                value = components[0];
            isRGB = false;
        }
        else if (model != kCGColorSpaceModelRGB)
        {
            if ( WX_IS_MACOS_OR_IOS_AVAILABLE(10, 11, 9, 0) )
            {
                rgbacol = CGColorCreateCopyByMatchingToColorSpace(wxMacGetGenericRGBColorSpace(), kCGRenderingIntentDefault, col, nullptr);
                noComp = CGColorGetNumberOfComponents(rgbacol);
                components = CGColorGetComponents(rgbacol);
            }
            else
            {
                isRGB = false;
            }
        }

        if (isRGB)
        {
            wxASSERT_MSG(3 <= noComp && noComp <= 4, "RGB Color unexpected components");
            // is alpha is requested look into component if available
            if ( rgbaIndex == 3 )
                value = noComp == 4 ? components[3] : 1.0;
            else
                value = components[rgbaIndex];
        }
    }

    return value;
}

CGFloat wxUIColorRefData::Red() const
{
    UITraitCollection* traitCollection = [UIScreen mainScreen].traitCollection;
    CGFloat component = 0.0;
    if ( [[m_uiColour resolvedColorWithTraitCollection:traitCollection] getRed:&component green:NULL blue:NULL alpha:NULL] )
        return component;

    return GetCGColorComponent([m_uiColour CGColor], 0);
}

CGFloat wxUIColorRefData::Green() const
{
    UITraitCollection* traitCollection = [UIScreen mainScreen].traitCollection;
    CGFloat component = 0.0;
    if ( [[m_uiColour resolvedColorWithTraitCollection:traitCollection] getRed:NULL green:&component blue:NULL alpha:NULL] )
        return component;

    return GetCGColorComponent([m_uiColour CGColor], 1);
}

CGFloat wxUIColorRefData::Blue() const
{
    UITraitCollection* traitCollection = [UIScreen mainScreen].traitCollection;
    CGFloat component = 0.0;
    if ( [[m_uiColour resolvedColorWithTraitCollection:traitCollection] getRed:NULL green:NULL blue:&component alpha:NULL] )
        return component;

    return GetCGColorComponent([m_uiColour CGColor], 2);
}

CGFloat wxUIColorRefData::Alpha() const
{
    UITraitCollection* traitCollection = [UIScreen mainScreen].traitCollection;
    CGFloat component = 0.0;
    if ( [[m_uiColour resolvedColorWithTraitCollection:traitCollection] getRed:NULL green:NULL blue:NULL alpha:&component] )
        return component;

    return GetCGColorComponent([m_uiColour CGColor], 3);
}

bool wxUIColorRefData::IsSolid() const
{
    return true;
//    return [m_uiColour colorUsingColorSpaceName:NSCalibratedRGBColorSpace] != nil;
}

CGColorRef wxUIColorRefData::GetCGColor() const
{
    return [m_uiColour CGColor];
}

WXImage wxUIColorRefData::GetWXPatternImage() const
{
    return nullptr;
}

wxGDIRefData* wxColourImpl::CreateGDIRefData() const
{
    return new wxUIColorRefData(0.0, 0.0, 0.0, 1.0);
}

wxColourImpl::wxColourImpl(WXColor col)
{
    m_refData = new wxUIColorRefData(col);
}

wxColourImpl::wxColourImpl(CGColorRef col)
{
    m_refData = new wxUIColorRefData([[UIColor colorWithCGColor:col] retain]);
    // as per contract CGColorRef is not retained
    CGColorRelease(col);
}
