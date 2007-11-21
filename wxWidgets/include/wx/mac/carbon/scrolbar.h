/////////////////////////////////////////////////////////////////////////////
// Name:        scrollbar.h
// Purpose:     wxScrollBar class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SCROLBAR_H_
#define _WX_SCROLBAR_H_

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const wxChar) wxScrollBarNameStr[];

// Scrollbar item
class WXDLLEXPORT wxScrollBar: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxScrollBar)

public:
  inline wxScrollBar() { m_pageSize = 0; m_viewSize = 0; m_objectSize = 0; }
  virtual ~wxScrollBar();

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
  inline int GetThumbSize() const { return m_viewSize; }
  inline int GetPageSize() const { return m_pageSize; }
  inline int GetRange() const { return m_objectSize; }

  bool IsVertical() const { return (m_windowStyle & wxVERTICAL) != 0; }

  virtual void SetThumbPosition(int viewStart);
  virtual void SetScrollbar(int position, int thumbSize, int range, int pageSize,
    bool refresh = TRUE);

  void Command(wxCommandEvent& event);
    virtual void  MacHandleControlClick( WXWidget control , wxInt16 controlpart , bool mouseStillDown ) ;
    virtual wxInt32 MacControlHit( WXEVENTHANDLERREF handler , WXEVENTREF mevent ) ;

    wxSize DoGetBestSize() const;
    
protected:
    int m_pageSize;
    int m_viewSize;
    int m_objectSize;

DECLARE_EVENT_TABLE()
};

#endif
    // _WX_SCROLBAR_H_
