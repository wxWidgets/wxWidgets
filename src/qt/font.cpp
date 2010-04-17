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

IMPLEMENT_DYNAMIC_CLASS( wxFont, wxFontBase )

wxFont::wxFont()
{
}

wxFont::wxFont(const wxString& nativeFontInfoString)
{
}

wxFont::wxFont(const wxNativeFontInfo& info)
{
}

wxFont::wxFont(int size,
       wxFontFamily family,
       wxFontStyle style,
       wxFontWeight weight,
       bool underlined,
       const wxString& face,
       wxFontEncoding encoding)
{
}

wxFont::wxFont(const wxSize& pixelSize,
       wxFontFamily family,
       wxFontStyle style,
       wxFontWeight weight,
       bool underlined,
       const wxString& face,
       wxFontEncoding encoding)
{
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
}

#endif

int wxFont::GetPointSize() const
{
    return 0;
}

wxFontFamily wxFont::GetFamily() const
{
    return wxFontFamily();
}

wxFontStyle wxFont::GetStyle() const
{
    return wxFontStyle();
}

wxFontWeight wxFont::GetWeight() const
{
    return wxFontWeight();
}

bool wxFont::GetUnderlined() const
{
    return false;
}

wxString wxFont::GetFaceName() const
{
    return wxString();
}

wxFontEncoding wxFont::GetEncoding() const
{
    return wxFontEncoding();
}

const wxNativeFontInfo *wxFont::GetNativeFontInfo() const
{
    return NULL;
}


void wxFont::SetPointSize( int pointSize )
{
}

void wxFont::SetFamily( wxFontFamily family )
{
}

void wxFont::SetStyle( wxFontStyle style )
{
}

void wxFont::SetWeight( wxFontWeight weight )
{
}

void wxFont::SetUnderlined( bool underlined )
{
}

void wxFont::SetEncoding(wxFontEncoding encoding)
{
}

wxGDIRefData *wxFont::CreateGDIRefData() const
{
    return NULL;
}

wxGDIRefData *wxFont::CloneGDIRefData(const wxGDIRefData *data) const
{
    return NULL;
}


