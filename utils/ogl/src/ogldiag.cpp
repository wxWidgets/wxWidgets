/////////////////////////////////////////////////////////////////////////////
// Name:        ogldiag.cpp
// Purpose:     wxDiagram
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "ogldiag.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#ifdef PROLOGIO
#include <wx/wxexpr.h>
#endif

#if wxUSE_IOSTREAMH
#include <iostream.h>
#else
#include <iostream>
#endif

#include <fstream.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>

#include "basic.h"
#include "basicp.h"
#include "canvas.h"
#include "ogldiag.h"
#include "lines.h"
#include "composit.h"
#include "misc.h"

IMPLEMENT_DYNAMIC_CLASS(wxDiagram, wxObject)

// Object canvas
wxDiagram::wxDiagram()
{
  m_diagramCanvas = NULL;
  m_quickEditMode = FALSE;
  m_snapToGrid = TRUE;
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
      GetCanvas()->SetCursor(wxHOURGLASS_CURSOR);
    wxNode *current = m_shapeList->First();

    while (current)
    {
      wxShape *object = (wxShape *)current->Data();
      if (!object->GetParent())
        object->Draw(dc);

      current = current->Next();
    }
    if (GetCanvas())
      GetCanvas()->SetCursor(wxSTANDARD_CURSOR);
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
      if (nodeAfter->Next())
        m_shapeList->Insert(nodeAfter->Next(), object);
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
  wxNode *node = m_shapeList->First();
  while (node)
  {
    wxShape *shape = (wxShape *)node->Data();
    if (!shape->GetParent())
    {
      RemoveShape(shape);
      delete shape;
      node = m_shapeList->First();
    }
    else
      node = node->Next();
  }
}

void wxDiagram::ShowAll(bool show)
{
  wxNode *current = m_shapeList->First();

  while (current)
  {
    wxShape *object = (wxShape *)current->Data();
    object->Show(show);

    current = current->Next();
  }
}

void wxDiagram::DrawOutline(wxDC& dc, double x1, double y1, double x2, double y2)
{
  wxPen dottedPen(wxColour(0, 0, 0), 1, wxDOT);
  dc.SetPen(dottedPen);
  dc.SetBrush((* wxTRANSPARENT_BRUSH));

  wxPoint points[5];

  points[0].x = x1;
  points[0].y = y1;

  points[1].x = x2;
  points[1].y = y1;

  points[2].x = x2;
  points[2].y = y2;

  points[3].x = x1;
  points[3].y = y2;

  points[4].x = x1;
  points[4].y = y1;
  dc.DrawLines(5, points);
}

// Make sure all text that should be centred, is centred.
void wxDiagram::RecentreAll(wxDC& dc)
{
  wxNode *object_node = m_shapeList->First();
  while (object_node)
  {
    wxShape *obj = (wxShape *)object_node->Data();
    obj->Recentre(dc);
    object_node = object_node->Next();
  }
}

// Input/output
#ifdef PROLOGIO
bool wxDiagram::SaveFile(const wxString& filename)
{
  wxBeginBusyCursor();
  
  wxExprDatabase *database = new wxExprDatabase;

  // First write the diagram type
  wxExpr *header = new wxExpr("diagram");
  OnHeaderSave(*database, *header);
  
  database->Append(header);
  
  wxNode *node = m_shapeList->First();
  while (node)
  {
    wxShape *shape = (wxShape *)node->Data();
    
    if (!shape->IsKindOf(CLASSINFO(wxControlPoint)))
    {
      wxExpr *expr = NULL;
      if (shape->IsKindOf(CLASSINFO(wxLineShape)))
        expr = new wxExpr("line");
       else
        expr = new wxExpr("shape");

      OnShapeSave(*database, *shape, *expr);
    }
    node = node->Next();
  }
  OnDatabaseSave(*database);
  
  char tempFile[400];
  wxGetTempFileName("diag", tempFile);
  ofstream stream(tempFile);
  if (stream.bad())
  {
    wxEndBusyCursor();
    delete database;
    return FALSE;
  }
  
  database->Write(stream);
  stream.close();
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
  return TRUE;
}

bool wxDiagram::LoadFile(const wxString& filename)
{
  wxBeginBusyCursor();
  
  wxExprDatabase database(wxExprInteger, "id");
  if (!database.Read(filename))
  {
    wxEndBusyCursor();
    return FALSE;
  }
  
  DeleteAllShapes();

  database.BeginFind();
  wxExpr *header = database.FindClauseByFunctor("diagram");

  if (header)
    OnHeaderLoad(database, *header);

  // Scan through all clauses and register the ids
  wxNode *node = database.First();
  while (node)
  {
    wxExpr *clause = (wxExpr *)node->Data();
    long id = -1;
    clause->GetAttributeValue("id", id);
    RegisterId(id);
    node = node->Next();
  }

  ReadNodes(database);
  ReadContainerGeometry(database);
  ReadLines(database);
  
  OnDatabaseLoad(database);

  wxEndBusyCursor();

  return TRUE;
}

