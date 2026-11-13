/////////////////////////////////////////////////////////////////////////////
// Name:        src/retro68/font.cpp
// Purpose:     wxFont implementation using Font Manager fonts
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/font.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif

#include "wx/fontutil.h"
#include "wx/fontenum.h"

#include "wx/retro68/private/qd.h"

// Implemented in src/retro68/dc.cpp.
extern void wxRetro68GetTextExtent(const wxFont& font,
                                   const wxString& text,
                                   wxCoord *x, wxCoord *y,
                                   wxCoord *descent,
                                   wxCoord *externalLeading);

// ----------------------------------------------------------------------------
// helpers
// ----------------------------------------------------------------------------

namespace
{

// QuickDraw style bits.
enum
{
    wxMAC_STYLE_BOLD = 1,
    wxMAC_STYLE_ITALIC = 2,
    wxMAC_STYLE_UNDERLINE = 4
};

// Default font size used by classic Mac OS.
const int wxMAC_DEFAULT_FONT_SIZE = 12;

// Return the font ID for the font with the given name or -1 if it doesn't
// exist.
short GetFontIDFromName(const wxString& name)
{
    if ( name.empty() )
        return -1;

    Str255 pname;
    wxMacStringToPascal(name, pname);

    short id = 0;
    GetFNum(pname, &id);

    if ( id == 0 )
    {
        // GetFNum() returns 0 for unknown fonts, but 0 is also the ID of the
        // system font, so check if this is really the system font.
        Str255 sysName;
        GetFontName(0, sysName);
        if ( !EqualString(pname, sysName, false, false) )
            return -1;
    }

    return id;
}

// Return the ID of the font to use for the given family.
short GetFontIDForFamily(wxFontFamily family)
{
    const char* names[3] = { nullptr, nullptr, nullptr };

    switch ( family )
    {
        case wxFONTFAMILY_ROMAN:
            names[0] = "Times";
            names[1] = "New York";
            break;

        case wxFONTFAMILY_MODERN:
        case wxFONTFAMILY_TELETYPE:
            names[0] = "Monaco";
            names[1] = "Courier";
            break;

        case wxFONTFAMILY_SWISS:
            names[0] = "Geneva";
            names[1] = "Helvetica";
            break;

        case wxFONTFAMILY_SCRIPT:
            names[0] = "Zapf Chancery";
            names[1] = "Apple Chancery";
            names[2] = "Geneva";
            break;

        case wxFONTFAMILY_DECORATIVE:
        case wxFONTFAMILY_DEFAULT:
        default:
            names[0] = "Geneva";
            break;
    }

    for ( const char* name : names )
    {
        if ( !name )
            break;

        const short id = GetFontIDFromName(name);
        if ( id != -1 )
            return id;
    }

    // Use the application font as the fallback.
    return applFont;
}

} // anonymous namespace

// ----------------------------------------------------------------------------
// wxFontRefData
// ----------------------------------------------------------------------------

class wxFontRefData : public wxGDIRefData
{
public:
    wxFontRefData(int size = wxDEFAULT,
                  wxFontFamily family = wxFONTFAMILY_DEFAULT,
                  wxFontStyle style = wxFONTSTYLE_NORMAL,
                  int weight = wxFONTWEIGHT_NORMAL,
                  bool underlined = false,
                  const wxString& faceName = wxEmptyString,
                  wxFontEncoding encoding = wxFONTENCODING_DEFAULT)
    {
        if ( family == wxFONTFAMILY_DEFAULT )
            family = wxFONTFAMILY_SWISS;
        if ( size == wxDEFAULT || size <= 0 )
            size = wxMAC_DEFAULT_FONT_SIZE;

        m_info.family = family;
        m_info.faceName = faceName;
        m_info.style = style;
        m_info.weight = weight;
        m_info.underlined = underlined;
        m_info.strikethrough = false;
        m_info.encoding = encoding;
        m_info.pointSize = size;
    }

    explicit wxFontRefData(const wxNativeFontInfo& info)
        : m_info(info)
    {
    }

    wxFontRefData(const wxFontRefData& data)
        : wxGDIRefData(),
          m_info(data.m_info),
          m_fontID(data.m_fontID)
    {
    }

    virtual bool IsOk() const override { return true; }

    // Return the font ID, computing it if necessary.
    short GetFontID() const
    {
        if ( m_fontID == -1 )
        {
            m_fontID = GetFontIDFromName(m_info.faceName);
            if ( m_fontID == -1 )
                m_fontID = GetFontIDForFamily(m_info.family);
        }

        return m_fontID;
    }

    // Must be called whenever the font face or family changes.
    void ResetFontID() { m_fontID = -1; }

    wxNativeFontInfo m_info;

private:
    mutable short m_fontID = -1;
};

#define M_FONTDATA ((wxFontRefData*)m_refData)

// ----------------------------------------------------------------------------
// wxFont
// ----------------------------------------------------------------------------

bool wxFont::Create(const wxNativeFontInfo& info)
{
    UnRef();

    m_refData = new wxFontRefData(info);

    return true;
}

bool wxFont::Create(int pointSize,
                    wxFontFamily family,
                    wxFontStyle style,
                    wxFontWeight weight,
                    bool underlined,
                    const wxString& face,
                    wxFontEncoding encoding)
{
    UnRef();

    m_refData = new wxFontRefData(pointSize, family, style, weight,
                                  underlined, face, encoding);

    return true;
}

