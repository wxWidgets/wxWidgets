/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/font.cpp
// Purpose:     wxFont class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/font.h"

#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/utils.h"
    #include "wx/gdicmn.h"
#endif

#include "wx/fontutil.h"

#include "wx/fontutil.h"

#include "wx/mac/private.h"
#include <ATSUnicode.h>

IMPLEMENT_DYNAMIC_CLASS(wxFont, wxGDIObject)

class WXDLLEXPORT wxFontRefData: public wxGDIRefData
{
    friend class WXDLLEXPORT wxFont;
public:
    wxFontRefData()
        : m_fontId(0)
        , m_pointSize(10)
        , m_family(wxDEFAULT)
        , m_style(wxNORMAL)
        , m_weight(wxNORMAL)
        , m_underlined(false)
        , m_faceName(wxT("Geneva"))
        , m_encoding(wxFONTENCODING_DEFAULT)
        , m_macFontNum(0)
        , m_macFontSize(0)
        , m_macFontStyle(0)
        , m_macATSUFontID()
    {
        Init(10, wxDEFAULT, wxNORMAL, wxNORMAL, false,
             wxT("Geneva"), wxFONTENCODING_DEFAULT);
    }

    wxFontRefData(const wxFontRefData& data)
        : wxGDIRefData()
        , m_fontId(data.m_fontId)
        , m_pointSize(data.m_pointSize)
        , m_family(data.m_family)
        , m_style(data.m_style)
        , m_weight(data.m_weight)
        , m_underlined(data.m_underlined)
        , m_faceName(data.m_faceName)
        , m_encoding(data.m_encoding)
        , m_macFontNum(data.m_macFontNum)
        , m_macFontSize(data.m_macFontSize)
        , m_macFontStyle(data.m_macFontStyle)
        , m_macATSUFontID(data.m_macATSUFontID)
    {
        Init(data.m_pointSize, data.m_family, data.m_style, data.m_weight,
             data.m_underlined, data.m_faceName, data.m_encoding);
    }

    wxFontRefData(int size,
                  int family,
                  int style,
                  int weight,
                  bool underlined,
                  const wxString& faceName,
                  wxFontEncoding encoding)
        : m_fontId(0)
        , m_pointSize(size)
        , m_family(family)
        , m_style(style)
        , m_weight(weight)
        , m_underlined(underlined)
        , m_faceName(faceName)
        , m_encoding(encoding)
        , m_macFontNum(0)
        , m_macFontSize(0)
        , m_macFontStyle(0)
        , m_macATSUFontID(0)
    {
        Init(size, family, style, weight, underlined, faceName, encoding);
    }

    virtual ~wxFontRefData();
    void SetNoAntiAliasing( bool no = true ) { m_noAA = no; }
    bool GetNoAntiAliasing() const { return m_noAA; }

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
    int            m_pointSize;
    int            m_family;
    int            m_style;
    int            m_weight;
    bool           m_underlined;
    wxString       m_faceName;
    wxFontEncoding m_encoding;
    bool           m_noAA;      // No anti-aliasing

public:
    short       m_macFontNum;
    short       m_macFontSize;
    unsigned char  m_macFontStyle;
    wxUint32       m_macATSUFontID;

    wxNativeFontInfo  m_info;

public:
    void        MacFindFont() ;
};

#define M_FONTDATA ((wxFontRefData*)m_refData)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxFontRefData
// ----------------------------------------------------------------------------

void wxFontRefData::Init(int pointSize,
                         int family,
                         int style,
                         int weight,
                         bool underlined,
                         const wxString& faceName,
                         wxFontEncoding encoding)
{
    m_style = style;
    m_pointSize = pointSize;
    m_family = family;
    m_style = style;
    m_weight = weight;
    m_underlined = underlined;
    m_faceName = faceName;
    m_encoding = encoding;

    m_macFontNum = 0 ;
    m_macFontSize = 0;
    m_macFontStyle = 0;
    m_fontId = 0;
    m_noAA = false;
}

wxFontRefData::~wxFontRefData()
{
}

