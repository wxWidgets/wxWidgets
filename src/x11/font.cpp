/////////////////////////////////////////////////////////////////////////////
// Name:        src/x11/font.cpp
// Purpose:     wxFont class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __VMS
#pragma message disable nosimpint
#include "wx/vms_x_fix.h"
#endif

#ifdef __VMS
#pragma message enable nosimpint
#endif

#include "wx/font.h"

#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/utils.h"       // for wxGetDisplay()
    #include "wx/settings.h"
    #include "wx/gdicmn.h"
    #include "wx/wxcrtvararg.h" // for wxSscanf
#endif

#include "wx/fontutil.h"    // for wxNativeFontInfo
#include "wx/tokenzr.h"
#include "wx/fontenum.h"

#include "wx/x11/private.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// the default size (in points) for the fonts
static const int wxDEFAULT_FONT_SIZE = 12;


// ----------------------------------------------------------------------------
// helper functions
// ----------------------------------------------------------------------------

static wxFontWeight ParseWeightString(wxString s)
{
    s.MakeUpper();

    if (s == "THIN") return wxFONTWEIGHT_THIN;
    if (s == "EXTRALIGHT" || s == "ULTRALIGHT") return wxFONTWEIGHT_EXTRALIGHT;
    if (s == "LIGHT") return wxFONTWEIGHT_LIGHT;
    if (s == "NORMAL") return wxFONTWEIGHT_NORMAL;
    if (s == "MEDIUM") return wxFONTWEIGHT_MEDIUM;
    if (s == "DEMIBOLD" || s == "SEMIBOLD") return wxFONTWEIGHT_SEMIBOLD;
    if (s == "BOLD") return wxFONTWEIGHT_BOLD;
    if (s == "EXTRABOLD" || s == "ULTRABOLD") return wxFONTWEIGHT_EXTRABOLD;
    if (s == "BLACK" || s == "HEAVY") return wxFONTWEIGHT_HEAVY;
    if (s == "EXTRAHEAVY") return wxFONTWEIGHT_EXTRAHEAVY;

    return wxFONTWEIGHT_NORMAL;
}

// ----------------------------------------------------------------------------
// wxFontRefData
// ----------------------------------------------------------------------------

class wxFontRefData: public wxGDIRefData
{
friend class wxFont;

public:
    wxFontRefData(int size = wxDEFAULT,
                  wxFontFamily family = wxFONTFAMILY_DEFAULT,
                  wxFontStyle style = wxFONTSTYLE_NORMAL,
                  wxFontWeight weight = wxFONTWEIGHT_NORMAL,
                  bool underlined = false,
                  bool strikethrough = false,
                  const wxString& faceName = wxEmptyString,
                  wxFontEncoding encoding = wxFONTENCODING_DEFAULT);

    // copy cstr
    wxFontRefData(const wxFontRefData& data);

    // from XFLD
    wxFontRefData(const wxString& fontname);

    // setters: all of them also take care to modify m_nativeFontInfo if we
    // have it so as to not lose the information not carried by our fields
    void SetFractionalPointSize(double pointSize);
    void SetFamily(wxFontFamily family);
    void SetStyle(wxFontStyle style);
    void SetNumericWeight(int weight);
    void SetUnderlined(bool underlined);
    void SetStrikethrough(bool strikethrough);
    bool SetFaceName(const wxString& facename);
    void SetEncoding(wxFontEncoding encoding);

    // and this one also modifies all the other font data fields
    void SetNativeFontInfo(const wxNativeFontInfo& info);

protected:
    // common part of all ctors
    void Init(int size,
              wxFontFamily family,
              wxFontStyle style,
              wxFontWeight weight,
              bool underlined,
              bool strikethrough,
              const wxString& faceName,
              wxFontEncoding encoding);

    // set all fields from (already initialized and valid) m_nativeFontInfo
    void InitFromNative();

