///////////////////////////////////////////////////////////////////////////////
// Name:        wx/slider.h
// Purpose:     wxSlider interface
// Author:      Vadim Zeitlin
// Modified by:
// Created:     09.02.01
// RCS-ID:      $Id$
// Copyright:   (c) 1996-2001 wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SLIDER_H_BASE_
#define _WX_SLIDER_H_BASE_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/defs.h"

#if wxUSE_SLIDER

#include "wx/control.h"

extern WXDLLEXPORT_DATA(const wxChar*) wxSliderNameStr;

// ----------------------------------------------------------------------------
// wxSliderBase: define wxSlider interface
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxSliderBase : public wxControl
{
public:
    /* the ctor of the derived class should have the following form:

    wxSlider(wxWindow *parent,
             wxWindowID id,
             int value, int minValue, int maxValue,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = wxSL_HORIZONTAL,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxSliderNameStr);
    */
    wxSliderBase() { }

    // get/set the current slider value (should be in range)
    virtual int GetValue() const = 0;
    virtual void SetValue(int value) = 0;

    // retrieve/change the range
    virtual void SetRange(int minValue, int maxValue) = 0;
    virtual int GetMin() const = 0;
    virtual int GetMax() const = 0;
    void SetMin( int minValue ) { SetRange( minValue , GetMax() ) ; }
    void SetMax( int maxValue ) { SetRange( GetMin() , maxValue ) ; }

    // the line/page size is the increment by which the slider moves when
    // cursor arrow key/page up or down are pressed (clicking the mouse is like
    // pressing PageUp/Down) and are by default set to 1 and 1/10 of the range
    virtual void SetLineSize(int lineSize) = 0;
    virtual void SetPageSize(int pageSize) = 0;
    virtual int GetLineSize() const = 0;
    virtual int GetPageSize() const = 0;

    // these methods get/set the length of the slider pointer in pixels
    virtual void SetThumbLength(int lenPixels) = 0;
    virtual int GetThumbLength() const = 0;

    // warning: most of subsequent methods are currently only implemented in
    //          wxMSW under Win95 and are silently ignored on other platforms

    virtual void SetTickFreq(int WXUNUSED(n), int WXUNUSED(pos)) { }
    virtual int GetTickFreq() const { return 0; }
    virtual void ClearTicks() { }
    virtual void SetTick(int WXUNUSED(tickPos)) { }

    virtual void ClearSel() { }
    virtual int GetSelEnd() const { return GetMin(); }
    virtual int GetSelStart() const { return GetMax(); }
    virtual void SetSelection(int WXUNUSED(min), int WXUNUSED(max)) { }


    virtual void ApplyParentThemeBackground(const wxColour& bg)
        { SetBackgroundColour(bg); }


private:
    DECLARE_NO_COPY_CLASS(wxSliderBase)
};

// ----------------------------------------------------------------------------
// include the real class declaration
// ----------------------------------------------------------------------------

#if defined(__WXUNIVERSAL__)
    #include "wx/univ/slider.h"
#elif defined(__WXMSW__)
    #include "wx/msw/slider95.h"
    #define wxSlider wxSlider95
#elif defined(__WXMOTIF__)
    #include "wx/motif/slider.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/slider.h"
#elif defined(__WXMAC__)
    #include "wx/mac/slider.h"
#elif defined(__WXCOCOA__)
    #include "wx/cocoa/slider.h"
#elif defined(__WXPM__)
    #include "wx/os2/slider.h"
#elif defined(__WXPALMOS__)
    #include "wx/palmos/slider.h"
#endif

#endif // wxUSE_SLIDER

#endif
    // _WX_SLIDER_H_BASE_
