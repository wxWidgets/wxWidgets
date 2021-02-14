/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/gauge.h
// Purpose:     wxGauge class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GAUGE_H_
#define _WX_GAUGE_H_

#include "wx/control.h"

// Group box
class WXDLLIMPEXP_CORE wxGauge: public wxGaugeBase
{
 public:
  wxGauge() { }

  wxGauge(wxWindow *parent, wxWindowID id,
           int range,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxGA_HORIZONTAL,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxASCII_STR(wxGaugeNameStr))
  {
    Create(parent, id, range, pos, size, style, validator, name);
  }

  bool Create(wxWindow *parent, wxWindowID id,
           int range,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxGA_HORIZONTAL,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxASCII_STR(wxGaugeNameStr));

    // set gauge range/value
    virtual void SetRange(int range) wxOVERRIDE;
    virtual void SetValue(int pos) wxOVERRIDE;
    virtual int  GetValue() const  wxOVERRIDE;

    void Pulse() wxOVERRIDE;

 protected:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxGauge);
};

#endif
    // _WX_GAUGE_H_
