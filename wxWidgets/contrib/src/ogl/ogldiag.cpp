/////////////////////////////////////////////////////////////////////////////
// Name:        ogldiag.cpp
// Purpose:     wxDiagram
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

#if wxUSE_PROLOGIO
#include "wx/deprecated/wxexpr.h"
#endif

#ifdef new
#undef new
#endif

#include <ctype.h>
#include <math.h>
#include <stdlib.h>

#include "wx/ogl/ogl.h"


IMPLEMENT_DYNAMIC_CLASS(wxDiagram, wxObject)

// Object canvas
wxDiagram::wxDiagram()
{
  m_diagramCanvas = NULL;
  m_quickEditMode = false;
  m_snapToGrid = true;
  m_gridSpacing = 5.0;
  m_shapeList = new wxList;
  m_mouseTolerance = DEFAULT_MOUSE_TOLERANCE;
}

wxDiagram::~wxDiagram()
{
  if (m_shapeList)
    delete m_shapeList;
}

void wxDiagram::SetSnapToGrid(bool snap)
{
  m_snapToGrid = snap;
}

void wxDiagram::SetGridSpacing(double spacing)
{
  m_gridSpacing = spacing;
}

void wxDiagram::Snap(double *x, double *y)
{
  if (m_snapToGrid)
  {
    *x = m_gridSpacing * ((int)(*x/m_gridSpacing + 0.5));
    *y = m_gridSpacing * ((int)(*y/m_gridSpacing + 0.5));
  }
}


void wxDiagram::Redraw(wxDC& dc)
{
  if (m_shapeList)
  {
    if (GetCanvas())
      GetCanvas()->SetCursor(* wxHOURGLASS_CURSOR);
    wxNode *current = m_shapeList->GetFirst();

    while (current)
    {
      wxShape *object = (wxShape *)current->GetData();
      if (!object->GetParent())
        object->Draw(dc);

      current = current->GetNext();
    }
    if (GetCanvas())
      GetCanvas()->SetCursor(* wxSTANDARD_CURSOR);
  }
}

void wxDiagram::Clear(wxDC& dc)
{
  dc.Clear();
}

// Insert object after addAfter, or at end of list.
void wxDiagram::AddShape(wxShape *object, wxShape *addAfter)
{
  wxNode *nodeAfter = NULL;
  if (addAfter)
    nodeAfter = m_shapeList->Member(addAfter);

  if (!m_shapeList->Member(object))
  {
    if (nodeAfter)
    {
      if (nodeAfter->GetNext())
        m_shapeList->Insert(nodeAfter->GetNext(), object);
      else
        m_shapeList->Append(object);
    }
    else
      m_shapeList->Append(object);
    object->SetCanvas(GetCanvas());
  }
}

void wxDiagram::InsertShape(wxShape *object)
{
  m_shapeList->Insert(object);
  object->SetCanvas(GetCanvas());
}

void wxDiagram::RemoveShape(wxShape *object)
{
  m_shapeList->DeleteObject(object);
}

// Should this delete the actual objects too? I think not.
void wxDiagram::RemoveAllShapes()
{
  m_shapeList->Clear();
}

void wxDiagram::DeleteAllShapes()
{
  wxNode *node = m_shapeList->GetFirst();
  while (node)
  {
    wxShape *shape = (wxShape *)node->GetData();
    if (!shape->GetParent())
    {
      RemoveShape(shape);
      delete shape;
      node = m_shapeList->GetFirst();
    }
    else
      node = node->GetNext();
  }
}

void wxDiagram::ShowAll(bool show)
{
  wxNode *current = m_shapeList->GetFirst();

  while (current)
  {
    wxShape *object = (wxShape *)current->GetData();
    object->Show(show);

    current = current->GetNext();
  }
}

void wxDiagram::DrawOutline(wxDC& dc, double x1, double y1, double x2, double y2)
{
  wxPen dottedPen(*wxBLACK, 1, wxDOT);
  dc.SetPen(dottedPen);
  dc.SetBrush((* wxTRANSPARENT_BRUSH));

  wxPoint points[5];

  points[0].x = (int) x1;
  points[0].y = (int) y1;

  points[1].x = (int) x2;
  points[1].y = (int) y1;

  points[2].x = (int) x2;
  points[2].y = (int) y2;

  points[3].x = (int) x1;
  points[3].y = (int) y2;

  points[4].x = (int) x1;
  points[4].y = (int) y1;
  dc.DrawLines(5, points);
}

