/////////////////////////////////////////////////////////////////////////////
// Name:        slider95.h
// Purpose:     wxSlider95 class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _SLIDER95_H_
#define _SLIDER95_H_

#ifdef __GNUG__
#pragma interface "slider95.h"
#endif

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const char*) wxSliderNameStr;

// Slider
class WXDLLEXPORT wxSlider95: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxSlider95)

public:
  wxSlider95(void);

  inline wxSlider95(wxWindow *parent, wxWindowID id,
           int value, int minValue, int maxValue,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxSL_HORIZONTAL,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxSliderNameStr)
  {
      Create(parent, id, value, minValue, maxValue, pos, size, style, validator, name);
  }

  ~wxSlider95(void);

  bool Create(wxWindow *parent, wxWindowID id,
           int value, int minValue, int maxValue,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxSL_HORIZONTAL,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxSliderNameStr);

  virtual int GetValue(void) const ;
  virtual void SetValue(int);
  void GetSize(int *x, int *y) const ;
  void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
  void GetPosition(int *x, int *y) const ;
  bool Show(bool show);

  void SetRange(int minValue, int maxValue);

  inline int GetMin(void) const { return m_rangeMin; }
  inline int GetMax(void) const { return m_rangeMax; }

  // For trackbars only
  void SetTickFreq(int n, int pos);
  inline int GetTickFreq(void) const { return m_tickFreq; }
  void SetPageSize(int pageSize);
  int GetPageSize(void) const ;
  void ClearSel(void) ;
  void ClearTicks(void) ;
  void SetLineSize(int lineSize);
  int GetLineSize(void) const ;
  int GetSelEnd(void) const ;
  int GetSelStart(void) const ;
  void SetSelection(int minPos, int maxPos);
  void SetThumbLength(int len) ;
  int GetThumbLength(void) const ;
  void SetTick(int tickPos) ;

  // IMPLEMENTATION
  inline WXHWND GetStaticMin() const { return m_staticMin; }
  inline WXHWND GetStaticMax() const { return m_staticMax; }
  inline WXHWND GetEditValue() const { return m_staticValue; }
  virtual bool ContainsHWND(WXHWND hWnd) const;

  // Backward compatibility: translate to familiar wxEVT_COMMAND_SLIDER_UPDATED
#if WXWIN_COMPATIBILITY
  void OnScroll(wxScrollEvent& event);
#endif

  void Command(wxCommandEvent& event);
  virtual WXHBRUSH OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
			WXUINT message, WXWPARAM wParam, WXLPARAM lParam);
  void MSWOnVScroll(WXWORD wParam, WXWORD pos, WXHWND control);
  void MSWOnHScroll(WXWORD wParam, WXWORD pos, WXHWND control);

 protected:
  WXHWND        m_staticMin;
  WXHWND        m_staticMax;
  WXHWND        m_staticValue;
  int           m_rangeMin;
  int           m_rangeMax;
  int           m_pageSize;
  int           m_lineSize;
  int           m_tickFreq;
DECLARE_EVENT_TABLE()
};

#endif
    // _SLIDER95_H_
