/////////////////////////////////////////////////////////////////////////////
// Name:        wx/palmos/fontenum.h
// Purpose:     wxFontEnumerator class for Palm OS
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id$
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FONTENUM_PALM_H_
#define _WX_FONTENUM_PALM_H_

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

    // Stop enumeration if FALSE is returned.
    // By default, the enumerator stores the facenames in a list for
    // retrieval via GetFacenames().
    virtual bool OnFont(const wxFont& font);

    // Return the list of facenames.
    wxStringList& GetFacenames() { return (wxStringList&) m_faceNames; }
protected:
    wxStringList    m_faceNames;
};

#endif
  // _WX_FONTENUM_PALM_H_
