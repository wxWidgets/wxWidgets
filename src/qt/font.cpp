/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/font.cpp
// Author:      Peter Most, Mariano Reingart, Javier Torres
// Copyright:   (c) 2009 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <QtGui/QFont>
#include <QtGui/QFontInfo>

#include "wx/font.h"
#include "wx/fontutil.h"
#include "wx/qt/private/utils.h"
#include "wx/qt/private/converter.h"


static QFont::StyleHint ConvertFontFamily(wxFontFamily family)
{
    switch (family)
    {
        case wxFONTFAMILY_DEFAULT:
            return QFont::AnyStyle;

        case wxFONTFAMILY_DECORATIVE:
            return QFont::Decorative;

        case wxFONTFAMILY_ROMAN:
            return QFont::Serif;

        case wxFONTFAMILY_SCRIPT:
            return QFont::Decorative;

        case wxFONTFAMILY_SWISS:
            return QFont::SansSerif;

        case wxFONTFAMILY_MODERN:
            return QFont::TypeWriter;

        case wxFONTFAMILY_TELETYPE:
            return QFont::TypeWriter;

        case wxFONTFAMILY_MAX:
            wxFAIL_MSG( "Invalid font family value" );
            break;
    }
    return QFont::AnyStyle;
}

static QFont::Weight ConvertFontWeight(wxFontWeight weight)
{
    switch (weight)
    {
        case wxFONTWEIGHT_NORMAL:
            return QFont::Normal;

        case wxFONTWEIGHT_LIGHT:
            return QFont::Light;

        case wxFONTWEIGHT_BOLD:
            return QFont::Bold;

        case wxFONTWEIGHT_MAX:
            wxFAIL_MSG( "Invalid font weight value" );
            break;
    }
    return QFont::Normal;
}

class wxFontRefData: public wxGDIRefData
{
    public:
        wxFontRefData() {}

        wxFontRefData( const wxFontRefData& data )
        : wxGDIRefData()
        {
            m_nativeFontInfo.m_qtFont = data.m_nativeFontInfo.m_qtFont;
        }

        bool operator == (const wxFontRefData& data) const
        {
            return m_nativeFontInfo.m_qtFont == data.m_nativeFontInfo.m_qtFont;
        }
        
    wxNativeFontInfo m_nativeFontInfo;
};

#define M_FONTDATA ((wxFontRefData *)m_refData)->m_nativeFontInfo

wxFont::wxFont()
{
    m_refData = new wxFontRefData();
}

wxFont::wxFont(const wxFontInfo& info)
{
    m_refData = new wxFontRefData();
    Create(wxSize(0, info.GetPointSize()),
           info.GetFamily(),
           info.GetStyle(),
           info.GetWeight(),
           info.IsUnderlined(),
           info.GetFaceName(),
           info.GetEncoding());

    SetStrikethrough(info.IsStrikethrough());

    wxSize pixelSize = info.GetPixelSize();
    if ( pixelSize != wxDefaultSize )
        SetPixelSize(pixelSize);
}

wxFont::wxFont(const wxString& nativeFontInfoString)
{
    m_refData = new wxFontRefData();

    QFont font;
    font.fromString(wxQtConvertString( nativeFontInfoString ));
    M_FONTDATA.m_qtFont = font;
}

wxFont::wxFont(const wxNativeFontInfo& info)
{
    m_refData = new wxFontRefData();

    M_FONTDATA.m_qtFont = info.m_qtFont;
}

wxFont::wxFont(const QFont& font)
{
    m_refData = new wxFontRefData();

    M_FONTDATA.m_qtFont = font;
}

wxFont::wxFont(int size,
       wxFontFamily family,
       wxFontStyle style,
       wxFontWeight weight,
       bool underlined,
       const wxString& face,
       wxFontEncoding encoding)
{
    m_refData = new wxFontRefData();
    Create(wxSize(0, size), family, style, weight, underlined, face, encoding);
}

