/////////////////////////////////////////////////////////////////////////////
// Name:        editrpal.cpp
// Purpose:     Editor tool palette
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "editrpal.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#if defined(__WINDOWS__) && !defined(__GNUWIN32__)
#include <strstrea.h>
#else
#include <strstream.h>
#endif

#include "reseditr.h"
#include "editrpal.h"

#ifdef __X__
#include "bitmaps/frame.xbm"
#include "bitmaps/dialog.xbm"
#include "bitmaps/panel.xbm"
#include "bitmaps/canvas.xbm"
#include "bitmaps/textsw.xbm"
#include "bitmaps/message.xbm"
#include "bitmaps/button.xbm"
#include "bitmaps/check.xbm"
#include "bitmaps/listbox.xbm"
#include "bitmaps/radio.xbm"
#include "bitmaps/choice.xbm"
#include "bitmaps/text.xbm"
#include "bitmaps/mtext.xbm"
#include "bitmaps/slider.xbm"
#include "bitmaps/arrow.xbm"
#include "bitmaps/group.xbm"
#include "bitmaps/gauge.xbm"
#include "bitmaps/scroll.xbm"
#include "bitmaps/picture.xbm"
#include "bitmaps/bmpbuttn.xbm"
#endif

/*
 * Object editor tool palette
 *
 */

BEGIN_EVENT_TABLE(EditorToolPalette, wxToolBarSimple)
	EVT_PAINT(EditorToolPalette::OnPaint)
END_EVENT_TABLE()

EditorToolPalette::EditorToolPalette(wxResourceManager *manager, wxFrame *frame, int x, int y, int w, int h,
            long style, int direction, int RowsOrColumns):
  TOOLPALETTECLASS(frame, -1, wxPoint(x, y), wxSize(w, h), style, direction, RowsOrColumns)
{
  currentlySelected = -1;
  resourceManager = manager;
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
/*
  if (MainFrame)
  {
    if (toggled && (toolIndex != PALETTE_ARROW))
      MainFrame->canvas->SetCursor(crossCursor);
    else
      MainFrame->canvas->SetCursor(handCursor);
  }
*/

  return TRUE;
}

void EditorToolPalette::OnMouseEnter(int toolIndex)
{
  if (!resourceManager) return;
  wxFrame *managerFrame = resourceManager->GetEditorFrame();
  
  if (toolIndex > -1)
  {
      switch (toolIndex)
      {
        case PALETTE_FRAME:
          managerFrame->SetStatusText("wxFrame");
          break;
        case PALETTE_DIALOG_BOX:
          managerFrame->SetStatusText("wxDialog");
          break;
        case PALETTE_PANEL:
          managerFrame->SetStatusText("wxPanel");
          break;
#if 0
        case PALETTE_CANVAS:
          managerFrame->SetStatusText("wxCanvas");
          break;
        case PALETTE_TEXT_WINDOW:
          managerFrame->SetStatusText("wxTextWindow");
          break;
#endif
        case PALETTE_BUTTON:
          managerFrame->SetStatusText("wxButton");
          break;
        case PALETTE_MESSAGE:
          managerFrame->SetStatusText("wxStaticText");
          break;
        case PALETTE_TEXT:
          managerFrame->SetStatusText("wxTextCtrl (single-line)");
          break;
        case PALETTE_MULTITEXT:
          managerFrame->SetStatusText("wxTextCtrl (multi-line)");
          break;
        case PALETTE_CHOICE:
          managerFrame->SetStatusText("wxChoice");
          break;
        case PALETTE_CHECKBOX:
          managerFrame->SetStatusText("wxCheckBox");
          break;
        case PALETTE_RADIOBOX:
          managerFrame->SetStatusText("wxRadioBox");
          break;
        case PALETTE_LISTBOX:
          managerFrame->SetStatusText("wxListBox");
          break;
        case PALETTE_SLIDER:
          managerFrame->SetStatusText("wxSlider");
          break;
        case PALETTE_GROUPBOX:
          managerFrame->SetStatusText("wxStaticBox");
          break;
        case PALETTE_GAUGE:
          managerFrame->SetStatusText("wxGauge");
          break;
        case PALETTE_BITMAP_MESSAGE:
          managerFrame->SetStatusText("wxStaticBitmap");
          break;
        case PALETTE_BITMAP_BUTTON:
          managerFrame->SetStatusText("wxBitmapButton");
          break;
        case PALETTE_SCROLLBAR:
          managerFrame->SetStatusText("wxScrollBar");
          break;
        case PALETTE_ARROW:
          managerFrame->SetStatusText("Pointer");
          break;
      }
  }
  else managerFrame->SetStatusText("");
}

void EditorToolPalette::OnPaint(wxPaintEvent& event)
{
  TOOLPALETTECLASS::OnPaint(event);

  wxPaintDC dc(this);

  int w, h;
  GetSize(&w, &h);
  dc.SetPen(wxBLACK_PEN);
  dc.SetBrush(wxTRANSPARENT_BRUSH);
  dc.DrawLine(0, h-1, w, h-1);
}

