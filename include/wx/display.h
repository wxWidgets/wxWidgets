/////////////////////////////////////////////////////////////////////////////
// Name:        wx/display.h
// Purpose:     wxDisplay class
// Author:      Royce Mitchell III
// Modified by: Vadim Zeitlin (resolution changes, display modes, ...)
// Created:     06/21/02
// RCS-ID:      $Id$
// Copyright:   (c) 2002-2003 wxWindows team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DISPLAY_H_BASE_
#define _WX_DISPLAY_H_BASE_

#if wxUSE_DISPLAY

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma interface "displaybase.h"
#endif

#include "wx/dynarray.h"

// ----------------------------------------------------------------------------
// wxVideoMode: contains video mode parameters for a display
// ----------------------------------------------------------------------------

struct WXDLLEXPORT wxVideoMode
{
    wxVideoMode(int width = 0, int height = 0, int depth = 0, int freq = 0)
    {
        w = width;
        h = height;

        bpp = depth;

        refresh = freq;
    }

    // default copy ctor and assignment operator are ok

    bool operator==(const wxVideoMode& m) const
    {
        return w == m.w && h == m.h && bpp == m.bpp && refresh == m.refresh;
    }
    bool operator!=(const wxVideoMode& mode) const
    {
        return !operator==(mode);
    }

    // returns true if this mode matches the other one in the sense that all
    // non zero fields of the other mode have the same value in this one
    // (except for refresh which is allowed to have a greater value)
    bool Matches(const wxVideoMode& other) const
    {
        return (!other.w || w == other.w) &&
                    (!other.h || h == other.h) &&
                        (!other.bpp || bpp == other.bpp) &&
                            (!other.refresh || refresh >= other.refresh);
    }

    // the screen size in pixels (e.g. 640*480), 0 means unspecified
    int w, h;

    // bits per pixel (e.g. 32), 1 is monochrome and 0 means unspecified/known
    int bpp;

    // refresh frequency in Hz, 0 means unspecified/unknown
    int refresh;
};

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

    // display 0 is always the primary display
    bool IsPrimary() const { return m_index == 0; }


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

    DECLARE_NO_COPY_CLASS(wxDisplayBase);
};


#if defined(__WXMSW__)
    #include "wx/msw/display.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/display.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/display.h"
#elif defined(__WXMAC__)
    #include "wx/mac/display.h"
#elif defined(__WXPM__)
    #include "wx/os2/display.h"
#endif

#endif // wxUSE_DISPLAY

#endif // _WX_DISPLAY_H_BASE_
