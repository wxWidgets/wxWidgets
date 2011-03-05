/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/font.cpp
// Author:      Peter Most, Javier Torres
// Id:          $Id$
// Copyright:   (c) Peter Most, Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/font.h"
#include "wx/fontutil.h"
#include "wx/qt/utils.h"
#include "wx/qt/converter.h"

#include <QtGui/QFont>

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
                m_qtFont.setStyleHint(family);

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

wxFont::wxFont(const wxString& nativeFontInfoString)
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    m_refData = new wxFontRefData();
}

wxFont::wxFont(const wxNativeFontInfo& info)
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    m_refData = new wxFontRefData();
}

wxFont::wxFont(int size,
       wxFontFamily family,
       wxFontStyle style,
       wxFontWeight weight,
       bool underlined,
       const wxString& face,
       wxFontEncoding encoding)
{
    Create(size, family, style, weight, underlined, face, encoding);
}

wxFont::wxFont(const wxSize& pixelSize,
       wxFontFamily family,
       wxFontStyle style,
       wxFontWeight weight,
       bool underlined,
       const wxString& face,
       wxFontEncoding encoding)
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    m_refData = new wxFontRefData();
}

#if FUTURE_WXWIN_COMPATIBILITY_3_0
wxFont::wxFont(int size,
       int family,
       int style,
       int weight,
       bool underlined,
       const wxString& face,
       wxFontEncoding encoding)
{
    Create(size, (wxFontFamily)family, (wxFontStyle)style, (wxFontWeight)weight, underlined, face, encoding);
}

#endif

bool wxFont::Create(int size, wxFontFamily family, wxFontStyle style,
        wxFontWeight weight, bool underlined, const wxString& face,
        wxFontEncoding encoding )
{
    m_refData = new wxFontRefData(size, ConvertFontFamily(family), style != wxFONTSTYLE_NORMAL,
                                  ConvertFontWeight(weight), underlined, wxQtConvertString(face));

    return true;
}

int wxFont::GetPointSize() const
{
    return M_FONTDATA.pointSize();
}

wxFontFamily wxFont::GetFamily() const
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    return wxFontFamily();
}

wxFontStyle wxFont::GetStyle() const
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    return wxFontStyle();
}

wxFontWeight wxFont::GetWeight() const
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    return wxFontWeight();
}

bool wxFont::GetUnderlined() const
{
    return M_FONTDATA.underline();
}

wxString wxFont::GetFaceName() const
{
    return wxQtConvertString(M_FONTDATA.family());
}

wxFontEncoding wxFont::GetEncoding() const
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    return wxFontEncoding();
}

const wxNativeFontInfo *wxFont::GetNativeFontInfo() const
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    return NULL;
}


void wxFont::SetPointSize( int pointSize )
{
    AllocExclusive();
    M_FONTDATA.setPointSize(pointSize);
}

void wxFont::SetFamily( wxFontFamily family )
{
    AllocExclusive();
    M_FONTDATA.setStyleHint(ConvertFontFamily(family));
}

void wxFont::SetStyle( wxFontStyle style )
{
    AllocExclusive();
    M_FONTDATA.setItalic(style != wxFONTSTYLE_NORMAL);
}

void wxFont::SetWeight( wxFontWeight weight )
{
    AllocExclusive();
    M_FONTDATA.setItalic(ConvertFontWeight(weight));
}

void wxFont::SetUnderlined( bool underlined )
{
    AllocExclusive();
    M_FONTDATA.setItalic(underlined);
}

void wxFont::SetEncoding(wxFontEncoding encoding)
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
    return wxFONTFAMILY_DEFAULT;
}

