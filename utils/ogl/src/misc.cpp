/////////////////////////////////////////////////////////////////////////////
// Name:        misc.cpp
// Purpose:     Miscellaneous OGL support functions
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "misc.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/wxexpr.h>

#include <wx/types.h>

#if wxUSE_IOSTREAMH
#include <iostream.h>
#else
#include <iostream>
#endif
#include <ctype.h>
#include <math.h>
#include <stdlib.h>

#include "basic.h"
#include "basicp.h"
#include "misc.h"
#include "constrnt.h"
#include "composit.h"

wxFont*         g_oglNormalFont;
wxPen*          g_oglBlackPen;
wxPen*          g_oglWhiteBackgroundPen;
wxPen*          g_oglTransparentPen;
wxBrush*        g_oglWhiteBackgroundBrush;
wxPen*          g_oglBlackForegroundPen;
wxCursor*       g_oglBullseyeCursor = NULL;

char*           oglBuffer = NULL;

wxList          oglObjectCopyMapping(wxKEY_INTEGER);

void wxOGLInitialize()
{
  g_oglBullseyeCursor = new wxCursor(wxCURSOR_BULLSEYE);

  g_oglNormalFont = new wxFont(10, wxSWISS, wxNORMAL, wxNORMAL);

  g_oglBlackPen = new wxPen("BLACK", 1, wxSOLID);

  g_oglWhiteBackgroundPen = new wxPen("WHITE", 1, wxSOLID);
  g_oglTransparentPen = new wxPen("WHITE", 1, wxTRANSPARENT);
  g_oglWhiteBackgroundBrush = new wxBrush("WHITE", wxSOLID);
  g_oglBlackForegroundPen = new wxPen("BLACK", 1, wxSOLID);

  OGLInitializeConstraintTypes();

  // Initialize big buffer used when writing images
  oglBuffer = new char[3000];

  if (!oglPopupDivisionMenu)
  {
    oglPopupDivisionMenu = new wxMenu("", (wxFunction)oglGraphicsDivisionMenuProc);
    oglPopupDivisionMenu->Append(DIVISION_MENU_SPLIT_HORIZONTALLY, "Split horizontally");
    oglPopupDivisionMenu->Append(DIVISION_MENU_SPLIT_VERTICALLY, "Split vertically");
    oglPopupDivisionMenu->AppendSeparator();
    oglPopupDivisionMenu->Append(DIVISION_MENU_EDIT_LEFT_EDGE, "Edit left edge");
    oglPopupDivisionMenu->Append(DIVISION_MENU_EDIT_TOP_EDGE, "Edit top edge");
  }
}

void wxOGLCleanUp()
{
    if (oglBuffer)
    {
        delete[] oglBuffer;
        oglBuffer = NULL;
    }
    oglBuffer = NULL;
    if (oglPopupDivisionMenu)
    {
        delete oglPopupDivisionMenu;
        oglPopupDivisionMenu = NULL;
    }
    if (g_oglBullseyeCursor)
    {
        delete g_oglBullseyeCursor;
        g_oglBullseyeCursor = NULL;
    }

    if (g_oglNormalFont)
    {
        delete g_oglNormalFont;
        g_oglNormalFont = NULL;
    }
    if (g_oglBlackPen)
    {
        delete g_oglBlackPen;
        g_oglBlackPen = NULL;
    }
    if (g_oglWhiteBackgroundPen)
    {
        delete g_oglWhiteBackgroundPen;
        g_oglWhiteBackgroundPen = NULL;
    }
    if (g_oglTransparentPen)
    {
        delete g_oglTransparentPen;
        g_oglTransparentPen = NULL;
    }
    if (g_oglWhiteBackgroundBrush)
    {
        delete g_oglWhiteBackgroundBrush;
        g_oglWhiteBackgroundBrush = NULL;
    }
    if (g_oglBlackForegroundPen)
    {
        delete g_oglBlackForegroundPen;
        g_oglBlackForegroundPen = NULL;
    }

    OGLCleanUpConstraintTypes();
}

