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

class WXDLLEXPORT wxMemoryDC;

// Non-Win95 (WIN32, WIN16, UNIX) version

class WXDLLEXPORT wxToolBarMSW: public wxToolBarBase
{
  DECLARE_DYNAMIC_CLASS(wxToolBarMSW)
public:
  /*
   * Public interface
   */
  wxToolBarMSW(void);

  inline wxToolBarMSW(wxWindow *parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
            long style = wxNO_BORDER|wxTB_HORIZONTAL, const wxString& name = wxButtonBarNameStr)
  {
    Create(parent, id, pos, size, style, name);
  }
  bool Create(wxWindow *parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
            long style = wxNO_BORDER|wxTB_HORIZONTAL, const wxString& name = wxButtonBarNameStr);

  ~wxToolBarMSW(void);

  // Handle wxWindows events
  void OnPaint(wxPaintEvent& event);
  void OnSize(wxSizeEvent& event);
  void OnMouseEvent(wxMouseEvent& event);

  // If pushedBitmap is NULL, a reversed version of bitmap is
  // created and used as the pushed/toggled image.
  // If toggle is TRUE, the button toggles between the two states.
  wxToolBarTool *AddTool(int toolIndex, const wxBitmap& bitmap, const wxBitmap& pushedBitmap = wxNullBitmap,
               bool toggle = FALSE, long xPos = -1, long yPos = -1, wxObject *clientData = NULL,
               const wxString& helpString1 = "", const wxString& helpString2 = "");

  void DrawTool(wxDC& dc, wxMemoryDC& memDc, wxToolBarTool *tool);

  // Set default bitmap size
  virtual void SetToolBitmapSize(const wxSize& size);
  void EnableTool(int toolIndex, bool enable); // additional drawing on enabling

  // The button size is bigger than the bitmap size
  wxSize GetToolSize(void) const;

  void Layout(void);

  // The post-tool-addition call
  bool Realize() { Layout(); return TRUE; };

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
  int               m_currentRowsOrColumns;
  long              m_lastX, m_lastY;

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
