/////////////////////////////////////////////////////////////////////////////
// Name:        toolbar.h
// Purpose:     wxToolBar class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TOOLBAR_H_
#define _WX_TOOLBAR_H_

#ifdef __GNUG__
#pragma interface "toolbar.h"
#endif

#include "wx/tbarbase.h"

WXDLLEXPORT_DATA(extern const char*) wxToolBarNameStr;

class WXDLLEXPORT wxToolBar: public wxToolBarBase
{
  DECLARE_DYNAMIC_CLASS(wxToolBar)
 public:
  /*
   * Public interface
   */

  wxToolBar();

  inline wxToolBar(wxWindow *parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
            long style = wxNO_BORDER|wxTB_HORIZONTAL,
		   const wxString& name = wxToolBarNameStr):
  m_widgets(wxKEY_INTEGER)

  {
    Create(parent, id, pos, size, style, name);
  }
  ~wxToolBar();

  bool Create(wxWindow *parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
            long style = wxNO_BORDER|wxTB_HORIZONTAL,
            const wxString& name = wxToolBarNameStr);

  // If pushedBitmap is NULL, a reversed version of bitmap is
  // created and used as the pushed/toggled image.
  // If toggle is TRUE, the button toggles between the two states.
  wxToolBarTool *AddTool(int toolIndex, const wxBitmap& bitmap, const wxBitmap& pushedBitmap = wxNullBitmap,
               bool toggle = FALSE, long xPos = -1, long yPos = -1, wxObject *clientData = NULL,
               const wxString& helpString1 = "", const wxString& helpString2 = "");

  // Set default bitmap size
  void SetToolBitmapSize(const wxSize& size);
  void EnableTool(int toolIndex, bool enable); // additional drawing on enabling
  void ToggleTool(int toolIndex, bool toggle); // toggle is TRUE if toggled on
  void ClearTools();

  // The button size is bigger than the bitmap size
  wxSize GetToolSize() const;

  wxSize GetMaxSize() const;

  // Add all the buttons
  virtual bool CreateTools();
  virtual void Layout() {}

  // The post-tool-addition call. TODO: do here whatever's
  // necessary for completing the toolbar construction.
  bool Realize() { return CreateTools(); };

// Implementation
  void DestroyPixmaps();
  int FindIndexForWidget(WXWidget w);
  WXWidget FindWidgetForIndex(int index);

  WXWidget GetTopWidget() const;
  WXWidget GetClientWidget() const;
  WXWidget GetMainWidget() const;

protected:
  // List of widgets in the toolbar, indexed by tool index
  wxList    m_widgets;

  // List of pixmaps to destroy when tools are recreated or
  // or toolbar is destroyed.
  wxList    m_pixmaps;

DECLARE_EVENT_TABLE()
};

#endif
    // _WX_TOOLBAR_H_
