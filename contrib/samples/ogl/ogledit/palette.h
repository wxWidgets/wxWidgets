/////////////////////////////////////////////////////////////////////////////
// Name:        contrib/samples/ogl/ogledit/palette.h
// Purpose:     OGL sample palette
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _OGLSAMPLE_PALETTE_H_
#define _OGLSAMPLE_PALETTE_H_

#include "wx/wx.h"
#include "wx/string.h"
#include "wx/deprecated/setup.h"
#include "wx/deprecated/tbarsmpl.h"

/*
 * Object editor tool palette
 *
 */

// TODO for wxWin: wxToolBar95 cannot be moved to a non-0,0 position!
// Needs to have a parent window...
// So use a simple toolbar at present.
// ABX: Simple toolbar is not available in default compilation
//      so I use wxToolBar anyway
// #if 0 // def __WXGTK__
// #define TOOLPALETTECLASS    wxToolBar
// #else
// #define TOOLPALETTECLASS    wxToolBarSimple
// #endif
#define TOOLPALETTECLASS    wxToolBar

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
