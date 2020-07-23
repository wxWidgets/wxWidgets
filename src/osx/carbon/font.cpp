/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/carbon/font.cpp
// Purpose:     wxFont class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/font.h"

#ifndef WX_PRECOMP
#include "wx/string.h"
#include "wx/utils.h"
#include "wx/intl.h"
#include "wx/gdicmn.h"
#include "wx/log.h"
#include "wx/math.h"
#endif

#include "wx/fontutil.h"
#include "wx/graphics.h"
#include "wx/settings.h"
#include "wx/tokenzr.h"

#include "wx/osx/private.h"

#include <map>
#include <string>

#include <float.h>       // for FLT_MAX

#define TRACE_CTFONT "ctfont"

class WXDLLEXPORT wxFontRefData : public wxGDIRefData
{
public:
    wxFontRefData(const wxFontInfo& info = wxFontInfo());

    wxFontRefData(const wxFontRefData& data);

    wxFontRefData(const wxNativeFontInfo& info)
        : m_info(info)
    {
    }

    wxFontRefData(CTFontRef font);

    double GetFractionalPointSize() const { return m_info.GetFractionalPointSize(); }

    wxFontFamily GetFamily() const { return m_info.GetFamily(); }

    wxFontStyle GetStyle() const { return m_info.GetStyle(); }

    int GetNumericWeight() const { return m_info.GetNumericWeight(); }

    bool GetUnderlined() const { return m_info.GetUnderlined(); }

    bool GetStrikethrough() const { return m_info.GetStrikethrough(); }

    wxString GetFaceName() const { return m_info.GetFaceName(); }

    wxFontEncoding GetEncoding() const { return m_info.GetEncoding(); }

    bool IsFixedWidth() const;

    CTFontRef OSXGetCTFont() const;

    CFDictionaryRef OSXGetCTFontAttributes() const;

    CGFontRef OSXGetCGFont() const;

    const wxNativeFontInfo& GetNativeFontInfo() const;

    void SetFractionalPointSize(double size)
    {
        if (GetFractionalPointSize() != size)
        {
            m_info.SetFractionalPointSize(size);
            Free();
        }
    }

    void SetFamily(wxFontFamily family)
    {
        if (m_info.GetFamily() != family)
        {
            m_info.SetFamily(family);
            Free();
        }
    }

    void SetStyle(wxFontStyle style)
    {
        if (m_info.GetStyle() != style)
        {
            m_info.SetStyle(style);
            Free();
        }
    }

    void SetNumericWeight(int weight)
    {
        if (m_info.GetNumericWeight() != weight)
        {
            m_info.SetNumericWeight(weight);
            Free();
        }
    }

    void SetStrikethrough(bool s)
    {
        if (m_info.GetStrikethrough() != s)
        {
            m_info.SetStrikethrough(s);
            Free();
        }
    }

    void SetUnderlined(bool u)
    {
        if (m_info.GetUnderlined() != u)
        {
            m_info.SetUnderlined(u);
            Free();
        }
    }

    void SetFaceName(const wxString& facename)
    {
        if (m_info.GetFaceName() != facename)
        {
            m_info.SetFaceName(facename);
            Free();
        }
    }

    void SetEncoding(wxFontEncoding encoding)
    {
        if (m_info.GetEncoding() != encoding)
        {
            m_info.SetEncoding(encoding);
            Free();
        }
    }

    void Free();

    void Alloc();
protected:
    void SetFont(CTFontRef font);
    void AllocIfNeeded() const;

    wxCFRef<CTFontRef> m_ctFont;
    wxCFMutableDictionaryRef m_ctFontAttributes;
    wxCFRef<CGFontRef> m_cgFont;
    wxNativeFontInfo m_info;
};

// ============================================================================
// implementation
// ============================================================================

namespace
{
    const int kCTWeightsCount = 12;
    static CGFloat gCTWeights[kCTWeightsCount] = {
        -1.000, // 0
        -0.800, // 100
        -0.600, // 200
        -0.400, // 300
        0.000, // 400
        0.230, // 500
        0.300, // 600
        0.400, // 700
        0.560, // 800
        0.620, // 900
        0.750, // 1000
    };

