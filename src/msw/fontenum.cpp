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
#include "wx/fontmap.h"

#include "wx/msw/private.h"

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// the helper class which calls ::EnumFontFamilies() and whose OnFont() is
// called from the callback passed to this function and, in its turn, calls the
// appropariate wxFontEnumerator method
class wxFontEnumeratorHelper
{
public:
    wxFontEnumeratorHelper(wxFontEnumerator *fontEnum);

    // control what exactly are we enumerating
        // we enumerate fonts with given enocding
    bool SetEncoding(wxFontEncoding encoding);
        // we enumerate fixed-width fonts
    void SetFixedOnly(bool fixedOnly) { m_fixedOnly = fixedOnly; }
        // we enumerate the encodings available in this family
    void SetFamily(const wxString& family);

    // call to start enumeration
    void DoEnumerate();

    // called by our font enumeration proc
    bool OnFont(const LPLOGFONT lf, const LPTEXTMETRIC tm) const;

private:
    // the object we forward calls to OnFont() to
    wxFontEnumerator *m_fontEnum;

    // if != -1, enum only fonts which have this encoding
    int m_charset;

    // if not empty, enum only the fonts with this facename
    wxString m_facename;

    // if not empty, enum only the fonts in this family
    wxString m_family;

    // if TRUE, enum only fixed fonts
    bool m_fixedOnly;

    // if TRUE, we enumerate the encodings, not fonts
    bool m_enumEncodings;

    // the list of charsets we already found while enumerating charsets
    wxArrayInt m_charsets;
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
    m_charset = DEFAULT_CHARSET;
    m_fixedOnly = FALSE;
    m_enumEncodings = FALSE;
}

void wxFontEnumeratorHelper::SetFamily(const wxString& family)
{
    m_enumEncodings = TRUE;
    m_family = family;
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

#if defined(__GNUWIN32__)
    #if wxUSE_NORLANDER_HEADERS
        #define wxFONTENUMPROC int(*)(const LOGFONTA *, const TEXTMETRICA *, long unsigned int, LPARAM)
    #else
        #define wxFONTENUMPROC int(*)(ENUMLOGFONTEX *, NEWTEXTMETRICEX*, int, LPARAM)
    #endif
#else
    #define wxFONTENUMPROC FONTENUMPROC
#endif

void wxFontEnumeratorHelper::DoEnumerate()
{
    HDC hDC = ::GetDC(NULL);

#ifdef __WIN32__
    LOGFONT lf;
    lf.lfCharSet = m_charset;
    wxStrncpy(lf.lfFaceName, m_facename, WXSIZEOF(lf.lfFaceName));
    lf.lfPitchAndFamily = 0;
    ::EnumFontFamiliesEx(hDC, &lf, (wxFONTENUMPROC)wxFontEnumeratorProc,
                         (LPARAM)this, 0 /* reserved */) ;
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
}

bool wxFontEnumeratorHelper::OnFont(const LPLOGFONT lf,
                                    const LPTEXTMETRIC tm) const
{
    if ( m_enumEncodings )
    {
        // is this a new charset?
        int cs = lf->lfCharSet;
        if ( m_charsets.Index(cs) == wxNOT_FOUND )
        {
            wxConstCast(this, wxFontEnumeratorHelper)->m_charsets.Add(cs);

            wxFontEncoding enc = wxGetFontEncFromCharSet(cs);
            return m_fontEnum->OnFontEncoding(lf->lfFaceName,
                                              wxFontMapper::GetEncodingName(enc));
        }
        else
        {
            // continue enumeration
            return TRUE;
        }
    }

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
    wxFontEnumeratorHelper fe(this);
    fe.SetFamily(family);
    fe.DoEnumerate();

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

