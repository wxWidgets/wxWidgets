/////////////////////////////////////////////////////////////////////////////
// Name:        palette.cpp
// Purpose:     OGLEdit palette
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
// #pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/toolbar.h>

#include <ctype.h>
#include <stdlib.h>
#include <math.h>

#include "doc.h"
#include "view.h"
#include "ogledit.h"
#include "palette.h"

// Include pixmaps
#if defined(__WXGTK__) || defined(__WXMOTIF__)
#include "bitmaps/arrow.xpm"
#include "bitmaps/tool1.xpm"
#include "bitmaps/tool2.xpm"
#include "bitmaps/tool3.xpm"
#include "bitmaps/tool4.xpm"
#endif

/*
 * Object editor tool palette
 *
 */

EditorToolPalette::EditorToolPalette(wxWindow* parent, const wxPoint& pos, const wxSize& size,
            long style):
  TOOLPALETTECLASS(parent, -1, pos, size, style)
{
  currentlySelected = -1;

  SetMaxRowsCols(1000, 1);
}

bool EditorToolPalette::OnLeftClick(int toolIndex, bool toggled)
{
  // BEGIN mutual exclusivity code
  if (toggled && (currentlySelected != -1) && (toolIndex != currentlySelected))
    ToggleTool(currentlySelected, FALSE);

  if (toggled)
    currentlySelected = toolIndex;
  else if (currentlySelected == toolIndex)
    currentlySelected = -1;
  //  END mutual exclusivity code

  return TRUE;
}

void EditorToolPalette::OnMouseEnter(int toolIndex)
{
}

void EditorToolPalette::SetSize(int x, int y, int width, int height, int sizeFlags)
{
  TOOLPALETTECLASS::SetSize(x, y, width, height, sizeFlags);
}

EditorToolPalette *MyApp::CreatePalette(wxFrame *parent)
{
  // Load palette bitmaps
#ifdef __WXMSW__
  wxBitmap PaletteTool1("TOOL1");
  wxBitmap PaletteTool2("TOOL2");
  wxBitmap PaletteTool3("TOOL3");
  wxBitmap PaletteTool4("TOOL4");
  wxBitmap PaletteArrow("ARROWTOOL");
#elif defined(__WXGTK__) || defined(__WXMOTIF__)
  wxBitmap PaletteTool1(tool1_xpm);
  wxBitmap PaletteTool2(tool2_xpm);
  wxBitmap PaletteTool3(tool3_xpm);
  wxBitmap PaletteTool4(tool4_xpm);
  wxBitmap PaletteArrow(arrow_xpm);
#endif

  EditorToolPalette *palette = new EditorToolPalette(parent, wxPoint(0, 0), wxSize(-1, -1), wxTB_HORIZONTAL);

  palette->SetMargins(2, 2);

#ifdef __WXMSW__
  if (palette->IsKindOf(CLASSINFO(wxToolBar95)))
    ((wxToolBar95 *)palette)->SetToolBitmapSize(wxSize(22, 22));
#endif

  palette->AddTool(PALETTE_ARROW, PaletteArrow, wxNullBitmap, TRUE, 0, -1, NULL, "Pointer");
  palette->AddTool(PALETTE_TOOL1, PaletteTool1, wxNullBitmap, TRUE, 0, -1, NULL, "Tool 1");
  palette->AddTool(PALETTE_TOOL2, PaletteTool2, wxNullBitmap, TRUE, 0, -1, NULL, "Tool 2");
  palette->AddTool(PALETTE_TOOL3, PaletteTool3, wxNullBitmap, TRUE, 0, -1, NULL, "Tool 3");
  palette->AddTool(PALETTE_TOOL4, PaletteTool4, wxNullBitmap, TRUE, 0, -1, NULL, "Tool 4");

  palette->Realize();

  palette->ToggleTool(PALETTE_ARROW, TRUE);
  palette->currentlySelected = PALETTE_ARROW;
  return palette;
}

