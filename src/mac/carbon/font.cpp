/////////////////////////////////////////////////////////////////////////////
// Name:        font.cpp
// Purpose:     wxFont class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "font.h"
#endif

#include "wx/wxprec.h"

#include "wx/string.h"
#include "wx/font.h"
#include "wx/fontutil.h"
#include "wx/gdicmn.h"
#include "wx/utils.h"

#include "wx/fontutil.h"

#include "wx/mac/private.h"
#include <ATSUnicode.h>

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxFont, wxGDIObject)
#endif

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
        , m_underlined(FALSE)
        , m_faceName(wxT("Geneva"))
        , m_encoding(wxFONTENCODING_DEFAULT)
        , m_macFontNum(0)
        , m_macFontSize(0)
        , m_macFontStyle(0)
        , m_macATSUFontID(0)
        , m_macATSUStyle(0)
    {
        Init(10, wxDEFAULT, wxNORMAL, wxNORMAL, FALSE,
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
        , m_macATSUStyle(0)
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
        , m_macATSUStyle(0)
    {
        Init(size, family, style, weight, underlined, faceName, encoding);
    }

    virtual ~wxFontRefData();
    void SetNoAntiAliasing( bool no = TRUE ) { m_noAA = no; }
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
    bool            m_noAA;      // No anti-aliasing
    
public:
    short           m_macFontNum;
    short           m_macFontSize;
    Style           m_macFontStyle;
    
    // ATSU Font Information
    
    // this is splitted into an ATSU font id that may
    // contain some styles (special bold fonts etc) and
    // these are the additional qd styles that are not
    // included in the ATSU font id
    ATSUStyle       m_macATSUStyle ;
    ATSUFontID      m_macATSUFontID;
    Style           m_macATSUAdditionalQDStyles ;
    
    // for true themeing support we must store the correct font
    // information here, as this speeds up and optimizes rendering
    ThemeFontID     m_macThemeFontID ;
   
    wxNativeFontInfo  m_info;

public:
    void        MacFindFont() ;
};
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
    m_macATSUFontID = 0;
    m_macATSUAdditionalQDStyles = 0 ;
    m_macATSUStyle = NULL ;
    
    m_macThemeFontID = kThemeCurrentPortFont ;
    m_noAA = FALSE;
}

wxFontRefData::~wxFontRefData()
{
    if ( m_macATSUStyle )
    {
        ::ATSUDisposeStyle((ATSUStyle)m_macATSUStyle);
        m_macATSUStyle = NULL ;
    }
}

