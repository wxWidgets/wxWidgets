/////////////////////////////////////////////////////////////////////////////
// Name:        scrollbar.h
// Purpose:     wxScrollBar class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SCROLBAR_H_
#define _WX_SCROLBAR_H_

#ifdef __GNUG__
#pragma interface "scrolbar.h"
#endif

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

  int GetPosition() const ;
  inline int GetThumbSize() const { return m_pageSize; }
  inline int GetPageSize() const { return m_viewSize; }
  inline int GetRange() const { return m_objectSize; }

  virtual void SetPosition(int viewStart);
  virtual void SetScrollbar(int position, int thumbSize, int range, int pageSize,
    bool refresh = TRUE);

  void Command(wxCommandEvent& event);

// Implementation
  virtual void ChangeFont(bool keepOriginalSize = TRUE);
  virtual void ChangeBackgroundColour();
  virtual void ChangeForegroundColour();

protected:
    int m_pageSize;
    int m_viewSize;
    int m_objectSize;

DECLARE_EVENT_TABLE()
};

#endif
    // _WX_SCROLBAR_H_
