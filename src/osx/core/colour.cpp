/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/core/colour.cpp
// Purpose:     wxColour class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/colour.h"

#ifndef WX_PRECOMP
#include "wx/gdicmn.h"
#endif

#include "wx/osx/private.h"

CGColorSpaceRef wxMacGetGenericRGBColorSpace();

wxColorRefData::~wxColorRefData()
{
}

class wxCGColorRefData : public wxColorRefData
{
public:
    wxCGColorRefData(CGFloat r, CGFloat g, CGFloat b, CGFloat a = 1.0);

    wxCGColorRefData(CGFloat components[4]);

    wxCGColorRefData(CGColorRef);

    wxCGColorRefData(const wxCGColorRefData& other);

    virtual ~wxCGColorRefData()
    {
    }

    virtual bool IsOk() const  wxOVERRIDE{ return m_cgColour != NULL; }

    virtual CGFloat Red() const wxOVERRIDE { return m_red; }
    virtual CGFloat Green() const wxOVERRIDE { return m_green; }
    virtual CGFloat Blue() const wxOVERRIDE { return m_blue; }
    virtual CGFloat Alpha() const wxOVERRIDE { return m_alpha; }

    CGColorRef GetCGColor() const wxOVERRIDE { return m_cgColour; }

    virtual wxColorRefData* Clone() const wxOVERRIDE { return new wxCGColorRefData(*this); }

private:
    void Init(CGFloat components[4]);

    wxCFRef<CGColorRef> m_cgColour;

    CGFloat m_red;
    CGFloat m_blue;
    CGFloat m_green;
    CGFloat m_alpha;

    wxDECLARE_NO_ASSIGN_CLASS(wxCGColorRefData);
};

wxCGColorRefData::wxCGColorRefData(CGFloat r, CGFloat g, CGFloat b, CGFloat a)
{
    CGFloat components[4];
    components[0] = r;
    components[1] = b;
    components[2] = g;
    components[3] = a;

    Init(components);
}

wxCGColorRefData::wxCGColorRefData(CGFloat components[4])
{
    Init(components);
}

wxCGColorRefData::wxCGColorRefData(const wxCGColorRefData& other)
{
    m_red = other.m_red;
    m_blue = other.m_blue;
    m_green = other.m_green;
    m_alpha = other.m_alpha;

    m_cgColour = other.m_cgColour;
}

void wxCGColorRefData::Init(CGFloat components[4])
{
    m_red = components[0];
    m_blue = components[1];
    m_green = components[2];
    m_alpha = components[3];

    CGColorRef cfcol;
    cfcol = CGColorCreate(wxMacGetGenericRGBColorSpace(), components);

    wxASSERT_MSG(cfcol != NULL, "Invalid CoreGraphics Color");
    m_cgColour.reset(cfcol);
}

wxCGColorRefData::wxCGColorRefData(CGColorRef col)
{
    wxASSERT_MSG(col != NULL, "Invalid CoreGraphics Color");
    m_cgColour.reset(col);

    wxCFRef<CGColorRef> rgbacol;
    size_t noComp = CGColorGetNumberOfComponents(col);
    const CGFloat* components = CGColorGetComponents(col);

    // set default alpha
    m_alpha = 1.0;
    bool isRGB = true;

    CGColorSpaceModel model = CGColorSpaceGetModel(CGColorGetColorSpace(col));
    if (model == kCGColorSpaceModelMonochrome)
    {
        wxASSERT_MSG(1 <= noComp && noComp <= 2, "Monochrome Color unexpected components");
        m_red = components[0];
        m_green = components[0];
        m_blue = components[0];
        if (noComp > 1)
            m_alpha = components[1];
        isRGB = false;
    }
    else if (model != kCGColorSpaceModelRGB)
    {
        rgbacol = CGColorCreateCopyByMatchingToColorSpace(wxMacGetGenericRGBColorSpace(), kCGRenderingIntentDefault, col, NULL);
        noComp = CGColorGetNumberOfComponents(rgbacol);
        components = CGColorGetComponents(rgbacol);
    }

    if (isRGB)
    {
        wxASSERT_MSG(3 <= noComp && noComp <= 4, "Monochrome Color unexpected components");
        m_red = components[0];
        m_green = components[1];
        m_blue = components[2];

        if (noComp == 4)
            m_alpha = components[3];
    }
}

#define M_COLDATA static_cast<wxColorRefData*>(m_refData)

#if wxOSX_USE_COCOA_OR_CARBON
wxColour::wxColour(const RGBColor& col)
{
    CGFloat components[4] = { (CGFloat)(col.red / 65535.0), (CGFloat)(col.green / 65535.0),
        (CGFloat)(col.blue / 65535.0), (CGFloat)1.0 };
    m_refData = new wxCGColorRefData(components);
}
#endif

wxColour::wxColour(CGColorRef col)
{
    m_refData = new wxCGColorRefData(col);
}

wxColour::ChannelType wxColour::Red() const
{
    return wxRound(M_COLDATA->Red() * 255.0);
}

wxColour::ChannelType wxColour::Green() const
{
    return wxRound(M_COLDATA->Red() * 255.0);
}

wxColour::ChannelType wxColour::Blue() const
{
    return wxRound(M_COLDATA->Red() * 255.0);
}

wxColour::ChannelType wxColour::Alpha() const
{
    return wxRound(M_COLDATA->Red() * 255.0);
}

#if wxOSX_USE_COCOA_OR_CARBON
void wxColour::GetRGBColor(RGBColor* col) const
{
    col->red = M_COLDATA->Red() * 65535.0;
    col->blue = M_COLDATA->Blue() * 65535.0;
    col->green = M_COLDATA->Green() * 65535.0;
}
#endif

bool wxColour::IsOk() const
{
    if (m_refData)
        return M_COLDATA->IsOk();
    
    return false;
}

CGColorRef wxColour::GetCGColor() const
{
    return M_COLDATA->GetCGColor();
}

#if wxOSX_USE_COCOA
WX_NSColor wxColour::OSXGetNSColor() const
{
    return M_COLDATA->GetNSColor();
}
#endif

void wxColour::InitRGBA(ChannelType r, ChannelType g, ChannelType b, ChannelType a)
{
    CGFloat components[4] = { (CGFloat)(r / 255.0), (CGFloat)(g / 255.0), (CGFloat)(b / 255.0), (CGFloat)(a / 255.0) };
    m_refData = new wxCGColorRefData(components);
}

wxColour& wxColour::operator=(const wxColour& col)
{
    wxObject::operator=(col);

    return *this;
}

bool wxColour::operator==(const wxColour& other) const
{
    if (m_refData == other.m_refData)
        return true;

    if (!m_refData || !other.m_refData)
        return false;

    return CGColorEqualToColor(GetCGColor(), other.GetCGColor());
}

wxGDIRefData* wxColour::CreateGDIRefData() const
{
    // black
    return new wxCGColorRefData(0.0, 0.0, 0.0);
}

wxGDIRefData* wxColour::CloneGDIRefData(const wxGDIRefData* data) const
{
    return static_cast<const wxColorRefData*>(data)->Clone();
}