    int CTWeightToWX(CGFloat weight)
    {
        for (int i = 0; i < kCTWeightsCount - 1; ++i)
        {
            if ( (weight - gCTWeights[i]) < (gCTWeights[i+1]-weight) )
                return i * 100;
        }
        return 1000;
    }

    CGFloat WXWeightToCT(int w)
    {
        if (w < 0)
            w = 0;
        else if (w > 1000)
            w = 1000;

        return gCTWeights[w / 100];
    }

    wxString FamilyToFaceName(wxFontFamily family)
    {
        wxString faceName;

        switch (family)
        {
            case wxFONTFAMILY_DEFAULT:
                faceName = wxT("Lucida Grande");
                break;

            case wxFONTFAMILY_SCRIPT:
            case wxFONTFAMILY_ROMAN:
            case wxFONTFAMILY_DECORATIVE:
                faceName = wxT("Times");
                break;

            case wxFONTFAMILY_SWISS:
                faceName = wxT("Helvetica");
                break;

            case wxFONTFAMILY_MODERN:
            case wxFONTFAMILY_TELETYPE:
                faceName = wxT("Courier");
                break;

            default:
                faceName = wxT("Times");
                break;
        }

        return faceName;
    }

} // anonymous namespace

// ----------------------------------------------------------------------------
// wxFontRefData
// ----------------------------------------------------------------------------

#define M_FONTDATA ((wxFontRefData*)m_refData)

wxFontRefData::wxFontRefData(const wxFontRefData& data)
: wxGDIRefData()
    , m_info(data.m_info)
{
    m_ctFont = data.m_ctFont;
    m_ctFontAttributes = data.m_ctFontAttributes;
    m_cgFont = data.m_cgFont;
}

wxFontRefData::wxFontRefData(const wxFontInfo& info)
{
    m_info.Init();

    if ( info.HasFaceName() )
        SetFaceName(info.GetFaceName());
    else
        SetFamily(info.GetFamily());

    m_info.SetSizeOrDefault(info.GetFractionalPointSize());
    SetNumericWeight(info.GetNumericWeight());
    SetStyle(info.GetStyle());
    SetUnderlined(info.IsUnderlined());
    SetStrikethrough(info.IsStrikethrough());
    SetEncoding(info.GetEncoding());
}

void wxFontRefData::Free()
{
    m_ctFont.reset();
    m_ctFontAttributes.reset();
    m_cgFont.reset();
}

wxFontRefData::wxFontRefData(CTFontRef font)
{
    SetFont(font);
    m_info.InitFromFont(font);
}

void wxFontRefData::SetFont(CTFontRef font)
{
    m_ctFont.reset(wxCFRetain(font));

    wxCFMutableDictionaryRef dict;
    dict.SetValue(kCTFontAttributeName, m_ctFont.get());
    dict.SetValue(kCTForegroundColorFromContextAttributeName, kCFBooleanTrue);

    m_ctFontAttributes = dict;

    m_cgFont = CTFontCopyGraphicsFont(m_ctFont, NULL);
}

static const CGAffineTransform kSlantTransform = CGAffineTransformMake(1, 0, tan(wxDegToRad(11)), 1, 0, 0);

namespace
{

struct CachedFontEntry
{
    CachedFontEntry()
    {
        used = false;
    }

    wxCFRef<CTFontRef> font;
    wxCFMutableDictionaryRef fontAttributes;
    wxCFRef<CGFontRef> cgFont;
    bool used;
};

} // anonymous namespace

void wxFontRefData::AllocIfNeeded() const
{
    if (!m_ctFont)
        const_cast<wxFontRefData *>(this)->Alloc();

}