    // font attributes
    float         m_pointSize;
    wxFontFamily  m_family;
    wxFontStyle   m_style;
    int           m_weight;
    bool          m_underlined;
    bool          m_strikethrough;
    wxString      m_faceName;
    wxFontEncoding m_encoding;   // Unused in Unicode mode

    wxNativeFontInfo m_nativeFontInfo;
};

#define M_FONTDATA ((wxFontRefData*)m_refData)

// ----------------------------------------------------------------------------
// wxFontRefData
// ----------------------------------------------------------------------------

void wxFontRefData::Init(int pointSize,
                         wxFontFamily family,
                         wxFontStyle style,
                         wxFontWeight weight,
                         bool underlined,
                         bool strikethrough,
                         const wxString& faceName,
                         wxFontEncoding encoding)
{
    m_family = family == wxFONTFAMILY_DEFAULT ? wxFONTFAMILY_SWISS : family;

    m_faceName = faceName;

    // we accept both wxDEFAULT and wxNORMAL here - should we?
    m_style = static_cast<int>(style) == wxDEFAULT ? wxFONTSTYLE_NORMAL : style;
    m_weight = static_cast<int>(weight) == wxDEFAULT ? wxFONTWEIGHT_NORMAL : weight;

    m_underlined = underlined;
    m_strikethrough = strikethrough;
    m_encoding = encoding;

    if ( m_nativeFontInfo.description )
        pango_font_description_free(m_nativeFontInfo.description);

    // Create native font info
    m_nativeFontInfo.description = pango_font_description_new();

    // if a face name is specified, use it if it's available, otherwise use
    // just the family
    if ( faceName.empty() || !wxFontEnumerator::IsValidFacename(faceName) )
    {
        // TODO: scan system for valid fonts matching the given family instead
        //       of hardcoding them here
        switch ( m_family )
        {
            case wxFONTFAMILY_TELETYPE:
                m_faceName = wxT("monospace");
                break;

            case wxFONTFAMILY_ROMAN:
                m_faceName = wxT("serif");
                break;

            default:
                m_faceName = wxT("sans");
        }
    }
    else // specified face name is available, use it
    {
        m_faceName = faceName;
    }

    m_nativeFontInfo.SetFaceName(m_faceName);
    m_nativeFontInfo.SetNumericWeight(m_weight);
    m_nativeFontInfo.SetStyle((wxFontStyle)m_style);
    m_nativeFontInfo.SetUnderlined(underlined);

    SetFractionalPointSize(pointSize);
}

void wxFontRefData::InitFromNative()
{
    // Get native info
    PangoFontDescription *desc = m_nativeFontInfo.description;

    // init fields
    m_faceName = wxString::FromUTF8( pango_font_description_get_family( desc ) );

    m_pointSize = static_cast<float>(pango_font_description_get_size( desc )) / PANGO_SCALE;

    switch (pango_font_description_get_style( desc ))
    {
        case PANGO_STYLE_NORMAL:
            m_style = wxFONTSTYLE_NORMAL;
            break;
        case PANGO_STYLE_ITALIC:
            m_style = wxFONTSTYLE_ITALIC;
            break;
        case PANGO_STYLE_OBLIQUE:
            m_style = wxFONTSTYLE_SLANT;
            break;
    }

    m_weight = pango_font_description_get_weight( desc );
}

wxFontRefData::wxFontRefData( const wxFontRefData& data )
             : wxGDIRefData()
{
    m_pointSize = data.m_pointSize;
    m_family = data.m_family;
    m_style = data.m_style;
    m_weight = data.m_weight;

    m_underlined = data.m_underlined;

    m_faceName = data.m_faceName;
    m_encoding = data.m_encoding;

    m_nativeFontInfo = data.m_nativeFontInfo;
}

