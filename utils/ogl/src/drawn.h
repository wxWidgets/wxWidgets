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

class wxDrawnShape;
class wxPseudoMetaFile: public wxObject
{
 DECLARE_DYNAMIC_CLASS(wxPseudoMetaFile)
 public:
  wxPseudoMetaFile();
  wxPseudoMetaFile(wxPseudoMetaFile& mf);
  ~wxPseudoMetaFile();

  void Draw(wxDC& dc, float xoffset, float yoffset);

#ifdef PROLOGIO
  void WritePrologAttributes(wxExpr *clause);
  void ReadPrologAttributes(wxExpr *clause);
#endif

  void Clear();

  void Copy(wxPseudoMetaFile& copy);

  void Scale(float sx, float sy);
  void ScaleTo(float w, float h); // Scale to fit size
  void Translate(float x, float y);

  // Rotate about the given axis by theta radians from the x axis.
  void Rotate(float x, float y, float theta);

  bool LoadFromMetaFile(char *filename, float *width, float *height);

  void GetBounds(float *minX, float *minY, float *maxX, float *maxY);

  // Calculate size from current operations
  void CalculateSize(wxDrawnShape* shape);

  inline wxList& GetOutlineColours() const { return (wxList&) m_outlineColours; }
  inline wxList& GetFillColours() const { return (wxList&) m_fillColours; }
  inline void SetRotateable(bool rot) { m_rotateable = rot; }
  inline bool GetRotateable() const { return m_rotateable; }

  inline void SetSize(float w, float h) { m_width = w; m_height = h; }

  inline void SetFillBrush(wxBrush* brush) { m_fillBrush = brush; }
  inline wxBrush* GetFillBrush() const { return m_fillBrush; }

  inline void SetOutlinePen(wxPen* pen) { m_outlinePen = pen; }
  inline wxPen* GetOutlinePen() const { return m_outlinePen; }

public:
  /// Set of functions for drawing into a pseudo metafile.
  /// They use integers, but doubles are used internally for accuracy
  /// when scaling.

  virtual void DrawLine(const wxPoint& pt1, const wxPoint& pt2);
  virtual void DrawRectangle(const wxRect& rect);
  virtual void DrawRoundedRectangle(const wxRect& rect, double radius);
  virtual void DrawEllipse(const wxRect& rect);
  virtual void DrawPoint(const wxPoint& pt);
  virtual void DrawText(const wxString& text, const wxPoint& pt);
  virtual void DrawLines(int n, wxPoint pts[]);
  virtual void DrawPolygon(int n, wxPoint pts[]);
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
  float             m_width;
  float             m_height;
  wxList            m_ops; // List of drawing operations (see drawnp.h)
  wxList            m_gdiObjects; // List of pens, brushes and fonts for this object.

  // Pen/brush specifying outline/fill colours
  // to override operations.
  wxPen*            m_outlinePen;
  wxBrush*          m_fillBrush;
  wxList            m_outlineColours; // List of the GDI operations that comprise the outline
  wxList            m_fillColours; // List of the GDI operations that fill the shape
  float             m_currentRotation;
};

class wxDrawnShape: public wxRectangleShape
{
 DECLARE_DYNAMIC_CLASS(wxDrawnShape)
 public:
  wxDrawnShape();
  ~wxDrawnShape();

  void OnDraw(wxDC& dc);

#ifdef PROLOGIO
  // Prolog database stuff
  char *GetFunctor();
  void WritePrologAttributes(wxExpr *clause);
  void ReadPrologAttributes(wxExpr *clause);
#endif

  // Does the copying for this object
  void Copy(wxShape& copy);

  void Scale(float sx, float sy);
  void Translate(float x, float y);
  // Rotate about the given axis by theta radians from the x axis.
  void Rotate(float x, float y, float theta);

  // Get current rotation
  inline float GetRotation() const { return m_rotation; }

  void SetSize(float w, float h, bool recursive = TRUE);
  bool LoadFromMetaFile(char *filename);

  inline void SetSaveToFile(bool save) { m_saveToFile = save; }
  inline wxPseudoMetaFile& GetMetaFile() const { return (wxPseudoMetaFile&) m_metafile; }

  /// Set of functions for drawing into a pseudo metafile.
  /// They use integers, but doubles are used internally for accuracy
  /// when scaling.

  virtual void DrawLine(const wxPoint& pt1, const wxPoint& pt2);
  virtual void DrawRectangle(const wxRect& rect);
  virtual void DrawRoundedRectangle(const wxRect& rect, double radius);
  virtual void DrawEllipse(const wxRect& rect);
  virtual void DrawPoint(const wxPoint& pt);
  virtual void DrawText(const wxString& text, const wxPoint& pt);
  virtual void DrawLines(int n, wxPoint pts[]);
  virtual void DrawPolygon(int n, wxPoint pts[]);
  virtual void DrawSpline(int n, wxPoint pts[]);

  virtual void SetClippingRect(const wxRect& rect);
  virtual void DestroyClippingRect();

  virtual void SetPen(wxPen* pen, bool isOutline = FALSE);     // TODO: eventually, just store GDI object attributes, not actual
  virtual void SetBrush(wxBrush* brush, bool isFill = FALSE);  // pens/brushes etc.
  virtual void SetFont(wxFont* font);
  virtual void SetTextColour(const wxColour& colour);
  virtual void SetBackgroundColour(const wxColour& colour);
  virtual void SetBackgroundMode(int mode);

  // Set the width/height according to the shapes in the metafile.
  // Call this after drawing into the shape.
  inline void CalculateSize() { m_metafile.CalculateSize(this); }

private:
  wxPseudoMetaFile      m_metafile;

  // Don't save all wxDrawnShape metafiles to file: sometimes
  // we take the metafile data from a symbol library.
  bool                  m_saveToFile;
};

#endif
    // _DRAWN_H_

