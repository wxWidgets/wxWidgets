/////////////////////////////////////////////////////////////////////////////
// Name:        colrdlgg.cpp
// Purpose:     Choice dialogs
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "colrdlgg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <stdio.h>
#include <stdio.h>
#include "wx/utils.h"
#include "wx/intl.h"
#include "wx/dialog.h"
#include "wx/listbox.h"
#include "wx/button.h"
#include "wx/stattext.h"
#include "wx/layout.h"
#include "wx/dcclient.h"
#include "wx/slider.h"
#include "wx/sizer.h"
#endif

#if wxUSE_STATLINE
    #include "wx/statline.h"
#endif

#include "wx/generic/colrdlgg.h"

IMPLEMENT_DYNAMIC_CLASS(wxGenericColourDialog, wxDialog)

BEGIN_EVENT_TABLE(wxGenericColourDialog, wxDialog)
    EVT_BUTTON(wxID_ADD_CUSTOM, wxGenericColourDialog::OnAddCustom)
    EVT_SLIDER(wxID_RED_SLIDER, wxGenericColourDialog::OnRedSlider)
    EVT_SLIDER(wxID_GREEN_SLIDER, wxGenericColourDialog::OnGreenSlider)
    EVT_SLIDER(wxID_BLUE_SLIDER, wxGenericColourDialog::OnBlueSlider)
    EVT_PAINT(wxGenericColourDialog::OnPaint)
    EVT_MOUSE_EVENTS(wxGenericColourDialog::OnMouseEvent)
    EVT_CLOSE(wxGenericColourDialog::OnCloseWindow)
END_EVENT_TABLE()


/*
 * Generic wxColourDialog
 */

#define NUM_COLS 48
static wxString wxColourDialogNames[NUM_COLS]={wxT("ORANGE"),
            wxT("GOLDENROD"),
            wxT("WHEAT"),
            wxT("SPRING GREEN"),
            wxT("SKY BLUE"),
            wxT("SLATE BLUE"),
            wxT("MEDIUM VIOLET RED"),
            wxT("PURPLE"),

            wxT("RED"),
            wxT("YELLOW"),
            wxT("MEDIUM SPRING GREEN"),
            wxT("PALE GREEN"),
            wxT("CYAN"),
            wxT("LIGHT STEEL BLUE"),
            wxT("ORCHID"),
            wxT("LIGHT MAGENTA"),

            wxT("BROWN"),
            wxT("YELLOW"),
            wxT("GREEN"),
            wxT("CADET BLUE"),
            wxT("MEDIUM BLUE"),
            wxT("MAGENTA"),
            wxT("MAROON"),
            wxT("ORANGE RED"),

            wxT("FIREBRICK"),
            wxT("CORAL"),
            wxT("FOREST GREEN"),
            wxT("AQUAMARINE"),
            wxT("BLUE"),
            wxT("NAVY"),
            wxT("THISTLE"),
            wxT("MEDIUM VIOLET RED"),

            wxT("INDIAN RED"),
            wxT("GOLD"),
            wxT("MEDIUM SEA GREEN"),
            wxT("MEDIUM BLUE"),
            wxT("MIDNIGHT BLUE"),
            wxT("GREY"),
            wxT("PURPLE"),
            wxT("KHAKI"),

            wxT("BLACK"),
            wxT("MEDIUM FOREST GREEN"),
            wxT("KHAKI"),
            wxT("DARK GREY"),
            wxT("SEA GREEN"),
            wxT("LIGHT GREY"),
            wxT("MEDIUM SLATE BLUE"),
            wxT("WHITE")
            };

wxGenericColourDialog::wxGenericColourDialog()
{
  dialogParent = NULL;
  whichKind = 1;
  colourSelection = 0;
}

wxGenericColourDialog::wxGenericColourDialog(wxWindow *parent,
                                             wxColourData *data)
{
  whichKind = 1;
  colourSelection = 0;
  Create(parent, data);
}

wxGenericColourDialog::~wxGenericColourDialog()
{
}

void wxGenericColourDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
  EndModal(wxID_CANCEL);
}

