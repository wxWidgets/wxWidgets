/////////////////////////////////////////////////////////////////////////////
// Name:        palette.h
// Purpose:     OGL sample palette
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _OGLSAMPLE_PALETTE_H_
#define _OGLSAMPLE_PALETTE_H_

#ifdef __GNUG__
// #pragma interface
#endif

#include <wx/wx.h>
#include <wx/string.h>
#if 0 // def __WXGTK__
#include <wx/toolbar.h>
#else
#include <wx/tbarsmpl.h>
#endif

/*
 * Object editor tool palette
 *
 */

// TODO for wxWin: wxToolBar95 cannot be moved to a non-0,0 position!
// Needs to have a parent window...
// So use a simple toolbar at present.
#if 0 // def __WXGTK__
#define TOOLPALETTECLASS    wxToolBar
#else
#define TOOLPALETTECLASS    wxToolBarSimple
#endif

class EditorToolPalette: public TOOLPALETTECLASS
{
 public:
  int currentlySelected;

  EditorToolPalette(wxWindow *parent, const wxPoint& pos = wxDefaultPosition,
    const wxSize& size = wxDefaultSize,
    long style = wxTB_VERTICAL);
  bool OnLeftClick(int toolIndex, bool toggled);
  void OnMouseEnter(int toolIndex);
  void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
};

#define PALETTE_TOOL1           1
#define PALETTE_TOOL2           2
#define PALETTE_TOOL3           3
#define PALETTE_TOOL4           4
#define PALETTE_TOOL5           5
#define PALETTE_TOOL6           6
#define PALETTE_TOOL7           7
#define PALETTE_TOOL8           8
#define PALETTE_TOOL9           9
#define PALETTE_ARROW           10

#endif
    // _OGLSAMPLE_PALETTE_H_
