/////////////////////////////////////////////////////////////////////////////
// Name:        linesp.h
// Purpose:     Lines private header file
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _OGL_LINESP_H_
#define _OGL_LINESP_H_

#ifdef __GNUG__
#pragma interface "linesp.h"
#endif

class wxLineShape;
class wxLineControlPoint: public wxControlPoint
{
  DECLARE_DYNAMIC_CLASS(wxLineControlPoint)
  friend class wxLineShape;
 public:

  wxLineControlPoint(wxShapeCanvas *the_canvas = NULL, wxShape *object = NULL, float size = 0.0,
     float x = 0.0, float y = 0.0, int the_type = 0);
  ~wxLineControlPoint();

  void OnDraw(wxDC& dc);
  void OnDragLeft(bool draw, float x, float y, int keys=0, int attachment = 0);
  void OnBeginDragLeft(float x, float y, int keys=0, int attachment = 0);
  void OnEndDragLeft(float x, float y, int keys=0, int attachment = 0);

  void OnDragRight(bool draw, float x, float y, int keys=0, int attachment = 0);
  void OnBeginDragRight(float x, float y, int keys=0, int attachment = 0);
  void OnEndDragRight(float x, float y, int keys=0, int attachment = 0);

public:

  int           m_type;
  wxRealPoint*  m_point;  // Line point

};

/*
 * Temporary arc label object
 */
 
class wxLabelShape: public wxRectangleShape
{
  DECLARE_DYNAMIC_CLASS(wxLabelShape)

 public:
  wxLabelShape(wxLineShape *parent = NULL, wxShapeRegion *region = NULL, float w = 0.0, float h = 0.0);
  ~wxLabelShape();

  void OnDraw(wxDC& dc);
  void OnDrawContents(wxDC& dc);
  void OnLeftClick(float x, float y, int keys = 0, int attachment = 0);
  void OnRightClick(float x, float y, int keys = 0, int attachment = 0);
  void OnDragLeft(bool draw, float x, float y, int keys=0, int attachment = 0);
  void OnBeginDragLeft(float x, float y, int keys=0, int attachment = 0);
  void OnEndDragLeft(float x, float y, int keys=0, int attachment = 0);
  bool OnMovePre(wxDC& dc, float x, float y, float old_x, float old_y, bool display = TRUE);

public:
  wxLineShape*      m_lineShape;
  wxShapeRegion*    m_shapeRegion;

};

/*
 * Get the point on the given line (x1, y1) (x2, y2)
 * distance 'length' along from the end,
 * returned values in x and y
 */

void GetPointOnLine(float x1, float y1, float x2, float y2,
                    float length, float *x, float *y);

#endif
    // _OGL_LINESP_H_

