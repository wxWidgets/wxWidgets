/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/statusbr.h
// Purpose:     wxStatusBarGeneric class
// Author:      Julian Smart
// Modified by: VZ at 05.02.00 to derive from wxStatusBarBase
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_STATUSBR_H_
#define _WX_GENERIC_STATUSBR_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "statusbr.h"
#endif

#include "wx/pen.h"
#include "wx/font.h"
#include "wx/statusbr.h"
#include "wx/arrstr.h"

extern WXDLLEXPORT_DATA(const wxChar*) wxPanelNameStr;

class WXDLLEXPORT wxStatusBarGeneric : public wxStatusBarBase
{
public:
  wxStatusBarGeneric() { Init(); }
  wxStatusBarGeneric(wxWindow *parent,
              wxWindowID winid,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxFULL_REPAINT_ON_RESIZE,
              const wxString& name = wxPanelNameStr)
  {
      Init();

      Create(parent, winid, pos, size, style, name);
  }
  wxStatusBarGeneric(wxWindow *parent,
                     wxWindowID winid,
                     long style,
                     const wxString& name = wxPanelNameStr)
  {
      Init();

      Create(parent, winid, style, name);
  }

  virtual ~wxStatusBarGeneric();

  bool Create(wxWindow *parent, wxWindowID winid,
              const wxPoint& WXUNUSED(pos) = wxDefaultPosition,
              const wxSize& WXUNUSED(size) = wxDefaultSize,
              long style = wxFULL_REPAINT_ON_RESIZE,
              const wxString& name = wxPanelNameStr)
  {
      return Create(parent, winid, style, name);
  }

  bool Create(wxWindow *parent, wxWindowID winid,
              long style,
              const wxString& name = wxPanelNameStr);

  // Create status line
  virtual void SetFieldsCount(int number = 1,
                              const int *widths = (const int *) NULL);

  // Set status line text
  virtual void SetStatusText(const wxString& text, int number = 0);
  virtual wxString GetStatusText(int number = 0) const;

  // Set status line widths
  virtual void SetStatusWidths(int n, const int widths_field[]);

  // Get the position and size of the field's internal bounding rectangle
  virtual bool GetFieldRect(int i, wxRect& rect) const;

  // sets the minimal vertical size of the status bar
  virtual void SetMinHeight(int height);

  virtual int GetBorderX() const { return m_borderX; }
  virtual int GetBorderY() const { return m_borderY; }

  ////////////////////////////////////////////////////////////////////////
  // Implementation

  virtual void DrawFieldText(wxDC& dc, int i);
  virtual void DrawField(wxDC& dc, int i);

  void SetBorderX(int x);
  void SetBorderY(int y);

  void OnPaint(wxPaintEvent& event);

  void OnLeftDown(wxMouseEvent& event);
  void OnRightDown(wxMouseEvent& event);

  virtual void InitColours();

  // Responds to colour changes
  void OnSysColourChanged(wxSysColourChangedEvent& event);

protected:
  // common part of all ctors
  void Init();

  wxArrayString     m_statusStrings;

  // the last known width of the client rect (used to rebuild cache)
  int               m_lastClientWidth;
  // the widths of the status bar panes in pixels
  wxArrayInt        m_widthsAbs;

  int               m_borderX;
  int               m_borderY;
  wxPen             m_mediumShadowPen;
  wxPen             m_hilightPen;

  virtual wxSize DoGetBestSize() const;

private:
  DECLARE_EVENT_TABLE()
  DECLARE_DYNAMIC_CLASS_NO_COPY(wxStatusBarGeneric)
};

#endif
    // _WX_GENERIC_STATUSBR_H_