wxFont::wxFont(const wxString& fontdesc)
{
    wxNativeFontInfo info;
    if ( info.FromString(fontdesc) )
        (void)Create(info);
}

wxGDIRefData *wxFont::CreateGDIRefData() const
{
    return new wxFontRefData;
}

wxGDIRefData *wxFont::CloneGDIRefData(const wxGDIRefData *data) const
{
    return new wxFontRefData(*(wxFontRefData *)data);
}

// ----------------------------------------------------------------------------
// accessors
// ----------------------------------------------------------------------------

double wxFont::GetFractionalPointSize() const
{
    wxCHECK_MSG( IsOk(), 0, wxT("invalid font") );

    return M_FONTDATA->m_info.pointSize;
}

wxString wxFont::GetFaceName() const
{
    wxCHECK_MSG( IsOk(), wxEmptyString, wxT("invalid font") );

    if ( !M_FONTDATA->m_info.faceName.empty() )
        return M_FONTDATA->m_info.faceName;

    // Return the name of the font actually used.
    Str255 name;
    GetFontName(M_FONTDATA->GetFontID(), name);
    return wxMacMakeStringFromPascal(name);
}

wxFontFamily wxFont::DoGetFamily() const
{
    return M_FONTDATA->m_info.family;
}

wxFontStyle wxFont::GetStyle() const
{
    wxCHECK_MSG( IsOk(), wxFONTSTYLE_MAX, wxT("invalid font") );

    return M_FONTDATA->m_info.style;
}

int wxFont::GetNumericWeight() const
{
    wxCHECK_MSG( IsOk(), wxFONTWEIGHT_MAX, wxT("invalid font") );

    return M_FONTDATA->m_info.weight;
}

bool wxFont::GetUnderlined() const
{
    wxCHECK_MSG( IsOk(), false, wxT("invalid font") );

    return M_FONTDATA->m_info.underlined;
}

wxFontEncoding wxFont::GetEncoding() const
{
    wxCHECK_MSG( IsOk(), wxFONTENCODING_DEFAULT, wxT("invalid font") );

    return M_FONTDATA->m_info.encoding;
}

bool wxFont::IsFixedWidth() const
{
    wxCHECK_MSG( IsOk(), false, wxT("invalid font") );

    wxCoord wNarrow, wWide;
    wxRetro68GetTextExtent(*this, "i", &wNarrow, nullptr, nullptr, nullptr);
    wxRetro68GetTextExtent(*this, "W", &wWide, nullptr, nullptr, nullptr);

    return wNarrow == wWide;
}

const wxNativeFontInfo *wxFont::GetNativeFontInfo() const
{
    wxCHECK_MSG( IsOk(), nullptr, wxT("invalid font") );

    return &M_FONTDATA->m_info;
}

short wxFont::GetMacFontID() const
{
    wxCHECK_MSG( IsOk(), systemFont, wxT("invalid font") );

    return M_FONTDATA->GetFontID();
}

short wxFont::GetMacFontSize() const
{
    wxCHECK_MSG( IsOk(), wxMAC_DEFAULT_FONT_SIZE, wxT("invalid font") );

    const int size = wxRound(M_FONTDATA->m_info.pointSize);
    return static_cast<short>(size > 0 ? size : wxMAC_DEFAULT_FONT_SIZE);
}

short wxFont::GetMacFontStyle() const
{
    wxCHECK_MSG( IsOk(), 0, wxT("invalid font") );

    const wxNativeFontInfo& info = M_FONTDATA->m_info;

    short style = 0;
    if ( info.weight >= wxFONTWEIGHT_SEMIBOLD )
        style |= wxMAC_STYLE_BOLD;
    if ( info.style == wxFONTSTYLE_ITALIC || info.style == wxFONTSTYLE_SLANT )
        style |= wxMAC_STYLE_ITALIC;
    if ( info.underlined )
        style |= wxMAC_STYLE_UNDERLINE;

    return style;
}

// ----------------------------------------------------------------------------
// change font attributes
// ----------------------------------------------------------------------------

void wxFont::SetFractionalPointSize(double pointSize)
{
    AllocExclusive();
    M_FONTDATA->m_info.pointSize = pointSize;
}

void wxFont::SetFamily(wxFontFamily family)
{
    AllocExclusive();
    M_FONTDATA->m_info.family = family;
    M_FONTDATA->ResetFontID();
}

void wxFont::SetStyle(wxFontStyle style)
{
    AllocExclusive();
    M_FONTDATA->m_info.style = style;
}

void wxFont::SetNumericWeight(int weight)
{
    AllocExclusive();
    M_FONTDATA->m_info.weight = weight;
}

bool wxFont::SetFaceName(const wxString& faceName)
{
    AllocExclusive();
    M_FONTDATA->m_info.faceName = faceName;
    M_FONTDATA->ResetFontID();

    return wxFontBase::SetFaceName(faceName);
}

void wxFont::SetUnderlined(bool underlined)
{
    AllocExclusive();
    M_FONTDATA->m_info.underlined = underlined;
}

void wxFont::SetEncoding(wxFontEncoding encoding)
{
    AllocExclusive();
    M_FONTDATA->m_info.encoding = encoding;
}