wxFontRefData::wxFontRefData(int size, wxFontFamily family, wxFontStyle style,
                             wxFontWeight weight, bool underlined, bool strikethrough,
                             const wxString& faceName,
                             wxFontEncoding encoding)
{
    Init(size, family, style, weight, underlined, strikethrough, faceName, encoding);
}

wxFontRefData::wxFontRefData(const wxString& fontname)
{
    // VZ: FromString() should really work in both cases, doesn't it?
    m_nativeFontInfo.FromString( fontname );

    InitFromNative();
}

// ----------------------------------------------------------------------------
// wxFontRefData SetXXX()
// ----------------------------------------------------------------------------

void wxFontRefData::SetFractionalPointSize(double pointSize)
{
    // NB: Pango doesn't support point sizes less than 1
    m_pointSize = pointSize == wxDEFAULT || pointSize < 1 ? wxDEFAULT_FONT_SIZE
                                                          : pointSize;

    m_nativeFontInfo.SetFractionalPointSize(m_pointSize);
}

void wxFontRefData::SetFamily(wxFontFamily family)
{
    m_family = family;

    // TODO: what are we supposed to do with m_nativeFontInfo here?
}

void wxFontRefData::SetStyle(wxFontStyle style)
{
    m_style = style;

    // Get native info
    PangoFontDescription *desc = m_nativeFontInfo.description;

    switch ( style )
    {
        case wxFONTSTYLE_ITALIC:
            pango_font_description_set_style( desc, PANGO_STYLE_ITALIC );
            break;
        case wxFONTSTYLE_SLANT:
            pango_font_description_set_style( desc, PANGO_STYLE_OBLIQUE );
            break;
        default:
            wxFAIL_MSG( wxT("unknown font style") );
            // fall through
        case wxFONTSTYLE_NORMAL:
            pango_font_description_set_style( desc, PANGO_STYLE_NORMAL );
            break;
    }
}

void wxFontRefData::SetNumericWeight(int weight)
{
    m_weight = weight;
}

void wxFontRefData::SetUnderlined(bool underlined)
{
    m_nativeFontInfo.SetUnderlined(underlined);
    // the XLFD doesn't have "underlined" field anyhow
}

void wxFontRefData::SetStrikethrough(bool strikethrough)
{
    m_nativeFontInfo.SetStrikethrough(strikethrough);
}

bool wxFontRefData::SetFaceName(const wxString& facename)
{
    m_faceName = facename;
    return true;
}

void wxFontRefData::SetEncoding(wxFontEncoding encoding)
{
    m_encoding = encoding;
}

void wxFontRefData::SetNativeFontInfo(const wxNativeFontInfo& info)
{
    m_nativeFontInfo = info;

    m_family = info.GetFamily();

    // set all the other font parameters from the native font info
    InitFromNative();
}

// ----------------------------------------------------------------------------
// wxFont
// ----------------------------------------------------------------------------

wxFont::wxFont(const wxNativeFontInfo& info)
{
    Create( info.GetPointSize(),
            info.GetFamily(),
            info.GetStyle(),
            info.GetWeight(),
            info.GetUnderlined(),
            info.GetFaceName(),
            info.GetEncoding() );

    if ( info.GetStrikethrough() )
        SetStrikethrough(true);
}

bool wxFont::Create(int pointSize,
                    wxFontFamily family,
                    wxFontStyle style,
                    wxFontWeight weight,
                    bool underlined,
                    const wxString& faceName,
                    wxFontEncoding encoding)
{
    UnRef();

    m_refData = new wxFontRefData(pointSize, family, style, weight,
                                  underlined, false, faceName, encoding);

    return true;
}