bool wxGenericColourDialog::Create(wxWindow *parent, wxColourData *data)
{
    if ( !wxDialog::Create(parent, -1, wxT("Colour"),
                           wxPoint(0, 0), wxSize(900, 900),
                           wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL) )
        return FALSE;

    dialogParent = parent;

    if (data)
        colourData = *data;

    InitializeColours();
    CalculateMeasurements();
    CreateWidgets();

    return TRUE;
}

int wxGenericColourDialog::ShowModal()
{
  return wxDialog::ShowModal();
}


// Internal functions
void wxGenericColourDialog::OnMouseEvent(wxMouseEvent& event)
{
  if (event.ButtonDown(1))
  {
    int x = (int)event.GetX();
    int y = (int)event.GetY();

    if ((x >= standardColoursRect.x && x <= (standardColoursRect.x + standardColoursRect.width)) &&
        (y >= standardColoursRect.y && y <= (standardColoursRect.y + standardColoursRect.height)))
    {
      int selX = (int)(x - standardColoursRect.x)/(smallRectangleSize.x + gridSpacing);
      int selY = (int)(y - standardColoursRect.y)/(smallRectangleSize.y + gridSpacing);
      int ptr = (int)(selX + selY*8);
      OnBasicColourClick(ptr);
    }
    else if ((x >= customColoursRect.x && x <= (customColoursRect.x + customColoursRect.width)) &&
        (y >= customColoursRect.y && y <= (customColoursRect.y + customColoursRect.height)))
    {
      int selX = (int)(x - customColoursRect.x)/(smallRectangleSize.x + gridSpacing);
      int selY = (int)(y - customColoursRect.y)/(smallRectangleSize.y + gridSpacing);
      int ptr = (int)(selX + selY*8);
      OnCustomColourClick(ptr);
    }
  }
}

void wxGenericColourDialog::OnPaint(wxPaintEvent& event)
{
#if !defined(__WXMOTIF__) && !defined(__WXMAC__)
  wxDialog::OnPaint(event);
#endif

  wxPaintDC dc(this);

  PaintBasicColours(dc);
  PaintCustomColours(dc);
  PaintCustomColour(dc);
  PaintHighlight(dc, TRUE);
}

void wxGenericColourDialog::CalculateMeasurements()
{
  smallRectangleSize.x = 18;
  smallRectangleSize.y = 14;
  customRectangleSize.x = 40;
  customRectangleSize.y = 40;

  gridSpacing = 6;
  sectionSpacing = 15;

  standardColoursRect.x = 10;
  standardColoursRect.y = 15;
  standardColoursRect.width = (8*smallRectangleSize.x) + (7*gridSpacing);
  standardColoursRect.height = (6*smallRectangleSize.y) + (5*gridSpacing);

  customColoursRect.x = standardColoursRect.x;
  customColoursRect.y = standardColoursRect.y + standardColoursRect.height  + 20;
  customColoursRect.width = (8*smallRectangleSize.x) + (7*gridSpacing);
  customColoursRect.height = (2*smallRectangleSize.y) + (1*gridSpacing);

  singleCustomColourRect.x = customColoursRect.width + customColoursRect.x + sectionSpacing;
  singleCustomColourRect.y = 80;
  singleCustomColourRect.width = customRectangleSize.x;
  singleCustomColourRect.height = customRectangleSize.y;

  okButtonX = 10;
  customButtonX = singleCustomColourRect.x ;
  buttonY = customColoursRect.y + customColoursRect.height + 10;
}

