/////////////////////////////////////////////////////////////////////////////
// Name:        gauge.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKGAUGEH__
#define __GTKGAUGEH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/list.h"
#include "wx/control.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxGauge;

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern const char* wxGaugeNameStr;

//-----------------------------------------------------------------------------
// wxGaugeBox
//-----------------------------------------------------------------------------

class wxGauge: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxGauge)
  
  public:
    inline wxGauge(void) { m_rangeMax = 0; m_gaugePos = 0; m_useProgressBar = TRUE; }

  inline wxGauge(wxWindow *parent, wxWindowID id,
           int range,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxGA_HORIZONTAL,
           const wxString& name = wxGaugeNameStr)
  {
    Create(parent, id, range, pos, size, style, name);
  };

  bool Create(wxWindow *parent, wxWindowID id,
           int range,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxGA_HORIZONTAL,
           const wxString& name = wxGaugeNameStr );

  void SetShadowWidth( int WXUNUSED(w) ) {};
  void SetBezelFace( int WXUNUSED(w) ) {};
  void SetRange( int r );
  void SetValue( int pos );
  int GetShadowWidth(void) const { return 0; };
  int GetBezelFace(void) const { return 0; };
  int GetRange(void) const;
  int GetValue(void) const;

  // Are we a Win95/GTK progress bar, or a normal gauge?
  inline bool GetProgressBar(void) const { return m_useProgressBar; }

 protected:
 
   int      m_rangeMax;
   int      m_gaugePos;
   bool     m_useProgressBar;
};

#endif // __GTKGAUGEH__
