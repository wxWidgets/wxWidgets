/////////////////////////////////////////////////////////////////////////////
// Name:        gaugemsw.h
// Purpose:     wxGauge class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _GAUGEMSW_H_
#define _GAUGEMSW_H_

#ifdef __GNUG__
#pragma interface "gaugemsw.h"
#endif

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const char*) wxGaugeNameStr;

// Group box
class WXDLLEXPORT wxGaugeMSW: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxGaugeMSW)
 public:
  inline wxGaugeMSW(void) { m_rangeMax = 0; m_gaugePos = 0; }

  inline wxGaugeMSW(wxWindow *parent, const wxWindowID id,
           const int range,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           const long style = wxGA_HORIZONTAL,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxGaugeNameStr)
  {
    Create(parent, id, range, pos, size, style, validator, name);
  }

  bool Create(wxWindow *parent, const wxWindowID id,
           const int range,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           const long style = wxGA_HORIZONTAL,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxGaugeNameStr);

  void SetShadowWidth(const int w);
  void SetBezelFace(const int w);
  void SetRange(const int r);
  void SetValue(const int pos);

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

  void SetSize(const int x, const int y, const int width, const int height, const int sizeFlags = wxSIZE_AUTO);

  virtual void Command(wxCommandEvent& WXUNUSED(event)) {} ;

 protected:
   int      m_rangeMax;
   int      m_gaugePos;
};

#endif
    // _GAUGEMSW_H_
