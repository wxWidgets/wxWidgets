/////////////////////////////////////////////////////////////////////////////
// Name:        slider.h
// Purpose:     wxSlider class
// Author:      David Webster
// Modified by:
// Created:     10/15/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SLIDER_H_
#define _WX_SLIDER_H_

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
#if wxUSE_VALIDATORS
           const wxValidator& validator = wxDefaultValidator,
#endif
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
#if wxUSE_VALIDATORS
           const wxValidator& validator = wxDefaultValidator,
#endif
           const wxString& name = wxSliderNameStr);

  virtual int GetValue() const ;
  virtual void SetValue(int);

  void GetSize(int *x, int *y) const ;
  void GetPosition(int *x, int *y) const ;

  bool Show(bool show);

  void SetRange(int minValue, int maxValue);

  inline int GetMin() const { return m_rangeMin; }
  inline int GetMax() const { return m_rangeMax; }

  void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);



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

  // IMPLEMENTATION
  WXHWND GetStaticMin() const { return m_staticMin; }
  WXHWND GetStaticMax() const { return m_staticMax; }
  WXHWND GetEditValue() const { return m_staticValue; }
  virtual bool ContainsHWND(WXHWND hWnd) const;

  void Command(wxCommandEvent& event);
  virtual WXHBRUSH OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
          WXUINT message, WXWPARAM wParam, WXLPARAM lParam);
  virtual bool OS2OnScroll(int orientation, WXWORD wParam,
                           WXWORD pos, WXHWND control);

 protected:
  WXHWND        m_staticMin;
  WXHWND        m_staticMax;
  WXHWND        m_staticValue;
  int           m_rangeMin;
  int           m_rangeMax;
  int           m_pageSize;
  int           m_lineSize;
  int           m_tickFreq;

  virtual void DoSetSize(int x, int y,
                         int width, int height,
                         int sizeFlags = wxSIZE_AUTO);
};

#endif
    // _WX_SLIDER_H_
