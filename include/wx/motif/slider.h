/////////////////////////////////////////////////////////////////////////////
// Name:        slider.h
// Purpose:     wxSlider class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
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
  void GetSize(int *x, int *y) const ;
  wxSize GetSize() const { return wxWindow::GetSize(); }

  void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
  void SetSize(const wxRect& rect, int sizeFlags = wxSIZE_AUTO)
    { wxWindow::SetSize(rect, sizeFlags); }
  void SetSize(const wxSize& size) { wxWindow::SetSize(size); }
  virtual void SetSize(int width, int height) { SetSize(-1, -1, width, height, wxSIZE_USE_EXISTING); }

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

// Implementation
  virtual void ChangeFont(bool keepOriginalSize = TRUE);
  virtual void ChangeBackgroundColour();
  virtual void ChangeForegroundColour();

 protected:
  int           m_rangeMin;
  int           m_rangeMax;
  int           m_pageSize;
  int           m_lineSize;
  int           m_tickFreq;
DECLARE_EVENT_TABLE()
};

#endif
    // _WX_SLIDER_H_