EditorToolPalette *wxResourceManager::OnCreatePalette(wxFrame *parent)
{
  // Load palette bitmaps
#ifdef __WINDOWS__
  wxBitmap PaletteMessageBitmap("MESSAGETOOL");
  wxBitmap PaletteButtonBitmap("BUTTONTOOL");
  wxBitmap PaletteCheckBoxBitmap("CHECKBOXTOOL");
  wxBitmap PaletteListBoxBitmap("LISTBOXTOOL");
  wxBitmap PaletteRadioBoxBitmap("RADIOBOXTOOL");
  wxBitmap PaletteChoiceBitmap("CHOICETOOL");
  wxBitmap PaletteTextBitmap("TEXTTOOL");
  wxBitmap PaletteMultiTextBitmap("MULTITEXTTOOL");
  wxBitmap PaletteSliderBitmap("SLIDERTOOL");
  wxBitmap PaletteArrowBitmap("ARROWTOOL");
  wxBitmap PaletteGroupBitmap("GROUPTOOL");
  wxBitmap PaletteGaugeBitmap("GAUGETOOL");
  wxBitmap PalettePictureBitmap("PICTURETOOL");
  wxBitmap PaletteBitmapButtonBitmap("BMPBUTTONTOOL");
  wxBitmap PaletteScrollbarBitmap("SCROLLBARTOOL");
#endif
#ifdef __X__
  wxBitmap PaletteMessageBitmap(message_bits, message_width, message_height);
  wxBitmap PaletteButtonBitmap(button_bits, button_width, button_height);
  wxBitmap PaletteCheckBoxBitmap(check_bits, check_width, check_height);
  wxBitmap PaletteListBoxBitmap(listbox_bits, listbox_width, listbox_height);
  wxBitmap PaletteRadioBoxBitmap(radio_bits, radio_width, radio_height);
  wxBitmap PaletteChoiceBitmap(choice_bits, choice_width, choice_height);
  wxBitmap PaletteTextBitmap(text_bits, text_width, text_height);
  wxBitmap PaletteMultiTextBitmap(mtext_bits, mtext_width, mtext_height);
  wxBitmap PaletteSliderBitmap(slider_bits, slider_width, slider_height);
  wxBitmap PaletteArrowBitmap(arrow_bits, arrow_width, arrow_height);
  wxBitmap PaletteGroupBitmap(group_bits, group_width, group_height);
  wxBitmap PaletteGaugeBitmap(gauge_bits, gauge_width, gauge_height);
  wxBitmap PalettePictureBitmap(picture_bits, picture_width, picture_height);
  wxBitmap PaletteBitmapButtonBitmap(bmpbuttn_bits, bmpbuttn_width, bmpbuttn_height);
  wxBitmap PaletteScrollbarBitmap(scroll_bits, scroll_width, scroll_height);
#endif

  EditorToolPalette *palette = new EditorToolPalette(this, parent, 0, 0, -1, -1, wxNO_BORDER, // wxTB_3DBUTTONS,
                                        wxVERTICAL, 1);
  
  palette->SetMargins(2, 2);
/*
#ifdef __WINDOWS__
  if (palette->IsKindOf(CLASSINFO(wxToolBarMSW)))
    ((wxToolBarMSW *)palette)->SetDefaultSize(22, 22);
#endif
*/

  palette->AddTool(PALETTE_ARROW, PaletteArrowBitmap, (wxBitmap *)NULL, TRUE, 0, -1, NULL, "Pointer");
  palette->AddTool(PALETTE_MESSAGE, PaletteMessageBitmap, (wxBitmap *)NULL, TRUE, 0, -1, NULL, "wxStaticText");
  palette->AddTool(PALETTE_BITMAP_MESSAGE, PalettePictureBitmap, (wxBitmap *)NULL, TRUE, 0, -1, NULL, "wxStaticBitmap");
  palette->AddTool(PALETTE_BUTTON, PaletteButtonBitmap, (wxBitmap *)NULL, TRUE, 0, -1, NULL, "Button");
  palette->AddTool(PALETTE_BITMAP_BUTTON, PaletteBitmapButtonBitmap, (wxBitmap *)NULL, TRUE, 0, -1, NULL, "wxBitmapButton");
  palette->AddTool(PALETTE_CHECKBOX, PaletteCheckBoxBitmap, (wxBitmap *)NULL, TRUE, 0, -1, NULL, "wxCheckBox");
  palette->AddTool(PALETTE_RADIOBOX, PaletteRadioBoxBitmap, (wxBitmap *)NULL, TRUE, 0, -1, NULL, "wxRadioBox");
  palette->AddTool(PALETTE_LISTBOX, PaletteListBoxBitmap, (wxBitmap *)NULL, TRUE, 0, -1, NULL, "wxListBox");
  palette->AddTool(PALETTE_CHOICE, PaletteChoiceBitmap, (wxBitmap *)NULL, TRUE, 0, -1, NULL, "wxChoice");
  palette->AddTool(PALETTE_TEXT, PaletteTextBitmap, (wxBitmap *)NULL, TRUE, 0, -1, NULL, "wxTextCtrl (single-line)");
  palette->AddTool(PALETTE_MULTITEXT, PaletteMultiTextBitmap, (wxBitmap *)NULL, TRUE, 0, -1, NULL, "wxTextCtrl (multi-line)");
  palette->AddTool(PALETTE_SLIDER, PaletteSliderBitmap, (wxBitmap *)NULL, TRUE, 0, -1, NULL, "wxSlider");
  palette->AddTool(PALETTE_GROUPBOX, PaletteGroupBitmap, (wxBitmap *)NULL, TRUE, 0, -1, NULL, "wxStaticBox");
  palette->AddTool(PALETTE_GAUGE, PaletteGaugeBitmap, (wxBitmap *)NULL, TRUE, 0, -1, NULL, "wxGauge");
  palette->AddTool(PALETTE_SCROLLBAR, PaletteScrollbarBitmap, (wxBitmap *)NULL, TRUE, 0, -1, NULL, "wxScrollBar");

  palette->Layout();
  palette->CreateTools();
  
  palette->ToggleTool(PALETTE_ARROW, TRUE);
  palette->currentlySelected = PALETTE_ARROW;
  return palette;
}

