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

// Older versions of QT don't define all the QFont::Weight enum values, so just
// do it ourselves here for all case instead.
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
#define wxQFontEnumOrInt(a, b) a
#else
#define wxQFontEnumOrInt(a, b) b
#endif

enum
{
    wxQFont_Thin        = wxQFontEnumOrInt( QFont::Thin, 0 ),
    wxQFont_ExtraLight  = wxQFontEnumOrInt( QFont::ExtraLight, 12 ),
    wxQFont_Light       = QFont::Light,
    wxQFont_Normal      = QFont::Normal,
    wxQFont_Medium      = wxQFontEnumOrInt( QFont::Medium, 57 ),
    wxQFont_DemiBold    = QFont::DemiBold,
    wxQFont_Bold        = QFont::Bold,
    wxQFont_ExtraBold   = wxQFontEnumOrInt( QFont::ExtraBold, 81 ),
    wxQFont_Black       = QFont::Black
};

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

// Helper of ConvertFontWeight() and GetNumericWeight(): if a value lies in
// ]fromMin, fromMax] interval, then map it to [toMin, toMax] interval linearly
// and return true, otherwise return false and don't modify it.
static bool TryToMap(int& x, int fromMin, int fromMax, int toMin, int toMax)
{
    if ( x > fromMin && x <= fromMax )
    {
        x = (toMin*(fromMax - x) + toMax*(x - fromMin))/(fromMax - fromMin);

        return true;
    }

    return false;
}

static int ConvertFontWeight(int w)
{
    // Note that wxQFont_Thin is 0, so we can't have anything lighter than it.
    if ( TryToMap(w, wxFONTWEIGHT_INVALID, wxFONTWEIGHT_THIN,
                     wxQFont_Thin, wxQFont_Thin) ||
         TryToMap(w, wxFONTWEIGHT_THIN, wxFONTWEIGHT_EXTRALIGHT,
                     wxQFont_Thin, wxQFont_ExtraLight) ||
         TryToMap(w, wxFONTWEIGHT_EXTRALIGHT, wxFONTWEIGHT_LIGHT,
                     wxQFont_ExtraLight, wxQFont_Light) ||
         TryToMap(w, wxFONTWEIGHT_LIGHT, wxFONTWEIGHT_NORMAL,
                     wxQFont_Light, wxQFont_Normal) ||
         TryToMap(w, wxFONTWEIGHT_NORMAL, wxFONTWEIGHT_MEDIUM,
                     wxQFont_Normal, wxQFont_Medium) ||
         TryToMap(w, wxFONTWEIGHT_MEDIUM, wxFONTWEIGHT_SEMIBOLD,
                     wxQFont_Medium, wxQFont_DemiBold) ||
         TryToMap(w, wxFONTWEIGHT_SEMIBOLD, wxFONTWEIGHT_BOLD,
                     wxQFont_DemiBold, wxQFont_Bold) ||
         TryToMap(w, wxFONTWEIGHT_BOLD, wxFONTWEIGHT_EXTRABOLD,
                     wxQFont_Bold, wxQFont_ExtraBold) ||
         TryToMap(w, wxFONTWEIGHT_EXTRABOLD, wxFONTWEIGHT_HEAVY,
                     wxQFont_ExtraBold, wxQFont_Black) ||
         TryToMap(w, wxFONTWEIGHT_HEAVY, wxFONTWEIGHT_EXTRAHEAVY,
                     wxQFont_Black, 99) )
    {
        return w;
    }

    wxFAIL_MSG("invalid wxFont weight");

    return wxQFont_Normal;
}

class wxFontRefData: public wxGDIRefData
{
public:
    wxFontRefData() {}

    wxFontRefData(const wxFontInfo& info)
    {
        if ( info.HasFaceName() )
            m_nativeFontInfo.SetFaceName(info.GetFaceName());
        else
            m_nativeFontInfo.SetFamily(info.GetFamily());

        if ( info.IsUsingSizeInPixels() )
            m_nativeFontInfo.SetPixelSize(info.GetPixelSize());
        else
            m_nativeFontInfo.SetSizeOrDefault(info.GetFractionalPointSize());

        m_nativeFontInfo.SetStyle(info.GetStyle());
        m_nativeFontInfo.SetWeight(info.GetWeight());
        m_nativeFontInfo.SetUnderlined(info.IsUnderlined());
        m_nativeFontInfo.SetStrikethrough(info.IsStrikethrough());
    }

