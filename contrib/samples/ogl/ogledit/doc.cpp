/////////////////////////////////////////////////////////////////////////////
// Name:        contrib/samples/ogl/ogledit/doc.cpp
// Purpose:     Implements document functionality in OGLEdit
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/ioswrap.h"

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in wx_setup.h!
#endif

#include "ogledit.h"
#include "doc.h"
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
    return true;
}

#if wxUSE_STD_IOSTREAM
wxSTD ostream& DiagramDocument::SaveObject(wxSTD ostream& stream)
{
#if wxUSE_PROLOGIO

    wxDocument::SaveObject(stream);

    char buf[400];
    (void) wxGetTempFileName("diag", buf);

    diagram.SaveFile(buf);
    wxTransferFileToStream(buf, stream);

    wxRemoveFile(buf);

#endif

    return stream;
}

wxSTD istream& DiagramDocument::LoadObject(wxSTD istream& stream)
{
#if wxUSE_PROLOGIO

    wxDocument::LoadObject(stream);

    char buf[400];
    (void) wxGetTempFileName("diag", buf);

    wxTransferStreamToFile(stream, buf);

    diagram.DeleteAllShapes();
    diagram.LoadFile(buf);
    wxRemoveFile(buf);

#endif

    return stream;
}
#else

wxOutputStream& DiagramDocument::SaveObject(wxOutputStream& stream)
{
#if wxUSE_PROLOGIO

    wxDocument::SaveObject(stream);
    wxChar buf[400];
    (void) wxGetTempFileName(_T("diag"), buf);

    diagram.SaveFile(buf);

    wxTransferFileToStream(buf, stream);

    wxRemoveFile(buf);

#endif

    return stream;
}

wxInputStream& DiagramDocument::LoadObject(wxInputStream& stream)
{
#if wxUSE_PROLOGIO
    wxDocument::LoadObject(stream);

    wxChar buf[400];
    (void) wxGetTempFileName(_T("diag"), buf);

    wxTransferStreamToFile(stream, buf);

    diagram.DeleteAllShapes();
    diagram.LoadFile(buf);
    wxRemoveFile(buf);
#endif

    return stream;
}

#endif

/*
 * Implementation of drawing command
 */

DiagramCommand::DiagramCommand(const wxString& name, int command, DiagramDocument *ddoc, wxClassInfo *info, double xx, double yy,
                               bool sel, wxShape *theShape, wxShape *fs, wxShape *ts)
               :wxCommand(true, name)
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
    deleteShape = false;
}

DiagramCommand::DiagramCommand(const wxString& name, int command, DiagramDocument *ddoc, wxBrush *backgroundColour, wxShape *theShape)
               :wxCommand(true, name)
{
    doc = ddoc;
    cmd = command;
    shape = theShape;
    fromShape = NULL;
    toShape = NULL;
    shapeInfo = NULL;
    x = 0.0;
    y = 0.0;
    selected = false;
    deleteShape = false;
    shapeBrush = backgroundColour;
    shapePen = NULL;
}