wxFont::wxFont(const wxSize& pixelSize,
       wxFontFamily family,
       wxFontStyle style,
       wxFontWeight weight,
       bool underlined,
       const wxString& face,
       wxFontEncoding encoding)
{
    m_refData = new wxFontRefData();
    Create(pixelSize, family, style, weight, underlined, face, encoding);
}

wxFont::wxFont(int size,
       int family,
       int style,
       int weight,
       bool underlined,
       const wxString& face,
       wxFontEncoding encoding)
{
    m_refData = new wxFontRefData();
    Create(wxSize(0, size), (wxFontFamily)family, (wxFontStyle)style, (wxFontWeight)weight, underlined, face, encoding);
}


bool wxFont::Create(wxSize size, wxFontFamily family, wxFontStyle style,
        wxFontWeight weight, bool underlined, const wxString& face,
        wxFontEncoding WXUNUSED(encoding) )
{
    if (!face.empty())
        M_FONTDATA.SetFaceName(face);
    else
        M_FONTDATA.SetFamily(family);

    M_FONTDATA.SetStyle(style);
    M_FONTDATA.SetWeight(weight);
    M_FONTDATA.SetUnderlined(underlined);
    M_FONTDATA.SetPointSize(size.GetHeight());

    return true;
}

int wxFont::GetPointSize() const
{
    return M_FONTDATA.GetPointSize();
}

wxFontStyle wxFont::GetStyle() const
{
    return M_FONTDATA.GetStyle();
}

wxFontWeight wxFont::GetWeight() const
{
    return M_FONTDATA.GetWeight();
}

bool wxFont::GetUnderlined() const
{
    return M_FONTDATA.GetUnderlined();
}

wxString wxFont::GetFaceName() const
{
    return M_FONTDATA.GetFaceName();
}

wxFontEncoding wxFont::GetEncoding() const
{
    return M_FONTDATA.GetEncoding();
}

const wxNativeFontInfo *wxFont::GetNativeFontInfo() const
{
    return &M_FONTDATA;
}

void wxFont::SetPointSize( int pointSize )
{
    AllocExclusive();

    M_FONTDATA.SetPointSize(pointSize);
}

bool wxFont::SetFaceName(const wxString& facename)
{
    AllocExclusive();

    return M_FONTDATA.SetFaceName(facename);
}

void wxFont::SetFamily( wxFontFamily family )
{
    AllocExclusive();

    M_FONTDATA.SetFamily(family);
}

void wxFont::SetStyle( wxFontStyle style )
{
    AllocExclusive();

    M_FONTDATA.SetStyle(style);
}

void wxFont::SetWeight( wxFontWeight weight )
{
    AllocExclusive();

    M_FONTDATA.SetWeight(weight);
}

void wxFont::SetUnderlined( bool underlined )
{
    AllocExclusive();

    M_FONTDATA.SetUnderlined(underlined);
}

void wxFont::SetEncoding(wxFontEncoding encoding)
{
    AllocExclusive();

    M_FONTDATA.SetEncoding(encoding);
}

wxGDIRefData *wxFont::CreateGDIRefData() const
{
    return new wxFontRefData;
}

wxGDIRefData *wxFont::CloneGDIRefData(const wxGDIRefData *data) const
{
    return new wxFontRefData(*(wxFontRefData *)data);
}

QFont wxFont::GetHandle() const
{
    return M_FONTDATA.m_qtFont;
}

wxFontFamily wxFont::DoGetFamily() const
{
    return M_FONTDATA.GetFamily();
}

// ----------------------------------------------------------------------------
// wxNativeFontInfo
// ----------------------------------------------------------------------------

void wxNativeFontInfo::Init()
{
}

int wxNativeFontInfo::GetPointSize() const
{
    return m_qtFont.pointSize();
}