void wxFontRefData::MacFindFont()
{
    if ( m_macThemeFontID != kThemeCurrentPortFont ) 
    {
        Str255 fontName ;
    	GetThemeFont(m_macThemeFontID , GetApplicationScript() , fontName , &m_macFontSize , &m_macFontStyle ) ;
        m_faceName = wxMacMakeStringFromPascal( fontName ) ;
        if ( m_macFontStyle & bold )
            m_weight = wxBOLD ;
        else
            m_weight = wxNORMAL ;
        if ( m_macFontStyle & italic )
            m_style = wxITALIC ;
        if ( m_macFontStyle & underline )
            m_underlined = true ;
        ::GetFNum( fontName, &m_macFontNum);
        m_pointSize = m_macFontSize ;
    }
    else
    {
        if( m_faceName.Length() == 0 )
        {
            switch( m_family )
            {
                case wxDEFAULT :
                    m_macFontNum = ::GetAppFont() ;
                    break ;
                case wxSCRIPT :
                case wxROMAN :
                case wxDECORATIVE :
#ifdef __WXMAC_OSX__
                    ::GetFNum( "\pTimes" , &m_macFontNum) ;
#else
                    ::GetFNum( "\pTimes" , &m_macFontNum) ;
#endif
                    break ;
                case wxSWISS :
#ifdef __WXMAC_OSX__
                    ::GetFNum( "\pLucida Grande" , &m_macFontNum) ;
#else
                    ::GetFNum( "\pGeneva" , &m_macFontNum) ;
#endif
                    break ;
                case wxMODERN :
#ifdef __WXMAC_OSX__
                    ::GetFNum( "\pMonaco" , &m_macFontNum) ;
#else
                    ::GetFNum( "\pMonaco" , &m_macFontNum) ;
#endif
                    break ;
            }
            Str255 name ;
            ::GetFontName( m_macFontNum , name ) ;
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
    }

    // we try to get as much styles as possible into ATSU

    Fixed atsuSize = IntToFixed( m_macFontSize ) ;

    Style atsuStyle = normal ;
    verify_noerr(::ATSUFONDtoFontID(m_macFontNum, atsuStyle , (UInt32*)&m_macATSUFontID) ); 
    if ( m_macFontStyle & bold )
    {
        ATSUFontID test ;
        if ( ::ATSUFONDtoFontID(m_macFontNum, atsuStyle | bold , &test) == noErr )
        {
            atsuStyle |= bold ;
            m_macATSUFontID = test ;
        }
    }
    if ( m_macFontStyle & italic )
    {
        ATSUFontID test ;
        if ( ::ATSUFONDtoFontID(m_macFontNum, atsuStyle | italic , &test) == noErr )
        {
            atsuStyle |= italic ;
            m_macATSUFontID = test ;
        }
    }
    if ( m_macFontStyle & underline )
    {
        ATSUFontID test ;
        if ( ::ATSUFONDtoFontID(m_macFontNum, atsuStyle | underline , &test) == noErr )
        {
            atsuStyle |= underline ;
            m_macATSUFontID = test ;
        }
    }
    
    m_macATSUAdditionalQDStyles = m_macFontStyle & (~atsuStyle ) ;

    if ( m_macATSUStyle )
    {
        ::ATSUDisposeStyle((ATSUStyle)m_macATSUStyle);
        m_macATSUStyle = NULL ;
    }
    OSStatus status = ::ATSUCreateStyle((ATSUStyle *)&m_macATSUStyle) ;
    wxASSERT_MSG( status == noErr , wxT("couldn't create ATSU style") ) ;
    ATSUAttributeTag atsuTags[] =
    {
            kATSUFontTag ,
            kATSUSizeTag ,
            kATSUVerticalCharacterTag,
            kATSUQDBoldfaceTag ,
            kATSUQDItalicTag ,
            kATSUQDUnderlineTag ,
            kATSUQDCondensedTag ,
            kATSUQDExtendedTag ,
    } ;
    ByteCount atsuSizes[sizeof(atsuTags)/sizeof(ATSUAttributeTag)] =
    {
            sizeof( ATSUFontID ) ,
            sizeof( Fixed ) ,
            sizeof( ATSUVerticalCharacterType),
            sizeof( Boolean ) ,
            sizeof( Boolean ) ,
            sizeof( Boolean ) ,
            sizeof( Boolean ) ,
            sizeof( Boolean ) ,
    } ;
    Boolean kTrue = true ;
    Boolean kFalse = false ;

    ATSUVerticalCharacterType kHorizontal = kATSUStronglyHorizontal;
    ATSUAttributeValuePtr    atsuValues[sizeof(atsuTags)/sizeof(ATSUAttributeTag)] =
    {
            &m_macATSUFontID ,
            &atsuSize ,
            &kHorizontal,
            (m_macATSUAdditionalQDStyles & bold) ? &kTrue : &kFalse ,
            (m_macATSUAdditionalQDStyles & italic) ? &kTrue : &kFalse ,
            (m_macATSUAdditionalQDStyles & underline) ? &kTrue : &kFalse ,
            (m_macATSUAdditionalQDStyles & condense) ? &kTrue : &kFalse ,
            (m_macATSUAdditionalQDStyles & extend) ? &kTrue : &kFalse ,
    } ;
    status = ::ATSUSetAttributes((ATSUStyle)m_macATSUStyle, sizeof(atsuTags)/sizeof(ATSUAttributeTag) ,
        atsuTags, atsuSizes, atsuValues);

    wxASSERT_MSG( status == noErr , wxT("couldn't Modify ATSU style") ) ;
}

// ----------------------------------------------------------------------------
// wxFont
// ----------------------------------------------------------------------------

void wxFont::Init()
{
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

    return TRUE;
}

bool wxFont::MacCreateThemeFont(wxUint16 themeFontID )
{
    UnRef();
    m_refData = new wxFontRefData(12, wxDEFAULT , wxFONTSTYLE_NORMAL , wxFONTWEIGHT_NORMAL ,false, wxEmptyString, wxFONTENCODING_DEFAULT);
    M_FONTDATA->m_macThemeFontID = themeFontID ;
    RealizeResource();

    return TRUE;
}

wxFont::~wxFont()
{
}

bool wxFont::RealizeResource()
{
    M_FONTDATA->MacFindFont() ;
    return TRUE;
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

void wxFont::SetFaceName(const wxString& faceName)
{
    Unshare();

    M_FONTDATA->m_faceName = faceName;

    RealizeResource();
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
    wxCHECK_MSG( M_FONTDATA != NULL , NULL, wxT("invalid font") );
    return M_FONTDATA->m_pointSize;
}

int wxFont::GetFamily() const
{
    wxCHECK_MSG( M_FONTDATA != NULL , NULL, wxT("invalid font") );
    return M_FONTDATA->m_family;
}

int wxFont::GetStyle() const
{
    wxCHECK_MSG( M_FONTDATA != NULL , NULL, wxT("invalid font") );
    return M_FONTDATA->m_style;
}

int wxFont::GetWeight() const
{
    wxCHECK_MSG( M_FONTDATA != NULL , NULL, wxT("invalid font") );
    return M_FONTDATA->m_weight;
}

bool wxFont::GetUnderlined() const
{
    wxCHECK_MSG( M_FONTDATA != NULL , NULL, wxT("invalid font") );
    return M_FONTDATA->m_underlined;
}

wxString wxFont::GetFaceName() const
{
    wxCHECK_MSG( M_FONTDATA != NULL , wxEmptyString , wxT("invalid font") );
    return M_FONTDATA->m_faceName;
}

wxFontEncoding wxFont::GetEncoding() const
{
    wxCHECK_MSG( M_FONTDATA != NULL , wxFONTENCODING_DEFAULT , wxT("invalid font") );
    return M_FONTDATA->m_encoding;
}

bool wxFont::GetNoAntiAliasing() const
{
    wxCHECK_MSG( M_FONTDATA != NULL , NULL, wxT("invalid font") );
    return M_FONTDATA->m_noAA;
}

short wxFont::MacGetFontNum() const
{
    wxCHECK_MSG( M_FONTDATA != NULL , NULL, wxT("invalid font") );
    return M_FONTDATA->m_macFontNum;
}

short wxFont::MacGetFontSize() const
{
    wxCHECK_MSG( M_FONTDATA != NULL , NULL, wxT("invalid font") );
    return M_FONTDATA->m_macFontSize;
}

wxByte wxFont::MacGetFontStyle() const
{
    wxCHECK_MSG( M_FONTDATA != NULL , NULL, wxT("invalid font") );
    return M_FONTDATA->m_macFontStyle;
}

wxUint32 wxFont::MacGetATSUFontID() const
{
    wxCHECK_MSG( M_FONTDATA != NULL , NULL, wxT("invalid font") );
    return M_FONTDATA->m_macATSUFontID;
}

void* wxFont::MacGetATSUStyle() const
{
    wxCHECK_MSG( M_FONTDATA != NULL , NULL, wxT("invalid font") );
    return M_FONTDATA->m_macATSUStyle;
}

wxUint32 wxFont::MacGetATSUAdditionalQDStyles() const
{
    wxCHECK_MSG( M_FONTDATA != NULL , NULL, wxT("invalid font") );
    return M_FONTDATA->m_macATSUAdditionalQDStyles;
}

wxUint16 wxFont::MacGetThemeFontID() const 
{
    wxCHECK_MSG( M_FONTDATA != NULL , NULL, wxT("invalid font") );
    return M_FONTDATA->m_macThemeFontID;
}


const wxNativeFontInfo *wxFont::GetNativeFontInfo() const
{
    wxCHECK_MSG( M_FONTDATA != NULL , NULL, wxT("invalid font") );
    wxCHECK_MSG( Ok(), NULL, wxT("invalid font") );

    M_FONTDATA->m_info.InitFromFont(*this);

    return &(M_FONTDATA->m_info);
}