DiagramCommand::DiagramCommand(const wxString& name, int command, DiagramDocument *ddoc, const wxString& lab, wxShape *theShape)
               :wxCommand(true, name)
{
    doc = ddoc;
    cmd = command;
    shape = theShape;
    fromShape = NULL;
    toShape = NULL;
    shapeInfo = NULL;
    x = 0.0;
    y = 0.0;
    selected = false;
    deleteShape = false;
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
    case wxID_CUT:
    {
      if (shape)
      {
        deleteShape = true;

        shape->Select(false);

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

        doc->Modify(true);
        doc->UpdateAllViews();
      }

      break;
    }
    case OGLEDIT_ADD_SHAPE:
    {
      wxShape *theShape;
      if (shape)
        theShape = shape; // Saved from undoing the shape
      else
      {
        theShape = (wxShape *)shapeInfo->CreateObject();
        theShape->AssignNewIds();
        theShape->SetEventHandler(new MyEvtHandler(theShape, theShape, wxEmptyString));
        theShape->SetCentreResize(false);
        theShape->SetPen(wxBLACK_PEN);
        theShape->SetBrush(wxCYAN_BRUSH);

        theShape->SetSize(60, 60);
      }
      doc->GetDiagram()->AddShape(theShape);
      theShape->Show(true);

      wxClientDC dc(theShape->GetCanvas());
      theShape->GetCanvas()->PrepareDC(dc);

      theShape->Move(dc, x, y);

      shape = theShape;
      deleteShape = false;

      doc->Modify(true);
      doc->UpdateAllViews();
      break;
    }
    case OGLEDIT_ADD_LINE:
    {
      wxShape *theShape;
      if (shape)
        theShape = shape; // Saved from undoing the line
      else
      {
        theShape = (wxShape *)shapeInfo->CreateObject();
        theShape->AssignNewIds();
        theShape->SetEventHandler(new MyEvtHandler(theShape, theShape, wxEmptyString));
        theShape->SetPen(wxBLACK_PEN);
        theShape->SetBrush(wxRED_BRUSH);

        wxLineShape *lineShape = (wxLineShape *)theShape;

        // Yes, you can have more than 2 control points, in which case
        // it becomes a multi-segment line.
        lineShape->MakeLineControlPoints(2);
        lineShape->AddArrow(ARROW_ARROW, ARROW_POSITION_END, 10.0, 0.0, _T("Normal arrowhead"));
      }

      doc->GetDiagram()->AddShape(theShape);

      fromShape->AddLine((wxLineShape *)theShape, toShape);

      theShape->Show(true);

      wxClientDC dc(theShape->GetCanvas());
      theShape->GetCanvas()->PrepareDC(dc);

      // It won't get drawn properly unless you move both
      // connected images
      fromShape->Move(dc, fromShape->GetX(), fromShape->GetY());
      toShape->Move(dc, toShape->GetX(), toShape->GetY());

      shape = theShape;
      deleteShape = false;

      doc->Modify(true);
      doc->UpdateAllViews();
      break;
    }
    case OGLEDIT_CHANGE_BACKGROUND_COLOUR:
    {
      if (shape)
      {
        wxClientDC dc(shape->GetCanvas());
        shape->GetCanvas()->PrepareDC(dc);

        const wxBrush *oldBrush = shape->GetBrush();
        shape->SetBrush(shapeBrush);
        shapeBrush = oldBrush;
        shape->Draw(dc);

        doc->Modify(true);
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

        shape->FormatText(dc, /* (char*) (const char*) */ myHandler->label);
        shape->Draw(dc);

        doc->Modify(true);
        doc->UpdateAllViews();
      }

      break;
    }
  }
  return true;
}

bool DiagramCommand::Undo(void)
{
  switch (cmd)
  {
    case wxID_CUT:
    {
      if (shape)
      {
        doc->GetDiagram()->AddShape(shape);
        shape->Show(true);

        if (shape->IsKindOf(CLASSINFO(wxLineShape)))
        {
          wxLineShape *lineShape = (wxLineShape *)shape;

          fromShape->AddLine(lineShape, toShape);
        }
        if (selected)
          shape->Select(true);

        deleteShape = false;
      }
      doc->Modify(true);
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

        shape->Select(false, &dc);
        doc->GetDiagram()->RemoveShape(shape);
        shape->Unlink();
        deleteShape = true;
      }
      doc->Modify(true);
      doc->UpdateAllViews();
      break;
    }
    case OGLEDIT_CHANGE_BACKGROUND_COLOUR:
    {
      if (shape)
      {
        wxClientDC dc(shape->GetCanvas());
        shape->GetCanvas()->PrepareDC(dc);

        const wxBrush *oldBrush = shape->GetBrush();
        shape->SetBrush(shapeBrush);
        shapeBrush = oldBrush;
        shape->Draw(dc);

        doc->Modify(true);
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

        shape->FormatText(dc, /* (char*) (const char*) */ myHandler->label);
        shape->Draw(dc);

        doc->Modify(true);
        doc->UpdateAllViews();
      }

      break;
    }
  }
  return true;
}

// Remove each individual line connected to a shape by sending a command.
void DiagramCommand::RemoveLines(wxShape *shape)
{
  wxObjectList::compatibility_iterator node = shape->GetLines().GetFirst();
  while (node)
  {
    wxLineShape *line = (wxLineShape *)node->GetData();
    doc->GetCommandProcessor()->Submit(new DiagramCommand(_T("Cut"), wxID_CUT, doc, NULL, 0.0, 0.0, line->Selected(), line));

    node = shape->GetLines().GetFirst();
  }
}

/*
 * MyEvtHandler: an event handler class for all shapes
 */

