/////////////////////////////////////////////////////////////////////////////
// Name:        drawn.h
// Purpose:     wxDrawnShape
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _OGL_DRAWN_H_
#define _OGL_DRAWN_H_

#ifdef __GNUG__
#pragma interface "drawn.h"
#endif

#include "basic.h"

#define oglMETAFLAGS_OUTLINE         1
#define oglMETAFLAGS_ATTACHMENTS     2

class wxDrawnShape;
class wxPseudoMetaFile: public wxObject
{
 DECLARE_DYNAMIC_CLASS(wxPseudoMetaFile)
 public:
  wxPseudoMetaFile();
  wxPseudoMetaFile(wxPseudoMetaFile& mf);
  ~wxPseudoMetaFile();

  void Draw(wxDC& dc, double xoffset, double yoffset);

#ifdef PROLOGIO
  void WriteAttributes(wxExpr *clause, int whichAngle);
  void ReadAttributes(wxExpr *clause, int whichAngle);
#endif

  void Clear();

  void Copy(wxPseudoMetaFile& copy);

  void Scale(double sx, double sy);
  void ScaleTo(double w, double h); // Scale to fit size
  void Translate(double x, double y);

  // Rotate about the given axis by theta radians from the x axis.
  void Rotate(double x, double y, double theta);

  bool LoadFromMetaFile(char *filename, double *width, double *height);

  void GetBounds(double *minX, double *minY, double *maxX, double *maxY);

  // Calculate size from current operations
  void CalculateSize(wxDrawnShape* shape);

  inline wxList& GetOutlineColours() const { return (wxList&) m_outlineColours; }
  inline wxList& GetFillColours() const { return (wxList&) m_fillColours; }
  inline void SetRotateable(bool rot) { m_rotateable = rot; }
  inline bool GetRotateable() const { return m_rotateable; }

  inline void SetSize(double w, double h) { m_width = w; m_height = h; }

  inline void SetFillBrush(wxBrush* brush) { m_fillBrush = brush; }
  inline wxBrush* GetFillBrush() const { return m_fillBrush; }

  inline void SetOutlinePen(wxPen* pen) { m_outlinePen = pen; }
  inline wxPen* GetOutlinePen() const { return m_outlinePen; }

  inline void SetOutlineOp(int op) { m_outlineOp = op; }
  inline int GetOutlineOp() const { return m_outlineOp; }

  inline wxList& GetOps() const { return (wxList&) m_ops; }

  // Is this a valid (non-empty) metafile?
  inline bool IsValid() const { return (m_ops.Number() > 0); }

public:
  /// Set of functions for drawing into a pseudo metafile.
  /// They use integers, but doubles are used internally for accuracy
  /// when scaling.

  virtual void DrawLine(const wxPoint& pt1, const wxPoint& pt2);
  virtual void DrawRectangle(const wxRect& rect);
  virtual void DrawRoundedRectangle(const wxRect& rect, double radius);
  virtual void DrawArc(const wxPoint& centrePt, const wxPoint& startPt, const wxPoint& endPt);
  virtual void DrawEllipticArc(const wxRect& rect, double startAngle, double endAngle);
  virtual void DrawEllipse(const wxRect& rect);
  virtual void DrawPoint(const wxPoint& pt);
  virtual void DrawText(const wxString& text, const wxPoint& pt);
  virtual void DrawLines(int n, wxPoint pts[]);
  // flags:
  // oglMETAFLAGS_OUTLINE: will be used for drawing the outline and
  //                       also drawing lines/arrows at the circumference.
  // oglMETAFLAGS_ATTACHMENTS: will be used for initialising attachment points at
  //                       the vertices (perhaps a rare case...)
  virtual void DrawPolygon(int n, wxPoint pts[], int flags = 0);
  virtual void DrawSpline(int n, wxPoint pts[]);

  virtual void SetClippingRect(const wxRect& rect);
  virtual void DestroyClippingRect();

  virtual void SetPen(wxPen* pen, bool isOutline = FALSE);     // TODO: eventually, just store GDI object attributes, not actual
  virtual void SetBrush(wxBrush* brush, bool isFill = FALSE);  // pens/brushes etc.
  virtual void SetFont(wxFont* font);
  virtual void SetTextColour(const wxColour& colour);
  virtual void SetBackgroundColour(const wxColour& colour);
  virtual void SetBackgroundMode(int mode);

public:
  bool              m_rotateable;
  double            m_width;
  double            m_height;
  wxList            m_ops; // List of drawing operations (see drawnp.h)
  wxList            m_gdiObjects; // List of pens, brushes and fonts for this object.
  int               m_outlineOp;  // The op representing the outline, if any

