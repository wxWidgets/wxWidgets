/////////////////////////////////////////////////////////////////////////////
// Name:        basic2.cpp
// Purpose:     Basic OGL classes (2)
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
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

#if USE_IOSTREAMH
#include <iostream.h>
#else
#include <iostream>
#endif

#include <stdio.h>
#include <ctype.h>
#include <math.h>

#include "basic.h"
#include "basicp.h"
#include "composit.h"
#include "lines.h"
#include "canvas.h"
#include "divided.h"
#include "misc.h"

// Control point types
// Rectangle and most other shapes
#define CONTROL_POINT_VERTICAL   1
#define CONTROL_POINT_HORIZONTAL 2
#define CONTROL_POINT_DIAGONAL   3

// Line
#define CONTROL_POINT_ENDPOINT_TO 4
#define CONTROL_POINT_ENDPOINT_FROM 5
#define CONTROL_POINT_LINE       6

// Two stage construction: need to call Create
IMPLEMENT_DYNAMIC_CLASS(wxPolygonShape, wxShape)

wxPolygonShape::wxPolygonShape()
{
  m_points = NULL;
  m_originalPoints = NULL;
}

void wxPolygonShape::Create(wxList *the_points)
{
  ClearPoints();

  m_originalPoints = the_points;

  // Duplicate the list of points
  m_points = new wxList;

  wxNode *node = the_points->First();
  while (node)
  {
    wxRealPoint *point = (wxRealPoint *)node->Data();
    wxRealPoint *new_point = new wxRealPoint(point->x, point->y);
    m_points->Append((wxObject*) new_point);
    node = node->Next();
  }
  CalculateBoundingBox();
  m_originalWidth = m_boundWidth;
  m_originalHeight = m_boundHeight;
  SetDefaultRegionSize();
}

wxPolygonShape::~wxPolygonShape()
{
    ClearPoints();
}

void wxPolygonShape::ClearPoints()
{
  if (m_points)
  {
    wxNode *node = m_points->First();
    while (node)
    {
      wxRealPoint *point = (wxRealPoint *)node->Data();
      delete point;
      delete node;
      node = m_points->First();
    }
    delete m_points;
    m_points = NULL;
  }
  if (m_originalPoints)
  {
    wxNode *node = m_originalPoints->First();
    while (node)
    {
      wxRealPoint *point = (wxRealPoint *)node->Data();
      delete point;
      delete node;
      node = m_originalPoints->First();
    }
    delete m_originalPoints;
    m_originalPoints = NULL;
  }
}


// Width and height. Centre of object is centre of box.
void wxPolygonShape::GetBoundingBoxMin(double *width, double *height)
{
  *width = m_boundWidth;
  *height = m_boundHeight;
}

void wxPolygonShape::CalculateBoundingBox()
{
  // Calculate bounding box at construction (and presumably resize) time
  double left = 10000;
  double right = -10000;
  double top = 10000;
  double bottom = -10000;

  wxNode *node = m_points->First();
  while (node)
  {
    wxRealPoint *point = (wxRealPoint *)node->Data();
    if (point->x < left) left = point->x;
    if (point->x > right) right = point->x;

    if (point->y < top) top = point->y;
    if (point->y > bottom) bottom = point->y;

    node = node->Next();
  }
  m_boundWidth = right - left;
  m_boundHeight = bottom - top;
}

// Recalculates the centre of the polygon, and
// readjusts the point offsets accordingly.
// Necessary since the centre of the polygon
// is expected to be the real centre of the bounding
// box.
void wxPolygonShape::CalculatePolygonCentre()
{
  double left = 10000;
  double right = -10000;
  double top = 10000;
  double bottom = -10000;

  wxNode *node = m_points->First();
  while (node)
  {
    wxRealPoint *point = (wxRealPoint *)node->Data();
    if (point->x < left) left = point->x;
    if (point->x > right) right = point->x;

    if (point->y < top) top = point->y;
    if (point->y > bottom) bottom = point->y;

    node = node->Next();
  }
  double bwidth = right - left;
  double bheight = bottom - top;

  double newCentreX = (double)(left + (bwidth/2.0));
  double newCentreY = (double)(top + (bheight/2.0));

  node = m_points->First();
  while (node)
  {
    wxRealPoint *point = (wxRealPoint *)node->Data();
    point->x -= newCentreX;
    point->y -= newCentreY;
    node = node->Next();
  }
  m_xpos += newCentreX;
  m_ypos += newCentreY;
}

bool PolylineHitTest(double n, double xvec[], double yvec[],
                           double x1, double y1, double x2, double y2)
{
  bool isAHit = FALSE;
  int i;
  double lastx = xvec[0];
  double lasty = yvec[0];

  double min_ratio = 1.0;
  double line_ratio;
  double other_ratio;

//  char buf[300];
  for (i = 1; i < n; i++)
  {
    oglCheckLineIntersection(x1, y1, x2, y2, lastx, lasty, xvec[i], yvec[i],
                            &line_ratio, &other_ratio);
    if (line_ratio != 1.0)
      isAHit = TRUE;
//    sprintf(buf, "Line ratio = %.2f, other ratio = %.2f\n", line_ratio, other_ratio);
//    ClipsErrorFunction(buf);
    lastx = xvec[i];
    lasty = yvec[i];

    if (line_ratio < min_ratio)
      min_ratio = line_ratio;
  }

  // Do last (implicit) line if last and first doubles are not identical
  if (!(xvec[0] == lastx && yvec[0] == lasty))
  {
    oglCheckLineIntersection(x1, y1, x2, y2, lastx, lasty, xvec[0], yvec[0],
                            &line_ratio, &other_ratio);
    if (line_ratio != 1.0)
      isAHit = TRUE;
//    sprintf(buf, "Line ratio = %.2f, other ratio = %.2f\n", line_ratio, other_ratio);
//    ClipsErrorFunction(buf);

    if (line_ratio < min_ratio)
      min_ratio = line_ratio;
  }
//  ClipsErrorFunction("\n");
  return isAHit;
}

bool wxPolygonShape::HitTest(double x, double y, int *attachment, double *distance)
{
  // Imagine four lines radiating from this point. If all of these lines hit the polygon,
  // we're inside it, otherwise we're not. Obviously we'd need more radiating lines
  // to be sure of correct results for very strange (concave) shapes.
  double endPointsX[4];
  double endPointsY[4];
  // North
  endPointsX[0] = x;
  endPointsY[0] = (double)(y - 1000.0);
  // East
  endPointsX[1] = (double)(x + 1000.0);
  endPointsY[1] = y;
  // South
  endPointsX[2] = x;
  endPointsY[2] = (double)(y + 1000.0);
  // West
  endPointsX[3] = (double)(x - 1000.0);
  endPointsY[3] = y;

  // Store polygon points in an array
  int np = m_points->Number();
  double *xpoints = new double[np];
  double *ypoints = new double[np];
  wxNode *node = m_points->First();
  int i = 0;
  while (node)
  {
    wxRealPoint *point = (wxRealPoint *)node->Data();
    xpoints[i] = point->x + m_xpos;
    ypoints[i] = point->y + m_ypos;
    node = node->Next();
    i ++;
  }

  // We assume it's inside the polygon UNLESS one or more
  // lines don't hit the outline.
  bool isContained = TRUE;

  int noPoints = 4;
  for (i = 0; i < noPoints; i++)
  {
    if (!PolylineHitTest(np, xpoints, ypoints, x, y, endPointsX[i], endPointsY[i]))
      isContained = FALSE;
  }
/*
  if (isContained)
    ClipsErrorFunction("It's a hit!\n");
  else
    ClipsErrorFunction("No hit.\n");
*/
  delete[] xpoints;
  delete[] ypoints;

  if (!isContained)
    return FALSE;

  int nearest_attachment = 0;

  // If a hit, check the attachment points within the object.
  int n = GetNumberOfAttachments();
  double nearest = 999999.0;

  for (i = 0; i < n; i++)
  {
    double xp, yp;
    if (GetAttachmentPosition(i, &xp, &yp))
    {
      double l = (double)sqrt(((xp - x) * (xp - x)) +
                 ((yp - y) * (yp - y)));
      if (l < nearest)
      {
        nearest = l;
        nearest_attachment = i;
      }
    }
  }
  *attachment = nearest_attachment;
  *distance = nearest;
  return TRUE;
}

