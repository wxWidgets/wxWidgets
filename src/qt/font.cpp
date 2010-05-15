/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/font.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/font.h"
#include "wx/fontutil.h"
#include "wx/qt/utils.h"

IMPLEMENT_DYNAMIC_CLASS( wxFont, wxFontBase )

wxFont::wxFont()
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    m_qtFont = new QFont();
}

wxFont::wxFont(const wxString& nativeFontInfoString)
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    m_qtFont = new QFont();
}

wxFont::wxFont(const wxNativeFontInfo& info)
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    m_qtFont = new QFont();
}

wxFont::wxFont(int size,
       wxFontFamily family,
       wxFontStyle style,
       wxFontWeight weight,
       bool underlined,
       const wxString& face,
       wxFontEncoding encoding)
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    m_qtFont = new QFont();
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

    m_qtFont = new QFont();
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
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    m_qtFont = new QFont();
}

#endif

bool wxFont::Create(int size, wxFontFamily family, wxFontStyle style,
        wxFontWeight weight, bool underlined, const wxString& face,
        wxFontEncoding encoding )
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    return false;
}

int wxFont::GetPointSize() const
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    return 0;
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
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    return false;
}

wxString wxFont::GetFaceName() const
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    return wxString();
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
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );
}

void wxFont::SetFamily( wxFontFamily family )
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );
}

void wxFont::SetStyle( wxFontStyle style )
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );
}

void wxFont::SetWeight( wxFontWeight weight )
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );
}

void wxFont::SetUnderlined( bool underlined )
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );
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
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );
    return NULL;
}


QFont *wxFont::GetHandle() const
{
    return m_qtFont;
}