  // Pen/brush specifying outline/fill colours
  // to override operations.
  wxPen*            m_outlinePen;
  wxBrush*          m_fillBrush;
  wxList            m_outlineColours; // List of the GDI operations that comprise the outline
  wxList            m_fillColours; // List of the GDI operations that fill the shape
  double             m_currentRotation;
};

#define oglDRAWN_ANGLE_0        0
#define oglDRAWN_ANGLE_90       1
#define oglDRAWN_ANGLE_180      2
#define oglDRAWN_ANGLE_270      3

class wxDrawnShape: public wxRectangleShape
{
 DECLARE_DYNAMIC_CLASS(wxDrawnShape)
 public:
  wxDrawnShape();
  ~wxDrawnShape();

  void OnDraw(wxDC& dc);

#ifdef PROLOGIO
  // I/O
  void WriteAttributes(wxExpr *clause);
  void ReadAttributes(wxExpr *clause);
#endif

  // Does the copying for this object
  void Copy(wxShape& copy);

  void Scale(double sx, double sy);
  void Translate(double x, double y);
  // Rotate about the given axis by theta radians from the x axis.
  void Rotate(double x, double y, double theta);

  // Get current rotation
  inline double GetRotation() const { return m_rotation; }

  void SetSize(double w, double h, bool recursive = TRUE);
  bool LoadFromMetaFile(char *filename);

  inline void SetSaveToFile(bool save) { m_saveToFile = save; }
  inline wxPseudoMetaFile& GetMetaFile(int which = 0) const { return (wxPseudoMetaFile&) m_metafiles[which]; }

  void OnDrawOutline(wxDC& dc, double x, double y, double w, double h);

  // Get the perimeter point using the special outline op, if there is one,
  // otherwise use default wxRectangleShape scheme
  bool GetPerimeterPoint(double x1, double y1,
                                     double x2, double y2,
                                     double *x3, double *y3);

  /// Set of functions for drawing into a pseudo metafile.
  /// They use integers, but doubles are used internally for accuracy
  /// when scaling.

  virtual void DrawLine(const wxPoint& pt1, const wxPoint& pt2);
  virtual void DrawRectangle(const wxRect& rect);
  virtual void DrawRoundedRectangle(const wxRect& rect, double radius);
  virtual void DrawArc(const wxPoint& centrePt, const wxPoint& startPt, const wxPoint& endPt);
  virtual void DrawEllipticArc(const wxRect& rect, double startAngle, double endAngle);
  virtual void DrawEllipse(const wxRect& rect);
  virtual void DrawPoint(const wxPoint& pt);
  virtual void DrawText(const wxString& text, const wxPoint& pt);
  virtual void DrawLines(int n, wxPoint pts[]);
  virtual void DrawPolygon(int n, wxPoint pts[], int flags = 0);
  virtual void DrawSpline(int n, wxPoint pts[]);

  virtual void SetClippingRect(const wxRect& rect);
  virtual void DestroyClippingRect();

  virtual void SetDrawnPen(wxPen* pen, bool isOutline = FALSE);     // TODO: eventually, just store GDI object attributes, not actual
  virtual void SetDrawnBrush(wxBrush* brush, bool isFill = FALSE);  // pens/brushes etc.
  virtual void SetDrawnFont(wxFont* font);
  virtual void SetDrawnTextColour(const wxColour& colour);
  virtual void SetDrawnBackgroundColour(const wxColour& colour);
  virtual void SetDrawnBackgroundMode(int mode);

  // Set the width/height according to the shapes in the metafile.
  // Call this after drawing into the shape.
  inline void CalculateSize() { m_metafiles[m_currentAngle].CalculateSize(this); }

  inline void DrawAtAngle(int angle) { m_currentAngle = angle; };

  inline int GetAngle() const { return m_currentAngle; }

// Implementation
protected:
  // Which metafile do we use now? Based on current rotation and validity
  // of metafiles.
  int DetermineMetaFile(double rotation);

private:
  // One metafile for each 90 degree rotation (or just a single one).
  wxPseudoMetaFile      m_metafiles[4];

  // Don't save all wxDrawnShape metafiles to file: sometimes
  // we take the metafile data from a symbol library.
  bool                  m_saveToFile;

  // Which angle are we using/drawing into?
  int                   m_currentAngle;
};

#endif
    // _DRAWN_H_