// Really need to be able to reset the shape! Otherwise, if the
// points ever go to zero, we've lost it, and can't resize.
void wxPolygonShape::SetSize(double new_width, double new_height, bool recursive)
{
  SetAttachmentSize(new_width, new_height);
  
  // Multiply all points by proportion of new size to old size
  double x_proportion = (double)(fabs(new_width/m_originalWidth));
  double y_proportion = (double)(fabs(new_height/m_originalHeight));

  wxNode *node = m_points->First();
  wxNode *original_node = m_originalPoints->First();
  while (node && original_node)
  {
    wxRealPoint *point = (wxRealPoint *)node->Data();
    wxRealPoint *original_point = (wxRealPoint *)original_node->Data();

    point->x = (original_point->x * x_proportion);
    point->y = (original_point->y * y_proportion);

    node = node->Next();
    original_node = original_node->Next();
  }

//  CalculateBoundingBox();
  m_boundWidth = (double)fabs(new_width);
  m_boundHeight = (double)fabs(new_height);
  SetDefaultRegionSize();
}

// Make the original points the same as the working points
void wxPolygonShape::UpdateOriginalPoints()
{
  if (!m_originalPoints) m_originalPoints = new wxList;
  wxNode *original_node = m_originalPoints->First();
  while (original_node)
  {
    wxNode *next_node = original_node->Next();
    wxRealPoint *original_point = (wxRealPoint *)original_node->Data();
    delete original_point;
    delete original_node;

    original_node = next_node;
  }

  wxNode *node = m_points->First();
  while (node)
  {
    wxRealPoint *point = (wxRealPoint *)node->Data();
    wxRealPoint *original_point = new wxRealPoint(point->x, point->y);
    m_originalPoints->Append((wxObject*) original_point);

    node = node->Next();
  }
  CalculateBoundingBox();
  m_originalWidth = m_boundWidth;
  m_originalHeight = m_boundHeight;
}

void wxPolygonShape::AddPolygonPoint(int pos)
{
  wxNode *node = m_points->Nth(pos);
  if (!node) node = m_points->First();
  wxRealPoint *firstPoint = (wxRealPoint *)node->Data();

  wxNode *node2 = m_points->Nth(pos + 1);
  if (!node2) node2 = m_points->First();
  wxRealPoint *secondPoint = (wxRealPoint *)node2->Data();

  double x = (double)((secondPoint->x - firstPoint->x)/2.0 + firstPoint->x);
  double y = (double)((secondPoint->y - firstPoint->y)/2.0 + firstPoint->y);
  wxRealPoint *point = new wxRealPoint(x, y);

  if (pos >= (m_points->Number() - 1))
    m_points->Append((wxObject*) point);
  else
    m_points->Insert(node2, (wxObject*) point);

  UpdateOriginalPoints();

  if (m_selected)
  {
    DeleteControlPoints();
    MakeControlPoints();
  }
}

void wxPolygonShape::DeletePolygonPoint(int pos)
{
  wxNode *node = m_points->Nth(pos);
  if (node)
  {
    wxRealPoint *point = (wxRealPoint *)node->Data();
    delete point;
    delete node;
    UpdateOriginalPoints();
    if (m_selected)
    {
      DeleteControlPoints();
      MakeControlPoints();
    }
  }
}

// Assume (x1, y1) is centre of box (most generally, line end at box)
bool wxPolygonShape::GetPerimeterPoint(double x1, double y1,
                                     double x2, double y2,
                                     double *x3, double *y3)
{
  int n = m_points->Number();

  // First check for situation where the line is vertical,
  // and we would want to connect to a point on that vertical --
  // oglFindEndForPolyline can't cope with this (the arrow
  // gets drawn to the wrong place).
  if ((!m_attachmentMode) && (x1 == x2))
  {
    // Look for the point we'd be connecting to. This is
    // a heuristic...
    wxNode *node = m_points->First();
    while (node)
    {
      wxRealPoint *point = (wxRealPoint *)node->Data();
      if (point->x == 0.0)
      {
        if ((y2 > y1) && (point->y > 0.0))
        {
          *x3 = point->x + m_xpos;
          *y3 = point->y + m_ypos;
          return TRUE;
        }
        else if ((y2 < y1) && (point->y < 0.0))
        {
          *x3 = point->x + m_xpos;
          *y3 = point->y + m_ypos;
          return TRUE;
        }
      }
      node = node->Next();
    }
  }
  
  double *xpoints = new double[n];
  double *ypoints = new double[n];

  wxNode *node = m_points->First();
  int i = 0;
  while (node)
  {
    wxRealPoint *point = (wxRealPoint *)node->Data();
    xpoints[i] = point->x + m_xpos;
    ypoints[i] = point->y + m_ypos;
    node = node->Next();
    i ++;
  }

  oglFindEndForPolyline(n, xpoints, ypoints, 
                        x1, y1, x2, y2, x3, y3);

  delete[] xpoints;
  delete[] ypoints;

  return TRUE;
}

void wxPolygonShape::OnDraw(wxDC& dc)
{
    int n = m_points->Number();
    wxPoint *intPoints = new wxPoint[n];
    int i;
    for (i = 0; i < n; i++)
    {
      wxRealPoint* point = (wxRealPoint*) m_points->Nth(i)->Data();
      intPoints[i].x = WXROUND(point->x);
      intPoints[i].y = WXROUND(point->y);
    }

    if (m_shadowMode != SHADOW_NONE)
    {
      if (m_shadowBrush)
        dc.SetBrush(m_shadowBrush);
      dc.SetPen(g_oglTransparentPen);

      dc.DrawPolygon(n, intPoints, WXROUND(m_xpos + m_shadowOffsetX), WXROUND(m_ypos + m_shadowOffsetY));
    }

    if (m_pen)
    {
      if (m_pen->GetWidth() == 0)
        dc.SetPen(g_oglTransparentPen);
      else
        dc.SetPen(m_pen);
    }
    if (m_brush)
      dc.SetBrush(m_brush);
    dc.DrawPolygon(n, intPoints, WXROUND(m_xpos), WXROUND(m_ypos));

    delete[] intPoints;
}

void wxPolygonShape::OnDrawOutline(wxDC& dc, double x, double y, double w, double h)
{
  dc.SetBrush(wxTRANSPARENT_BRUSH);
  // Multiply all points by proportion of new size to old size
  double x_proportion = (double)(fabs(w/m_originalWidth));
  double y_proportion = (double)(fabs(h/m_originalHeight));

  int n = m_originalPoints->Number();
  wxPoint *intPoints = new wxPoint[n];
  int i;
  for (i = 0; i < n; i++)
  {
    wxRealPoint* point = (wxRealPoint*) m_originalPoints->Nth(i)->Data();
    intPoints[i].x = WXROUND(x_proportion * point->x);
    intPoints[i].y = WXROUND(y_proportion * point->y);
  }
  dc.DrawPolygon(n, intPoints, WXROUND(x), WXROUND(y));
  delete[] intPoints;
}

