/////////////////////////////////////////////////////////////////////////////
// Name:        scrollbar.h
// Purpose:     wxScrollBar class
// Author:      David Webster
// Modified by:
// Created:     10/15/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SCROLBAR_H_
#define _WX_SCROLBAR_H_

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const char*) wxScrollBarNameStr;

// Scrollbar item
class WXDLLEXPORT wxScrollBar: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxScrollBar)

public:
  inline wxScrollBar() { m_pageSize = 0; m_viewSize = 0; m_objectSize = 0; }
  ~wxScrollBar();

  inline wxScrollBar(wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxSB_HORIZONTAL,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxScrollBarNameStr)
  {
      Create(parent, id, pos, size, style, validator, name);
  }
  bool Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxSB_HORIZONTAL,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxScrollBarNameStr);

  int GetThumbPosition() const ;
  inline int GetThumbSize() const { return m_pageSize; }
  inline int GetPageSize() const { return m_viewSize; }
  inline int GetRange() const { return m_objectSize; }

  virtual void SetThumbPosition(int viewStart);
  virtual void SetScrollbar(int position, int thumbSize, int range, int pageSize,
    bool refresh = TRUE);

#if WXWIN_COMPATIBILITY
  // Backward compatibility
  int GetValue() const { return GetThumbPosition(); }
  void SetValue(int viewStart) { SetThumbPosition(viewStart); }
  void GetValues(int *viewStart, int *viewLength, int *objectLength,
          int *pageLength) const ;
  int GetViewLength() const { return m_viewSize; }
  int GetObjectLength() const { return m_objectSize; }

  void SetPageSize(int pageLength);
  void SetObjectLength(int objectLength);
  void SetViewLength(int viewLength);
#endif

  void Command(wxCommandEvent& event);

  virtual WXHBRUSH OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
          WXUINT message, WXWPARAM wParam, WXLPARAM lParam);
  virtual bool OS2OnScroll(int orientation, WXWORD wParam,
                           WXWORD pos, WXHWND control);

#if WXWIN_COMPATIBILITY
  // Backward compatibility: generate an old-style scroll command
  void OnScroll(wxScrollEvent& event);
#endif // WXWIN_COMPATIBILITY

protected:
  int m_pageSize;
  int m_viewSize;
  int m_objectSize;

DECLARE_EVENT_TABLE()
};

#endif
    // _WX_SCROLBAR_H_
