/////////////////////////////////////////////////////////////////////////////
// Name:        common/fontcmn.cpp
// Purpose:     implementation of wxFontBase methods
// Author:      Vadim Zeitlin
// Modified by:
// Created:     20.09.99
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "fontbase.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/font.h"
#endif // WX_PRECOMP

#include "wx/tokenzr.h"

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxFontBase
// ----------------------------------------------------------------------------

wxFontEncoding wxFontBase::ms_encodingDefault = wxFONTENCODING_SYSTEM;

/* static */
wxFont *wxFontBase::New(int size,
                        int family,
                        int style,
                        int weight,
                        bool underlined,
                        const wxString& face,
                        wxFontEncoding encoding)
{
    return new wxFont(size, family, style, weight, underlined, face, encoding);
}

/* static */
wxFont *wxFontBase::New(const wxNativeFontInfo& info)
{
    return new wxFont(info);
}

wxNativeFontInfo wxFontBase::GetNativeFontInfo() const
{
#if !defined(__WXGTK__)
    wxNativeFontInfo fontInfo;

    fontInfo.pointSize = GetPointSize();
    fontInfo.family = GetFamily();
    fontInfo.style = GetStyle();
    fontInfo.weight = GetWeight();
    fontInfo.underlined = GetUnderlined();
    fontInfo.faceName = GetFaceName();
    fontInfo.encoding = GetEncoding();

    return fontInfo;
#else
    return wxNullNativeFontInfo;
#endif
}

void wxFontBase::SetNativeFontInfo(const wxNativeFontInfo& info)
{
#if !defined(__WXGTK__)
    SetPointSize(info.pointSize);
    SetFamily(info.family);
    SetStyle(info.style);
    SetWeight(info.weight);
    SetUnderlined(info.underlined);
    SetFaceName(info.faceName);
    SetEncoding(info.encoding);
#endif
}

wxFont& wxFont::operator=(const wxFont& font)
{
    if ( this != &font )
        Ref(font);

    return (wxFont &)*this;
}

// VZ: is it correct to compare pointers and not the contents? (FIXME)
bool wxFontBase::operator==(const wxFont& font) const
{
    return GetFontData() == font.GetFontData();
}

bool wxFontBase::operator!=(const wxFont& font) const
{
    return GetFontData() != font.GetFontData();
}

wxString wxFontBase::GetFamilyString() const
{
    wxCHECK_MSG( Ok(), wxT("wxDEFAULT"), wxT("invalid font") );

    switch ( GetFamily() )
    {
        case wxDECORATIVE:   return wxT("wxDECORATIVE");
        case wxROMAN:        return wxT("wxROMAN");
        case wxSCRIPT:       return wxT("wxSCRIPT");
        case wxSWISS:        return wxT("wxSWISS");
        case wxMODERN:       return wxT("wxMODERN");
        case wxTELETYPE:     return wxT("wxTELETYPE");
        default:             return wxT("wxDEFAULT");
    }
}

wxString wxFontBase::GetStyleString() const
{
    wxCHECK_MSG( Ok(), wxT("wxDEFAULT"), wxT("invalid font") );

    switch ( GetStyle() )
    {
        case wxNORMAL:   return wxT("wxNORMAL");
        case wxSLANT:    return wxT("wxSLANT");
        case wxITALIC:   return wxT("wxITALIC");
        default:         return wxT("wxDEFAULT");
    }
}

wxString wxFontBase::GetWeightString() const
{
    wxCHECK_MSG( Ok(), wxT("wxDEFAULT"), wxT("invalid font") );

    switch ( GetWeight() )
    {
        case wxNORMAL:   return wxT("wxNORMAL");
        case wxBOLD:     return wxT("wxBOLD");
        case wxLIGHT:    return wxT("wxLIGHT");
        default:         return wxT("wxDEFAULT");
    }
}

#if !defined(__WXGTK__)

// ----------------------------------------------------------------------------
// wxNativeFontInfo
// ----------------------------------------------------------------------------

// These are the generic forms of FromString()/ToString.
//
// convert to/from the string representation: format is
//      pointsize;family;style;weight;underlined;facename;encoding

bool wxNativeFontInfo::FromString(const wxString& s)
{
    long l;

    wxStringTokenizer tokenizer(s, _T(";"));

    wxString token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return FALSE;
    pointSize = (int)l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return FALSE;
    family = (int)l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return FALSE;
    style = (int)l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return FALSE;
    weight = (int)l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return FALSE;
    underlined = l != 0;

    faceName = tokenizer.GetNextToken();
    if( !faceName )
        return FALSE;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return FALSE;
    encoding = (wxFontEncoding)l;

    return TRUE;
}

wxString wxNativeFontInfo::ToString() const
{
    wxString s;

    s.Printf("%d;%d;%d;%d;%d;%s;%d",
             pointSize,
             family,
             style,
             weight,
             underlined,
             faceName.GetData(),
             (int)encoding);

    return s;
}

#endif