void wxDiagram::ReadNodes(wxExprDatabase& database)
{
  // Find and create the node images
  database.BeginFind();
  wxExpr *clause = database.FindClauseByFunctor("shape");
  while (clause)
  {
    char *type = NULL;
    long parentId = -1;

    clause->AssignAttributeValue("type", &type);
    clause->AssignAttributeValue("parent", &parentId);
    wxClassInfo *classInfo = wxClassInfo::FindClass(type);
    if (classInfo)
    {
      wxShape *shape = (wxShape *)classInfo->CreateObject();
      OnShapeLoad(database, *shape, *clause);
      
      shape->SetCanvas(GetCanvas());
      shape->Show(TRUE);

      m_shapeList->Append(shape);

      // If child of composite, link up
      if (parentId > -1)
      {
        wxExpr *parentExpr = database.HashFind("shape", parentId);
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
      
    clause = database.FindClauseByFunctor("shape");
  }
  return;
}

void wxDiagram::ReadLines(wxExprDatabase& database)
{
  database.BeginFind();
  wxExpr *clause = database.FindClauseByFunctor("line");
  while (clause)
  {
    wxString type("");
    long parentId = -1;

    clause->GetAttributeValue("type", type);
    clause->GetAttributeValue("parent", parentId);
    wxClassInfo *classInfo = wxClassInfo::FindClass((char*) (const char*) type);
    if (classInfo)
    {
      wxLineShape *shape = (wxLineShape *)classInfo->CreateObject();
      shape->Show(TRUE);

      OnShapeLoad(database, *shape, *clause);
      shape->SetCanvas(GetCanvas());

      long image_to = -1; long image_from = -1;
      clause->GetAttributeValue("to", image_to);
      clause->GetAttributeValue("from", image_from);

      wxExpr *image_to_expr = database.HashFind("shape", image_to);

      if (!image_to_expr)
      {
        // Error
      }
      wxExpr *image_from_expr = database.HashFind("shape", image_from);

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

    clause = database.FindClauseByFunctor("line");
  }
}

// Containers have divisions that reference adjoining divisions,
// so we need a separate pass to link everything up.
// Also used by Symbol Library.
void wxDiagram::ReadContainerGeometry(wxExprDatabase& database)
{
  database.BeginFind();
  wxExpr *clause = database.FindClauseByFunctor("shape");
  while (clause)
  {
    wxShape *image = (wxShape *)clause->GetClientData();
    if (image && image->IsKindOf(CLASSINFO(wxCompositeShape)))
    {
      wxCompositeShape *composite = (wxCompositeShape *)image;
      wxExpr *divisionExpr = NULL;

      // Find the list of divisions in the composite
      clause->GetAttributeValue("divisions", &divisionExpr);
      if (divisionExpr)
      {
        int i = 0;
        wxExpr *idExpr = divisionExpr->Nth(i);
        while (idExpr)
        {
          long divisionId = idExpr->IntegerValue();
          wxExpr *childExpr = database.HashFind("shape", divisionId);
          if (childExpr && childExpr->GetClientData())
          {
            wxDivisionShape *child = (wxDivisionShape *)childExpr->GetClientData();
            composite->GetDivisions().Append(child);

            // Find the adjoining shapes
            long leftSideId = -1;
            long topSideId = -1;
            long rightSideId = -1;
            long bottomSideId = -1;
            childExpr->GetAttributeValue("left_side", leftSideId);
            childExpr->GetAttributeValue("top_side", topSideId);
            childExpr->GetAttributeValue("right_side", rightSideId);
            childExpr->GetAttributeValue("bottom_side", bottomSideId);
            if (leftSideId > -1)
            {
              wxExpr *leftExpr = database.HashFind("shape", leftSideId);
              if (leftExpr && leftExpr->GetClientData())
              {
                wxDivisionShape *leftSide = (wxDivisionShape *)leftExpr->GetClientData();
                child->SetLeftSide(leftSide);
              }
            }
            if (topSideId > -1)
            {
              wxExpr *topExpr = database.HashFind("shape", topSideId);
              if (topExpr && topExpr->GetClientData())
              {
                wxDivisionShape *topSide = (wxDivisionShape *)topExpr->GetClientData();
                child->SetTopSide(topSide);
              }
            }
            if (rightSideId > -1)
            {
              wxExpr *rightExpr = database.HashFind("shape", rightSideId);
              if (rightExpr && rightExpr->GetClientData())
              {
                wxDivisionShape *rightSide = (wxDivisionShape *)rightExpr->GetClientData();
                child->SetRightSide(rightSide);
              }
            }
            if (bottomSideId > -1)
            {
              wxExpr *bottomExpr = database.HashFind("shape", bottomSideId);
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

    clause = database.FindClauseByFunctor("shape");
  }
}

// Allow for modifying file
bool wxDiagram::OnDatabaseLoad(wxExprDatabase& db)
{
  return TRUE;
}

bool wxDiagram::OnDatabaseSave(wxExprDatabase& db)
{
  return TRUE;
}

bool wxDiagram::OnShapeSave(wxExprDatabase& db, wxShape& shape, wxExpr& expr)
{
  shape.WriteAttributes(&expr);
  db.Append(&expr);

  if (shape.IsKindOf(CLASSINFO(wxCompositeShape)))
  {
    wxNode *node = shape.GetChildren().First();
    while (node)
    {
      wxShape *childShape = (wxShape *)node->Data();
      wxExpr *childExpr = new wxExpr("shape");
      OnShapeSave(db, *childShape, *childExpr);
      node = node->Next();
    }
  }

  return TRUE;
}

bool wxDiagram::OnShapeLoad(wxExprDatabase& db, wxShape& shape, wxExpr& expr)
{
  shape.ReadAttributes(&expr);
  return TRUE;
}

bool wxDiagram::OnHeaderSave(wxExprDatabase& db, wxExpr& expr)
{
  return TRUE;
}

bool wxDiagram::OnHeaderLoad(wxExprDatabase& db, wxExpr& expr)
{
  return TRUE;
}

#endif

void wxDiagram::SetCanvas(wxShapeCanvas *can)
{
  m_diagramCanvas = can;
}

// Find a shape by its id
wxShape* wxDiagram::FindShape(long id) const
{
    wxNode* node = GetShapeList()->First();
    while (node)
    {
        wxShape* shape = (wxShape*) node->Data();
        if (shape->GetId() == id)
            return shape;
        node = node->Next();
    }
    return NULL;
}