void wxFontRefData::Alloc()
{
    wxCHECK_RET(m_info.GetPointSize() > 0, wxT("Point size should not be zero."));

    // make sure the font descriptor has been processed properly
    // otherwise the Post Script Name may not be valid yet
    m_info.RealizeResource();
    
    // use font caching, we cache a font with a certain size and a font with just any size for faster creation
    wxString lookupnameNoSize = wxString::Format("%s_%d_%d", m_info.GetPostScriptName(), (int)m_info.GetStyle(), m_info.GetNumericWeight());

    wxString lookupnameWithSize = wxString::Format("%s_%d_%d_%.2f", m_info.GetPostScriptName(), (int)m_info.GetStyle(), m_info.GetNumericWeight(), m_info.GetFractionalPointSize());

    static std::map<wxString, CachedFontEntry> fontcache;

    CachedFontEntry& entryWithSize = fontcache[lookupnameWithSize];
    if (entryWithSize.used)
    {
        m_ctFont = entryWithSize.font;
        m_ctFontAttributes = entryWithSize.fontAttributes;
    }
    else
    {
        CachedFontEntry& entryNoSize = fontcache[lookupnameNoSize];
        if ( entryNoSize.used )
        {
            m_ctFont = CTFontCreateCopyWithAttributes(entryNoSize.font, m_info.GetPointSize(), NULL, NULL);
            m_ctFontAttributes = entryNoSize.fontAttributes.CreateCopy();
            m_ctFontAttributes.SetValue(kCTFontAttributeName,m_ctFont.get());
            m_cgFont = CTFontCopyGraphicsFont(m_ctFont, NULL);
            entryWithSize.font = m_ctFont;
            entryWithSize.cgFont = m_cgFont;
            entryWithSize.fontAttributes = m_ctFontAttributes;
            entryWithSize.used = true;
        }
        else
        {
            // emulate slant if necessary, the font descriptor itself carries that information,
            // while the weight can only be determined properly from the generated font itself
            const CGAffineTransform* remainingTransform = NULL;
            if ( m_info.GetStyle() != wxFONTSTYLE_NORMAL && m_info.GetCTSlant(m_info.GetCTFontDescriptor()) < 0.01 )
                remainingTransform = &kSlantTransform;

            wxCFRef<CTFontRef> font = CTFontCreateWithFontDescriptor(m_info.GetCTFontDescriptor(), m_info.GetPointSize(), remainingTransform);

            // emulate weigth if necessary
            int difference = m_info.GetNumericWeight() - CTWeightToWX(wxNativeFontInfo::GetCTWeight(font));

            SetFont(font); // Sets m_ctFont, m_ctFontAttributes, m_cgFont
            if ( difference != 0 )
            {
                if ( difference > 0 )
                {
                    // TODO: find better heuristics to determine target stroke width
                    CGFloat width = 0;
                    width = -1.0 * (1 + (difference / 100));
                    m_ctFontAttributes.SetValue(kCTStrokeWidthAttributeName, width);
                }
                else
                {
                    // we cannot emulate lighter fonts
                }
            }

            entryWithSize.font = m_ctFont;
            entryWithSize.cgFont = m_cgFont;
            entryWithSize.fontAttributes = m_ctFontAttributes;
            entryWithSize.used = true;

            entryNoSize.font = m_ctFont;
            entryNoSize.fontAttributes = m_ctFontAttributes;
            // no reason to copy cgFont as will have to be regenerated anyway
            entryNoSize.used = true;
         }
    }
    m_cgFont.reset(CTFontCopyGraphicsFont(m_ctFont, NULL));
}

bool wxFontRefData::IsFixedWidth() const
{
    CTFontSymbolicTraits traits = CTFontGetSymbolicTraits(m_ctFont);
    return (traits & kCTFontMonoSpaceTrait) != 0;
}

CTFontRef wxFontRefData::OSXGetCTFont() const
{
    AllocIfNeeded();
    return m_ctFont;
}

CFDictionaryRef wxFontRefData::OSXGetCTFontAttributes() const
{
    AllocIfNeeded();
    return m_ctFontAttributes;
}

CGFontRef wxFontRefData::OSXGetCGFont() const
{
    AllocIfNeeded();
    return m_cgFont;
}

const wxNativeFontInfo& wxFontRefData::GetNativeFontInfo() const
{
    AllocIfNeeded();
    return m_info;
}

// ----------------------------------------------------------------------------
// wxFont
// ----------------------------------------------------------------------------

bool wxFont::Create(const wxNativeFontInfo& info)
{
    UnRef();

    m_refData = new wxFontRefData(info);
    RealizeResource();

    return true;
}

