/////////////////////////////////////////////////////////////////////////////
// Name:        lines.cpp
// Purpose:     wxLineShape
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "lines.h"
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

#include <ctype.h>
#include <math.h>

#include "basic.h"
#include "basicp.h"
#include "lines.h"
#include "linesp.h"
#include "drawn.h"
#include "misc.h"
#include "canvas.h"

// Line shape
IMPLEMENT_DYNAMIC_CLASS(wxLineShape, wxShape)

wxLineShape::wxLineShape()
{
  m_sensitivity = OP_CLICK_LEFT | OP_CLICK_RIGHT;
  m_draggable = FALSE;
  m_attachmentTo = 0;
  m_attachmentFrom = 0;
/*
  m_actualTextWidth = 0.0;
  m_actualTextHeight = 0.0;
*/
  m_from = NULL;
  m_to = NULL;
  m_erasing = FALSE;
  m_arrowSpacing = 5.0; // For the moment, don't bother saving this to file.
  m_ignoreArrowOffsets = FALSE;
  m_isSpline = FALSE;
  m_maintainStraightLines = FALSE;
  m_alignmentStart = 0;
  m_alignmentEnd = 0;

  m_lineControlPoints = NULL;

  // Clear any existing regions (created in an earlier constructor)
  // and make the three line regions.
  ClearRegions();
  wxShapeRegion *newRegion = new wxShapeRegion;
  newRegion->SetName("Middle");
  newRegion->SetSize(150, 50);
  m_regions.Append((wxObject *)newRegion);

  newRegion = new wxShapeRegion;
  newRegion->SetName("Start");
  newRegion->SetSize(150, 50);
  m_regions.Append((wxObject *)newRegion);  

  newRegion = new wxShapeRegion;
  newRegion->SetName("End");
  newRegion->SetSize(150, 50);
  m_regions.Append((wxObject *)newRegion);

  for (int i = 0; i < 3; i++)
    m_labelObjects[i] = NULL;
}

wxLineShape::~wxLineShape()
{
  if (m_lineControlPoints)
  {
    ClearPointList(*m_lineControlPoints);
    delete m_lineControlPoints;
  }
  for (int i = 0; i < 3; i++)
  {
    if (m_labelObjects[i])
    {
      m_labelObjects[i]->Select(FALSE);
      m_labelObjects[i]->RemoveFromCanvas(m_canvas);
      delete m_labelObjects[i];
      m_labelObjects[i] = NULL;
    }
  }
}

void wxLineShape::MakeLineControlPoints(int n)
{
  if (m_lineControlPoints)
  {
    ClearPointList(*m_lineControlPoints);
    delete m_lineControlPoints;
  }
  m_lineControlPoints = new wxList;

  int i = 0;
  for (i = 0; i < n; i++)
  {
    wxRealPoint *point = new wxRealPoint(-999, -999);
    m_lineControlPoints->Append((wxObject*) point);
  }
}

wxNode *wxLineShape::InsertLineControlPoint(wxDC* dc)
{
    if (dc)
        Erase(*dc);

  wxNode *last = m_lineControlPoints->Last();
  wxNode *second_last = last->Previous();
  wxRealPoint *last_point = (wxRealPoint *)last->Data();
  wxRealPoint *second_last_point = (wxRealPoint *)second_last->Data();

  // Choose a point half way between the last and penultimate points
  float line_x = ((last_point->x + second_last_point->x)/2);
  float line_y = ((last_point->y + second_last_point->y)/2);

  wxRealPoint *point = new wxRealPoint(line_x, line_y);
  wxNode *node = m_lineControlPoints->Insert(last, (wxObject*) point);
  return node;
}

bool wxLineShape::DeleteLineControlPoint()
{
  if (m_lineControlPoints->Number() < 3)
    return FALSE;

  wxNode *last = m_lineControlPoints->Last();
  wxNode *second_last = last->Previous();

  wxRealPoint *second_last_point = (wxRealPoint *)second_last->Data();
  delete second_last_point;
  delete second_last;

  return TRUE;
}

void wxLineShape::Initialise()
{
  if (m_lineControlPoints)
  {
    // Just move the first and last control points
    wxNode *first = m_lineControlPoints->First();
    wxRealPoint *first_point = (wxRealPoint *)first->Data();

    wxNode *last = m_lineControlPoints->Last();
    wxRealPoint *last_point = (wxRealPoint *)last->Data();

    // If any of the line points are at -999, we must
    // initialize them by placing them half way between the first
    // and the last.
    wxNode *node = first->Next();
    while (node)
    {
      wxRealPoint *point = (wxRealPoint *)node->Data();
      if (point->x == -999)
      {
        float x1, y1, x2, y2;
        if (first_point->x < last_point->x)
          { x1 = first_point->x; x2 = last_point->x; }
        else
          { x2 = first_point->x; x1 = last_point->x; }

        if (first_point->y < last_point->y)
          { y1 = first_point->y; y2 = last_point->y; }
        else
          { y2 = first_point->y; y1 = last_point->y; }

        point->x = ((x2 - x1)/2 + x1);
        point->y = ((y2 - y1)/2 + y1);
      }
      node = node->Next();
    }
  }
}

// Format a text string according to the region size, adding
// strings with positions to region text list
void wxLineShape::FormatText(wxDC& dc, const wxString& s, int i)
{
  float w, h;
  ClearText(i);

  if (m_regions.Number() < 1)
    return;
  wxNode *node = m_regions.Nth(i);
  if (!node)
    return;
    
  wxShapeRegion *region = (wxShapeRegion *)node->Data();
  region->SetText(s);
  dc.SetFont(region->GetFont());

  region->GetSize(&w, &h);
  // Initialize the size if zero
  if (((w == 0) || (h == 0)) && (strlen(s) > 0))
  {
    w = 100; h = 50;
    region->SetSize(w, h);
  }
    
  wxList *string_list = ::FormatText(dc, s, (w-5), (h-5), region->GetFormatMode());
  node = string_list->First();
  while (node)
  {
    char *s = (char *)node->Data();
    wxShapeTextLine *line = new wxShapeTextLine(0.0, 0.0, s);
    region->GetFormattedText().Append((wxObject *)line);
    delete node;
    node = string_list->First();
  }
  delete string_list;
  float actualW = w;
  float actualH = h;
  if (region->GetFormatMode() & FORMAT_SIZE_TO_CONTENTS)
  {
    GetCentredTextExtent(dc, &(region->GetFormattedText()), m_xpos, m_ypos, w, h, &actualW, &actualH);
    if ((actualW != w ) || (actualH != h))
    {
      float xx, yy;
      GetLabelPosition(i, &xx, &yy);
      EraseRegion(dc, region, xx, yy);
      if (m_labelObjects[i])
      {
        m_labelObjects[i]->Select(FALSE);
        m_labelObjects[i]->Erase(dc);
        m_labelObjects[i]->SetSize(actualW, actualH);
      }

      region->SetSize(actualW, actualH);

      if (m_labelObjects[i])
      {
        m_labelObjects[i]->Select(TRUE, & dc);
        m_labelObjects[i]->Draw(dc);
      }
    }
  }
  CentreText(dc, &(region->GetFormattedText()), m_xpos, m_ypos, actualW, actualH, region->GetFormatMode());
  m_formatted = TRUE;
}

void wxLineShape::DrawRegion(wxDC& dc, wxShapeRegion *region, float x, float y)
{
  if (GetDisableLabel())
    return;

  float w, h;
  float xx, yy;
  region->GetSize(&w, &h);

  // Get offset from x, y
  region->GetPosition(&xx, &yy);

  float xp = xx + x;
  float yp = yy + y;

  // First, clear a rectangle for the text IF there is any
  if (region->GetFormattedText().Number() > 0)
  {
      dc.SetPen(white_background_pen);
      dc.SetBrush(white_background_brush);

      // Now draw the text
      if (region->GetFont()) dc.SetFont(region->GetFont());

      dc.DrawRectangle((float)(xp - w/2.0), (float)(yp - h/2.0), (float)w, (float)h);

      if (m_pen) dc.SetPen(m_pen);
      dc.SetTextForeground(* region->GetActualColourObject());

#ifdef __WXMSW__
      dc.SetTextBackground(white_background_brush->GetColour());
#endif

      DrawFormattedText(dc, &(region->GetFormattedText()), xp, yp, w, h, region->GetFormatMode());
  }
}

void wxLineShape::EraseRegion(wxDC& dc, wxShapeRegion *region, float x, float y)
{
  if (GetDisableLabel())
    return;

  float w, h;
  float xx, yy;
  region->GetSize(&w, &h);

  // Get offset from x, y
  region->GetPosition(&xx, &yy);

  float xp = xx + x;
  float yp = yy + y;

  if (region->GetFormattedText().Number() > 0)
  {
      dc.SetPen(white_background_pen);
      dc.SetBrush(white_background_brush);

      dc.DrawRectangle((float)(xp - w/2.0), (float)(yp - h/2.0), (float)w, (float)h);
  }
}

// Get the reference point for a label. Region x and y
// are offsets from this.
// position is 0, 1, 2
void wxLineShape::GetLabelPosition(int position, float *x, float *y)
{
  switch (position)
  {
    case 0:
    {
      // Want to take the middle section for the label
      int n = m_lineControlPoints->Number();
      int half_way = (int)(n/2);

      // Find middle of this line
      wxNode *node = m_lineControlPoints->Nth(half_way - 1);
      wxRealPoint *point = (wxRealPoint *)node->Data();
      wxNode *next_node = node->Next();
      wxRealPoint *next_point = (wxRealPoint *)next_node->Data();

      float dx = (next_point->x - point->x);
      float dy = (next_point->y - point->y);
      *x = (float)(point->x + dx/2.0);
      *y = (float)(point->y + dy/2.0);
      break;
    }
    case 1:
    {
      wxNode *node = m_lineControlPoints->First();
      *x = ((wxRealPoint *)node->Data())->x;
      *y = ((wxRealPoint *)node->Data())->y;
      break;
    }
    case 2:
    {
      wxNode *node = m_lineControlPoints->Last();
      *x = ((wxRealPoint *)node->Data())->x;
      *y = ((wxRealPoint *)node->Data())->y;
      break;
    }
    default:
      break;
  }
}

