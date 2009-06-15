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

#include "wx/defs.h"

// ----------------------------------------------------------------------------
// wxButton flags shared with other classes
// ----------------------------------------------------------------------------

#if wxUSE_TOGGLEBTN || wxUSE_BUTTON

// These flags affect label alignment
#define wxBU_LEFT            0x0040
#define wxBU_TOP             0x0080
#define wxBU_RIGHT           0x0100
#define wxBU_BOTTOM          0x0200
#define wxBU_ALIGN_MASK      ( wxBU_LEFT | wxBU_TOP | wxBU_RIGHT | wxBU_BOTTOM )
#endif

#if wxUSE_BUTTON

// ----------------------------------------------------------------------------
// wxButton specific flags
// ----------------------------------------------------------------------------

// These two flags are obsolete
#define wxBU_NOAUTODRAW      0x0000
#define wxBU_AUTODRAW        0x0004

// by default, the buttons will be created with some (system dependent)
// minimal size to make them look nicer, giving this style will make them as
// small as possible
#define wxBU_EXACTFIT        0x0001

#include "wx/bitmap.h"
#include "wx/control.h"

extern WXDLLIMPEXP_DATA_CORE(const char) wxButtonNameStr[];

// ----------------------------------------------------------------------------
// wxButton: a push button
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxButtonBase : public wxControl
{
public:
    wxButtonBase() { }

    // show the image in the button in addition to the label: this method is
    // supported on all (major) platforms
    void SetBitmap(const wxBitmap& bitmap, wxDirection dir = wxLEFT)
    {
        SetBitmapLabel(bitmap);
        SetBitmapPosition(dir);
    }

    wxBitmap GetBitmap() const { return DoGetBitmap(State_Normal); }

    // Methods for setting individual images for different states: normal,
    // selected (meaning pushed or pressed), focused (meaning normal state for
    // a focused button), disabled or hover (a.k.a. hot or current).
    //
    // Remember that SetBitmap() itself must be called before any other
    // SetBitmapXXX() methods (except for SetBitmapLabel() which is a synonym
    // for it anyhow) and that all bitmaps passed to these functions should be
    // of the same size.
    void SetBitmapLabel(const wxBitmap& bitmap)
        { DoSetBitmap(bitmap, State_Normal); }
    void SetBitmapPressed(const wxBitmap& bitmap)
        { DoSetBitmap(bitmap, State_Pressed); }
    void SetBitmapDisabled(const wxBitmap& bitmap)
        { DoSetBitmap(bitmap, State_Disabled); }
    void SetBitmapCurrent(const wxBitmap& bitmap)
        { DoSetBitmap(bitmap, State_Current); }
    void SetBitmapFocus(const wxBitmap& bitmap)
        { DoSetBitmap(bitmap, State_Focused); }

    wxBitmap GetBitmapLabel() const { return DoGetBitmap(State_Normal); }
    wxBitmap GetBitmapPressed() const { return DoGetBitmap(State_Pressed); }
    wxBitmap GetBitmapDisabled() const { return DoGetBitmap(State_Disabled); }
    wxBitmap GetBitmapCurrent() const { return DoGetBitmap(State_Current); }
    wxBitmap GetBitmapFocus() const { return DoGetBitmap(State_Focused); }


    // set the margins around the image
    void SetBitmapMargins(wxCoord x, wxCoord y) { DoSetBitmapMargins(x, y); }
    void SetBitmapMargins(const wxSize& sz) { DoSetBitmapMargins(sz.x, sz.y); }

    // set the image position relative to the text, i.e. wxLEFT means that the
    // image is to the left of the text (this is the default)
    virtual void SetBitmapPosition(wxDirection WXUNUSED(dir)) { }


    // make this button the default button in its top level window
    //
    // returns the old default item (possibly NULL)
    virtual wxWindow *SetDefault();

    // Buttons on MSW can look bad if they are not native colours, because
    // then they become owner-drawn and not theme-drawn.  Disable it here
    // in wxButtonBase to make it consistent.
    virtual bool ShouldInheritColours() const { return false; }

    // returns the default button size for this platform
    static wxSize GetDefaultSize();

    // wxUniv-compatible and deprecated equivalents to SetBitmapXXX()
#if WXWIN_COMPATIBILITY_2_8
    void SetImageLabel(const wxBitmap& bitmap) { SetBitmap(bitmap); }
    void SetImageMargins(wxCoord x, wxCoord y) { SetBitmapMargins(x, y); }
#endif // WXWIN_COMPATIBILITY_2_8

    // backwards compatible names for pressed/current bitmaps: they're not
    // deprecated as there is nothing really wrong with using them and no real
    // advantage to using the new names but the new names are still preferred
    wxBitmap GetBitmapSelected() const { return GetBitmapPressed(); }
    wxBitmap GetBitmapHover() const { return GetBitmapCurrent(); }

    void SetBitmapSelected(const wxBitmap& bitmap) { SetBitmapPressed(bitmap); }
    void SetBitmapHover(const wxBitmap& bitmap) { SetBitmapCurrent(bitmap); }

protected:
    // choose the default border for this window
    virtual wxBorder GetDefaultBorder() const { return wxBORDER_NONE; }

    enum State
    {
        State_Normal,
        State_Pressed,    // a.k.a. "selected" in public API for some reason
        State_Current,    // a.k.a. hot or "hovering"
        State_Disabled,
        State_Focused,
        State_Max
    };

    virtual wxBitmap DoGetBitmap(State WXUNUSED(which)) const
        { return wxBitmap(); }
    virtual void DoSetBitmap(const wxBitmap& WXUNUSED(bitmap),
                             State WXUNUSED(which))
        { }
    virtual void DoSetBitmapMargins(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y))
        { }


    wxDECLARE_NO_COPY_CLASS(wxButtonBase);
};

#if defined(__WXUNIVERSAL__)
    #include "wx/univ/button.h"
#elif defined(__WXMSW__)
    #include "wx/msw/button.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/button.h"
#elif defined(__WXGTK20__)
    #include "wx/gtk/button.h"
#elif defined(__WXGTK__)
    #include "wx/gtk1/button.h"
#elif defined(__WXMAC__)
    #include "wx/osx/button.h"
#elif defined(__WXCOCOA__)
    #include "wx/cocoa/button.h"
#elif defined(__WXPM__)
    #include "wx/os2/button.h"
#elif defined(__WXPALMOS__)
    #include "wx/palmos/button.h"
#endif

#endif // wxUSE_BUTTON

#endif
    // _WX_BUTTON_H_BASE_