// Make sure all text that should be centred, is centred.
void wxDiagram::RecentreAll(wxDC& dc)
{
  wxNode *object_node = m_shapeList->GetFirst();
  while (object_node)
  {
    wxShape *obj = (wxShape *)object_node->GetData();
    obj->Recentre(dc);
    object_node = object_node->GetNext();
  }
}

// Input/output
#if wxUSE_PROLOGIO
bool wxDiagram::SaveFile(const wxString& filename)
{
  wxBeginBusyCursor();

  wxExprDatabase *database = new wxExprDatabase;

  // First write the diagram type
  wxExpr *header = new wxExpr(_T("diagram"));
  OnHeaderSave(*database, *header);

  database->Append(header);

  wxNode *node = m_shapeList->GetFirst();
  while (node)
  {
    wxShape *shape = (wxShape *)node->GetData();

    if (!shape->IsKindOf(CLASSINFO(wxControlPoint)))
    {
      wxExpr *expr;
      if (shape->IsKindOf(CLASSINFO(wxLineShape)))
        expr = new wxExpr(_T("line"));
       else
        expr = new wxExpr(_T("shape"));

      OnShapeSave(*database, *shape, *expr);
    }
    node = node->GetNext();
  }
  OnDatabaseSave(*database);

  wxString tempFile;
  wxGetTempFileName(wxT("diag"), tempFile);
  FILE* file = fopen(tempFile.mb_str(wxConvFile), "w");
  if (! file)
  {
    wxEndBusyCursor();
    delete database;
    return false;
  }

  database->Write(file);
  fclose(file);
  delete database;

/*
  // Save backup
  if (FileExists(filename))
  {
    char buf[400];
#ifdef __X__
    sprintf(buf, "%s.bak", filename);
#endif
#ifdef __WXMSW__
    sprintf(buf, "_diagram.bak");
#endif
    if (FileExists(buf)) wxRemoveFile(buf);
    if (!wxRenameFile(filename, buf))
    {
      wxCopyFile(filename, buf);
      wxRemoveFile(filename);
    }
  }
*/

  // Copy the temporary file to the correct filename
  if (!wxRenameFile(tempFile, filename))
  {
    wxCopyFile(tempFile, filename);
    wxRemoveFile(tempFile);
  }

  wxEndBusyCursor();
  return true;
}

bool wxDiagram::LoadFile(const wxString& filename)
{
  wxBeginBusyCursor();

  wxExprDatabase database(wxExprInteger, _T("id"));
  if (!database.Read(filename))
  {
    wxEndBusyCursor();
    return false;
  }

  DeleteAllShapes();

  database.BeginFind();
  wxExpr *header = database.FindClauseByFunctor(_T("diagram"));

  if (header)
    OnHeaderLoad(database, *header);

  // Scan through all clauses and register the ids
  wxNode *node = database.GetFirst();
  while (node)
  {
    wxExpr *clause = (wxExpr *)node->GetData();
    long id = -1;
    clause->GetAttributeValue(_T("id"), id);
    wxRegisterId(id);
    node = node->GetNext();
  }

  ReadNodes(database);
  ReadContainerGeometry(database);
  ReadLines(database);

  OnDatabaseLoad(database);

  wxEndBusyCursor();

  return true;
}

void wxDiagram::ReadNodes(wxExprDatabase& database)
{
  // Find and create the node images
  database.BeginFind();
  wxExpr *clause = database.FindClauseByFunctor(_T("shape"));
  while (clause)
  {
    wxChar *type = NULL;
    long parentId = -1;

    clause->AssignAttributeValue(wxT("type"), &type);
    clause->AssignAttributeValue(wxT("parent"), &parentId);
    wxClassInfo *classInfo = wxClassInfo::FindClass(type);
    if (classInfo)
    {
      wxShape *shape = (wxShape *)classInfo->CreateObject();
      OnShapeLoad(database, *shape, *clause);

      shape->SetCanvas(GetCanvas());
      shape->Show(true);

      m_shapeList->Append(shape);

      // If child of composite, link up
      if (parentId > -1)
      {
        wxExpr *parentExpr = database.HashFind(_T("shape"), parentId);
        if (parentExpr && parentExpr->GetClientData())
        {
          wxShape *parent = (wxShape *)parentExpr->GetClientData();
          shape->SetParent(parent);
          parent->GetChildren().Append(shape);
        }
      }

      clause->SetClientData(shape);
    }
    if (type)
      delete[] type;

    clause = database.FindClauseByFunctor(_T("shape"));
  }
  return;
}