wxFont::wxFont(wxOSXSystemFont font)
{
    wxASSERT(font != wxOSX_SYSTEM_FONT_NONE);
    CTFontUIFontType uifont = kCTFontSystemFontType;
    switch (font)
    {
        case wxOSX_SYSTEM_FONT_NORMAL:
            uifont = kCTFontSystemFontType;
            break;
        case wxOSX_SYSTEM_FONT_BOLD:
            uifont = kCTFontEmphasizedSystemFontType;
            break;
        case wxOSX_SYSTEM_FONT_SMALL:
            uifont = kCTFontSmallSystemFontType;
            break;
        case wxOSX_SYSTEM_FONT_SMALL_BOLD:
            uifont = kCTFontSmallEmphasizedSystemFontType;
            break;
        case wxOSX_SYSTEM_FONT_MINI:
            uifont = kCTFontMiniSystemFontType;
            break;
        case wxOSX_SYSTEM_FONT_MINI_BOLD:
            uifont = kCTFontMiniEmphasizedSystemFontType;
            break;
        case wxOSX_SYSTEM_FONT_LABELS:
            uifont = kCTFontLabelFontType;
            break;
        case wxOSX_SYSTEM_FONT_VIEWS:
            uifont = kCTFontViewsFontType;
            break;
        case wxOSX_SYSTEM_FONT_FIXED:
            uifont = kCTFontUIFontUserFixedPitch;
        default:
            break;
    }
    wxCFRef<CTFontRef> ctfont(CTFontCreateUIFontForLanguage(uifont, 0.0, NULL));
    m_refData = new wxFontRefData(ctfont);
}

#if wxOSX_USE_COCOA

wxFont::wxFont(WX_NSFont nsfont)
{
    m_refData = new wxFontRefData((CTFontRef)nsfont);
}

#endif

wxFont::wxFont(CTFontRef font)
{
    m_refData = new wxFontRefData(font);
}

wxFont::wxFont(const wxString& fontdesc)
{
    wxNativeFontInfo info;
    if (info.FromString(fontdesc))
        (void)Create(info);
}

wxFont::wxFont(const wxFontInfo& info)
{
    m_refData = new wxFontRefData(info);

    if ( info.IsUsingSizeInPixels() )
        SetPixelSize(info.GetPixelSize());
}

wxFont::wxFont(int size,
    int family,
    int style,
    int weight,
    bool underlined,
    const wxString& face,
    wxFontEncoding encoding)
{
    (void)Create(size, (wxFontFamily)family, (wxFontStyle)style,
        (wxFontWeight)weight, underlined, face, encoding);
}

bool wxFont::Create(int pointSize,
    wxFontFamily family,
    wxFontStyle style,
    wxFontWeight weight,
    bool underlined,
    const wxString& faceName,
    wxFontEncoding encoding)
{
    m_refData = new wxFontRefData(InfoFromLegacyParams(pointSize, family,
                                                       style, weight, underlined,
                                                       faceName, encoding));

    return true;
}

wxFont::~wxFont()
{
}

void wxFont::DoSetNativeFontInfo(const wxNativeFontInfo& info)
{
    UnRef();

    m_refData = new wxFontRefData(info);
}

bool wxFont::RealizeResource()
{
    return OSXGetCTFont();
}

void wxFont::SetEncoding(wxFontEncoding encoding)
{
    AllocExclusive();

    M_FONTDATA->SetEncoding(encoding);
}

wxGDIRefData* wxFont::CreateGDIRefData() const
{
    return new wxFontRefData;
}

wxGDIRefData* wxFont::CloneGDIRefData(const wxGDIRefData* data) const
{
    return new wxFontRefData(*static_cast<const wxFontRefData*>(data));
}

void wxFont::SetFractionalPointSize(double pointSize)
{
    AllocExclusive();

    M_FONTDATA->SetFractionalPointSize(pointSize);
}

void wxFont::SetFamily(wxFontFamily family)
{
    AllocExclusive();

    M_FONTDATA->SetFamily(family);
}

void wxFont::SetStyle(wxFontStyle style)
{
    AllocExclusive();

    M_FONTDATA->SetStyle(style);
}

void wxFont::SetNumericWeight(int weight)
{
    AllocExclusive();

    M_FONTDATA->SetNumericWeight(weight);
}

bool wxFont::SetFaceName(const wxString& faceName)
{
    AllocExclusive();

    M_FONTDATA->SetFaceName(faceName);

    return wxFontBase::SetFaceName(faceName);
}

void wxFont::SetUnderlined(bool underlined)
{
    AllocExclusive();

    M_FONTDATA->SetUnderlined(underlined);
}

void wxFont::SetStrikethrough(bool strikethrough)
{
    AllocExclusive();

    M_FONTDATA->SetStrikethrough(strikethrough);
}

