/////////////////////////////////////////////////////////////////////////////
// Name:        colrdlgg.h
// Purpose:     wxGenericColourDialog
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __COLORDLGH_G__
#define __COLORDLGH_G__

#ifdef __GNUG__
#pragma interface "colrdlgg.h"
#endif

#include "wx/setup.h"
#include "wx/gdicmn.h"
#include "wx/dialog.h"
#include "wx/cmndata.h"

#define wxID_ADD_CUSTOM     3000
#define wxID_RED_SLIDER     3001
#define wxID_GREEN_SLIDER   3002
#define wxID_BLUE_SLIDER    3003

class WXDLLEXPORT wxSlider;
class WXDLLEXPORT wxGenericColourDialog: public wxDialog
{
 DECLARE_DYNAMIC_CLASS(wxGenericColourDialog)
 protected:
  wxColourData colourData;
  wxWindow *dialogParent;

  // Area reserved for grids of colours
  wxRectangle standardColoursRect;
  wxRectangle customColoursRect;
  wxRectangle singleCustomColourRect;

  // Size of each colour rectangle
  wxIntPoint smallRectangleSize;

  // For single customizable colour
  wxIntPoint customRectangleSize;

  // Grid spacing (between rectangles)
  int gridSpacing;

  // Section spacing (between left and right halves of dialog box)
  int sectionSpacing;

  // 48 'standard' colours
  wxColour standardColours[48];

  // 16 'custom' colours
  wxColour customColours[16];

  // One single custom colour (use sliders)
  wxColour singleCustomColour;

  // Which colour is selected? An index into one of the two areas.
  int colourSelection;
  int whichKind; // 1 for standard colours, 2 for custom colours,

  wxSlider *redSlider;
  wxSlider *greenSlider;
  wxSlider *blueSlider;

  int buttonY;

  int okButtonX;
  int customButtonX;

//  static bool colourDialogCancelled;
 public:
  wxGenericColourDialog(void);
  wxGenericColourDialog(wxWindow *parent, wxColourData *data = NULL);
  ~wxGenericColourDialog(void);

  bool Create(wxWindow *parent, wxColourData *data = NULL);

  int ShowModal(void);
  wxColourData GetColourData(void) { return colourData; }

  // Internal functions
  void OnMouseEvent(wxMouseEvent& event);
  void OnPaint(wxPaintEvent& event);

  bool OnClose(void);

  virtual void CalculateMeasurements(void);
  virtual void CreateWidgets(void);
  virtual void InitializeColours(void);
  
  virtual void PaintBasicColours(wxDC& dc);
  virtual void PaintCustomColours(wxDC& dc);
  virtual void PaintCustomColour(wxDC& dc);
  virtual void PaintHighlight(wxDC& dc, bool draw);

  virtual void OnBasicColourClick(int which);
  virtual void OnCustomColourClick(int which);

/*
  virtual void OnOk(void);
  virtual void OnCancel(void);
  virtual void OnAddCustom(void);
*/
  void OnAddCustom(wxCommandEvent& event);

  void OnRedSlider(wxCommandEvent& event);
  void OnGreenSlider(wxCommandEvent& event);
  void OnBlueSlider(wxCommandEvent& event);

DECLARE_EVENT_TABLE()
};

#ifdef __GTK__
typedef wxGenericColourDialog wxColourDialog;
#endif

#endif