/*
 * Find whether line is supposed to be vertical or horizontal and
 * make it so.
 *
 */
void GraphicsStraightenLine(wxRealPoint *point1, wxRealPoint *point2)
{
  float dx = point2->x - point1->x;
  float dy = point2->y - point1->y;

  if (dx == 0.0)
    return;
  else if (fabs(dy/dx) > 1.0)
  {
    point2->x = point1->x;
  }
  else point2->y = point1->y;
}

void wxLineShape::Straighten(wxDC& dc)
{
  if (!m_lineControlPoints || m_lineControlPoints->Number() < 3)
    return;

  Erase(dc);

  wxNode *first_point_node = m_lineControlPoints->First();
  wxNode *last_point_node = m_lineControlPoints->Last();
  wxNode *second_last_point_node = last_point_node->Previous();

  wxRealPoint *last_point = (wxRealPoint *)last_point_node->Data();
  wxRealPoint *second_last_point = (wxRealPoint *)second_last_point_node->Data();

  GraphicsStraightenLine(last_point, second_last_point);

  wxNode *node = first_point_node;
  while (node && (node != second_last_point_node))
  {
    wxRealPoint *point = (wxRealPoint *)node->Data();
    wxRealPoint *next_point = (wxRealPoint *)(node->Next()->Data());

    GraphicsStraightenLine(point, next_point);
    node = node->Next();
  }

  Draw(dc);
}


void wxLineShape::Unlink()
{
  if (m_to)
    m_to->GetLines().DeleteObject(this);
  if (m_from)
    m_from->GetLines().DeleteObject(this);
  m_to = NULL;
  m_from = NULL;
}

void wxLineShape::SetEnds(float x1, float y1, float x2, float y2)
{
  // Find centre point
  wxNode *first_point_node = m_lineControlPoints->First();
  wxNode *last_point_node = m_lineControlPoints->Last();
  wxRealPoint *first_point = (wxRealPoint *)first_point_node->Data();
  wxRealPoint *last_point = (wxRealPoint *)last_point_node->Data();

  first_point->x = x1;
  first_point->y = y1;
  last_point->x = x2;
  last_point->y = y2;
  
  m_xpos = (float)((x1 + x2)/2.0);
  m_ypos = (float)((y1 + y2)/2.0);
}

// Get absolute positions of ends
void wxLineShape::GetEnds(float *x1, float *y1, float *x2, float *y2)
{
  wxNode *first_point_node = m_lineControlPoints->First();
  wxNode *last_point_node = m_lineControlPoints->Last();
  wxRealPoint *first_point = (wxRealPoint *)first_point_node->Data();
  wxRealPoint *last_point = (wxRealPoint *)last_point_node->Data();

  *x1 = first_point->x; *y1 = first_point->y;
  *x2 = last_point->x; *y2 = last_point->y;
}

void wxLineShape::SetAttachments(int from_attach, int to_attach)
{
  m_attachmentFrom = from_attach;
  m_attachmentTo = to_attach;
}

bool wxLineShape::HitTest(float x, float y, int *attachment, float *distance)
{
  if (!m_lineControlPoints)
    return FALSE;

  // Look at label regions in case mouse is over a label
  bool inLabelRegion = FALSE;
  for (int i = 0; i < 3; i ++)
  {
    wxNode *regionNode = m_regions.Nth(i);
    if (regionNode)
    {
      wxShapeRegion *region = (wxShapeRegion *)regionNode->Data();
      if (region->m_formattedText.Number() > 0)
      {
        float xp, yp, cx, cy, cw, ch;
        GetLabelPosition(i, &xp, &yp);
        // Offset region from default label position
        region->GetPosition(&cx, &cy);
        region->GetSize(&cw, &ch);
        cx += xp;
        cy += yp;
        float rLeft = (float)(cx - (cw/2.0));
        float rTop = (float)(cy - (ch/2.0));
        float rRight = (float)(cx + (cw/2.0));
        float rBottom = (float)(cy + (ch/2.0));
        if (x > rLeft && x < rRight && y > rTop && y < rBottom)
        {
          inLabelRegion = TRUE;
          i = 3;
        }
      }
    }
  }

  wxNode *node = m_lineControlPoints->First();

  while (node && node->Next())
  {
    wxRealPoint *point1 = (wxRealPoint *)node->Data();
    wxRealPoint *point2 = (wxRealPoint *)node->Next()->Data();

    // Allow for inaccurate mousing or vert/horiz lines
    int extra = 4;
    float left = wxMin(point1->x, point2->x) - extra;
    float right = wxMax(point1->x, point2->x) + extra;

    float bottom = wxMin(point1->y, point2->y) - extra;
    float top = wxMax(point1->y, point2->y) + extra;

    if ((x > left && x < right && y > bottom && y < top) || inLabelRegion)
    {
      // Work out distance from centre of line
      float centre_x = (float)(left + (right - left)/2.0);
      float centre_y = (float)(bottom + (top - bottom)/2.0);

      *attachment = 0;
      *distance = (float)sqrt((centre_x - x)*(centre_x - x) + (centre_y - y)*(centre_y - y));
      return TRUE;
    }

    node = node->Next();
  }
  return FALSE;
}

void wxLineShape::DrawArrows(wxDC& dc)
{
  // Distance along line of each arrow: space them out evenly.
  float startArrowPos = 0.0;
  float endArrowPos = 0.0;
  float middleArrowPos = 0.0;
    
  wxNode *node = m_arcArrows.First();
  while (node)
  {
    wxArrowHead *arrow = (wxArrowHead *)node->Data();
    switch (arrow->GetArrowEnd())
    {
      case ARROW_POSITION_START:
      {
        if ((arrow->GetXOffset() != 0.0) && !m_ignoreArrowOffsets)
          // If specified, x offset is proportional to line length
          DrawArrow(dc, arrow, arrow->GetXOffset(), TRUE);
        else
        {
          DrawArrow(dc, arrow, startArrowPos, FALSE);      // Absolute distance
          startArrowPos += arrow->GetSize() + arrow->GetSpacing();
        }
        break;
      }
      case ARROW_POSITION_END:
      {
        if ((arrow->GetXOffset() != 0.0) && !m_ignoreArrowOffsets)
          DrawArrow(dc, arrow, arrow->GetXOffset(), TRUE);
        else
        {
          DrawArrow(dc, arrow, endArrowPos, FALSE);
          endArrowPos += arrow->GetSize() + arrow->GetSpacing();
        }
        break;
      }
      case ARROW_POSITION_MIDDLE:
      {
        arrow->SetXOffset(middleArrowPos);
        if ((arrow->GetXOffset() != 0.0) && !m_ignoreArrowOffsets)
          DrawArrow(dc, arrow, arrow->GetXOffset(), TRUE);
        else
        {
          DrawArrow(dc, arrow, middleArrowPos, FALSE);
          middleArrowPos += arrow->GetSize() + arrow->GetSpacing();
        }
        break;
      }
    }
    node = node->Next();
  }
}

