/////////////////////////////////////////////////////////////////////////////
// Name:        fontenum.h
// Purpose:     wxFontEnumerator class for getting available fonts
// Author:      Julian Smart, Vadim Zeitlin
// Modified by: extended to enumerate more than just font facenames and works
//              not only on Windows now (VZ)
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

#include "wx/font.h"

// ----------------------------------------------------------------------------
// wxFontEnumerator enumerates all available fonts on the system or only the
// fonts with given attributes
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxFontEnumerator
{
public:
    // start enumerating font facenames (either all of them or those which
    // support the given encoding) - will result in OnFacename() being
    // called for each available facename (until they are exhausted or
    // OnFacename returns FALSE)
    virtual bool EnumerateFacenames
                 (
                    wxFontEncoding encoding = wxFONTENCODING_SYSTEM, // all
                    bool fixedWidthOnly = FALSE
                 );

    // enumerate the different encodings either for given font facename or for
    // all facenames - will result in OnFontEncoding() being called for each
    // available (facename, encoding) couple
    virtual bool EnumerateEncodings(const wxString& facename = wxT(""));

    // callbacks which are called after one of EnumerateXXX() functions from
    // above is invoked - all of them may return FALSE to stop enumeration or
    // TRUE to continue with it

    // called by EnumerateFacenames
    virtual bool OnFacename(const wxString& WXUNUSED(facename))
        { return FALSE; }

    // called by EnumerateEncodings
    virtual bool OnFontEncoding(const wxString& WXUNUSED(facename),
                                const wxString& WXUNUSED(encoding))
        { return FALSE; }

    // virtual dtor for the base class
    virtual ~wxFontEnumerator() { }
};

#endif // _WX_FONTENUM_H_
