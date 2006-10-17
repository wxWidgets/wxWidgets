/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/font.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling and Julian Smart
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
    #include "wx/log.h"
    #include "wx/utils.h"
    #include "wx/settings.h"
    #include "wx/cmndata.h"
    #include "wx/gdicmn.h"
#endif

#include "wx/fontutil.h"
#include "wx/tokenzr.h"

#include "wx/gtk/private.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// the default size (in points) for the fonts
static const int wxDEFAULT_FONT_SIZE = 12;

// ----------------------------------------------------------------------------
// wxScaledFontList: maps the font sizes to the GDK fonts for the given font
// ----------------------------------------------------------------------------

WX_DECLARE_HASH_MAP(int, GdkFont *, wxIntegerHash, wxIntegerEqual,
                    wxScaledFontList);

// ----------------------------------------------------------------------------
// wxFontRefData
// ----------------------------------------------------------------------------

class wxFontRefData : public wxObjectRefData
{
public:
    // from broken down font parameters, also default ctor
    wxFontRefData(int size = -1,
                  int family = wxFONTFAMILY_DEFAULT,
                  int style = wxFONTSTYLE_NORMAL,
                  int weight = wxFONTWEIGHT_NORMAL,
                  bool underlined = false,
                  const wxString& faceName = wxEmptyString,
                  wxFontEncoding encoding = wxFONTENCODING_DEFAULT);

    // from XFLD
    wxFontRefData(const wxString& fontname);

    // copy ctor
    wxFontRefData( const wxFontRefData& data );

    virtual ~wxFontRefData();

    // do we have the native font info?
    bool HasNativeFont() const
    {
        // we always have a Pango font description
        return true;
    }

    // setters: all of them also take care to modify m_nativeFontInfo if we
    // have it so as to not lose the information not carried by our fields
    void SetPointSize(int pointSize);
    void SetFamily(int family);
    void SetStyle(int style);
    void SetWeight(int weight);
    void SetUnderlined(bool underlined);
    bool SetFaceName(const wxString& facename);
    void SetEncoding(wxFontEncoding encoding);

    void SetNoAntiAliasing( bool no = true ) { m_noAA = no; }
    bool GetNoAntiAliasing() const { return m_noAA; }

    // and this one also modifies all the other font data fields
    void SetNativeFontInfo(const wxNativeFontInfo& info);

protected:
    // common part of all ctors
    void Init(int pointSize,
              int family,
              int style,
              int weight,
              bool underlined,
              const wxString& faceName,
              wxFontEncoding encoding);

    // set all fields from (already initialized and valid) m_nativeFontInfo
    void InitFromNative();

private:
    // clear m_scaled_xfonts if any
    void ClearGdkFonts();

    int             m_pointSize;
    int             m_family,
                    m_style,
                    m_weight;
    bool            m_underlined;
    wxString        m_faceName;
    wxFontEncoding  m_encoding;
    bool            m_noAA;      // No anti-aliasing

    // The native font info, basicly an XFLD under GTK 1.2 and
    // the pango font description under GTK 2.0.
    wxNativeFontInfo m_nativeFontInfo;

    friend class wxFont;
};

// ----------------------------------------------------------------------------
// wxFontRefData
// ----------------------------------------------------------------------------

void wxFontRefData::Init(int pointSize,
                         int family,
                         int style,
                         int weight,
                         bool underlined,
                         const wxString& faceName,
                         wxFontEncoding encoding)
{
    m_family = family == wxFONTFAMILY_DEFAULT ? wxFONTFAMILY_SWISS : family;

    m_faceName = faceName;

    // we accept both wxDEFAULT and wxNORMAL here - should we?
    m_style = style == wxDEFAULT ? wxFONTSTYLE_NORMAL : style;
    m_weight = weight == wxDEFAULT ? wxFONTWEIGHT_NORMAL : weight;

    // and here, do we really want to forbid creation of the font of the size
    // 90 (the value of wxDEFAULT)??
    m_pointSize = pointSize == wxDEFAULT || pointSize == -1
                    ? wxDEFAULT_FONT_SIZE
                    : pointSize;

    m_underlined = underlined;
    m_encoding = encoding;

    m_noAA = false;

    // Create native font info
    m_nativeFontInfo.description = pango_font_description_new();

    // And set its values
    if (!m_faceName.empty())
    {
       pango_font_description_set_family( m_nativeFontInfo.description,
                                          wxGTK_CONV_SYS(m_faceName) );
    }
    else
    {
        switch (m_family)
        {
            case wxFONTFAMILY_MODERN:
            case wxFONTFAMILY_TELETYPE:
               pango_font_description_set_family( m_nativeFontInfo.description, "monospace" );
               break;
            case wxFONTFAMILY_ROMAN:
               pango_font_description_set_family( m_nativeFontInfo.description, "serif" );
               break;
            case wxFONTFAMILY_SWISS:
               // SWISS = sans serif
            default:
               pango_font_description_set_family( m_nativeFontInfo.description, "sans" );
               break;
        }
    }

    SetStyle( m_style );
    SetPointSize( m_pointSize );
    SetWeight( m_weight );
}

