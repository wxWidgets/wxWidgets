///////////////////////////////////////////////////////////////////////////////
// Name:        msw/fontenum.cpp
// Purpose:     wxFontEnumerator class for Windows
// Author:      Julian Smart
// Modified by: Vadim Zeitlin to add support for font encodings
// Created:     04/01/98
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

#ifdef __GNUG__
    #pragma implementation "fontenum.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#ifndef WX_PRECOMP
  #include "wx/font.h"
#endif

#include "wx/fontenum.h"

#include "wx/msw/private.h"

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
    bool OnFont(const LPLOGFONT lf, const LPTEXTMETRIC tm) const;

private:
    // the object we forward calls to OnFont() to
    wxFontEnumerator *m_fontEnum;

    // if != -1, enum only fonts which have this encoding
    int m_charset;

    // if TRUE, enum only fixed fonts
    bool m_fixedOnly;
};

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

int CALLBACK wxFontEnumeratorProc(LPLOGFONT lplf, LPTEXTMETRIC lptm,
                                  DWORD dwStyle, LONG lParam);

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
    bool exact;
    m_charset = wxCharsetFromEncoding(encoding, &exact);
#ifdef __WIN32__
    if ( !exact )
    {
        m_charset = DEFAULT_CHARSET;
    }
#endif // Win32

    return exact;
}

void wxFontEnumeratorHelper::DoEnumerate()
{
    HDC hDC = ::GetDC(NULL);

#ifdef __WIN32__
    LOGFONT lf;
    lf.lfCharSet = m_charset;
    lf.lfFaceName[0] = _T('\0');
    lf.lfPitchAndFamily = 0;
    ::EnumFontFamiliesEx(hDC, &lf, (FONTENUMPROC)wxFontEnumeratorProc,
                         (LPARAM)this, 0 /* reserved */) ;
#else // Win16
    ::EnumFonts(hDC, (LPTSTR)NULL, (FONTENUMPROC)wxFontEnumeratorProc,
                (LPARAM) (void*) this) ;
#endif // Win32/16

    ::ReleaseDC(NULL, hDC);
}

bool wxFontEnumeratorHelper::OnFont(const LPLOGFONT lf,
                                    const LPTEXTMETRIC tm) const
{
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

    return m_fontEnum->OnFontFamily(lf->lfFaceName);
}

// ----------------------------------------------------------------------------
// wxFontEnumerator
// ----------------------------------------------------------------------------

bool wxFontEnumerator::EnumerateFamilies(wxFontEncoding encoding,
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

