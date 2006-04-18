/////////////////////////////////////////////////////////////////////////////
// Name:        lines.cpp
// Purpose:     wxLineShape
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

#include "wx/ogl/ogl.h"


// Line shape
IMPLEMENT_DYNAMIC_CLASS(wxLineShape, wxShape)

wxLineShape::wxLineShape()
{
  m_sensitivity = OP_CLICK_LEFT | OP_CLICK_RIGHT;
  m_draggable = false;
  m_attachmentTo = 0;
  m_attachmentFrom = 0;
/*
  m_actualTextWidth = 0.0;
  m_actualTextHeight = 0.0;
*/
  m_from = NULL;
  m_to = NULL;
  m_erasing = false;
  m_arrowSpacing = 5.0; // For the moment, don't bother saving this to file.
  m_ignoreArrowOffsets = false;
  m_isSpline = false;
  m_maintainStraightLines = false;
  m_alignmentStart = 0;
  m_alignmentEnd = 0;

  m_lineControlPoints = NULL;

  // Clear any existing regions (created in an earlier constructor)
  // and make the three line regions.
  ClearRegions();
  wxShapeRegion *newRegion = new wxShapeRegion;
  newRegion->SetName(wxT("Middle"));
  newRegion->SetSize(150, 50);
  m_regions.Append((wxObject *)newRegion);

  newRegion = new wxShapeRegion;
  newRegion->SetName(wxT("Start"));
  newRegion->SetSize(150, 50);
  m_regions.Append((wxObject *)newRegion);

  newRegion = new wxShapeRegion;
  newRegion->SetName(wxT("End"));
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
      m_labelObjects[i]->Select(false);
      m_labelObjects[i]->RemoveFromCanvas(m_canvas);
      delete m_labelObjects[i];
      m_labelObjects[i] = NULL;
    }
  }
  ClearArrowsAtPosition(-1);
}

void wxLineShape::MakeLineControlPoints(int n)
{
  if (m_lineControlPoints)
  {
    ClearPointList(*m_lineControlPoints);
    delete m_lineControlPoints;
  }
  m_lineControlPoints = new wxList;

  for (int i = 0; i < n; i++)
  {
    wxRealPoint *point = new wxRealPoint(-999, -999);
    m_lineControlPoints->Append((wxObject*) point);
  }
}

wxNode *wxLineShape::InsertLineControlPoint(wxDC* dc)
{
    if (dc)
        Erase(*dc);

  wxNode *last = m_lineControlPoints->GetLast();
  wxNode *second_last = last->GetPrevious();
  wxRealPoint *last_point = (wxRealPoint *)last->GetData();
  wxRealPoint *second_last_point = (wxRealPoint *)second_last->GetData();

  // Choose a point half way between the last and penultimate points
  double line_x = ((last_point->x + second_last_point->x)/2);
  double line_y = ((last_point->y + second_last_point->y)/2);

  wxRealPoint *point = new wxRealPoint(line_x, line_y);
  wxNode *node = m_lineControlPoints->Insert(last, (wxObject*) point);
  return node;
}

bool wxLineShape::DeleteLineControlPoint()
{
  if (m_lineControlPoints->GetCount() < 3)
    return false;

  wxNode *last = m_lineControlPoints->GetLast();
  wxNode *second_last = last->GetPrevious();

  wxRealPoint *second_last_point = (wxRealPoint *)second_last->GetData();
  delete second_last_point;
  delete second_last;

  return true;
}

