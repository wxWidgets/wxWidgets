/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/colrdlgg.h
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
class WXDLLEXPORT wxGenericColourDialog : public wxDialog
{
public:
    wxGenericColourDialog();
    wxGenericColourDialog(wxWindow *parent,
                          wxColourData *data = (wxColourData *) NULL);
    virtual ~wxGenericColourDialog();

    bool Create(wxWindow *parent, wxColourData *data = (wxColourData *) NULL);

    wxColourData &GetColourData() { return colourData; }

    virtual int ShowModal();

    // Internal functions
    void OnMouseEvent(wxMouseEvent& event);
    void OnPaint(wxPaintEvent& event);

    virtual void CalculateMeasurements();
    virtual void CreateWidgets();
    virtual void InitializeColours();

    virtual void PaintBasicColours(wxDC& dc);
    virtual void PaintCustomColours(wxDC& dc);
    virtual void PaintCustomColour(wxDC& dc);
    virtual void PaintHighlight(wxDC& dc, bool draw);

    virtual void OnBasicColourClick(int which);
    virtual void OnCustomColourClick(int which);

    void OnAddCustom(wxCommandEvent& event);

    void OnRedSlider(wxCommandEvent& event);
    void OnGreenSlider(wxCommandEvent& event);
    void OnBlueSlider(wxCommandEvent& event);

    void OnCloseWindow(wxCloseEvent& event);

protected:
    wxColourData colourData;
    wxWindow *dialogParent;

    // Area reserved for grids of colours
    wxRect standardColoursRect;
    wxRect customColoursRect;
    wxRect singleCustomColourRect;

    // Size of each colour rectangle
    wxPoint smallRectangleSize;

    // For single customizable colour
    wxPoint customRectangleSize;

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

    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxGenericColourDialog)
};

/* This shouldn't be necessary, we have a #define in wx/colordlg.h.
#ifdef __WXGTK__
typedef wxGenericColourDialog wxColourDialog;
#endif
*/

#endif