// ----------------------------------------------------------------------------
// accessors
// ----------------------------------------------------------------------------

// TODO: insert checks everywhere for M_FONTDATA == NULL!

double wxFont::GetFractionalPointSize() const
{
    wxCHECK_MSG(IsOk(), 0, wxT("invalid font"));

    return M_FONTDATA->GetFractionalPointSize();
}

wxSize wxFont::GetPixelSize() const
{
#if wxUSE_GRAPHICS_CONTEXT
    // TODO: consider caching the value
    wxGraphicsContext* dc = wxGraphicsContext::CreateFromNative((CGContextRef)NULL);
    dc->SetFont(*this, *wxBLACK);
    wxDouble width, height = 0;
    dc->GetTextExtent(wxT("g"), &width, &height, NULL, NULL);
    delete dc;
    return wxSize((int)width, (int)height);
#else
    return wxFontBase::GetPixelSize();
#endif
}

bool wxFont::IsFixedWidth() const
{
    wxCHECK_MSG(IsOk(), false, wxT("invalid font"));

    // cast away constness otherwise lazy font resolution is not possible
    const_cast<wxFont*>(this)->RealizeResource();

    return M_FONTDATA->IsFixedWidth();
}

wxFontFamily wxFont::DoGetFamily() const
{
    return M_FONTDATA->GetFamily();
}

wxFontStyle wxFont::GetStyle() const
{
    wxCHECK_MSG(IsOk(), wxFONTSTYLE_MAX, wxT("invalid font"));

    return M_FONTDATA->GetStyle();
}

int wxFont::GetNumericWeight() const
{
    wxCHECK_MSG(IsOk(), wxFONTWEIGHT_MAX, wxT("invalid font"));

    return M_FONTDATA->GetNumericWeight();
}

bool wxFont::GetUnderlined() const
{
    wxCHECK_MSG(IsOk(), false, wxT("invalid font"));

    return M_FONTDATA->GetUnderlined();
}

bool wxFont::GetStrikethrough() const
{
    wxCHECK_MSG(IsOk(), false, wxT("invalid font"));

    return M_FONTDATA->GetStrikethrough();
}

wxString wxFont::GetFaceName() const
{
    wxCHECK_MSG(IsOk(), wxEmptyString, wxT("invalid font"));

    return M_FONTDATA->GetFaceName();
}

wxFontEncoding wxFont::GetEncoding() const
{
    wxCHECK_MSG(IsOk(), wxFONTENCODING_DEFAULT, wxT("invalid font"));

    return M_FONTDATA->GetEncoding();
}

CTFontRef wxFont::OSXGetCTFont() const
{
    wxCHECK_MSG(IsOk(), 0, wxT("invalid font"));
    return M_FONTDATA->OSXGetCTFont();
}

CFDictionaryRef wxFont::OSXGetCTFontAttributes() const
{
    wxCHECK_MSG(IsOk(), 0, wxT("invalid font"));
    return M_FONTDATA->OSXGetCTFontAttributes();
}

#if wxOSX_USE_COCOA_OR_CARBON

CGFontRef wxFont::OSXGetCGFont() const
{
    wxCHECK_MSG(IsOk(), 0, wxT("invalid font"));
    return M_FONTDATA->OSXGetCGFont();
}

#endif

const wxNativeFontInfo* wxFont::GetNativeFontInfo() const
{
    return IsOk() ? &(M_FONTDATA->GetNativeFontInfo()) : NULL;
}

// ----------------------------------------------------------------------------
// wxNativeFontInfo
// ----------------------------------------------------------------------------

/* from Core Text Manual Common Operations */

void wxNativeFontInfo::Init()
{
    m_descriptor.reset();

    m_underlined = false;
    m_strikethrough = false;
    m_encoding = wxFONTENCODING_UTF8;

    m_ctWeight = 0.0;
    m_style = wxFONTSTYLE_NORMAL;
    m_ctSize = 0.0;
    m_family = wxFONTFAMILY_DEFAULT;

    m_familyName.clear();
    m_postScriptName.clear();
}

