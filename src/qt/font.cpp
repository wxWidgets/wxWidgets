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
    // Note that QFont::Thin is 0, so we can't have anything lighter than it.
    if ( TryToMap(w, wxFONTWEIGHT_INVALID, wxFONTWEIGHT_THIN,
                     QFont::Thin, QFont::Thin) ||
         TryToMap(w, wxFONTWEIGHT_THIN, wxFONTWEIGHT_EXTRALIGHT,
                     QFont::Thin, QFont::ExtraLight) ||
         TryToMap(w, wxFONTWEIGHT_EXTRALIGHT, wxFONTWEIGHT_LIGHT,
                     QFont::ExtraLight, QFont::Light) ||
         TryToMap(w, wxFONTWEIGHT_LIGHT, wxFONTWEIGHT_NORMAL,
                     QFont::Light, QFont::Normal) ||
         TryToMap(w, wxFONTWEIGHT_NORMAL, wxFONTWEIGHT_MEDIUM,
                     QFont::Normal, QFont::Medium) ||
         TryToMap(w, wxFONTWEIGHT_MEDIUM, wxFONTWEIGHT_SEMIBOLD,
                     QFont::Medium, QFont::DemiBold) ||
         TryToMap(w, wxFONTWEIGHT_SEMIBOLD, wxFONTWEIGHT_BOLD,
                     QFont::DemiBold, QFont::Bold) ||
         TryToMap(w, wxFONTWEIGHT_BOLD, wxFONTWEIGHT_EXTRABOLD,
                     QFont::Bold, QFont::ExtraBold) ||
         TryToMap(w, wxFONTWEIGHT_EXTRABOLD, wxFONTWEIGHT_HEAVY,
                     QFont::ExtraBold, QFont::Black) ||
         TryToMap(w, wxFONTWEIGHT_HEAVY, wxFONTWEIGHT_EXTRAHEAVY,
                     QFont::Black, 99) )
    {
        return w;
    }

    wxFAIL_MSG("invalid wxFont weight");

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
    int pointSize = size.GetHeight();
    AccountForCompatValues(pointSize, style, weight);

    if (!face.empty())
        M_FONTDATA.SetFaceName(face);
    else
        M_FONTDATA.SetFamily(family);

    M_FONTDATA.SetStyle(style);
    M_FONTDATA.SetWeight(weight);
    M_FONTDATA.SetUnderlined(underlined);
    M_FONTDATA.SetPointSize(pointSize);

    return true;
}

float wxFont::GetFractionalPointSize() const
{
    return M_FONTDATA.GetFractionalPointSize();
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

void wxFont::SetFractionalPointSize(float pointSize)
{
    AllocExclusive();

    M_FONTDATA.SetFractionalPointSize(pointSize);
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

float wxNativeFontInfo::GetFractionalPointSize() const
{
    return m_qtFont.pointSizeF();
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
    int w = m_qtFont.weight();

    // Special case of QFont::Thin == 0.
    if ( w == QFont::Thin )
        return wxFONTWEIGHT_THIN;

    if ( TryToMap(w, QFont::Thin, QFont::ExtraLight,
                     wxFONTWEIGHT_THIN, wxFONTWEIGHT_EXTRALIGHT) ||
         TryToMap(w, QFont::ExtraLight, QFont::Light,
                     wxFONTWEIGHT_EXTRALIGHT, wxFONTWEIGHT_LIGHT) ||
         TryToMap(w, QFont::Light, QFont::Normal,
                     wxFONTWEIGHT_LIGHT, wxFONTWEIGHT_NORMAL) ||
         TryToMap(w, QFont::Normal, QFont::Medium,
                     wxFONTWEIGHT_NORMAL, wxFONTWEIGHT_MEDIUM) ||
         TryToMap(w, QFont::Medium, QFont::DemiBold,
                     wxFONTWEIGHT_MEDIUM, wxFONTWEIGHT_SEMIBOLD) ||
         TryToMap(w, QFont::DemiBold, QFont::Bold,
                     wxFONTWEIGHT_SEMIBOLD, wxFONTWEIGHT_BOLD) ||
         TryToMap(w, QFont::Bold, QFont::ExtraBold,
                     wxFONTWEIGHT_BOLD, wxFONTWEIGHT_EXTRABOLD) ||
         TryToMap(w, QFont::ExtraBold, QFont::Black,
                     wxFONTWEIGHT_EXTRABOLD, wxFONTWEIGHT_HEAVY) ||
         TryToMap(w, QFont::Black, 99,
                     wxFONTWEIGHT_HEAVY, wxFONTWEIGHT_EXTRAHEAVY) )
    {
        return w;
    }

    wxFAIL_MSG( "Invalid QFont weight" );

    return wxFONTWEIGHT_NORMAL;
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
    m_qtFont.setItalic(style == wxFONTSTYLE_ITALIC);
//case wxFONTSTYLE_SLANT:
//case wxFONTSTYLE_NORMAL:
}

void wxNativeFontInfo::SetNumericWeight(int weight)
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