void wxLineShape::DrawArrow(wxDC& dc, wxArrowHead *arrow, float xOffset, bool proportionalOffset)
{
  wxNode *first_line_node = m_lineControlPoints->First();
  wxRealPoint *first_line_point = (wxRealPoint *)first_line_node->Data();
  wxNode *second_line_node = first_line_node->Next();
  wxRealPoint *second_line_point = (wxRealPoint *)second_line_node->Data();

  wxNode *last_line_node = m_lineControlPoints->Last();
  wxRealPoint *last_line_point = (wxRealPoint *)last_line_node->Data();
  wxNode *second_last_line_node = last_line_node->Previous();
  wxRealPoint *second_last_line_point = (wxRealPoint *)second_last_line_node->Data();

  // Position where we want to start drawing
  float positionOnLineX, positionOnLineY;

  // Position of start point of line, at the end of which we draw the arrow.
  float startPositionX, startPositionY;

  switch (arrow->GetPosition())
  {
    case ARROW_POSITION_START:
    {
      // If we're using a proportional offset, calculate just where this will
      // be on the line.
      float realOffset = xOffset;
      if (proportionalOffset)
      {
        float totalLength =
          (float)sqrt((second_line_point->x - first_line_point->x)*(second_line_point->x - first_line_point->x) +
                      (second_line_point->y - first_line_point->y)*(second_line_point->y - first_line_point->y));
        realOffset = (float)(xOffset * totalLength);
      }
      GetPointOnLine(second_line_point->x, second_line_point->y,
                     first_line_point->x, first_line_point->y,
                     realOffset, &positionOnLineX, &positionOnLineY);
      startPositionX = second_line_point->x;
      startPositionY = second_line_point->y;
      break;
    }
    case ARROW_POSITION_END:
    {
      // If we're using a proportional offset, calculate just where this will
      // be on the line.
      float realOffset = xOffset;
      if (proportionalOffset)
      {
        float totalLength =
          (float)sqrt((second_last_line_point->x - last_line_point->x)*(second_last_line_point->x - last_line_point->x) +
                      (second_last_line_point->y - last_line_point->y)*(second_last_line_point->y - last_line_point->y));
        realOffset = (float)(xOffset * totalLength);
      }
      GetPointOnLine(second_last_line_point->x, second_last_line_point->y,
                     last_line_point->x, last_line_point->y,
                     realOffset, &positionOnLineX, &positionOnLineY);
      startPositionX = second_last_line_point->x;
      startPositionY = second_last_line_point->y;
      break;
    }
    case ARROW_POSITION_MIDDLE:
    {
      // Choose a point half way between the last and penultimate points
      float x = ((last_line_point->x + second_last_line_point->x)/2);
      float y = ((last_line_point->y + second_last_line_point->y)/2);

      // If we're using a proportional offset, calculate just where this will
      // be on the line.
      float realOffset = xOffset;
      if (proportionalOffset)
      {
        float totalLength =
          (float)sqrt((second_last_line_point->x - x)*(second_last_line_point->x - x) +
                      (second_last_line_point->y - y)*(second_last_line_point->y - y));
        realOffset = (float)(xOffset * totalLength);
      }

      GetPointOnLine(second_last_line_point->x, second_last_line_point->y,
                     x, y, realOffset, &positionOnLineX, &positionOnLineY);
      startPositionX = second_last_line_point->x;
      startPositionY = second_last_line_point->y;
      break;
    }
  }

  /*
   * Add yOffset to arrow, if any
   */

  const float myPi = (float) 3.14159265;
  // The translation that the y offset may give
  float deltaX = 0.0;
  float deltaY = 0.0;
  if ((arrow->GetYOffset() != 0.0) && !m_ignoreArrowOffsets)
  {
    /*
                                 |(x4, y4)
                                 |d
                                 |
       (x1, y1)--------------(x3, y3)------------------(x2, y2)
       x4 = x3 - d * sin(theta)
       y4 = y3 + d * cos(theta)

       Where theta = tan(-1) of (y3-y1)/(x3-x1)
     */
     float x1 = startPositionX;
     float y1 = startPositionY;
     float x3 = positionOnLineX;
     float y3 = positionOnLineY;
     float d = -arrow->GetYOffset(); // Negate so +offset is above line

     float theta = 0.0;
     if (x3 == x1)
       theta = (float)(myPi/2.0);
     else
       theta = (float)atan((y3-y1)/(x3-x1));
       
     float x4 = (float)(x3 - (d*sin(theta)));
     float y4 = (float)(y3 + (d*cos(theta)));

     deltaX = x4 - positionOnLineX;
     deltaY = y4 - positionOnLineY;
  }
  
  switch (arrow->_GetType())
  {
    case ARROW_ARROW:
    {
      float arrowLength = arrow->GetSize();
      float arrowWidth = (float)(arrowLength/3.0);

      float tip_x, tip_y, side1_x, side1_y, side2_x, side2_y;
      get_arrow_points(startPositionX+deltaX, startPositionY+deltaY,
                       positionOnLineX+deltaX, positionOnLineY+deltaY,
                       arrowLength, arrowWidth, &tip_x, &tip_y,
                       &side1_x, &side1_y, &side2_x, &side2_y);

      wxPoint points[4];
      points[0].x = tip_x; points[0].y = tip_y;
      points[1].x = side1_x; points[1].y = side1_y;
      points[2].x = side2_x; points[2].y = side2_y;
      points[3].x = tip_x; points[3].y = tip_y;

      dc.SetPen(m_pen);
      dc.SetBrush(m_brush);
      dc.DrawPolygon(4, points);
      break;
    }
    case ARROW_HOLLOW_CIRCLE:
    case ARROW_FILLED_CIRCLE:
    {
      // Find point on line of centre of circle, which is a radius away
      // from the end position
      float diameter = (float)(arrow->GetSize());
      float x, y;
      GetPointOnLine(startPositionX+deltaX, startPositionY+deltaY,
                   positionOnLineX+deltaX, positionOnLineY+deltaY,
                   (float)(diameter/2.0),
                   &x, &y);

      // Convert ellipse centre to top-left coordinates
      float x1 = (float)(x - (diameter/2.0));
      float y1 = (float)(y - (diameter/2.0));

      dc.SetPen(m_pen);
      if (arrow->_GetType() == ARROW_HOLLOW_CIRCLE)
        dc.SetBrush(white_background_brush);
      else
        dc.SetBrush(m_brush);

      dc.DrawEllipse(x1, y1, diameter, diameter);
      break;
    }
    case ARROW_SINGLE_OBLIQUE:
    {
      break;
    }
    case ARROW_METAFILE:
    {
      if (arrow->GetMetaFile())
      {
        // Find point on line of centre of object, which is a half-width away
        // from the end position
        /*
         *                width
         * <-- start pos  <-----><-- positionOnLineX
         *                _____
         * --------------|  x  | <-- e.g. rectangular arrowhead
         *                -----
         */
        float x, y;
        GetPointOnLine(startPositionX, startPositionY,
                   positionOnLineX, positionOnLineY,
                   (float)(arrow->GetMetaFile()->m_width/2.0),
                   &x, &y);

        // Calculate theta for rotating the metafile.
        /*
          |
          |     o(x2, y2)   'o' represents the arrowhead.
          |    /
          |   /
          |  /theta
          | /(x1, y1) 
          |______________________
        */
        float theta = 0.0;
        float x1 = startPositionX;
        float y1 = startPositionY;
        float x2 = positionOnLineX;
        float y2 = positionOnLineY;

        if ((x1 == x2) && (y1 == y2))
          theta = 0.0;

        else if ((x1 == x2) && (y1 > y2))
          theta = (float)(3.0*myPi/2.0);

        else if ((x1 == x2) && (y2 > y1))
          theta = (float)(myPi/2.0);

        else if ((x2 > x1) && (y2 >= y1))
          theta = (float)atan((y2 - y1)/(x2 - x1));

        else if (x2 < x1)
          theta = (float)(myPi + atan((y2 - y1)/(x2 - x1)));

        else if ((x2 > x1) && (y2 < y1))
          theta = (float)(2*myPi + atan((y2 - y1)/(x2 - x1)));

        else
        {
          wxFatalError("Unknown arrowhead rotation case in lines.cc");
        }

        // Rotate about the centre of the object, then place
        // the object on the line.
        if (arrow->GetMetaFile()->GetRotateable())
          arrow->GetMetaFile()->Rotate(0.0, 0.0, theta);

        if (m_erasing)
        {
          // If erasing, just draw a rectangle.
          float minX, minY, maxX, maxY;
          arrow->GetMetaFile()->GetBounds(&minX, &minY, &maxX, &maxY);
          // Make erasing rectangle slightly bigger or you get droppings.
          int extraPixels = 4;
          dc.DrawRectangle((float)(deltaX + x + minX - (extraPixels/2.0)), (float)(deltaY + y + minY - (extraPixels/2.0)),
                           (float)(maxX - minX + extraPixels), (float)(maxY - minY + extraPixels));
        }
        else
          arrow->GetMetaFile()->Draw(dc, x+deltaX, y+deltaY);
      }
      break;
    }
    default:
    {
    }
  }
}

void wxLineShape::OnErase(wxDC& dc)
{
    wxPen *old_pen = m_pen;
    wxBrush *old_brush = m_brush;
    SetPen(white_background_pen);
    SetBrush(white_background_brush);

    float bound_x, bound_y;
    GetBoundingBoxMax(&bound_x, &bound_y);
    if (m_font) dc.SetFont(m_font);

    // Undraw text regions
    for (int i = 0; i < 3; i++)
    {
      wxNode *node = m_regions.Nth(i);
      if (node)
      {
        float x, y;
        wxShapeRegion *region = (wxShapeRegion *)node->Data();
        GetLabelPosition(i, &x, &y);
        EraseRegion(dc, region, x, y);
      }
    }

    // Undraw line
    dc.SetPen(white_background_pen);
    dc.SetBrush(white_background_brush);

    // Drawing over the line only seems to work if the line has a thickness
    // of 1.
    if (old_pen && (old_pen->GetWidth() > 1))
    {
      dc.DrawRectangle((float)(m_xpos - (bound_x/2.0) - 2.0), (float)(m_ypos - (bound_y/2.0) - 2.0),
                        (float)(bound_x+4.0),  (float)(bound_y+4.0));
    }
    else
    {
      m_erasing = TRUE;
      GetEventHandler()->OnDraw(dc);
      GetEventHandler()->OnEraseControlPoints(dc);
      m_erasing = FALSE;
    }

    if (old_pen) SetPen(old_pen);
    if (old_brush) SetBrush(old_brush);
}

void wxLineShape::GetBoundingBoxMin(float *w, float *h)
{
  float x1 = 10000;
  float y1 = 10000;
  float x2 = -10000;
  float y2 = -10000;

  wxNode *node = m_lineControlPoints->First();
  while (node)
  {
    wxRealPoint *point = (wxRealPoint *)node->Data();

    if (point->x < x1) x1 = point->x;
    if (point->y < y1) y1 = point->y;
    if (point->x > x2) x2 = point->x;
    if (point->y > y2) y2 = point->y;

    node = node->Next();
  }
  *w = (float)(x2 - x1);
  *h = (float)(y2 - y1);
}

/*
 * For a node image of interest, finds the position of this arc
 * amongst all the arcs which are attached to THIS SIDE of the node image,
 * and the number of same.
 */
void wxLineShape::FindNth(wxShape *image, int *nth, int *no_arcs, bool incoming)
{
  int n = -1;
  int num = 0;
  wxNode *node = image->GetLines().First();
  int this_attachment;
  if (image == m_to)
    this_attachment = m_attachmentTo;
  else
    this_attachment = m_attachmentFrom;

  // Find number of lines going into/out of this particular attachment point
  while (node)
  {
    wxLineShape *line = (wxLineShape *)node->Data();

    if (line->m_from == image)
    {
      // This is the nth line attached to 'image'
      if ((line == this) && !incoming)
        n = num;

      // Increment num count if this is the same side (attachment number)
      if (line->m_attachmentFrom == this_attachment)
        num ++;
    }

    if (line->m_to == image)
    {
      // This is the nth line attached to 'image'
      if ((line == this) && incoming)
        n = num;

      // Increment num count if this is the same side (attachment number)
      if (line->m_attachmentTo == this_attachment)
        num ++;
    }

    node = node->Next();
  }
  *nth = n;
  *no_arcs = num;
}

