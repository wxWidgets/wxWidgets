/////////////////////////////////////////////////////////////////////////////
// Name:        cspalette.h
// Purpose:     OGL sample palette
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _STUDIO_CSPALETTE_H_
#define _STUDIO_CSPALETTE_H_

#ifdef __GNUG__
// #pragma interface
#endif

#include <wx/wx.h>
#include <wx/string.h>
#include <wx/tbarsmpl.h>

/*
 * Object editor tool palette
 *
 */

// TODO for wxWin: wxToolBar95 cannot be moved to a non-0,0 position!
// Needs to have a parent window...
// So use a simple toolbar at present.
#define TOOLPALETTECLASS    wxToolBarSimple

class csEditorToolPalette: public TOOLPALETTECLASS
{
public:

  csEditorToolPalette(wxWindow *parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition,
    const wxSize& size = wxDefaultSize,
    long style = wxTB_VERTICAL);

  bool OnLeftClick(int toolIndex, bool toggled);
  void OnMouseEnter(int toolIndex);

  inline int GetSelection() const { return m_currentlySelected; }
  void SetSelection(int sel);

  void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);

protected:
  int           m_currentlySelected;
};

#define PALETTE_ARROW           200
#define PALETTE_TEXT_TOOL       201

#endif
    // _STUDIO_CSPALETTE_H_
