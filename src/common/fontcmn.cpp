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

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/font.h"
#endif // WX_PRECOMP

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxFontBase
// ----------------------------------------------------------------------------

wxFontEncoding wxFontBase::ms_encodingDefault = wxFONTENCODING_SYSTEM;

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
    wxCHECK_MSG( Ok(), _T("wxDEFAULT"), _T("invalid font") );

    switch ( GetFamily() )
    {
        case wxDECORATIVE:   return _T("wxDECORATIVE");
        case wxROMAN:        return _T("wxROMAN");
        case wxSCRIPT:       return _T("wxSCRIPT");
        case wxSWISS:        return _T("wxSWISS");
        case wxMODERN:       return _T("wxMODERN");
        case wxTELETYPE:     return _T("wxTELETYPE");
        default:             return _T("wxDEFAULT");
    }
}

wxString wxFontBase::GetStyleString() const
{
    wxCHECK_MSG( Ok(), _T("wxDEFAULT"), _T("invalid font") );

    switch ( GetStyle() )
    {
        case wxNORMAL:   return _T("wxNORMAL");
        case wxSLANT:    return _T("wxSLANT");
        case wxITALIC:   return _T("wxITALIC");
        default:         return _T("wxDEFAULT");
    }
}

wxString wxFontBase::GetWeightString() const
{
    wxCHECK_MSG( Ok(), _T("wxDEFAULT"), _T("invalid font") );

    switch ( GetWeight() )
    {
        case wxNORMAL:   return _T("wxNORMAL");
        case wxBOLD:     return _T("wxBOLD");
        case wxLIGHT:    return _T("wxLIGHT");
        default:         return _T("wxDEFAULT");
    }
}