// Make as many control points as there are vertices.
void wxPolygonShape::MakeControlPoints()
{
  wxNode *node = m_points->First();
  while (node)
  {
    wxRealPoint *point = (wxRealPoint *)node->Data();
    wxPolygonControlPoint *control = new wxPolygonControlPoint(m_canvas, this, CONTROL_POINT_SIZE,
      point, point->x, point->y);
    m_canvas->AddShape(control);
    m_controlPoints.Append(control);
    node = node->Next();
  }

/*
  double maxX, maxY, minX, minY;

  GetBoundingBoxMax(&maxX, &maxY);
  GetBoundingBoxMin(&minX, &minY);

  double widthMin = (double)(minX + CONTROL_POINT_SIZE + 2);
  double heightMin = (double)(minY + CONTROL_POINT_SIZE + 2);

  // Offsets from main object
  double top = (double)(- (heightMin / 2.0));
  double bottom = (double)(heightMin / 2.0 + (maxY - minY));
  double left = (double)(- (widthMin / 2.0));
  double right = (double)(widthMin / 2.0 + (maxX - minX));

  wxControlPoint *control = new wxControlPoint(m_canvas, this, CONTROL_POINT_SIZE, left, top, 
                                           CONTROL_POINT_DIAGONAL);
  m_canvas->AddShape(control);
  m_controlPoints.Append(control);

  control = new wxControlPoint(m_canvas, this, CONTROL_POINT_SIZE, 0, top, 
                                           CONTROL_POINT_VERTICAL);
  m_canvas->AddShape(control);
  m_controlPoints.Append(control);

  control = new wxControlPoint(m_canvas, this, CONTROL_POINT_SIZE, right, top, 
                                           CONTROL_POINT_DIAGONAL);
  m_canvas->AddShape(control);
  m_controlPoints.Append(control);

  control = new wxControlPoint(m_canvas, this, CONTROL_POINT_SIZE, right, 0, 
                                           CONTROL_POINT_HORIZONTAL);
  m_canvas->AddShape(control);
  m_controlPoints.Append(control);

  control = new wxControlPoint(m_canvas, this, CONTROL_POINT_SIZE, right, bottom, 
                                           CONTROL_POINT_DIAGONAL);
  m_canvas->AddShape(control);
  m_controlPoints.Append(control);

  control = new wxControlPoint(m_canvas, this, CONTROL_POINT_SIZE, 0, bottom, 
                                           CONTROL_POINT_VERTICAL);
  m_canvas->AddShape(control);
  m_controlPoints.Append(control);

  control = new wxControlPoint(m_canvas, this, CONTROL_POINT_SIZE, left, bottom, 
                                           CONTROL_POINT_DIAGONAL);
  m_canvas->AddShape(control);
  m_controlPoints.Append(control);

  control = new wxControlPoint(m_canvas, this, CONTROL_POINT_SIZE, left, 0, 
                                           CONTROL_POINT_HORIZONTAL);
  m_canvas->AddShape(control);
  m_controlPoints.Append(control);
*/
}

void wxPolygonShape::ResetControlPoints()
{
  wxNode *node = m_points->First();
  wxNode *controlPointNode = m_controlPoints.First();
  while (node && controlPointNode)
  {
    wxRealPoint *point = (wxRealPoint *)node->Data();
    wxPolygonControlPoint *controlPoint = (wxPolygonControlPoint *)controlPointNode->Data();

    controlPoint->m_xoffset = point->x;
    controlPoint->m_yoffset = point->y;
    controlPoint->m_polygonVertex = point;

    node = node->Next();
    controlPointNode = controlPointNode->Next();
  }
/*

  if (m_controlPoints.Number() < 1)
    return;

  double maxX, maxY, minX, minY;

  GetBoundingBoxMax(&maxX, &maxY);
  GetBoundingBoxMin(&minX, &minY);

  double widthMin = (double)(minX + CONTROL_POINT_SIZE + 2);
  double heightMin = (double)(minY + CONTROL_POINT_SIZE + 2);

  // Offsets from main object
  double top = (double)(- (heightMin / 2.0));
  double bottom = (double)(heightMin / 2.0 + (maxY - minY));
  double left = (double)(- (widthMin / 2.0));
  double right = (double)(widthMin / 2.0 + (maxX - minX));

  wxNode *node = m_controlPoints.First();
  wxControlPoint *control = (wxControlPoint *)node->Data();
  control->xoffset = left; control->yoffset = top;

  node = node->Next(); control = (wxControlPoint *)node->Data();
  control->xoffset = 0; control->yoffset = top;

  node = node->Next(); control = (wxControlPoint *)node->Data();
  control->xoffset = right; control->yoffset = top;

  node = node->Next(); control = (wxControlPoint *)node->Data();
  control->xoffset = right; control->yoffset = 0;

  node = node->Next(); control = (wxControlPoint *)node->Data();
  control->xoffset = right; control->yoffset = bottom;

  node = node->Next(); control = (wxControlPoint *)node->Data();
  control->xoffset = 0; control->yoffset = bottom;

  node = node->Next(); control = (wxControlPoint *)node->Data();
  control->xoffset = left; control->yoffset = bottom;

  node = node->Next(); control = (wxControlPoint *)node->Data();
  control->xoffset = left; control->yoffset = 0;
*/
}


#ifdef PROLOGIO
void wxPolygonShape::WritePrologAttributes(wxExpr *clause)
{
  wxShape::WritePrologAttributes(clause);

  clause->AddAttributeValue("x", m_xpos);
  clause->AddAttributeValue("y", m_ypos);

  // Make a list of lists for the coordinates
  wxExpr *list = new wxExpr(PrologList);
  wxNode *node = m_points->First();
  while (node)
  {
    wxRealPoint *point = (wxRealPoint *)node->Data();
    wxExpr *point_list = new wxExpr(PrologList);
    wxExpr *x_expr = new wxExpr((double)point->x);
    wxExpr *y_expr = new wxExpr((double)point->y);

    point_list->Append(x_expr);
    point_list->Append(y_expr);
    list->Append(point_list);

    node = node->Next();
  }
  clause->AddAttributeValue("points", list);

  // Save the original (unscaled) points
  list = new wxExpr(PrologList);
  node = m_originalPoints->First();
  while (node)
  {
    wxRealPoint *point = (wxRealPoint *)node->Data();
    wxExpr *point_list = new wxExpr(PrologList);
    wxExpr *x_expr = new wxExpr((double) point->x);
    wxExpr *y_expr = new wxExpr((double) point->y);
    point_list->Append(x_expr);
    point_list->Append(y_expr);
    list->Append(point_list);

    node = node->Next();
  }
  clause->AddAttributeValue("m_originalPoints", list);
}

void wxPolygonShape::ReadPrologAttributes(wxExpr *clause)
{
  wxShape::ReadPrologAttributes(clause);

  // Read a list of lists
  m_points = new wxList;
  m_originalPoints = new wxList;

  wxExpr *points_list = NULL;
  clause->AssignAttributeValue("points", &points_list);

  // If no points_list, don't crash!! Assume a diamond instead.
  double the_height = 100.0;
  double the_width = 100.0;
  if (!points_list)
  {
    wxRealPoint *point = new wxRealPoint(0.0, (-the_height/2));
    m_points->Append((wxObject*) point);

    point = new wxRealPoint((the_width/2), 0.0);
    m_points->Append((wxObject*) point);

    point = new wxRealPoint(0.0, (the_height/2));
    m_points->Append((wxObject*) point);

    point = new wxRealPoint((-the_width/2), 0.0);
    m_points->Append((wxObject*) point);

    point = new wxRealPoint(0.0, (-the_height/2));
    m_points->Append((wxObject*) point);
  }
  else
  {
    wxExpr *node = points_list->value.first;

    while (node)
    {
      wxExpr *xexpr = node->value.first;
      long x = xexpr->IntegerValue();

      wxExpr *yexpr = xexpr->next;
      long y = yexpr->IntegerValue();

      wxRealPoint *point = new wxRealPoint((double)x, (double)y);
      m_points->Append((wxObject*) point);

      node = node->next;
    }
  }

  points_list = NULL;
  clause->AssignAttributeValue("m_originalPoints", &points_list);

  // If no points_list, don't crash!! Assume a diamond instead.
  if (!points_list)
  {
    wxRealPoint *point = new wxRealPoint(0.0, (-the_height/2));
    m_originalPoints->Append((wxObject*) point);

    point = new wxRealPoint((the_width/2), 0.0);
    m_originalPoints->Append((wxObject*) point);

    point = new wxRealPoint(0.0, (the_height/2));
    m_originalPoints->Append((wxObject*) point);

    point = new wxRealPoint((-the_width/2), 0.0);
    m_originalPoints->Append((wxObject*) point);

    point = new wxRealPoint(0.0, (-the_height/2));
    m_originalPoints->Append((wxObject*) point);

    m_originalWidth = the_width;
    m_originalHeight = the_height;
  }
  else
  {
    wxExpr *node = points_list->value.first;
    double min_x = 1000;
    double min_y = 1000;
    double max_x = -1000;
    double max_y = -1000;
    while (node)
    {
      wxExpr *xexpr = node->value.first;
      long x = xexpr->IntegerValue();

      wxExpr *yexpr = xexpr->next;
      long y = yexpr->IntegerValue();

      wxRealPoint *point = new wxRealPoint((double)x, (double)y);
      m_originalPoints->Append((wxObject*) point);

      if (x < min_x)
        min_x = (double)x;
      if (y < min_y)
        min_y = (double)y;
      if (x > max_x)
        max_x = (double)x;
      if (y > max_y)
        max_y = (double)y;

      node = node->next;
    }
    m_originalWidth = max_x - min_x;
    m_originalHeight = max_y - min_y;
  }

  CalculateBoundingBox();
}
#endif

