/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/statusbr.h
// Purpose:     wxStatusBarGeneric class
// Author:      Julian Smart
// Modified by: VZ at 05.02.00 to derive from wxStatusBarBase
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_STATUSBR_H_
#define _WX_GENERIC_STATUSBR_H_

#ifdef __GNUG__
#pragma interface "statusbr.h"
#endif

#include "wx/pen.h"
#include "wx/font.h"

WXDLLEXPORT_DATA(extern const wxChar*) wxPanelNameStr;

class WXDLLEXPORT wxStatusBarGeneric : public wxStatusBarBase
{
public:
  wxStatusBarGeneric();
  wxStatusBarGeneric(wxWindow *parent,
              wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = 0,
              const wxString& name = wxPanelNameStr)
  {
      Create(parent, id, pos, size, style, name);
  }
  wxStatusBarGeneric(wxWindow *parent,
              wxWindowID id,
              long style,
              const wxString& name = wxPanelNameStr)
  {
      Create(parent, id, style, name);
  }

  ~wxStatusBarGeneric();

  bool Create(wxWindow *parent, wxWindowID id,
              const wxPoint& WXUNUSED(pos) = wxDefaultPosition,
              const wxSize& WXUNUSED(size) = wxDefaultSize,
              long style = 0,
              const wxString& name = wxPanelNameStr)
  {
      return Create(parent, id, style, name);
  }

  bool Create(wxWindow *parent, wxWindowID id,
              long style = 0,
              const wxString& name = wxPanelNameStr);

  // Create status line
  virtual void SetFieldsCount(int number = 1,
                              const int *widths = (const int *) NULL);
  int GetFieldsCount() const { return m_nFields; }

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

  virtual void InitColours();

  // Responds to colour changes
  void OnSysColourChanged(wxSysColourChangedEvent& event);

protected:
  wxString *        m_statusStrings;
  int               m_borderX;
  int               m_borderY;
  wxFont            m_defaultStatusBarFont;
  wxPen             m_mediumShadowPen;
  wxPen             m_hilightPen;

private:
  DECLARE_EVENT_TABLE()
  DECLARE_DYNAMIC_CLASS(wxStatusBarGeneric)
};

#endif
    // _WX_GENERIC_STATUSBR_H_