bool wxFont::Create(const wxString& fontname, wxFontEncoding WXUNUSED(enc))
{
    if( !fontname )
    {
        *this = wxSystemSettings::GetFont( wxSYS_DEFAULT_GUI_FONT);
        return true;
    }

    m_refData = new wxFontRefData();

    M_FONTDATA->m_nativeFontInfo.FromString( fontname );

    wxString tmp;

    wxStringTokenizer tn( fontname, wxT("-") );

    tn.GetNextToken();                           // skip initial empty token
    tn.GetNextToken();                           // foundry


    M_FONTDATA->m_faceName = tn.GetNextToken();  // family

    M_FONTDATA->m_weight = ParseWeightString(tn.GetNextToken()); // weight

    tmp = tn.GetNextToken().MakeUpper();        // slant
    if (tmp == wxT("I")) M_FONTDATA->m_style = wxFONTSTYLE_ITALIC;
    if (tmp == wxT("O")) M_FONTDATA->m_style = wxFONTSTYLE_ITALIC;

    tn.GetNextToken();                           // set width
    tn.GetNextToken();                           // add. style
    tn.GetNextToken();                           // pixel size

    tmp = tn.GetNextToken();                     // pointsize
    if (tmp != wxT("*"))
    {
        long num = wxStrtol (tmp.c_str(), (wxChar **) nullptr, 10);
        M_FONTDATA->m_pointSize = (int)(num / 10);
    }

    tn.GetNextToken();                           // x-res
    tn.GetNextToken();                           // y-res

    tmp = tn.GetNextToken().MakeUpper();         // spacing

    if (tmp == wxT("M"))
        M_FONTDATA->m_family = wxFONTFAMILY_MODERN;
    else if (M_FONTDATA->m_faceName == wxT("TIMES"))
        M_FONTDATA->m_family = wxFONTFAMILY_ROMAN;
    else if (M_FONTDATA->m_faceName == wxT("HELVETICA"))
        M_FONTDATA->m_family = wxFONTFAMILY_SWISS;
    else if (M_FONTDATA->m_faceName == wxT("LUCIDATYPEWRITER"))
        M_FONTDATA->m_family = wxFONTFAMILY_TELETYPE;
    else if (M_FONTDATA->m_faceName == wxT("LUCIDA"))
        M_FONTDATA->m_family = wxFONTFAMILY_DECORATIVE;
    else if (M_FONTDATA->m_faceName == wxT("UTOPIA"))
        M_FONTDATA->m_family = wxFONTFAMILY_SCRIPT;

    tn.GetNextToken();                           // avg width

    return true;
}

wxFont::~wxFont()
{
}

wxGDIRefData *wxFont::CreateGDIRefData() const
{
    return new wxFontRefData;
}

wxGDIRefData *wxFont::CloneGDIRefData(const wxGDIRefData *data) const
{
    return new wxFontRefData(*static_cast<const wxFontRefData *>(data));
}

// ----------------------------------------------------------------------------
// change the font attributes
// ----------------------------------------------------------------------------

void wxFont::Unshare()
{
    // Don't change shared data
    if (!m_refData)
    {
        m_refData = new wxFontRefData();
    }
    else
    {
        wxFontRefData* ref = new wxFontRefData(*(wxFontRefData*)m_refData);
        UnRef();
        m_refData = ref;
    }
}

// ----------------------------------------------------------------------------
// accessors
// ----------------------------------------------------------------------------

double wxFont::GetFractionalPointSize() const
{
    wxCHECK_MSG( IsOk(), 0, wxT("invalid font") );

    return M_FONTDATA->m_nativeFontInfo.GetFractionalPointSize();
}

wxString wxFont::GetFaceName() const
{
    wxCHECK_MSG( IsOk(), wxEmptyString, wxT("invalid font") );

    return M_FONTDATA->m_nativeFontInfo.GetFaceName();
}

wxFontFamily wxFont::DoGetFamily() const
{
    return M_FONTDATA->m_family;
}

wxFontStyle wxFont::GetStyle() const
{
    wxCHECK_MSG( IsOk(), wxFONTSTYLE_MAX, wxT("invalid font") );

    return M_FONTDATA->m_style;
}

