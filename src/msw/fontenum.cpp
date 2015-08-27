///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/fontenum.cpp
// Purpose:     wxFontEnumerator class for Windows
// Author:      Julian Smart
// Modified by: Vadim Zeitlin to add support for font encodings
// Created:     04/01/98
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

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#if wxUSE_FONTENUM

#include "wx/fontenum.h"

#ifndef WX_PRECOMP
    #include "wx/gdicmn.h"
    #include "wx/font.h"
    #include "wx/dynarray.h"
    #include "wx/msw/private.h"
#endif

#include "wx/encinfo.h"
#include "wx/fontutil.h"
#include "wx/fontmap.h"

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
        // we enumerate fonts with the given encoding
    bool SetEncoding(wxFontEncoding encoding);
        // we enumerate fixed-width fonts
    void SetFixedOnly(bool fixedOnly) { m_fixedOnly = fixedOnly; }
        // we enumerate the encodings this font face is available in
    void SetFaceName(const wxString& facename);

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

    // if true, enum only fixed fonts
    bool m_fixedOnly;

    // if true, we enumerate the encodings, not fonts
    bool m_enumEncodings;

    // the list of charsets we already found while enumerating charsets
    wxArrayInt m_charsets;

    // the list of facenames we already found while enumerating facenames
    wxArrayString m_facenames;

    wxDECLARE_NO_COPY_CLASS(wxFontEnumeratorHelper);
};

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

int CALLBACK wxFontEnumeratorProc(LPLOGFONT lplf, LPTEXTMETRIC lptm,
                                  DWORD dwStyle, LPARAM lParam);

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
    m_fixedOnly = false;
    m_enumEncodings = false;
}

void wxFontEnumeratorHelper::SetFaceName(const wxString& facename)
{
    m_enumEncodings = true;
    m_facename = facename;
}

bool wxFontEnumeratorHelper::SetEncoding(wxFontEncoding encoding)
{
    if ( encoding != wxFONTENCODING_SYSTEM )
    {
        wxNativeEncodingInfo info;
        if ( !wxGetNativeFontEncoding(encoding, &info) )
        {
#if wxUSE_FONTMAP
            if ( !wxFontMapper::Get()->GetAltForEncoding(encoding, &info) )
#endif // wxUSE_FONTMAP
            {
                // no such encodings at all
                return false;
            }
        }

        m_charset = info.charset;
        m_facename = info.facename;
    }

    return true;
}

void wxFontEnumeratorHelper::DoEnumerate()
{
    HDC hDC = ::GetDC(NULL);

    LOGFONT lf;
    lf.lfCharSet = (BYTE)m_charset;
    wxStrlcpy(lf.lfFaceName, m_facename.c_str(), WXSIZEOF(lf.lfFaceName));
    lf.lfPitchAndFamily = 0;
    ::EnumFontFamiliesEx(hDC, &lf, (FONTENUMPROC)wxFontEnumeratorProc,
                         (LPARAM)this, 0 /* reserved */) ;

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

#if wxUSE_FONTMAP
            wxFontEncoding enc = wxGetFontEncFromCharSet(cs);
            return m_fontEnum->OnFontEncoding(lf->lfFaceName,
                                              wxFontMapper::GetEncodingName(enc));
#else // !wxUSE_FONTMAP
            // Just use some unique and, hopefully, understandable, name.
            return m_fontEnum->OnFontEncoding
                               (
                                lf->lfFaceName,
                                wxString::Format(wxS("Code page %d"), cs)
                               );
#endif // wxUSE_FONTMAP/!wxUSE_FONTMAP
        }
        else
        {
            // continue enumeration
            return true;
        }
    }

    if ( m_fixedOnly )
    {
        // check that it's a fixed pitch font (there is *no* error here, the
        // flag name is misleading!)
        if ( tm->tmPitchAndFamily & TMPF_FIXED_PITCH )
        {
            // not a fixed pitch font
            return true;
        }
    }

    if ( m_charset != DEFAULT_CHARSET )
    {
        // check that we have the right encoding
        if ( lf->lfCharSet != m_charset )
        {
            return true;
        }
    }
    else // enumerating fonts in all charsets
    {
        // we can get the same facename twice or more in this case because it
        // may exist in several charsets but we only want to return one copy of
        // it (note that this can't happen for m_charset != DEFAULT_CHARSET)
        if ( m_facenames.Index(lf->lfFaceName) != wxNOT_FOUND )
        {
            // continue enumeration
            return true;
        }

        wxConstCast(this, wxFontEnumeratorHelper)->
            m_facenames.Add(lf->lfFaceName);
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

    return true;
}

bool wxFontEnumerator::EnumerateEncodings(const wxString& facename)
{
    wxFontEnumeratorHelper fe(this);
    fe.SetFaceName(facename);
    fe.DoEnumerate();

    return true;
}

// ----------------------------------------------------------------------------
// Windows callbacks
// ----------------------------------------------------------------------------

int CALLBACK wxFontEnumeratorProc(LPLOGFONT lplf, LPTEXTMETRIC lptm,
                                  DWORD WXUNUSED(dwStyle), LPARAM lParam)
{

    // we used to process TrueType fonts only, but there doesn't seem to be any
    // reasons to restrict ourselves to them here
#if 0
    // Get rid of any fonts that we don't want...
    if ( dwStyle != TRUETYPE_FONTTYPE )
    {
        // continue enumeration
        return TRUE;
    }
#endif // 0

    wxFontEnumeratorHelper *fontEnum = (wxFontEnumeratorHelper *)lParam;

    return fontEnum->OnFont(lplf, lptm);
}

#endif // wxUSE_FONTENUM