void wxLineShape::OnDrawOutline(wxDC& dc, float x, float y, float w, float h)
{
  wxPen *old_pen = m_pen;
  wxBrush *old_brush = m_brush;

  wxPen dottedPen(wxColour(0, 0, 0), 1, wxDOT);
  SetPen(& dottedPen);
  SetBrush( wxTRANSPARENT_BRUSH );

/*
  if (m_isSpline)
    dc.DrawSpline(m_lineControlPoints);
  else
    dc.DrawLines(m_lineControlPoints);
*/
  GetEventHandler()->OnDraw(dc);

  if (old_pen) SetPen(old_pen);
  else SetPen(NULL);
  if (old_brush) SetBrush(old_brush);
  else SetBrush(NULL);
}

bool wxLineShape::OnMovePre(wxDC& dc, float x, float y, float old_x, float old_y, bool display)
{
  float x_offset = x - old_x;
  float y_offset = y - old_y;

  if (m_lineControlPoints && !(x_offset == 0.0 && y_offset == 0.0))
  {
    wxNode *node = m_lineControlPoints->First();
    while (node)
    {
      wxRealPoint *point = (wxRealPoint *)node->Data();
      point->x += x_offset;
      point->y += y_offset;
      node = node->Next();
    }

  }

  // Move temporary label rectangles if necessary
  for (int i = 0; i < 3; i++)
  {
    if (m_labelObjects[i])
    {
      m_labelObjects[i]->Erase(dc);
      float xp, yp, xr, yr;
      GetLabelPosition(i, &xp, &yp);
      wxNode *node = m_regions.Nth(i);
      if (node)
      {
        wxShapeRegion *region = (wxShapeRegion *)node->Data();
        region->GetPosition(&xr, &yr);
      }
      else
      {
        xr = 0.0; yr = 0.0;
      }
      
      m_labelObjects[i]->Move(dc, xp+xr, yp+yr);
    }
  }
  return TRUE;
}

void wxLineShape::OnMoveLink(wxDC& dc, bool moveControlPoints)
{
  if (!m_from || !m_to)
   return;
   
    if (m_lineControlPoints->Number() > 2)
      Initialise();

    // Do each end - nothing in the middle. User has to move other points
    // manually if necessary.
    float end_x, end_y;
    float other_end_x, other_end_y;

    FindLineEndPoints(&end_x, &end_y, &other_end_x, &other_end_y);
    
    wxNode *first = m_lineControlPoints->First();
    wxRealPoint *first_point = (wxRealPoint *)first->Data();
    wxNode *last = m_lineControlPoints->Last();
    wxRealPoint *last_point = (wxRealPoint *)last->Data();

/* This is redundant, surely? Done by SetEnds.
    first_point->x = end_x; first_point->y = end_y;
    last_point->x = other_end_x; last_point->y = other_end_y;
*/

    float oldX = m_xpos;
    float oldY = m_ypos;

    SetEnds(end_x, end_y, other_end_x, other_end_y);

    // Do a second time, because one may depend on the other.
    FindLineEndPoints(&end_x, &end_y, &other_end_x, &other_end_y);
    SetEnds(end_x, end_y, other_end_x, other_end_y);

    // Try to move control points with the arc
    float x_offset = m_xpos - oldX;
    float y_offset = m_ypos - oldY;

//    if (moveControlPoints && m_lineControlPoints && !(x_offset == 0.0 && y_offset == 0.0))
    // Only move control points if it's a self link. And only works if attachment mode is ON.
    if ((m_from == m_to) && m_from->GetAttachmentMode() && moveControlPoints && m_lineControlPoints && !(x_offset == 0.0 && y_offset == 0.0))
    {
      wxNode *node = m_lineControlPoints->First();
      while (node)
      {
        if ((node != m_lineControlPoints->First()) && (node != m_lineControlPoints->Last()))
        {
          wxRealPoint *point = (wxRealPoint *)node->Data();
          point->x += x_offset;
          point->y += y_offset;
        }
        node = node->Next();
      }
    }

    Move(dc, m_xpos, m_ypos);
}

// Finds the x, y points at the two ends of the line.
// This function can be used by e.g. line-routing routines to
// get the actual points on the two node images where the lines will be drawn
// to/from.
void wxLineShape::FindLineEndPoints(float *fromX, float *fromY, float *toX, float *toY)
{
  if (!m_from || !m_to)
   return;
   
  // Do each end - nothing in the middle. User has to move other points
  // manually if necessary.
  float end_x, end_y;
  float other_end_x, other_end_y;

  wxNode *first = m_lineControlPoints->First();
  wxRealPoint *first_point = (wxRealPoint *)first->Data();
  wxNode *last = m_lineControlPoints->Last();
  wxRealPoint *last_point = (wxRealPoint *)last->Data();

  wxNode *second = first->Next();
  wxRealPoint *second_point = (wxRealPoint *)second->Data();

  wxNode *second_last = last->Previous();
  wxRealPoint *second_last_point = (wxRealPoint *)second_last->Data();

  if (m_lineControlPoints->Number() > 2)
  {
    if (m_from->GetAttachmentMode())
    {
      int nth, no_arcs;
      FindNth(m_from, &nth, &no_arcs, FALSE); // Not incoming
      m_from->GetAttachmentPosition(m_attachmentFrom, &end_x, &end_y, nth, no_arcs, this);
    }
    else
      (void) m_from->GetPerimeterPoint(m_from->GetX(), m_from->GetY(),
                                   (float)second_point->x, (float)second_point->y,
                                    &end_x, &end_y);

    if (m_to->GetAttachmentMode())
    {
      int nth, no_arcs;
      FindNth(m_to, &nth, &no_arcs, TRUE); // Incoming
      m_to->GetAttachmentPosition(m_attachmentTo, &other_end_x, &other_end_y, nth, no_arcs, this);
    }
    else
      (void) m_to->GetPerimeterPoint(m_to->GetX(), m_to->GetY(),
                                (float)second_last_point->x, (float)second_last_point->y,
                                &other_end_x, &other_end_y);
  }
  else
  {
    float fromX = m_from->GetX();
    float fromY = m_from->GetY();
    float toX = m_to->GetX();
    float toY = m_to->GetY();

    if (m_from->GetAttachmentMode())
    {
      int nth, no_arcs;
      FindNth(m_from, &nth, &no_arcs, FALSE);
      m_from->GetAttachmentPosition(m_attachmentFrom, &end_x, &end_y, nth, no_arcs, this);
      fromX = end_x;
      fromY = end_y;
    }

    if (m_to->GetAttachmentMode())
    {
      int nth, no_arcs;
      FindNth(m_to, &nth, &no_arcs, TRUE);
      m_to->GetAttachmentPosition(m_attachmentTo, &other_end_x, &other_end_y, nth, no_arcs, this);
      toX = other_end_x;
      toY = other_end_y;
    }

    if (!m_from->GetAttachmentMode())
      (void) m_from->GetPerimeterPoint(m_from->GetX(), m_from->GetY(),
                                  toX, toY,
                                  &end_x, &end_y);

    if (!m_to->GetAttachmentMode())
      (void) m_to->GetPerimeterPoint(m_to->GetX(), m_to->GetY(),
                                fromX, fromY,
                                &other_end_x, &other_end_y);
  }
  *fromX = end_x;
  *fromY = end_y;
  *toX = other_end_x;
  *toY = other_end_y;
}

void wxLineShape::OnDraw(wxDC& dc)
{
  if (m_lineControlPoints)
  {
    if (m_pen)
      dc.SetPen(m_pen);
    if (m_brush)
      dc.SetBrush(m_brush);

    int n = m_lineControlPoints->Number();
    wxPoint *points = new wxPoint[n];
    int i;
    for (i = 0; i < n; i++)
    {
        wxRealPoint* point = (wxRealPoint*) m_lineControlPoints->Nth(i)->Data();
        points[i].x = (int) point->x;
        points[i].y = (int) point->y;
    }

    if (m_isSpline)
      dc.DrawSpline(n, points);
    else
      dc.DrawLines(n, points);

    delete[] points;

    // Problem with pen - if not a solid pen, does strange things
    // to the arrowhead. So make (get) a new pen that's solid.
    if (m_pen && (m_pen->GetStyle() != wxSOLID))
    {
      wxPen *solid_pen =
        wxThePenList->FindOrCreatePen(m_pen->GetColour(), 1, wxSOLID);
      if (solid_pen)
        dc.SetPen(solid_pen);
    }
    DrawArrows(dc);
  }
}

void wxLineShape::OnDrawControlPoints(wxDC& dc)
{
  if (!m_drawHandles)
    return;
    
  // Draw temporary label rectangles if necessary
  for (int i = 0; i < 3; i++)
  {
    if (m_labelObjects[i])
      m_labelObjects[i]->Draw(dc);
  }
  wxShape::OnDrawControlPoints(dc);
}

void wxLineShape::OnEraseControlPoints(wxDC& dc)
{
  // Erase temporary label rectangles if necessary
  for (int i = 0; i < 3; i++)
  {
    if (m_labelObjects[i])
      m_labelObjects[i]->Erase(dc);
  }
  wxShape::OnEraseControlPoints(dc);
}

void wxLineShape::OnDragLeft(bool draw, float x, float y, int keys, int attachment)
{
}

void wxLineShape::OnBeginDragLeft(float x, float y, int keys, int attachment)
{
}

