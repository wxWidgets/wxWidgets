/////////////////////////////////////////////////////////////////////////////
// Name:        editrpal.h
// Purpose:     Tool palette
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _EDITRPAL_H_
#define _EDITRPAL_H_

#ifdef __GNUG__
#pragma interface "editrpal.h"
#endif

#include "wx/wx.h"
#include "wx/string.h"
#include "wx/layout.h"
#include "wx/resource.h"
#include "wx/tbarsmpl.h"

#include "proplist.h"

/*
 * Object editor tool palette
 *
 */

// For some reason, wxButtonBar under Windows 95 cannot be moved to a non-0,0 position!
#define TOOLPALETTECLASS    wxToolBarSimple

class EditorToolPalette: public TOOLPALETTECLASS
{
 public:
  int currentlySelected;
  wxResourceManager *resourceManager;

  EditorToolPalette(wxResourceManager *manager, wxFrame *frame, int x = 0, int y = 0, int w = -1, int h = -1,
            long style = wxNO_BORDER, int direction = wxVERTICAL, int RowsOrColumns = 2);
  bool OnLeftClick(int toolIndex, bool toggled);
  void OnMouseEnter(int toolIndex);
  void OnPaint(wxPaintEvent& event);

DECLARE_EVENT_TABLE()
};

#define PALETTE_FRAME           1
#define PALETTE_DIALOG_BOX      2
#define PALETTE_PANEL           3
#define PALETTE_CANVAS          4
#define PALETTE_TEXT_WINDOW     5
#define PALETTE_MESSAGE         6
#define PALETTE_BUTTON          7
#define PALETTE_CHECKBOX        8
#define PALETTE_LISTBOX         9
#define PALETTE_RADIOBOX        10
#define PALETTE_CHOICE          11
#define PALETTE_TEXT            12
#define PALETTE_MULTITEXT       13
#define PALETTE_SLIDER          14
#define PALETTE_ARROW           15
#define PALETTE_GAUGE           16
#define PALETTE_GROUPBOX        17
#define PALETTE_BITMAP_MESSAGE  18
#define PALETTE_BITMAP_BUTTON   19
#define PALETTE_SCROLLBAR       20

#endif
