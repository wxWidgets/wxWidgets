/////////////////////////////////////////////////////////////////////////////
// Name:        font.cpp
// Purpose:     wxFont class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "font.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <stdio.h>
    #include "wx/setup.h"
    #include "wx/list.h"
    #include "wx/utils.h"
    #include "wx/app.h"
    #include "wx/font.h"
#endif // WX_PRECOMP

#include "wx/msw/private.h"

#if !USE_SHARED_LIBRARIES
    IMPLEMENT_DYNAMIC_CLASS(wxFont, wxGDIObject)

    #if wxUSE_PORTABLE_FONTS_IN_MSW
        IMPLEMENT_DYNAMIC_CLASS(wxFontNameDirectory, wxObject)
    #endif
#endif

// ----------------------------------------------------------------------------
// wxFontRefData - the internal description of the font
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxFontRefData: public wxGDIRefData
{
friend class WXDLLEXPORT wxFont;

public:
    wxFontRefData()
    {
        Init(12, wxDEFAULT, wxNORMAL, wxNORMAL, FALSE,
             "", wxFONTENCODING_DEFAULT);
    }

    wxFontRefData(const wxFontRefData& data)
    {
        Init(data.m_pointSize, data.m_family, data.m_style, data.m_weight,
             data.m_underlined, data.m_faceName, data.m_encoding);

        m_fontId = data.m_fontId;
    }

    wxFontRefData(int size,
                  int family,
                  int style,
                  int weight,
                  bool underlined,
                  const wxString& faceName,
                  wxFontEncoding encoding)
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

    // If TRUE, the pointer to the actual font is temporary and SHOULD NOT BE
    // DELETED by destructor
    bool          m_temporary;

    int           m_fontId;

    // font characterstics
    int           m_pointSize;
    int           m_family;
    int           m_style;
    int           m_weight;
    bool          m_underlined;
    wxString      m_faceName;
    wxFontEncoding m_encoding;

    // Windows font handle
    WXHFONT       m_hFont;
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

    m_fontId = 0;
    m_temporary = FALSE;

    m_hFont = 0;
}

wxFontRefData::~wxFontRefData()
{
    if ( m_hFont )
    {
        if ( !::DeleteObject((HFONT) m_hFont) )
        {
            wxLogLastError("DeleteObject(font)");
        }
    }
}

// ----------------------------------------------------------------------------
// wxFont
// ----------------------------------------------------------------------------

void wxFont::Init()
{
    if ( wxTheFontList )
        wxTheFontList->Append(this);
}

/* Constructor for a font. Note that the real construction is done
 * in wxDC::SetFont, when information is available about scaling etc.
 */
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

wxFont::~wxFont()
{
    if ( wxTheFontList )
        wxTheFontList->DeleteObject(this);
}

// ----------------------------------------------------------------------------
// real implementation
// ----------------------------------------------------------------------------