void wxNativeFontInfo::Init(const wxNativeFontInfo& info)
{
    Init();

    m_descriptor = info.m_descriptor;

    m_underlined = info.m_underlined;
    m_strikethrough = info.m_strikethrough;
    m_encoding = info.m_encoding;

    m_ctWeight = info.m_ctWeight;
    m_style = info.m_style;
    m_ctSize = info.m_ctSize;
    m_family = info.m_family;

    m_familyName = info.m_familyName;
    m_postScriptName = info.m_postScriptName;
}

void wxNativeFontInfo::InitFromFont(CTFontRef font)
{
    Init();

    InitFromFontDescriptor(CTFontCopyFontDescriptor(font) );
}

void wxNativeFontInfo::InitFromFontDescriptor(CTFontDescriptorRef desc)
{
    Init();

    m_descriptor.reset(wxCFRetain(desc));

    m_ctWeight = GetCTWeight(desc);
    m_style = GetCTSlant(desc) > 0.01 ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL;
    wxCFTypeRef(CTFontDescriptorCopyAttribute(desc, kCTFontSizeAttribute)).GetValue(m_ctSize, CGFloat(0.0));

    // determine approximate family

    CTFontSymbolicTraits symbolicTraits;
    wxCFDictionaryRef traits((CFDictionaryRef)CTFontDescriptorCopyAttribute(desc, kCTFontTraitsAttribute));
    traits.GetValue(kCTFontSymbolicTrait).GetValue((int32_t*)&symbolicTraits, 0);

    if (symbolicTraits & kCTFontMonoSpaceTrait)
        m_family = wxFONTFAMILY_TELETYPE;
    else
    {
        uint32_t stylisticClass = symbolicTraits & kCTFontClassMaskTrait;

        if (stylisticClass == kCTFontSansSerifClass)
            m_family = wxFONTFAMILY_SWISS;
        else if (stylisticClass == kCTFontScriptsClass)
            m_family = wxFONTFAMILY_SCRIPT;
        else if (stylisticClass == kCTFontOrnamentalsClass)
            m_family = wxFONTFAMILY_DECORATIVE;
        else if (stylisticClass == kCTFontSymbolicClass)
            m_family = wxFONTFAMILY_DECORATIVE;
        else
            m_family = wxFONTFAMILY_ROMAN;
    }

    wxCFTypeRef(CTFontDescriptorCopyAttribute(m_descriptor, kCTFontFamilyNameAttribute)).GetValue(m_familyName);
    wxCFTypeRef(CTFontDescriptorCopyAttribute(m_descriptor, kCTFontNameAttribute)).GetValue(m_postScriptName);
}

void wxNativeFontInfo::Free()
{
    m_descriptor.reset();
}

CTFontDescriptorRef wxNativeFontInfo::GetCTFontDescriptor() const
{
    if ( !m_descriptor )
        const_cast<wxNativeFontInfo *>(this)->CreateCTFontDescriptor();

    return m_descriptor;
}

void wxNativeFontInfo::RealizeResource() const
{
    (void) GetCTFontDescriptor();
}