void wxPolygonShape::Copy(wxShape& copy)
{
  wxShape::Copy(copy);

  wxASSERT( copy.IsKindOf(CLASSINFO(wxPolygonShape)) );

  wxPolygonShape& polyCopy = (wxPolygonShape&) copy;

  polyCopy.ClearPoints();

  polyCopy.m_points = new wxList;
  polyCopy.m_originalPoints = new wxList;

  wxNode *node = m_points->First();
  while (node)
  {
    wxRealPoint *point = (wxRealPoint *)node->Data();
    wxRealPoint *new_point = new wxRealPoint(point->x, point->y);
    polyCopy.m_points->Append((wxObject*) new_point);
    node = node->Next();
  }
  node = m_originalPoints->First();
  while (node)
  {
    wxRealPoint *point = (wxRealPoint *)node->Data();
    wxRealPoint *new_point = new wxRealPoint(point->x, point->y);
    polyCopy.m_originalPoints->Append((wxObject*) new_point);
    node = node->Next();
  }
  polyCopy.m_boundWidth = m_boundWidth;
  polyCopy.m_boundHeight = m_boundHeight;
  polyCopy.m_originalWidth = m_originalWidth;
  polyCopy.m_originalHeight = m_originalHeight;
}

int wxPolygonShape::GetNumberOfAttachments()
{
  int maxN = (m_points ? (m_points->Number() - 1) : 0);
  wxNode *node = m_attachmentPoints.First();
  while (node)
  {
    wxAttachmentPoint *point = (wxAttachmentPoint *)node->Data();
    if (point->m_id > maxN)
      maxN = point->m_id;
    node = node->Next();
  }
  return maxN+1;;
}

bool wxPolygonShape::GetAttachmentPosition(int attachment, double *x, double *y,
                                         int nth, int no_arcs, wxLineShape *line)
{
  if (m_attachmentMode && m_points && attachment < m_points->Number())
  {
    wxRealPoint *point = (wxRealPoint *)m_points->Nth(attachment)->Data();
    *x = point->x + m_xpos;
    *y = point->y + m_ypos;
    return TRUE;
  }
  else
  { return wxShape::GetAttachmentPosition(attachment, x, y, nth, no_arcs, line); }
}

bool wxPolygonShape::AttachmentIsValid(int attachment)
{
  if (!m_points)
    return FALSE;
    
  if ((attachment >= 0) && (attachment < m_points->Number()))
    return TRUE;

  wxNode *node = m_attachmentPoints.First();
  while (node)
  {
    wxAttachmentPoint *point = (wxAttachmentPoint *)node->Data();
    if (point->m_id == attachment)
      return TRUE;
    node = node->Next();
  }
  return FALSE;
}

// Rectangle object

IMPLEMENT_DYNAMIC_CLASS(wxRectangleShape, wxShape)

wxRectangleShape::wxRectangleShape(double w, double h)
{
  m_width = w; m_height = h; m_cornerRadius = 0.0;
  SetDefaultRegionSize();
}

void wxRectangleShape::OnDraw(wxDC& dc)
{
    double x1 = (double)(m_xpos - m_width/2.0);
    double y1 = (double)(m_ypos - m_height/2.0);

    if (m_shadowMode != SHADOW_NONE)
    {
      if (m_shadowBrush)
        dc.SetBrush(m_shadowBrush);
      dc.SetPen(g_oglTransparentPen);

      if (m_cornerRadius != 0.0)
        dc.DrawRoundedRectangle(WXROUND(x1 + m_shadowOffsetX), WXROUND(y1 + m_shadowOffsetY),
                                 WXROUND(m_width), WXROUND(m_height), m_cornerRadius);
      else
        dc.DrawRectangle(WXROUND(x1 + m_shadowOffsetX), WXROUND(y1 + m_shadowOffsetY), WXROUND(m_width), WXROUND(m_height));
    }

    if (m_pen)
    {
      if (m_pen->GetWidth() == 0)
        dc.SetPen(g_oglTransparentPen);
      else
        dc.SetPen(m_pen);
    }
    if (m_brush)
      dc.SetBrush(m_brush);

    if (m_cornerRadius != 0.0)
      dc.DrawRoundedRectangle(WXROUND(x1), WXROUND(y1), WXROUND(m_width), WXROUND(m_height), m_cornerRadius);
    else
      dc.DrawRectangle(WXROUND(x1), WXROUND(y1), WXROUND(m_width), WXROUND(m_height));
}

void wxRectangleShape::GetBoundingBoxMin(double *the_width, double *the_height)
{
  *the_width = m_width;
  *the_height = m_height;
}

void wxRectangleShape::SetSize(double x, double y, bool recursive)
{
  SetAttachmentSize(x, y);
  m_width = (double)wxMax(x, 1.0);
  m_height = (double)wxMax(y, 1.0);
  SetDefaultRegionSize();
}

void wxRectangleShape::SetCornerRadius(double rad)
{
  m_cornerRadius = rad;
}

// Assume (x1, y1) is centre of box (most generally, line end at box)
bool wxRectangleShape::GetPerimeterPoint(double x1, double y1,
                                     double x2, double y2,
                                     double *x3, double *y3)
{
  double bound_x, bound_y;
  GetBoundingBoxMax(&bound_x, &bound_y);
  oglFindEndForBox(bound_x, bound_y, m_xpos, m_ypos, x2, y2, x3, y3);

  return TRUE;
}

#ifdef PROLOGIO
void wxRectangleShape::WritePrologAttributes(wxExpr *clause)
{
  wxShape::WritePrologAttributes(clause);
  clause->AddAttributeValue("x", m_xpos);
  clause->AddAttributeValue("y", m_ypos);

  clause->AddAttributeValue("width", m_width);
  clause->AddAttributeValue("height", m_height);
  if (m_cornerRadius != 0.0)
    clause->AddAttributeValue("corner", m_cornerRadius);
}

void wxRectangleShape::ReadPrologAttributes(wxExpr *clause)
{
  wxShape::ReadPrologAttributes(clause);
  clause->AssignAttributeValue("width", &m_width);
  clause->AssignAttributeValue("height", &m_height);
  clause->AssignAttributeValue("corner", &m_cornerRadius);

  // In case we're reading an old file, set the region's size
  if (m_regions.Number() == 1)
  {
    wxShapeRegion *region = (wxShapeRegion *)m_regions.First()->Data();
    region->SetSize(m_width, m_height);
  }
}
#endif

void wxRectangleShape::Copy(wxShape& copy)
{
  wxShape::Copy(copy);

  wxASSERT( copy.IsKindOf(CLASSINFO(wxRectangleShape)) );

  wxRectangleShape& rectCopy = (wxRectangleShape&) copy;
  rectCopy.m_width = m_width;
  rectCopy.m_height = m_height;
  rectCopy.m_cornerRadius = m_cornerRadius;
}

int wxRectangleShape::GetNumberOfAttachments()
{
  return wxShape::GetNumberOfAttachments();
}