wxFont *oglMatchFont(int point_size)
{
  wxFont *font = wxTheFontList->FindOrCreateFont(point_size, wxSWISS, wxNORMAL, wxNORMAL);
#if 0
  switch (point_size)
  {
    case 4:
      font = swiss_font_4;
      break;
    case 6:
      font = swiss_font_6;
      break;
    case 8:
      font = swiss_font_8;
      break;
    case 12:
      font = swiss_font_12;
      break;
    case 14:
      font = swiss_font_14;
      break;
    case 18:
      font = swiss_font_18;
      break;
    case 24:
      font = swiss_font_24;
      break;
    default:
    case 10:
      font = swiss_font_10;
      break;
  }
#endif
  return font;
}

int FontSizeDialog(wxFrame *parent, int old_size)
{
  if (old_size <= 0)
    old_size = 10;
  char buf[40];
  sprintf(buf, "%d", old_size);
  wxString ans = wxGetTextFromUser("Enter point size", "Font size", buf, parent);
  if (ans == "")
    return 0;

  int new_size = atoi(ans);
  if ((new_size <= 0) || (new_size > 40))
  {
    wxMessageBox("Invalid point size!", "Error", wxOK);
    return 0;
  }
  return new_size;
/*    
  char *strings[8];
  strings[0] = "4";
  strings[1] = "6";
  strings[2] = "8";
  strings[3] = "10";
  strings[4] = "12";
  strings[5] = "14";
  strings[6] = "18";
  strings[7] = "24";
  char *ans = wxGetSingleChoice("Choose", "Choose a font size", 8, strings, parent);
  if (ans)
  {
    int size;
    sscanf(ans, "%d", &size);
    return oglMatchFont(size);
  }
  else return NULL;
*/
}

// Centre a list of strings in the given box. xOffset and yOffset are the
// the positions that these lines should be relative to, and this might be
// the same as m_xpos, m_ypos, but might be zero if formatting from left-justifying.
void oglCentreText(wxDC& dc, wxList *text_list,
                double m_xpos, double m_ypos, double width, double height,
                int formatMode)
{
  int n = text_list->Number();

  if (!text_list || (n == 0))
    return;

  // First, get maximum dimensions of box enclosing text

  long char_height = 0;
  long max_width = 0;
  long current_width = 0;

  // Store text extents for speed
  double *widths = new double[n];

  wxNode *current = text_list->First();
  int i = 0;
  while (current)
  {
    wxShapeTextLine *line = (wxShapeTextLine *)current->Data();
    dc.GetTextExtent(line->GetText(), &current_width, &char_height);
    widths[i] = current_width;

    if (current_width > max_width)
      max_width = current_width;
    current = current->Next();
    i ++;
  }

  double max_height = n*char_height;

  double xoffset, yoffset, xOffset, yOffset;

  if (formatMode & FORMAT_CENTRE_VERT)
  {
    if (max_height < height)
      yoffset = (double)(m_ypos - (height/2.0) + (height - max_height)/2.0);
    else
      yoffset = (double)(m_ypos - (height/2.0));
    yOffset = m_ypos;
  }
  else
  {
    yoffset = 0.0;
    yOffset = 0.0;
  }

  if (formatMode & FORMAT_CENTRE_HORIZ)
  {
    xoffset = (double)(m_xpos - width/2.0);
    xOffset = m_xpos;
  }
  else
  {
    xoffset = 0.0;
    xOffset = 0.0;
  }

  current = text_list->First();
  i = 0;

  while (current)
  {
    wxShapeTextLine *line = (wxShapeTextLine *)current->Data();

    double x;
    if ((formatMode & FORMAT_CENTRE_HORIZ) && (widths[i] < width))
      x = (double)((width - widths[i])/2.0 + xoffset);
    else
      x = xoffset;
    double y = (double)(i*char_height + yoffset);

    line->SetX( x - xOffset ); line->SetY( y - yOffset );
    current = current->Next();
    i ++;
  }

  delete widths;
}