void wxLineShape::OnEndDragLeft(float x, float y, int keys, int attachment)
{
}

/*
void wxLineShape::SetArrowSize(float length, float width)
{
  arrow_length = length;
  arrow_width = width;
}

void wxLineShape::SetStartArrow(int style)
{
  start_style = style;
}

void wxLineShape::SetMiddleArrow(int style)
{
  middle_style = style;
}

void wxLineShape::SetEndArrow(int style)
{
  end_style = style;
}
*/

void wxLineShape::OnDrawContents(wxDC& dc)
{
  if (GetDisableLabel())
    return;

  for (int i = 0; i < 3; i++)
  {
    wxNode *node = m_regions.Nth(i);
    if (node)
    {
      wxShapeRegion *region = (wxShapeRegion *)node->Data();
      float x, y;
      GetLabelPosition(i, &x, &y);
      DrawRegion(dc, region, x, y);
    }
  }
}


#ifdef PROLOGIO
char *wxLineShape::GetFunctor()
{
  return "arc_image";
}
#endif

void wxLineShape::SetTo(wxShape *object)
{
  m_to = object;
}

void wxLineShape::SetFrom(wxShape *object)
{
  m_from = object;
}

void wxLineShape::MakeControlPoints()
{
  if (m_canvas && m_lineControlPoints)
  {
    wxNode *first = m_lineControlPoints->First();
    wxNode *last = m_lineControlPoints->Last();
    wxRealPoint *first_point = (wxRealPoint *)first->Data();
    wxRealPoint *last_point = (wxRealPoint *)last->Data();

    wxLineControlPoint *control = new wxLineControlPoint(m_canvas, this, CONTROL_POINT_SIZE, 
                                               first_point->x, first_point->y,
                                               CONTROL_POINT_ENDPOINT_FROM);
    control->m_point = first_point;
    m_canvas->AddShape(control);
    m_controlPoints.Append(control);


    wxNode *node = first->Next();
    while (node != last)
    {
      wxRealPoint *point = (wxRealPoint *)node->Data();

      control = new wxLineControlPoint(m_canvas, this, CONTROL_POINT_SIZE, 
                                               point->x, point->y,
                                               CONTROL_POINT_LINE);
      control->m_point = point;

      m_canvas->AddShape(control);
      m_controlPoints.Append(control);

      node = node->Next();
    }
    control = new wxLineControlPoint(m_canvas, this, CONTROL_POINT_SIZE, 
                                               last_point->x, last_point->y,
                                               CONTROL_POINT_ENDPOINT_TO);
    control->m_point = last_point;
    m_canvas->AddShape(control);
    m_controlPoints.Append(control);

  }

}

void wxLineShape::ResetControlPoints()
{
  if (m_canvas && m_lineControlPoints && m_controlPoints.Number() > 0)
  {
    wxNode *node = m_controlPoints.First();
    wxNode *control_node = m_lineControlPoints->First();
    while (node && control_node)
    {
      wxRealPoint *point = (wxRealPoint *)control_node->Data();
      wxLineControlPoint *control = (wxLineControlPoint *)node->Data();
      control->SetX(point->x);
      control->SetY(point->y);

      node = node->Next();
      control_node = control_node->Next();
    }
  }
}

#ifdef PROLOGIO
void wxLineShape::WritePrologAttributes(wxExpr *clause)
{
  wxShape::WritePrologAttributes(clause);

  if (m_from)
    clause->AddAttributeValue("from", m_from->GetId());
  if (m_to)
    clause->AddAttributeValue("to", m_to->GetId());

  if (m_attachmentTo != 0)
    clause->AddAttributeValue("attachment_to", (long)m_attachmentTo);
  if (m_attachmentFrom != 0)
    clause->AddAttributeValue("attachment_from", (long)m_attachmentFrom);

  if (m_alignmentStart != 0)
    clause->AddAttributeValue("align_start", (long)m_alignmentStart);
  if (m_alignmentEnd != 0)
    clause->AddAttributeValue("align_end", (long)m_alignmentEnd);

  clause->AddAttributeValue("is_spline", (long)m_isSpline);
  if (m_maintainStraightLines)
    clause->AddAttributeValue("keep_lines_straight", (long)m_maintainStraightLines);

  // Make a list of lists for the (sp)line controls
  wxExpr *list = new wxExpr(PrologList);
  wxNode *node = m_lineControlPoints->First();
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
  clause->AddAttributeValue("controls", list);

  // Write arc arrows in new OGL format, if there are any.
  // This is a list of lists. Each sublist comprises:
  // (arrowType arrowEnd xOffset arrowSize)
  if (m_arcArrows.Number() > 0)
  {
    wxExpr *arrow_list = new wxExpr(PrologList);
    node = m_arcArrows.First();
    while (node)
    {
      wxArrowHead *head = (wxArrowHead *)node->Data();
      wxExpr *head_list = new wxExpr(PrologList);
      head_list->Append(new wxExpr((long)head->_GetType()));
      head_list->Append(new wxExpr((long)head->GetArrowEnd()));
      head_list->Append(new wxExpr(head->GetXOffset()));
      head_list->Append(new wxExpr(head->GetArrowSize()));
      head_list->Append(new wxExpr(PrologString, (head->GetName() ? head->GetName() : "")));
      head_list->Append(new wxExpr(head->GetId()));

      // New members of wxArrowHead
      head_list->Append(new wxExpr(head->GetYOffset()));
      head_list->Append(new wxExpr(head->GetSpacing()));

      arrow_list->Append(head_list);

      node = node->Next();
    }
    clause->AddAttributeValue("arrows", arrow_list);
  }
}

void wxLineShape::ReadPrologAttributes(wxExpr *clause)
{
  wxShape::ReadPrologAttributes(clause);

  int iVal = (int) m_isSpline;
  clause->AssignAttributeValue("is_spline", &iVal);
  m_isSpline = (iVal != 0);

  iVal = (int) m_maintainStraightLines;
  clause->AssignAttributeValue("keep_lines_straight", &iVal);
  m_maintainStraightLines = (iVal != 0);

  clause->AssignAttributeValue("align_start", &m_alignmentStart);
  clause->AssignAttributeValue("align_end", &m_alignmentEnd);

  // Compatibility: check for no regions.
  if (m_regions.Number() == 0)
  {
    wxShapeRegion *newRegion = new wxShapeRegion;
    newRegion->SetName("Middle");
    newRegion->SetSize(150, 50);
    m_regions.Append((wxObject *)newRegion);
    if (m_text.Number() > 0)
    {
      newRegion->ClearText();
      wxNode *node = m_text.First();
      while (node)
      {
        wxShapeTextLine *textLine = (wxShapeTextLine *)node->Data();
        wxNode *next = node->Next();
        newRegion->GetFormattedText().Append((wxObject *)textLine);
        delete node;
        node = next;
      }
    }
  
    newRegion = new wxShapeRegion;
    newRegion->SetName("Start");
    newRegion->SetSize(150, 50);
    m_regions.Append((wxObject *)newRegion);  

    newRegion = new wxShapeRegion;
    newRegion->SetName("End");
    newRegion->SetSize(150, 50);
    m_regions.Append((wxObject *)newRegion);
  }

  m_attachmentTo = 0;
  m_attachmentFrom = 0;

  clause->AssignAttributeValue("attachment_to", &m_attachmentTo);
  clause->AssignAttributeValue("attachmen_from", &m_attachmentFrom);

  wxExpr *line_list = NULL;

  // When image is created, there are default control points. Override
  // them if there are some in the file.
  clause->AssignAttributeValue("controls", &line_list);

  if (line_list)
  {
    // Read a list of lists for the spline controls
    if (m_lineControlPoints)
    {
      ClearPointList(*m_lineControlPoints);
    }
    else
      m_lineControlPoints = new wxList;

    wxExpr *node = line_list->value.first;

    while (node)
    {
      wxExpr *xexpr = node->value.first;
      float x = xexpr->RealValue();

      wxExpr *yexpr = xexpr->next;
      float y = yexpr->RealValue();

      wxRealPoint *point = new wxRealPoint(x, y);
      m_lineControlPoints->Append((wxObject*) point);

      node = node->next;
    }
  }

  // Read arrow list, for new OGL code
  wxExpr *arrow_list = NULL;

  clause->AssignAttributeValue("arrows", &arrow_list);
  if (arrow_list)
  {
    wxExpr *node = arrow_list->value.first;

    while (node)
    {
      WXTYPE arrowType = ARROW_ARROW;
      int arrowEnd = 0;
      float xOffset = 0.0;
      float arrowSize = 0.0;
      wxString arrowName("");
      long arrowId = -1;
      
      wxExpr *type_expr = node->Nth(0);
      wxExpr *end_expr = node->Nth(1);
      wxExpr *dist_expr = node->Nth(2);
      wxExpr *size_expr = node->Nth(3);
      wxExpr *name_expr = node->Nth(4);
      wxExpr *id_expr = node->Nth(5);

      // New members of wxArrowHead
      wxExpr *yOffsetExpr = node->Nth(6);
      wxExpr *spacingExpr = node->Nth(7);

      if (type_expr)
        arrowType = (int)type_expr->IntegerValue();
      if (end_expr)
        arrowEnd = (int)end_expr->IntegerValue();
      if (dist_expr)
        xOffset = dist_expr->RealValue();
      if (size_expr)
        arrowSize = size_expr->RealValue();
      if (name_expr)
        arrowName = name_expr->StringValue();
      if (id_expr)
        arrowId = id_expr->IntegerValue();

      if (arrowId == -1)
        arrowId = NewId();
      else
        RegisterId(arrowId);

      wxArrowHead *arrowHead = AddArrow(arrowType, arrowEnd, arrowSize, xOffset, (char*) (const char*) arrowName, NULL, arrowId);
      if (yOffsetExpr)
        arrowHead->SetYOffset(yOffsetExpr->RealValue());
      if (spacingExpr)
        arrowHead->SetSpacing(spacingExpr->RealValue());

      node = node->next;
    }
  }
}
#endif

