/////////////////////////////////////////////////////////////////////////////
// Name:        toolbar.h
// Purpose:     wxToolBar class
// Author:      David Webster
// Modified by:
// Created:     10/17/98
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TOOLBAR_H_
#define _WX_TOOLBAR_H_

#if wxUSE_TOOLBAR
#include "wx/tbarbase.h"

WXDLLEXPORT_DATA(extern const wxChar*) wxToolBarNameStr;

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

  // Call default behaviour
  void OnMouseEvent(wxMouseEvent& event);

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

  virtual bool GetToolState(int toolIndex) const;

  // Add all the buttons
  virtual bool CreateTools();
  virtual void SetRows(int nRows);
  virtual void LayoutButtons() {}

  // The post-tool-addition call
  bool Realize() { return CreateTools(); };

  // IMPLEMENTATION
  virtual bool OS2Command(WXUINT param, WXWORD id);
  virtual bool OS2OnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result);

  // Responds to colour changes
  void OnSysColourChanged(wxSysColourChangedEvent& event);

protected:
  WXHBITMAP         m_hBitmap;

DECLARE_EVENT_TABLE()
private:
  //Virtual function hiding suppression
  inline wxToolBarTool *AddTool(int toolIndex, const wxBitmap& bitmap, const wxBitmap& pushedBitmap = wxNullBitmap,
                                bool toggle = FALSE, wxCoord xPos = -1, wxCoord yPos = -1, wxObject *clientData = NULL,
                                const wxString& helpString1 = "", const wxString& helpString2 = "")
  { return(AddTool(toolIndex, bitmap, pushedBitmap, toggle, (long)xPos, (long)yPos, clientData, helpString1, helpString2)); }
};

#endif // wxUSE_TOOLBAR

#endif
    // _WX_TOOLBAR_H_