void wxDiagram::ReadLines(wxExprDatabase& database)
{
  database.BeginFind();
  wxExpr *clause = database.FindClauseByFunctor(_T("line"));
  while (clause)
  {
    wxString type;
    long parentId = -1;

    clause->GetAttributeValue(_T("type"), type);
    clause->GetAttributeValue(_T("parent"), parentId);
    wxClassInfo *classInfo = wxClassInfo::FindClass(type);
    if (classInfo)
    {
      wxLineShape *shape = (wxLineShape *)classInfo->CreateObject();
      shape->Show(true);

      OnShapeLoad(database, *shape, *clause);
      shape->SetCanvas(GetCanvas());

      long image_to = -1; long image_from = -1;
      clause->GetAttributeValue(_T("to"), image_to);
      clause->GetAttributeValue(_T("from"), image_from);

      wxExpr *image_to_expr = database.HashFind(_T("shape"), image_to);

      if (!image_to_expr)
      {
        // Error
      }
      wxExpr *image_from_expr = database.HashFind(_T("shape"), image_from);

      if (!image_from_expr)
      {
        // Error
      }

      if (image_to_expr && image_from_expr)
      {
        wxShape *image_to_object = (wxShape *)image_to_expr->GetClientData();
        wxShape *image_from_object = (wxShape *)image_from_expr->GetClientData();

        if (image_to_object && image_from_object)
        {
          image_from_object->AddLine(shape, image_to_object, shape->GetAttachmentFrom(), shape->GetAttachmentTo());
        }
      }
      clause->SetClientData(shape);

      m_shapeList->Append(shape);
    }

    clause = database.FindClauseByFunctor(_T("line"));
  }
}

// Containers have divisions that reference adjoining divisions,
// so we need a separate pass to link everything up.
// Also used by Symbol Library.
void wxDiagram::ReadContainerGeometry(wxExprDatabase& database)
{
  database.BeginFind();
  wxExpr *clause = database.FindClauseByFunctor(_T("shape"));
  while (clause)
  {
    wxShape *image = (wxShape *)clause->GetClientData();
    if (image && image->IsKindOf(CLASSINFO(wxCompositeShape)))
    {
      wxCompositeShape *composite = (wxCompositeShape *)image;
      wxExpr *divisionExpr = NULL;

      // Find the list of divisions in the composite
      clause->GetAttributeValue(_T("divisions"), &divisionExpr);
      if (divisionExpr)
      {
        int i = 0;
        wxExpr *idExpr = divisionExpr->Nth(i);
        while (idExpr)
        {
          long divisionId = idExpr->IntegerValue();
          wxExpr *childExpr = database.HashFind(_T("shape"), divisionId);
          if (childExpr && childExpr->GetClientData())
          {
            wxDivisionShape *child = (wxDivisionShape *)childExpr->GetClientData();
            composite->GetDivisions().Append(child);

            // Find the adjoining shapes
            long leftSideId = -1;
            long topSideId = -1;
            long rightSideId = -1;
            long bottomSideId = -1;
            childExpr->GetAttributeValue(_T("left_side"), leftSideId);
            childExpr->GetAttributeValue(_T("top_side"), topSideId);
            childExpr->GetAttributeValue(_T("right_side"), rightSideId);
            childExpr->GetAttributeValue(_T("bottom_side"), bottomSideId);
            if (leftSideId > -1)
            {
              wxExpr *leftExpr = database.HashFind(_T("shape"), leftSideId);
              if (leftExpr && leftExpr->GetClientData())
              {
                wxDivisionShape *leftSide = (wxDivisionShape *)leftExpr->GetClientData();
                child->SetLeftSide(leftSide);
              }
            }
            if (topSideId > -1)
            {
              wxExpr *topExpr = database.HashFind(_T("shape"), topSideId);
              if (topExpr && topExpr->GetClientData())
              {
                wxDivisionShape *topSide = (wxDivisionShape *)topExpr->GetClientData();
                child->SetTopSide(topSide);
              }
            }
            if (rightSideId > -1)
            {
              wxExpr *rightExpr = database.HashFind(_T("shape"), rightSideId);
              if (rightExpr && rightExpr->GetClientData())
              {
                wxDivisionShape *rightSide = (wxDivisionShape *)rightExpr->GetClientData();
                child->SetRightSide(rightSide);
              }
            }
            if (bottomSideId > -1)
            {
              wxExpr *bottomExpr = database.HashFind(_T("shape"), bottomSideId);
              if (bottomExpr && bottomExpr->GetClientData())
              {
                wxDivisionShape *bottomSide = (wxDivisionShape *)bottomExpr->GetClientData();
                child->SetBottomSide(bottomSide);
              }
            }
          }
          i ++;
          idExpr = divisionExpr->Nth(i);
        }
      }
    }

    clause = database.FindClauseByFunctor(_T("shape"));
  }
}

