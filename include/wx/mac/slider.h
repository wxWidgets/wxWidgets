/////////////////////////////////////////////////////////////////////////////
// Name:        slider.h
// Purpose:     wxSlider class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SLIDER_H_
#define _WX_SLIDER_H_

#ifdef __GNUG__
#pragma interface "slider.h"
#endif

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const char*) wxSliderNameStr;

// Slider
class WXDLLEXPORT wxSlider: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxSlider)

public:
  wxSlider();

  inline wxSlider(wxWindow *parent, wxWindowID id,
           int value, int minValue, int maxValue,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxSL_HORIZONTAL,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxSliderNameStr)
  {
      Create(parent, id, value, minValue, maxValue, pos, size, style, validator, name);
  }

  ~wxSlider();

  bool Create(wxWindow *parent, wxWindowID id,
           int value, int minValue, int maxValue,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxSL_HORIZONTAL,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxSliderNameStr);

  virtual int GetValue() const ;
  virtual void SetValue(int);
  bool Show(bool show);

  void SetRange(int minValue, int maxValue);

  inline int GetMin() const { return m_rangeMin; }
  inline int GetMax() const { return m_rangeMax; }

  // For trackbars only
  void SetTickFreq(int n, int pos);
  inline int GetTickFreq() const { return m_tickFreq; }
  void SetPageSize(int pageSize);
  int GetPageSize() const ;
  void ClearSel() ;
  void ClearTicks() ;
  void SetLineSize(int lineSize);
  int GetLineSize() const ;
  int GetSelEnd() const ;
  int GetSelStart() const ;
  void SetSelection(int minPos, int maxPos);
  void SetThumbLength(int len) ;
  int GetThumbLength() const ;
  void SetTick(int tickPos) ;

  void Command(wxCommandEvent& event);
	void 					MacHandleControlClick( ControlHandle control , SInt16 controlpart ) ;
 protected:
 	wxStaticText*	m_macMinimumStatic ;
 	wxStaticText*	m_macMaximumStatic ;
 	wxStaticText*	m_macValueStatic ;

  int           m_rangeMin;
  int           m_rangeMax;
  int           m_pageSize;
  int           m_lineSize;
  int           m_tickFreq;
DECLARE_EVENT_TABLE()
};

#endif
    // _WX_SLIDER_H_