void MyEvtHandler::OnLeftClick(double WXUNUSED(x), double WXUNUSED(y), int keys, int WXUNUSED(attachment))
{
  wxClientDC dc(GetShape()->GetCanvas());
  GetShape()->GetCanvas()->PrepareDC(dc);

  if (keys == 0)
  {
    // Selection is a concept the library knows about
    if (GetShape()->Selected())
    {
      GetShape()->Select(false, &dc);
      GetShape()->GetCanvas()->Redraw(dc); // Redraw because bits of objects will be are missing
    }
    else
    {
      // Ensure no other shape is selected, to simplify Undo/Redo code
      bool redraw = false;
      wxObjectList::compatibility_iterator node = GetShape()->GetCanvas()->GetDiagram()->GetShapeList()->GetFirst();
      while (node)
      {
        wxShape *eachShape = (wxShape *)node->GetData();
        if (eachShape->GetParent() == NULL)
        {
          if (eachShape->Selected())
          {
            eachShape->Select(false, &dc);
            redraw = true;
          }
        }
        node = node->GetNext();
      }
      GetShape()->Select(true, &dc);
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
#if wxUSE_STATUSBAR
    wxGetApp().frame->SetStatusText(label);
#endif // wxUSE_STATUSBAR
  }
}

/*
 * Implement connection of two shapes by right-dragging between them.
 */

void MyEvtHandler::OnBeginDragRight(double x, double y, int WXUNUSED(keys), int attachment)
{
  // Force attachment to be zero for now. Eventually we can deal with
  // the actual attachment point, e.g. a rectangle side if attachment mode is on.
  attachment = 0;

  wxClientDC dc(GetShape()->GetCanvas());
  GetShape()->GetCanvas()->PrepareDC(dc);

  wxPen dottedPen(*wxBLACK, 1, wxDOT);
  dc.SetLogicalFunction(OGLRBLF);
  dc.SetPen(dottedPen);
  double xp, yp;
  GetShape()->GetAttachmentPosition(attachment, &xp, &yp);
  dc.DrawLine((long) xp, (long) yp, (long) x, (long) y);
  GetShape()->GetCanvas()->CaptureMouse();
}

void MyEvtHandler::OnDragRight(bool WXUNUSED(draw), double x, double y, int WXUNUSED(keys), int attachment)
{
  // Force attachment to be zero for now
  attachment = 0;

  wxClientDC dc(GetShape()->GetCanvas());
  GetShape()->GetCanvas()->PrepareDC(dc);

  wxPen dottedPen(*wxBLACK, 1, wxDOT);
  dc.SetLogicalFunction(OGLRBLF);
  dc.SetPen(dottedPen);
  double xp, yp;
  GetShape()->GetAttachmentPosition(attachment, &xp, &yp);
  dc.DrawLine((long) xp, (long) yp, (long) x, (long) y);
}

void MyEvtHandler::OnEndDragRight(double x, double y, int WXUNUSED(keys), int WXUNUSED(attachment))
{
  GetShape()->GetCanvas()->ReleaseMouse();
  MyCanvas *canvas = (MyCanvas *)GetShape()->GetCanvas();

  // Check if we're on an object
  int new_attachment;
  wxShape *otherShape = canvas->FindFirstSensitiveShape(x, y, &new_attachment, OP_DRAG_RIGHT);

  if (otherShape && !otherShape->IsKindOf(CLASSINFO(wxLineShape)))
  {
    canvas->view->GetDocument()->GetCommandProcessor()->Submit(
      new DiagramCommand(_T("wxLineShape"), OGLEDIT_ADD_LINE, (DiagramDocument *)canvas->view->GetDocument(), CLASSINFO(wxLineShape),
      0.0, 0.0, false, NULL, GetShape(), otherShape));
  }
}

void MyEvtHandler::OnEndSize(double WXUNUSED(x), double WXUNUSED(y))
{
  wxClientDC dc(GetShape()->GetCanvas());
  GetShape()->GetCanvas()->PrepareDC(dc);

  GetShape()->FormatText(dc, /* (char*) (const char*) */ label);
}

/*
 * Diagram
 */

#if wxUSE_PROLOGIO

bool MyDiagram::OnShapeSave(wxExprDatabase& db, wxShape& shape, wxExpr& expr)
{
  wxDiagram::OnShapeSave(db, shape, expr);
  MyEvtHandler *handler = (MyEvtHandler *)shape.GetEventHandler();
  expr.AddAttributeValueString(_T("label"), handler->label);
  return true;
}

bool MyDiagram::OnShapeLoad(wxExprDatabase& db, wxShape& shape, wxExpr& expr)
{
  wxDiagram::OnShapeLoad(db, shape, expr);
  wxChar *label = NULL;
  expr.AssignAttributeValue(_T("label"), &label);
  MyEvtHandler *handler = new MyEvtHandler(&shape, &shape, wxString(label));
  shape.SetEventHandler(handler);

  if (label)
    delete[] label;
  return true;
}

#endif

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