void wxGenericColourDialog::CreateWidgets()
{
    wxBeginBusyCursor();

    int sliderX = singleCustomColourRect.x + singleCustomColourRect.width + sectionSpacing;
#if defined(__WXMOTIF__)
    int sliderSpacing = 65;
    int sliderHeight = 160;
#else
    int sliderSpacing = 45;
    int sliderHeight = 160;
#endif

    redSlider = new wxSlider(this, wxID_RED_SLIDER, 0, 0, 255,
        wxPoint(sliderX, 10), wxSize(-1, sliderHeight), wxVERTICAL|wxSL_LABELS);
    greenSlider = new wxSlider(this, wxID_GREEN_SLIDER, 0, 0, 255,
        wxPoint(sliderX + sliderSpacing, 10), wxSize(-1, sliderHeight), wxVERTICAL|wxSL_LABELS);
    blueSlider = new wxSlider(this, wxID_BLUE_SLIDER, 0, 0, 255,
        wxPoint(sliderX + 2*sliderSpacing, 10), wxSize(-1, sliderHeight), wxVERTICAL|wxSL_LABELS);

    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

    // 1) space for explicitly layouted controls
    topsizer->Add( sliderX + 3*sliderSpacing, sliderHeight+25 );
    
#if wxUSE_STATLINE
    // 2) static line
    topsizer->Add( new wxStaticLine( this, -1 ), 0, wxEXPAND | wxLEFT|wxRIGHT|wxTOP, 10 );
#endif

    // 3) buttons
    wxSizer *buttonsizer = CreateButtonSizer( wxOK|wxCANCEL );
    buttonsizer->Add( new wxButton(this, wxID_ADD_CUSTOM, _("Add to custom colours") ), 0, wxLEFT|wxRIGHT, 10 );
    topsizer->Add( buttonsizer, 0, wxCENTRE | wxALL, 10 );

    SetAutoLayout( TRUE );
    SetSizer( topsizer );
    
    topsizer->SetSizeHints( this );
    topsizer->Fit( this );

    Centre( wxBOTH );

    wxEndBusyCursor();
}

void wxGenericColourDialog::InitializeColours(void)
{
  int i;
  for (i = 0; i < 48; i++)
  {
    wxColour *col = wxTheColourDatabase->FindColour(wxColourDialogNames[i]);
    if (col)
      standardColours[i].Set(col->Red(), col->Green(), col->Blue());
    else
      standardColours[i].Set(0, 0, 0);
  }

  for (i = 0; i < 16; i++)
    customColours[i] =
/*
#ifndef __VMS__
     (wxColour&)
#endif
*/
       colourData.GetCustomColour(i);

  singleCustomColour.Set(0, 0, 0);
}

void wxGenericColourDialog::PaintBasicColours(wxDC& dc)
{
  dc.BeginDrawing();

  int i;
  for (i = 0; i < 6; i++)
  {
    int j;
    for (j = 0; j < 8; j++)
    {
      int ptr = i*8 + j;

      int x = (j*(smallRectangleSize.x+gridSpacing) + standardColoursRect.x);
      int y = (i*(smallRectangleSize.y+gridSpacing) + standardColoursRect.y);

      dc.SetPen(*wxBLACK_PEN);
      wxBrush brush(standardColours[ptr], wxSOLID);
      dc.SetBrush(brush);

      dc.DrawRectangle( x, y, smallRectangleSize.x, smallRectangleSize.y);
    }
  }
  dc.EndDrawing();
}

void wxGenericColourDialog::PaintCustomColours(wxDC& dc)
{
  dc.BeginDrawing();

  int i;
  for (i = 0; i < 2; i++)
  {
    int j;
    for (j = 0; j < 8; j++)
    {
      int ptr = i*8 + j;

      int x = (j*(smallRectangleSize.x+gridSpacing)) + customColoursRect.x;
      int y = (i*(smallRectangleSize.y+gridSpacing)) + customColoursRect.y;

      dc.SetPen(*wxBLACK_PEN);

      wxBrush brush(customColours[ptr], wxSOLID);
      dc.SetBrush(brush);

      dc.DrawRectangle( x, y, smallRectangleSize.x, smallRectangleSize.y);
    }
  }
  dc.EndDrawing();
}

