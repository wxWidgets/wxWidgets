/////////////////////////////////////////////////////////////////////////////
// Name:        tbarsmpl.h
// Purpose:     wxToolBarSimple class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TBARSMPLH__
#define _WX_TBARSMPLH__

#ifdef __GNUG__
#pragma interface "tbarsmpl.h"
#endif

#include "wx/defs.h"

#if wxUSE_TOOLBAR

#include "wx/bitmap.h"
#include "wx/list.h"
#include "wx/tbarbase.h"

class WXDLLEXPORT wxMemoryDC;

WXDLLEXPORT_DATA(extern const wxChar*) wxToolBarNameStr;
WXDLLEXPORT_DATA(extern const wxSize) wxDefaultSize;
WXDLLEXPORT_DATA(extern const wxPoint) wxDefaultPosition;

// XView can't cope properly with panels that behave like canvases
// (e.g. no scrollbars in panels)
class WXDLLEXPORT wxToolBarSimple : public wxToolBarBase
{
  DECLARE_DYNAMIC_CLASS(wxToolBarSimple)

 public:

  wxToolBarSimple(void);
  inline wxToolBarSimple(wxWindow *parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
            long style = wxNO_BORDER|wxTB_HORIZONTAL,
            const wxString& name = wxToolBarNameStr)
  {
    Create(parent, id, pos, size, style, name);
  }
  ~wxToolBarSimple(void);

  bool Create(wxWindow *parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
            long style = wxNO_BORDER|wxTB_HORIZONTAL, const wxString& name = wxToolBarNameStr);

  void OnPaint(wxPaintEvent& event);
  void OnSize(wxSizeEvent& event);
  void OnMouseEvent(wxMouseEvent& event);
  void OnKillFocus(wxFocusEvent& event);

  // Handle wxToolBar events

  virtual void DrawTool(wxDC& dc, wxMemoryDC& memDC, wxToolBarTool *tool);
  virtual void ToggleTool(int toolIndex, bool toggle); // toggle is TRUE if toggled on

  virtual void SpringUpButton(int index);

  void Layout(void);

  // The post-tool-addition call
  bool Realize() { Layout(); return TRUE; };

protected:
  int                   m_currentRowsOrColumns;
  long                  m_lastX, m_lastY;

DECLARE_EVENT_TABLE()

};

#endif // wxUSE_TOOLBAR
#endif
    // _WX_TBARSMPLH__

