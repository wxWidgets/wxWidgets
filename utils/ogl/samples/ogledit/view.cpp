/////////////////////////////////////////////////////////////////////////////
// Name:        view.cpp
// Purpose:     Implements view functionality in OGLEdit
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

#include <wx/colordlg.h>

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in wx_setup.h!
#endif

#include "ogledit.h"
#include "doc.h"
#include "view.h"
#include "palette.h"

IMPLEMENT_DYNAMIC_CLASS(DiagramView, wxView)

BEGIN_EVENT_TABLE(DiagramView, wxView)
    EVT_MENU(OGLEDIT_CUT, DiagramView::OnCut)
    EVT_MENU(OGLEDIT_CHANGE_BACKGROUND_COLOUR, DiagramView::OnChangeBackgroundColour)
    EVT_MENU(OGLEDIT_EDIT_LABEL, DiagramView::OnEditLabel)
END_EVENT_TABLE()

// What to do when a view is created. Creates actual
// windows for displaying the view.
bool DiagramView::OnCreate(wxDocument *doc, long flags)
{
  frame = GetMainFrame();
  canvas = GetMainFrame()->canvas;
  canvas->view = this;

  SetFrame(frame);
  Activate(TRUE);

  // Initialize the edit menu Undo and Redo items
  doc->GetCommandProcessor()->SetEditMenu(((MyFrame *)frame)->editMenu);
  doc->GetCommandProcessor()->Initialize();

  wxShapeCanvas *shapeCanvas = (wxShapeCanvas *)canvas;
  DiagramDocument *diagramDoc = (DiagramDocument *)doc;
  shapeCanvas->SetDiagram(diagramDoc->GetDiagram());
  diagramDoc->GetDiagram()->SetCanvas(shapeCanvas);

  return TRUE;
}

// Sneakily gets used for default print/preview
// as well as drawing on the screen.
void DiagramView::OnDraw(wxDC *dc)
{
}

void DiagramView::OnUpdate(wxView *sender, wxObject *hint)
{
  if (canvas)
    canvas->Refresh();
}

// Clean up windows used for displaying the view.
bool DiagramView::OnClose(bool deleteWindow)
{
  if (!GetDocument()->Close())
    return FALSE;

  DiagramDocument *diagramDoc = (DiagramDocument *)GetDocument();
  diagramDoc->GetDiagram()->SetCanvas(NULL);

  canvas->Clear();
  canvas->SetDiagram(NULL);
  canvas->view = NULL;
  canvas = NULL;

  wxString s = wxTheApp->GetAppName();
  if (frame)
    frame->SetTitle(s);

  SetFrame(NULL);

  Activate(FALSE);
  
  return TRUE;
}

wxShape *DiagramView::FindSelectedShape(void)
{
  DiagramDocument *doc = (DiagramDocument *)GetDocument();
  wxShape *theShape = NULL;
  wxNode *node = doc->GetDiagram()->GetShapeList()->First();
  while (node)
  {
    wxShape *eachShape = (wxShape *)node->Data();
    if ((eachShape->GetParent() == NULL) && eachShape->Selected())
    {
      theShape = eachShape;
      node = NULL;
    }
    else node = node->Next();
  }
  return theShape;
}

void DiagramView::OnCut(wxCommandEvent& event)
{
  DiagramDocument *doc = (DiagramDocument *)GetDocument();

  wxShape *theShape = FindSelectedShape();
  if (theShape)
    doc->GetCommandProcessor()->Submit(new DiagramCommand("Cut", OGLEDIT_CUT, doc, NULL, 0.0, 0.0, TRUE, theShape));
}

void DiagramView::OnChangeBackgroundColour(wxCommandEvent& event)
{
      DiagramDocument *doc = (DiagramDocument *)GetDocument();

      wxShape *theShape = FindSelectedShape();
      if (theShape)
      {
        wxColourData data;
        data.SetChooseFull(TRUE);
        data.SetColour(theShape->GetBrush()->GetColour());

        wxColourDialog *dialog = new wxColourDialog(frame, &data);
        wxBrush *theBrush = NULL;
        if (dialog->ShowModal() == wxID_OK)
        {
          wxColourData retData = dialog->GetColourData();
          wxColour col = retData.GetColour();
          theBrush = wxTheBrushList->FindOrCreateBrush(col, wxSOLID);
        }
        dialog->Close();

        if (theBrush)
          doc->GetCommandProcessor()->Submit(new DiagramCommand("Change colour", OGLEDIT_CHANGE_BACKGROUND_COLOUR, doc,
            theBrush, theShape));
      }
}

void DiagramView::OnEditLabel(wxCommandEvent& event)
{
      wxShape *theShape = FindSelectedShape();
      if (theShape)
      {
        wxString newLabel = wxGetTextFromUser("Enter new label", "Shape Label", ((MyEvtHandler *)theShape->GetEventHandler())->label);
        GetDocument()->GetCommandProcessor()->Submit(new DiagramCommand("Edit label", OGLEDIT_EDIT_LABEL, (DiagramDocument*) GetDocument(), newLabel, theShape));
      }
}

/*
 * Window implementations
 */

BEGIN_EVENT_TABLE(MyCanvas, wxShapeCanvas)
    EVT_MOUSE_EVENTS(MyCanvas::OnMouseEvent)
    EVT_PAINT(MyCanvas::OnPaint)
END_EVENT_TABLE()

// Define a constructor for my canvas
MyCanvas::MyCanvas(wxView *v, wxWindow *parent, wxWindowID id, const wxPoint& pos,
    const wxSize& size, long style):
 wxShapeCanvas(parent, id, pos, size, style)
{
  view = v;
}

MyCanvas::~MyCanvas(void)
{
}

void MyCanvas::OnLeftClick(double x, double y, int keys)
{
  EditorToolPalette *palette = wxGetApp().frame->palette;
  wxClassInfo *info = NULL;
  switch (palette->currentlySelected)
  {
    case PALETTE_TOOL1:
    {
      info = CLASSINFO(wxRectangleShape);
      break;
    }
    case PALETTE_TOOL2:
    {
      info = CLASSINFO(wxRoundedRectangleShape);
      break;
    }
    case PALETTE_TOOL3:
    {
      info = CLASSINFO(wxEllipseShape);
      break;
    }
    case PALETTE_TOOL4:
    {
      info = CLASSINFO(wxDiamondShape);
      break;
    }
    default:
      break;
  }
  if (info)
  {
    view->GetDocument()->GetCommandProcessor()->Submit(new DiagramCommand(info->GetClassName(), OGLEDIT_ADD_SHAPE, (DiagramDocument *)view->GetDocument(), info,
      x, y));
  }
}

void MyCanvas::OnRightClick(double x, double y, int keys)
{
}

void MyCanvas::OnDragLeft(bool draw, double x, double y, int keys)
{
}

void MyCanvas::OnBeginDragLeft(double x, double y, int keys)
{
}

void MyCanvas::OnEndDragLeft(double x, double y, int keys)
{
}

void MyCanvas::OnDragRight(bool draw, double x, double y, int keys)
{
}

void MyCanvas::OnBeginDragRight(double x, double y, int keys)
{
}

void MyCanvas::OnEndDragRight(double x, double y, int keys)
{
}

void MyCanvas::OnMouseEvent(wxMouseEvent& event)
{
    wxShapeCanvas::OnMouseEvent(event);
}

void MyCanvas::OnPaint(wxPaintEvent& event)
{
//  if (GetDiagram())
    wxShapeCanvas::OnPaint(event);
}