void wxGenericColourDialog::PaintHighlight(wxDC& dc, bool draw)
{
  dc.BeginDrawing();

  // Number of pixels bigger than the standard rectangle size
  // for drawing a highlight
  int deltaX = 2;
  int deltaY = 2;

  if (whichKind == 1)
  {
    // Standard colours
    int y = (int)(colourSelection / 8);
    int x = (int)(colourSelection - (y*8));

    x = (x*(smallRectangleSize.x + gridSpacing) + standardColoursRect.x) - deltaX;
    y = (y*(smallRectangleSize.y + gridSpacing) + standardColoursRect.y) - deltaY;

    if (draw)
      dc.SetPen(*wxBLACK_PEN);
    else
      dc.SetPen(*wxLIGHT_GREY_PEN);

    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle( x, y, (smallRectangleSize.x + (2*deltaX)), (smallRectangleSize.y + (2*deltaY)));
  }
  else
  {
    // User-defined colours
    int y = (int)(colourSelection / 8);
    int x = (int)(colourSelection - (y*8));

    x = (x*(smallRectangleSize.x + gridSpacing) + customColoursRect.x) - deltaX;
    y = (y*(smallRectangleSize.y + gridSpacing) + customColoursRect.y) - deltaY;

    if (draw)
      dc.SetPen(*wxBLACK_PEN);
    else
      dc.SetPen(*wxLIGHT_GREY_PEN);

    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle( x, y, (smallRectangleSize.x + (2*deltaX)), (smallRectangleSize.y + (2*deltaY)));
  }

  dc.EndDrawing();
}

void wxGenericColourDialog::PaintCustomColour(wxDC& dc)
{
  dc.BeginDrawing();

  dc.SetPen(*wxBLACK_PEN);

  wxBrush *brush = new wxBrush(singleCustomColour, wxSOLID);
  dc.SetBrush(*brush);

  dc.DrawRectangle( singleCustomColourRect.x, singleCustomColourRect.y,
                    customRectangleSize.x, customRectangleSize.y);

  dc.SetBrush(wxNullBrush);
  delete brush;

  dc.EndDrawing();
}

void wxGenericColourDialog::OnBasicColourClick(int which)
{
  wxClientDC dc(this);

  PaintHighlight(dc, FALSE);
  whichKind = 1;
  colourSelection = which;
  colourData.SetColour(standardColours[colourSelection]);

  PaintHighlight(dc, TRUE);
}

void wxGenericColourDialog::OnCustomColourClick(int which)
{
  wxClientDC dc(this);
  PaintHighlight(dc, FALSE);
  whichKind = 2;
  colourSelection = which;
  colourData.SetColour(customColours[colourSelection]);

  PaintHighlight(dc, TRUE);
}

/*
void wxGenericColourDialog::OnOk(void)
{
  Show(FALSE);
}

void wxGenericColourDialog::OnCancel(void)
{
  colourDialogCancelled = TRUE;
  Show(FALSE);
}
*/

void wxGenericColourDialog::OnAddCustom(wxCommandEvent& WXUNUSED(event))
{
  wxClientDC dc(this);
  if (whichKind != 2)
  {
    PaintHighlight(dc, FALSE);
    whichKind = 2;
    colourSelection = 0;
    PaintHighlight(dc, TRUE);
  }

  customColours[colourSelection].Set(singleCustomColour.Red(), singleCustomColour.Green(), singleCustomColour.Blue());
  colourData.SetColour(customColours[colourSelection]);
  colourData.SetCustomColour(colourSelection, customColours[colourSelection]);

  PaintCustomColours(dc);
}

void wxGenericColourDialog::OnRedSlider(wxCommandEvent& WXUNUSED(event))
{
  if (!redSlider)
    return;

  wxClientDC dc(this);
  singleCustomColour.Set(redSlider->GetValue(), singleCustomColour.Green(), singleCustomColour.Blue());
  PaintCustomColour(dc);
}

void wxGenericColourDialog::OnGreenSlider(wxCommandEvent& WXUNUSED(event))
{
  if (!greenSlider)
    return;

  wxClientDC dc(this);
  singleCustomColour.Set(singleCustomColour.Red(), greenSlider->GetValue(), singleCustomColour.Blue());
  PaintCustomColour(dc);
}

void wxGenericColourDialog::OnBlueSlider(wxCommandEvent& WXUNUSED(event))
{
  if (!blueSlider)
    return;

  wxClientDC dc(this);
  singleCustomColour.Set(singleCustomColour.Red(), singleCustomColour.Green(), blueSlider->GetValue());
  PaintCustomColour(dc);
}


