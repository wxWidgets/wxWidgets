/////////////////////////////////////////////////////////////////////////////
// Name:        wx/display.h
// Purpose:     wxDisplay class
// Author:      Royce Mitchell III
// Modified by: Vadim Zeitlin (resolution changes, display modes, ...)
// Created:     06/21/02
// RCS-ID:      $Id$
// Copyright:   (c) 2002-2003 wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DISPLAY_H_BASE_
#define _WX_DISPLAY_H_BASE_

#if wxUSE_DISPLAY

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "displaybase.h"
#endif

#include "wx/dynarray.h"
#include "wx/vidmode.h"

class WXDLLEXPORT wxWindow;
class WXDLLEXPORT wxPoint;
class WXDLLEXPORT wxRect;
class WXDLLEXPORT wxString;

WX_DECLARE_EXPORTED_OBJARRAY(wxVideoMode, wxArrayVideoModes);

// default, uninitialized, video mode object
WXDLLEXPORT_DATA(extern const wxVideoMode) wxDefaultVideoMode;

// ----------------------------------------------------------------------------
// wxDisplayBase: represents a display/monitor attached to the system
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxDisplayBase
{
public:
    // initialize the object containing all information about the given
    // display
    //
    // the displays are numbered from 0 to GetCount() - 1, 0 is always the
    // primary display and the only one which is always supported
    wxDisplayBase(size_t index = 0);

    // return the number of available displays, valid parameters to
    // wxDisplay ctor are from 0 up to this number
    static size_t GetCount();

    // find the display where the given point lies, return wxNOT_FOUND if
    // it doesn't belong to any display
    static int GetFromPoint(const wxPoint& pt);

    // find the display where the given window lies, return wxNOT_FOUND if it
    // is not shown at all
    static int GetFromWindow(wxWindow *window);


    // return true if the object was initialized successfully
    virtual bool IsOk() const { return true; }

    // get the display size
    virtual wxRect GetGeometry() const = 0;

    // name may be empty
    virtual wxString GetName() const = 0;

    // display 0 is usually the primary display
    virtual bool IsPrimary() const { return m_index == 0; }


    // enumerate all video modes supported by this display matching the given
    // one (in the sense of wxVideoMode::Match())
    //
    // as any mode matches the default value of the argument and there is
    // always at least one video mode supported by display, the returned array
    // is only empty for the default value of the argument if this function is
    // not supported at all on this platform
    virtual wxArrayVideoModes
        GetModes(const wxVideoMode& mode = wxDefaultVideoMode) const = 0;

    // get current video mode
    virtual wxVideoMode GetCurrentMode() const = 0;

    // change current mode, return true if succeeded, false otherwise
    //
    // for the default value of the argument restores the video mode to default
    virtual bool ChangeMode(const wxVideoMode& mode = wxDefaultVideoMode) = 0;

    // restore the default video mode (just a more readable synonym)
    void ResetMode() { (void)ChangeMode(); }

    // virtual dtor as for any base class
    virtual ~wxDisplayBase() { }

protected:
    // the index of this display (0 is always the primary one)
    size_t m_index;

    DECLARE_NO_COPY_CLASS(wxDisplayBase)
};


#if defined(__WXMSW__)
    #include "wx/msw/display.h"
#elif defined(__WXMOTIF__)
    #include "wx/unix/displayx11.h"
#elif defined(__WXGTK__)
    #include "wx/unix/displayx11.h"
#elif defined(__WXX11__)
    #include "wx/unix/displayx11.h"
#elif defined(__WXCOCOA__)
    #include "wx/cocoa/display.h"
#elif defined(__WXMAC__)
    #include "wx/mac/display.h"
#elif defined(__WXPM__)
    #include "wx/os2/display.h"
#endif

#endif // wxUSE_DISPLAY

#endif // _WX_DISPLAY_H_BASE_
