/////////////////////////////////////////////////////////////////////////////
// Name:        tbarmsw.h
// Purpose:     wxToolBarMSW class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __TBARMSWH__
#define __TBARMSWH__

#ifdef __GNUG__
#pragma interface "tbarmsw.h"
#endif

#if USE_BUTTONBAR && USE_TOOLBAR
#include "wx/tbarbase.h"

WXDLLEXPORT_DATA(extern const char*) wxButtonBarNameStr;

// Non-Win95 (WIN32, WIN16, UNIX) version

class WXDLLEXPORT wxToolBarMSW: public wxToolBarBase
{
  DECLARE_DYNAMIC_CLASS(wxToolBarMSW)
public:
  /*
   * Public interface
   */
  wxToolBarMSW(void);

#if WXWIN_COMPATIBILITY > 0
  inline wxToolBarMSW(wxWindow *parent, int x, int y, int w, int h,
            long style = wxNO_BORDER, int orientation = wxVERTICAL, int RowsOrColumns = 2,
            const char *name = wxButtonBarNameStr)
  {
    Create(parent, -1, wxPoint(x, y), wxSize(w, h), style, orientation, RowsOrColumns, name);
  }
#endif
  inline wxToolBarMSW(wxWindow *parent, const wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
            const long style = wxNO_BORDER, const int orientation = wxVERTICAL,
            const int RowsOrColumns = 2, const wxString& name = wxButtonBarNameStr)
  {
    Create(parent, id, pos, size, style, orientation, RowsOrColumns, name);
  }
  bool Create(wxWindow *parent, const wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
            const long style = wxNO_BORDER, const int orientation = wxVERTICAL,
            const int RowsOrColumns = 2, const wxString& name = wxButtonBarNameStr);

  ~wxToolBarMSW(void);

  // Handle wxWindows events
  void OnPaint(wxPaintEvent& event);
  void OnSize(wxSizeEvent& event);
  void OnMouseEvent(wxMouseEvent& event);

  // If pushedBitmap is NULL, a reversed version of bitmap is
  // created and used as the pushed/toggled image.
  // If toggle is TRUE, the button toggles between the two states.
  wxToolBarTool *AddTool(const int toolIndex, const wxBitmap& bitmap, const wxBitmap& pushedBitmap = wxNullBitmap,
               const bool toggle = FALSE, const long xPos = -1, const long yPos = -1, wxObject *clientData = NULL,
               const wxString& helpString1 = "", const wxString& helpString2 = "");

  void DrawTool(wxDC& dc, wxMemoryDC& memDc, wxToolBarTool *tool);

  // New members
  // Set default bitmap size
  virtual void SetDefaultSize(const wxSize& size);
  void EnableTool(const int toolIndex, const bool enable); // additional drawing on enabling

  // The button size is bigger than the bitmap size
  wxSize GetDefaultButtonSize(void) const;
 protected:
  void DrawTool(wxDC& dc, wxToolBarTool *tool, int state);

  void GetSysColors(void);
  bool InitGlobalObjects(void);
  void FreeGlobalObjects(void);
  void PatB(WXHDC hdc,int x,int y,int dx,int dy, long rgb);
  void CreateMask(WXHDC hDC, int xoffset, int yoffset, int dx, int dy);
  void DrawBlankButton(WXHDC hdc, int x, int y, int dx, int dy, int state);
  void DrawButton(WXHDC hdc, int x, int y, int dx, int dy, wxToolBarTool *tool, int state);
  WXHBITMAP CreateDitherBitmap();
  bool CreateDitherBrush(void);
  bool FreeDitherBrush(void);
  WXHBITMAP CreateMappedBitmap(WXHINSTANCE hInstance, void *lpBitmapInfo);
  WXHBITMAP CreateMappedBitmap(WXHINSTANCE hInstance, WXHBITMAP hBitmap);

 protected:

  WXHBRUSH          m_hbrDither;
  WXDWORD           m_rgbFace;
  WXDWORD           m_rgbShadow;
  WXDWORD           m_rgbHilight;
  WXDWORD           m_rgbFrame;

//
// m_hdcMono is the DC that holds a mono bitmap, m_hbmMono
// that is used to create highlights
// of button faces.
// m_hbmDefault hold the default bitmap if there is one.
//
  WXHDC             m_hdcMono;
  WXHBITMAP         m_hbmMono;
  WXHBITMAP         m_hbmDefault;

DECLARE_EVENT_TABLE()
};

#define DEFAULTBITMAPX   16
#define DEFAULTBITMAPY   15
#define DEFAULTBUTTONX   24
#define DEFAULTBUTTONY   22
#define DEFAULTBARHEIGHT 27

//
// States (not all of them currently used)
//
#define wxTBSTATE_CHECKED         0x01    // radio button is checked
#define wxTBSTATE_PRESSED         0x02    // button is being depressed (any style)
#define wxTBSTATE_ENABLED         0x04    // button is enabled
#define wxTBSTATE_HIDDEN          0x08    // button is hidden
#define wxTBSTATE_INDETERMINATE   0x10    // button is indeterminate

#endif // USE_TOOL/BUTTONBAR
#endif
    // __TBARMSWH__