// There are 4 attachment points on a rectangle - 0 = top, 1 = right, 2 = bottom,
// 3 = left.
bool wxRectangleShape::GetAttachmentPosition(int attachment, double *x, double *y,
                                         int nth, int no_arcs, wxLineShape *line)
{
  if (m_attachmentMode)
  {
    double top = (double)(m_ypos + m_height/2.0);
    double bottom = (double)(m_ypos - m_height/2.0);
    double left = (double)(m_xpos - m_width/2.0);
    double right = (double)(m_xpos + m_width/2.0);

    bool isEnd = (line && line->IsEnd(this));

    // Simplified code
    switch (attachment)
    {
      case 0:
      {
        wxRealPoint pt = CalcSimpleAttachment(wxRealPoint(left, bottom), wxRealPoint(right, bottom),
            nth, no_arcs, line);

        *x = pt.x; *y = pt.y;
        break;
      }
      case 1:
      {
        wxRealPoint pt = CalcSimpleAttachment(wxRealPoint(right, bottom), wxRealPoint(right, top),
            nth, no_arcs, line);

        *x = pt.x; *y = pt.y;
        break;
      }
      case 2:
      {
        wxRealPoint pt = CalcSimpleAttachment(wxRealPoint(left, top), wxRealPoint(right, top),
            nth, no_arcs, line);

        *x = pt.x; *y = pt.y;
        break;
      }
      case 3:
      {
        wxRealPoint pt = CalcSimpleAttachment(wxRealPoint(left, bottom), wxRealPoint(left, top),
            nth, no_arcs, line);

        *x = pt.x; *y = pt.y;
        break;
      }
      default:
      {
        return wxShape::GetAttachmentPosition(attachment, x, y, nth, no_arcs, line);
        break;
      }
    }

    // Old code
#if 0
    switch (attachment)
    {
      case 0:
      {
        if (m_spaceAttachments)
        {
          if (line && (line->GetAlignmentType(isEnd) == LINE_ALIGNMENT_TO_NEXT_HANDLE))
          {
            // Align line according to the next handle along
            wxRealPoint *point = line->GetNextControlPoint(this);
            if (point->x < left)
              *x = left;
            else if (point->x > right)
              *x = right;
            else
              *x = point->x;
          }
          else
            *x = left + (nth + 1)*m_width/(no_arcs + 1);
        }
        else *x = m_xpos;

        *y = bottom;
        break;
      }
      case 1:
      {
        *x = right;
        if (m_spaceAttachments)
        {
          if (line && (line->GetAlignmentType(isEnd) == LINE_ALIGNMENT_TO_NEXT_HANDLE))
          {
            // Align line according to the next handle along
            wxRealPoint *point = line->GetNextControlPoint(this);
            if (point->y < bottom)
              *y = bottom;
            else if (point->y > top)
              *y = top;
            else
              *y = point->y;
          }
          else
            *y = bottom + (nth + 1)*m_height/(no_arcs + 1);
        }
        else *y = m_ypos;
        break;
      }
      case 2:
      {
        if (m_spaceAttachments)
        {
          if (line && (line->GetAlignmentType(isEnd) == LINE_ALIGNMENT_TO_NEXT_HANDLE))
          {
            // Align line according to the next handle along
            wxRealPoint *point = line->GetNextControlPoint(this);
            if (point->x < left)
              *x = left;
            else if (point->x > right)
              *x = right;
            else
              *x = point->x;
          }
          else
            *x = left + (nth + 1)*m_width/(no_arcs + 1);
        }
        else *x = m_xpos;
        *y = top;
        break;
      }
      case 3:
      {
        *x = left;
        if (m_spaceAttachments)
        {
          if (line && (line->GetAlignmentType(isEnd) == LINE_ALIGNMENT_TO_NEXT_HANDLE))
          {
            // Align line according to the next handle along
            wxRealPoint *point = line->GetNextControlPoint(this);
            if (point->y < bottom)
              *y = bottom;
            else if (point->y > top)
              *y = top;
            else
              *y = point->y;
          }
          else
            *y = bottom + (nth + 1)*m_height/(no_arcs + 1);
        }
        else *y = m_ypos;
        break;
      }
      default:
      {
        return wxShape::GetAttachmentPosition(attachment, x, y, nth, no_arcs, line);
        break;
      }
    }
#endif
    return TRUE;
  }
  else
  { *x = m_xpos; *y = m_ypos; return TRUE; }
}

// Text object (no box)

IMPLEMENT_DYNAMIC_CLASS(wxTextShape, wxRectangleShape)

wxTextShape::wxTextShape(double width, double height):
  wxRectangleShape(width, height)
{
}

void wxTextShape::OnDraw(wxDC& dc)
{
}

void wxTextShape::Copy(wxShape& copy)
{
  wxRectangleShape::Copy(copy);
}

#ifdef PROLOGIO
void wxTextShape::WritePrologAttributes(wxExpr *clause)
{
  wxRectangleShape::WritePrologAttributes(clause);
}
#endif

// Ellipse object

IMPLEMENT_DYNAMIC_CLASS(wxEllipseShape, wxShape)

wxEllipseShape::wxEllipseShape(double w, double h)
{
  m_width = w; m_height = h;
  SetDefaultRegionSize();
}

void wxEllipseShape::GetBoundingBoxMin(double *w, double *h)
{
  *w = m_width; *h = m_height;
}

bool wxEllipseShape::GetPerimeterPoint(double x1, double y1,
                                      double x2, double y2,
                                      double *x3, double *y3)
{
  double bound_x, bound_y;
  GetBoundingBoxMax(&bound_x, &bound_y);

//  oglFindEndForBox(bound_x, bound_y, m_xpos, m_ypos, x2, y2, x3, y3);
  oglDrawArcToEllipse(m_xpos, m_ypos, bound_x, bound_y, x2, y2, x1, y1, x3, y3);

  return TRUE;
}

void wxEllipseShape::OnDraw(wxDC& dc)
{
    if (m_shadowMode != SHADOW_NONE)
    {
      if (m_shadowBrush)
        dc.SetBrush(m_shadowBrush);
      dc.SetPen(g_oglTransparentPen);
      dc.DrawEllipse((m_xpos - GetWidth()/2) + m_shadowOffsetX,
                      (m_ypos - GetHeight()/2) + m_shadowOffsetY,
                      GetWidth(), GetHeight());
    }

    if (m_pen)
    {
      if (m_pen->GetWidth() == 0)
        dc.SetPen(g_oglTransparentPen);
      else
        dc.SetPen(m_pen);
    }
    if (m_brush)
      dc.SetBrush(m_brush);
    dc.DrawEllipse((m_xpos - GetWidth()/2), (m_ypos - GetHeight()/2), GetWidth(), GetHeight());
}

void wxEllipseShape::SetSize(double x, double y, bool recursive)
{
  SetAttachmentSize(x, y);
  m_width = x;
  m_height = y;
  SetDefaultRegionSize();
}

#ifdef PROLOGIO
void wxEllipseShape::WritePrologAttributes(wxExpr *clause)
{
  wxShape::WritePrologAttributes(clause);
  clause->AddAttributeValue("x", m_xpos);
  clause->AddAttributeValue("y", m_ypos);

  clause->AddAttributeValue("width", m_width);
  clause->AddAttributeValue("height", m_height);
}

void wxEllipseShape::ReadPrologAttributes(wxExpr *clause)
{
  wxShape::ReadPrologAttributes(clause);
  clause->AssignAttributeValue("width", &m_width);
  clause->AssignAttributeValue("height", &m_height);

  // In case we're reading an old file, set the region's size
  if (m_regions.Number() == 1)
  {
    wxShapeRegion *region = (wxShapeRegion *)m_regions.First()->Data();
    region->SetSize(m_width, m_height);
  }
}
#endif

void wxEllipseShape::Copy(wxShape& copy)
{
  wxShape::Copy(copy);

  wxASSERT( copy.IsKindOf(CLASSINFO(wxEllipseShape)) );

  wxEllipseShape& ellipseCopy = (wxEllipseShape&) copy;

  ellipseCopy.m_width = m_width;
  ellipseCopy.m_height = m_height;
}

int wxEllipseShape::GetNumberOfAttachments()
{
  return wxShape::GetNumberOfAttachments();
}

