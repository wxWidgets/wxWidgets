/////////////////////////////////////////////////////////////////////////////
// Name:        wx/button.h
// Purpose:     wxButtonBase class
// Author:      Vadim Zetlin
// Modified by:
// Created:     15.08.00
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zetlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BUTTON_H_BASE_
#define _WX_BUTTON_H_BASE_

#if wxUSE_BUTTON

// ----------------------------------------------------------------------------
// wxButton flags
// ----------------------------------------------------------------------------

// These two flags are obsolete
#define wxBU_NOAUTODRAW      0x0000
#define wxBU_AUTODRAW        0x0004

// These flags affect label alignment
#define wxBU_LEFT            0x0040
#define wxBU_TOP             0x0080
#define wxBU_RIGHT           0x0100
#define wxBU_BOTTOM          0x0200

// by default, the buttons will be created with some (system dependent)
// minimal size to make them look nicer, giving this style will make them as
// small as possible
#define wxBU_EXACTFIT        0x0001

#include "wx/control.h"

class WXDLLEXPORT wxBitmap;

WXDLLEXPORT_DATA(extern const wxChar*) wxButtonNameStr;

// ----------------------------------------------------------------------------
// wxButton: a push button
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxButtonBase : public wxControl
{
public:
    // show the image in the button in addition to the label
    virtual void SetImageLabel(const wxBitmap& WXUNUSED(bitmap)) { }

    // set the margins around the image
    virtual void SetImageMargins(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y)) { }

    // this wxButton method is called when the button becomes the default one
    // on its panel
    virtual void SetDefault() { }

    // returns the default button size for this platform
    static wxSize GetDefaultSize();
};

#if defined(__WXUNIVERSAL__)
    #include "wx/univ/button.h"
#elif defined(__WXMSW__)
    #include "wx/msw/button.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/button.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/button.h"
#elif defined(__WXMAC__)
    #include "wx/mac/button.h"
#elif defined(__WXPM__)
    #include "wx/os2/button.h"
#elif defined(__WXSTUBS__)
    #include "wx/stubs/button.h"
#endif

#endif // wxUSE_BUTTON

#endif
    // _WX_BUTTON_H_BASE_