void wxFontRefData::InitFromNative()
{
    m_noAA = false;

    // Get native info
    PangoFontDescription *desc = m_nativeFontInfo.description;

    // init fields
    m_faceName = wxGTK_CONV_BACK( pango_font_description_get_family( desc ) );

    // Pango sometimes needs to have a size
    int pango_size = pango_font_description_get_size( desc );
    if (pango_size == 0)
        m_nativeFontInfo.SetPointSize(12);

    m_pointSize = m_nativeFontInfo.GetPointSize();
    m_style = m_nativeFontInfo.GetStyle();
    m_weight = m_nativeFontInfo.GetWeight();

    if (m_faceName == wxT("monospace"))
    {
        m_family = wxFONTFAMILY_TELETYPE;
    }
    else if (m_faceName == wxT("sans"))
    {
        m_family = wxFONTFAMILY_SWISS;
    }
    else if (m_faceName == wxT("serif"))
    {
        m_family = wxFONTFAMILY_ROMAN;
    }
    else
    {
        m_family = wxFONTFAMILY_UNKNOWN;
    }

    // Pango description are never underlined (?)
    m_underlined = false;

    // always with GTK+ 2
    m_encoding = wxFONTENCODING_UTF8;
}

wxFontRefData::wxFontRefData( const wxFontRefData& data )
             : wxObjectRefData()
{
    m_pointSize = data.m_pointSize;
    m_family = data.m_family;
    m_style = data.m_style;
    m_weight = data.m_weight;

    m_underlined = data.m_underlined;

    m_faceName = data.m_faceName;
    m_encoding = data.m_encoding;

    m_noAA = data.m_noAA;

    // Forces a copy of the internal data.  wxNativeFontInfo should probably
    // have a copy ctor and assignment operator to fix this properly but that
    // would break binary compatibility...
    m_nativeFontInfo.FromString(data.m_nativeFontInfo.ToString());
}

wxFontRefData::wxFontRefData(int size, int family, int style,
                             int weight, bool underlined,
                             const wxString& faceName,
                             wxFontEncoding encoding)
{
    Init(size, family, style, weight, underlined, faceName, encoding);
}

wxFontRefData::wxFontRefData(const wxString& fontname)
{
    m_nativeFontInfo.FromString( fontname );

    InitFromNative();
}

void wxFontRefData::ClearGdkFonts()
{
}

wxFontRefData::~wxFontRefData()
{
    ClearGdkFonts();
}

// ----------------------------------------------------------------------------
// wxFontRefData SetXXX()
// ----------------------------------------------------------------------------

void wxFontRefData::SetPointSize(int pointSize)
{
    m_pointSize = pointSize;

    m_nativeFontInfo.SetPointSize(pointSize);
}

void wxFontRefData::SetFamily(int family)
{
    m_family = family;

    // TODO: what are we supposed to do with m_nativeFontInfo here?
}

void wxFontRefData::SetStyle(int style)
{
    m_style = style;

    m_nativeFontInfo.SetStyle((wxFontStyle)style);
}

void wxFontRefData::SetWeight(int weight)
{
    m_weight = weight;

    m_nativeFontInfo.SetWeight((wxFontWeight)weight);
}

void wxFontRefData::SetUnderlined(bool underlined)
{
    m_underlined = underlined;

    // the XLFD doesn't have "underlined" field anyhow
}

bool wxFontRefData::SetFaceName(const wxString& facename)
{
    m_faceName = facename;

    return m_nativeFontInfo.SetFaceName(facename);
}

void wxFontRefData::SetEncoding(wxFontEncoding encoding)
{
    m_encoding = encoding;
}

void wxFontRefData::SetNativeFontInfo(const wxNativeFontInfo& info)
{
    // previously cached fonts shouldn't be used
    ClearGdkFonts();

    m_nativeFontInfo = info;

    // set all the other font parameters from the native font info
    InitFromNative();
}

// ----------------------------------------------------------------------------
// wxFont creation
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxFont, wxGDIObject)

wxFont::wxFont(const wxNativeFontInfo& info)
{
    Create( info.GetPointSize(),
            info.GetFamily(),
            info.GetStyle(),
            info.GetWeight(),
            info.GetUnderlined(),
            info.GetFaceName(),
            info.GetEncoding() );
}