void wxLineShape::Copy(wxLineShape& copy)
{
  wxShape::Copy(copy);

  copy.m_isSpline = m_isSpline;
  copy.m_alignmentStart = m_alignmentStart;
  copy.m_alignmentEnd = m_alignmentEnd;
  copy.m_maintainStraightLines = m_maintainStraightLines;
  copy.m_lineOrientations.Clear();
  wxNode *node = m_lineOrientations.First();
  while (node)
  {
    copy.m_lineOrientations.Append(node->Data());
    node = node->Next();
  }

  if (copy.m_lineControlPoints)
  {
    ClearPointList(*copy.m_lineControlPoints);
    delete copy.m_lineControlPoints;
  }

  copy.m_lineControlPoints = new wxList;

  node = m_lineControlPoints->First();
  while (node)
  {
    wxRealPoint *point = (wxRealPoint *)node->Data();
    wxRealPoint *new_point = new wxRealPoint(point->x, point->y);
    copy.m_lineControlPoints->Append((wxObject*) new_point);
    node = node->Next();
  }

/*
  copy.start_style = start_style;
  copy.end_style = end_style;
  copy.middle_style = middle_style;

  copy.arrow_length = arrow_length;
  copy.arrow_width = arrow_width;
*/

  // Copy new OGL stuff
  copy.ClearArrowsAtPosition(-1);
  node = m_arcArrows.First();
  while (node)
  {
    wxArrowHead *arrow = (wxArrowHead *)node->Data();
    copy.m_arcArrows.Append(new wxArrowHead(*arrow));
    node = node->Next();
  }
}

wxShape *wxLineShape::PrivateCopy()
{
  wxLineShape *line = new wxLineShape;
  Copy(*line);
  return line;
}

// Override select, to create/delete temporary label-moving objects
void wxLineShape::Select(bool select, wxDC* dc)
{
  wxShape::Select(select);
  if (select)
  {
    for (int i = 0; i < 3; i++)
    {
      wxNode *node = m_regions.Nth(i);
      if (node)
      {
        wxShapeRegion *region = (wxShapeRegion *)node->Data();
        if (region->m_formattedText.Number() > 0)
        {
          float w, h, x, y, xx, yy;
          region->GetSize(&w, &h);
          region->GetPosition(&x, &y);
          GetLabelPosition(i, &xx, &yy);
          if (m_labelObjects[i])
          {
            m_labelObjects[i]->Select(FALSE);
            m_labelObjects[i]->RemoveFromCanvas(m_canvas);
            delete m_labelObjects[i];
          }
          m_labelObjects[i] = new wxLabelShape(this, region, w, h);
          m_labelObjects[i]->AddToCanvas(m_canvas);
          m_labelObjects[i]->Show(TRUE);
          if (dc)
            m_labelObjects[i]->Move(*dc, (float)(x + xx), (float)(y + yy));
          m_labelObjects[i]->Select(TRUE);
        }
      }
    }
  }
  else
  {
    for (int i = 0; i < 3; i++)
    {
      if (m_labelObjects[i])
      {
        m_labelObjects[i]->Select(FALSE, dc);
        m_labelObjects[i]->RemoveFromCanvas(m_canvas);
        delete m_labelObjects[i];
        m_labelObjects[i] = NULL;
      }
    }
  }
}

/*
 * Line control point
 *
 */

IMPLEMENT_DYNAMIC_CLASS(wxLineControlPoint, wxControlPoint)

wxLineControlPoint::wxLineControlPoint(wxShapeCanvas *theCanvas, wxShape *object, float size, float x, float y, int the_type):
  wxControlPoint(theCanvas, object, size, x, y, the_type)
{
  m_xpos = x;
  m_ypos = y;
  m_type = the_type;
}

wxLineControlPoint::~wxLineControlPoint()
{
}

void wxLineControlPoint::OnDraw(wxDC& dc)
{
  wxRectangleShape::OnDraw(dc);
}

// Implement movement of Line point
void wxLineControlPoint::OnDragLeft(bool draw, float x, float y, int keys, int attachment)
{
  wxClientDC dc(GetCanvas());
  GetCanvas()->PrepareDC(dc);

  dc.SetLogicalFunction(wxXOR);

  wxPen dottedPen(wxColour(0, 0, 0), 1, wxDOT);
  dc.SetPen(dottedPen);
  dc.SetBrush((* wxTRANSPARENT_BRUSH));

  if (m_type == CONTROL_POINT_LINE)
  {
    m_canvas->Snap(&x, &y);

    m_xpos = x; m_ypos = y;
    m_point->x = x; m_point->y = y;

    wxLineShape *lineShape = (wxLineShape *)m_shape;

    wxPen *old_pen = lineShape->GetPen();
    wxBrush *old_brush = lineShape->GetBrush();

    wxPen dottedPen(wxColour(0, 0, 0), 1, wxDOT);
    lineShape->SetPen(& dottedPen);
    lineShape->SetBrush(wxTRANSPARENT_BRUSH);

    lineShape->GetEventHandler()->OnMoveLink(dc, FALSE);

    lineShape->SetPen(old_pen);
    lineShape->SetBrush(old_brush);
  }

  if (m_type == CONTROL_POINT_ENDPOINT_FROM || m_type == CONTROL_POINT_ENDPOINT_TO)
  {
    m_xpos = x; m_ypos = y;
  }

}

void wxLineControlPoint::OnBeginDragLeft(float x, float y, int keys, int attachment)
{
  wxClientDC dc(GetCanvas());
  GetCanvas()->PrepareDC(dc);

  wxLineShape *lineShape = (wxLineShape *)m_shape;
  if (m_type == CONTROL_POINT_LINE)
  {
    m_canvas->Snap(&x, &y);

    m_shape->Erase(dc);

    // Redraw start and end objects because we've left holes
    // when erasing the line
    lineShape->GetFrom()->OnDraw(dc);
    lineShape->GetFrom()->OnDrawContents(dc);
    lineShape->GetTo()->OnDraw(dc);
    lineShape->GetTo()->OnDrawContents(dc);

    m_shape->SetDisableLabel(TRUE);
    dc.SetLogicalFunction(wxXOR);

    m_xpos = x; m_ypos = y;
    m_point->x = x; m_point->y = y;

    wxPen *old_pen = lineShape->GetPen();
    wxBrush *old_brush = lineShape->GetBrush();

    wxPen dottedPen(wxColour(0, 0, 0), 1, wxDOT);
    lineShape->SetPen(& dottedPen);
    lineShape->SetBrush(wxTRANSPARENT_BRUSH);

    lineShape->GetEventHandler()->OnMoveLink(dc, FALSE);

    lineShape->SetPen(old_pen);
    lineShape->SetBrush(old_brush);
  }

  if (m_type == CONTROL_POINT_ENDPOINT_FROM || m_type == CONTROL_POINT_ENDPOINT_TO)
  {
    Erase(dc);
    lineShape->OnDraw(dc);
    if (m_type == CONTROL_POINT_ENDPOINT_FROM)
    {
      lineShape->GetFrom()->OnDraw(dc);
      lineShape->GetFrom()->OnDrawContents(dc);
    }
    else
    {
      lineShape->GetTo()->OnDraw(dc);
      lineShape->GetTo()->OnDrawContents(dc);
    }
    m_canvas->SetCursor(GraphicsBullseyeCursor);
    m_oldCursor = wxSTANDARD_CURSOR;
  }
}
  
void wxLineControlPoint::OnEndDragLeft(float x, float y, int keys, int attachment)
{
  wxClientDC dc(GetCanvas());
  GetCanvas()->PrepareDC(dc);

  m_shape->SetDisableLabel(FALSE);
  wxLineShape *lineShape = (wxLineShape *)m_shape;

  if (m_type == CONTROL_POINT_LINE)
  {
    m_canvas->Snap(&x, &y);

    dc.SetLogicalFunction(wxCOPY);
    m_xpos = x; m_ypos = y;
    m_point->x = x; m_point->y = y;

    lineShape->GetEventHandler()->OnMoveLink(dc);
  }
  if (m_type == CONTROL_POINT_ENDPOINT_FROM)
  {
    if (m_oldCursor)
      m_canvas->SetCursor(m_oldCursor);
    m_shape->Erase(dc);

    m_xpos = x; m_ypos = y;

    if (lineShape->GetFrom())
    {
      lineShape->GetFrom()->MoveLineToNewAttachment(dc, lineShape, x, y);
      lineShape->GetFrom()->MoveLinks(dc);
    }
  }
  if (m_type == CONTROL_POINT_ENDPOINT_TO)
  {
    if (m_oldCursor)
      m_canvas->SetCursor(m_oldCursor);

    m_xpos = x; m_ypos = y;

    if (lineShape->GetTo())
    {
      lineShape->GetTo()->MoveLineToNewAttachment(dc, lineShape, x, y);
      lineShape->GetTo()->MoveLinks(dc);
    }
  }
  int i = 0;
  for (i = 0; i < lineShape->GetLineControlPoints()->Number(); i++)
    if (((wxRealPoint *)(lineShape->GetLineControlPoints()->Nth(i)->Data())) == m_point)
      break;

  // N.B. in OnMoveControlPoint, an event handler in Hardy could have deselected
  // the line and therefore deleted 'this'. -> GPF, intermittently.
  // So assume at this point that we've been blown away.
  wxLineShape *lineObj = lineShape;
  wxShapeCanvas *objCanvas = m_canvas;

  lineObj->OnMoveControlPoint(i+1, x, y);
  
  if (!objCanvas->GetQuickEditMode()) objCanvas->Redraw(dc);
}

