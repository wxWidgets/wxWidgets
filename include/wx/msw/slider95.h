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

  inline wxSlider95(wxWindow *parent, const wxWindowID id,
           const int value, const int minValue, const int maxValue,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           const long style = wxSL_HORIZONTAL,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxSliderNameStr)
  {
      Create(parent, id, value, minValue, maxValue, pos, size, style, validator, name);
  }

  ~wxSlider95(void);

  bool Create(wxWindow *parent, const wxWindowID id,
           const int value, const int minValue, const int maxValue,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           const long style = wxSL_HORIZONTAL,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxSliderNameStr);

  virtual int GetValue(void) const ;
  virtual void SetValue(const int);
  void GetSize(int *x, int *y) const ;
  void SetSize(const int x, const int y, const int width, const int height, const int sizeFlags = wxSIZE_AUTO);
  void GetPosition(int *x, int *y) const ;
  bool Show(const bool show);

  void SetRange(const int minValue, const int maxValue);

  inline int GetMin(void) const { return m_rangeMin; }
  inline int GetMax(void) const { return m_rangeMax; }

  // For trackbars only
  void SetTickFreq(const int n, const int pos);
  inline int GetTickFreq(void) const { return m_tickFreq; }
  void SetPageSize(const int pageSize);
  int GetPageSize(void) const ;
  void ClearSel(void) ;
  void ClearTicks(void) ;
  void SetLineSize(const int lineSize);
  int GetLineSize(void) const ;
  int GetSelEnd(void) const ;
  int GetSelStart(void) const ;
  void SetSelection(const int minPos, const int maxPos);
  void SetThumbLength(const int len) ;
  int GetThumbLength(void) const ;
  void SetTick(const int tickPos) ;

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
  virtual WXHBRUSH OnCtlColor(const WXHDC pDC, const WXHWND pWnd, const WXUINT nCtlColor,
			WXUINT message, WXWPARAM wParam, WXLPARAM lParam);
  void MSWOnVScroll(const WXWORD wParam, const WXWORD pos, const WXHWND control);
  void MSWOnHScroll(const WXWORD wParam, const WXWORD pos, const WXHWND control);

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
