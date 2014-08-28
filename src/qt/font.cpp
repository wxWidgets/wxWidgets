/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/font.cpp
// Author:      Peter Most, Mariano Reingart, Javier Torres
// Copyright:   (c) 2009 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/font.h"
#include "wx/fontutil.h"
#include "wx/qt/private/utils.h"
#include "wx/qt/private/converter.h"

#include <QtGui/QFont>
#include <QtGui/QFontInfo>

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
        wxFontRefData()
        {
        }

        wxFontRefData(int size, QFont::StyleHint family, bool italic, QFont::Weight weight, bool underlined, QString face)
        {
            if (!face.isEmpty())
                m_qtFont.setFamily(face);
            else
            {
                m_qtFont.setStyleHint(family);
                // force qt to choose a font using the hint:
                m_qtFont.setFamily("");
            }
            m_qtFont.setItalic(italic);
            m_qtFont.setWeight(weight);
            m_qtFont.setUnderline(underlined);
            m_qtFont.setPointSize(size);
        }

        wxFontRefData( const wxFontRefData& data )
        : wxGDIRefData()
        {
            m_qtFont = data.m_qtFont;
        }

        bool operator == (const wxFontRefData& data) const
        {
            return m_qtFont == data.m_qtFont;
        }
        
        QFont m_qtFont;
};

#define M_FONTDATA ((wxFontRefData *)m_refData)->m_qtFont

wxFont::wxFont()
{
    m_refData = new wxFontRefData();
}

wxFont::wxFont(const wxFontInfo& info)
{
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
    M_FONTDATA = font;
}

wxFont::wxFont(const wxNativeFontInfo& info)
{
    Create(wxSize(0, info.pointSize), info.family, info.style, info.weight, info.underlined, info.faceName, info.encoding);
}

wxFont::wxFont(const QFont& font)
{
    m_refData = new wxFontRefData();

    M_FONTDATA = font;
}

wxFont::wxFont(int size,
       wxFontFamily family,
       wxFontStyle style,
       wxFontWeight weight,
       bool underlined,
       const wxString& face,
       wxFontEncoding encoding)
{
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
        wxFontEncoding WXUNUSED(encoding) )
{
    m_refData = new wxFontRefData(size.GetHeight(), ConvertFontFamily(family), style != wxFONTSTYLE_NORMAL,
                                  ConvertFontWeight(weight), underlined, wxQtConvertString(face));

    return true;
}

int wxFont::GetPointSize() const
{
    return M_FONTDATA.pointSize();
}

wxFontStyle wxFont::GetStyle() const
{
    switch (M_FONTDATA.style())
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

wxFontWeight wxFont::GetWeight() const
{
    switch ( M_FONTDATA.weight() )
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

bool wxFont::GetUnderlined() const
{
    return M_FONTDATA.underline();
}

wxString wxFont::GetFaceName() const
{
    // use font info to get the matched face name (not the family given)
    QFontInfo info = QFontInfo(M_FONTDATA);
    return wxQtConvertString(info.family());
}

wxFontEncoding wxFont::GetEncoding() const
{
    QFontInfo info = QFontInfo(M_FONTDATA);
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    return wxFontEncoding();
}

const wxNativeFontInfo *wxFont::GetNativeFontInfo() const
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    return new wxNativeFontInfo();
}


void wxFont::SetPointSize( int pointSize )
{
    AllocExclusive();
    M_FONTDATA.setPointSize(pointSize);
}

bool wxFont::SetFaceName(const wxString& facename)
{
    AllocExclusive();
    M_FONTDATA.setFamily(wxQtConvertString(facename));
    // qt uses a "font matching algoritm" so the font will be allways valid
    return true;
}

void wxFont::SetFamily( wxFontFamily family )
{
    AllocExclusive();
    M_FONTDATA.setStyleHint(ConvertFontFamily(family));
    // reset the face name to force qt to choose a new font
    M_FONTDATA.setFamily("");
}

void wxFont::SetStyle( wxFontStyle style )
{
    AllocExclusive();
    M_FONTDATA.setItalic(style != wxFONTSTYLE_NORMAL);
}

void wxFont::SetWeight( wxFontWeight weight )
{
    AllocExclusive();
    M_FONTDATA.setWeight(ConvertFontWeight(weight));
}

void wxFont::SetUnderlined( bool underlined )
{
    AllocExclusive();
    M_FONTDATA.setUnderline(underlined);
}

void wxFont::SetEncoding(wxFontEncoding WXUNUSED(encoding))
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );
}

wxGDIRefData *wxFont::CreateGDIRefData() const
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );
    return NULL;
}

wxGDIRefData *wxFont::CloneGDIRefData(const wxGDIRefData *data) const
{
    return new wxFontRefData(*(wxFontRefData *)data);
}


QFont wxFont::GetHandle() const
{
    return M_FONTDATA;
}

wxFontFamily wxFont::DoGetFamily() const
{
    switch (M_FONTDATA.styleHint())
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
