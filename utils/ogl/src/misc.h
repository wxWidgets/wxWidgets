/////////////////////////////////////////////////////////////////////////////
// Name:        misc.h
// Purpose:     Miscellaneous utilities for OGL
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _OGL_MISC_H_
#define _OGL_MISC_H_

#ifdef __GNUG__
#pragma interface "misc.h"
#endif

// List to use when copying objects; may need to associate elements of new objects
// with elements of old objects, e.g. when copying constraint.s
extern wxList wxObjectCopyMapping;

/*
 * TEXT FORMATTING FUNCTIONS
 *
 */

// Centres the given list of wxShapeTextLine strings in the given box
// (changing the positions in situ). Doesn't actually draw into the DC.
void CentreText(wxDC& dc, wxList *text, float m_xpos, float m_ypos,
                float width, float height,
                int formatMode = FORMAT_CENTRE_HORIZ | FORMAT_CENTRE_VERT);

// Given a string, returns a list of strings that fit within the given
// width of box. Height is ignored.
wxList *FormatText(wxDC& dc, const wxString& text, float width, float height, int formatMode = 0);

// Centres the list of wxShapeTextLine strings, doesn't clip.
// Doesn't actually draw into the DC.
void CentreTextNoClipping(wxDC& dc, wxList *text_list,
                              float m_xpos, float m_ypos, float width, float height);

// Gets the maximum width and height of the given list of wxShapeTextLines.
void GetCentredTextExtent(wxDC& dc, wxList *text_list,
                              float m_xpos, float m_ypos, float width, float height,
                              float *actual_width, float *actual_height);

// Actually draw the preformatted list of wxShapeTextLines.
void DrawFormattedText(wxDC& context, wxList *text_list,
                       float m_xpos, float m_ypos, float width, float height,
                       int formatMode = FORMAT_CENTRE_HORIZ | FORMAT_CENTRE_VERT);

// Give it a list of points, finds the centre.
void find_polyline_centroid(wxList *points, float *x, float *y);

void check_line_intersection(float x1, float y1, float x2, float y2, 
                             float x3, float y3, float x4, float y4,
                             float *ratio1, float *ratio2);

void find_end_for_polyline(float n, float xvec[], float yvec[], 
                           float x1, float y1, float x2, float y2, float *x3, float *y3);


void find_end_for_box(float width, float height, 
                      float x1, float y1,         // Centre of box (possibly)
                      float x2, float y2,         // other end of line
                      float *x3, float *y3);      // End on box edge

void find_end_for_circle(float radius, 
                         float x1, float y1,  // Centre of circle
                         float x2, float y2,  // Other end of line
                         float *x3, float *y3);

void get_arrow_points(float x1, float y1, float x2, float y2,
                      float length, float width,
                      float *tip_x, float *tip_y,
                      float *side1_x, float *side1_y,
                      float *side2_x, float *side2_y);

/*
 * Given an ellipse and endpoints of a line, returns the point at which
 * the line touches the ellipse in values x4, y4.
 * This function assumes that the centre of the ellipse is at x1, y1, and the
 * ellipse has a width of a1 and a height of b1. It also assumes you are
 * wanting to draw an arc FROM point x2, y2 TOWARDS point x3, y3.
 * This function calculates the x,y coordinates of the intersection point of 
 * the arc with the ellipse.
 * Author: Ian Harrison
 */

void draw_arc_to_ellipse(float x1, float y1, float a1, float b1, float x2, float y2, float x3, float y3,
  float *x4, float *y4);

extern wxFont *g_oglNormalFont;
extern wxPen *black_pen;

extern wxPen *white_background_pen;
extern wxPen *transparent_pen;
extern wxBrush *white_background_brush;
extern wxPen *black_foreground_pen;

extern wxCursor *GraphicsBullseyeCursor;

extern wxFont *MatchFont(int point_size);

#endif
 // _OGL_MISC_H_
