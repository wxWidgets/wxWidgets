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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "fontenum.h"
#endif

#include "wx/fontenc.h"
#include "wx/arrstr.h"

// ----------------------------------------------------------------------------
// wxFontEnumerator enumerates all available fonts on the system or only the
// fonts with given attributes
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxFontEnumerator
{
public:
    wxFontEnumerator() : m_Facenames(NULL), m_Encodings(NULL) { }

    // start enumerating font facenames (either all of them or those which
    // support the given encoding) - will result in OnFacename() being
    // called for each available facename (until they are exhausted or
    // OnFacename returns false)
    virtual bool EnumerateFacenames
                 (
                    wxFontEncoding encoding = wxFONTENCODING_SYSTEM, // all
                    bool fixedWidthOnly = false
                 );

    // enumerate the different encodings either for given font facename or for
    // all facenames - will result in OnFontEncoding() being called for each
    // available (facename, encoding) couple
    virtual bool EnumerateEncodings(const wxString& facename = wxEmptyString);

    // callbacks which are called after one of EnumerateXXX() functions from
    // above is invoked - all of them may return false to stop enumeration or
    // true to continue with it

    // called by EnumerateFacenames
    virtual bool OnFacename(const wxString& facename)
        {
            if (m_Facenames == NULL) m_Facenames = new wxArrayString;
            m_Facenames -> Add(facename);
            return true;
        }

    // called by EnumerateEncodings
    virtual bool OnFontEncoding(const wxString& WXUNUSED(facename),
                                const wxString& encoding)
        {
            if (m_Encodings == NULL) m_Encodings = new wxArrayString;
            m_Encodings -> Add(encoding);
            return true;
        }

    // convenience function that returns array of facenames. Cannot be called
    // before EnumerateFacenames.
    wxArrayString *GetFacenames()
        { return m_Facenames; }

    // convenience function that returns array of encodings.
    // Cannot be called before EnumerateEncodings.
    wxArrayString *GetEncodings()
        { return m_Encodings; }

    // virtual dtor for the base class
    virtual ~wxFontEnumerator()
        {
            if (m_Facenames) delete m_Facenames;
            if (m_Encodings) delete m_Encodings;
        }

private:
    wxArrayString *m_Facenames, *m_Encodings;

    DECLARE_NO_COPY_CLASS(wxFontEnumerator)
};

#endif // _WX_FONTENUM_H_