void wxNativeFontInfo::CreateCTFontDescriptor()
{
    CTFontDescriptorRef descriptor = NULL;
    wxCFMutableDictionaryRef attributes;

    // build all attributes that define our font.


    if ( m_postScriptName.empty() )
    {
        wxString fontname = m_familyName;
        if ( fontname.empty() )
            fontname = FamilyToFaceName(m_family);

        CFDictionaryAddValue(attributes, kCTFontFamilyNameAttribute, wxCFStringRef(fontname));
    }
    else
    {
        CFDictionaryAddValue(attributes, kCTFontNameAttribute, wxCFStringRef(m_postScriptName));
    }

    wxCFMutableDictionaryRef traits;
    if ( m_style != wxFONTSTYLE_NORMAL )
        traits.SetValue(kCTFontSymbolicTrait, kCTFontItalicTrait);

    traits.SetValue(kCTFontWeightTrait,m_ctWeight);

    attributes.SetValue(kCTFontTraitsAttribute,traits.get());
    attributes.SetValue(kCTFontSizeAttribute, m_ctSize);

    // Create the font descriptor with our attributes
    descriptor = CTFontDescriptorCreateWithAttributes(attributes);
    wxASSERT(descriptor != NULL);

    m_descriptor = descriptor;
    
    wxCFTypeRef(CTFontDescriptorCopyAttribute(m_descriptor, kCTFontFamilyNameAttribute)).GetValue(m_familyName);
    wxCFTypeRef(CTFontDescriptorCopyAttribute(m_descriptor, kCTFontNameAttribute)).GetValue(m_postScriptName);

#if wxDEBUG_LEVEL >= 2
    // for debugging: show all different font names
    wxCFRef<CTFontRef> font = CTFontCreateWithFontDescriptor(m_descriptor, 12, NULL);
    wxString familyname;
    wxCFTypeRef(CTFontDescriptorCopyAttribute(m_descriptor, kCTFontFamilyNameAttribute)).GetValue(familyname);
    wxLogTrace(TRACE_CTFONT,"****** CreateCTFontDescriptor ******");
    wxLogTrace(TRACE_CTFONT,"Descriptor FontFamilyName: %s",familyname.c_str());
    
    wxString name;
    wxCFTypeRef(CTFontDescriptorCopyAttribute(m_descriptor, kCTFontNameAttribute)).GetValue(name);
    wxLogTrace(TRACE_CTFONT,"Descriptor FontName: %s",name.c_str());
    
    wxString display;
    wxCFTypeRef(CTFontDescriptorCopyAttribute(m_descriptor, kCTFontDisplayNameAttribute)).GetValue(display);
    wxLogTrace(TRACE_CTFONT,"Descriptor DisplayName: %s",display.c_str());
    
    wxString style;
    wxCFTypeRef(CTFontDescriptorCopyAttribute(m_descriptor, kCTFontStyleNameAttribute)).GetValue(style);
    wxLogTrace(TRACE_CTFONT,"Descriptor StyleName: %s",style.c_str());

    wxString psname;
    wxCFTypeRef(CTFontCopyPostScriptName(font)).GetValue(psname);
    wxLogTrace(TRACE_CTFONT,"Created Font PostScriptName: %s",psname.c_str());
    
    wxString fullname;
    wxCFTypeRef(CTFontCopyFullName(font)).GetValue(fullname);
    wxLogTrace(TRACE_CTFONT,"Created Font FullName: %s",fullname.c_str());
    
    wxLogTrace(TRACE_CTFONT,"************************************");
#endif
}

// Core Text Helpers

CGFloat wxNativeFontInfo::GetCTWeight(CTFontRef font)
{
    CGFloat weight;
    CFTypeRef fonttraitstype = CTFontCopyAttribute(font, kCTFontTraitsAttribute);
    wxCFDictionaryRef traits((CFDictionaryRef)fonttraitstype);
    traits.GetValue(kCTFontWeightTrait).GetValue(&weight, CGFloat(0.0));
    return weight;
}

CGFloat wxNativeFontInfo::GetCTWeight(CTFontDescriptorRef descr)
{
    CGFloat weight;
    CFTypeRef fonttraitstype = CTFontDescriptorCopyAttribute(descr, kCTFontTraitsAttribute);
    wxCFDictionaryRef traits((CFDictionaryRef)fonttraitstype);
    traits.GetValue(kCTFontWeightTrait).GetValue(&weight, CGFloat(0.0));
    return weight;
}

CGFloat wxNativeFontInfo::GetCTSlant(CTFontDescriptorRef descr)
{
    CGFloat slant;
    CFTypeRef fonttraitstype = CTFontDescriptorCopyAttribute(descr, kCTFontTraitsAttribute);
    wxCFDictionaryRef traits((CFDictionaryRef)fonttraitstype);
    traits.GetValue(kCTFontSlantTrait).GetValue(&slant, CGFloat(0.0));
    return slant;
}


//
bool wxNativeFontInfo::FromString(const wxString& s)
{
    double d;
    long l, version;

    Init();

    wxStringTokenizer tokenizer(s, wxT(";"));

    wxString token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return false;
    version = l;
    //
    //  Ignore the version for now
    //

    token = tokenizer.GetNextToken();
    if ( !token.ToCDouble(&d) )
        return false;
    if ( d < 0 || d > FLT_MAX )
        return false;
#ifdef __LP64__
    // CGFloat is just double in this case.
    m_ctSize = d;
#else // !__LP64__
    m_ctSize = static_cast<CGFloat>(d);
#endif // __LP64__/!__LP64__

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return false;
    m_family = (wxFontFamily)l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return false;
    m_style = (wxFontStyle)l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return false;
    m_ctWeight = WXWeightToCT(wxFont::ConvertFromLegacyWeightIfNecessary(l));

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return false;
    m_underlined = l != 0;

    if ( version == 0L )
    {
        m_strikethrough = false;
    }
    else
    {
        token = tokenizer.GetNextToken();
        if ( !token.ToLong(&l) )
            return false;
        m_strikethrough = l != 0;
    }

    // this works correctly via fallback even if this is (backwards compatibility) a font family name
    SetPostScriptName( tokenizer.GetNextToken() );
    
    RealizeResource();

#ifndef __WXMAC__
    if( !m_familyName )
        return false;
#endif

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return false;
    m_encoding = (wxFontEncoding)l;

    return true;
}

