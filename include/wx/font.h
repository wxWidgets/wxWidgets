/////////////////////////////////////////////////////////////////////////////
// Name:        wx/font.h
// Purpose:     wxFontBase class: the interface of wxFont
// Author:      Vadim Zeitlin
// Modified by:
// Created:     20.09.99
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FONT_H_BASE_
#define _WX_FONT_H_BASE_

#ifdef __GNUG__
    #pragma interface "fontbase.h"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/defs.h"        // for wxDEFAULT &c
#include "wx/fontenc.h"     // the font encoding constants
#include "wx/gdiobj.h"      // the base class

// ----------------------------------------------------------------------------
// forward declarations
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxFontData;
class WXDLLEXPORT wxFontBase;
class WXDLLEXPORT wxFont;

// ----------------------------------------------------------------------------
// font constants
// ----------------------------------------------------------------------------

// standard font families
enum wxFontFamily
{
    wxFONTFAMILY_DEFAULT = wxDEFAULT,
    wxFONTFAMILY_DECORATIVE = wxDECORATIVE,
    wxFONTFAMILY_ROMAN = wxROMAN,
    wxFONTFAMILY_SCRIPT = wxSCRIPT,
    wxFONTFAMILY_SWISS = wxSWISS,
    wxFONTFAMILY_MODERN = wxMODERN,
    wxFONTFAMILY_TELETYPE = wxTELETYPE,
    wxFONTFAMILY_MAX
};

// font styles
enum wxFontStyle
{
    wxFONTSTYLE_NORMAL = wxNORMAL,
    wxFONTSTYLE_ITALIC = wxITALIC,
    wxFONTSTYLE_SLANT = wxSLANT,
    wxFONTSTYLE_MAX
};

// font weights
enum wxFontWeight
{
    wxFONTWEIGHT_NORMAL = wxNORMAL,
    wxFONTWEIGHT_LIGHT = wxLIGHT,
    wxFONTWEIGHT_BOLD = wxBOLD,
    wxFONTWEIGHT_MAX
};

// ----------------------------------------------------------------------------
// wxFontBase represents a font object
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxFontRefData;

class WXDLLEXPORT wxFontBase : public wxGDIObject
{
public:
    // creator function
    static wxFont *New(
        int pointSize,              // size of the font in points
        int family,                 // see wxFontFamily enum
        int style,                  // see wxFontStyle enum
        int weight,                 // see wxFontWeight enum
        bool underlined = FALSE,    // not underlined by default
        const wxString& face = wxEmptyString,              // facename
        wxFontEncoding encoding = wxFONTENCODING_DEFAULT); // ISO8859-X, ...

    // was the font successfully created?
    bool Ok() const { return m_refData != NULL; }

    // comparison
    bool operator == (const wxFont& font) const;
    bool operator != (const wxFont& font) const;

    // accessors: get the font characteristics
    virtual int GetPointSize() const = 0;
    virtual int GetFamily() const = 0;
    virtual int GetStyle() const = 0;
    virtual int GetWeight() const = 0;
    virtual bool GetUnderlined() const = 0;
    virtual wxString GetFaceName() const = 0;
    virtual wxFontEncoding GetEncoding() const = 0;

    // change the font characteristics
    virtual void SetPointSize( int pointSize ) = 0;
    virtual void SetFamily( int family ) = 0;
    virtual void SetStyle( int style ) = 0;
    virtual void SetWeight( int weight ) = 0;
    virtual void SetFaceName( const wxString& faceName ) = 0;
    virtual void SetUnderlined( bool underlined ) = 0;
    virtual void SetEncoding(wxFontEncoding encoding) = 0;

    // translate the fonts into human-readable string (i.e. GetStyleString()
    // will return "wxITALIC" for an italic font, ...)
    wxString GetFamilyString() const;
    wxString GetStyleString() const;
    wxString GetWeightString() const;

    // the default encoding is used for creating all fonts with default
    // encoding parameter
    static wxFontEncoding GetDefaultEncoding()
        { return ms_encodingDefault; }
    static void SetDefaultEncoding(wxFontEncoding encoding)
        { ms_encodingDefault = encoding; }

protected:
    // get the internal data
    wxFontRefData *GetFontData() const
        { return (wxFontRefData *)m_refData; }

private:
    // the currently default encoding: by default, it's the default system
    // encoding, but may be changed by the application using
    // SetDefaultEncoding() to make all subsequent fonts created without
    // specifing encoding parameter using this encoding
    static wxFontEncoding ms_encodingDefault;
};

// include the real class declaration
#if defined(__WXMSW__)
    #include "wx/msw/font.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/font.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/font.h"
#elif defined(__WXQT__)
    #include "wx/qt/font.h"
#elif defined(__WXMAC__)
    #include "wx/mac/font.h"
#elif defined(__WXPM__)
    #include "wx/os2/font.h"
#elif defined(__WXSTUBS__)
    #include "wx/stubs/font.h"
#endif

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

#define M_FONTDATA GetFontData()

#endif
    // _WX_FONT_H_BASE_
