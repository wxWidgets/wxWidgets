///////////////////////////////////////////////////////////////////////////////
// Name:        palmos/fontenum.cpp
// Purpose:     wxFontEnumerator class for Palm OS
// Author:      William Osborne
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id: 
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "fontenum.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#if wxUSE_FONTMAP

#ifndef WX_PRECOMP
    #include "wx/font.h"
    #include "wx/encinfo.h"
#endif

#include "wx/palmos/private.h"

#include "wx/fontutil.h"
#include "wx/fontenum.h"
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

    // the list of facenames we already found while enumerating facenames
    wxArrayString m_facenames;

    DECLARE_NO_COPY_CLASS(wxFontEnumeratorHelper)
};

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

#ifndef __WXMICROWIN__
int CALLBACK wxFontEnumeratorProc(LPLOGFONT lplf, LPTEXTMETRIC lptm,
                                  DWORD dwStyle, LONG lParam);
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxFontEnumeratorHelper
// ----------------------------------------------------------------------------

wxFontEnumeratorHelper::wxFontEnumeratorHelper(wxFontEnumerator *fontEnum)
{
}

void wxFontEnumeratorHelper::SetFamily(const wxString& family)
{
}

bool wxFontEnumeratorHelper::SetEncoding(wxFontEncoding encoding)
{
    return FALSE;
}

#define wxFONTENUMPROC FONTENUMPROC

void wxFontEnumeratorHelper::DoEnumerate()
{
}

bool wxFontEnumeratorHelper::OnFont(const LPLOGFONT lf,
                                    const LPTEXTMETRIC tm) const
{
    return false;
}

// ----------------------------------------------------------------------------
// wxFontEnumerator
// ----------------------------------------------------------------------------

bool wxFontEnumerator::EnumerateFacenames(wxFontEncoding encoding,
                                          bool fixedWidthOnly)
{
    return false;
}

bool wxFontEnumerator::EnumerateEncodings(const wxString& family)
{
    return false;
}

#endif // wxUSE_FONTMAP
