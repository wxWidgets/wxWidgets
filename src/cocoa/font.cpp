/////////////////////////////////////////////////////////////////////////////
// Name:        src/cocoa/font.cpp
// Purpose:     wxFont class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/font.h"

#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/gdicmn.h"
#endif

#include "wx/encinfo.h"

IMPLEMENT_DYNAMIC_CLASS(wxFont, wxGDIObject)

void wxFontRefData::Init(int size, int family, int style, int weight, bool underlined, const wxString& faceName, wxFontEncoding encoding)
{
    m_family = family;
    m_style = style;
    m_weight = weight;
    m_underlined = underlined;
    m_faceName = faceName;
    m_encoding = encoding;
}

wxFontRefData::~wxFontRefData()
{
    // TODO: delete font data
}

#define M_FONTDATA (wxFontRefData*)m_refData)

bool wxFont::Create(const wxNativeFontInfo&)
{
    return false;
}

void wxFont::SetEncoding(wxFontEncoding)
{
}

wxFontEncoding wxFont::GetEncoding() const
{
    return wxFontEncoding();
}

int wxFont::GetPointSize() const
{
    return 0;
}

bool wxFont::GetUnderlined() const
{
    return false;
}

int wxFont::GetStyle() const
{
    return 0;
}

int wxFont::GetFamily() const
{
    return 0;
}

int wxFont::GetWeight() const
{
    return 0;
}

const wxNativeFontInfo *wxFont::GetNativeFontInfo() const
{
    return NULL;
}

void wxGetNativeFontEncoding(wxFontEncoding, wxNativeEncodingInfo*);

bool wxFont::Create(int pointSize, int family, int style, int weight, bool underlined, const wxString& faceName, wxFontEncoding encoding)
{
    UnRef();
    m_refData = new wxFontRefData;

    M_FONTDATA->m_family = family;
    M_FONTDATA->m_style = style;
    M_FONTDATA->m_weight = weight;
    M_FONTDATA->m_pointSize = pointSize;
    M_FONTDATA->m_underlined = underlined;
    M_FONTDATA->m_faceName = faceName;

    RealizeResource();

    return true;
}

wxFont::~wxFont()
{
}

bool wxFont::RealizeResource()
{
    // TODO: create the font (if there is a native font object)
    return false;
}

void wxFont::Unshare()
{
    // Don't change shared data
    if (!m_refData)
    {
        m_refData = new wxFontRefData();
    }
    else
    {
        wxFontRefData* ref = new wxFontRefData(*(wxFontRefData*)m_refData);
        UnRef();
        m_refData = ref;
    }
}

void wxFont::SetPointSize(int pointSize)
{
    Unshare();

    M_FONTDATA->m_pointSize = pointSize;

    RealizeResource();
}

void wxFont::SetFamily(int family)
{
    Unshare();

    M_FONTDATA->m_family = family;

    RealizeResource();
}

void wxFont::SetStyle(int style)
{
    Unshare();

    M_FONTDATA->m_style = style;

    RealizeResource();
}

void wxFont::SetWeight(int weight)
{
    Unshare();

    M_FONTDATA->m_weight = weight;

    RealizeResource();
}

bool wxFont::SetFaceName(const wxString& faceName)
{
    Unshare();

    M_FONTDATA->m_faceName = faceName;

    RealizeResource();

    return wxFontBase::SetFaceName(faceName);
}

void wxFont::SetUnderlined(bool underlined)
{
    Unshare();

    M_FONTDATA->m_underlined = underlined;

    RealizeResource();
}

/* New font system */
wxString wxFont::GetFaceName() const
{
    wxString str;
    if (M_FONTDATA)
        str = M_FONTDATA->m_faceName ;
    return str;
}

// vim:sts=4:sw=4:et
