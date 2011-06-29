/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/native/gauge.h
// Purpose:     wxMobile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_NATIVE_GAUGE_H_
#define _WX_MOBILE_NATIVE_GAUGE_H_

#include "wx/gauge.h"

extern WXDLLEXPORT_DATA(const char) wxGaugeNameStr[];

/**
 @class wxMoGauge
 
 A gauge class, representing a range and current value.
 
 Currently only determinate mode is supported.
 
 @category{wxMobile}
 */

class WXDLLEXPORT wxMoGauge : public wxGauge
{
public:
    /// Default constructor.
    wxMoGauge() { Init(); }
    
    /// Constructor.
    wxMoGauge(wxWindow *parent,
              wxWindowID id,
              int range,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxGA_HORIZONTAL,
              const wxValidator& validator = wxDefaultValidator,
              const wxString& name = wxGaugeNameStr)
    {
        Init();
        Create(parent, id, range, pos, size, style, validator, name);
    }
    
    /// Creation function.
    bool Create(wxWindow *parent,
                wxWindowID id,
                int range,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxGA_HORIZONTAL,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxGaugeNameStr);
    
#if 0
    /// Set the gauge maximum value (the minimum value is zero).
    virtual void SetRange(int range);
    
    /// Set the gauge value, from zero to the current range.
    virtual void SetValue(int pos);
    
    virtual bool SetForegroundColour(const wxColour& col);
    virtual bool SetBackgroundColour(const wxColour& col);
    
    /// Sets indeterminate mode (not currently implemented).
    void SetIndeterminateMode();
    
    /// Sets determinate mode (not currently implemented).
    void SetDeterminateMode();
    
    // Switches the gauge to indeterminate mode (if required) and makes
    // the gauge move a bit to indicate the user that some progress has been made.
    // Not currently implemented.
    void Pulse();
#endif  // 0
    
protected:
    
    void Init();

    //virtual wxSize DoGetBestSize() const;
    
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMoGauge)
    DECLARE_EVENT_TABLE()
};

#endif
    // _WX_MOBILE_NATIVE_GAUGE_H_
