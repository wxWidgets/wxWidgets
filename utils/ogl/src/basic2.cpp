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
  }
}


// Width and height. Centre of object is centre of box.
void wxPolygonShape::GetBoundingBoxMin(float *width, float *height)
{
  *width = m_boundWidth;
  *height = m_boundHeight;
}

void wxPolygonShape::CalculateBoundingBox()
{
  // Calculate bounding box at construction (and presumably resize) time
  float left = 10000;
  float right = -10000;
  float top = 10000;
  float bottom = -10000;

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
  float left = 10000;
  float right = -10000;
  float top = 10000;
  float bottom = -10000;

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
  float bwidth = right - left;
  float bheight = bottom - top;
  
  float newCentreX = (float)(left + (bwidth/2.0));
  float newCentreY = (float)(top + (bheight/2.0));

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

bool PolylineHitTest(float n, float xvec[], float yvec[],
                           float x1, float y1, float x2, float y2)
{
  bool isAHit = FALSE;
  int i;
  float lastx = xvec[0];
  float lasty = yvec[0];

  float min_ratio = 1.0;
  float line_ratio;
  float other_ratio;

//  char buf[300];
  for (i = 1; i < n; i++)
  {
    check_line_intersection(x1, y1, x2, y2, lastx, lasty, xvec[i], yvec[i],
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

  // Do last (implicit) line if last and first floats are not identical
  if (!(xvec[0] == lastx && yvec[0] == lasty))
  {
    check_line_intersection(x1, y1, x2, y2, lastx, lasty, xvec[0], yvec[0],
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

bool wxPolygonShape::HitTest(float x, float y, int *attachment, float *distance)
{
  // Imagine four lines radiating from this point. If all of these lines hit the polygon,
  // we're inside it, otherwise we're not. Obviously we'd need more radiating lines
  // to be sure of correct results for very strange (concave) shapes.
  float endPointsX[4];
  float endPointsY[4];
  // North
  endPointsX[0] = x;
  endPointsY[0] = (float)(y - 1000.0);
  // East
  endPointsX[1] = (float)(x + 1000.0);
  endPointsY[1] = y;
  // South
  endPointsX[2] = x;
  endPointsY[2] = (float)(y + 1000.0);
  // West
  endPointsX[3] = (float)(x - 1000.0);
  endPointsY[3] = y;

  // Store polygon points in an array
  int np = m_points->Number();
  float *xpoints = new float[np];
  float *ypoints = new float[np];
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
  float nearest = 999999.0;

  for (i = 0; i < n; i++)
  {
    float xp, yp;
    if (GetAttachmentPosition(i, &xp, &yp))
    {
      float l = (float)sqrt(((xp - x) * (xp - x)) +
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
void wxPolygonShape::SetSize(float new_width, float new_height, bool recursive)
{
  SetAttachmentSize(new_width, new_height);
  
  // Multiply all points by proportion of new size to old size
  float x_proportion = (float)(fabs(new_width/m_originalWidth));
  float y_proportion = (float)(fabs(new_height/m_originalHeight));

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
  m_boundWidth = (float)fabs(new_width);
  m_boundHeight = (float)fabs(new_height);
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

  float x = (float)((secondPoint->x - firstPoint->x)/2.0 + firstPoint->x);
  float y = (float)((secondPoint->y - firstPoint->y)/2.0 + firstPoint->y);
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
bool wxPolygonShape::GetPerimeterPoint(float x1, float y1,
                                     float x2, float y2,
                                     float *x3, float *y3)
{
  int n = m_points->Number();

  // First check for situation where the line is vertical,
  // and we would want to connect to a point on that vertical --
  // find_end_for_polyline can't cope with this (the arrow
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
  
  float *xpoints = new float[n];
  float *ypoints = new float[n];

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
/*
  wxRealPoint *point = (wxRealPoint *)m_points->First()->Data();
  xpoints[i] = point->x + m_xpos;
  ypoints[i] = point->y + m_ypos;
*/

  find_end_for_polyline(n, xpoints, ypoints, 
                        x1, y1, x2, y2, x3, y3);

  delete xpoints;
  delete ypoints;

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
      intPoints[i].x = (int) point->x;
      intPoints[i].y = (int) point->y;
    }

    if (m_shadowMode != SHADOW_NONE)
    {
      if (m_shadowBrush)
        dc.SetBrush(m_shadowBrush);
      dc.SetPen(transparent_pen);

      dc.DrawPolygon(n, intPoints, m_xpos + m_shadowOffsetX, m_ypos + m_shadowOffsetY);
    }

    if (m_pen)
    {
      if (m_pen->GetWidth() == 0)
        dc.SetPen(transparent_pen);
      else
        dc.SetPen(m_pen);
    }
    if (m_brush)
      dc.SetBrush(m_brush);
    dc.DrawPolygon(n, intPoints, m_xpos, m_ypos);

    delete[] intPoints;
}

void wxPolygonShape::OnDrawOutline(wxDC& dc, float x, float y, float w, float h)
{
  dc.SetBrush(wxTRANSPARENT_BRUSH);

  int n = m_points->Number();
  wxPoint *intPoints = new wxPoint[n];
  int i;
  for (i = 0; i < n; i++)
  {
    wxRealPoint* point = (wxRealPoint*) m_points->Nth(i)->Data();
    intPoints[i].x = (int) point->x;
    intPoints[i].y = (int) point->y;
  }
  dc.DrawPolygon(n, intPoints, x, y);
//  wxShape::OnDrawOutline(x, y, w, h);
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
  float maxX, maxY, minX, minY;

  GetBoundingBoxMax(&maxX, &maxY);
  GetBoundingBoxMin(&minX, &minY);

  float widthMin = (float)(minX + CONTROL_POINT_SIZE + 2);
  float heightMin = (float)(minY + CONTROL_POINT_SIZE + 2);

  // Offsets from main object
  float top = (float)(- (heightMin / 2.0));
  float bottom = (float)(heightMin / 2.0 + (maxY - minY));
  float left = (float)(- (widthMin / 2.0));
  float right = (float)(widthMin / 2.0 + (maxX - minX));

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

  float maxX, maxY, minX, minY;

  GetBoundingBoxMax(&maxX, &maxY);
  GetBoundingBoxMin(&minX, &minY);

  float widthMin = (float)(minX + CONTROL_POINT_SIZE + 2);
  float heightMin = (float)(minY + CONTROL_POINT_SIZE + 2);

  // Offsets from main object
  float top = (float)(- (heightMin / 2.0));
  float bottom = (float)(heightMin / 2.0 + (maxY - minY));
  float left = (float)(- (widthMin / 2.0));
  float right = (float)(widthMin / 2.0 + (maxX - minX));

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
    wxExpr *x_expr = new wxExpr((float)point->x);
    wxExpr *y_expr = new wxExpr((float)point->y);

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
    wxExpr *x_expr = new wxExpr((float) point->x);
    wxExpr *y_expr = new wxExpr((float) point->y);
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
  float the_height = 100.0;
  float the_width = 100.0;
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

      wxRealPoint *point = new wxRealPoint((float)x, (float)y);
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
    float min_x = 1000;
    float min_y = 1000;
    float max_x = -1000;
    float max_y = -1000;
    while (node)
    {
      wxExpr *xexpr = node->value.first;
      long x = xexpr->IntegerValue();

      wxExpr *yexpr = xexpr->next;
      long y = yexpr->IntegerValue();

      wxRealPoint *point = new wxRealPoint((float)x, (float)y);
      m_originalPoints->Append((wxObject*) point);

      if (x < min_x)
        min_x = (float)x;
      if (y < min_y)
        min_y = (float)y;
      if (x > max_x)
        max_x = (float)x;
      if (y > max_y)
        max_y = (float)y;

      node = node->next;
    }
    m_originalWidth = max_x - min_x;
    m_originalHeight = max_y - min_y;
  }

  CalculateBoundingBox();
}
#endif

void wxPolygonShape::Copy(wxPolygonShape& copy)
{
  wxShape::Copy(copy);

  if (copy.m_points)
    delete copy.m_points;

  copy.m_points = new wxList;

  if (copy.m_originalPoints)
    delete copy.m_originalPoints;

  copy.m_originalPoints = new wxList;

  wxNode *node = m_points->First();
  while (node)
  {
    wxRealPoint *point = (wxRealPoint *)node->Data();
    wxRealPoint *new_point = new wxRealPoint(point->x, point->y);
    copy.m_points->Append((wxObject*) new_point);
    node = node->Next();
  }
  node = m_originalPoints->First();
  while (node)
  {
    wxRealPoint *point = (wxRealPoint *)node->Data();
    wxRealPoint *new_point = new wxRealPoint(point->x, point->y);
    copy.m_originalPoints->Append((wxObject*) new_point);
    node = node->Next();
  }
  copy.m_boundWidth = m_boundWidth;
  copy.m_boundHeight = m_boundHeight;
  copy.m_originalWidth = m_originalWidth;
  copy.m_originalHeight = m_originalHeight;
}

wxShape *wxPolygonShape::PrivateCopy()
{
  wxPolygonShape *obj = new wxPolygonShape;
  Copy(*obj);
  return obj;
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

bool wxPolygonShape::GetAttachmentPosition(int attachment, float *x, float *y,
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

wxRectangleShape::wxRectangleShape(float w, float h)
{
  m_width = w; m_height = h; m_cornerRadius = 0.0;
  SetDefaultRegionSize();
}

void wxRectangleShape::OnDraw(wxDC& dc)
{
    float x1 = (float)(m_xpos - m_width/2.0);
    float y1 = (float)(m_ypos - m_height/2.0);

    if (m_shadowMode != SHADOW_NONE)
    {
      if (m_shadowBrush)
        dc.SetBrush(m_shadowBrush);
      dc.SetPen(transparent_pen);

      if (m_cornerRadius != 0.0)
        dc.DrawRoundedRectangle(x1 + m_shadowOffsetX, y1 + m_shadowOffsetY,
                                 m_width, m_height, m_cornerRadius);
      else
        dc.DrawRectangle(x1 + m_shadowOffsetX, y1 + m_shadowOffsetY, m_width, m_height);
    }

    if (m_pen)
    {
      if (m_pen->GetWidth() == 0)
        dc.SetPen(transparent_pen);
      else
        dc.SetPen(m_pen);
    }
    if (m_brush)
      dc.SetBrush(m_brush);

    if (m_cornerRadius != 0.0)
      dc.DrawRoundedRectangle(x1, y1, m_width, m_height, m_cornerRadius);
    else
      dc.DrawRectangle(x1, y1, m_width, m_height);
}

void wxRectangleShape::GetBoundingBoxMin(float *the_width, float *the_height)
{
  *the_width = m_width;
  *the_height = m_height;
}

void wxRectangleShape::SetSize(float x, float y, bool recursive)
{
  SetAttachmentSize(x, y);
  m_width = (float)wxMax(x, 1.0);
  m_height = (float)wxMax(y, 1.0);
  SetDefaultRegionSize();
}

void wxRectangleShape::SetCornerRadius(float rad)
{
  m_cornerRadius = rad;
}

// Assume (x1, y1) is centre of box (most generally, line end at box)
bool wxRectangleShape::GetPerimeterPoint(float x1, float y1,
                                     float x2, float y2,
                                     float *x3, float *y3)
{
  float bound_x, bound_y;
  GetBoundingBoxMax(&bound_x, &bound_y);
  find_end_for_box(bound_x, bound_y, m_xpos, m_ypos, x2, y2, x3, y3);

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

void wxRectangleShape::Copy(wxRectangleShape& copy)
{
  wxShape::Copy(copy);
  copy.m_width = m_width;
  copy.m_height = m_height;
  copy.m_cornerRadius = m_cornerRadius;
}

wxShape *wxRectangleShape::PrivateCopy()
{
  wxRectangleShape *obj = new wxRectangleShape(0.0, 0.0);
  Copy(*obj);
  return obj;
}


int wxRectangleShape::GetNumberOfAttachments()
{
  return wxShape::GetNumberOfAttachments();
}

// There are 4 attachment points on a rectangle - 0 = top, 1 = right, 2 = bottom,
// 3 = left.
bool wxRectangleShape::GetAttachmentPosition(int attachment, float *x, float *y,
                                         int nth, int no_arcs, wxLineShape *line)
{
  if (m_attachmentMode)
  {
    float top = (float)(m_ypos + m_height/2.0);
    float bottom = (float)(m_ypos - m_height/2.0);
    float left = (float)(m_xpos - m_width/2.0);
    float right = (float)(m_xpos + m_width/2.0);

    bool isEnd = (line && line->IsEnd(this));

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
    return TRUE;
  }
  else
  { *x = m_xpos; *y = m_ypos; return TRUE; }
}

// Text object (no box)

IMPLEMENT_DYNAMIC_CLASS(wxTextShape, wxRectangleShape)

wxTextShape::wxTextShape(float width, float height):
  wxRectangleShape(width, height)
{
}

void wxTextShape::OnDraw(wxDC& dc)
{
}

wxShape *wxTextShape::PrivateCopy()
{
  wxTextShape *obj = new wxTextShape(0.0, 0.0);
  Copy(*obj);
  return obj;
}

void wxTextShape::Copy(wxTextShape& copy)
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

wxEllipseShape::wxEllipseShape(float w, float h)
{
  m_width = w; m_height = h;
  SetDefaultRegionSize();
}

void wxEllipseShape::GetBoundingBoxMin(float *w, float *h)
{
  *w = m_width; *h = m_height;
}

bool wxEllipseShape::GetPerimeterPoint(float x1, float y1,
                                      float x2, float y2,
                                      float *x3, float *y3)
{
  float bound_x, bound_y;
  GetBoundingBoxMax(&bound_x, &bound_y);

//  find_end_for_box(bound_x, bound_y, m_xpos, m_ypos, x2, y2, x3, y3);
  draw_arc_to_ellipse(m_xpos, m_ypos, bound_x, bound_y, x2, y2, x1, y1, x3, y3);

  return TRUE;
}

void wxEllipseShape::OnDraw(wxDC& dc)
{
    if (m_shadowMode != SHADOW_NONE)
    {
      if (m_shadowBrush)
        dc.SetBrush(m_shadowBrush);
      dc.SetPen(transparent_pen);
      dc.DrawEllipse((m_xpos - GetWidth()/2) + m_shadowOffsetX,
                      (m_ypos - GetHeight()/2) + m_shadowOffsetY,
                      GetWidth(), GetHeight());
    }

    if (m_pen)
    {
      if (m_pen->GetWidth() == 0)
        dc.SetPen(transparent_pen);
      else
        dc.SetPen(m_pen);
    }
    if (m_brush)
      dc.SetBrush(m_brush);
    dc.DrawEllipse((m_xpos - GetWidth()/2), (m_ypos - GetHeight()/2), GetWidth(), GetHeight());
}

void wxEllipseShape::SetSize(float x, float y, bool recursive)
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

void wxEllipseShape::Copy(wxEllipseShape& copy)
{
  wxShape::Copy(copy);

  copy.m_width = m_width;
  copy.m_height = m_height;
}

wxShape *wxEllipseShape::PrivateCopy()
{
  wxEllipseShape *obj = new wxEllipseShape(0.0, 0.0);
  Copy(*obj);
  return obj;
}

int wxEllipseShape::GetNumberOfAttachments()
{
  return wxShape::GetNumberOfAttachments();
}

// There are 4 attachment points on an ellipse - 0 = top, 1 = right, 2 = bottom,
// 3 = left.
bool wxEllipseShape::GetAttachmentPosition(int attachment, float *x, float *y,
                                         int nth, int no_arcs, wxLineShape *line)
{
  if (m_attachmentMode)
  {
    float top = (float)(m_ypos + m_height/2.0);
    float bottom = (float)(m_ypos - m_height/2.0);
    float left = (float)(m_xpos - m_width/2.0);
    float right = (float)(m_xpos + m_width/2.0);
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
        draw_arc_to_ellipse(m_xpos, m_ypos, m_width, m_height, *x, (float)(m_ypos-m_height-500), *x, m_ypos, x, y);
        break;
      }
      case 1:
      {
        *x = right;
        if (m_spaceAttachments)
          *y = bottom + (nth + 1)*m_height/(no_arcs + 1);
        else *y = m_ypos;
        draw_arc_to_ellipse(m_xpos, m_ypos, m_width, m_height, (float)(m_xpos+m_width+500), *y, m_xpos, *y, x, y);
        break;
      }
      case 2:
      {
        if (m_spaceAttachments)
          *x = left + (nth + 1)*m_width/(no_arcs + 1);
        else *x = m_xpos;
        *y = bottom;
        draw_arc_to_ellipse(m_xpos, m_ypos, m_width, m_height, *x, (float)(m_ypos+m_height+500), *x, m_ypos, x, y);
        break;
      }
      case 3:
      {
        *x = left;
        if (m_spaceAttachments)
          *y = bottom + (nth + 1)*m_height/(no_arcs + 1);
        else *y = m_ypos;
        draw_arc_to_ellipse(m_xpos, m_ypos, m_width, m_height, (float)(m_xpos-m_width-500), *y, m_xpos, *y, x, y);
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

wxCircleShape::wxCircleShape(float diameter):wxEllipseShape(diameter, diameter)
{
}

wxShape *wxCircleShape::PrivateCopy()
{
  wxCircleShape *obj = new wxCircleShape(0.0);
  Copy(*obj);
  return obj;
}

void wxCircleShape::Copy(wxCircleShape& copy)
{
  wxEllipseShape::Copy(copy);
}

bool wxCircleShape::GetPerimeterPoint(float x1, float y1,
                                      float x2, float y2,
                                      float *x3, float *y3)
{
  find_end_for_circle(m_width/2, 
                      m_xpos, m_ypos,  // Centre of circle
                      x2, y2,  // Other end of line
                      x3, y3);

  return TRUE;
}

// Control points

IMPLEMENT_DYNAMIC_CLASS(wxControlPoint, wxRectangleShape)

wxControlPoint::wxControlPoint(wxShapeCanvas *theCanvas, wxShape *object, float size, float the_xoffset, float the_yoffset, int the_type):wxRectangleShape(size, size)
{
  m_canvas = theCanvas;
  m_shape = object;
  m_xoffset = the_xoffset;
  m_yoffset = the_yoffset;
  m_type = the_type;
  SetPen(black_foreground_pen);
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

/*
 * Store original top-left, bottom-right coordinates
 * in case we're doing non-vertical resizing.
 */
static float controlPointDragStartX = 0.0;
static float controlPointDragStartY = 0.0;
static float controlPointDragStartWidth = 0.0;
static float controlPointDragStartHeight = 0.0;
static float controlPointDragEndWidth = 0.0;
static float controlPointDragEndHeight = 0.0;
static float controlPointDragPosX = 0.0;
static float controlPointDragPosY = 0.0;

// Implement resizing of canvas object
void wxControlPoint::OnDragLeft(bool draw, float x, float y, int keys, int attachment)
{
  float bound_x;
  float bound_y;
  m_shape->GetBoundingBoxMin(&bound_x, &bound_y);

  wxClientDC dc(GetCanvas());
  GetCanvas()->PrepareDC(dc);

  dc.SetLogicalFunction(wxXOR);

  wxPen dottedPen(wxColour(0, 0, 0), 1, wxDOT);
  dc.SetPen(dottedPen);
  dc.SetBrush((* wxTRANSPARENT_BRUSH));

  if (m_shape->GetCentreResize())
  {
    // Maintain the same centre point.
    float new_width = (float)(2.0*fabs(x - m_shape->GetX()));
    float new_height = (float)(2.0*fabs(y - m_shape->GetY()));

    // Constrain sizing according to what control point you're dragging
    if (m_type == CONTROL_POINT_HORIZONTAL)
      new_height = bound_y;
    else if (m_type == CONTROL_POINT_VERTICAL)
      new_width = bound_x;
    else if (m_type == CONTROL_POINT_DIAGONAL && (keys & KEY_SHIFT))
      new_height = bound_y*(new_width/bound_x);

//    m_shape->OnBeginSize(m_shape->m_fixedWidth ? bound_x : new_width,
//                               m_shape->m_fixedHeight ? bound_y : new_height);

//    m_shape->SetSize(m_shape->m_fixedWidth ? bound_x : new_width,
//                           m_shape->m_fixedHeight ? bound_y : new_height);
    if (m_shape->GetFixedWidth())
      new_width = bound_x;
      
    if (m_shape->GetFixedHeight())
      new_height = bound_y;

    controlPointDragEndWidth = new_width;
    controlPointDragEndHeight = new_height;

    m_shape->GetEventHandler()->OnDrawOutline(dc, m_shape->GetX(), m_shape->GetY(),
                                new_width, new_height);
  }
  else
  {
    // Don't maintain the same centre point!
    float newX1 = wxMin(controlPointDragStartX, x);
    float newY1 = wxMin(controlPointDragStartY, y);
    float newX2 = wxMax(controlPointDragStartX, x);
    float newY2 = wxMax(controlPointDragStartY, y);
    if (m_type == CONTROL_POINT_HORIZONTAL)
    {
      newY1 = controlPointDragStartY;
      newY2 = newY1 + controlPointDragStartHeight;
    }
    else if (m_type == CONTROL_POINT_VERTICAL)
    {
      newX1 = controlPointDragStartX;
      newX2 = newX1 + controlPointDragStartWidth;
    }
    else if (m_type == CONTROL_POINT_DIAGONAL && (keys & KEY_SHIFT))
    {
      float newH = (float)((newX2 - newX1)*(controlPointDragStartHeight/controlPointDragStartWidth));
      if (GetY() > controlPointDragStartY)
        newY2 = (float)(newY1 + newH);
      else
        newY1 = (float)(newY2 - newH);
    }
    float newWidth = (float)(newX2 - newX1);
    float newHeight = (float)(newY2 - newY1);

//    m_shape->OnBeginSize(newWidth,
//                               newHeight);
    
//    m_shape->SetSize(newWidth,
//                           newHeight);
    controlPointDragPosX = (float)(newX1 + (newWidth/2.0));
    controlPointDragPosY = (float)(newY1 + (newHeight/2.0));
    if (m_shape->GetFixedWidth())
      newWidth = bound_x;
     
    if (m_shape->GetFixedHeight())
      newHeight = bound_y;

    controlPointDragEndWidth = newWidth;
    controlPointDragEndHeight = newHeight;
    m_shape->GetEventHandler()->OnDrawOutline(dc, controlPointDragPosX, controlPointDragPosY, newWidth, newHeight);
  }
}

void wxControlPoint::OnBeginDragLeft(float x, float y, int keys, int attachment)
{
  m_canvas->CaptureMouse();

  wxClientDC dc(GetCanvas());
  GetCanvas()->PrepareDC(dc);

  if (m_eraseObject)
    m_shape->Erase(dc);

  dc.SetLogicalFunction(wxXOR);

  float bound_x;
  float bound_y;
  m_shape->GetBoundingBoxMin(&bound_x, &bound_y);

  // Choose the 'opposite corner' of the object as the stationary
  // point in case this is non-centring resizing.
  if (GetX() < m_shape->GetX())
    controlPointDragStartX = (float)(m_shape->GetX() + (bound_x/2.0));
  else
    controlPointDragStartX = (float)(m_shape->GetX() - (bound_x/2.0));

  if (GetY() < m_shape->GetY())
    controlPointDragStartY = (float)(m_shape->GetY() + (bound_y/2.0));
  else
    controlPointDragStartY = (float)(m_shape->GetY() - (bound_y/2.0));

  if (m_type == CONTROL_POINT_HORIZONTAL)
    controlPointDragStartY = (float)(m_shape->GetY() - (bound_y/2.0));
  else if (m_type == CONTROL_POINT_VERTICAL)
    controlPointDragStartX = (float)(m_shape->GetX() - (bound_x/2.0));

  // We may require the old width and height.
  controlPointDragStartWidth = bound_x;
  controlPointDragStartHeight = bound_y;

  wxPen dottedPen(wxColour(0, 0, 0), 1, wxDOT);
  dc.SetPen(dottedPen);
  dc.SetBrush((* wxTRANSPARENT_BRUSH));

  if (m_shape->GetCentreResize())
  {
    float new_width = (float)(2.0*fabs(x - m_shape->GetX()));
    float new_height = (float)(2.0*fabs(y - m_shape->GetY()));

    // Constrain sizing according to what control point you're dragging
    if (m_type == CONTROL_POINT_HORIZONTAL)
      new_height = bound_y;
    else if (m_type == CONTROL_POINT_VERTICAL)
      new_width = bound_x;
    else if (m_type == CONTROL_POINT_DIAGONAL && (keys & KEY_SHIFT))
      new_height = bound_y*(new_width/bound_x);

    // Non-recursive SetSize for speed
//    m_shape->SetSize(new_width, new_height, FALSE);

    if (m_shape->GetFixedWidth())
      new_width = bound_x;
      
    if (m_shape->GetFixedHeight())
      new_height = bound_y;

    controlPointDragEndWidth = new_width;
    controlPointDragEndHeight = new_height;
    m_shape->GetEventHandler()->OnDrawOutline(dc, m_shape->GetX(), m_shape->GetY(),
                                new_width, new_height);
  }
  else
  {
    // Don't maintain the same centre point!
    float newX1 = wxMin(controlPointDragStartX, x);
    float newY1 = wxMin(controlPointDragStartY, y);
    float newX2 = wxMax(controlPointDragStartX, x);
    float newY2 = wxMax(controlPointDragStartY, y);
    if (m_type == CONTROL_POINT_HORIZONTAL)
    {
      newY1 = controlPointDragStartY;
      newY2 = newY1 + controlPointDragStartHeight;
    }
    else if (m_type == CONTROL_POINT_VERTICAL)
    {
      newX1 = controlPointDragStartX;
      newX2 = newX1 + controlPointDragStartWidth;
    }
    else if (m_type == CONTROL_POINT_DIAGONAL && (keys & KEY_SHIFT))
    {
      float newH = (float)((newX2 - newX1)*(controlPointDragStartHeight/controlPointDragStartWidth));
      if (GetY() > controlPointDragStartY)
        newY2 = (float)(newY1 + newH);
      else
        newY1 = (float)(newY2 - newH);
    }
    float newWidth = (float)(newX2 - newX1);
    float newHeight = (float)(newY2 - newY1);

//    m_shape->OnBeginSize(newWidth,
//                               newHeight);
    
//    m_shape->SetSize(newWidth,
//                           newHeight);
    controlPointDragPosX = (float)(newX1 + (newWidth/2.0));
    controlPointDragPosY = (float)(newY1 + (newHeight/2.0));
    if (m_shape->GetFixedWidth())
      newWidth = bound_x;

    if (m_shape->GetFixedHeight())
      newHeight = bound_y;

    controlPointDragEndWidth = newWidth;
    controlPointDragEndHeight = newHeight;
    m_shape->GetEventHandler()->OnDrawOutline(dc, controlPointDragPosX, controlPointDragPosY, newWidth, newHeight);
  }
}

void wxControlPoint::OnEndDragLeft(float x, float y, int keys, int attachment)
{
  wxClientDC dc(GetCanvas());
  GetCanvas()->PrepareDC(dc);

  m_canvas->ReleaseMouse();
  dc.SetLogicalFunction(wxCOPY);
  m_shape->Recompute();
  m_shape->ResetControlPoints();

  if (!m_eraseObject)
    m_shape->Show(FALSE);

  m_shape->SetSize(controlPointDragEndWidth, controlPointDragEndHeight);

  // The next operation could destroy this control point (it does for label objects,
  // via formatting the text), so save all values we're going to use, or
  // we'll be accessing garbage.
  wxShape *theObject = m_shape;
  wxShapeCanvas *theCanvas = m_canvas;
  bool eraseIt = m_eraseObject;

  if (theObject->GetCentreResize())
    theObject->Move(dc, theObject->GetX(), theObject->GetY());
  else
    theObject->Move(dc, controlPointDragPosX, controlPointDragPosY);

  if (!eraseIt)
    theObject->Show(TRUE);
    
  // Recursively redraw links if we have a composite.
  if (theObject->GetChildren().Number() > 0)
    theObject->DrawLinks(dc, -1, TRUE);
    
  float width, height;
  theObject->GetBoundingBoxMax(&width, &height);
  theObject->GetEventHandler()->OnEndSize(width, height);

  if (!theCanvas->GetQuickEditMode() && eraseIt) theCanvas->Redraw(dc);
}

int wxControlPoint::GetNumberOfAttachments()
{
  return 1;
}

bool wxControlPoint::GetAttachmentPosition(int attachment, float *x, float *y,
                                         int nth, int no_arcs, wxLineShape *line)
{
  *x = m_xpos; *y = m_ypos;
  return TRUE;
}


// Polygon control points

IMPLEMENT_DYNAMIC_CLASS(wxPolygonControlPoint, wxControlPoint)

wxPolygonControlPoint::wxPolygonControlPoint(wxShapeCanvas *theCanvas, wxShape *object, float size,
  wxRealPoint *vertex, float the_xoffset, float the_yoffset):
  wxControlPoint(theCanvas, object, size, the_xoffset, the_yoffset, 0)
{
  m_polygonVertex = vertex;
}

wxPolygonControlPoint::~wxPolygonControlPoint()
{
}

// Implement resizing polygon or moving the vertex.
void wxPolygonControlPoint::OnDragLeft(bool draw, float x, float y, int keys, int attachment)
{
  wxClientDC dc(GetCanvas());
  GetCanvas()->PrepareDC(dc);

  dc.SetLogicalFunction(wxXOR);

  wxPen dottedPen(wxColour(0, 0, 0), 1, wxDOT);
  dc.SetPen(dottedPen);
  dc.SetBrush((* wxTRANSPARENT_BRUSH));

  float bound_x;
  float bound_y;
  m_shape->GetBoundingBoxMin(&bound_x, &bound_y);
/*
  float new_width = (float)(2.0*fabs(x - m_shape->GetX()));
  float new_height = (float)(2.0*fabs(y - m_shape->GetY()));
*/
  float dist = (float)sqrt((x - m_shape->GetX())*(x - m_shape->GetX()) +
                    (y - m_shape->GetY())*(y - m_shape->GetY()));

  if (keys & KEY_CTRL)
  {
    m_canvas->Snap(&x, &y);

    // Move point
    m_polygonVertex->x = x - m_shape->GetX();
    m_polygonVertex->y = y - m_shape->GetY();
    m_xpos = x;
    m_xpos = y;
    ((wxPolygonShape *)m_shape)->CalculateBoundingBox();
    ((wxPolygonShape *)m_shape)->CalculatePolygonCentre();
  }
  else
  {
    float new_width = (float)(dist/m_originalDistance)*m_originalSize.x;
    float new_height = (float)(dist/m_originalDistance)*m_originalSize.y;

    // Non-recursive SetSize for speed
    m_shape->SetSize(new_width, new_height, FALSE);
  }
  float w, h;
  m_shape->GetBoundingBoxMax(&w, &h);
  m_shape->GetEventHandler()->OnDrawOutline(dc, m_shape->GetX(), m_shape->GetY(), w, h);
}

void wxPolygonControlPoint::OnBeginDragLeft(float x, float y, int keys, int attachment)
{
  wxClientDC dc(GetCanvas());
  GetCanvas()->PrepareDC(dc);

  m_shape->Erase(dc);

  dc.SetLogicalFunction(wxXOR);

  float bound_x;
  float bound_y;
  m_shape->GetBoundingBoxMin(&bound_x, &bound_y);

  float dist = (float)sqrt((x - m_shape->GetX())*(x - m_shape->GetX()) +
                    (y - m_shape->GetY())*(y - m_shape->GetY()));

  m_originalDistance = dist;
  m_originalSize.x = bound_x;
  m_originalSize.y = bound_y;

  if (m_originalDistance == 0.0) m_originalDistance = (float) 0.0001;

  wxPen dottedPen(wxColour(0, 0, 0), 1, wxDOT);
  dc.SetPen(dottedPen);
  dc.SetBrush((* wxTRANSPARENT_BRUSH));

  if (keys & KEY_CTRL)
  {
    m_canvas->Snap(&x, &y);

    // Move point
    m_polygonVertex->x = x - m_shape->GetX();
    m_polygonVertex->y = y - m_shape->GetY();
    m_xpos = x;
    m_xpos = y;
    ((wxPolygonShape *)m_shape)->CalculateBoundingBox();
    ((wxPolygonShape *)m_shape)->CalculatePolygonCentre();
  }
  else
  {
    float new_width = (float)(dist/m_originalDistance)*m_originalSize.x;
    float new_height = (float)(dist/m_originalDistance)*m_originalSize.y;

    // Non-recursive SetSize for speed
    m_shape->SetSize(new_width, new_height, FALSE);
  }

  float w, h;
  m_shape->GetBoundingBoxMax(&w, &h);
  m_shape->GetEventHandler()->OnDrawOutline(dc, m_shape->GetX(), m_shape->GetY(), w, h);

  m_canvas->CaptureMouse();
}

void wxPolygonControlPoint::OnEndDragLeft(float x, float y, int keys, int attachment)
{
  wxClientDC dc(GetCanvas());
  GetCanvas()->PrepareDC(dc);

  m_canvas->ReleaseMouse();
  dc.SetLogicalFunction(wxCOPY);

  // If we're changing shape, must reset the original points
  if (keys & KEY_CTRL)
  {
    ((wxPolygonShape *)m_shape)->CalculateBoundingBox();
    ((wxPolygonShape *)m_shape)->UpdateOriginalPoints();
  }

  ((wxPolygonShape *)m_shape)->CalculateBoundingBox();
  ((wxPolygonShape *)m_shape)->CalculatePolygonCentre();

  m_shape->Recompute();
  m_shape->ResetControlPoints();
  m_shape->Move(dc, m_shape->GetX(), m_shape->GetY());
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

void wxShapeRegion::SetMinSize(float w, float h)
{
  m_minWidth = w;
  m_minHeight = h;
}

void wxShapeRegion::SetSize(float w, float h)
{
  m_width = w;
  m_height = h;
}

void wxShapeRegion::SetPosition(float xp, float yp)
{
  m_x = xp;
  m_y = yp;
}

void wxShapeRegion::SetProportions(float xp, float yp)
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


