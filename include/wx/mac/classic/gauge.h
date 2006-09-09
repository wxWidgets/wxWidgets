/////////////////////////////////////////////////////////////////////////////
// Name:        gauge.h
// Purpose:     wxGauge class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GAUGE_H_
#define _WX_GAUGE_H_

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const wxChar) wxGaugeNameStr[];

// Group box
class WXDLLEXPORT wxGauge: public wxGaugeBase
{
  DECLARE_DYNAMIC_CLASS(wxGauge)
 public:
  inline wxGauge() { m_rangeMax = 0; m_gaugePos = 0; }

  inline wxGauge(wxWindow *parent, wxWindowID id,
           int range,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxGA_HORIZONTAL,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxGaugeNameStr)
  {
    Create(parent, id, range, pos, size, style, validator, name);
  }

  bool Create(wxWindow *parent, wxWindowID id,
           int range,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxGA_HORIZONTAL,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxGaugeNameStr);

  void SetShadowWidth(int w);
  void SetBezelFace(int w);
  void SetRange(int r);
  void SetValue(int pos);

  int GetShadowWidth() const ;
  int GetBezelFace() const ;
  int GetRange() const ;
  int GetValue() const ;

  void Pulse();

  virtual void Command(wxCommandEvent& WXUNUSED(event)) {} ;

 protected:
   int      m_rangeMax;
   int      m_gaugePos;
};

#endif
    // _WX_GAUGE_H_
