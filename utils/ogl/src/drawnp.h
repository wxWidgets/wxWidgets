/////////////////////////////////////////////////////////////////////////////
// Name:        drawnp.h
// Purpose:     Private header for wxDrawnShape
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _OGL_DRAWNP_H_
#define _OGL_DRAWNP_H_

#ifdef __GNUG__
#pragma interface "drawnp.h"
#endif

#include "drawn.h"

/*
 * Drawing operations
 *
 */
 
#define  DRAWOP_SET_PEN             1
#define  DRAWOP_SET_BRUSH           2
#define  DRAWOP_SET_FONT            3
#define  DRAWOP_SET_TEXT_COLOUR     4
#define  DRAWOP_SET_BK_COLOUR       5
#define  DRAWOP_SET_BK_MODE         6
#define  DRAWOP_SET_CLIPPING_RECT   7
#define  DRAWOP_DESTROY_CLIPPING_RECT 8

/*
#define  DRAWOP_CREATE_PEN          10
#define  DRAWOP_CREATE_BRUSH        11
#define  DRAWOP_CREATE_FONT         12
*/

#define  DRAWOP_DRAW_LINE           20
#define  DRAWOP_DRAW_POLYLINE       21
#define  DRAWOP_DRAW_POLYGON        22
#define  DRAWOP_DRAW_RECT           23
#define  DRAWOP_DRAW_ROUNDED_RECT   24
#define  DRAWOP_DRAW_ELLIPSE        25
#define  DRAWOP_DRAW_POINT          26
#define  DRAWOP_DRAW_ARC            27
#define  DRAWOP_DRAW_TEXT           28
#define  DRAWOP_DRAW_SPLINE         29

/*
 * Base, virtual class
 *
 */
 
class wxDrawOp: public wxObject
{
 public:
  int op;

  inline wxDrawOp(int theOp) { op = theOp; }
  inline ~wxDrawOp() {}
  inline virtual void Scale(float xScale, float yScale) {};
  inline virtual void Translate(float x, float y) {};
  inline virtual void Rotate(float x, float y, float sinTheta, float cosTheta) {};
  virtual void Do(wxDC& dc, float xoffset, float yoffset) = 0;
  virtual wxDrawOp *Copy(wxPseudoMetaFile *newImage) = 0;
  virtual wxExpr *WritewxExpr(wxPseudoMetaFile *image) = 0;
  virtual void ReadwxExpr(wxPseudoMetaFile *image, wxExpr *expr) = 0;
};

/*
 * Set font, brush, text colour
 *
 */
 
class wxOpSetGDI: public wxDrawOp
{
 public:
  int mode;
  int gdiIndex;
  wxPseudoMetaFile *image;
  unsigned char r;
  unsigned char g;
  unsigned char b;
  wxOpSetGDI(int theOp, wxPseudoMetaFile *theImage, int theGdiIndex, int theMode = 0);
  void Do(wxDC& dc, float xoffset, float yoffset);
  wxDrawOp *Copy(wxPseudoMetaFile *newImage);
  wxExpr *WritewxExpr(wxPseudoMetaFile *image);
  void ReadwxExpr(wxPseudoMetaFile *image, wxExpr *expr);
};

/*
 * Set/destroy clipping
 *
 */
 
class wxOpSetClipping: public wxDrawOp
{
 public:
  float x1;
  float y1;
  float x2;
  float y2;
  wxOpSetClipping(int theOp, float theX1, float theY1, float theX2, float theY2);
  void Do(wxDC& dc, float xoffset, float yoffset);
  void Scale(float xScale, float yScale);
  void Translate(float x, float y);
  wxDrawOp *Copy(wxPseudoMetaFile *newImage);
  wxExpr *WritewxExpr(wxPseudoMetaFile *image);
  void ReadwxExpr(wxPseudoMetaFile *image, wxExpr *expr);
};

/*
 * Draw line, rectangle, rounded rectangle, ellipse, point, arc, text
 *
 */
 
class wxOpDraw: public wxDrawOp
{
 public:
  float x1;
  float y1;
  float x2;
  float y2;
  float x3;
  float radius;
  char *textString;

  wxOpDraw(int theOp, float theX1, float theY1, float theX2, float theY2,
         float radius = 0.0, char *s = NULL);
  ~wxOpDraw();
  void Do(wxDC& dc, float xoffset, float yoffset);
  void Scale(float scaleX, float scaleY);
  void Translate(float x, float y);
  void Rotate(float x, float y, float sinTheta, float cosTheta);
  wxDrawOp *Copy(wxPseudoMetaFile *newImage);
  wxExpr *WritewxExpr(wxPseudoMetaFile *image);
  void ReadwxExpr(wxPseudoMetaFile *image, wxExpr *expr);
};

/*
 * Draw polyline, spline, polygon
 *
 */

class wxOpPolyDraw: public wxDrawOp
{
 public:
  wxRealPoint *points;
  int noPoints;
  
  wxOpPolyDraw(int theOp, int n, wxRealPoint *thePoints);
  ~wxOpPolyDraw();
  void Do(wxDC& dc, float xoffset, float yoffset);
  void Scale(float scaleX, float scaleY);
  void Translate(float x, float y);
  void Rotate(float x, float y, float sinTheta, float cosTheta);
  wxDrawOp *Copy(wxPseudoMetaFile *newImage);
  wxExpr *WritewxExpr(wxPseudoMetaFile *image);
  void ReadwxExpr(wxPseudoMetaFile *image, wxExpr *expr);
};

#endif
 // _OGL_DRAWNP_H_


