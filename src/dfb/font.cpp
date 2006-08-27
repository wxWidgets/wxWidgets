/////////////////////////////////////////////////////////////////////////////
// Name:        src/dfb/font.cpp
// Purpose:     wxFont implementation
// Author:      Vaclav Slavik
// Created:     2006-08-08
// RCS-ID:      $Id$
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

#include "wx/fontutil.h"
#include "wx/dfb/private.h"

// ----------------------------------------------------------------------------
// wxFontRefData
// ----------------------------------------------------------------------------

// FIXME: for now, always use single font
static IDirectFBFontPtr gs_font = NULL;
static unsigned gs_fontRefCnt = 0;

class wxFontRefData : public wxObjectRefData
{
public:
    wxFontRefData(int size = wxDEFAULT,
                  int family = wxDEFAULT,
                  int style = wxDEFAULT,
                  int weight = wxDEFAULT,
                  bool underlined = false,
                  const wxString& faceName = wxEmptyString,
                  wxFontEncoding encoding = wxFONTENCODING_DEFAULT)
    {
        if ( family == wxDEFAULT )
            family = wxSWISS;
        if ( style == wxDEFAULT )
            style = wxNORMAL;
        if ( weight == wxDEFAULT )
            weight = wxNORMAL;
        if ( size == wxDEFAULT )
            size = 12;

        m_info.family = (wxFontFamily)family;
        m_info.faceName = faceName;
        m_info.style = (wxFontStyle)style;
        m_info.weight = (wxFontWeight)weight;
        m_info.pointSize = size;
        m_info.underlined = underlined;
        m_info.encoding = encoding;

        // FIXME: always use default font for now
        if ( !gs_font )
        {
            IDirectFBPtr dfb(wxTheApp->GetDirectFBInterface());

            DFBFontDescription desc;
            desc.flags = (DFBFontDescriptionFlags)0;
            IDirectFBFontPtr f;
            if ( DFB_CALL( dfb->CreateFont(dfb, NULL, &desc, &f) ) )
                gs_font = f;
        }
        if ( gs_font ) // the above may fail
        {
            gs_fontRefCnt++;
            m_font = gs_font;
        }
    }

    wxFontRefData(const wxFontRefData& data)
    {
        m_info = data.m_info;
        m_font = data.m_font;
    }

    ~wxFontRefData()
    {
        if ( m_font )
        {
            m_font.Reset();
            // FIXME
            if ( --gs_fontRefCnt == 0 )
                gs_font = NULL;
        }
    }

    wxNativeFontInfo m_info;
    IDirectFBFontPtr m_font;
};


// ----------------------------------------------------------------------------
// wxFont
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxFont, wxGDIObject)

bool wxFont::Create(const wxNativeFontInfo& info)
{
    return Create(info.pointSize, info.family, info.style, info.weight,
                  info.underlined, info.faceName, info.encoding);
}

bool wxFont::Create(int pointSize,
                    int family,
                    int style,
                    int weight,
                    bool underlined,
                    const wxString& face,
                    wxFontEncoding encoding)
{
    m_refData = new wxFontRefData(pointSize, family, style, weight,
                                  underlined, face, encoding);
    return true;
}

wxObjectRefData *wxFont::CreateRefData() const
{
    return new wxFontRefData;
}

wxObjectRefData *wxFont::CloneRefData(const wxObjectRefData *data) const
{
    return new wxFontRefData(*(wxFontRefData *)data);
}


// ----------------------------------------------------------------------------
// accessors
// ----------------------------------------------------------------------------

IDirectFBFontPtr wxFont::GetDirectFBFont() const
{
    wxCHECK_MSG( Ok(), NULL, wxT("invalid font") );

    return M_FONTDATA->m_font;
}

int wxFont::GetPointSize() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid font") );

    return M_FONTDATA->m_info.pointSize;
}

wxString wxFont::GetFaceName() const
{
    wxCHECK_MSG( Ok(), wxEmptyString, wxT("invalid font") );

    return M_FONTDATA->m_info.faceName;
}

int wxFont::GetFamily() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid font") );

    return M_FONTDATA->m_info.family;
}

int wxFont::GetStyle() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid font") );

    return M_FONTDATA->m_info.style;
}

int wxFont::GetWeight() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid font") );

    return M_FONTDATA->m_info.weight;
}

bool wxFont::GetUnderlined() const
{
    wxCHECK_MSG( Ok(), false, wxT("invalid font") );

    return M_FONTDATA->m_info.underlined;
}


wxFontEncoding wxFont::GetEncoding() const
{
    wxCHECK_MSG( Ok(), wxFONTENCODING_DEFAULT, wxT("invalid font") );

    return M_FONTDATA->m_info.encoding;
}

bool wxFont::IsFixedWidth() const
{
    wxCHECK_MSG( Ok(), false, wxT("invalid font") );

    return true; // FIXME_DFB
}

const wxNativeFontInfo *wxFont::GetNativeFontInfo() const
{
    wxCHECK_MSG( Ok(), NULL, wxT("invalid font") );

    return &(M_FONTDATA->m_info);
}

// ----------------------------------------------------------------------------
// change font attributes
// ----------------------------------------------------------------------------

void wxFont::SetPointSize(int pointSize)
{
    AllocExclusive();

    M_FONTDATA->m_info.pointSize = pointSize;
}

void wxFont::SetFamily(int family)
{
    AllocExclusive();

    M_FONTDATA->m_info.family = (wxFontFamily)family;
}

void wxFont::SetStyle(int style)
{
    AllocExclusive();

    M_FONTDATA->m_info.style = (wxFontStyle)style;
}

void wxFont::SetWeight(int weight)
{
    AllocExclusive();

    M_FONTDATA->m_info.weight = (wxFontWeight)weight;
}

bool wxFont::SetFaceName(const wxString& faceName)
{
    AllocExclusive();

    M_FONTDATA->m_info.faceName = faceName;

    return wxFontBase::SetFaceName(faceName);
}

void wxFont::SetUnderlined(bool underlined)
{
    AllocExclusive();

    M_FONTDATA->m_info.underlined = underlined;
}

void wxFont::SetEncoding(wxFontEncoding encoding)
{
    AllocExclusive();

    M_FONTDATA->m_info.encoding = encoding;
}