wxFontStyle wxNativeFontInfo::GetStyle() const
{
    switch (m_qtFont.style())
    {
        case QFont::StyleNormal:
            return wxFONTSTYLE_NORMAL;

        case QFont::StyleItalic:
            return wxFONTSTYLE_ITALIC;

        case QFont::StyleOblique:
            return wxFONTSTYLE_SLANT;
    }
    wxFAIL_MSG( "Invalid font style value" );
    return wxFontStyle();
}

wxFontWeight wxNativeFontInfo::GetWeight() const
{
    switch ( m_qtFont.weight() )
    {
        case QFont::Normal:
            return wxFONTWEIGHT_NORMAL;

        case QFont::Light:
            return wxFONTWEIGHT_LIGHT;

        case QFont::DemiBold:
        case QFont::Black:
        case QFont::Bold:
            return wxFONTWEIGHT_BOLD;
    }
    wxFAIL_MSG( "Invalid font weight value" );
    return wxFontWeight();
}

bool wxNativeFontInfo::GetUnderlined() const
{
    return m_qtFont.underline();
}

bool wxNativeFontInfo::GetStrikethrough() const
{
    return m_qtFont.strikeOut();
}

wxString wxNativeFontInfo::GetFaceName() const
{
    // use font info to get the matched face name (not the family given)
    QFontInfo info = QFontInfo(m_qtFont);
    return wxQtConvertString(info.family());
}

wxFontFamily wxNativeFontInfo::GetFamily() const
{
    switch (m_qtFont.styleHint())
    {
        case QFont::System:
        case QFont::AnyStyle:
            return wxFONTFAMILY_DEFAULT;

        case QFont::Fantasy:
        case QFont::Cursive:
        case QFont::Decorative:
            return wxFONTFAMILY_DECORATIVE;

        case QFont::Serif:
            return wxFONTFAMILY_ROMAN;

        case QFont::SansSerif:
            return wxFONTFAMILY_SWISS;

        case QFont::Monospace:
        case QFont::TypeWriter:
            return wxFONTFAMILY_TELETYPE;

    }
    return wxFONTFAMILY_UNKNOWN;
}

wxFontEncoding wxNativeFontInfo::GetEncoding() const
{
//    QFontInfo info = QFontInfo(m_qtFont);
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    return wxFONTENCODING_MAX;
}

void wxNativeFontInfo::SetPointSize(int pointsize)
{
    m_qtFont.setPointSize(pointsize);
}

void wxNativeFontInfo::SetStyle(wxFontStyle style)
{
    m_qtFont.setItalic(style == wxFONTSTYLE_ITALIC);
//case wxFONTSTYLE_SLANT:
//case wxFONTSTYLE_NORMAL:
}

void wxNativeFontInfo::SetWeight(wxFontWeight weight)
{
    m_qtFont.setWeight(ConvertFontWeight(weight));
}

void wxNativeFontInfo::SetUnderlined(bool underlined)
{
    m_qtFont.setUnderline(underlined);
}

void wxNativeFontInfo::SetStrikethrough(bool strikethrough)
{
    m_qtFont.setStrikeOut(strikethrough);
}

bool wxNativeFontInfo::SetFaceName(const wxString& facename)
{
    m_qtFont.setFamily(wxQtConvertString(facename));
    // qt uses a "font matching algoritm" so the font will be allways valid
    return true;
}

void wxNativeFontInfo::SetFamily(wxFontFamily family)
{
    m_qtFont.setStyleHint(ConvertFontFamily(family));
    // reset the face name to force qt to choose a new font
    m_qtFont.setFamily("");
}

void wxNativeFontInfo::SetEncoding(wxFontEncoding WXUNUSED(encoding))
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );
}

bool wxNativeFontInfo::FromString(const wxString& s)
{
    return m_qtFont.fromString( wxQtConvertString( s ) );
}

wxString wxNativeFontInfo::ToString() const
{
    return wxQtConvertString( m_qtFont.toString() );
}

bool wxNativeFontInfo::FromUserString(const wxString& s)
{
    return FromString(s);
}

wxString wxNativeFontInfo::ToUserString() const
{
    return ToString();
}
