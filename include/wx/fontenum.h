/////////////////////////////////////////////////////////////////////////////
// Name:        fontenum.h
// Purpose:     wxFontEnumerator class for getting available fonts
// Author:      Julian Smart, Vadim Zeitlin
// Modified by: extended to enumerate more than just font families and work ot
//              only on Windows (VZ)
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FONTENUM_H_
#define _WX_FONTENUM_H_

#ifdef __GNUG__
    #pragma interface "fontenum.h"
#endif

// ----------------------------------------------------------------------------
// wxFontEnumerator enumerates all available fonts on the system or only the
// fonts with given attributes
// ----------------------------------------------------------------------------

class wxFontEnumerator
{
public:
    // start enumerating font families - will result in OnFontFamily() being
    // called for each available font family (unless it returns FALSE)
    virtual bool EnumerateFamilies(bool fixedWidthOnly = FALSE);

    // enumerate the different encodings either for given font family or for
    // all font families - will result in OnFontEncoding() being called for
    // each available (family, encoding) couple
    virtual bool EnumerateEncodings(const wxString& family = wxT(""));

    // callbacks which are called after one of EnumerateXXX() functions from
    // above is invoked - all of them may return FALSE to stop enumeration or
    // TRUE to continue with it

    // called by EnumerateFamilies
    virtual bool OnFontFamily(const wxString& WXUNUSED(family))
        { return FALSE; }

    // called by EnumerateEncodings
    virtual bool OnFontEncoding(const wxString& WXUNUSED(family),
                                const wxString& WXUNUSED(encoding))
        { return FALSE; }
};

#endif // _WX_FONTENUM_H_
