/////////////////////////////////////////////////////////////////////////////
// Name:        doc.cpp
// Purpose:     Implements document functionality in OGLEdit
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

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in wx_setup.h!
#endif

#include "ogledit.h"
#include "doc.h"
#include <wx/wxexpr.h>
#include "view.h"

IMPLEMENT_DYNAMIC_CLASS(DiagramDocument, wxDocument)

DiagramDocument::DiagramDocument(void)
{
}

DiagramDocument::~DiagramDocument(void)
{
}

bool DiagramDocument::OnCloseDocument(void)
{
  diagram.DeleteAllShapes();
  return TRUE;
}

ostream& DiagramDocument::SaveObject(ostream& stream)
{
  wxDocument::SaveObject(stream);
  
  char buf[400];
  (void) wxGetTempFileName("diag", buf);

  diagram.SaveFile(buf);
  wxTransferFileToStream(buf, stream);

  wxRemoveFile(buf);
  
  return stream;
}

istream& DiagramDocument::LoadObject(istream& stream)
{
  wxDocument::LoadObject(stream);

  char buf[400];
  (void) wxGetTempFileName("diag", buf);

  wxTransferStreamToFile(stream, buf);

  diagram.DeleteAllShapes();
  diagram.LoadFile(buf);
  wxRemoveFile(buf);

  return stream;
}

/*
 * Implementation of drawing command
 */

DiagramCommand::DiagramCommand(char *name, int command, DiagramDocument *ddoc, wxClassInfo *info, double xx, double yy,
  bool sel, wxShape *theShape, wxShape *fs, wxShape *ts):
  wxCommand(TRUE, name)
{
  doc = ddoc;
  cmd = command;
  shape = theShape;
  fromShape = fs;
  toShape = ts;
  shapeInfo = info;
  shapeBrush = NULL;
  shapePen = NULL;
  x = xx;
  y = yy;
  selected = sel;
  deleteShape = FALSE;
}

DiagramCommand::DiagramCommand(char *name, int command, DiagramDocument *ddoc, wxBrush *backgroundColour, wxShape *theShape):
  wxCommand(TRUE, name)
{
  doc = ddoc;
  cmd = command;
  shape = theShape;
  fromShape = NULL;
  toShape = NULL;
  shapeInfo = NULL;
  x = 0.0;
  y = 0.0;
  selected = FALSE;
  deleteShape = FALSE;
  shapeBrush = backgroundColour;
  shapePen = NULL;
}

DiagramCommand::DiagramCommand(char *name, int command, DiagramDocument *ddoc, const wxString& lab, wxShape *theShape):
  wxCommand(TRUE, name)
{
  doc = ddoc;
  cmd = command;
  shape = theShape;
  fromShape = NULL;
  toShape = NULL;
  shapeInfo = NULL;
  x = 0.0;
  y = 0.0;
  selected = FALSE;
  deleteShape = FALSE;
  shapeBrush = NULL;
  shapePen = NULL;
  shapeLabel = lab;
}

DiagramCommand::~DiagramCommand(void)
{
  if (shape && deleteShape)
  {
    shape->SetCanvas(NULL);
    delete shape;
  }
}

