/////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/gauge.h
// Purpose:     wxGauge class
// Author:      David Elliott
// Modified by:
// Created:     2003/07/15
// RCS-ID:      $Id$
// Copyright:   (c) 2003 David Elliott
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __WX_COCOA_GAUGE_H__
#define __WX_COCOA_GAUGE_H__

// #include "wx/cocoa/NSProgressIndicator.h"

// ========================================================================
// wxGauge
// ========================================================================
class WXDLLEXPORT wxGauge: public wxGaugeBase// , protected wxCocoaNSProgressIndicator
{
    DECLARE_DYNAMIC_CLASS(wxGauge)
    DECLARE_EVENT_TABLE()
//    WX_DECLARE_COCOA_OWNER(NSProgressIndicator,NSView,NSView)
// ------------------------------------------------------------------------
// initialization
// ------------------------------------------------------------------------
public:
    wxGauge() { }
    wxGauge(wxWindow *parent, wxWindowID winid, int range,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxGA_HORIZONTAL,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxGaugeNameStr)
    {
        Create(parent, winid, range, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent, wxWindowID winid, int range,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxSL_HORIZONTAL,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxGaugeNameStr);
    virtual ~wxGauge();

// ------------------------------------------------------------------------
// Cocoa callbacks
// ------------------------------------------------------------------------
protected:
// ------------------------------------------------------------------------
// Implementation
// ------------------------------------------------------------------------
public:
    // Pure Virtuals
    virtual int GetValue() const { return 0; }
    virtual void SetValue(int value) { }

    // retrieve/change the range
    virtual void SetRange(int maxValue) { }
    int GetRange(void) const { return 0; }
};

#endif // __WX_COCOA_GAUGE_H__
