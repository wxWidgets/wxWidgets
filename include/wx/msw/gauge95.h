/////////////////////////////////////////////////////////////////////////////
// Name:        gauge95.h
// Purpose:     wxGauge95 class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _GAUGE95_H_
#define _GAUGE95_H_

#ifdef __GNUG__
#pragma interface "gauge95.h"
#endif

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const char*) wxGaugeNameStr;

// Group box
class WXDLLEXPORT wxGauge95: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxGauge95)
 public:
  inline wxGauge95(void) { m_rangeMax = 0; m_gaugePos = 0; }

  inline wxGauge95(wxWindow *parent, wxWindowID id,
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

  int GetShadowWidth(void) const ;
  int GetBezelFace(void) const ;
  int GetRange(void) const ;
  int GetValue(void) const ;

  void SetForegroundColour(const wxColour& col);
  void SetBackgroundColour(const wxColour& col);

  // Backward compatibility
#if WXWIN_COMPATIBILITY
  inline void SetButtonColour(const wxColour& col) { SetForegroundColour(col); }
#endif

  void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);

  virtual void Command(wxCommandEvent& WXUNUSED(event)) {} ;

 protected:
   int      m_rangeMax;
   int      m_gaugePos;
};

#endif
    // _GAUGEMSW_H_
