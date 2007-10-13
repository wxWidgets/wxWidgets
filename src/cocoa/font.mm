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

#include "wx/fontutil.h"
#include "wx/encinfo.h"

class WXDLLEXPORT wxFontRefData: public wxGDIRefData
{
    friend class WXDLLIMPEXP_FWD_CORE wxFont;
public:
    wxFontRefData()
    :   m_fontId(0)
    {
        Init(10, wxDEFAULT, wxNORMAL, wxNORMAL, FALSE,
             wxT("Geneva"), wxFONTENCODING_DEFAULT);
    }

    wxFontRefData(const wxFontRefData& data)
    :   wxGDIRefData()
    ,   m_fontId(data.m_fontId)
    ,   m_info(data.m_info)
    {
    }

    wxFontRefData(const wxNativeFontInfo& info)
    :   wxGDIRefData()
    ,   m_fontId(0)
    ,   m_info(info)
    {}

    wxFontRefData(int size,
                  int family,
                  int style,
                  int weight,
                  bool underlined,
                  const wxString& faceName,
                  wxFontEncoding encoding)
    :   m_fontId(0)
    {
        Init(size, family, style, weight, underlined, faceName, encoding);
    }

    virtual ~wxFontRefData();
protected:
    // common part of all ctors
    void Init(int size,
              int family,
              int style,
              int weight,
              bool underlined,
              const wxString& faceName,
              wxFontEncoding encoding);

    // font characterstics
    int            m_fontId;
    wxNativeFontInfo m_info;

public:
};

IMPLEMENT_DYNAMIC_CLASS(wxFont, wxGDIObject)

void wxFontRefData::Init(int size, int family, int style, int weight, bool underlined, const wxString& faceName, wxFontEncoding encoding)
{
    m_info.pointSize = size;
    m_info.family = static_cast<wxFontFamily>(family);
    m_info.style = static_cast<wxFontStyle>(style);
    m_info.weight = static_cast<wxFontWeight>(weight);
    m_info.underlined = underlined;
    m_info.faceName = faceName;
    m_info.encoding = encoding;
}

wxFontRefData::~wxFontRefData()
{
    // TODO: delete font data
}

#define M_FONTDATA ((wxFontRefData*)m_refData)

bool wxFont::Create(const wxNativeFontInfo& nativeFontInfo)
{
    UnRef();
    m_refData = new wxFontRefData(nativeFontInfo);
    
    return true;
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
    wxCHECK_MSG( Ok(), 0, wxT("invalid font") );
    return M_FONTDATA->m_info.pointSize;
}

bool wxFont::GetUnderlined() const
{
    if(M_FONTDATA)
        return M_FONTDATA->m_info.underlined;
    else
        return false;
}

int wxFont::GetStyle() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid font") );
    return M_FONTDATA->m_info.style;
}

int wxFont::GetFamily() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid font") );
    return M_FONTDATA->m_info.family;
}

int wxFont::GetWeight() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid font") );
    return M_FONTDATA->m_info.weight;
}

const wxNativeFontInfo *wxFont::GetNativeFontInfo() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid font") );
    return &M_FONTDATA->m_info;
}

bool wxFont::Create(int pointSize, int family, int style, int weight, bool underlined, const wxString& faceName, wxFontEncoding encoding)
{
    UnRef();
    m_refData = new wxFontRefData(pointSize, family, style, weight, underlined, faceName, encoding);

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

    M_FONTDATA->m_info.pointSize = pointSize;

    RealizeResource();
}

void wxFont::SetFamily(int family)
{
    Unshare();

    M_FONTDATA->m_info.family = static_cast<wxFontFamily>(family);

    RealizeResource();
}

void wxFont::SetStyle(int style)
{
    Unshare();

    M_FONTDATA->m_info.style = static_cast<wxFontStyle>(style);

    RealizeResource();
}

void wxFont::SetWeight(int weight)
{
    Unshare();

    M_FONTDATA->m_info.weight = static_cast<wxFontWeight>(weight);

    RealizeResource();
}

bool wxFont::SetFaceName(const wxString& faceName)
{
    Unshare();

    M_FONTDATA->m_info.faceName = faceName;

    RealizeResource();

    return wxFontBase::SetFaceName(faceName);
}

void wxFont::SetUnderlined(bool underlined)
{
    Unshare();

    M_FONTDATA->m_info.underlined = underlined;

    RealizeResource();
}

/* New font system */
wxString wxFont::GetFaceName() const
{
    wxString str;
    if (M_FONTDATA)
        str = M_FONTDATA->m_info.faceName;
    return str;
}

// vim:sts=4:sw=4:et
