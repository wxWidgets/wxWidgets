/////////////////////////////////////////////////////////////////////////////
// Name:        src/dfb/font.cpp
// Purpose:     wxFont implementation
// Author:      Vaclav Slavik
// Created:     2006-08-08
// Copyright:   (c) 2006 REA Elektronik GmbH
// Licence:     wxWindows licence
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

#include "wx/font.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif

#include "wx/dfb/private.h"
#include "wx/private/fontmgr.h"

typedef wxFontMgrFontRefData wxFontRefData;
#define M_FONTDATA ((wxFontRefData*)m_refData)

// ----------------------------------------------------------------------------
// wxFont
// ----------------------------------------------------------------------------

bool wxFont::Create(const wxNativeFontInfo& info)
{
    return Create(info.pointSize, info.family, info.style, info.weight,
                  info.underlined, info.faceName, info.encoding);
}

bool wxFont::Create(int pointSize,
                    wxFontFamily family,
                    wxFontStyle style,
                    wxFontWeight weight,
                    bool underlined,
                    const wxString& face,
                    wxFontEncoding encoding)
{
    m_refData = new wxFontRefData(pointSize, family, style, weight,
                                  underlined, face, encoding);
    return true;
}

wxGDIRefData *wxFont::CreateGDIRefData() const
{
    return new wxFontRefData;
}

wxGDIRefData *wxFont::CloneGDIRefData(const wxGDIRefData *data) const
{
    return new wxFontRefData(*(wxFontRefData *)data);
}


// ----------------------------------------------------------------------------
// accessors
// ----------------------------------------------------------------------------

wxIDirectFBFontPtr wxFont::GetDirectFBFont(bool antialiased) const
{
    wxCHECK_MSG( IsOk(), NULL, wxT("invalid font") );

    // we don't support DC scaling yet, so use scale=1
    wxFontInstance *i = M_FONTDATA->GetFontInstance(1.0, antialiased);
    return i ? i->GetDirectFBFont() : wxIDirectFBFontPtr();
}

int wxFont::GetPointSize() const
{
    wxCHECK_MSG( IsOk(), 0, wxT("invalid font") );

    return M_FONTDATA->GetPointSize();
}

wxString wxFont::GetFaceName() const
{
    wxCHECK_MSG( IsOk(), wxEmptyString, wxT("invalid font") );

    return M_FONTDATA->GetFaceName();
}

wxFontFamily wxFont::DoGetFamily() const
{
    return M_FONTDATA->GetFamily();
}

wxFontStyle wxFont::GetStyle() const
{
    wxCHECK_MSG( IsOk(), wxFONTSTYLE_MAX, wxT("invalid font") );

    return M_FONTDATA->GetStyle();
}

wxFontWeight wxFont::GetWeight() const
{
    wxCHECK_MSG( IsOk(), wxFONTWEIGHT_MAX, wxT("invalid font") );

    return M_FONTDATA->GetWeight();
}

bool wxFont::GetUnderlined() const
{
    wxCHECK_MSG( IsOk(), false, wxT("invalid font") );

    return M_FONTDATA->GetUnderlined();
}


wxFontEncoding wxFont::GetEncoding() const
{
    wxCHECK_MSG( IsOk(), wxFONTENCODING_DEFAULT, wxT("invalid font") );

    return M_FONTDATA->GetEncoding();
}

bool wxFont::IsFixedWidth() const
{
    wxCHECK_MSG( IsOk(), false, wxT("invalid font") );

    return M_FONTDATA->IsFixedWidth();
}

const wxNativeFontInfo *wxFont::GetNativeFontInfo() const
{
    wxCHECK_MSG( IsOk(), NULL, wxT("invalid font") );

    return M_FONTDATA->GetNativeFontInfo();
}

// ----------------------------------------------------------------------------
// change font attributes
// ----------------------------------------------------------------------------

void wxFont::SetPointSize(int pointSize)
{
    AllocExclusive();
    M_FONTDATA->SetPointSize(pointSize);
}

void wxFont::SetFamily(wxFontFamily family)
{
    AllocExclusive();
    M_FONTDATA->SetFamily(family);
}

void wxFont::SetStyle(wxFontStyle style)
{
    AllocExclusive();
    M_FONTDATA->SetStyle(style);
}

void wxFont::SetWeight(wxFontWeight weight)
{
    AllocExclusive();
    M_FONTDATA->SetWeight(weight);
}

bool wxFont::SetFaceName(const wxString& faceName)
{
    AllocExclusive();
    M_FONTDATA->SetFaceName(faceName);
    return wxFontBase::SetFaceName(faceName);
}

void wxFont::SetUnderlined(bool underlined)
{
    AllocExclusive();
    M_FONTDATA->SetUnderlined(underlined);
}

void wxFont::SetEncoding(wxFontEncoding encoding)
{
    AllocExclusive();
    M_FONTDATA->SetEncoding(encoding);
}