bool wxFont::Create( int pointSize,
                     int family,
                     int style,
                     int weight,
                     bool underlined,
                     const wxString& face,
                     wxFontEncoding encoding)
{
    UnRef();

    m_refData = new wxFontRefData(pointSize, family, style, weight,
                                  underlined, face, encoding);

    return true;
}

bool wxFont::Create(const wxString& fontname)
{
    // VZ: does this really happen?
    if ( fontname.empty() )
    {
        *this = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);

        return true;
    }

    m_refData = new wxFontRefData(fontname);

    return true;
}

wxFont::~wxFont()
{
}

// ----------------------------------------------------------------------------
// accessors
// ----------------------------------------------------------------------------

int wxFont::GetPointSize() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid font") );

    return M_FONTDATA->HasNativeFont() ? M_FONTDATA->m_nativeFontInfo.GetPointSize()
                                       : M_FONTDATA->m_pointSize;
}

wxString wxFont::GetFaceName() const
{
    wxCHECK_MSG( Ok(), wxEmptyString, wxT("invalid font") );

    return M_FONTDATA->HasNativeFont() ? M_FONTDATA->m_nativeFontInfo.GetFaceName()
                                       : M_FONTDATA->m_faceName;
}

int wxFont::GetFamily() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid font") );

    int ret = M_FONTDATA->m_family;
    if (M_FONTDATA->HasNativeFont())
        // wxNativeFontInfo::GetFamily is expensive, must not call more than once
        ret = M_FONTDATA->m_nativeFontInfo.GetFamily();

    if (ret == wxFONTFAMILY_DEFAULT)
        ret = M_FONTDATA->m_family;

    return ret;
}

int wxFont::GetStyle() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid font") );

    return M_FONTDATA->HasNativeFont() ? M_FONTDATA->m_nativeFontInfo.GetStyle()
                                       : M_FONTDATA->m_style;
}

int wxFont::GetWeight() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid font") );

    return M_FONTDATA->HasNativeFont() ? M_FONTDATA->m_nativeFontInfo.GetWeight()
                                       : M_FONTDATA->m_weight;
}

bool wxFont::GetUnderlined() const
{
    wxCHECK_MSG( Ok(), false, wxT("invalid font") );

    return M_FONTDATA->m_underlined;
}

wxFontEncoding wxFont::GetEncoding() const
{
    wxCHECK_MSG( Ok(), wxFONTENCODING_SYSTEM, wxT("invalid font") );

    return M_FONTDATA->m_encoding;
}

bool wxFont::GetNoAntiAliasing() const
{
    wxCHECK_MSG( Ok(), false, wxT("invalid font") );

    return M_FONTDATA->m_noAA;
}

const wxNativeFontInfo *wxFont::GetNativeFontInfo() const
{
    wxCHECK_MSG( Ok(), (wxNativeFontInfo *)NULL, wxT("invalid font") );

    return &(M_FONTDATA->m_nativeFontInfo);
}

bool wxFont::IsFixedWidth() const
{
    wxCHECK_MSG( Ok(), false, wxT("invalid font") );

    return wxFontBase::IsFixedWidth();
}

// ----------------------------------------------------------------------------
// change font attributes
// ----------------------------------------------------------------------------

void wxFont::SetPointSize(int pointSize)
{
    AllocExclusive();

    M_FONTDATA->SetPointSize(pointSize);
}

void wxFont::SetFamily(int family)
{
    AllocExclusive();

    M_FONTDATA->SetFamily(family);
}

void wxFont::SetStyle(int style)
{
    AllocExclusive();

    M_FONTDATA->SetStyle(style);
}

void wxFont::SetWeight(int weight)
{
    AllocExclusive();

    M_FONTDATA->SetWeight(weight);
}

bool wxFont::SetFaceName(const wxString& faceName)
{
    AllocExclusive();

    return M_FONTDATA->SetFaceName(faceName) &&
           wxFontBase::SetFaceName(faceName);
}

void wxFont::SetUnderlined(bool underlined)
{
    AllocExclusive();

    M_FONTDATA->SetUnderlined(underlined);
}

void wxFont::SetEncoding(wxFontEncoding encoding)
{
    AllocExclusive();

    M_FONTDATA->SetEncoding(encoding);
}

void wxFont::DoSetNativeFontInfo( const wxNativeFontInfo& info )
{
    AllocExclusive();

    M_FONTDATA->SetNativeFontInfo( info );
}

void wxFont::SetNoAntiAliasing( bool no )
{
    AllocExclusive();

    M_FONTDATA->SetNoAntiAliasing( no );
}

wxObjectRefData* wxFont::CreateRefData() const
{
    return new wxFontRefData;
}

wxObjectRefData* wxFont::CloneRefData(const wxObjectRefData* data) const
{
    return new wxFontRefData(*wx_static_cast(const wxFontRefData*, data));
}
