/////////////////////////////////////////////////////////////////////////////
// Name:        tbar95.h
// Purpose:     wxToolBar95 (Windows 95 toolbar) class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __TBAR95H__
#define __TBAR95H__

#ifdef __GNUG__
#pragma interface "tbar95.h"
#endif

#if USE_BUTTONBAR && USE_TOOLBAR
#include "wx/tbarbase.h"

WXDLLEXPORT_DATA(extern const char*) wxToolBarNameStr;

#define DEFAULTBITMAPX   16
#define DEFAULTBITMAPY   15
#define DEFAULTBUTTONX   24
#define DEFAULTBUTTONY   24
#define DEFAULTBARHEIGHT 27

class WXDLLEXPORT wxToolBar95: public wxToolBarBase
{
  DECLARE_DYNAMIC_CLASS(wxToolBar95)
 public:
  /*
   * Public interface
   */

  wxToolBar95(void);

#if WXWIN_COMPATIBILITY > 0
  inline wxToolBar95(wxWindow *parent, int x, int y, int w, int h,
            long style = wxNO_BORDER, int orientation = wxVERTICAL, int RowsOrColumns = 2,
            const char *name = wxToolBarNameStr)
  {
    Create(parent, -1, wxPoint(x, y), wxSize(w, h), style, orientation, RowsOrColumns, name);
  }
#endif
  inline wxToolBar95(wxWindow *parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
            long style = wxNO_BORDER, int orientation = wxVERTICAL,
            int RowsOrColumns = 1, const wxString& name = wxToolBarNameStr)
  {
    Create(parent, id, pos, size, style, orientation, RowsOrColumns, name);
  }
  ~wxToolBar95(void);

  bool Create(wxWindow *parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
            long style = wxNO_BORDER, int orientation = wxVERTICAL,
            int RowsOrColumns = 1, const wxString& name = wxToolBarNameStr);

  // Call default behaviour
  void OnPaint(wxPaintEvent& event) { Default() ; }
  void OnSize(wxSizeEvent& event) { Default() ; }
  void OnMouseEvent(wxMouseEvent& event) { Default() ; }
  void OnKillFocus(wxFocusEvent& event) { Default() ; }

  // Handle wxToolBar95 events

  // If pushedBitmap is NULL, a reversed version of bitmap is
  // created and used as the pushed/toggled image.
  // If toggle is TRUE, the button toggles between the two states.
  wxToolBarTool *AddTool(int toolIndex, const wxBitmap& bitmap, const wxBitmap& pushedBitmap = wxNullBitmap,
               bool toggle = FALSE, long xPos = -1, long yPos = -1, wxObject *clientData = NULL,
               const wxString& helpString1 = "", const wxString& helpString2 = "");

  // New members
  // Set default bitmap size
  void SetDefaultSize(const wxSize& size);
  void EnableTool(int toolIndex, bool enable); // additional drawing on enabling
  void ToggleTool(int toolIndex, bool toggle); // toggle is TRUE if toggled on
  void ClearTools(void);

  // The button size is bigger than the bitmap size
  wxSize GetDefaultButtonSize(void) const;

  wxSize GetMaxSize(void) const;
  void GetSize(int *w, int *y) const;

  // Add all the buttons: required for Win95.
  virtual bool CreateTools(void);
  virtual void SetRows(int nRows);
  virtual void Layout(void) {}

  // IMPLEMENTATION
  bool MSWCommand(WXUINT param, WXWORD id);
  bool MSWNotify(WXWPARAM wParam, WXLPARAM lParam);

  // Responds to colour changes
  void OnSysColourChanged(wxSysColourChangedEvent& event);

protected:
  WXHBITMAP         m_hBitmap;

DECLARE_EVENT_TABLE()
};

#endif // USE_TOOL/BUTTONBAR
#endif
    // __TBAR95H__