bool DiagramCommand::Do(void)
{
  switch (cmd)
  {
    case OGLEDIT_CUT:
    {
      if (shape)
      {
        deleteShape = TRUE;
        
        shape->Select(FALSE);
        
        // Generate commands to explicitly remove each connected line.
        RemoveLines(shape);
        
        doc->GetDiagram()->RemoveShape(shape);
        if (shape->IsKindOf(CLASSINFO(wxLineShape)))
        {
          wxLineShape *lineShape = (wxLineShape *)shape;
          fromShape = lineShape->GetFrom();
          toShape = lineShape->GetTo();
        }
        shape->Unlink();
        
        doc->Modify(TRUE);
        doc->UpdateAllViews();
      }

      break;
    }
    case OGLEDIT_ADD_SHAPE:
    {
      wxShape *theShape = NULL;
      if (shape)
        theShape = shape; // Saved from undoing the shape
      else
      {
        theShape = (wxShape *)shapeInfo->CreateObject();
        theShape->AssignNewIds();
        theShape->SetEventHandler(new MyEvtHandler(theShape, theShape, wxString("")));
        theShape->SetCentreResize(FALSE);
        theShape->SetPen(wxBLACK_PEN);
        theShape->SetBrush(wxCYAN_BRUSH);
      
        theShape->SetSize(60, 60);
      }
      doc->GetDiagram()->AddShape(theShape);
      theShape->Show(TRUE);

      wxClientDC dc(theShape->GetCanvas());
      theShape->GetCanvas()->PrepareDC(dc);

      theShape->Move(dc, x, y);
      
      shape = theShape;
      deleteShape = FALSE;

      doc->Modify(TRUE);
      doc->UpdateAllViews();
      break;
    }
    case OGLEDIT_ADD_LINE:
    {
      wxShape *theShape = NULL;
      if (shape)
        theShape = shape; // Saved from undoing the line
      else
      {
        theShape = (wxShape *)shapeInfo->CreateObject();
        theShape->AssignNewIds();
        theShape->SetEventHandler(new MyEvtHandler(theShape, theShape, wxString("")));
        theShape->SetPen(wxBLACK_PEN);
        theShape->SetBrush(wxRED_BRUSH);

        wxLineShape *lineShape = (wxLineShape *)theShape;

        // Yes, you can have more than 2 control points, in which case
        // it becomes a multi-segment line.
        lineShape->MakeLineControlPoints(2);
        lineShape->AddArrow(ARROW_ARROW, ARROW_POSITION_END, 10.0, 0.0, "Normal arrowhead");
      }
      
      doc->GetDiagram()->AddShape(theShape);
      
      fromShape->AddLine((wxLineShape *)theShape, toShape);
      
      theShape->Show(TRUE);

      wxClientDC dc(theShape->GetCanvas());
      theShape->GetCanvas()->PrepareDC(dc);

      // It won't get drawn properly unless you move both
      // connected images
      fromShape->Move(dc, fromShape->GetX(), fromShape->GetY());
      toShape->Move(dc, toShape->GetX(), toShape->GetY());
      
      shape = theShape;
      deleteShape = FALSE;

      doc->Modify(TRUE);
      doc->UpdateAllViews();
      break;
    }
    case OGLEDIT_CHANGE_BACKGROUND_COLOUR:
    {
      if (shape)
      {
        wxClientDC dc(shape->GetCanvas());
        shape->GetCanvas()->PrepareDC(dc);

        wxBrush *oldBrush = shape->GetBrush();
        shape->SetBrush(shapeBrush);
        shapeBrush = oldBrush;
        shape->Draw(dc);
        
        doc->Modify(TRUE);
        doc->UpdateAllViews();
      }

      break;
    }
    case OGLEDIT_EDIT_LABEL:
    {
      if (shape)
      {
        MyEvtHandler *myHandler = (MyEvtHandler *)shape->GetEventHandler();
        wxString oldLabel(myHandler->label);
        myHandler->label = shapeLabel;
        shapeLabel = oldLabel;

        wxClientDC dc(shape->GetCanvas());
        shape->GetCanvas()->PrepareDC(dc);

        shape->FormatText(dc, (char*) (const char*) myHandler->label);
        shape->Draw(dc);
        
        doc->Modify(TRUE);
        doc->UpdateAllViews();
      }

      break;
    }
  }
  return TRUE;
}

