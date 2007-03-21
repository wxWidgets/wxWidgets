/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/font.cpp
// Purpose:     wxFont class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/font.h"

#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/utils.h"
    #include "wx/intl.h"
    #include "wx/gdicmn.h"
    #include "wx/log.h"
#endif

#include "wx/fontutil.h"
#include "wx/graphics.h"

#include "wx/mac/private.h"

#ifndef __DARWIN__
#include <ATSUnicode.h>
#endif


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
        , m_faceName(wxT("applicationfont"))
        , m_encoding(wxFONTENCODING_DEFAULT)
        , m_macFontFamily(0)
        , m_macFontSize(0)
        , m_macFontStyle(0)
        , m_macATSUStyle(0)
        , m_macATSUFontID(0)
    {
        Init(m_pointSize, m_family, m_style, m_weight,
             m_underlined, m_faceName, m_encoding);
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
        , m_macFontFamily(data.m_macFontFamily)
        , m_macFontSize(data.m_macFontSize)
        , m_macFontStyle(data.m_macFontStyle)
        , m_macATSUStyle(0)
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
        , m_macFontFamily(0)
        , m_macFontSize(0)
        , m_macFontStyle(0)
        , m_macATSUStyle(0)
        , m_macATSUFontID(0)
    {
        Init(size, family, style, weight, underlined, faceName, encoding);
    }

    virtual ~wxFontRefData();

    void SetNoAntiAliasing( bool no = true )
    { m_noAA = no; }

    bool GetNoAntiAliasing() const
    { return m_noAA; }

    void MacFindFont();

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
    int             m_fontId;
    int             m_pointSize;
    int             m_family;
    int             m_style;
    int             m_weight;
    bool            m_underlined;
    wxString        m_faceName;
    wxFontEncoding  m_encoding;
    bool            m_noAA;      // No anti-aliasing

public:
    FMFontFamily    m_macFontFamily;
    FMFontSize      m_macFontSize;
    FMFontStyle     m_macFontStyle;

    // ATSU Font Information

    // this is split into an ATSU font id that may
    // contain some styles (special bold fonts etc) and
    // these are the additional qd styles that are not
    // included in the ATSU font id
    ATSUStyle       m_macATSUStyle ;
    ATSUFontID      m_macATSUFontID;
    FMFontStyle     m_macATSUAdditionalQDStyles ;

    // for true themeing support we must store the correct font
    // information here, as this speeds up and optimizes rendering
    ThemeFontID     m_macThemeFontID ;

    wxNativeFontInfo  m_info;
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

    m_macFontFamily = 0 ;
    m_macFontSize = 0;
    m_macFontStyle = 0;
    m_macATSUFontID = 0;
    m_macATSUAdditionalQDStyles = 0 ;
    m_macATSUStyle = NULL ;

    m_macThemeFontID = kThemeCurrentPortFont ;
    m_noAA = false;
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
    OSStatus status ;

    Str255 qdFontName ;
    if ( m_macThemeFontID != kThemeCurrentPortFont )
    {
        Style style ;
        GetThemeFont( m_macThemeFontID, GetApplicationScript(), qdFontName, &m_macFontSize, &style );
        m_macFontStyle = style ;
        m_faceName = wxMacMakeStringFromPascal( qdFontName );
        if ( m_macFontStyle & bold )
            m_weight = wxBOLD ;
        else
            m_weight = wxNORMAL ;
        if ( m_macFontStyle & italic )
            m_style = wxITALIC ;
        if ( m_macFontStyle & underline )
            m_underlined = true ;
        m_pointSize = m_macFontSize ;
#ifndef __LP64__
        m_macFontFamily = FMGetFontFamilyFromName( qdFontName );
#endif
    }
    else
    {
        if ( m_faceName.empty() )
        {
            if ( m_family == wxDEFAULT )
            {
#ifndef __LP64__
                m_macFontFamily = GetAppFont();
                FMGetFontFamilyName(m_macFontFamily,qdFontName);
                m_faceName = wxMacMakeStringFromPascal( qdFontName );
#endif
            }
            else
            {
                switch ( m_family )
                {
                    case wxSCRIPT :
                    case wxROMAN :
                    case wxDECORATIVE :
                        m_faceName = wxT("Times");
                        break ;

                    case wxSWISS :
                        m_faceName =  wxT("Lucida Grande");
                        break ;

                    case wxMODERN :
                        m_faceName =  wxT("Monaco");
                        break ;

                    default:
                        m_faceName =  wxT("Times");
                        break ;
                }
#ifndef __LP64__
                wxMacStringToPascal( m_faceName , qdFontName );
                m_macFontFamily = FMGetFontFamilyFromName( qdFontName );
                if ( m_macFontFamily == kInvalidFontFamily )
                {
                    wxLogDebug( wxT("ATSFontFamilyFindFromName failed for %s"), m_faceName.c_str() );
                    m_macFontFamily = GetAppFont();
                }
#endif
            }
        }
        else
        {
#ifndef __LP64__
            if ( m_faceName == wxT("systemfont") )
                m_macFontFamily = GetSysFont();
            else if ( m_faceName == wxT("applicationfont") )
                m_macFontFamily = GetAppFont();
            else
#else
            if ( m_faceName == wxT("systemfont") )
                m_faceName =  wxT("Lucida Grande");
            else if ( m_faceName == wxT("applicationfont") )
                m_faceName =  wxT("Lucida Grande");
#endif
            {
                wxMacCFStringHolder cf( m_faceName, wxLocale::GetSystemEncoding() );
                ATSFontFamilyRef atsfamily = ATSFontFamilyFindFromName( cf , kATSOptionFlagsDefault );
                
                // ATSFontFamilyRef is an unsigned type, so check against max
                // for an invalid value, not -1.
                if ( atsfamily == 0xffffffff  )
                {
                    wxLogDebug( wxT("ATSFontFamilyFindFromName failed for ") + m_faceName );
                    m_macFontFamily = GetAppFont();
                }
                else
                    m_macFontFamily = FMGetFontFamilyFromATSFontFamilyRef( atsfamily );
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


    // ATSUFontID and FMFont are equivalent
    FMFontStyle intrinsicStyle = 0 ;
#ifndef __LP64__
    status = FMGetFontFromFontFamilyInstance( m_macFontFamily , m_macFontStyle , &m_macATSUFontID , &intrinsicStyle);
    wxASSERT_MSG( status == noErr , wxT("couldn't get an ATSUFont from font family") );
#endif
    m_macATSUAdditionalQDStyles = m_macFontStyle & (~intrinsicStyle );

    if ( m_macATSUStyle )
    {
        ::ATSUDisposeStyle((ATSUStyle)m_macATSUStyle);
        m_macATSUStyle = NULL ;
    }

    status = ::ATSUCreateStyle((ATSUStyle *)&m_macATSUStyle);
    wxASSERT_MSG( status == noErr , wxT("couldn't create ATSU style") );

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
    };
    ByteCount atsuSizes[sizeof(atsuTags) / sizeof(ATSUAttributeTag)] =
    {
        sizeof( ATSUFontID ) ,
        sizeof( Fixed ) ,
        sizeof( ATSUVerticalCharacterType),
        sizeof( Boolean ) ,
        sizeof( Boolean ) ,
        sizeof( Boolean ) ,
        sizeof( Boolean ) ,
        sizeof( Boolean ) ,
    };

    Boolean kTrue = true ;
    Boolean kFalse = false ;

    Fixed atsuSize = IntToFixed( m_macFontSize );
    ATSUVerticalCharacterType kHorizontal = kATSUStronglyHorizontal;
    ATSUAttributeValuePtr    atsuValues[sizeof(atsuTags) / sizeof(ATSUAttributeTag)] =
    {
            &m_macATSUFontID ,
            &atsuSize ,
            &kHorizontal,
            (m_macATSUAdditionalQDStyles & bold) ? &kTrue : &kFalse ,
            (m_macATSUAdditionalQDStyles & italic) ? &kTrue : &kFalse ,
            (m_macATSUAdditionalQDStyles & underline) ? &kTrue : &kFalse ,
            (m_macATSUAdditionalQDStyles & condense) ? &kTrue : &kFalse ,
            (m_macATSUAdditionalQDStyles & extend) ? &kTrue : &kFalse ,
    };

    status = ::ATSUSetAttributes(
        (ATSUStyle)m_macATSUStyle,
        sizeof(atsuTags) / sizeof(ATSUAttributeTag) ,
        atsuTags, atsuSizes, atsuValues);

    wxASSERT_MSG( status == noErr , wxT("couldn't modify ATSU style") );
}

// ----------------------------------------------------------------------------
// wxFont
// ----------------------------------------------------------------------------

bool wxFont::Create(const wxNativeFontInfo& info)
{
    return Create(
        info.pointSize, info.family, info.style, info.weight,
        info.underlined, info.faceName, info.encoding );
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

    m_refData = new wxFontRefData(
        pointSize, family, style, weight,
        underlined, faceName, encoding);

    RealizeResource();

    return true;
}

bool wxFont::MacCreateThemeFont(wxUint16 themeFontID)
{
    UnRef();

    m_refData = new wxFontRefData(
        12, wxDEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL,
        false, wxEmptyString, wxFONTENCODING_DEFAULT );

    M_FONTDATA->m_macThemeFontID = themeFontID ;
    RealizeResource();

    return true;
}

wxFont::~wxFont()
{
}

bool wxFont::RealizeResource()
{
    M_FONTDATA->MacFindFont();

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
    wxCHECK_MSG( M_FONTDATA != NULL , 0, wxT("invalid font") );

    return M_FONTDATA->m_pointSize;
}

wxSize wxFont::GetPixelSize() const
{
#if wxUSE_GRAPHICS_CONTEXT
    // TODO: consider caching the value
    wxGraphicsContext* dc = wxGraphicsContext::CreateFromNative((CGContextRef) NULL);
    dc->SetFont(*(wxFont *)this,*wxBLACK);
    wxDouble width, height = 0;
    dc->GetTextExtent( wxT("g"), &width, &height, NULL, NULL);
    return wxSize((int)width, (int)height);
#else
    wxFontBase::GetPixelSize();
#endif
}

int wxFont::GetFamily() const
{
    wxCHECK_MSG( M_FONTDATA != NULL , 0, wxT("invalid font") );

    return M_FONTDATA->m_family;
}

int wxFont::GetStyle() const
{
    wxCHECK_MSG( M_FONTDATA != NULL , 0, wxT("invalid font") );

    return M_FONTDATA->m_style;
}

int wxFont::GetWeight() const
{
    wxCHECK_MSG( M_FONTDATA != NULL , 0, wxT("invalid font") );

    return M_FONTDATA->m_weight;
}

bool wxFont::GetUnderlined() const
{
    wxCHECK_MSG( M_FONTDATA != NULL , false, wxT("invalid font") );

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
    wxCHECK_MSG( M_FONTDATA != NULL , false, wxT("invalid font") );

    return M_FONTDATA->m_noAA;
}

short wxFont::MacGetFontNum() const
{
    wxCHECK_MSG( M_FONTDATA != NULL , 0, wxT("invalid font") );

    return M_FONTDATA->m_macFontFamily;
}

short wxFont::MacGetFontSize() const
{
    wxCHECK_MSG( M_FONTDATA != NULL , 0, wxT("invalid font") );

    return M_FONTDATA->m_macFontSize;
}

wxByte wxFont::MacGetFontStyle() const
{
    wxCHECK_MSG( M_FONTDATA != NULL , 0, wxT("invalid font") );

    return M_FONTDATA->m_macFontStyle;
}

wxUint32 wxFont::MacGetATSUFontID() const
{
    wxCHECK_MSG( M_FONTDATA != NULL , 0, wxT("invalid font") );

    return M_FONTDATA->m_macATSUFontID;
}

void * wxFont::MacGetATSUStyle() const
{
    wxCHECK_MSG( M_FONTDATA != NULL , NULL, wxT("invalid font") );

    return M_FONTDATA->m_macATSUStyle;
}

wxUint32 wxFont::MacGetATSUAdditionalQDStyles() const
{
    wxCHECK_MSG( M_FONTDATA != NULL , 0, wxT("invalid font") );

    return M_FONTDATA->m_macATSUAdditionalQDStyles;
}

wxUint16 wxFont::MacGetThemeFontID() const
{
    wxCHECK_MSG( M_FONTDATA != NULL , 0, wxT("invalid font") );

    return M_FONTDATA->m_macThemeFontID;
}

const wxNativeFontInfo * wxFont::GetNativeFontInfo() const
{
    wxCHECK_MSG( M_FONTDATA != NULL , NULL, wxT("invalid font") );
    wxCHECK_MSG( Ok(), NULL, wxT("invalid font") );

    M_FONTDATA->m_info.InitFromFont(*this);

    return &(M_FONTDATA->m_info);
}