int wxFont::GetNumericWeight() const
{
    wxCHECK_MSG( IsOk(), wxFONTWEIGHT_MAX, wxT("invalid font") );

    return M_FONTDATA->m_weight;
}

bool wxFont::GetUnderlined() const
{
    wxCHECK_MSG( IsOk(), false, wxT("invalid font") );

    return M_FONTDATA->m_nativeFontInfo.GetUnderlined();
}

bool wxFont::GetStrikethrough() const
{
    wxCHECK_MSG( IsOk(), false, wxT("invalid font") );

    return M_FONTDATA->m_nativeFontInfo.GetStrikethrough();
}

#if defined( __WXX11__ ) && ( wxUSE_PANGO == 0 )
bool wxNativeFontInfo::GetStrikethrough() const
{
   return false;
}
#endif

wxFontEncoding wxFont::GetEncoding() const
{
    wxCHECK_MSG( IsOk(), wxFONTENCODING_DEFAULT, wxT("invalid font") );

    // unicode didn't use font encoding
    return wxFONTENCODING_DEFAULT;

}

const wxNativeFontInfo *wxFont::GetNativeFontInfo() const
{
    wxCHECK_MSG( IsOk(), nullptr, wxT("invalid font") );

    return &(M_FONTDATA->m_nativeFontInfo);
}

bool wxFont::IsFixedWidth() const
{
    wxCHECK_MSG( IsOk(), false, wxT("invalid font") );

   return wxFontBase::IsFixedWidth();
}

// ----------------------------------------------------------------------------
// change font attributes
// ----------------------------------------------------------------------------

void wxFont::SetFractionalPointSize(double pointSize)
{
    Unshare();

    M_FONTDATA->SetFractionalPointSize(pointSize);
}

void wxFont::SetFamily(wxFontFamily family)
{
    Unshare();

    M_FONTDATA->SetFamily(family);
}

void wxFont::SetStyle(wxFontStyle style)
{
    Unshare();

    M_FONTDATA->SetStyle(style);
}

void wxFont::SetNumericWeight(int weight)
{
    Unshare();

    M_FONTDATA->SetNumericWeight(weight);
}

bool wxFont::SetFaceName(const wxString& faceName)
{
    Unshare();

    return M_FONTDATA->SetFaceName(faceName) &&
        wxFontBase::SetFaceName(faceName);
}

void wxFont::SetUnderlined(bool underlined)
{
    Unshare();

    M_FONTDATA->SetUnderlined(underlined);
}

void wxFont::SetStrikethrough(bool strikethrough)
{
    Unshare();

    M_FONTDATA->SetStrikethrough(strikethrough);
}

void wxFont::SetEncoding(wxFontEncoding encoding)
{
    Unshare();

    M_FONTDATA->SetEncoding(encoding);
}

void wxFont::DoSetNativeFontInfo( const wxNativeFontInfo& info )
{
    Unshare();

    M_FONTDATA->SetNativeFontInfo( info );
}

#if wxUSE_PANGO
// Although we don't use this function yet, but we must create it here.
// first, for the prepare the unicode drawing support in wxUniv/x11 port.
// If we use pango to draw the text, then we must set some attributes
// for pango layout, such as "strikethrough" and "underline".
bool wxFont::SetPangoAttrs(PangoLayout* layout) const
{
    if ( !IsOk() || !(GetUnderlined() || GetStrikethrough()) )
        return false;

    PangoAttrList* attrs = pango_attr_list_new();
    PangoAttribute* a;

    if ( GetUnderlined() )
    {
        a = pango_attr_underline_new(PANGO_UNDERLINE_SINGLE);
        pango_attr_list_insert(attrs, a);
    }
    if ( GetStrikethrough() )
    {
        a = pango_attr_strikethrough_new(true);
        pango_attr_list_insert(attrs, a);
    }

    pango_layout_set_attributes(layout, attrs);
    pango_attr_list_unref(attrs);

    return true;
}
#endif