// There are 4 attachment points on an ellipse - 0 = top, 1 = right, 2 = bottom,
// 3 = left.
bool wxEllipseShape::GetAttachmentPosition(int attachment, double *x, double *y,
                                         int nth, int no_arcs, wxLineShape *line)
{
  if (m_attachmentMode)
  {
    double top = (double)(m_ypos + m_height/2.0);
    double bottom = (double)(m_ypos - m_height/2.0);
    double left = (double)(m_xpos - m_width/2.0);
    double right = (double)(m_xpos + m_width/2.0);
    switch (attachment)
    {
      case 0:
      {
        if (m_spaceAttachments)
          *x = left + (nth + 1)*m_width/(no_arcs + 1);
        else *x = m_xpos;
        *y = top;
        // We now have the point on the bounding box: but get the point on the ellipse
        // by imagining a vertical line from (*x, m_ypos - m_height- 500) to (*x, m_ypos) intersecting
        // the ellipse.
        oglDrawArcToEllipse(m_xpos, m_ypos, m_width, m_height, *x, (double)(m_ypos-m_height-500), *x, m_ypos, x, y);
        break;
      }
      case 1:
      {
        *x = right;
        if (m_spaceAttachments)
          *y = bottom + (nth + 1)*m_height/(no_arcs + 1);
        else *y = m_ypos;
        oglDrawArcToEllipse(m_xpos, m_ypos, m_width, m_height, (double)(m_xpos+m_width+500), *y, m_xpos, *y, x, y);
        break;
      }
      case 2:
      {
        if (m_spaceAttachments)
          *x = left + (nth + 1)*m_width/(no_arcs + 1);
        else *x = m_xpos;
        *y = bottom;
        oglDrawArcToEllipse(m_xpos, m_ypos, m_width, m_height, *x, (double)(m_ypos+m_height+500), *x, m_ypos, x, y);
        break;
      }
      case 3:
      {
        *x = left;
        if (m_spaceAttachments)
          *y = bottom + (nth + 1)*m_height/(no_arcs + 1);
        else *y = m_ypos;
        oglDrawArcToEllipse(m_xpos, m_ypos, m_width, m_height, (double)(m_xpos-m_width-500), *y, m_xpos, *y, x, y);
        break;
      }
      default:
      {
        return wxShape::GetAttachmentPosition(attachment, x, y, nth, no_arcs, line);
        break;
      }
    }
    return TRUE;
  }
  else
  { *x = m_xpos; *y = m_ypos; return TRUE; }
}


// Circle object
IMPLEMENT_DYNAMIC_CLASS(wxCircleShape, wxEllipseShape)

wxCircleShape::wxCircleShape(double diameter):wxEllipseShape(diameter, diameter)
{
}

void wxCircleShape::Copy(wxShape& copy)
{
  wxEllipseShape::Copy(copy);
}

bool wxCircleShape::GetPerimeterPoint(double x1, double y1,
                                      double x2, double y2,
                                      double *x3, double *y3)
{
  oglFindEndForCircle(m_width/2, 
                      m_xpos, m_ypos,  // Centre of circle
                      x2, y2,  // Other end of line
                      x3, y3);

  return TRUE;
}

// Control points

double wxControlPoint::controlPointDragStartX = 0.0;
double wxControlPoint::controlPointDragStartY = 0.0;
double wxControlPoint::controlPointDragStartWidth = 0.0;
double wxControlPoint::controlPointDragStartHeight = 0.0;
double wxControlPoint::controlPointDragEndWidth = 0.0;
double wxControlPoint::controlPointDragEndHeight = 0.0;
double wxControlPoint::controlPointDragPosX = 0.0;
double wxControlPoint::controlPointDragPosY = 0.0;

IMPLEMENT_DYNAMIC_CLASS(wxControlPoint, wxRectangleShape)

wxControlPoint::wxControlPoint(wxShapeCanvas *theCanvas, wxShape *object, double size, double the_xoffset, double the_yoffset, int the_type):wxRectangleShape(size, size)
{
  m_canvas = theCanvas;
  m_shape = object;
  m_xoffset = the_xoffset;
  m_yoffset = the_yoffset;
  m_type = the_type;
  SetPen(g_oglBlackForegroundPen);
  SetBrush(wxBLACK_BRUSH);
  m_oldCursor = NULL;
  m_visible = TRUE;
  m_eraseObject = TRUE;
}

wxControlPoint::~wxControlPoint()
{
}

// Don't even attempt to draw any text - waste of time!
void wxControlPoint::OnDrawContents(wxDC& dc)
{
}

void wxControlPoint::OnDraw(wxDC& dc)
{
  m_xpos = m_shape->GetX() + m_xoffset;
  m_ypos = m_shape->GetY() + m_yoffset;
  wxRectangleShape::OnDraw(dc);
}

void wxControlPoint::OnErase(wxDC& dc)
{
  wxRectangleShape::OnErase(dc);
}

// Implement resizing of canvas object
void wxControlPoint::OnDragLeft(bool draw, double x, double y, int keys, int attachment)
{
    m_shape->GetEventHandler()->OnSizingDragLeft(this, draw, x, y, keys, attachment);
}

void wxControlPoint::OnBeginDragLeft(double x, double y, int keys, int attachment)
{
    m_shape->GetEventHandler()->OnSizingBeginDragLeft(this, x, y, keys, attachment);
}

void wxControlPoint::OnEndDragLeft(double x, double y, int keys, int attachment)
{
    m_shape->GetEventHandler()->OnSizingEndDragLeft(this, x, y, keys, attachment);
}

int wxControlPoint::GetNumberOfAttachments()
{
  return 1;
}

bool wxControlPoint::GetAttachmentPosition(int attachment, double *x, double *y,
                                         int nth, int no_arcs, wxLineShape *line)
{
  *x = m_xpos; *y = m_ypos;
  return TRUE;
}

// Control points ('handles') redirect control to the actual shape, to make it easier
// to override sizing behaviour.
void wxShape::OnSizingDragLeft(wxControlPoint* pt, bool draw, double x, double y, int keys, int attachment)
{
  double bound_x;
  double bound_y;
  this->GetBoundingBoxMin(&bound_x, &bound_y);

  wxClientDC dc(GetCanvas());
  GetCanvas()->PrepareDC(dc);

  dc.SetLogicalFunction(wxXOR);

  wxPen dottedPen(wxColour(0, 0, 0), 1, wxDOT);
  dc.SetPen(dottedPen);
  dc.SetBrush((* wxTRANSPARENT_BRUSH));

  if (this->GetCentreResize())
  {
    // Maintain the same centre point.
    double new_width = (double)(2.0*fabs(x - this->GetX()));
    double new_height = (double)(2.0*fabs(y - this->GetY()));

    // Constrain sizing according to what control point you're dragging
    if (pt->m_type == CONTROL_POINT_HORIZONTAL)
      new_height = bound_y;
    else if (pt->m_type == CONTROL_POINT_VERTICAL)
      new_width = bound_x;
    else if (pt->m_type == CONTROL_POINT_DIAGONAL && (keys & KEY_SHIFT))
      new_height = bound_y*(new_width/bound_x);

    if (this->GetFixedWidth())
      new_width = bound_x;
      
    if (this->GetFixedHeight())
      new_height = bound_y;

    pt->controlPointDragEndWidth = new_width;
    pt->controlPointDragEndHeight = new_height;

    this->GetEventHandler()->OnDrawOutline(dc, this->GetX(), this->GetY(),
                                new_width, new_height);
  }
  else
  {
    // Don't maintain the same centre point!
    double newX1 = wxMin(pt->controlPointDragStartX, x);
    double newY1 = wxMin(pt->controlPointDragStartY, y);
    double newX2 = wxMax(pt->controlPointDragStartX, x);
    double newY2 = wxMax(pt->controlPointDragStartY, y);
    if (pt->m_type == CONTROL_POINT_HORIZONTAL)
    {
      newY1 = pt->controlPointDragStartY;
      newY2 = newY1 + pt->controlPointDragStartHeight;
    }
    else if (pt->m_type == CONTROL_POINT_VERTICAL)
    {
      newX1 = pt->controlPointDragStartX;
      newX2 = newX1 + pt->controlPointDragStartWidth;
    }
    else if (pt->m_type == CONTROL_POINT_DIAGONAL && (keys & KEY_SHIFT))
    {
      double newH = (double)((newX2 - newX1)*(pt->controlPointDragStartHeight/pt->controlPointDragStartWidth));
      if (GetY() > pt->controlPointDragStartY)
        newY2 = (double)(newY1 + newH);
      else
        newY1 = (double)(newY2 - newH);
    }
    double newWidth = (double)(newX2 - newX1);
    double newHeight = (double)(newY2 - newY1);

    pt->controlPointDragPosX = (double)(newX1 + (newWidth/2.0));
    pt->controlPointDragPosY = (double)(newY1 + (newHeight/2.0));
    if (this->GetFixedWidth())
      newWidth = bound_x;
     
    if (this->GetFixedHeight())
      newHeight = bound_y;

    pt->controlPointDragEndWidth = newWidth;
    pt->controlPointDragEndHeight = newHeight;
    this->GetEventHandler()->OnDrawOutline(dc, pt->controlPointDragPosX, pt->controlPointDragPosY, newWidth, newHeight);
  }
}