// Implement movement of endpoint to a new attachment
void wxLineControlPoint::OnDragRight(bool draw, float x, float y, int keys, int attachment)
{
  if (m_type == CONTROL_POINT_ENDPOINT_FROM || m_type == CONTROL_POINT_ENDPOINT_TO)
  {
    m_xpos = x; m_ypos = y;
  }
}

void wxLineControlPoint::OnBeginDragRight(float x, float y, int keys, int attachment)
{
  wxClientDC dc(GetCanvas());
  GetCanvas()->PrepareDC(dc);

  wxLineShape *lineShape = (wxLineShape *)m_shape;
  if (m_type == CONTROL_POINT_ENDPOINT_FROM || m_type == CONTROL_POINT_ENDPOINT_TO)
  {
    Erase(dc);
    lineShape->GetEventHandler()->OnDraw(dc);
    if (m_type == CONTROL_POINT_ENDPOINT_FROM)
    {
      lineShape->GetFrom()->GetEventHandler()->OnDraw(dc);
      lineShape->GetFrom()->GetEventHandler()->OnDrawContents(dc);
    }
    else
    {
      lineShape->GetTo()->GetEventHandler()->OnDraw(dc);
      lineShape->GetTo()->GetEventHandler()->OnDrawContents(dc);
    }
    m_canvas->SetCursor(GraphicsBullseyeCursor);
    m_oldCursor = wxSTANDARD_CURSOR;
  }
}
  
void wxLineControlPoint::OnEndDragRight(float x, float y, int keys, int attachment)
{
  wxClientDC dc(GetCanvas());
  GetCanvas()->PrepareDC(dc);

  wxLineShape *lineShape = (wxLineShape *)m_shape;
  if (m_type == CONTROL_POINT_ENDPOINT_FROM)
  {
    if (m_oldCursor)
      m_canvas->SetCursor(m_oldCursor);

    m_xpos = x; m_ypos = y;

    if (lineShape->GetFrom())
    {
      lineShape->GetFrom()->EraseLinks(dc);

      int new_attachment;
      float distance;

      if (lineShape->GetFrom()->HitTest(x, y, &new_attachment, &distance))
        lineShape->SetAttachments(new_attachment, lineShape->GetAttachmentTo());

      lineShape->GetFrom()->MoveLinks(dc);
    }
  }
  if (m_type == CONTROL_POINT_ENDPOINT_TO)
  {
    if (m_oldCursor)
      m_canvas->SetCursor(m_oldCursor);
    m_shape->Erase(dc);

    m_xpos = x; m_ypos = y;

    if (lineShape->GetTo())
    {
      lineShape->GetTo()->EraseLinks(dc);

      int new_attachment;
      float distance;
      if (lineShape->GetTo()->HitTest(x, y, &new_attachment, &distance))
        lineShape->SetAttachments(lineShape->GetAttachmentFrom(), new_attachment);

      lineShape->GetTo()->MoveLinks(dc);
    }
  }
  int i = 0;
  for (i = 0; i < lineShape->GetLineControlPoints()->Number(); i++)
    if (((wxRealPoint *)(lineShape->GetLineControlPoints()->Nth(i)->Data())) == m_point)
      break;
  lineShape->OnMoveControlPoint(i+1, x, y);
  if (!m_canvas->GetQuickEditMode()) m_canvas->Redraw(dc);
}

/*
 * Get the point on the given line (x1, y1) (x2, y2)
 * distance 'length' along from the end,
 * returned values in x and y
 */

void GetPointOnLine(float x1, float y1, float x2, float y2,
                    float length, float *x, float *y)
{
  float l = (float)sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));

  if (l < 0.01)
    l = (float) 0.01;

  float i_bar = (x2 - x1)/l;
  float j_bar = (y2 - y1)/l;

  *x = (- length*i_bar) + x2;
  *y = (- length*j_bar) + y2;
}

wxArrowHead *wxLineShape::AddArrow(WXTYPE type, int end, float size, float xOffset,
    const wxString& name, wxPseudoMetaFile *mf, long arrowId)
{
  wxArrowHead *arrow = new wxArrowHead(type, end, size, xOffset, name, mf, arrowId);
  m_arcArrows.Append(arrow);
  return arrow;
}

/*
 * Add arrowhead at a particular position in the arrowhead list.
 */
bool wxLineShape::AddArrowOrdered(wxArrowHead *arrow, wxList& referenceList, int end)
{
  wxNode *refNode = referenceList.First();
  wxNode *currNode = m_arcArrows.First();
  wxString targetName(arrow->GetName());
  if (!refNode) return FALSE;

  // First check whether we need to insert in front of list,
  // because this arrowhead is the first in the reference
  // list and should therefore be first in the current list.
  wxArrowHead *refArrow = (wxArrowHead *)refNode->Data();
  if (refArrow->GetName() == targetName)
  {
    m_arcArrows.Insert(arrow);
    return TRUE;
  }

  while (refNode && currNode)
  {
    wxArrowHead *currArrow = (wxArrowHead *)currNode->Data();
    refArrow = (wxArrowHead *)refNode->Data();

    // Matching: advance current arrow pointer
    if ((currArrow->GetArrowEnd() == end) &&
        (currArrow->GetName() == refArrow->GetName()))
    {
      currNode = currNode->Next(); // Could be NULL now
      if (currNode)
        currArrow = (wxArrowHead *)currNode->Data();
    }

    // Check if we're at the correct position in the
    // reference list
    if (targetName == refArrow->GetName())
    {
      if (currNode)
        m_arcArrows.Insert(currNode, arrow);
      else
        m_arcArrows.Append(arrow);
      return TRUE;
    }
    refNode = refNode->Next();
  }
  m_arcArrows.Append(arrow);
  return TRUE;
}

void wxLineShape::ClearArrowsAtPosition(int end)
{
  wxNode *node = m_arcArrows.First();
  while (node)
  {
    wxArrowHead *arrow = (wxArrowHead *)node->Data();
    wxNode *next = node->Next();
    switch (end)
    {
      case -1:
      {
        delete arrow;
        delete node;
        break;
      }
      case ARROW_POSITION_START:
      {
        if (arrow->GetArrowEnd() == ARROW_POSITION_START)
        {
          delete arrow;
          delete node;
        }
        break;
      }
      case ARROW_POSITION_END:
      {
        if (arrow->GetArrowEnd() == ARROW_POSITION_END)
        {
          delete arrow;
          delete node;
        }
        break;
      }
      case ARROW_POSITION_MIDDLE:
      {
        if (arrow->GetArrowEnd() == ARROW_POSITION_MIDDLE)
        {
          delete arrow;
          delete node;
        }
        break;
      }
    }
    node = next;
  }
}

bool wxLineShape::ClearArrow(const wxString& name)
{
  wxNode *node = m_arcArrows.First();
  while (node)
  {
    wxArrowHead *arrow = (wxArrowHead *)node->Data();
    if (arrow->GetName() == name)
    {
      delete arrow;
      delete node;
      return TRUE;
    }
    node = node->Next();
  }
  return FALSE;
}

/*
 * Finds an arrowhead at the given position (if -1, any position)
 *
 */
 
wxArrowHead *wxLineShape::FindArrowHead(int position, const wxString& name)
{
  wxNode *node = m_arcArrows.First();
  while (node)
  {
    wxArrowHead *arrow = (wxArrowHead *)node->Data();
    if (((position == -1) || (position == arrow->GetArrowEnd())) &&
        (arrow->GetName() == name))
      return arrow;
    node = node->Next();
  }
  return NULL;
}

wxArrowHead *wxLineShape::FindArrowHead(long arrowId)
{
  wxNode *node = m_arcArrows.First();
  while (node)
  {
    wxArrowHead *arrow = (wxArrowHead *)node->Data();
    if (arrowId == arrow->GetId())
      return arrow;
    node = node->Next();
  }
  return NULL;
}

/*
 * Deletes an arrowhead at the given position (if -1, any position)
 *
 */
 
bool wxLineShape::DeleteArrowHead(int position, const wxString& name)
{
  wxNode *node = m_arcArrows.First();
  while (node)
  {
    wxArrowHead *arrow = (wxArrowHead *)node->Data();
    if (((position == -1) || (position == arrow->GetArrowEnd())) &&
        (arrow->GetName() == name))
    {
      delete arrow;
      delete node;
      return TRUE;
    }
    node = node->Next();
  }
  return FALSE;
}

// Overloaded DeleteArrowHead: pass arrowhead id.
bool wxLineShape::DeleteArrowHead(long id)
{
  wxNode *node = m_arcArrows.First();
  while (node)
  {
    wxArrowHead *arrow = (wxArrowHead *)node->Data();
    if (arrow->GetId() == id)
    {
      delete arrow;
      delete node;
      return TRUE;
    }
    node = node->Next();
  }
  return FALSE;
}

/*
 * Calculate the minimum width a line
 * occupies, for the purposes of drawing lines in tools.
 *
 */

float wxLineShape::FindMinimumWidth()
{
  float minWidth = 0.0;
  wxNode *node = m_arcArrows.First();
  while (node)
  {
    wxArrowHead *arrowHead = (wxArrowHead *)node->Data();
    minWidth += arrowHead->GetSize();
    if (node->Next())
      minWidth += arrowHead->GetSpacing();

    node = node->Next();
  }
  // We have ABSOLUTE minimum now. So
  // scale it to give it reasonable aesthetics
  // when drawing with line.
  if (minWidth > 0.0)
    minWidth = (float)(minWidth * 1.4);
  else
    minWidth = 20.0;

  SetEnds(0.0, 0.0, minWidth, 0.0);
  Initialise();

  return minWidth;
}