    wxFontRefData( const wxFontRefData& data )
    : wxGDIRefData()
    {
        m_nativeFontInfo.m_qtFont = data.m_nativeFontInfo.m_qtFont;
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
    m_refData = new wxFontRefData(info);
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
    Create(wxSize(0, size), (wxFontFamily)family, (wxFontStyle)style, (wxFontWeight)weight, underlined, face, encoding);
}


bool wxFont::Create(wxSize size, wxFontFamily family, wxFontStyle style,
        wxFontWeight weight, bool underlined, const wxString& face,
        wxFontEncoding encoding )
{
    UnRef();

    m_refData = new wxFontRefData(InfoFromLegacyParams(size.GetHeight(), family,
                                                       style, weight, underlined,
                                                       face, encoding));

    return true;
}

int wxFont::GetPointSize() const
{
    return M_FONTDATA.wxNativeFontInfo::GetPointSize();
}

float wxFont::GetFractionalPointSize() const
{
    return M_FONTDATA.GetFractionalPointSize();
}

wxSize wxFont::GetPixelSize() const
{
    return M_FONTDATA.GetPixelSize();
}

wxFontStyle wxFont::GetStyle() const
{
    return M_FONTDATA.GetStyle();
}

int wxFont::GetNumericWeight() const
{
    return M_FONTDATA.GetNumericWeight();
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

bool wxFont::GetStrikethrough() const
{
    return M_FONTDATA.GetStrikethrough();
}


void wxFont::SetFractionalPointSize(float pointSize)
{
    AllocExclusive();

    M_FONTDATA.SetFractionalPointSize(pointSize);
}

void wxFont::SetPixelSize(const wxSize& pixelSize)
{
    AllocExclusive();

    M_FONTDATA.SetPixelSize(pixelSize);
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

void wxFont::SetNumericWeight(int weight)
{
    AllocExclusive();

    M_FONTDATA.SetNumericWeight(weight);
}

void wxFont::SetUnderlined( bool underlined )
{
    AllocExclusive();

    M_FONTDATA.SetUnderlined(underlined);
}

void wxFont::SetStrikethrough(bool strikethrough)
{
    AllocExclusive();

    M_FONTDATA.SetStrikethrough(strikethrough);
}

void wxFont::SetEncoding(wxFontEncoding encoding)
{
    AllocExclusive();

    M_FONTDATA.SetEncoding(encoding);
}

bool wxFont::SetNativeFontInfo(const wxString& info)
{
    wxNativeFontInfo fontInfo;
    if ( !info.empty() && fontInfo.FromString(info) )
    {
        DoSetNativeFontInfo( fontInfo );
        return true;
    }

    return false;
}

void wxFont::DoSetNativeFontInfo(const wxNativeFontInfo& info)
{
    SetFractionalPointSize( info.GetPointSize() );
    SetFamily( info.GetFamily() );
    SetStyle( info.GetStyle() );
    SetNumericWeight( info.GetWeight() );
    SetUnderlined( info.GetUnderlined() );
    SetStrikethrough( info.GetStrikethrough() );
    SetFaceName( info.GetFaceName() );
    SetEncoding( info.GetEncoding() );
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
    m_wxFontWeight = wxFONTWEIGHT_NORMAL;
}

float wxNativeFontInfo::GetFractionalPointSize() const
{
    return m_qtFont.pointSizeF();
}

wxSize wxNativeFontInfo::GetPixelSize() const
{
    return wxSize( m_qtFont.pixelSize(), m_qtFont.pixelSize() );
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

int wxNativeFontInfo::GetNumericWeight() const
{
    return m_wxFontWeight;
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
    return wxFONTENCODING_UTF8;
}

void wxNativeFontInfo::SetFractionalPointSize(float pointsize)
{
    m_qtFont.setPointSizeF(pointsize);
}

void wxNativeFontInfo::SetPixelSize(const wxSize& size)
{
    m_qtFont.setPixelSize(size.GetHeight());
}

void wxNativeFontInfo::SetStyle(wxFontStyle style)
{
    switch(style)
    {
        case wxFONTSTYLE_ITALIC:
            m_qtFont.setStyle(QFont::StyleItalic);
            break;
        case wxFONTSTYLE_NORMAL:
            m_qtFont.setStyle(QFont::StyleNormal);
            break;
        case wxFONTSTYLE_SLANT:
            m_qtFont.setStyle(QFont::StyleOblique);
            break;
    }
}

void wxNativeFontInfo::SetNumericWeight(int weight)
{
    m_wxFontWeight = weight;
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
    // qt uses a "font matching algorithm" so the font will be always valid
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
