/////////////////////////////////////////////////////////////////////////////
// Name:        font.cpp
// Purpose:     wxFont class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
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
        Init();
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
        wxLogDebug(_T("Calling wxFont::RealizeResource() twice"));

        return TRUE;
    }

    BYTE ff_italic;
    int ff_weight = 0;
    int ff_family = 0;
    wxString ff_face;

    switch (M_FONTDATA->m_family)
    {
        case wxSCRIPT:     ff_family = FF_SCRIPT ;
                           ff_face = _T("Script") ;
                           break ;
        case wxDECORATIVE: ff_family = FF_DECORATIVE;
                           break;
        case wxROMAN:      ff_family = FF_ROMAN;
                           ff_face = _T("Times New Roman") ;
                           break;
        case wxTELETYPE:
        case wxMODERN:     ff_family = FF_MODERN;
                           ff_face = _T("Courier New") ;
                           break;
        case wxSWISS:      ff_family = FF_SWISS;
                           ff_face = _T("Arial") ;
                           break;
        case wxDEFAULT:
        default:           ff_family = FF_SWISS;
                           ff_face = _T("Arial") ; 
    }

    if (M_FONTDATA->m_style == wxITALIC || M_FONTDATA->m_style == wxSLANT)
        ff_italic = 1;
    else
        ff_italic = 0;

    if (M_FONTDATA->m_weight == wxNORMAL)
        ff_weight = FW_NORMAL;
    else if (M_FONTDATA->m_weight == wxLIGHT)
        ff_weight = FW_LIGHT;
    else if (M_FONTDATA->m_weight == wxBOLD)
        ff_weight = FW_BOLD;

    const wxChar* pzFace = (const wxChar*) ff_face;
    if (!M_FONTDATA->m_faceName.IsNull())
        pzFace = (const wxChar*) M_FONTDATA->m_faceName ;

    /* Always calculate fonts using the screen DC (is this the best strategy?)
     * There may be confusion if a font is selected into a printer
     * DC (say), because the height will be calculated very differently.
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
     */
    // Have to get screen DC Caps, because a metafile will return 0.
    HDC dc2 = ::GetDC(NULL);
    int ppInch = ::GetDeviceCaps(dc2, LOGPIXELSY);
    ::ReleaseDC(NULL, dc2);

    // New behaviour: apparently ppInch varies according to
    // Large/Small Fonts setting in Windows. This messes
    // up fonts. So, set ppInch to a constant 96 dpi.
    ppInch = 96;

#if wxFONT_SIZE_COMPATIBILITY
    // Incorrect, but compatible with old wxWindows behaviour
    int nHeight = (M_FONTDATA->m_pointSize*ppInch/72);
#else
    // Correct for Windows compatibility
    int nHeight = - (M_FONTDATA->m_pointSize*ppInch/72);
#endif

    bool ff_underline = M_FONTDATA->m_underlined;

    M_FONTDATA->m_hFont = (WXHFONT) CreateFont(nHeight, 0, 0, 0,ff_weight,ff_italic,(BYTE)ff_underline,
            0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            PROOF_QUALITY, DEFAULT_PITCH | ff_family, pzFace);
#ifdef WXDEBUG_CREATE
    if (m_hFont==NULL) wxError(_T("Cannot create font"),_T("Internal Error")) ;
#endif
    return (M_FONTDATA->m_hFont != (WXHFONT) NULL);
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