bool wxFont::RealizeResource()
{
    if ( GetResourceHandle() )
    {
        // VZ: the old code returned FALSE in this case, but it doesn't seem
        //     to make sense because the font _was_ created
        wxLogDebug(wxT("Calling wxFont::RealizeResource() twice"));

        return TRUE;
    }

    int ff_family = 0;
    wxString ff_face;

    switch ( M_FONTDATA->m_family )
    {
        case wxSCRIPT:
            ff_family = FF_SCRIPT ;
            ff_face = wxT("Script") ;
            break ;

        case wxDECORATIVE:
            ff_family = FF_DECORATIVE;
            break;

        case wxROMAN:
            ff_family = FF_ROMAN;
            ff_face = wxT("Times New Roman") ;
            break;

        case wxTELETYPE:
        case wxMODERN:
            ff_family = FF_MODERN;
            ff_face = wxT("Courier New") ;
            break;

        case wxSWISS:
            ff_family = FF_SWISS;
            ff_face = wxT("Arial") ;
            break;

        case wxDEFAULT:
        default:
            ff_family = FF_SWISS;
            ff_face = wxT("Arial") ; 
    }

    BYTE ff_italic;
    switch ( M_FONTDATA->m_style )
    {
        case wxITALIC:
        case wxSLANT:
            ff_italic = 1;
            break;

        default:
            wxFAIL_MSG(wxT("unknown font slant"));
            // fall through

        case wxNORMAL:
            ff_italic = 0;
    }

    int ff_weight = 0;
    switch ( M_FONTDATA->m_weight )
    {
        default:
            wxFAIL_MSG(wxT("unknown font weight"));
            // fall through

        case wxNORMAL:
            ff_weight = FW_NORMAL;
            break;

        case wxLIGHT:
            ff_weight = FW_LIGHT;
            break;

        case wxBOLD:
            ff_weight = FW_BOLD;
            break;
    }

    const wxChar* pzFace;
    if ( M_FONTDATA->m_faceName.IsEmpty() )
        pzFace = ff_face;
    else
        pzFace = M_FONTDATA->m_faceName ;

#if 0
    /* Always calculate fonts using the screen DC (is this the best strategy?)
     * There may be confusion if a font is selected into a printer
     * DC (say), because the height will be calculated very differently.
     */
    // What sort of display is it?
    int technology = ::GetDeviceCaps(dc, TECHNOLOGY);

    int nHeight;

    if (technology != DT_RASDISPLAY && technology != DT_RASPRINTER)
    {
        // Have to get screen DC Caps, because a metafile will return 0.
        HDC dc2 = ::GetDC(NULL);
        nHeight = M_FONTDATA->m_pointSize*GetDeviceCaps(dc2, LOGPIXELSY)/72;
        ::ReleaseDC(NULL, dc2);
    }
    else
    {
        nHeight = M_FONTDATA->m_pointSize*GetDeviceCaps(dc, LOGPIXELSY)/72;
    }
#endif // 0

#if 0
    // Have to get screen DC Caps, because a metafile will return 0.
    HDC dc2 = ::GetDC(NULL);
    ppInch = ::GetDeviceCaps(dc2, LOGPIXELSY);
    ::ReleaseDC(NULL, dc2);
#endif // 0

    // New behaviour: apparently ppInch varies according to Large/Small Fonts
    // setting in Windows. This messes up fonts. So, set ppInch to a constant
    // 96 dpi.
    static const int ppInch = 96;

#if wxFONT_SIZE_COMPATIBILITY
    // Incorrect, but compatible with old wxWindows behaviour
    int nHeight = (M_FONTDATA->m_pointSize*ppInch/72);
#else
    // Correct for Windows compatibility
    int nHeight = - (M_FONTDATA->m_pointSize*ppInch/72);
#endif

    BYTE ff_underline = M_FONTDATA->m_underlined;

    wxFontEncoding encoding = M_FONTDATA->m_encoding;
    if ( encoding == wxFONTENCODING_DEFAULT )
    {
        encoding = wxFont::GetDefaultEncoding();
    }

    DWORD charset;
    switch ( encoding )
    {
        case wxFONTENCODING_ISO8859_1:
        case wxFONTENCODING_ISO8859_15:
        case wxFONTENCODING_CP1250:
            charset = ANSI_CHARSET;
            break;
#if !defined(__WIN16__)
        case wxFONTENCODING_ISO8859_2:
        case wxFONTENCODING_CP1252:
            charset = EASTEUROPE_CHARSET;
            break;

        case wxFONTENCODING_ISO8859_4:
        case wxFONTENCODING_ISO8859_10:
            charset = BALTIC_CHARSET;
            break;

        case wxFONTENCODING_ISO8859_5:
        case wxFONTENCODING_CP1251:
            charset = RUSSIAN_CHARSET;
            break;

        case wxFONTENCODING_ISO8859_6:
            charset = ARABIC_CHARSET;
            break;

        case wxFONTENCODING_ISO8859_7:
            charset = GREEK_CHARSET;
            break;

        case wxFONTENCODING_ISO8859_8:
            charset = HEBREW_CHARSET;
            break;

        case wxFONTENCODING_ISO8859_9:
            charset = TURKISH_CHARSET;
            break;

        case wxFONTENCODING_ISO8859_11:
            charset = THAI_CHARSET;
            break;
#endif // BC++ 16-bit

        case wxFONTENCODING_CP437:
            charset = OEM_CHARSET;
            break;

        default:
            wxFAIL_MSG(wxT("unsupported encoding"));
            // fall through

        case wxFONTENCODING_SYSTEM:
            charset = ANSI_CHARSET;
    }

    HFONT hFont = ::CreateFont
                  (
                   nHeight,             // height
                   0,                   // width (choose best)
                   0,                   // escapement
                   0,                   // orientation
                   ff_weight,           // weight
                   ff_italic,           // italic?
                   ff_underline,        // underlined?
                   0,                   // strikeout?
                   charset,             // charset
                   OUT_DEFAULT_PRECIS,  // precision
                   CLIP_DEFAULT_PRECIS, // clip precision
                   PROOF_QUALITY,       // quality of match
                   DEFAULT_PITCH |      // fixed or variable
                   ff_family,           // family id
                   pzFace               // face name
                  );

    M_FONTDATA->m_hFont = (WXHFONT)hFont;
    if ( !hFont )
    {
        wxLogLastError("CreateFont");
    }

    return hFont != 0;
}

bool wxFont::FreeResource(bool force)
{
    if ( GetResourceHandle() )
    {
        if ( !::DeleteObject((HFONT) M_FONTDATA->m_hFont) )
        {
            wxLogLastError("DeleteObject(font)");
        }

        M_FONTDATA->m_hFont = 0;

        return TRUE;
    }
    return FALSE;
}

WXHANDLE wxFont::GetResourceHandle()
{
    if ( !M_FONTDATA )
        return 0;
    else
        return (WXHANDLE)M_FONTDATA->m_hFont ;
}

bool wxFont::IsFree() const
{
    return (M_FONTDATA && (M_FONTDATA->m_hFont == 0));
}

void wxFont::Unshare()
{
    // Don't change shared data
    if ( !m_refData )
    {
        m_refData = new wxFontRefData();
    }
    else
    {
        wxFontRefData* ref = new wxFontRefData(*M_FONTDATA);
        UnRef();
        m_refData = ref;
    }
}

// ----------------------------------------------------------------------------
// change font attribute: we recreate font when doing it
// ----------------------------------------------------------------------------

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

void wxFont::SetEncoding(wxFontEncoding encoding)
{
    Unshare();

    M_FONTDATA->m_encoding = encoding;

    RealizeResource();
}

// ----------------------------------------------------------------------------
// accessors
// ----------------------------------------------------------------------------

int wxFont::GetPointSize() const
{
    return M_FONTDATA->m_pointSize;
}

int wxFont::GetFamily() const
{
    return M_FONTDATA->m_family;
}

int wxFont::GetFontId() const
{
    return M_FONTDATA->m_fontId;
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