// Centre a list of strings in the given box
void oglCentreTextNoClipping(wxDC& dc, wxList *text_list,
                              double m_xpos, double m_ypos, double width, double height)
{
  int n = text_list->Number();

  if (!text_list || (n == 0))
    return;

  // First, get maximum dimensions of box enclosing text

  long char_height = 0;
  long max_width = 0;
  long current_width = 0;

  // Store text extents for speed
  double *widths = new double[n];

  wxNode *current = text_list->First();
  int i = 0;
  while (current)
  {
    wxShapeTextLine *line = (wxShapeTextLine *)current->Data();
    dc.GetTextExtent(line->GetText(), &current_width, &char_height);
    widths[i] = current_width;

    if (current_width > max_width)
      max_width = current_width;
    current = current->Next();
    i ++;
  }

  double max_height = n*char_height;

  double yoffset = (double)(m_ypos - (height/2.0) + (height - max_height)/2.0);

  double xoffset = (double)(m_xpos - width/2.0);

  current = text_list->First();
  i = 0;

  while (current)
  {
    wxShapeTextLine *line = (wxShapeTextLine *)current->Data();

    double x = (double)((width - widths[i])/2.0 + xoffset);
    double y = (double)(i*char_height + yoffset);

    line->SetX( x - m_xpos ); line->SetY( y - m_ypos );
    current = current->Next();
    i ++;
  }
  delete widths;
}

void oglGetCentredTextExtent(wxDC& dc, wxList *text_list,
                              double m_xpos, double m_ypos, double width, double height,
                              double *actual_width, double *actual_height)
{
  int n = text_list->Number();

  if (!text_list || (n == 0))
  {
    *actual_width = 0;
    *actual_height = 0;
    return;
  }

  // First, get maximum dimensions of box enclosing text

  long char_height = 0;
  long max_width = 0;
  long current_width = 0;

  wxNode *current = text_list->First();
  int i = 0;
  while (current)
  {
    wxShapeTextLine *line = (wxShapeTextLine *)current->Data();
    dc.GetTextExtent(line->GetText(), &current_width, &char_height);

    if (current_width > max_width)
      max_width = current_width;
    current = current->Next();
    i ++;
  }

  *actual_height = n*char_height;
  *actual_width = max_width;
}

// Format a string to a list of strings that fit in the given box.
// Interpret %n and 10 or 13 as a new line.
wxStringList *oglFormatText(wxDC& dc, const wxString& text, double width, double height, int formatMode)
{
  // First, parse the string into a list of words
  wxStringList word_list;

  // Make new lines into NULL strings at this point
  int i = 0; int j = 0; int len = strlen(text);
  char word[200]; word[0] = 0;
  bool end_word = FALSE; bool new_line = FALSE;
  while (i < len)
  {
    switch (text[i])
    {
      case '%':
      {
        i ++;
        if (i == len)
        { word[j] = '%'; j ++; }
        else
        {
          if (text[i] == 'n')
          { new_line = TRUE; end_word = TRUE; i++; }
          else
          { word[j] = '%'; j ++; word[j] = text[i]; j ++; i ++; }
        }
        break;
      }
      case 10:
      {
        new_line = TRUE; end_word = TRUE; i++;
        break;
      }
      case 13:
      {
        new_line = TRUE; end_word = TRUE; i++;
      }
      case ' ':
      {
        end_word = TRUE;
        i ++;
        break;
      }
      default:
      {
        word[j] = text[i];
        j ++; i ++;
        break;
      }
    }
    if (i == len) end_word = TRUE;
    if (end_word)
    {
      word[j] = 0;
      j = 0;
      word_list.Append((wxObject *)copystring(word));
      end_word = FALSE;
    }
    if (new_line)
    {
      word_list.Append((wxObject *)NULL);
      new_line = FALSE;
    }
  }
  // Now, make a list of strings which can fit in the box
  wxStringList *string_list = new wxStringList;

  char buffer[400];
  buffer[0] = 0;
  wxNode *node = word_list.First();
  long x, y;

  while (node)
  {
    wxString oldBuffer(buffer);

    char *s = (char *)node->Data();
    if (!s)
    {
      // FORCE NEW LINE
      if (strlen(buffer) > 0)
        string_list->Add(buffer);

      buffer[0] = 0;
    }
    else
    {
      if (buffer[0] != 0)
        strcat(buffer, " ");

      strcat(buffer, s);
      dc.GetTextExtent(buffer, &x, &y);

      // Don't fit within the bounding box if we're fitting shape to contents
      if ((x > width) && !(formatMode & FORMAT_SIZE_TO_CONTENTS))
      {
        // Deal with first word being wider than box
        if (oldBuffer.Length() > 0)
          string_list->Add(oldBuffer);

        buffer[0] = 0;
        strcat(buffer, s);
      }
    }

    node = node->Next();
  }
  if (buffer[0] != 0)
    string_list->Add(buffer);

  return string_list;
}

