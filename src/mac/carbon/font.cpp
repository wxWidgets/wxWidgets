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
#endif

#include "wx/fontutil.h"
#include "wx/graphics.h"
#include "wx/log.h"
#include "wx/settings.h"

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
#ifdef __LP64__
#else
        , m_macFontFamily(0)
        , m_macFontSize(0)
        , m_macFontStyle(0)
        , m_macATSUFontID(0)
#endif
        , m_macATSUStyle(0)
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
#ifdef __LP64__
#else
        , m_macFontFamily(data.m_macFontFamily)
        , m_macFontSize(data.m_macFontSize)
        , m_macFontStyle(data.m_macFontStyle)
        , m_macATSUFontID(data.m_macATSUFontID)
#endif
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
#ifdef __LP64__
#else
        , m_macFontFamily(0)
        , m_macFontSize(0)
        , m_macFontStyle(0)
        , m_macATSUFontID(0)
#endif
        , m_macATSUStyle(0)
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
#ifndef __LP64__
    FMFontFamily    m_macFontFamily;
    FMFontSize      m_macFontSize;
    FMFontStyle     m_macFontStyle;

    // ATSU Font Information

    // this is split into an ATSU font id that may
    // contain some styles (special bold fonts etc) and
    // these are the additional qd styles that are not
    // included in the ATSU font id
    ATSUFontID      m_macATSUFontID;
    FMFontStyle     m_macATSUAdditionalQDStyles ;

    // for true themeing support we must store the correct font
    // information here, as this speeds up and optimizes rendering
    ThemeFontID     m_macThemeFontID ;
#else
    CTFontRef       m_macFontRef;
    CTFontUIFontType m_macUIFontType;
#endif
    ATSUStyle       m_macATSUStyle ;
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
    m_pointSize = (pointSize == -1) ? wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize() : pointSize;
    m_family = family;
    m_style = style;
    m_weight = weight;
    m_underlined = underlined;
    m_faceName = faceName;
    if ( encoding == wxFONTENCODING_DEFAULT )
        m_encoding = wxFont::GetDefaultEncoding();
    else
        m_encoding = encoding;
#ifdef __LP64__
    m_macUIFontType = kCTFontNoFontType;
    m_macFontRef = 0;
#else
    m_macFontFamily = 0 ;
    m_macFontSize = 0;
    m_macFontStyle = 0;
    m_macATSUFontID = 0;
    m_macATSUAdditionalQDStyles = 0 ;

    m_macThemeFontID = kThemeCurrentPortFont ;
#endif
    m_macATSUStyle = NULL ;
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
    OSStatus status = noErr;
    Str255 qdFontName ;

    wxCHECK_RET( m_pointSize > 0, wxT("Point size should not be zero.") );

#ifdef __LP64__
    if ( m_faceName.empty() && m_family == wxDEFAULT )
    {
        m_macUIFontType = kCTFontSystemFontType;
    }
    
    if ( m_macUIFontType != kCTFontNoFontType )
    {
        m_macFontRef = CTFontCreateUIFontForLanguage( m_macUIFontType, 0.0, NULL );
        wxMacCFStringHolder name( CTFontCopyFamilyName( m_macFontRef ) );
        m_faceName = name.AsString();
    }
    else
    {
        if ( m_faceName.empty() )
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
                case wxTELETYPE:
                    m_faceName =  wxT("Monaco");
                    break ;

                default:
                    m_faceName =  wxT("Times");
                    break ;
            }
        }

        wxMacCFStringHolder cf( m_faceName, wxLocale::GetSystemEncoding() );
        m_macFontRef = CTFontCreateWithName( cf, m_pointSize, NULL);
    }

    if ( m_macATSUStyle )
    {
        ::ATSUDisposeStyle((ATSUStyle)m_macATSUStyle);
        m_macATSUStyle = NULL ;
    }

    status = ::ATSUCreateStyle((ATSUStyle *)&m_macATSUStyle);
    wxASSERT_MSG( status == noErr , wxT("couldn't create ATSU style") );

    ATSUAttributeTag atsuTags[] =
    {
        kATSUSizeTag ,
        kATSUVerticalCharacterTag,
        kATSUQDBoldfaceTag ,
        kATSUQDItalicTag ,
        kATSUQDUnderlineTag ,
        kATSUQDCondensedTag ,
        kATSUQDExtendedTag ,
        kATSUFontTag ,
    };
    ByteCount atsuSizes[sizeof(atsuTags) / sizeof(ATSUAttributeTag)] =
    {
        sizeof( Fixed ) ,
        sizeof( ATSUVerticalCharacterType),
        sizeof( Boolean ) ,
        sizeof( Boolean ) ,
        sizeof( Boolean ) ,
        sizeof( Boolean ) ,
        sizeof( Boolean ) ,
        sizeof( ATSUFontID ) ,
    };

    Boolean kTrue = true ;
    Boolean kFalse = false ;

    Fixed atsuSize = IntToFixed( m_pointSize );
    short m_macATSUAdditionalQDStyles = 0;
    ATSUVerticalCharacterType kHorizontal = kATSUStronglyHorizontal;
    ATSUFontID atsuFontID = 0;
    int attributeCount = sizeof(atsuTags) / sizeof(ATSUAttributeTag) ;
    
    // attempt to add atsu font 
    status = ATSUFindFontFromName(m_faceName.c_str(), strlen(m_faceName.c_str()), kFontFamilyName, kFontNoPlatform, kFontNoScript, kFontNoLanguage, &atsuFontID);
    if ( status != noErr )
    {
        attributeCount--;
    }
    
    ATSUAttributeValuePtr    atsuValues[sizeof(atsuTags) / sizeof(ATSUAttributeTag)] =
    {
            &atsuSize ,
            &kHorizontal,
            (m_macATSUAdditionalQDStyles & bold) ? &kTrue : &kFalse ,
            (m_macATSUAdditionalQDStyles & italic) ? &kTrue : &kFalse ,
            (m_macATSUAdditionalQDStyles & underline) ? &kTrue : &kFalse ,
            (m_macATSUAdditionalQDStyles & condense) ? &kTrue : &kFalse ,
            (m_macATSUAdditionalQDStyles & extend) ? &kTrue : &kFalse ,
            &atsuFontID ,
      };

    status = ::ATSUSetAttributes( (ATSUStyle)m_macATSUStyle, attributeCount, atsuTags, atsuSizes, atsuValues);
    wxASSERT_MSG( status == noErr , wxString::Format(wxT("couldn't modify ATSU style. Status was %d"), (int) status).c_str() );
