/////////////////////////////////////////////////////////////////////////////
// Name:        scrolwin.h
// Purpose:     wxScrolledWindow class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __SCROLWINH_G__
#define __SCROLWINH_G__

#ifdef __GNUG__
#pragma interface "scrolwin.h"
#endif

#include "wx/window.h"

WXDLLEXPORT_DATA(extern const char*) wxPanelNameStr;

class WXDLLEXPORT wxScrolledWindow: public wxWindow
{
  DECLARE_ABSTRACT_CLASS(wxScrolledWindow)

public:
  wxScrolledWindow(void);
  inline wxScrolledWindow(wxWindow *parent, wxWindowID id = -1,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxHSCROLL|wxVSCROLL,
           const wxString& name = wxPanelNameStr)
  {
      Create(parent, id, pos, size, style, name);
  }

  inline ~wxScrolledWindow(void) {}

  bool Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxHSCROLL|wxVSCROLL,
           const wxString& name = wxPanelNameStr);

  // Set client size
  // Should take account of scrollbars
//  virtual void SetClientSize(int width, int size);

  // Is the window retained?
//  inline bool IsRetained(void) const;

  // Number of pixels per user unit (0 or -1 for no scrollbar)
  // Length of virtual canvas in user units
  // Length of page in user units
  virtual void SetScrollbars(int pixelsPerUnitX, int pixelsPerUnitY,
                             int noUnitsX, int noUnitsY,
                             int xPos = 0, int yPos = 0,
			     bool noRefresh = FALSE );

  // Physically scroll the window
  virtual void Scroll(int x_pos, int y_pos);

#if WXWIN_COMPATIBILITY
  virtual void GetScrollUnitsPerPage(int *x_page, int *y_page) const;
#endif

  int GetScrollPageSize(int orient) const ;
  void SetScrollPageSize(int orient, int pageSize);

  virtual void GetScrollPixelsPerUnit(int *x_unit, int *y_unit) const;
  // Enable/disable Windows scrolling in either direction.
  // If TRUE, wxWindows scrolls the canvas and only a bit of
  // the canvas is invalidated; no Clear() is necessary.
  // If FALSE, the whole canvas is invalidated and a Clear() is
  // necessary. Disable for when the scroll increment is used
  // to actually scroll a non-constant distance
  virtual void EnableScrolling(bool x_scrolling, bool y_scrolling);

  // Get the view start
  virtual void ViewStart(int *x, int *y) const;

  // Actual size in pixels when scrolling is taken into account
  virtual void GetVirtualSize(int *x, int *y) const;

  // Set the scale factor, used in PrepareDC
  inline void SetScale(double xs, double ys) { m_scaleX = xs; m_scaleY = ys; }
  inline double GetScaleX() const { return m_scaleX; }
  inline double GetScaleY() const { return m_scaleY; }

  virtual void CalcScrolledPosition(int x, int y, int *xx, int *yy) const ;
  virtual void CalcUnscrolledPosition(int x, int y, float *xx, float *yy) const ;

  // Adjust the scrollbars
  virtual void AdjustScrollbars(void);

  void OnScroll(wxScrollEvent& event);
  void OnSize(wxSizeEvent& event);
  void OnPaint(wxPaintEvent& event);

  // Override this function to draw the graphic.
  virtual void OnDraw(wxDC& WXUNUSED(dc)) {};

  // Override this function if you don't want to have wxScrolledWindow
  // automatically change the origin according to the scroll position.
  virtual void PrepareDC(wxDC& dc);

public:
  ////////////////////////////////////////////////////////////////////////
  //// IMPLEMENTATION
  
  // Calculate scroll increment
  virtual int CalcScrollInc(wxScrollEvent& event);

  ////////////////////////////////////////////////////////////////////////
  //// PROTECTED DATA
protected:
  int                   m_xScrollPixelsPerLine;
  int                   m_yScrollPixelsPerLine;
  bool                  m_xScrollingEnabled;
  bool                  m_yScrollingEnabled;
  int                   m_xScrollPosition;
  int                   m_yScrollPosition;
  int                   m_xScrollLines;
  int                   m_yScrollLines;
  int                   m_xScrollLinesPerPage;
  int                   m_yScrollLinesPerPage;
  double                m_scaleX;
  double                m_scaleY;

DECLARE_EVENT_TABLE()
};

////////////////////////////////////////////////////////////////////////
//// INLINES

#endif
    // __SCROLWINH_G__
