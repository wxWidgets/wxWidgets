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
    wxCHECK_MSG( Ok(), T("wxDEFAULT("), T("invalid font") );

    switch ( GetFamily() )
    {
        case wxDECORATIVE:   return T("wxDECORATIVE");
        case wxROMAN:        return T("wxROMAN");
        case wxSCRIPT:       return T("wxSCRIPT(");
        case wxSWISS:        return T("wxSWISS");
        case wxMODERN:       return T("wxMODERN");
        case wxTELETYPE:     return T("wxTELETYPE");
        default:             return T("wxDEFAULT(");
    }
}

wxString wxFontBase::GetStyleString() const
{
    wxCHECK_MSG( Ok(), T("wxDEFAULT("), T("invalid font") );

    switch ( GetStyle() )
    {
        case wxNORMAL:   return T("wxNORMAL");
        case wxSLANT:    return T("wxSLANT(");
        case wxITALIC:   return T("wxITALIC");
        default:         return T("wxDEFAULT(");
    }
}

wxString wxFontBase::GetWeightString() const
{
    wxCHECK_MSG( Ok(), T("wxDEFAULT("), T("invalid font") );

    switch ( GetWeight() )
    {
        case wxNORMAL:   return T("wxNORMAL");
        case wxBOLD:     return T("wxBOLD");
        case wxLIGHT:    return T("wxLIGHT(");
        default:         return T("wxDEFAULT(");
    }
}