#else
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
        m_macFontFamily = FMGetFontFamilyFromName( qdFontName );
    }
    else
    {
        if ( m_faceName.empty() )
        {
            if ( m_family == wxDEFAULT )
            {
                m_macFontFamily = GetAppFont();
                FMGetFontFamilyName(m_macFontFamily,qdFontName);
                m_faceName = wxMacMakeStringFromPascal( qdFontName );
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
                    case wxTELETYPE:
                        m_faceName =  wxT("Monaco");
                        break ;

                    default:
                        m_faceName =  wxT("Times");
                        break ;
                }
                wxMacStringToPascal( m_faceName , qdFontName );
                m_macFontFamily = FMGetFontFamilyFromName( qdFontName );
                if ( m_macFontFamily == kInvalidFontFamily )
                {
                    wxLogDebug( wxT("ATSFontFamilyFindFromName failed for %s"), m_faceName.c_str() );
                    m_macFontFamily = GetAppFont();
                }
            }
        }
        else
        {
            if ( m_faceName == wxT("systemfont") )
                m_macFontFamily = GetSysFont();
            else if ( m_faceName == wxT("applicationfont") )
                m_macFontFamily = GetAppFont();
            else
            {
                wxMacCFStringHolder cf( m_faceName, wxLocale::GetSystemEncoding() );
                ATSFontFamilyRef atsfamily = ATSFontFamilyFindFromName( cf , kATSOptionFlagsDefault );
                if ( atsfamily == (ATSFontFamilyRef) -1 )
                {
                    wxLogDebug( wxT("ATSFontFamilyFindFromName failed for %s"), m_faceName.c_str() );
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
    status = FMGetFontFromFontFamilyInstance( m_macFontFamily , m_macFontStyle , &m_macATSUFontID , &intrinsicStyle);
    wxASSERT_MSG( status == noErr , wxT("couldn't get an ATSUFont from font family") );
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
#endif
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

#ifdef __LP64__

bool wxFont::MacCreateUIFont(wxUint32 ctFontType )
{
    UnRef();

    m_refData = new wxFontRefData(
        12, wxDEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL,
        false, wxEmptyString, wxFONTENCODING_DEFAULT );

    M_FONTDATA->m_macUIFontType = ctFontType ;
    RealizeResource();

    return true;
}

#endif

bool wxFont::MacCreateThemeFont(wxUint16 themeFontID)
{
#ifdef __LP64__
    return MacCreateUIFont(HIThemeGetUIFontType(themeFontID));
#else
    UnRef();

    m_refData = new wxFontRefData(
        12, wxDEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL,
        false, wxEmptyString, wxFONTENCODING_DEFAULT );

    M_FONTDATA->m_macThemeFontID = themeFontID ;
    RealizeResource();

    return true;
#endif
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
    delete dc;
    return wxSize((int)width, (int)height);
#else
    return wxFontBase::GetPixelSize();
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

#ifndef __LP64__

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
#else
const void * wxFont::MacGetCTFont() const
{
    wxCHECK_MSG( M_FONTDATA != NULL , 0, wxT("invalid font") );

    return M_FONTDATA->m_macFontRef;
}

// to be removed
void * wxFont::MacGetATSUStyle() const
{
    wxCHECK_MSG( M_FONTDATA != NULL , NULL, wxT("invalid font") );

    return M_FONTDATA->m_macATSUStyle;
}

#endif

const wxNativeFontInfo * wxFont::GetNativeFontInfo() const
{
    wxCHECK_MSG( M_FONTDATA != NULL , NULL, wxT("invalid font") );
    wxCHECK_MSG( Ok(), NULL, wxT("invalid font") );

    M_FONTDATA->m_info.InitFromFont(*this);

    return &(M_FONTDATA->m_info);
}