// Find which position we're talking about at this (x, y).
// Returns ARROW_POSITION_START, ARROW_POSITION_MIDDLE, ARROW_POSITION_END
int wxLineShape::FindLinePosition(float x, float y)
{
  float startX, startY, endX, endY;
  GetEnds(&startX, &startY, &endX, &endY);
  
  // Find distances from centre, start and end. The smallest wins.
  float centreDistance = (float)(sqrt((x - m_xpos)*(x - m_xpos) + (y - m_ypos)*(y - m_ypos)));
  float startDistance = (float)(sqrt((x - startX)*(x - startX) + (y - startY)*(y - startY)));
  float endDistance = (float)(sqrt((x - endX)*(x - endX) + (y - endY)*(y - endY)));

  if (centreDistance < startDistance && centreDistance < endDistance)
    return ARROW_POSITION_MIDDLE;
  else if (startDistance < endDistance)
    return ARROW_POSITION_START;
  else
    return ARROW_POSITION_END;
}

// Set alignment flags
void wxLineShape::SetAlignmentOrientation(bool isEnd, bool isHoriz)
{
  if (isEnd)
  {
    if (isHoriz && ((m_alignmentEnd & LINE_ALIGNMENT_HORIZ) != LINE_ALIGNMENT_HORIZ))
      m_alignmentEnd |= LINE_ALIGNMENT_HORIZ;
    else if (!isHoriz && ((m_alignmentEnd & LINE_ALIGNMENT_HORIZ) == LINE_ALIGNMENT_HORIZ))
      m_alignmentEnd -= LINE_ALIGNMENT_HORIZ;
  }
  else
  {
    if (isHoriz && ((m_alignmentStart & LINE_ALIGNMENT_HORIZ) != LINE_ALIGNMENT_HORIZ))
      m_alignmentStart |= LINE_ALIGNMENT_HORIZ;
    else if (!isHoriz && ((m_alignmentStart & LINE_ALIGNMENT_HORIZ) == LINE_ALIGNMENT_HORIZ))
      m_alignmentStart -= LINE_ALIGNMENT_HORIZ;
  }
}

void wxLineShape::SetAlignmentType(bool isEnd, int alignType)
{
  if (isEnd)
  {
    if (alignType == LINE_ALIGNMENT_TO_NEXT_HANDLE)
    {
      if ((m_alignmentEnd & LINE_ALIGNMENT_TO_NEXT_HANDLE) != LINE_ALIGNMENT_TO_NEXT_HANDLE)
        m_alignmentEnd |= LINE_ALIGNMENT_TO_NEXT_HANDLE;
    }
    else if ((m_alignmentEnd & LINE_ALIGNMENT_TO_NEXT_HANDLE) == LINE_ALIGNMENT_TO_NEXT_HANDLE)
      m_alignmentEnd -= LINE_ALIGNMENT_TO_NEXT_HANDLE;
  }
  else
  {
    if (alignType == LINE_ALIGNMENT_TO_NEXT_HANDLE)
    {
      if ((m_alignmentStart & LINE_ALIGNMENT_TO_NEXT_HANDLE) != LINE_ALIGNMENT_TO_NEXT_HANDLE)
        m_alignmentStart |= LINE_ALIGNMENT_TO_NEXT_HANDLE;
    }
    else if ((m_alignmentStart & LINE_ALIGNMENT_TO_NEXT_HANDLE) == LINE_ALIGNMENT_TO_NEXT_HANDLE)
      m_alignmentStart -= LINE_ALIGNMENT_TO_NEXT_HANDLE;
  }
}

bool wxLineShape::GetAlignmentOrientation(bool isEnd)
{
  if (isEnd)
    return ((m_alignmentEnd & LINE_ALIGNMENT_HORIZ) == LINE_ALIGNMENT_HORIZ);
  else
    return ((m_alignmentStart & LINE_ALIGNMENT_HORIZ) == LINE_ALIGNMENT_HORIZ);
}

int wxLineShape::GetAlignmentType(bool isEnd)
{
  if (isEnd)
    return (m_alignmentEnd & LINE_ALIGNMENT_TO_NEXT_HANDLE);
  else
    return (m_alignmentStart & LINE_ALIGNMENT_TO_NEXT_HANDLE);
}

wxRealPoint *wxLineShape::GetNextControlPoint(wxShape *nodeObject)
{
  int n = m_lineControlPoints->Number();
  int nn = 0;
  if (m_to == nodeObject)
  {
    // Must be END of line, so we want (n - 1)th control point.
    // But indexing ends at n-1, so subtract 2.
    nn = n - 2;
  }
  else nn = 1;
  wxNode *node = m_lineControlPoints->Nth(nn);
  if (node)
  {
    return (wxRealPoint *)node->Data();
  }
  else
    return FALSE;
}

/*
 * Arrowhead
 *
 */

IMPLEMENT_DYNAMIC_CLASS(wxArrowHead, wxObject)

wxArrowHead::wxArrowHead(WXTYPE type, int end, float size, float dist, const wxString& name,
                     wxPseudoMetaFile *mf, long arrowId)
{
  m_arrowType = type; m_arrowEnd = end; m_arrowSize = size;
  m_xOffset = dist;
  m_yOffset = 0.0;
  m_spacing = 5.0;

  m_arrowName = name;
  m_metaFile = mf;
  m_id = arrowId;
  if (m_id == -1)
    m_id = NewId();
}

wxArrowHead::wxArrowHead(wxArrowHead& toCopy)
{
  m_arrowType = toCopy.m_arrowType; m_arrowEnd = toCopy.GetArrowEnd();
  m_arrowSize = toCopy.m_arrowSize;
  m_xOffset = toCopy.m_xOffset;
  m_yOffset = toCopy.m_yOffset;
  m_spacing = toCopy.m_spacing;
  m_arrowName = toCopy.m_arrowName ;
  if (toCopy.m_metaFile)
    m_metaFile = new wxPseudoMetaFile(*(toCopy.m_metaFile));
  else
    m_metaFile = NULL;
  m_id = NewId();
}
 
wxArrowHead::~wxArrowHead()
{
  if (m_metaFile) delete m_metaFile;
}

void wxArrowHead::SetSize(float size)
{
  m_arrowSize = size;
  if ((m_arrowType == ARROW_METAFILE) && m_metaFile)
  {
    float oldWidth = m_metaFile->m_width;
    if (oldWidth == 0.0)
      return;
      
    float scale = (float)(size/oldWidth);
    if (scale != 1.0)
      m_metaFile->Scale(scale, scale);
  }
}

/*
 * Label object
 *
 */

IMPLEMENT_DYNAMIC_CLASS(wxLabelShape, wxRectangleShape)

wxLabelShape::wxLabelShape(wxLineShape *parent, wxShapeRegion *region, float w, float h):wxRectangleShape(w, h)
{
  m_lineShape = parent;
  m_shapeRegion = region;
  SetPen(wxThePenList->FindOrCreatePen(wxColour(0, 0, 0), 1, wxDOT));
}

wxLabelShape::~wxLabelShape()
{
}

void wxLabelShape::OnDraw(wxDC& dc)
{
  if (m_lineShape && !m_lineShape->GetDrawHandles())
    return;
    
    float x1 = (float)(m_xpos - m_width/2.0);
    float y1 = (float)(m_ypos - m_height/2.0);

    if (m_pen)
    {
      if (m_pen->GetWidth() == 0)
        dc.SetPen(transparent_pen);
      else
        dc.SetPen(m_pen);
    }
    dc.SetBrush(wxTRANSPARENT_BRUSH);

    if (m_cornerRadius > 0.0)
      dc.DrawRoundedRectangle(x1, y1, m_width, m_height, m_cornerRadius);
    else
      dc.DrawRectangle(x1, y1, m_width, m_height);
}

void wxLabelShape::OnDrawContents(wxDC& dc)
{
}

void wxLabelShape::OnDragLeft(bool draw, float x, float y, int keys, int attachment)
{
  wxRectangleShape::OnDragLeft(draw, x, y, keys, attachment);
}

void wxLabelShape::OnBeginDragLeft(float x, float y, int keys, int attachment)
{
  wxRectangleShape::OnBeginDragLeft(x, y, keys, attachment);
}

void wxLabelShape::OnEndDragLeft(float x, float y, int keys, int attachment)
{
  wxRectangleShape::OnEndDragLeft(x, y, keys, attachment);
}

bool wxLabelShape::OnMovePre(wxDC& dc, float x, float y, float old_x, float old_y, bool display)
{
  m_shapeRegion->SetSize(m_width, m_height);

  // Find position in line's region list
  int i = 0;
  wxNode *node = m_lineShape->GetRegions().First();
  while (node)
  {
    if (m_shapeRegion == (wxShapeRegion *)node->Data())
      node = NULL;
    else
    {
      node = node->Next();
      i ++;
    }
  }
  float xx, yy;
  m_lineShape->GetLabelPosition(i, &xx, &yy);
  // Set the region's offset, relative to the default position for
  // each region.
  m_shapeRegion->SetPosition((float)(x - xx), (float)(y - yy));

  // Need to reformat to fit region.
  if (m_shapeRegion->GetText())
  {

    wxString s(m_shapeRegion->GetText());
    m_lineShape->FormatText(dc, s, i);
    m_lineShape->DrawRegion(dc, m_shapeRegion, xx, yy);
  }
  return TRUE;
}

// Divert left and right clicks to line object
void wxLabelShape::OnLeftClick(float x, float y, int keys, int attachment)
{
  m_lineShape->GetEventHandler()->OnLeftClick(x, y, keys, attachment);
}

void wxLabelShape::OnRightClick(float x, float y, int keys, int attachment)
{
  m_lineShape->GetEventHandler()->OnRightClick(x, y, keys, attachment);
}

