///////////////////////////////////////////////////////////////////////////////
// Name:        msw/fontenum.cpp
// Purpose:     wxFontEnumerator class for Windows
// Author:      Julian Smart
// Modified by: David Webster to add support for font encodings
// Created:     01/03/00
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
  #include "wx/font.h"
#endif

#include "wx/fontenum.h"
#include "wx/fontmap.h"

#include "wx/os2/private.h"

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class wxFontEnumeratorHelper
{
public:
    wxFontEnumeratorHelper(wxFontEnumerator *fontEnum);

    // control what exactly are we enumerating
    bool SetEncoding(wxFontEncoding encoding);
    void SetFixedOnly(bool fixedOnly)
        { m_fixedOnly = fixedOnly; }

    // call to start enumeration
    void DoEnumerate();

    // called by our font enumeration proc
    bool OnFont(/*const LPLOGFONT lf, const LPTEXTMETRIC tm*/) const;

private:
    // the object we forward calls to OnFont() to
    wxFontEnumerator *m_fontEnum;

    // if != -1, enum only fonts which have this encoding
    int m_charset;

    // if not empty, enum only the fonts with this facename
    wxString m_facename;

    // if TRUE, enum only fixed fonts
    bool m_fixedOnly;
};

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// TODO:
/*
int CALLBACK wxFontEnumeratorProc(LPLOGFONT lplf, LPTEXTMETRIC lptm,
                                  DWORD dwStyle, LONG lParam);
*/

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxFontEnumeratorHelper
// ----------------------------------------------------------------------------

wxFontEnumeratorHelper::wxFontEnumeratorHelper(wxFontEnumerator *fontEnum)
{
    m_fontEnum = fontEnum;
    m_charset = -1;
    m_fixedOnly = FALSE;
}

bool wxFontEnumeratorHelper::SetEncoding(wxFontEncoding encoding)
{
    wxNativeEncodingInfo info;
    if ( !wxGetNativeFontEncoding(encoding, &info) )
    {
        if ( !wxTheFontMapper->GetAltForEncoding(encoding, &info) )
        {
            // no such encodings at all
            return FALSE;
        }
    }
    m_charset = info.charset;
    m_facename = info.facename;

    return TRUE;
}

#define wxFONTENUMPROC FONTENUMPROC

void wxFontEnumeratorHelper::DoEnumerate()
{
   // TODO:
   /*
    HDC hDC = ::GetDC(NULL);

#ifdef __WIN32__
    LOGFONT lf;
    lf.lfCharSet = m_charset;
    wxStrncpy(lf.lfFaceName, m_facename, WXSIZEOF(lf.lfFaceName));
    lf.lfPitchAndFamily = 0;
    ::EnumFontFamiliesEx(hDC, &lf, (wxFONTENUMPROC)wxFontEnumeratorProc,
                         (LPARAM)this, 0) ;
#else // Win16
    ::EnumFonts(hDC, (LPTSTR)NULL, (FONTENUMPROC)wxFontEnumeratorProc,
    #ifdef STRICT
               (LPARAM)
    #else
               (LPSTR)
    #endif
               this);
#endif // Win32/16

    ::ReleaseDC(NULL, hDC);
    */
}

bool wxFontEnumeratorHelper::OnFont(/*const LPLOGFONT lf,
                                      const LPTEXTMETRIC tm */) const
{
   // TODO:
   /*
    if ( m_fixedOnly )
    {
        // check that it's a fixed pitch font (there is *no* error here, the
        // flag name is misleading!)
        if ( tm->tmPitchAndFamily & TMPF_FIXED_PITCH )
        {
            // not a fixed pitch font
            return TRUE;
        }
    }

    if ( m_charset != -1 )
    {
        // check that we have the right encoding
        if ( lf->lfCharSet != m_charset )
        {
            return TRUE;
        }
    }

    return m_fontEnum->OnFacename(lf->lfFaceName);
   */
    return TRUE;
}

// ----------------------------------------------------------------------------
// wxFontEnumerator
// ----------------------------------------------------------------------------

bool wxFontEnumerator::EnumerateFacenames(wxFontEncoding encoding,
                                          bool fixedWidthOnly)
{
    wxFontEnumeratorHelper fe(this);
    if ( fe.SetEncoding(encoding) )
    {
        fe.SetFixedOnly(fixedWidthOnly);

        fe.DoEnumerate();
    }
    // else: no such fonts, unknown encoding

    return TRUE;
}

bool wxFontEnumerator::EnumerateEncodings(const wxString& family)
{
    wxFAIL_MSG(wxT("TODO"));

    return TRUE;
}

// ----------------------------------------------------------------------------
// Windows callbacks
// ----------------------------------------------------------------------------

// TODO:
/*
int CALLBACK wxFontEnumeratorProc(LPLOGFONT lplf, LPTEXTMETRIC lptm,
                                  DWORD dwStyle, LONG lParam)
{
    // Get rid of any fonts that we don't want...
    if ( dwStyle != TRUETYPE_FONTTYPE )
    {
        // continue enumeration
        return TRUE;
    }

    wxFontEnumeratorHelper *fontEnum = (wxFontEnumeratorHelper *)lParam;

    return fontEnum->OnFont(lplf, lptm);
}
*/

