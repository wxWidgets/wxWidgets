/////////////////////////////////////////////////////////////////////////////
// Name:        tbarbase.h
// Purpose:     Base class for toolbar classes
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __TBARBASEH__
#define __TBARBASEH__

#ifdef __GNUG__
#pragma interface "tbarbase.h"
#endif

#include "wx/defs.h"

#if USE_TOOLBAR

#include "wx/bitmap.h"
#include "wx/list.h"
#include "wx/control.h"

WXDLLEXPORT_DATA(extern const char*) wxToolBarNameStr;
WXDLLEXPORT_DATA(extern const wxSize) wxDefaultSize;
WXDLLEXPORT_DATA(extern const wxPoint) wxDefaultPosition;

#define wxTOOL_STYLE_BUTTON          1
#define wxTOOL_STYLE_SEPARATOR       2

class WXDLLEXPORT wxToolBarTool: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxToolBarTool)
 public:
  wxToolBarTool(int theIndex = 0, const wxBitmap& bitmap1 = wxNullBitmap, const wxBitmap& bitmap2 = wxNullBitmap,
                bool toggle = FALSE, long xPos = -1, long yPos = -1,
                const wxString& shortHelpString = "", const wxString& longHelpString = "");
  ~wxToolBarTool ();
  inline void SetSize( long w, long h ) { m_width = w; m_height = h; }
  inline long GetWidth () const { return m_width; }
  inline long GetHeight () const { return m_height; }

public:
  int                   m_toolStyle;
  wxObject *            m_clientData;
  int                   m_index;
  long                 m_x;
  long                 m_y;
  long                 m_width;
  long                 m_height;
  bool                  m_toggleState;
  bool                  m_isToggle;
  bool                  m_deleteSecondBitmap;
  bool                  m_enabled;
  wxBitmap              m_bitmap1;
  wxBitmap              m_bitmap2;
  bool                  m_isMenuCommand;
  wxString              m_shortHelpString;
  wxString              m_longHelpString;
};

class WXDLLEXPORT wxToolBarBase : public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxToolBarBase)
 public:

  wxToolBarBase(void);
  ~wxToolBarBase(void);

  // Handle wxToolBar events

  // Only allow toggle if returns TRUE. Call when left button up.
  virtual bool OnLeftClick(int toolIndex, bool toggleDown);

  // Call when right button down.
  virtual void OnRightClick(int toolIndex, long x, long y);

  // Called when the mouse cursor enters a tool bitmap.
  // Argument is -1 if mouse is exiting the toolbar.
  virtual void OnMouseEnter(int toolIndex);

  // If pushedBitmap is NULL, a reversed version of bitmap is
  // created and used as the pushed/toggled image.
  // If toggle is TRUE, the button toggles between the two states.
  virtual wxToolBarTool *AddTool(int toolIndex, const wxBitmap& bitmap, const wxBitmap& pushedBitmap = wxNullBitmap,
               bool toggle = FALSE, long xPos = -1, long yPos = -1, wxObject *clientData = NULL,
               const wxString& helpString1 = "", const wxString& helpString2 = "");
  virtual void AddSeparator(void);
  virtual void ClearTools(void);

  virtual void EnableTool(int toolIndex, bool enable);
  virtual void ToggleTool(int toolIndex, bool toggle); // toggle is TRUE if toggled on
  virtual void SetToggle(int toolIndex, bool toggle); // Set this to be togglable (or not)
  virtual wxObject *GetToolClientData(int index) const;
  inline wxList& GetTools(void) const { return (wxList&) m_tools; }

  // After the toolbar has initialized, this is the size the tools take up
#if WXWXIN_COMPATIBILITY
  inline void GetMaxSize ( long * width, long * height ) const
   { wxSize maxSize(GetMaxSize()); *width = maxSize.x; *height = maxSize.y; }