void wxShape::OnSizingBeginDragLeft(wxControlPoint* pt, double x, double y, int keys, int attachment)
{
  m_canvas->CaptureMouse();

  wxClientDC dc(GetCanvas());
  GetCanvas()->PrepareDC(dc);

  if (pt->m_eraseObject)
    this->Erase(dc);

  dc.SetLogicalFunction(wxXOR);

  double bound_x;
  double bound_y;
  this->GetBoundingBoxMin(&bound_x, &bound_y);

  // Choose the 'opposite corner' of the object as the stationary
  // point in case this is non-centring resizing.
  if (pt->GetX() < this->GetX())
    pt->controlPointDragStartX = (double)(this->GetX() + (bound_x/2.0));
  else
    pt->controlPointDragStartX = (double)(this->GetX() - (bound_x/2.0));

  if (pt->GetY() < this->GetY())
    pt->controlPointDragStartY = (double)(this->GetY() + (bound_y/2.0));
  else
    pt->controlPointDragStartY = (double)(this->GetY() - (bound_y/2.0));

  if (pt->m_type == CONTROL_POINT_HORIZONTAL)
    pt->controlPointDragStartY = (double)(this->GetY() - (bound_y/2.0));
  else if (pt->m_type == CONTROL_POINT_VERTICAL)
    pt->controlPointDragStartX = (double)(this->GetX() - (bound_x/2.0));

  // We may require the old width and height.
  pt->controlPointDragStartWidth = bound_x;
  pt->controlPointDragStartHeight = bound_y;

  wxPen dottedPen(wxColour(0, 0, 0), 1, wxDOT);
  dc.SetPen(dottedPen);
  dc.SetBrush((* wxTRANSPARENT_BRUSH));

  if (this->GetCentreResize())
  {
    double new_width = (double)(2.0*fabs(x - this->GetX()));
    double new_height = (double)(2.0*fabs(y - this->GetY()));

    // Constrain sizing according to what control point you're dragging
    if (pt->m_type == CONTROL_POINT_HORIZONTAL)
      new_height = bound_y;
    else if (pt->m_type == CONTROL_POINT_VERTICAL)
      new_width = bound_x;
    else if (pt->m_type == CONTROL_POINT_DIAGONAL && (keys & KEY_SHIFT))
      new_height = bound_y*(new_width/bound_x);

    if (this->GetFixedWidth())
      new_width = bound_x;
      
    if (this->GetFixedHeight())
      new_height = bound_y;

    pt->controlPointDragEndWidth = new_width;
    pt->controlPointDragEndHeight = new_height;
    this->GetEventHandler()->OnDrawOutline(dc, this->GetX(), this->GetY(),
                                new_width, new_height);
  }
  else
  {
    // Don't maintain the same centre point!
    double newX1 = wxMin(pt->controlPointDragStartX, x);
    double newY1 = wxMin(pt->controlPointDragStartY, y);
    double newX2 = wxMax(pt->controlPointDragStartX, x);
    double newY2 = wxMax(pt->controlPointDragStartY, y);
    if (pt->m_type == CONTROL_POINT_HORIZONTAL)
    {
      newY1 = pt->controlPointDragStartY;
      newY2 = newY1 + pt->controlPointDragStartHeight;
    }
    else if (pt->m_type == CONTROL_POINT_VERTICAL)
    {
      newX1 = pt->controlPointDragStartX;
      newX2 = newX1 + pt->controlPointDragStartWidth;
    }
    else if (pt->m_type == CONTROL_POINT_DIAGONAL && (keys & KEY_SHIFT))
    {
      double newH = (double)((newX2 - newX1)*(pt->controlPointDragStartHeight/pt->controlPointDragStartWidth));
      if (pt->GetY() > pt->controlPointDragStartY)
        newY2 = (double)(newY1 + newH);
      else
        newY1 = (double)(newY2 - newH);
    }
    double newWidth = (double)(newX2 - newX1);
    double newHeight = (double)(newY2 - newY1);

    pt->controlPointDragPosX = (double)(newX1 + (newWidth/2.0));
    pt->controlPointDragPosY = (double)(newY1 + (newHeight/2.0));
    if (this->GetFixedWidth())
      newWidth = bound_x;

    if (this->GetFixedHeight())
      newHeight = bound_y;

    pt->controlPointDragEndWidth = newWidth;
    pt->controlPointDragEndHeight = newHeight;
    this->GetEventHandler()->OnDrawOutline(dc, pt->controlPointDragPosX, pt->controlPointDragPosY, newWidth, newHeight);
  }
}

void wxShape::OnSizingEndDragLeft(wxControlPoint* pt, double x, double y, int keys, int attachment)
{
  wxClientDC dc(GetCanvas());
  GetCanvas()->PrepareDC(dc);

  m_canvas->ReleaseMouse();
  dc.SetLogicalFunction(wxCOPY);
  this->Recompute();
  this->ResetControlPoints();

  if (!pt->m_eraseObject)
    this->Show(FALSE);

  this->SetSize(pt->controlPointDragEndWidth, pt->controlPointDragEndHeight);

  // The next operation could destroy this control point (it does for label objects,
  // via formatting the text), so save all values we're going to use, or
  // we'll be accessing garbage.
  wxShape *theObject = this;
  wxShapeCanvas *theCanvas = m_canvas;
  bool eraseIt = pt->m_eraseObject;

  if (theObject->GetCentreResize())
    theObject->Move(dc, theObject->GetX(), theObject->GetY());
  else
    theObject->Move(dc, pt->controlPointDragPosX, pt->controlPointDragPosY);

  if (!eraseIt)
    theObject->Show(TRUE);
    
  // Recursively redraw links if we have a composite.
  if (theObject->GetChildren().Number() > 0)
    theObject->DrawLinks(dc, -1, TRUE);
    
  double width, height;
  theObject->GetBoundingBoxMax(&width, &height);
  theObject->GetEventHandler()->OnEndSize(width, height);

  if (!theCanvas->GetQuickEditMode() && eraseIt) theCanvas->Redraw(dc);
}



// Polygon control points

IMPLEMENT_DYNAMIC_CLASS(wxPolygonControlPoint, wxControlPoint)

wxPolygonControlPoint::wxPolygonControlPoint(wxShapeCanvas *theCanvas, wxShape *object, double size,
  wxRealPoint *vertex, double the_xoffset, double the_yoffset):
  wxControlPoint(theCanvas, object, size, the_xoffset, the_yoffset, 0)
{
  m_polygonVertex = vertex;
  m_originalDistance = 0.0;
}

wxPolygonControlPoint::~wxPolygonControlPoint()
{
}

// Calculate what new size would be, at end of resize
void wxPolygonControlPoint::CalculateNewSize(double x, double y)
{
  double bound_x;
  double bound_y;
  GetShape()->GetBoundingBoxMin(&bound_x, &bound_y);

  double dist = (double)sqrt((x - m_shape->GetX())*(x - m_shape->GetX()) +
                    (y - m_shape->GetY())*(y - m_shape->GetY()));

  m_newSize.x = (double)(dist/this->m_originalDistance)*this->m_originalSize.x;
  m_newSize.y = (double)(dist/this->m_originalDistance)*this->m_originalSize.y;
}


// Implement resizing polygon or moving the vertex.
void wxPolygonControlPoint::OnDragLeft(bool draw, double x, double y, int keys, int attachment)
{
    m_shape->GetEventHandler()->OnSizingDragLeft(this, draw, x, y, keys, attachment);
}

void wxPolygonControlPoint::OnBeginDragLeft(double x, double y, int keys, int attachment)
{
    m_shape->GetEventHandler()->OnSizingBeginDragLeft(this, x, y, keys, attachment);
}

void wxPolygonControlPoint::OnEndDragLeft(double x, double y, int keys, int attachment)
{
    m_shape->GetEventHandler()->OnSizingEndDragLeft(this, x, y, keys, attachment);
}