void oglDrawFormattedText(wxDC& dc, wxList *text_list,
                       double m_xpos, double m_ypos, double width, double height,
                       int formatMode)
{
  double xoffset, yoffset;
  if (formatMode & FORMAT_CENTRE_HORIZ)
    xoffset = m_xpos;
  else
    xoffset = (double)(m_xpos - (width / 2.0));

  if (formatMode & FORMAT_CENTRE_VERT)
    yoffset = m_ypos;
  else
    yoffset = (double)(m_ypos - (height / 2.0));

  dc.SetClippingRegion(
                    (double)(m_xpos - width/2.0), (double)(m_ypos - height/2.0),
                    (double)width, (double)height);

  wxNode *current = text_list->First();
  while (current)
  {
    wxShapeTextLine *line = (wxShapeTextLine *)current->Data();

    dc.DrawText(line->GetText(), WXROUND(xoffset + line->GetX()), WXROUND(yoffset + line->GetY()));
    current = current->Next();
  }

  dc.DestroyClippingRegion();
}

/*
 * Find centroid given list of points comprising polyline
 *
 */

void oglFindPolylineCentroid(wxList *points, double *x, double *y)
{
  double xcount = 0;
  double ycount = 0;

  wxNode *node = points->First();
  while (node)
  {
    wxRealPoint *point = (wxRealPoint *)node->Data();
    xcount += point->x;
    ycount += point->y;
    node = node->Next();
  }

  *x = (xcount/points->Number());
  *y = (ycount/points->Number());
}

/*
 * Check that (x1, y1) -> (x2, y2) hits (x3, y3) -> (x4, y4).
 * If so, ratio1 gives the proportion along the first line
 * that the intersection occurs (or something like that).
 * Used by functions below.
 *
 */
void oglCheckLineIntersection(double x1, double y1, double x2, double y2, 
                             double x3, double y3, double x4, double y4,
                             double *ratio1, double *ratio2)
{
  double denominator_term = (y4 - y3)*(x2 - x1) - (y2 - y1)*(x4 - x3);
  double numerator_term = (x3 - x1)*(y4 - y3) + (x4 - x3)*(y1 - y3);

  double line_constant;
  double length_ratio = 1.0;
  double k_line = 1.0;

  // Check for parallel lines
  if ((denominator_term < 0.005) && (denominator_term > -0.005))
    line_constant = -1.0;
  else
    line_constant = numerator_term/denominator_term;

  // Check for intersection
  if ((line_constant < 1.0) && (line_constant > 0.0))
  {
    // Now must check that other line hits 
    if (((y4 - y3) < 0.005) && ((y4 - y3) > -0.005))
      k_line = ((x1 - x3) + line_constant*(x2 - x1))/(x4 - x3);
    else
      k_line = ((y1 - y3) + line_constant*(y2 - y1))/(y4 - y3);

    if ((k_line >= 0.0) && (k_line < 1.0))
      length_ratio = line_constant;
    else
      k_line = 1.0;
  }
  *ratio1 = length_ratio;
  *ratio2 = k_line;
}

/*
 * Find where (x1, y1) -> (x2, y2) hits one of the lines in xvec, yvec.
 * (*x3, *y3) is the point where it hits.
 *
 */
void oglFindEndForPolyline(double n, double xvec[], double yvec[], 
                           double x1, double y1, double x2, double y2, double *x3, double *y3)
{
  int i;
  double lastx = xvec[0];
  double lasty = yvec[0];

  double min_ratio = 1.0;
  double line_ratio;
  double other_ratio;

  for (i = 1; i < n; i++)
  {
    oglCheckLineIntersection(x1, y1, x2, y2, lastx, lasty, xvec[i], yvec[i],
                            &line_ratio, &other_ratio);
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

    if (line_ratio < min_ratio)
      min_ratio = line_ratio;
  }

  *x3 = (x1 + (x2 - x1)*min_ratio);
  *y3 = (y1 + (y2 - y1)*min_ratio);

}