wxString wxNativeFontInfo::ToString() const
{
    // make sure the font descriptor has been processed properly
    // otherwise the Post Script Name may not be valid yet
    RealizeResource();
    
    wxString s;

    s.Printf(wxT("%d;%s;%d;%d;%d;%d;%d;%s;%d"),
        1, // version
        wxString::FromCDouble(GetFractionalPointSize()),
        GetFamily(),
        (int)GetStyle(),
        GetNumericWeight(),
        GetUnderlined(),
        GetStrikethrough(),
        GetPostScriptName().GetData(),
        (int)GetEncoding());

    return s;
}

double wxNativeFontInfo::GetFractionalPointSize() const
{
    return m_ctSize;
}

wxFontStyle wxNativeFontInfo::GetStyle() const
{
    return m_style;
}

int wxNativeFontInfo::GetNumericWeight() const
{
    return CTWeightToWX(m_ctWeight /* GetCTWeight(m_descriptor)*/);
}

bool wxNativeFontInfo::GetUnderlined() const
{
    return m_underlined;
}

wxString wxNativeFontInfo::GetPostScriptName() const
{
    return m_postScriptName;
}

wxString wxNativeFontInfo::GetFaceName() const
{
    return m_familyName;
}

wxFontFamily wxNativeFontInfo::GetFamily() const
{
    return m_family;
}

wxFontEncoding wxNativeFontInfo::GetEncoding() const
{
    return m_encoding;
}

bool wxNativeFontInfo::GetStrikethrough() const
{
    return m_strikethrough;
}

// changing the font descriptor

void wxNativeFontInfo::SetFractionalPointSize(double pointsize)
{
    if (GetFractionalPointSize() != pointsize)
    {
        m_ctSize = pointsize;

        if ( m_descriptor)
        {
            wxCFMutableDictionaryRef attributes;
            attributes.SetValue(kCTFontSizeAttribute, wxCFNumberRef((CGFloat)pointsize));
            m_descriptor.reset(CTFontDescriptorCreateCopyWithAttributes(m_descriptor, attributes));
        }
    }
}

void wxNativeFontInfo::SetStyle(wxFontStyle style_)
{
    bool formerIsItalic = GetStyle() != wxFONTSTYLE_NORMAL;
    bool newIsItalic = style_ != wxFONTSTYLE_NORMAL;

    m_style = style_;

    if (formerIsItalic != newIsItalic)
    {
        Free();
    }
}

void wxNativeFontInfo::SetNumericWeight(int weight)
{
    int formerWeight = GetNumericWeight();
    if (formerWeight != weight)
    {
        Free();
        m_ctWeight = WXWeightToCT(weight);
    }
}

void wxNativeFontInfo::SetUnderlined(bool underlined)
{
    m_underlined = underlined;
}

bool wxNativeFontInfo::SetFaceName(const wxString& facename)
{
    if (GetFaceName() != facename)
    {
        Free();
        m_familyName = facename;
    }

    return true;
}

bool wxNativeFontInfo::SetPostScriptName(const wxString& postScriptName)
{
    if (m_postScriptName != postScriptName)
    {
        Free();
        m_postScriptName = postScriptName;
    }
    
    return true;
}

void wxNativeFontInfo::SetFamily(wxFontFamily family)
{
    Free();
    m_familyName.clear();
    m_family = family;
}

void wxNativeFontInfo::SetEncoding(wxFontEncoding encoding)
{
    wxUnusedVar(encoding);
}

void wxNativeFontInfo::SetStrikethrough(bool strikethrough)
{
    m_strikethrough = strikethrough;
}