void wxLineShape::Initialise()
{
  if (m_lineControlPoints)
  {
    // Just move the first and last control points
    wxNode *first = m_lineControlPoints->GetFirst();
    wxRealPoint *first_point = (wxRealPoint *)first->GetData();

    wxNode *last = m_lineControlPoints->GetLast();
    wxRealPoint *last_point = (wxRealPoint *)last->GetData();

    // If any of the line points are at -999, we must
    // initialize them by placing them half way between the first
    // and the last.
    wxNode *node = first->GetNext();
    while (node)
    {
      wxRealPoint *point = (wxRealPoint *)node->GetData();
      if (point->x == -999)
      {
        double x1, y1, x2, y2;
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
      node = node->GetNext();
    }
  }
}

// Format a text string according to the region size, adding
// strings with positions to region text list
void wxLineShape::FormatText(wxDC& dc, const wxString& s, int i)
{
  double w, h;
  ClearText(i);

  if (m_regions.GetCount() < 1)
    return;
  wxNode *node = m_regions.Item(i);
  if (!node)
    return;

  wxShapeRegion *region = (wxShapeRegion *)node->GetData();
  region->SetText(s);
  dc.SetFont(* region->GetFont());

  region->GetSize(&w, &h);
  // Initialize the size if zero
  if (((w == 0) || (h == 0)) && (s.Length() > 0))
  {
    w = 100; h = 50;
    region->SetSize(w, h);
  }

  wxStringList *string_list = oglFormatText(dc, s, (w-5), (h-5), region->GetFormatMode());
  node = (wxNode*)string_list->GetFirst();
  while (node)
  {
    wxChar *s = (wxChar *)node->GetData();
    wxShapeTextLine *line = new wxShapeTextLine(0.0, 0.0, s);
    region->GetFormattedText().Append((wxObject *)line);
    node = node->GetNext();
  }
  delete string_list;
  double actualW = w;
  double actualH = h;
  if (region->GetFormatMode() & FORMAT_SIZE_TO_CONTENTS)
  {
    oglGetCentredTextExtent(dc, &(region->GetFormattedText()), m_xpos, m_ypos, w, h, &actualW, &actualH);
    if ((actualW != w ) || (actualH != h))
    {
      double xx, yy;
      GetLabelPosition(i, &xx, &yy);
      EraseRegion(dc, region, xx, yy);
      if (m_labelObjects[i])
      {
        m_labelObjects[i]->Select(false, &dc);
        m_labelObjects[i]->Erase(dc);
        m_labelObjects[i]->SetSize(actualW, actualH);
      }

      region->SetSize(actualW, actualH);

      if (m_labelObjects[i])
      {
        m_labelObjects[i]->Select(true, & dc);
        m_labelObjects[i]->Draw(dc);
      }
    }
  }
  oglCentreText(dc, &(region->GetFormattedText()), m_xpos, m_ypos, actualW, actualH, region->GetFormatMode());
  m_formatted = true;
}

void wxLineShape::DrawRegion(wxDC& dc, wxShapeRegion *region, double x, double y)
{
  if (GetDisableLabel())
    return;

  double w, h;
  double xx, yy;
  region->GetSize(&w, &h);

  // Get offset from x, y
  region->GetPosition(&xx, &yy);

  double xp = xx + x;
  double yp = yy + y;

  // First, clear a rectangle for the text IF there is any
  if (region->GetFormattedText().GetCount() > 0)
  {
      dc.SetPen(GetBackgroundPen());
      dc.SetBrush(GetBackgroundBrush());

      // Now draw the text
      if (region->GetFont()) dc.SetFont(* region->GetFont());

      dc.DrawRectangle((long)(xp - w/2.0), (long)(yp - h/2.0), (long)w, (long)h);

      if (m_pen) dc.SetPen(* m_pen);
      dc.SetTextForeground(region->GetActualColourObject());

#ifdef __WXMSW__
      dc.SetTextBackground(GetBackgroundBrush().GetColour());
#endif

      oglDrawFormattedText(dc, &(region->GetFormattedText()), xp, yp, w, h, region->GetFormatMode());
  }
}

void wxLineShape::EraseRegion(wxDC& dc, wxShapeRegion *region, double x, double y)
{
  if (GetDisableLabel())
    return;

  double w, h;
  double xx, yy;
  region->GetSize(&w, &h);

  // Get offset from x, y
  region->GetPosition(&xx, &yy);

  double xp = xx + x;
  double yp = yy + y;

  if (region->GetFormattedText().GetCount() > 0)
  {
      dc.SetPen(GetBackgroundPen());
      dc.SetBrush(GetBackgroundBrush());

      dc.DrawRectangle((long)(xp - w/2.0), (long)(yp - h/2.0), (long)w, (long)h);
  }
}

// Get the reference point for a label. Region x and y
// are offsets from this.
// position is 0, 1, 2
void wxLineShape::GetLabelPosition(int position, double *x, double *y)
{
  switch (position)
  {
    case 0:
    {
      // Want to take the middle section for the label
      int n = m_lineControlPoints->GetCount();
      int half_way = (int)(n/2);

      // Find middle of this line
      wxNode *node = m_lineControlPoints->Item(half_way - 1);
      wxRealPoint *point = (wxRealPoint *)node->GetData();
      wxNode *next_node = node->GetNext();
      wxRealPoint *next_point = (wxRealPoint *)next_node->GetData();

      double dx = (next_point->x - point->x);
      double dy = (next_point->y - point->y);
      *x = (double)(point->x + dx/2.0);
      *y = (double)(point->y + dy/2.0);
      break;
    }
    case 1:
    {
      wxNode *node = m_lineControlPoints->GetFirst();
      *x = ((wxRealPoint *)node->GetData())->x;
      *y = ((wxRealPoint *)node->GetData())->y;
      break;
    }
    case 2:
    {
      wxNode *node = m_lineControlPoints->GetLast();
      *x = ((wxRealPoint *)node->GetData())->x;
      *y = ((wxRealPoint *)node->GetData())->y;
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
  double dx = point2->x - point1->x;
  double dy = point2->y - point1->y;

  if (dx == 0.0)
    return;
  else if (fabs(dy/dx) > 1.0)
  {
    point2->x = point1->x;
  }
  else point2->y = point1->y;
}

void wxLineShape::Straighten(wxDC *dc)
{
  if (!m_lineControlPoints || m_lineControlPoints->GetCount() < 3)
    return;

  if (dc)
    Erase(* dc);

  wxNode *first_point_node = m_lineControlPoints->GetFirst();
  wxNode *last_point_node = m_lineControlPoints->GetLast();
  wxNode *second_last_point_node = last_point_node->GetPrevious();

  wxRealPoint *last_point = (wxRealPoint *)last_point_node->GetData();
  wxRealPoint *second_last_point = (wxRealPoint *)second_last_point_node->GetData();

  GraphicsStraightenLine(last_point, second_last_point);

  wxNode *node = first_point_node;
  while (node && (node != second_last_point_node))
  {
    wxRealPoint *point = (wxRealPoint *)node->GetData();
    wxRealPoint *next_point = (wxRealPoint *)(node->GetNext()->GetData());

    GraphicsStraightenLine(point, next_point);
    node = node->GetNext();
  }

  if (dc)
    Draw(* dc);
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

void wxLineShape::SetEnds(double x1, double y1, double x2, double y2)
{
  // Find centre point
  wxNode *first_point_node = m_lineControlPoints->GetFirst();
  wxNode *last_point_node = m_lineControlPoints->GetLast();
  wxRealPoint *first_point = (wxRealPoint *)first_point_node->GetData();
  wxRealPoint *last_point = (wxRealPoint *)last_point_node->GetData();

  first_point->x = x1;
  first_point->y = y1;
  last_point->x = x2;
  last_point->y = y2;

  m_xpos = (double)((x1 + x2)/2.0);
  m_ypos = (double)((y1 + y2)/2.0);
}

// Get absolute positions of ends
void wxLineShape::GetEnds(double *x1, double *y1, double *x2, double *y2)
{
  wxNode *first_point_node = m_lineControlPoints->GetFirst();
  wxNode *last_point_node = m_lineControlPoints->GetLast();
  wxRealPoint *first_point = (wxRealPoint *)first_point_node->GetData();
  wxRealPoint *last_point = (wxRealPoint *)last_point_node->GetData();

  *x1 = first_point->x; *y1 = first_point->y;
  *x2 = last_point->x; *y2 = last_point->y;
}

void wxLineShape::SetAttachments(int from_attach, int to_attach)
{
  m_attachmentFrom = from_attach;
  m_attachmentTo = to_attach;
}

bool wxLineShape::HitTest(double x, double y, int *attachment, double *distance)
{
  if (!m_lineControlPoints)
    return false;

  // Look at label regions in case mouse is over a label
  bool inLabelRegion = false;
  for (int i = 0; i < 3; i ++)
  {
    wxNode *regionNode = m_regions.Item(i);
    if (regionNode)
    {
      wxShapeRegion *region = (wxShapeRegion *)regionNode->GetData();
      if (region->m_formattedText.GetCount() > 0)
      {
        double xp, yp, cx, cy, cw, ch;
        GetLabelPosition(i, &xp, &yp);
        // Offset region from default label position
        region->GetPosition(&cx, &cy);
        region->GetSize(&cw, &ch);
        cx += xp;
        cy += yp;
        double rLeft = (double)(cx - (cw/2.0));
        double rTop = (double)(cy - (ch/2.0));
        double rRight = (double)(cx + (cw/2.0));
        double rBottom = (double)(cy + (ch/2.0));
        if (x > rLeft && x < rRight && y > rTop && y < rBottom)
        {
          inLabelRegion = true;
          i = 3;
        }
      }
    }
  }

  wxNode *node = m_lineControlPoints->GetFirst();

  while (node && node->GetNext())
  {
    wxRealPoint *point1 = (wxRealPoint *)node->GetData();
    wxRealPoint *point2 = (wxRealPoint *)node->GetNext()->GetData();

    // For inaccurate mousing allow 8 pixel corridor
    int extra = 4;

    double dx = point2->x - point1->x;
    double dy = point2->y - point1->y;
    double seg_len = sqrt(dx*dx+dy*dy);
    double distance_from_seg =
      seg_len*((x-point1->x)*dy-(y-point1->y)*dx)/(dy*dy+dx*dx);
    double distance_from_prev =
      seg_len*((y-point1->y)*dy+(x-point1->x)*dx)/(dy*dy+dx*dx);

    if ((fabs(distance_from_seg) < extra &&
         distance_from_prev >= 0 && distance_from_prev <= seg_len)
        || inLabelRegion)
    {
      *attachment = 0;
      *distance = distance_from_seg;
      return true;
    }

    node = node->GetNext();
  }
  return false;
}

void wxLineShape::DrawArrows(wxDC& dc)
{
  // Distance along line of each arrow: space them out evenly.
  double startArrowPos = 0.0;
  double endArrowPos = 0.0;
  double middleArrowPos = 0.0;

  wxNode *node = m_arcArrows.GetFirst();
  while (node)
  {
    wxArrowHead *arrow = (wxArrowHead *)node->GetData();
    switch (arrow->GetArrowEnd())
    {
      case ARROW_POSITION_START:
      {
        if ((arrow->GetXOffset() != 0.0) && !m_ignoreArrowOffsets)
          // If specified, x offset is proportional to line length
          DrawArrow(dc, arrow, arrow->GetXOffset(), true);
        else
        {
          DrawArrow(dc, arrow, startArrowPos, false);      // Absolute distance
          startArrowPos += arrow->GetSize() + arrow->GetSpacing();
        }
        break;
      }
      case ARROW_POSITION_END:
      {
        if ((arrow->GetXOffset() != 0.0) && !m_ignoreArrowOffsets)
          DrawArrow(dc, arrow, arrow->GetXOffset(), true);
        else
        {
          DrawArrow(dc, arrow, endArrowPos, false);
          endArrowPos += arrow->GetSize() + arrow->GetSpacing();
        }
        break;
      }
      case ARROW_POSITION_MIDDLE:
      {
        arrow->SetXOffset(middleArrowPos);
        if ((arrow->GetXOffset() != 0.0) && !m_ignoreArrowOffsets)
          DrawArrow(dc, arrow, arrow->GetXOffset(), true);
        else
        {
          DrawArrow(dc, arrow, middleArrowPos, false);
          middleArrowPos += arrow->GetSize() + arrow->GetSpacing();
        }
        break;
      }
    }
    node = node->GetNext();
  }
}

void wxLineShape::DrawArrow(wxDC& dc, wxArrowHead *arrow, double xOffset, bool proportionalOffset)
{
  wxNode *first_line_node = m_lineControlPoints->GetFirst();
  wxRealPoint *first_line_point = (wxRealPoint *)first_line_node->GetData();
  wxNode *second_line_node = first_line_node->GetNext();
  wxRealPoint *second_line_point = (wxRealPoint *)second_line_node->GetData();

  wxNode *last_line_node = m_lineControlPoints->GetLast();
  wxRealPoint *last_line_point = (wxRealPoint *)last_line_node->GetData();
  wxNode *second_last_line_node = last_line_node->GetPrevious();
  wxRealPoint *second_last_line_point = (wxRealPoint *)second_last_line_node->GetData();

  // Position where we want to start drawing
  double positionOnLineX = 0.0, positionOnLineY = 0.0;

  // Position of start point of line, at the end of which we draw the arrow.
  double startPositionX = 0.0 , startPositionY = 0.0;

  switch (arrow->GetPosition())
  {
    case ARROW_POSITION_START:
    {
      // If we're using a proportional offset, calculate just where this will
      // be on the line.
      double realOffset = xOffset;
      if (proportionalOffset)
      {
        double totalLength =
          (double)sqrt((second_line_point->x - first_line_point->x)*(second_line_point->x - first_line_point->x) +
                      (second_line_point->y - first_line_point->y)*(second_line_point->y - first_line_point->y));
        realOffset = (double)(xOffset * totalLength);
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
      double realOffset = xOffset;
      if (proportionalOffset)
      {
        double totalLength =
          (double)sqrt((second_last_line_point->x - last_line_point->x)*(second_last_line_point->x - last_line_point->x) +
                      (second_last_line_point->y - last_line_point->y)*(second_last_line_point->y - last_line_point->y));
        realOffset = (double)(xOffset * totalLength);
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
      double x = ((last_line_point->x + second_last_line_point->x)/2);
      double y = ((last_line_point->y + second_last_line_point->y)/2);

      // If we're using a proportional offset, calculate just where this will
      // be on the line.
      double realOffset = xOffset;
      if (proportionalOffset)
      {
        double totalLength =
          (double)sqrt((second_last_line_point->x - x)*(second_last_line_point->x - x) +
                      (second_last_line_point->y - y)*(second_last_line_point->y - y));
        realOffset = (double)(xOffset * totalLength);
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

  const double myPi = (double) M_PI;
  // The translation that the y offset may give
  double deltaX = 0.0;
  double deltaY = 0.0;
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
     double x1 = startPositionX;
     double y1 = startPositionY;
     double x3 = positionOnLineX;
     double y3 = positionOnLineY;
     double d = -arrow->GetYOffset(); // Negate so +offset is above line

     double theta;
     if (x3 == x1)
       theta = (double)(myPi/2.0);
     else
       theta = (double)atan((y3-y1)/(x3-x1));

     double x4 = (double)(x3 - (d*sin(theta)));
     double y4 = (double)(y3 + (d*cos(theta)));

     deltaX = x4 - positionOnLineX;
     deltaY = y4 - positionOnLineY;
  }

  switch (arrow->_GetType())
  {
    case ARROW_ARROW:
    {
      double arrowLength = arrow->GetSize();
      double arrowWidth = (double)(arrowLength/3.0);

      double tip_x, tip_y, side1_x, side1_y, side2_x, side2_y;
      oglGetArrowPoints(startPositionX+deltaX, startPositionY+deltaY,
                       positionOnLineX+deltaX, positionOnLineY+deltaY,
                       arrowLength, arrowWidth, &tip_x, &tip_y,
                       &side1_x, &side1_y, &side2_x, &side2_y);

      wxPoint points[4];
      points[0].x = (int) tip_x; points[0].y = (int) tip_y;
      points[1].x = (int) side1_x; points[1].y = (int) side1_y;
      points[2].x = (int) side2_x; points[2].y = (int) side2_y;
      points[3].x = (int) tip_x; points[3].y = (int) tip_y;

      dc.SetPen(* m_pen);
      dc.SetBrush(* m_brush);
      dc.DrawPolygon(4, points);
      break;
    }
    case ARROW_HOLLOW_CIRCLE:
    case ARROW_FILLED_CIRCLE:
    {
      // Find point on line of centre of circle, which is a radius away
      // from the end position
      double diameter = (double)(arrow->GetSize());
      double x, y;
      GetPointOnLine(startPositionX+deltaX, startPositionY+deltaY,
                   positionOnLineX+deltaX, positionOnLineY+deltaY,
                   (double)(diameter/2.0),
                   &x, &y);

      // Convert ellipse centre to top-left coordinates
      double x1 = (double)(x - (diameter/2.0));
      double y1 = (double)(y - (diameter/2.0));

      dc.SetPen(* m_pen);
      if (arrow->_GetType() == ARROW_HOLLOW_CIRCLE)
        dc.SetBrush(GetBackgroundBrush());
      else
        dc.SetBrush(* m_brush);

      dc.DrawEllipse((long) x1, (long) y1, (long) diameter, (long) diameter);
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
        double x, y;
        GetPointOnLine(startPositionX, startPositionY,
                   positionOnLineX, positionOnLineY,
                   (double)(arrow->GetMetaFile()->m_width/2.0),
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
        double theta = 0.0;
        double x1 = startPositionX;
        double y1 = startPositionY;
        double x2 = positionOnLineX;
        double y2 = positionOnLineY;

        if ((x1 == x2) && (y1 == y2))
          theta = 0.0;

        else if ((x1 == x2) && (y1 > y2))
          theta = (double)(3.0*myPi/2.0);

        else if ((x1 == x2) && (y2 > y1))
          theta = (double)(myPi/2.0);

        else if ((x2 > x1) && (y2 >= y1))
          theta = (double)atan((y2 - y1)/(x2 - x1));

        else if (x2 < x1)
          theta = (double)(myPi + atan((y2 - y1)/(x2 - x1)));

        else if ((x2 > x1) && (y2 < y1))
          theta = (double)(2*myPi + atan((y2 - y1)/(x2 - x1)));

        else
        {
          wxLogFatalError(wxT("Unknown arrowhead rotation case in lines.cc"));
        }

        // Rotate about the centre of the object, then place
        // the object on the line.
        if (arrow->GetMetaFile()->GetRotateable())
          arrow->GetMetaFile()->Rotate(0.0, 0.0, theta);

        if (m_erasing)
        {
          // If erasing, just draw a rectangle.
          double minX, minY, maxX, maxY;
          arrow->GetMetaFile()->GetBounds(&minX, &minY, &maxX, &maxY);
          // Make erasing rectangle slightly bigger or you get droppings.
          int extraPixels = 4;
          dc.DrawRectangle((long)(deltaX + x + minX - (extraPixels/2.0)), (long)(deltaY + y + minY - (extraPixels/2.0)),
                           (long)(maxX - minX + extraPixels), (long)(maxY - minY + extraPixels));
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
    const wxPen *old_pen = m_pen;
    const wxBrush *old_brush = m_brush;
    wxPen bg_pen = GetBackgroundPen();
    wxBrush bg_brush = GetBackgroundBrush();
    SetPen(&bg_pen);
    SetBrush(&bg_brush);

    double bound_x, bound_y;
    GetBoundingBoxMax(&bound_x, &bound_y);
    if (m_font) dc.SetFont(* m_font);

    // Undraw text regions
    for (int i = 0; i < 3; i++)
    {
      wxNode *node = m_regions.Item(i);
      if (node)
      {
        double x, y;
        wxShapeRegion *region = (wxShapeRegion *)node->GetData();
        GetLabelPosition(i, &x, &y);
        EraseRegion(dc, region, x, y);
      }
    }

    // Undraw line
    dc.SetPen(GetBackgroundPen());
    dc.SetBrush(GetBackgroundBrush());

    // Drawing over the line only seems to work if the line has a thickness
    // of 1.
    if (old_pen && (old_pen->GetWidth() > 1))
    {
      dc.DrawRectangle((long)(m_xpos - (bound_x/2.0) - 2.0), (long)(m_ypos - (bound_y/2.0) - 2.0),
                        (long)(bound_x+4.0),  (long)(bound_y+4.0));
    }
    else
    {
      m_erasing = true;
      GetEventHandler()->OnDraw(dc);
      GetEventHandler()->OnEraseControlPoints(dc);
      m_erasing = false;
    }

    if (old_pen) SetPen(old_pen);
    if (old_brush) SetBrush(old_brush);
}

void wxLineShape::GetBoundingBoxMin(double *w, double *h)
{
  double x1 = 10000;
  double y1 = 10000;
  double x2 = -10000;
  double y2 = -10000;

  wxNode *node = m_lineControlPoints->GetFirst();
  while (node)
  {
    wxRealPoint *point = (wxRealPoint *)node->GetData();

    if (point->x < x1) x1 = point->x;
    if (point->y < y1) y1 = point->y;
    if (point->x > x2) x2 = point->x;
    if (point->y > y2) y2 = point->y;

    node = node->GetNext();
  }
  *w = (double)(x2 - x1);
  *h = (double)(y2 - y1);
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
  wxNode *node = image->GetLines().GetFirst();
  int this_attachment;
  if (image == m_to)
    this_attachment = m_attachmentTo;
  else
    this_attachment = m_attachmentFrom;

  // Find number of lines going into/out of this particular attachment point
  while (node)
  {
    wxLineShape *line = (wxLineShape *)node->GetData();

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

    node = node->GetNext();
  }
  *nth = n;
  *no_arcs = num;
}

void wxLineShape::OnDrawOutline(wxDC& dc, double WXUNUSED(x), double WXUNUSED(y), double WXUNUSED(w), double WXUNUSED(h))
{
  const wxPen *old_pen = m_pen;
  const wxBrush *old_brush = m_brush;

  wxPen dottedPen(*wxBLACK, 1, wxDOT);
  SetPen(& dottedPen);
  SetBrush( wxTRANSPARENT_BRUSH );

  GetEventHandler()->OnDraw(dc);

  if (old_pen) SetPen(old_pen);
  else SetPen(NULL);
  if (old_brush) SetBrush(old_brush);
  else SetBrush(NULL);
}

bool wxLineShape::OnMovePre(wxDC& dc, double x, double y, double old_x, double old_y, bool WXUNUSED(display))
{
  double x_offset = x - old_x;
  double y_offset = y - old_y;

  if (m_lineControlPoints && !(x_offset == 0.0 && y_offset == 0.0))
  {
    wxNode *node = m_lineControlPoints->GetFirst();
    while (node)
    {
      wxRealPoint *point = (wxRealPoint *)node->GetData();
      point->x += x_offset;
      point->y += y_offset;
      node = node->GetNext();
    }

  }

  // Move temporary label rectangles if necessary
  for (int i = 0; i < 3; i++)
  {
    if (m_labelObjects[i])
    {
      m_labelObjects[i]->Erase(dc);
      double xp, yp, xr, yr;
      GetLabelPosition(i, &xp, &yp);
      wxNode *node = m_regions.Item(i);
      if (node)
      {
        wxShapeRegion *region = (wxShapeRegion *)node->GetData();
        region->GetPosition(&xr, &yr);
      }
      else
      {
        xr = 0.0; yr = 0.0;
      }

      m_labelObjects[i]->Move(dc, xp+xr, yp+yr);
    }
  }
  return true;
}

void wxLineShape::OnMoveLink(wxDC& dc, bool moveControlPoints)
{
  if (!m_from || !m_to)
   return;

    if (m_lineControlPoints->GetCount() > 2)
      Initialise();

    // Do each end - nothing in the middle. User has to move other points
    // manually if necessary.
    double end_x, end_y;
    double other_end_x, other_end_y;

    FindLineEndPoints(&end_x, &end_y, &other_end_x, &other_end_y);

    wxNode *first = m_lineControlPoints->GetFirst();
    /* wxRealPoint *first_point = */ (wxRealPoint *)first->GetData();
    wxNode *last = m_lineControlPoints->GetLast();
    /* wxRealPoint *last_point = */ (wxRealPoint *)last->GetData();

/* This is redundant, surely? Done by SetEnds.
    first_point->x = end_x; first_point->y = end_y;
    last_point->x = other_end_x; last_point->y = other_end_y;
*/

    double oldX = m_xpos;
    double oldY = m_ypos;

    SetEnds(end_x, end_y, other_end_x, other_end_y);

    // Do a second time, because one may depend on the other.
    FindLineEndPoints(&end_x, &end_y, &other_end_x, &other_end_y);
    SetEnds(end_x, end_y, other_end_x, other_end_y);

    // Try to move control points with the arc
    double x_offset = m_xpos - oldX;
    double y_offset = m_ypos - oldY;

//    if (moveControlPoints && m_lineControlPoints && !(x_offset == 0.0 && y_offset == 0.0))
    // Only move control points if it's a self link. And only works if attachment mode is ON.
    if ((m_from == m_to) && (m_from->GetAttachmentMode() != ATTACHMENT_MODE_NONE) && moveControlPoints && m_lineControlPoints && !(x_offset == 0.0 && y_offset == 0.0))
    {
      wxNode *node = m_lineControlPoints->GetFirst();
      while (node)
      {
        if ((node != m_lineControlPoints->GetFirst()) && (node != m_lineControlPoints->GetLast()))
        {
          wxRealPoint *point = (wxRealPoint *)node->GetData();
          point->x += x_offset;
          point->y += y_offset;
        }
        node = node->GetNext();
      }
    }

    Move(dc, m_xpos, m_ypos);
}

// Finds the x, y points at the two ends of the line.
// This function can be used by e.g. line-routing routines to
// get the actual points on the two node images where the lines will be drawn
// to/from.
void wxLineShape::FindLineEndPoints(double *fromX, double *fromY, double *toX, double *toY)
{
  if (!m_from || !m_to)
   return;

  // Do each end - nothing in the middle. User has to move other points
  // manually if necessary.
  double end_x = 0.0, end_y = 0.0;
  double other_end_x = 0.0, other_end_y = 0.0;

  wxNode *first = m_lineControlPoints->GetFirst();
  /* wxRealPoint *first_point = */ (wxRealPoint *)first->GetData();
  wxNode *last = m_lineControlPoints->GetLast();
  /* wxRealPoint *last_point = */ (wxRealPoint *)last->GetData();

  wxNode *second = first->GetNext();
  wxRealPoint *second_point = (wxRealPoint *)second->GetData();

  wxNode *second_last = last->GetPrevious();
  wxRealPoint *second_last_point = (wxRealPoint *)second_last->GetData();

  if (m_lineControlPoints->GetCount() > 2)
  {
    if (m_from->GetAttachmentMode() != ATTACHMENT_MODE_NONE)
    {
      int nth, no_arcs;
      FindNth(m_from, &nth, &no_arcs, false); // Not incoming
      m_from->GetAttachmentPosition(m_attachmentFrom, &end_x, &end_y, nth, no_arcs, this);
    }
    else
      (void) m_from->GetPerimeterPoint(m_from->GetX(), m_from->GetY(),
                                   (double)second_point->x, (double)second_point->y,
                                    &end_x, &end_y);

    if (m_to->GetAttachmentMode() != ATTACHMENT_MODE_NONE)
    {
      int nth, no_arcs;
      FindNth(m_to, &nth, &no_arcs, true); // Incoming
      m_to->GetAttachmentPosition(m_attachmentTo, &other_end_x, &other_end_y, nth, no_arcs, this);
    }
    else
      (void) m_to->GetPerimeterPoint(m_to->GetX(), m_to->GetY(),
                                (double)second_last_point->x, (double)second_last_point->y,
                                &other_end_x, &other_end_y);
  }
  else
  {
    double fromX = m_from->GetX();
    double fromY = m_from->GetY();
    double toX = m_to->GetX();
    double toY = m_to->GetY();

    if (m_from->GetAttachmentMode() != ATTACHMENT_MODE_NONE)
    {
      int nth, no_arcs;
      FindNth(m_from, &nth, &no_arcs, false);
      m_from->GetAttachmentPosition(m_attachmentFrom, &end_x, &end_y, nth, no_arcs, this);
      fromX = end_x;
      fromY = end_y;
    }

    if (m_to->GetAttachmentMode() != ATTACHMENT_MODE_NONE)
    {
      int nth, no_arcs;
      FindNth(m_to, &nth, &no_arcs, true);
      m_to->GetAttachmentPosition(m_attachmentTo, &other_end_x, &other_end_y, nth, no_arcs, this);
      toX = other_end_x;
      toY = other_end_y;
    }

    if (m_from->GetAttachmentMode() == ATTACHMENT_MODE_NONE)
      (void) m_from->GetPerimeterPoint(m_from->GetX(), m_from->GetY(),
                                  toX, toY,
                                  &end_x, &end_y);

    if (m_to->GetAttachmentMode() == ATTACHMENT_MODE_NONE)
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
      dc.SetPen(* m_pen);
    if (m_brush)
      dc.SetBrush(* m_brush);

    int n = m_lineControlPoints->GetCount();
    wxPoint *points = new wxPoint[n];
    int i;
    for (i = 0; i < n; i++)
    {
        wxRealPoint* point = (wxRealPoint*) m_lineControlPoints->Item(i)->GetData();
        points[i].x = WXROUND(point->x);
        points[i].y = WXROUND(point->y);
    }

    if (m_isSpline)
      dc.DrawSpline(n, points);
    else
      dc.DrawLines(n, points);

#ifdef __WXMSW__
    // For some reason, last point isn't drawn under Windows.
    dc.DrawPoint(points[n-1]);
#endif

    delete[] points;


    // Problem with pen - if not a solid pen, does strange things
    // to the arrowhead. So make (get) a new pen that's solid.
    if (m_pen && (m_pen->GetStyle() != wxSOLID))
    {
      wxPen *solid_pen =
        wxThePenList->FindOrCreatePen(m_pen->GetColour(), 1, wxSOLID);
      if (solid_pen)
        dc.SetPen(* solid_pen);
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

void wxLineShape::OnDragLeft(bool WXUNUSED(draw), double WXUNUSED(x), double WXUNUSED(y), int WXUNUSED(keys), int WXUNUSED(attachment))
{
}

void wxLineShape::OnBeginDragLeft(double WXUNUSED(x), double WXUNUSED(y), int WXUNUSED(keys), int WXUNUSED(attachment))
{
}

void wxLineShape::OnEndDragLeft(double WXUNUSED(x), double WXUNUSED(y), int WXUNUSED(keys), int WXUNUSED(attachment))
{
}

/*
void wxLineShape::SetArrowSize(double length, double width)
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
    wxNode *node = m_regions.Item(i);
    if (node)
    {
      wxShapeRegion *region = (wxShapeRegion *)node->GetData();
      double x, y;
      GetLabelPosition(i, &x, &y);
      DrawRegion(dc, region, x, y);
    }
  }
}

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
    wxNode *first = m_lineControlPoints->GetFirst();
    wxNode *last = m_lineControlPoints->GetLast();
    wxRealPoint *first_point = (wxRealPoint *)first->GetData();
    wxRealPoint *last_point = (wxRealPoint *)last->GetData();

    wxLineControlPoint *control = new wxLineControlPoint(m_canvas, this, CONTROL_POINT_SIZE,
                                               first_point->x, first_point->y,
                                               CONTROL_POINT_ENDPOINT_FROM);
    control->m_point = first_point;
    m_canvas->AddShape(control);
    m_controlPoints.Append(control);


    wxNode *node = first->GetNext();
    while (node != last)
    {
      wxRealPoint *point = (wxRealPoint *)node->GetData();

      control = new wxLineControlPoint(m_canvas, this, CONTROL_POINT_SIZE,
                                               point->x, point->y,
                                               CONTROL_POINT_LINE);
      control->m_point = point;

      m_canvas->AddShape(control);
      m_controlPoints.Append(control);

      node = node->GetNext();
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
  if (m_canvas && m_lineControlPoints && m_controlPoints.GetCount() > 0)
  {
    wxNode *node = m_controlPoints.GetFirst();
    wxNode *control_node = m_lineControlPoints->GetFirst();
    while (node && control_node)
    {
      wxRealPoint *point = (wxRealPoint *)control_node->GetData();
      wxLineControlPoint *control = (wxLineControlPoint *)node->GetData();
      control->SetX(point->x);
      control->SetY(point->y);

      node = node->GetNext();
      control_node = control_node->GetNext();
    }
  }
}

#if wxUSE_PROLOGIO
void wxLineShape::WriteAttributes(wxExpr *clause)
{
  wxShape::WriteAttributes(clause);

  if (m_from)
    clause->AddAttributeValue(_T("from"), m_from->GetId());
  if (m_to)
    clause->AddAttributeValue(_T("to"), m_to->GetId());

  if (m_attachmentTo != 0)
    clause->AddAttributeValue(_T("attachment_to"), (long)m_attachmentTo);
  if (m_attachmentFrom != 0)
    clause->AddAttributeValue(_T("attachment_from"), (long)m_attachmentFrom);

  if (m_alignmentStart != 0)
    clause->AddAttributeValue(_T("align_start"), (long)m_alignmentStart);
  if (m_alignmentEnd != 0)
    clause->AddAttributeValue(_T("align_end"), (long)m_alignmentEnd);

  clause->AddAttributeValue(_T("is_spline"), (long)m_isSpline);
  if (m_maintainStraightLines)
    clause->AddAttributeValue(_T("keep_lines_straight"), (long)m_maintainStraightLines);

  // Make a list of lists for the (sp)line controls
  wxExpr *list = new wxExpr(wxExprList);
  wxNode *node = m_lineControlPoints->GetFirst();
  while (node)
  {
    wxRealPoint *point = (wxRealPoint *)node->GetData();
    wxExpr *point_list = new wxExpr(wxExprList);
    wxExpr *x_expr = new wxExpr((double) point->x);
    wxExpr *y_expr = new wxExpr((double) point->y);
    point_list->Append(x_expr);
    point_list->Append(y_expr);
    list->Append(point_list);

    node = node->GetNext();
  }
  clause->AddAttributeValue(_T("controls"), list);

  // Write arc arrows in new OGL format, if there are any.
  // This is a list of lists. Each sublist comprises:
  // (arrowType arrowEnd xOffset arrowSize)
  if (m_arcArrows.GetCount() > 0)
  {
    wxExpr *arrow_list = new wxExpr(wxExprList);
    node = m_arcArrows.GetFirst();
    while (node)
    {
      wxArrowHead *head = (wxArrowHead *)node->GetData();
      wxExpr *head_list = new wxExpr(wxExprList);
      head_list->Append(new wxExpr((long)head->_GetType()));
      head_list->Append(new wxExpr((long)head->GetArrowEnd()));
      head_list->Append(new wxExpr(head->GetXOffset()));
      head_list->Append(new wxExpr(head->GetArrowSize()));
      head_list->Append(new wxExpr(wxExprString, head->GetName()));
      head_list->Append(new wxExpr(head->GetId()));

      // New members of wxArrowHead
      head_list->Append(new wxExpr(head->GetYOffset()));
      head_list->Append(new wxExpr(head->GetSpacing()));

      arrow_list->Append(head_list);

      node = node->GetNext();
    }
    clause->AddAttributeValue(_T("arrows"), arrow_list);
  }
}

void wxLineShape::ReadAttributes(wxExpr *clause)
{
  wxShape::ReadAttributes(clause);

  int iVal = (int) m_isSpline;
  clause->AssignAttributeValue(wxT("is_spline"), &iVal);
  m_isSpline = (iVal != 0);

  iVal = (int) m_maintainStraightLines;
  clause->AssignAttributeValue(wxT("keep_lines_straight"), &iVal);
  m_maintainStraightLines = (iVal != 0);

  clause->AssignAttributeValue(wxT("align_start"), &m_alignmentStart);
  clause->AssignAttributeValue(wxT("align_end"), &m_alignmentEnd);

  // Compatibility: check for no regions.
  if (m_regions.GetCount() == 0)
  {
    wxShapeRegion *newRegion = new wxShapeRegion;
    newRegion->SetName(_T("Middle"));
    newRegion->SetSize(150, 50);
    m_regions.Append((wxObject *)newRegion);
    if (m_text.GetCount() > 0)
    {
      newRegion->ClearText();
      wxNode *node = m_text.GetFirst();
      while (node)
      {
        wxShapeTextLine *textLine = (wxShapeTextLine *)node->GetData();
        wxNode *next = node->GetNext();
        newRegion->GetFormattedText().Append((wxObject *)textLine);
        delete node;
        node = next;
      }
    }

    newRegion = new wxShapeRegion;
    newRegion->SetName(wxT("Start"));
    newRegion->SetSize(150, 50);
    m_regions.Append((wxObject *)newRegion);

    newRegion = new wxShapeRegion;
    newRegion->SetName(wxT("End"));
    newRegion->SetSize(150, 50);
    m_regions.Append((wxObject *)newRegion);
  }

  m_attachmentTo = 0;
  m_attachmentFrom = 0;

  clause->AssignAttributeValue(wxT("attachment_to"), &m_attachmentTo);
  clause->AssignAttributeValue(wxT("attachment_from"), &m_attachmentFrom);

  wxExpr *line_list = NULL;

  // When image is created, there are default control points. Override
  // them if there are some in the file.
  clause->AssignAttributeValue(wxT("controls"), &line_list);

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
      double x = xexpr->RealValue();

      wxExpr *yexpr = xexpr->next;
      double y = yexpr->RealValue();

      wxRealPoint *point = new wxRealPoint(x, y);
      m_lineControlPoints->Append((wxObject*) point);

      node = node->next;
    }
  }

  // Read arrow list, for new OGL code
  wxExpr *arrow_list = NULL;

  clause->AssignAttributeValue(wxT("arrows"), &arrow_list);
  if (arrow_list)
  {
    wxExpr *node = arrow_list->value.first;

    while (node)
    {
      WXTYPE arrowType = ARROW_ARROW;
      int arrowEnd = 0;
      double xOffset = 0.0;
      double arrowSize = 0.0;
      wxString arrowName;
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
          arrowType = (WXTYPE)type_expr->IntegerValue();
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
        arrowId = wxNewId();
      else
        wxRegisterId(arrowId);

      wxArrowHead *arrowHead = AddArrow(arrowType, arrowEnd, arrowSize, xOffset, arrowName, NULL, arrowId);
      if (yOffsetExpr)
        arrowHead->SetYOffset(yOffsetExpr->RealValue());
      if (spacingExpr)
        arrowHead->SetSpacing(spacingExpr->RealValue());

      node = node->next;
    }
  }
}
#endif

void wxLineShape::Copy(wxShape& copy)
{
  wxShape::Copy(copy);

  wxASSERT( copy.IsKindOf(CLASSINFO(wxLineShape)) );

  wxLineShape& lineCopy = (wxLineShape&) copy;

  lineCopy.m_to = m_to;
  lineCopy.m_from = m_from;
  lineCopy.m_attachmentTo = m_attachmentTo;
  lineCopy.m_attachmentFrom = m_attachmentFrom;
  lineCopy.m_isSpline = m_isSpline;
  lineCopy.m_alignmentStart = m_alignmentStart;
  lineCopy.m_alignmentEnd = m_alignmentEnd;
  lineCopy.m_maintainStraightLines = m_maintainStraightLines;
  lineCopy.m_lineOrientations.Clear();

  wxNode *node = m_lineOrientations.GetFirst();
  while (node)
  {
    lineCopy.m_lineOrientations.Append(node->GetData());
    node = node->GetNext();
  }

  if (lineCopy.m_lineControlPoints)
  {
    ClearPointList(*lineCopy.m_lineControlPoints);
    delete lineCopy.m_lineControlPoints;
  }

  lineCopy.m_lineControlPoints = new wxList;

  node = m_lineControlPoints->GetFirst();
  while (node)
  {
    wxRealPoint *point = (wxRealPoint *)node->GetData();
    wxRealPoint *new_point = new wxRealPoint(point->x, point->y);
    lineCopy.m_lineControlPoints->Append((wxObject*) new_point);
    node = node->GetNext();
  }

  // Copy arrows
  lineCopy.ClearArrowsAtPosition(-1);
  node = m_arcArrows.GetFirst();
  while (node)
  {
    wxArrowHead *arrow = (wxArrowHead *)node->GetData();
    lineCopy.m_arcArrows.Append(new wxArrowHead(*arrow));
    node = node->GetNext();
  }
}

// Override select, to create/delete temporary label-moving objects
void wxLineShape::Select(bool select, wxDC* dc)
{
  wxShape::Select(select, dc);
  if (select)
  {
    for (int i = 0; i < 3; i++)
    {
      wxNode *node = m_regions.Item(i);
      if (node)
      {
        wxShapeRegion *region = (wxShapeRegion *)node->GetData();
        if (region->m_formattedText.GetCount() > 0)
        {
          double w, h, x, y, xx, yy;
          region->GetSize(&w, &h);
          region->GetPosition(&x, &y);
          GetLabelPosition(i, &xx, &yy);
          if (m_labelObjects[i])
          {
            m_labelObjects[i]->Select(false);
            m_labelObjects[i]->RemoveFromCanvas(m_canvas);
            delete m_labelObjects[i];
          }
          m_labelObjects[i] = OnCreateLabelShape(this, region, w, h);
          m_labelObjects[i]->AddToCanvas(m_canvas);
          m_labelObjects[i]->Show(true);
          if (dc)
            m_labelObjects[i]->Move(*dc, (double)(x + xx), (double)(y + yy));
          m_labelObjects[i]->Select(true, dc);
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
        m_labelObjects[i]->Select(false, dc);
        m_labelObjects[i]->Erase(*dc);
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

wxLineControlPoint::wxLineControlPoint(wxShapeCanvas *theCanvas, wxShape *object, double size, double x, double y, int the_type):
  wxControlPoint(theCanvas, object, size, x, y, the_type)
{
  m_xpos = x;
  m_ypos = y;
  m_type = the_type;
  m_point = NULL;
}

wxLineControlPoint::~wxLineControlPoint()
{
}

void wxLineControlPoint::OnDraw(wxDC& dc)
{
  wxRectangleShape::OnDraw(dc);
}

// Implement movement of Line point
void wxLineControlPoint::OnDragLeft(bool draw, double x, double y, int keys, int attachment)
{
    m_shape->GetEventHandler()->OnSizingDragLeft(this, draw, x, y, keys, attachment);
}

void wxLineControlPoint::OnBeginDragLeft(double x, double y, int keys, int attachment)
{
    m_shape->GetEventHandler()->OnSizingBeginDragLeft(this, x, y, keys, attachment);
}

void wxLineControlPoint::OnEndDragLeft(double x, double y, int keys, int attachment)
{
    m_shape->GetEventHandler()->OnSizingEndDragLeft(this, x, y, keys, attachment);
}

// Control points ('handles') redirect control to the actual shape, to make it easier
// to override sizing behaviour.
void wxLineShape::OnSizingDragLeft(wxControlPoint* pt, bool WXUNUSED(draw), double x, double y, int WXUNUSED(keys), int WXUNUSED(attachment))
{
  wxLineControlPoint* lpt = (wxLineControlPoint*) pt;

  wxClientDC dc(GetCanvas());
  GetCanvas()->PrepareDC(dc);

  dc.SetLogicalFunction(OGLRBLF);

  wxPen dottedPen(*wxBLACK, 1, wxDOT);
  dc.SetPen(dottedPen);
  dc.SetBrush((* wxTRANSPARENT_BRUSH));

  if (lpt->m_type == CONTROL_POINT_LINE)
  {
    m_canvas->Snap(&x, &y);

    lpt->SetX(x); lpt->SetY(y);
    lpt->m_point->x = x; lpt->m_point->y = y;

    wxLineShape *lineShape = (wxLineShape *)this;

    const wxPen *old_pen = lineShape->GetPen();
    const wxBrush *old_brush = lineShape->GetBrush();

    wxPen dottedPen(*wxBLACK, 1, wxDOT);
    lineShape->SetPen(& dottedPen);
    lineShape->SetBrush(wxTRANSPARENT_BRUSH);

    lineShape->GetEventHandler()->OnMoveLink(dc, false);

    lineShape->SetPen(old_pen);
    lineShape->SetBrush(old_brush);
  }

  if (lpt->m_type == CONTROL_POINT_ENDPOINT_FROM || lpt->m_type == CONTROL_POINT_ENDPOINT_TO)
  {
//    lpt->SetX(x); lpt->SetY(y);
  }

}

void wxLineShape::OnSizingBeginDragLeft(wxControlPoint* pt, double x, double y, int WXUNUSED(keys), int WXUNUSED(attachment))
{
  wxLineControlPoint* lpt = (wxLineControlPoint*) pt;

  wxClientDC dc(GetCanvas());
  GetCanvas()->PrepareDC(dc);

  wxLineShape *lineShape = (wxLineShape *)this;
  if (lpt->m_type == CONTROL_POINT_LINE)
  {
    lpt->m_originalPos = * (lpt->m_point);
    m_canvas->Snap(&x, &y);

    this->Erase(dc);

    // Redraw start and end objects because we've left holes
    // when erasing the line
    lineShape->GetFrom()->OnDraw(dc);
    lineShape->GetFrom()->OnDrawContents(dc);
    lineShape->GetTo()->OnDraw(dc);
    lineShape->GetTo()->OnDrawContents(dc);

    this->SetDisableLabel(true);
    dc.SetLogicalFunction(OGLRBLF);

    lpt->m_xpos = x; lpt->m_ypos = y;
    lpt->m_point->x = x; lpt->m_point->y = y;

    const wxPen *old_pen = lineShape->GetPen();
    const wxBrush *old_brush = lineShape->GetBrush();

    wxPen dottedPen(*wxBLACK, 1, wxDOT);
    lineShape->SetPen(& dottedPen);
    lineShape->SetBrush(wxTRANSPARENT_BRUSH);

    lineShape->GetEventHandler()->OnMoveLink(dc, false);

    lineShape->SetPen(old_pen);
    lineShape->SetBrush(old_brush);
  }

  if (lpt->m_type == CONTROL_POINT_ENDPOINT_FROM || lpt->m_type == CONTROL_POINT_ENDPOINT_TO)
  {
    m_canvas->SetCursor(wxCursor(wxCURSOR_BULLSEYE));
    lpt->m_oldCursor = wxSTANDARD_CURSOR;
  }
}

void wxLineShape::OnSizingEndDragLeft(wxControlPoint* pt, double x, double y, int WXUNUSED(keys), int WXUNUSED(attachment))
{
  wxLineControlPoint* lpt = (wxLineControlPoint*) pt;

  wxClientDC dc(GetCanvas());
  GetCanvas()->PrepareDC(dc);

  this->SetDisableLabel(false);
  wxLineShape *lineShape = (wxLineShape *)this;

  if (lpt->m_type == CONTROL_POINT_LINE)
  {
    m_canvas->Snap(&x, &y);

    wxRealPoint pt = wxRealPoint(x, y);

    // Move the control point back to where it was;
    // MoveControlPoint will move it to the new position
    // if it decides it wants. We only moved the position
    // during user feedback so we could redraw the line
    // as it changed shape.
    lpt->m_xpos = lpt->m_originalPos.x; lpt->m_ypos = lpt->m_originalPos.y;
    lpt->m_point->x = lpt->m_originalPos.x; lpt->m_point->y = lpt->m_originalPos.y;

    OnMoveMiddleControlPoint(dc, lpt, pt);
  }
  if (lpt->m_type == CONTROL_POINT_ENDPOINT_FROM)
  {
    if (lpt->m_oldCursor)
      m_canvas->SetCursor(* lpt->m_oldCursor);

//    this->Erase(dc);

//    lpt->m_xpos = x; lpt->m_ypos = y;

    if (lineShape->GetFrom())
    {
      lineShape->GetFrom()->MoveLineToNewAttachment(dc, lineShape, x, y);
    }
  }
  if (lpt->m_type == CONTROL_POINT_ENDPOINT_TO)
  {
    if (lpt->m_oldCursor)
      m_canvas->SetCursor(* lpt->m_oldCursor);

//    lpt->m_xpos = x; lpt->m_ypos = y;

    if (lineShape->GetTo())
    {
      lineShape->GetTo()->MoveLineToNewAttachment(dc, lineShape, x, y);
    }
  }

  // Needed?
#if 0
  int i = 0;
  for (i = 0; i < lineShape->GetLineControlPoints()->GetCount(); i++)
    if (((wxRealPoint *)(lineShape->GetLineControlPoints()->Item(i)->GetData())) == lpt->m_point)
      break;

  // N.B. in OnMoveControlPoint, an event handler in Hardy could have deselected
  // the line and therefore deleted 'this'. -> GPF, intermittently.
  // So assume at this point that we've been blown away.

  lineShape->OnMoveControlPoint(i+1, x, y);
#endif
}

// This is called only when a non-end control point is moved.
bool wxLineShape::OnMoveMiddleControlPoint(wxDC& dc, wxLineControlPoint* lpt, const wxRealPoint& pt)
{
    lpt->m_xpos = pt.x; lpt->m_ypos = pt.y;
    lpt->m_point->x = pt.x; lpt->m_point->y = pt.y;

    GetEventHandler()->OnMoveLink(dc);

    return true;
}

// Implement movement of endpoint to a new attachment
// OBSOLETE: done by dragging with the left button.

#if 0
void wxLineControlPoint::OnDragRight(bool draw, double x, double y, int keys, int attachment)
{
  if (m_type == CONTROL_POINT_ENDPOINT_FROM || m_type == CONTROL_POINT_ENDPOINT_TO)
  {
    m_xpos = x; m_ypos = y;
  }
}

void wxLineControlPoint::OnBeginDragRight(double x, double y, int keys, int attachment)
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
    m_canvas->SetCursor(wxCursor(wxCURSOR_BULLSEYE));
    m_oldCursor = wxSTANDARD_CURSOR;
  }
}

void wxLineControlPoint::OnEndDragRight(double x, double y, int keys, int attachment)
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
      double distance;

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
      double distance;
      if (lineShape->GetTo()->HitTest(x, y, &new_attachment, &distance))
        lineShape->SetAttachments(lineShape->GetAttachmentFrom(), new_attachment);

      lineShape->GetTo()->MoveLinks(dc);
    }
  }
  int i = 0;
  for (i = 0; i < lineShape->GetLineControlPoints()->GetCount(); i++)
    if (((wxRealPoint *)(lineShape->GetLineControlPoints()->Item(i)->GetData())) == m_point)
      break;
  lineShape->OnMoveControlPoint(i+1, x, y);
  if (!m_canvas->GetQuickEditMode()) m_canvas->Redraw(dc);
}
#endif

/*
 * Get the point on the given line (x1, y1) (x2, y2)
 * distance 'length' along from the end,
 * returned values in x and y
 */

void GetPointOnLine(double x1, double y1, double x2, double y2,
                    double length, double *x, double *y)
{
  double l = (double)sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));

  if (l < 0.01)
    l = (double) 0.01;

  double i_bar = (x2 - x1)/l;
  double j_bar = (y2 - y1)/l;

  *x = (- length*i_bar) + x2;
  *y = (- length*j_bar) + y2;
}

wxArrowHead *wxLineShape::AddArrow(WXTYPE type, int end, double size, double xOffset,
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
  wxNode *refNode = referenceList.GetFirst();
  wxNode *currNode = m_arcArrows.GetFirst();
  wxString targetName(arrow->GetName());
  if (!refNode) return false;

  // First check whether we need to insert in front of list,
  // because this arrowhead is the first in the reference
  // list and should therefore be first in the current list.
  wxArrowHead *refArrow = (wxArrowHead *)refNode->GetData();
  if (refArrow->GetName() == targetName)
  {
    m_arcArrows.Insert(arrow);
    return true;
  }

  wxArrowHead *currArrow = (wxArrowHead *)currNode->GetData();
  while (refNode && currNode)
  {
    refArrow = (wxArrowHead *)refNode->GetData();

    // Matching: advance current arrow pointer
    if ((currArrow->GetArrowEnd() == end) &&
        (currArrow->GetName() == refArrow->GetName()))
    {
      currNode = currNode->GetNext(); // Could be NULL now
      if (currNode)
        currArrow = (wxArrowHead *)currNode->GetData();
    }

    // Check if we're at the correct position in the
    // reference list
    if (targetName == refArrow->GetName())
    {
      if (currNode)
        m_arcArrows.Insert(currNode, arrow);
      else
        m_arcArrows.Append(arrow);
      return true;
    }
    refNode = refNode->GetNext();
  }
  m_arcArrows.Append(arrow);
  return true;
}

void wxLineShape::ClearArrowsAtPosition(int end)
{
  wxNode *node = m_arcArrows.GetFirst();
  while (node)
  {
    wxArrowHead *arrow = (wxArrowHead *)node->GetData();
    wxNode *next = node->GetNext();
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
  wxNode *node = m_arcArrows.GetFirst();
  while (node)
  {
    wxArrowHead *arrow = (wxArrowHead *)node->GetData();
    if (arrow->GetName() == name)
    {
      delete arrow;
      delete node;
      return true;
    }
    node = node->GetNext();
  }
  return false;
}

/*
 * Finds an arrowhead at the given position (if -1, any position)
 *
 */

wxArrowHead *wxLineShape::FindArrowHead(int position, const wxString& name)
{
  wxNode *node = m_arcArrows.GetFirst();
  while (node)
  {
    wxArrowHead *arrow = (wxArrowHead *)node->GetData();
    if (((position == -1) || (position == arrow->GetArrowEnd())) &&
        (arrow->GetName() == name))
      return arrow;
    node = node->GetNext();
  }
  return NULL;
}

wxArrowHead *wxLineShape::FindArrowHead(long arrowId)
{
  wxNode *node = m_arcArrows.GetFirst();
  while (node)
  {
    wxArrowHead *arrow = (wxArrowHead *)node->GetData();
    if (arrowId == arrow->GetId())
      return arrow;
    node = node->GetNext();
  }
  return NULL;
}

/*
 * Deletes an arrowhead at the given position (if -1, any position)
 *
 */

bool wxLineShape::DeleteArrowHead(int position, const wxString& name)
{
  wxNode *node = m_arcArrows.GetFirst();
  while (node)
  {
    wxArrowHead *arrow = (wxArrowHead *)node->GetData();
    if (((position == -1) || (position == arrow->GetArrowEnd())) &&
        (arrow->GetName() == name))
    {
      delete arrow;
      delete node;
      return true;
    }
    node = node->GetNext();
  }
  return false;
}

// Overloaded DeleteArrowHead: pass arrowhead id.
bool wxLineShape::DeleteArrowHead(long id)
{
  wxNode *node = m_arcArrows.GetFirst();
  while (node)
  {
    wxArrowHead *arrow = (wxArrowHead *)node->GetData();
    if (arrow->GetId() == id)
    {
      delete arrow;
      delete node;
      return true;
    }
    node = node->GetNext();
  }
  return false;
}

/*
 * Calculate the minimum width a line
 * occupies, for the purposes of drawing lines in tools.
 *
 */

double wxLineShape::FindMinimumWidth()
{
  double minWidth = 0.0;
  wxNode *node = m_arcArrows.GetFirst();
  while (node)
  {
    wxArrowHead *arrowHead = (wxArrowHead *)node->GetData();
    minWidth += arrowHead->GetSize();
    if (node->GetNext())
      minWidth += arrowHead->GetSpacing();

    node = node->GetNext();
  }
  // We have ABSOLUTE minimum now. So
  // scale it to give it reasonable aesthetics
  // when drawing with line.
  if (minWidth > 0.0)
    minWidth = (double)(minWidth * 1.4);
  else
    minWidth = 20.0;

  SetEnds(0.0, 0.0, minWidth, 0.0);
  Initialise();

  return minWidth;
}

// Find which position we're talking about at this (x, y).
// Returns ARROW_POSITION_START, ARROW_POSITION_MIDDLE, ARROW_POSITION_END
int wxLineShape::FindLinePosition(double x, double y)
{
  double startX, startY, endX, endY;
  GetEnds(&startX, &startY, &endX, &endY);

  // Find distances from centre, start and end. The smallest wins.
  double centreDistance = (double)(sqrt((x - m_xpos)*(x - m_xpos) + (y - m_ypos)*(y - m_ypos)));
  double startDistance = (double)(sqrt((x - startX)*(x - startX) + (y - startY)*(y - startY)));
  double endDistance = (double)(sqrt((x - endX)*(x - endX) + (y - endY)*(y - endY)));

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
  int n = m_lineControlPoints->GetCount();
  int nn;
  if (m_to == nodeObject)
  {
    // Must be END of line, so we want (n - 1)th control point.
    // But indexing ends at n-1, so subtract 2.
    nn = n - 2;
  }
  else nn = 1;
  wxNode *node = m_lineControlPoints->Item(nn);
  if (node)
  {
    return (wxRealPoint *)node->GetData();
  }
  else
    return NULL;
}

/*
 * Arrowhead
 *
 */

IMPLEMENT_DYNAMIC_CLASS(wxArrowHead, wxObject)

wxArrowHead::wxArrowHead(WXTYPE type, int end, double size, double dist, const wxString& name,
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
    m_id = wxNewId();
}

wxArrowHead::wxArrowHead(wxArrowHead& toCopy):wxObject()
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
  m_id = wxNewId();
}

wxArrowHead::~wxArrowHead()
{
  if (m_metaFile) delete m_metaFile;
}

void wxArrowHead::SetSize(double size)
{
  m_arrowSize = size;
  if ((m_arrowType == ARROW_METAFILE) && m_metaFile)
  {
    double oldWidth = m_metaFile->m_width;
    if (oldWidth == 0.0)
      return;

    double scale = (double)(size/oldWidth);
    if (scale != 1.0)
      m_metaFile->Scale(scale, scale);
  }
}

// Can override this to create a different class of label shape
wxLabelShape* wxLineShape::OnCreateLabelShape(wxLineShape *parent, wxShapeRegion *region, double w, double h)
{
    return new wxLabelShape(parent, region, w, h);
}

/*
 * Label object
 *
 */

IMPLEMENT_DYNAMIC_CLASS(wxLabelShape, wxRectangleShape)

wxLabelShape::wxLabelShape(wxLineShape *parent, wxShapeRegion *region, double w, double h):wxRectangleShape(w, h)
{
  m_lineShape = parent;
  m_shapeRegion = region;
  SetPen(wxThePenList->FindOrCreatePen(*wxBLACK, 1, wxDOT));
}

wxLabelShape::~wxLabelShape()
{
}

void wxLabelShape::OnDraw(wxDC& dc)
{
  if (m_lineShape && !m_lineShape->GetDrawHandles())
    return;

    double x1 = (double)(m_xpos - m_width/2.0);
    double y1 = (double)(m_ypos - m_height/2.0);

    if (m_pen)
    {
      if (m_pen->GetWidth() == 0)
        dc.SetPen(* g_oglTransparentPen);
      else
        dc.SetPen(* m_pen);
    }
    dc.SetBrush(* wxTRANSPARENT_BRUSH);

    if (m_cornerRadius > 0.0)
      dc.DrawRoundedRectangle(WXROUND(x1), WXROUND(y1), WXROUND(m_width), WXROUND(m_height), m_cornerRadius);
    else
      dc.DrawRectangle(WXROUND(x1), WXROUND(y1), WXROUND(m_width), WXROUND(m_height));
}

void wxLabelShape::OnDrawContents(wxDC& WXUNUSED(dc))
{
}

void wxLabelShape::OnDragLeft(bool draw, double x, double y, int keys, int attachment)
{
  wxRectangleShape::OnDragLeft(draw, x, y, keys, attachment);
}

void wxLabelShape::OnBeginDragLeft(double x, double y, int keys, int attachment)
{
  wxRectangleShape::OnBeginDragLeft(x, y, keys, attachment);
}

void wxLabelShape::OnEndDragLeft(double x, double y, int keys, int attachment)
{
  wxRectangleShape::OnEndDragLeft(x, y, keys, attachment);
}

bool wxLabelShape::OnMovePre(wxDC& dc, double x, double y, double old_x, double old_y, bool display)
{
    return m_lineShape->OnLabelMovePre(dc, this, x, y, old_x, old_y, display);
}

bool wxLineShape::OnLabelMovePre(wxDC& dc, wxLabelShape* labelShape, double x, double y, double WXUNUSED(old_x), double WXUNUSED(old_y), bool WXUNUSED(display))
{
  labelShape->m_shapeRegion->SetSize(labelShape->GetWidth(), labelShape->GetHeight());

  // Find position in line's region list
  int i = 0;
  wxNode *node = GetRegions().GetFirst();
  while (node)
  {
    if (labelShape->m_shapeRegion == (wxShapeRegion *)node->GetData())
      node = NULL;
    else
    {
      node = node->GetNext();
      i ++;
    }
  }
  double xx, yy;
  GetLabelPosition(i, &xx, &yy);
  // Set the region's offset, relative to the default position for
  // each region.
  labelShape->m_shapeRegion->SetPosition((double)(x - xx), (double)(y - yy));

  labelShape->SetX(x);
  labelShape->SetY(y);

  // Need to reformat to fit region.
  if (labelShape->m_shapeRegion->GetText())
  {

    wxString s(labelShape->m_shapeRegion->GetText());
    labelShape->FormatText(dc, s, i);
    DrawRegion(dc, labelShape->m_shapeRegion, xx, yy);
  }
  return true;
}

// Divert left and right clicks to line object
void wxLabelShape::OnLeftClick(double x, double y, int keys, int attachment)
{
  m_lineShape->GetEventHandler()->OnLeftClick(x, y, keys, attachment);
}

void wxLabelShape::OnRightClick(double x, double y, int keys, int attachment)
{
  m_lineShape->GetEventHandler()->OnRightClick(x, y, keys, attachment);
}