/*
 * Find where the line hits the box.
 *
 */

void oglFindEndForBox(double width, double height, 
                      double x1, double y1,         // Centre of box (possibly)
                      double x2, double y2,         // other end of line
                      double *x3, double *y3)       // End on box edge
{
  double xvec[5];
  double yvec[5];

  xvec[0] = (double)(x1 - width/2.0);
  yvec[0] = (double)(y1 - height/2.0);
  xvec[1] = (double)(x1 - width/2.0);
  yvec[1] = (double)(y1 + height/2.0);
  xvec[2] = (double)(x1 + width/2.0);
  yvec[2] = (double)(y1 + height/2.0);
  xvec[3] = (double)(x1 + width/2.0);
  yvec[3] = (double)(y1 - height/2.0);
  xvec[4] = (double)(x1 - width/2.0);
  yvec[4] = (double)(y1 - height/2.0);

  oglFindEndForPolyline(5, xvec, yvec, x2, y2, x1, y1, x3, y3);
}

/*
 * Find where the line hits the circle.
 *
 */

void oglFindEndForCircle(double radius, 
                         double x1, double y1,  // Centre of circle
                         double x2, double y2,  // Other end of line
                         double *x3, double *y3)
{
  double H = (double)sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));

  if (H == 0.0)
  {
    *x3 = x1;
    *y3 = y1;
  }
  else
  {
   *y3 = radius * (y2 - y1)/H + y1;
   *x3 = radius * (x2 - x1)/H + x1;
  }
}

/*
 * Given the line (x1, y1) -> (x2, y2), and an arrow size of given length and width,
 * return the position of the tip of the arrow and the left and right vertices of the arrow.
 *
 */

void oglGetArrowPoints(double x1, double y1, double x2, double y2,
                      double length, double width,
                      double *tip_x, double *tip_y,
                      double *side1_x, double *side1_y,
                      double *side2_x, double *side2_y)
{
  double l = (double)sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));

  if (l < 0.01)
    l = (double) 0.01;

  double i_bar = (x2 - x1)/l;
  double j_bar = (y2 - y1)/l;

  double x3 = (- length*i_bar) + x2;
  double y3 = (- length*j_bar) + y2;

  *side1_x = width*(-j_bar) + x3;
  *side1_y = width*i_bar + y3;

  *side2_x = -width*(-j_bar) + x3;
  *side2_y = -width*i_bar + y3;

  *tip_x = x2; *tip_y = y2;
}

/*
 * Given an ellipse and endpoints of a line, returns the point at which
 * the line touches the ellipse in values x4, y4.
 * This function assumes that the centre of the ellipse is at x1, y1, and the
 * ellipse has a width of width1 and a height of height1. It also assumes you are
 * wanting to draw an arc FROM point x2, y2 TOWARDS point x3, y3.
 * This function calculates the x,y coordinates of the intersection point of 
 * the arc with the ellipse.
 * Author: Ian Harrison
 */

