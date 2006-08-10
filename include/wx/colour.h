/////////////////////////////////////////////////////////////////////////////
// Name:        wx/colour.h
// Purpose:     wxColourBase definition
// Author:      Julian Smart
// Modified by: Francesco Montorsi
// Created:
// RCS-ID:      $Id$
// Copyright:   Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COLOUR_H_BASE_
#define _WX_COLOUR_H_BASE_

#include "wx/defs.h"
#include "wx/gdiobj.h"


// the standard wxColour constructors;
// this macro avoids to repeat these lines across all colour.h files, since
// Set() is a virtual function and thus cannot be called by wxColourBase
// constructors
#define DEFINE_STD_WXCOLOUR_CONSTRUCTORS                                    \
    wxColour( unsigned char red, unsigned char green, unsigned char blue )  \
        { Set(red, green, blue); }                                          \
    wxColour( unsigned long colRGB ) { Set(colRGB); }                       \
    wxColour(const wxString &colourName) { Set(colourName); }               \
    wxColour(const wxChar *colourName) { Set(colourName); }


// flags for wxColour -> wxString conversion (see wxColour::GetAsString)
#define wxC2S_NAME              1   // return colour name, when possible
#define wxC2S_CSS_SYNTAX        2   // return colour in rgb(r,g,b) syntax
#define wxC2S_HTML_SYNTAX       4   // return colour in #rrggbb syntax


class WXDLLEXPORT wxColour;


//-----------------------------------------------------------------------------
// wxColourBase: this class has no data members, just some functions to avoid
//               code redundancy in all native wxColour implementations
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxColourBase : public wxGDIObject
{
protected:

    virtual void InitWith(unsigned char red, unsigned char green, unsigned char blue) = 0;
    virtual bool FromString(const wxChar *);

public:
    wxColourBase() {}
    virtual ~wxColourBase() {}


    // Set() functions
    // ---------------

    void Set(unsigned char red, unsigned char green, unsigned char blue)
        { InitWith(red,green,blue); }

    // implemented in colourcmn.cpp
    bool Set(const wxChar *str)
        { return FromString(str); }

    bool Set(const wxString &str)
        { return Set((const wxChar *)str); }

    void Set(unsigned long colRGB)
    {
        // we don't need to know sizeof(long) here because we assume that the three
        // least significant bytes contain the R, G and B values
        Set((unsigned char)colRGB,
            (unsigned char)(colRGB >> 8),
            (unsigned char)(colRGB >> 16));
    }



    // accessors
    // ---------

    virtual bool Ok() const = 0;

    virtual unsigned char Red() const = 0;
    virtual unsigned char Green() const = 0;
    virtual unsigned char Blue() const = 0;

    // implemented in colourcmn.cpp
    virtual wxString GetAsString(long flags = wxC2S_NAME | wxC2S_CSS_SYNTAX) const;



    // old, deprecated
    // ---------------

#if WXWIN_COMPATIBILITY_2_6
    wxDEPRECATED( static wxColour CreateByName(const wxString& name) );
    wxDEPRECATED( void InitFromName(const wxString& col) );
#endif
};



#if defined(__WXPALMOS__)
#include "wx/palmos/colour.h"
#elif defined(__WXMSW__)
#include "wx/msw/colour.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/colour.h"
#elif defined(__WXGTK20__)
#include "wx/gtk/colour.h"
#elif defined(__WXGTK__)
#include "wx/gtk1/colour.h"
#elif defined(__WXMGL__)
#include "wx/generic/colour.h"
#elif defined(__WXX11__)
#include "wx/x11/colour.h"
#elif defined(__WXMAC__)
#include "wx/mac/colour.h"
#elif defined(__WXCOCOA__)
#include "wx/cocoa/colour.h"
#elif defined(__WXPM__)
#include "wx/os2/colour.h"
#endif

#define wxColor wxColour

#endif
    // _WX_COLOUR_H_BASE_
