/////////////////////////////////////////////////////////////////////////////
// Name:        toolbar.h
// Purpose:     wxToolBar class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TOOLBAR_H_
#define _WX_TOOLBAR_H_

#ifdef __GNUG__
#pragma interface "toolbar.h"
#endif

#if wxUSE_TOOLBAR

#include "wx/tbarbase.h"
#include "wx/dynarray.h"

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
            const wxString& name = wxToolBarNameStr)
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
 	virtual void LayoutButtons() {}

  // The post-tool-addition call. TODO: do here whatever's
  // necessary for completing the toolbar construction.
  bool Realize() { return CreateTools(); };
	virtual void MacHandleControlClick( ControlHandle control , SInt16 controlpart ) ;
protected:
	wxArrayPtrVoid	m_macToolHandles ;

DECLARE_EVENT_TABLE()
};

#endif // wxUSE_TOOLBAR

#endif
    // _WX_TOOLBAR_H_
