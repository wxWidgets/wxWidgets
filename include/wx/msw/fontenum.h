/////////////////////////////////////////////////////////////////////////////
// Name:        fontenum.h
// Purpose:     wxFontEnumerator class for Windows
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FONTENUM_H_
#define _WX_FONTENUM_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "fontenum.h"
#endif

/*
 * wxFontEnumerator: for gathering font information
 */

class wxFontEnumerator: public wxObject
{
DECLARE_CLASS(wxFontEnumerator)
public:
    wxFontEnumerator() {};

    // Enumerate the fonts.
    bool Enumerate();

    // Stop enumeration if false is returned.
    // By default, the enumerator stores the facenames in a list for
    // retrieval via GetFacenames().
    virtual bool OnFont(const wxFont& font);

    // Return the list of facenames.
    wxStringList& GetFacenames() { return (wxStringList&) m_faceNames; }
protected:
    wxStringList    m_faceNames;
};

#endif
  // _WX_FONTENUM_H_