void wxFontRefData::MacFindFont()
{
    if( m_faceName.empty() )
    {
        switch( m_family )
        {
            case wxDEFAULT :
                m_macFontNum = ::GetAppFont() ;
                break ;
            case wxDECORATIVE :
                ::GetFNum( "\pTimes" , &m_macFontNum) ;
                break ;
            case wxROMAN :
                ::GetFNum( "\pTimes" , &m_macFontNum) ;
                break ;
            case wxSCRIPT :
                ::GetFNum( "\pTimes" , &m_macFontNum) ;
                break ;
            case wxSWISS :
                ::GetFNum( "\pGeneva" , &m_macFontNum) ;
                break ;
            case wxMODERN :
                ::GetFNum( "\pMonaco" , &m_macFontNum) ;
                break ;
        }
        Str255 name ;
        GetFontName( m_macFontNum , name ) ;
        m_faceName = wxMacMakeStringFromPascal( name ) ;
    }
    else
    {
        if ( m_faceName == wxT("systemfont") )
            m_macFontNum = ::GetSysFont() ;
        else if ( m_faceName == wxT("applicationfont") )
            m_macFontNum = ::GetAppFont() ;
        else
        {
            Str255 fontname ;
            wxMacStringToPascal( m_faceName , fontname ) ;
            ::GetFNum( fontname, &m_macFontNum);
        }
    }

    m_macFontStyle = 0;
    if (m_weight == wxBOLD)
         m_macFontStyle |= bold;
    if (m_style == wxITALIC || m_style == wxSLANT)
        m_macFontStyle |= italic;
    if (m_underlined)
        m_macFontStyle |= underline;
    m_macFontSize = m_pointSize ;

    //TODO:if we supply the style as an additional parameter we must make a testing
    //sequence in order to degrade gracefully while trying to maintain most of the style
    //information, meanwhile we just take the normal font and apply the features after
#ifdef __WXDEBUG__
    OSStatus status =
#endif // __WXDEBUG__
        ::ATSUFONDtoFontID(m_macFontNum, normal /*qdStyle*/, (UInt32*)&m_macATSUFontID);
    /*
    status = ATSUFindFontFromName ( (Ptr) m_faceName , strlen( m_faceName ) ,
        kFontFullName,    kFontMacintoshPlatform, kFontRomanScript , kFontNoLanguage  ,  (UInt32*)&m_macATSUFontID ) ;
    */
    wxASSERT_MSG( status == noErr , wxT("couldn't retrieve font identifier") ) ;
}

// ----------------------------------------------------------------------------
// wxFont
// ----------------------------------------------------------------------------
}

bool wxFont::Create(const wxNativeFontInfo& info)
{
    return Create(info.pointSize, info.family, info.style, info.weight,
                  info.underlined, info.faceName, info.encoding);
}

wxFont::wxFont(const wxString& fontdesc)
{
    wxNativeFontInfo info;
    if ( info.FromString(fontdesc) )
        (void)Create(info);
}

bool wxFont::Create(int pointSize,
                    int family,
                    int style,
                    int weight,
                    bool underlined,
                    const wxString& faceName,
                    wxFontEncoding encoding)
{
    UnRef();
    m_refData = new wxFontRefData(pointSize, family, style, weight,
                                  underlined, faceName, encoding);

    RealizeResource();

    return true;
}

wxFont::~wxFont()
{
}

bool wxFont::RealizeResource()
{
    M_FONTDATA->MacFindFont() ;
    return true;
}

void wxFont::SetEncoding(wxFontEncoding encoding)
{
    Unshare();

    M_FONTDATA->m_encoding = encoding;

    RealizeResource();
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

void wxFont::SetNoAntiAliasing( bool no )
{
    Unshare();

    M_FONTDATA->SetNoAntiAliasing( no );

    RealizeResource();
}

// ----------------------------------------------------------------------------
// accessors
// ----------------------------------------------------------------------------

// TODO: insert checks everywhere for M_FONTDATA == NULL!

int wxFont::GetPointSize() const
{
    return M_FONTDATA->m_pointSize;
}

int wxFont::GetFamily() const
{
    return M_FONTDATA->m_family;
}

int wxFont::GetStyle() const
{
    return M_FONTDATA->m_style;
}

int wxFont::GetWeight() const
{
    return M_FONTDATA->m_weight;
}

bool wxFont::GetUnderlined() const
{
    return M_FONTDATA->m_underlined;
}

wxString wxFont::GetFaceName() const
{
    wxString str;
    if ( M_FONTDATA )
        str = M_FONTDATA->m_faceName ;
    return str;
}

wxFontEncoding wxFont::GetEncoding() const
{
    return M_FONTDATA->m_encoding;
}

bool wxFont::GetNoAntiAliasing() const
{
    return M_FONTDATA->m_noAA;
}

short wxFont::GetMacFontNum() const
{
    return M_FONTDATA->m_macFontNum;
}

short wxFont::GetMacFontSize() const
{
    return M_FONTDATA->m_macFontSize;
}

wxByte wxFont::GetMacFontStyle() const
{
    return M_FONTDATA->m_macFontStyle;
}

wxUint32 wxFont::GetMacATSUFontID() const
{
    return M_FONTDATA->m_macATSUFontID;
}

const wxNativeFontInfo *wxFont::GetNativeFontInfo() const
{
    wxCHECK_MSG( Ok(), NULL, wxT("invalid font") );

    M_FONTDATA->m_info.InitFromFont(*this);

    return &(M_FONTDATA->m_info);
}