void oglDrawArcToEllipse(double x1, double y1, double width1, double height1, double x2, double y2, double x3, double y3,
  double *x4, double *y4)
{
  double a1 = (double)(width1/2.0);
  double b1 = (double)(height1/2.0);

  // These are required to give top left x and y coordinates for DrawEllipse
//  double top_left_x1 = (double)(x1 - a1);
//  double top_left_y1 = (double)(y1 - b1);
/*
  // Check for vertical line
  if (fabs(x2 - x3) < 0.05)
  {
    *x4 = x3;
    if (y2 < y3)
      *y4 = (double)(y1 - b1);
    else
      *y4 = (double)(y1 + b1);
    return;
  }
*/  
  // Check that x2 != x3
  if (fabs(x2 - x3) < 0.05)
  {
    *x4 = x2;
    if (y3 > y2)
      *y4 = (double)(y1 - sqrt((b1*b1 - (((x2-x1)*(x2-x1))*(b1*b1)/(a1*a1)))));
    else
      *y4 = (double)(y1 + sqrt((b1*b1 - (((x2-x1)*(x2-x1))*(b1*b1)/(a1*a1)))));
    return;
  }

  // Calculate the x and y coordinates of the point where arc intersects ellipse

  double A, B, C, D, E, F, G, H, K;
  double ellipse1_x, ellipse1_y;

  A = (double)(1/(a1 * a1));
  B = (double)((y3 - y2) * (y3 - y2)) / ((x3 - x2) * (x3 - x2) * b1 * b1);
  C = (double)(2 * (y3 - y2) * (y2 - y1)) / ((x3 - x2) * b1 * b1);
  D = (double)((y2 - y1) * (y2 - y1)) / (b1 * b1);
  E = (double)(A + B);
  F = (double)(C - (2 * A * x1) - (2 * B * x2));
  G = (double)((A * x1 * x1) + (B * x2 * x2) - (C * x2) + D - 1);
  H = (double)((y3 - y2) / (x3 - x2));
  K = (double)((F * F) - (4 * E * G));

  if (K >= 0)
  // In this case the line intersects the ellipse, so calculate intersection
  { 
    if(x2 >= x1)
    {
      ellipse1_x = (double)(((F * -1) + sqrt(K)) / (2 * E));
      ellipse1_y = (double)((H * (ellipse1_x - x2)) + y2);
    }
    else
    {
      ellipse1_x = (double)(((F * -1) -  sqrt(K)) / (2 * E));
      ellipse1_y = (double)((H * (ellipse1_x - x2)) + y2);
    }
  }
  else
  // in this case, arc does not intersect ellipse, so just draw arc
  {
    ellipse1_x = x3;
    ellipse1_y = y3;
  }
  *x4 = ellipse1_x;
  *y4 = ellipse1_y;

/*	    
  // Draw a little circle (radius = 2) at the end of the arc where it hits
  // the ellipse .

  double circle_x = ellipse1_x - 2.0;
  double circle_y = ellipse1_y - 2.0;
  m_canvas->DrawEllipse(circle_x, circle_y, 4.0, 4.0);
*/
}

// Update a list item from a list of strings
void UpdateListBox(wxListBox *item, wxList *list)
{
  item->Clear();
  if (!list)
    return;

  wxNode *node = list->First();
  while (node)
  {
    char *s = (char *)node->Data();
    item->Append(s);
    node = node->Next();
  }
}

bool oglRoughlyEqual(double val1, double val2, double tol)
{
    return ( (val1 < (val2 + tol)) && (val1 > (val2 - tol)) &&
             (val2 < (val1 + tol)) && (val2 > (val1 - tol)));
}

/*
 * Hex<->Dec conversion
 */

// Array used in DecToHex conversion routine.
static char sg_HexArray[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B',
  'C', 'D', 'E', 'F' };

// Convert 2-digit hex number to decimal
unsigned int oglHexToDec(char* buf)
{
  int firstDigit, secondDigit;
  
  if (buf[0] >= 'A')
    firstDigit = buf[0] - 'A' + 10;
  else
    firstDigit = buf[0] - '0';

  if (buf[1] >= 'A')
    secondDigit = buf[1] - 'A' + 10;
  else
    secondDigit = buf[1] - '0';
    
  return firstDigit * 16 + secondDigit;
}

// Convert decimal integer to 2-character hex string
void oglDecToHex(unsigned int dec, char *buf)
{
    int firstDigit = (int)(dec/16.0);
    int secondDigit = (int)(dec - (firstDigit*16.0));
    buf[0] = sg_HexArray[firstDigit];
    buf[1] = sg_HexArray[secondDigit];
    buf[2] = 0;
}

// 3-digit hex to wxColour
wxColour oglHexToColour(const wxString& hex)
{
    if (hex.Length() == 6)
	{
        char buf[7];
        strncpy(buf, hex, 7);
        unsigned int r = oglHexToDec((char *)buf);
        unsigned int g = oglHexToDec((char *)(buf+2));
        unsigned int b = oglHexToDec((char *)(buf+4));
        return wxColour(r, g, b);
	}
	else
	    return wxColour(0,0,0);
}

// RGB to 3-digit hex
wxString oglColourToHex(const wxColour& colour)
{
    char buf[7];
    unsigned int red = colour.Red();
    unsigned int green = colour.Green();
    unsigned int blue = colour.Blue();

    oglDecToHex(red, buf);
    oglDecToHex(green, buf+2);
    oglDecToHex(blue, buf+4);

    return wxString(buf);
}


