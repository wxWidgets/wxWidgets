/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/fontenumcmn.cpp
// Purpose:     wxFontEnumerator class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     7/5/2006
// RCS-ID:      $Id$
// Copyright:   (c) 1999-2003 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

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

#include "wx/fontenum.h"

// ============================================================================
// implementation
// ============================================================================

// A simple wxFontEnumerator which doesn't perform any filtering and
// just returns all facenames and encodings found in the system
class WXDLLEXPORT wxSimpleFontEnumerator : public wxFontEnumerator
{
public:
    wxSimpleFontEnumerator() { }

    // called by EnumerateFacenames
    virtual bool OnFacename(const wxString& facename)
    {
        m_arrFacenames.Add(facename);
        return true;
    }

    // called by EnumerateEncodings
    virtual bool OnFontEncoding(const wxString& WXUNUSED(facename),
                                const wxString& encoding)
    {
        m_arrEncodings.Add(encoding);
        return true;
    }

public:
    wxArrayString m_arrFacenames, m_arrEncodings;
};


/* static */
wxArrayString wxFontEnumerator::GetFacenames(wxFontEncoding encoding, bool fixedWidthOnly)
{
    wxSimpleFontEnumerator temp;
    temp.EnumerateFacenames(encoding, fixedWidthOnly);
    return temp.m_arrFacenames;
}

/* static */
wxArrayString wxFontEnumerator::GetEncodings(const wxString& facename)
{
    wxSimpleFontEnumerator temp;
    temp.EnumerateEncodings(facename);
    return temp.m_arrEncodings;
}
