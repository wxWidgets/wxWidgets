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
    virtual void SetImageLabel(const wxBitmap& bitmap) { }

    // set the margins around the image
    virtual void SetImageMargins(wxCoord x, wxCoord y) { }

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
#elif defined(__WXQT__)
    #include "wx/qt/button.h"
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