bool DiagramCommand::Undo(void)
{
  switch (cmd)
  {
    case OGLEDIT_CUT:
    {
      if (shape)
      {
        doc->GetDiagram()->AddShape(shape);
        shape->Show(TRUE);

        if (shape->IsKindOf(CLASSINFO(wxLineShape)))
        {
          wxLineShape *lineShape = (wxLineShape *)shape;

          fromShape->AddLine(lineShape, toShape);
        }
        if (selected)
          shape->Select(TRUE);

        deleteShape = FALSE;
      }
      doc->Modify(TRUE);
      doc->UpdateAllViews();
      break;
    }
    case OGLEDIT_ADD_SHAPE:
    case OGLEDIT_ADD_LINE:
    {
      if (shape)
      {
        wxClientDC dc(shape->GetCanvas());
        shape->GetCanvas()->PrepareDC(dc);

        shape->Select(FALSE, &dc);
        doc->GetDiagram()->RemoveShape(shape);
        shape->Unlink();
        deleteShape = TRUE;
      }
      doc->Modify(TRUE);
      doc->UpdateAllViews();
      break;
    }
    case OGLEDIT_CHANGE_BACKGROUND_COLOUR:
    {
      if (shape)
      {
        wxClientDC dc(shape->GetCanvas());
        shape->GetCanvas()->PrepareDC(dc);

        wxBrush *oldBrush = shape->GetBrush();
        shape->SetBrush(shapeBrush);
        shapeBrush = oldBrush;
        shape->Draw(dc);
        
        doc->Modify(TRUE);
        doc->UpdateAllViews();
      }
      break;
    }
    case OGLEDIT_EDIT_LABEL:
    {
      if (shape)
      {
        MyEvtHandler *myHandler = (MyEvtHandler *)shape->GetEventHandler();
        wxString oldLabel(myHandler->label);
        myHandler->label = shapeLabel;
        shapeLabel = oldLabel;

        wxClientDC dc(shape->GetCanvas());
        shape->GetCanvas()->PrepareDC(dc);

        shape->FormatText(dc, (char*) (const char*) myHandler->label);
        shape->Draw(dc);
        
        doc->Modify(TRUE);
        doc->UpdateAllViews();
      }

      break;
    }
  }
  return TRUE;
}

// Remove each individual line connected to a shape by sending a command.
void DiagramCommand::RemoveLines(wxShape *shape)
{
  wxNode *node = shape->GetLines().First();
  while (node)
  {
    wxLineShape *line = (wxLineShape *)node->Data();
    doc->GetCommandProcessor()->Submit(new DiagramCommand("Cut", OGLEDIT_CUT, doc, NULL, 0.0, 0.0, line->Selected(), line));
    
    node = shape->GetLines().First();
  }
}

/*
 * MyEvtHandler: an event handler class for all shapes
 */
 
void MyEvtHandler::OnLeftClick(double x, double y, int keys, int attachment)
{
  wxClientDC dc(GetShape()->GetCanvas());
  GetShape()->GetCanvas()->PrepareDC(dc);

  if (keys == 0)
  {
    // Selection is a concept the library knows about
    if (GetShape()->Selected())
    {
      GetShape()->Select(FALSE, &dc);
      GetShape()->GetCanvas()->Redraw(dc); // Redraw because bits of objects will be are missing
    }
    else
    {
      // Ensure no other shape is selected, to simplify Undo/Redo code
      bool redraw = FALSE;
      wxNode *node = GetShape()->GetCanvas()->GetDiagram()->GetShapeList()->First();
      while (node)
      {
        wxShape *eachShape = (wxShape *)node->Data();
        if (eachShape->GetParent() == NULL)
        {
          if (eachShape->Selected())
          {
            eachShape->Select(FALSE, &dc);
            redraw = TRUE;
          }
        }
        node = node->Next();
      }
      GetShape()->Select(TRUE, &dc);
      if (redraw)
        GetShape()->GetCanvas()->Redraw(dc);
    }
  }
  else if (keys & KEY_CTRL)
  {
    // Do something for CONTROL
  }
  else
  {
    wxGetApp().frame->SetStatusText(label);
  }
}

/*
 * Implement connection of two shapes by right-dragging between them.
 */

void MyEvtHandler::OnBeginDragRight(double x, double y, int keys, int attachment)
{
  // Force attachment to be zero for now. Eventually we can deal with
  // the actual attachment point, e.g. a rectangle side if attachment mode is on.
  attachment = 0;
  
  wxClientDC dc(GetShape()->GetCanvas());
  GetShape()->GetCanvas()->PrepareDC(dc);

  wxPen dottedPen(wxColour(0, 0, 0), 1, wxDOT);
  dc.SetLogicalFunction(wxXOR);
  dc.SetPen(dottedPen);
  double xp, yp;
  GetShape()->GetAttachmentPosition(attachment, &xp, &yp);
  dc.DrawLine(xp, yp, x, y);
  GetShape()->GetCanvas()->CaptureMouse();
}