// Control points ('handles') redirect control to the actual shape, to make it easier
// to override sizing behaviour.
void wxPolygonShape::OnSizingDragLeft(wxControlPoint* pt, bool draw, double x, double y, int keys, int attachment)
{
  wxPolygonControlPoint* ppt = (wxPolygonControlPoint*) pt;

  wxClientDC dc(GetCanvas());
  GetCanvas()->PrepareDC(dc);

  dc.SetLogicalFunction(wxXOR);

  wxPen dottedPen(wxColour(0, 0, 0), 1, wxDOT);
  dc.SetPen(dottedPen);
  dc.SetBrush((* wxTRANSPARENT_BRUSH));

  if (0) // keys & KEY_CTRL)
  {
    // TODO: mend this code. Currently we rely on altering the
    // actual points, but we should assume we're not, as per
    // the normal sizing case.
    m_canvas->Snap(&x, &y);

    // Move point
    ppt->m_polygonVertex->x = x - this->GetX();
    ppt->m_polygonVertex->y = y - this->GetY();
    ppt->SetX(x);
    ppt->SetY(y);
    ((wxPolygonShape *)this)->CalculateBoundingBox();
    ((wxPolygonShape *)this)->CalculatePolygonCentre();
  }
  else
  {
    ppt->CalculateNewSize(x, y);
  }

  this->GetEventHandler()->OnDrawOutline(dc, this->GetX(), this->GetY(),
       ppt->GetNewSize().x, ppt->GetNewSize().y);
}

void wxPolygonShape::OnSizingBeginDragLeft(wxControlPoint* pt, double x, double y, int keys, int attachment)
{
  wxPolygonControlPoint* ppt = (wxPolygonControlPoint*) pt;

  wxClientDC dc(GetCanvas());
  GetCanvas()->PrepareDC(dc);

  this->Erase(dc);

  dc.SetLogicalFunction(wxXOR);

  double bound_x;
  double bound_y;
  this->GetBoundingBoxMin(&bound_x, &bound_y);

  double dist = (double)sqrt((x - this->GetX())*(x - this->GetX()) +
                    (y - this->GetY())*(y - this->GetY()));
  ppt->m_originalDistance = dist;
  ppt->m_originalSize.x = bound_x;
  ppt->m_originalSize.y = bound_y;

  if (ppt->m_originalDistance == 0.0) ppt->m_originalDistance = (double) 0.0001;

  wxPen dottedPen(wxColour(0, 0, 0), 1, wxDOT);
  dc.SetPen(dottedPen);
  dc.SetBrush((* wxTRANSPARENT_BRUSH));

  if (0) // keys & KEY_CTRL)
  {
    // TODO: mend this code. Currently we rely on altering the
    // actual points, but we should assume we're not, as per
    // the normal sizing case.
    m_canvas->Snap(&x, &y);

    // Move point
    ppt->m_polygonVertex->x = x - this->GetX();
    ppt->m_polygonVertex->y = y - this->GetY();
    ppt->SetX(x);
    ppt->SetY(y);
    ((wxPolygonShape *)this)->CalculateBoundingBox();
    ((wxPolygonShape *)this)->CalculatePolygonCentre();
  }
  else
  {
    ppt->CalculateNewSize(x, y);
  }

  this->GetEventHandler()->OnDrawOutline(dc, this->GetX(), this->GetY(),
       ppt->GetNewSize().x, ppt->GetNewSize().y);

  m_canvas->CaptureMouse();
}

void wxPolygonShape::OnSizingEndDragLeft(wxControlPoint* pt, double x, double y, int keys, int attachment)
{
  wxPolygonControlPoint* ppt = (wxPolygonControlPoint*) pt;

  wxClientDC dc(GetCanvas());
  GetCanvas()->PrepareDC(dc);

  m_canvas->ReleaseMouse();
  dc.SetLogicalFunction(wxCOPY);

  // If we're changing shape, must reset the original points
  if (keys & KEY_CTRL)
  {
    ((wxPolygonShape *)this)->CalculateBoundingBox();
    ((wxPolygonShape *)this)->UpdateOriginalPoints();
  }
  else
  {
    SetSize(ppt->GetNewSize().x, ppt->GetNewSize().y);
  }

  ((wxPolygonShape *)this)->CalculateBoundingBox();
  ((wxPolygonShape *)this)->CalculatePolygonCentre();

  this->Recompute();
  this->ResetControlPoints();
  this->Move(dc, this->GetX(), this->GetY());
  if (!m_canvas->GetQuickEditMode()) m_canvas->Redraw(dc);
}

/*
 * Object region
 *
 */
IMPLEMENT_DYNAMIC_CLASS(wxShapeRegion, wxObject)

wxShapeRegion::wxShapeRegion()
{
  m_regionText = "";
  m_font = g_oglNormalFont;
  m_minHeight = 5.0;
  m_minWidth = 5.0;
  m_width = 0.0;
  m_height = 0.0;
  m_x = 0.0;
  m_y = 0.0;

  m_regionProportionX = -1.0;
  m_regionProportionY = -1.0;
  m_formatMode = FORMAT_CENTRE_HORIZ | FORMAT_CENTRE_VERT;
  m_regionName = "";
  m_textColour = "BLACK";
  m_penColour = "BLACK";
  m_penStyle = wxSOLID;
  m_actualColourObject = NULL;
  m_actualPenObject = NULL;
}

wxShapeRegion::wxShapeRegion(wxShapeRegion& region)
{
  m_regionText = region.m_regionText;
  m_regionName = region.m_regionName;
  m_textColour = region.m_textColour;

  m_font = region.m_font;
  m_minHeight = region.m_minHeight;
  m_minWidth = region.m_minWidth;
  m_width = region.m_width;
  m_height = region.m_height;
  m_x = region.m_x;
  m_y = region.m_y;

  m_regionProportionX = region.m_regionProportionX;
  m_regionProportionY = region.m_regionProportionY;
  m_formatMode = region.m_formatMode;
  m_actualColourObject = NULL;
  m_actualPenObject = NULL;
  m_penStyle = region.m_penStyle;
  m_penColour = region.m_penColour;

  ClearText();
  wxNode *node = region.m_formattedText.First();
  while (node)
  {
    wxShapeTextLine *line = (wxShapeTextLine *)node->Data();
    wxShapeTextLine *new_line =
      new wxShapeTextLine(line->GetX(), line->GetY(), line->GetText());
    m_formattedText.Append(new_line);
    node = node->Next();
  }
}

wxShapeRegion::~wxShapeRegion()
{
  ClearText();
}

void wxShapeRegion::ClearText()
{
  wxNode *node = m_formattedText.First();
  while (node)
  {
    wxShapeTextLine *line = (wxShapeTextLine *)node->Data();
    wxNode *next = node->Next();
    delete line;
    delete node;
    node = next;
  }
}

void wxShapeRegion::SetFont(wxFont *f)
{
  m_font = f;
}

void wxShapeRegion::SetMinSize(double w, double h)
{
  m_minWidth = w;
  m_minHeight = h;
}

void wxShapeRegion::SetSize(double w, double h)
{
  m_width = w;
  m_height = h;
}

void wxShapeRegion::SetPosition(double xp, double yp)
{
  m_x = xp;
  m_y = yp;
}

void wxShapeRegion::SetProportions(double xp, double yp)
{
  m_regionProportionX = xp;
  m_regionProportionY = yp;
}

void wxShapeRegion::SetFormatMode(int mode)
{
  m_formatMode = mode;
}

void wxShapeRegion::SetColour(const wxString& col)
{
  m_textColour = col;
  m_actualColourObject = NULL;
}

wxColour *wxShapeRegion::GetActualColourObject()
{
  if (!m_actualColourObject)
    m_actualColourObject = wxTheColourDatabase->FindColour(GetColour());
  if (!m_actualColourObject)
    m_actualColourObject = wxBLACK;
  return m_actualColourObject;
}

void wxShapeRegion::SetPenColour(const wxString& col)
{
  m_penColour = col;
  m_actualPenObject = NULL;
}

// Returns NULL if the pen is invisible
// (different to pen being transparent; indicates that
// region boundary should not be drawn.)
wxPen *wxShapeRegion::GetActualPen()
{
  if (m_actualPenObject)
    return m_actualPenObject;

  if (!m_penColour) return NULL;
  if (m_penColour == "Invisible")
    return NULL;
  m_actualPenObject = wxThePenList->FindOrCreatePen(m_penColour, 1, m_penStyle);
  return m_actualPenObject;
}