// Allow for modifying file
bool wxDiagram::OnDatabaseLoad(wxExprDatabase& WXUNUSED(db))
{
  return true;
}

bool wxDiagram::OnDatabaseSave(wxExprDatabase& WXUNUSED(db))
{
  return true;
}

bool wxDiagram::OnShapeSave(wxExprDatabase& db, wxShape& shape, wxExpr& expr)
{
  shape.WriteAttributes(&expr);
  db.Append(&expr);

  if (shape.IsKindOf(CLASSINFO(wxCompositeShape)))
  {
    wxNode *node = shape.GetChildren().GetFirst();
    while (node)
    {
      wxShape *childShape = (wxShape *)node->GetData();
      wxExpr *childExpr = new wxExpr(_T("shape"));
      OnShapeSave(db, *childShape, *childExpr);
      node = node->GetNext();
    }
  }

  return true;
}

bool wxDiagram::OnShapeLoad(wxExprDatabase& WXUNUSED(db), wxShape& shape, wxExpr& expr)
{
  shape.ReadAttributes(&expr);
  return true;
}

bool wxDiagram::OnHeaderSave(wxExprDatabase& WXUNUSED(db), wxExpr& WXUNUSED(expr))
{
  return true;
}

bool wxDiagram::OnHeaderLoad(wxExprDatabase& WXUNUSED(db), wxExpr& WXUNUSED(expr))
{
  return true;
}

#endif

void wxDiagram::SetCanvas(wxShapeCanvas *can)
{
  m_diagramCanvas = can;
}

// Find a shape by its id
wxShape* wxDiagram::FindShape(long id) const
{
    wxNode* node = GetShapeList()->GetFirst();
    while (node)
    {
        wxShape* shape = (wxShape*) node->GetData();
        if (shape->GetId() == id)
            return shape;
        node = node->GetNext();
    }
    return NULL;
}


//// Crossings classes

wxLineCrossings::wxLineCrossings()
{
}

wxLineCrossings::~wxLineCrossings()
{
    ClearCrossings();
}