void MyEvtHandler::OnDragRight(bool draw, double x, double y, int keys, int attachment)
{
  // Force attachment to be zero for now
  attachment = 0;

  wxClientDC dc(GetShape()->GetCanvas());
  GetShape()->GetCanvas()->PrepareDC(dc);

  wxPen dottedPen(wxColour(0, 0, 0), 1, wxDOT);
  dc.SetLogicalFunction(wxXOR);
  dc.SetPen(dottedPen);
  double xp, yp;
  GetShape()->GetAttachmentPosition(attachment, &xp, &yp);
  dc.DrawLine(xp, yp, x, y);
}

void MyEvtHandler::OnEndDragRight(double x, double y, int keys, int attachment)
{
  GetShape()->GetCanvas()->ReleaseMouse();
  MyCanvas *canvas = (MyCanvas *)GetShape()->GetCanvas();

  // Check if we're on an object
  int new_attachment;
  wxShape *otherShape = canvas->FindFirstSensitiveShape(x, y, &new_attachment, OP_DRAG_RIGHT);
  
  if (otherShape && !otherShape->IsKindOf(CLASSINFO(wxLineShape)))
  {
    canvas->view->GetDocument()->GetCommandProcessor()->Submit(
      new DiagramCommand("wxLineShape", OGLEDIT_ADD_LINE, (DiagramDocument *)canvas->view->GetDocument(), CLASSINFO(wxLineShape),
      0.0, 0.0, FALSE, NULL, GetShape(), otherShape));
  }
}

void MyEvtHandler::OnEndSize(double x, double y)
{
  wxClientDC dc(GetShape()->GetCanvas());
  GetShape()->GetCanvas()->PrepareDC(dc);

  GetShape()->FormatText(dc, (char*) (const char*) label);
}

/*
 * Diagram
 */
 
bool MyDiagram::OnShapeSave(wxExprDatabase& db, wxShape& shape, wxExpr& expr)
{
  wxDiagram::OnShapeSave(db, shape, expr);
  MyEvtHandler *handler = (MyEvtHandler *)shape.GetEventHandler();
  expr.AddAttributeValueString("label", handler->label);
  return TRUE;
}

bool MyDiagram::OnShapeLoad(wxExprDatabase& db, wxShape& shape, wxExpr& expr)
{
  wxDiagram::OnShapeLoad(db, shape, expr);
  char *label = NULL;
  expr.AssignAttributeValue("label", &label);
  MyEvtHandler *handler = new MyEvtHandler(&shape, &shape, wxString(label));
  shape.SetEventHandler(handler);
  
  if (label)
    delete[] label;
  return TRUE;
}

/*
 * New shapes
 */

IMPLEMENT_DYNAMIC_CLASS(wxRoundedRectangleShape, wxRectangleShape)

wxRoundedRectangleShape::wxRoundedRectangleShape(double w, double h):
 wxRectangleShape(w, h)
{
  // 0.3 of the smaller rectangle dimension
  SetCornerRadius((double) -0.3);
}

IMPLEMENT_DYNAMIC_CLASS(wxDiamondShape, wxPolygonShape)

wxDiamondShape::wxDiamondShape(double w, double h):
  wxPolygonShape()
{
  // wxPolygonShape::SetSize relies on the shape having non-zero
  // size initially.
  if (w == 0.0)
    w = 60.0;
  if (h == 0.0)
    h = 60.0;
    
  wxList *thePoints = new wxList;
  wxRealPoint *point = new wxRealPoint(0.0, (-h/2.0));
  thePoints->Append((wxObject*) point);

  point = new wxRealPoint((w/2.0), 0.0);
  thePoints->Append((wxObject*) point);

  point = new wxRealPoint(0.0, (h/2.0));
  thePoints->Append((wxObject*) point);

  point = new wxRealPoint((-w/2.0), 0.0);
  thePoints->Append((wxObject*) point);

  Create(thePoints);
}