#endif
  virtual wxSize GetMaxSize ( void ) const;

  virtual bool GetToolState(int toolIndex) const;
  virtual bool GetToolEnabled(int toolIndex) const;
  virtual wxToolBarTool *FindToolForPosition(long x, long y) const;

  virtual void SetToolShortHelp(int toolIndex, const wxString& helpString);
  virtual wxString GetToolShortHelp(int toolIndex) const;
  virtual void SetToolLongHelp(int toolIndex, const wxString& helpString);
  virtual wxString GetToolLongHelp(int toolIndex) const;

  virtual void SetMargins(int x, int y);
  inline void SetMargins(const wxSize& size) { SetMargins(size.x, size.y); }
  virtual void SetToolPacking(int packing);
  virtual void SetToolSeparation(int separation);

  inline virtual wxSize GetToolMargins(void) { return wxSize(m_xMargin, m_yMargin); }
  inline virtual int GetToolPacking(void) { return m_toolPacking; }
  inline virtual int GetToolSeparation(void) { return m_toolSeparation; }

  virtual void SetDefaultSize(const wxSize& size) { m_defaultWidth = size.x; m_defaultHeight = size.y; };
  virtual wxSize GetDefaultSize(void) const { return wxSize(m_defaultWidth, m_defaultHeight); }

  // The button size (in some implementations) is bigger than the bitmap size: this returns
  // the total button size.
  virtual wxSize GetDefaultButtonSize(void) const { return wxSize(m_defaultWidth, m_defaultHeight); } ;

  // Compatibility
#if WXWIN_COMPATIBILITY
  inline void SetDefaultSize(int w, int h) { SetDefaultSize(wxSize(w, h)); }
  inline long GetDefaultWidth(void) const { return m_defaultWidth; }
  inline long GetDefaultHeight(void) const { return m_defaultHeight; }
  inline int GetDefaultButtonWidth(void) const { return GetDefaultButtonSize().x; };
  inline int GetDefaultButtonHeight(void) const { return GetDefaultButtonSize().y; };
#endif

  // Lay the tools out
  virtual void Layout(void);
  
  // Add all the buttons: required for Win95.
  // TODO: unify API so there's no ambiguity
  virtual bool CreateTools(void) { return TRUE; }

  void Command(wxCommandEvent& event);

  // SCROLLING: this has to be copied from wxScrolledWindow since wxToolBarBase
  // inherits from wxControl. This could have been put into wxToolBarSimple,
  // but we might want any derived toolbar class to be scrollable.

  // Number of pixels per user unit (0 or -1 for no scrollbar)
  // Length of virtual canvas in user units
  virtual void SetScrollbars(int horizontal, int vertical,
                             int x_length, int y_length,
                             int x_pos = 0, int y_pos = 0);

  // Physically scroll the window
  virtual void Scroll(int x_pos, int y_pos);
  virtual void GetScrollPixelsPerUnit(int *x_unit, int *y_unit) const;
  virtual void EnableScrolling(bool x_scrolling, bool y_scrolling);
  virtual void AdjustScrollbars(void);

  // Prepare the DC by translating it according to the current scroll position
  virtual void PrepareDC(wxDC& dc);

  int GetScrollPageSize(int orient) const ;
  void SetScrollPageSize(int orient, int pageSize);

  // Get the view start
  virtual void ViewStart(int *x, int *y) const;

  // Actual size in pixels when scrolling is taken into account
  virtual void GetVirtualSize(int *x, int *y) const;

  // Do the toolbar button updates (check for EVT_UPDATE_UI handlers)
  virtual void DoToolbarUpdates(void);

/*
  virtual void CalcScrolledPosition(int x, int y, int *xx, int *yy) const ;
  virtual void CalcUnscrolledPosition(int x, int y, long *xx, long *yy) const ;
*/

  void OnScroll(wxScrollEvent& event);
  void OnSize(wxSizeEvent& event);
  void OnIdle(wxIdleEvent& event);

  // Required to force normal cursor-setting behaviour in Windows
#ifdef __WXMSW__
  virtual void MSWOnMouseMove(int x, int y, const WXUINT flags);
#endif

 protected:
  wxList                m_tools;
  int                   m_tilingDirection;
  int                   m_rowsOrColumns;
  int                   m_currentRowsOrColumns;
  long                  m_lastX, m_lastY;
  long                  m_maxWidth, m_maxHeight;
  int                   m_currentTool; // Tool where mouse currently is
  int                   m_pressedTool; // Tool where mouse pressed
  int                   m_xMargin;
  int                   m_yMargin;
  int                   m_toolPacking;
  int                   m_toolSeparation;
  long                  m_defaultWidth;
  long                  m_defaultHeight;

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
  bool                  m_calcScrolledOffset; // If TRUE, wxCanvasDC uses scrolled offsets
  int                   m_xScrollLines;
  int                   m_yScrollLines;
  int                   m_xScrollLinesPerPage;
  int                   m_yScrollLinesPerPage;

public:
    DECLARE_EVENT_TABLE()
};

#endif // USE_TOOLBAR
#endif
    // __TBARBASEH__