void wxLineCrossings::FindCrossings(wxDiagram& diagram)
{
    ClearCrossings();
    wxNode* node1 = diagram.GetShapeList()->GetFirst();
    while (node1)
    {
        wxShape* shape1 = (wxShape*) node1->GetData();
        if (shape1->IsKindOf(CLASSINFO(wxLineShape)))
        {
            wxLineShape* lineShape1 = (wxLineShape*) shape1;
            // Iterate through the segments
            wxList* pts1 = lineShape1->GetLineControlPoints();
            size_t i;
            for (i = 0; i < (pts1->GetCount() - 1); i++)
            {
                wxRealPoint* pt1_a = (wxRealPoint*) (pts1->Item(i)->GetData());
                wxRealPoint* pt1_b = (wxRealPoint*) (pts1->Item(i+1)->GetData());

                // Now we iterate through the segments again

                wxNode* node2 = diagram.GetShapeList()->GetFirst();
                while (node2)
                {
                    wxShape* shape2 = (wxShape*) node2->GetData();

                    // Assume that the same line doesn't cross itself
                    if (shape2->IsKindOf(CLASSINFO(wxLineShape)) && (shape1 != shape2))
                    {
                        wxLineShape* lineShape2 = (wxLineShape*) shape2;
                        // Iterate through the segments
                        wxList* pts2 = lineShape2->GetLineControlPoints();
                        int j;
                        for (j = 0; j < (int) (pts2->GetCount() - 1); j++)
                        {
                            wxRealPoint* pt2_a = (wxRealPoint*) (pts2->Item(j)->GetData());
                            wxRealPoint* pt2_b = (wxRealPoint*) (pts2->Item(j+1)->GetData());

                            // Now let's see if these two segments cross.
                            double ratio1, ratio2;
                            oglCheckLineIntersection(pt1_a->x, pt1_a->y, pt1_b->x, pt1_b->y,
                               pt2_a->x, pt2_a->y, pt2_b->x, pt2_b->y,
                             & ratio1, & ratio2);

                            if ((ratio1 < 1.0) && (ratio1 > -1.0))
                            {
                                // Intersection!
                                wxLineCrossing* crossing = new wxLineCrossing;
                                crossing->m_intersect.x = (pt1_a->x + (pt1_b->x - pt1_a->x)*ratio1);
                                crossing->m_intersect.y = (pt1_a->y + (pt1_b->y - pt1_a->y)*ratio1);

                                crossing->m_pt1 = * pt1_a;
                                crossing->m_pt2 = * pt1_b;
                                crossing->m_pt3 = * pt2_a;
                                crossing->m_pt4 = * pt2_b;

                                crossing->m_lineShape1 = lineShape1;
                                crossing->m_lineShape2 = lineShape2;

                                m_crossings.Append(crossing);
                            }
                        }
                    }
                    node2 = node2->GetNext();
                }
            }
        }

        node1 = node1->GetNext();
    }
}

void wxLineCrossings::DrawCrossings(wxDiagram& WXUNUSED(diagram), wxDC& dc)
{
    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    long arcWidth = 8;

    wxNode* node = m_crossings.GetFirst();
    while (node)
    {
        wxLineCrossing* crossing = (wxLineCrossing*) node->GetData();
//        dc.DrawEllipse((long) (crossing->m_intersect.x - (arcWidth/2.0) + 0.5), (long) (crossing->m_intersect.y - (arcWidth/2.0) + 0.5),
//           arcWidth, arcWidth);


        // Let's do some geometry to find the points on either end of the arc.
/*

(x1, y1)
    |\
    | \
    |  \
    |   \
    |    \
    |    |\ c    c1
    |  a | \
         |  \
    |     -  x <-- centre of arc
 a1 |     b  |\
    |        | \ c2
    |     a2 |  \
    |         -  \
    |         b2  \
    |              \
    |_______________\ (x2, y2)
          b1

*/

        double a1 = wxMax(crossing->m_pt1.y, crossing->m_pt2.y) - wxMin(crossing->m_pt1.y, crossing->m_pt2.y) ;
        double b1 = wxMax(crossing->m_pt1.x, crossing->m_pt2.x) - wxMin(crossing->m_pt1.x, crossing->m_pt2.x) ;
        double c1 = sqrt( (a1*a1) + (b1*b1) );

        double c = arcWidth / 2.0;
        double a = c * a1/c1 ;
        double b = c * b1/c1 ;

        // I'm not sure this is right, since we don't know which direction we should be going in - need
        // to know which way the line slopes and choose the sign appropriately.
        double arcX1 = crossing->m_intersect.x - b;
        double arcY1 = crossing->m_intersect.y - a;

        double arcX2 = crossing->m_intersect.x + b;
        double arcY2 = crossing->m_intersect.y + a;

        dc.SetPen(*wxBLACK_PEN);
        dc.DrawArc( (long) arcX1, (long) arcY1, (long) arcX2, (long) arcY2,
            (long) crossing->m_intersect.x, (long) crossing->m_intersect.y);

        dc.SetPen(*wxWHITE_PEN);
        dc.DrawLine( (long) arcX1, (long) arcY1, (long) arcX2, (long) arcY2 );

        node = node->GetNext();
    }
}

void wxLineCrossings::ClearCrossings()
{
    wxNode* node = m_crossings.GetFirst();
    while (node)
    {
        wxLineCrossing* crossing = (wxLineCrossing*) node->GetData();
        delete crossing;
        node = node->GetNext();
    }
    m_crossings.Clear();
}

