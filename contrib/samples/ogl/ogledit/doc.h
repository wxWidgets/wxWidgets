/////////////////////////////////////////////////////////////////////////////
// Name:        doc.h
// Purpose:     Document classes
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _OGLSAMPLE_DOC_H_
#define _OGLSAMPLE_DOC_H_

#ifdef __GNUG__
// #pragma interface
#endif

#include <wx/docview.h>
#include <wx/string.h>
#include <wx/wxexpr.h>

#include <wx/ogl/ogl.h>

#if wxUSE_STD_IOSTREAM
class ostream;
class istream;
#endif

/*
 * Override a few members for this application
 */
 
class MyDiagram: public wxDiagram
{
 public:
  MyDiagram(void) {}
  bool OnShapeSave(wxExprDatabase& db, wxShape& shape, wxExpr& expr);
  bool OnShapeLoad(wxExprDatabase& db, wxShape& shape, wxExpr& expr);
};

/*
 * A few new shape classes so we have a 1:1 mapping
 * between palette symbol and unique class
 */
 
class wxRoundedRectangleShape: public wxRectangleShape
{
  DECLARE_DYNAMIC_CLASS(wxRoundedRectangleShape)
 private:
 public:
  wxRoundedRectangleShape(double w = 0.0, double h = 0.0);
};

class wxDiamondShape: public wxPolygonShape
{
  DECLARE_DYNAMIC_CLASS(wxDiamondShape)
 private:
 public:
  wxDiamondShape(double w = 0.0, double h = 0.0);
};

/*
 * All shape event behaviour is routed through this handler, so we don't
 * have to derive from each shape class. We plug this in to each shape.
 */
 
class MyEvtHandler: public wxShapeEvtHandler
{
 public:
  wxString label;
  MyEvtHandler(wxShapeEvtHandler *prev = NULL, wxShape *shape = NULL, const wxString& lab = ""):wxShapeEvtHandler(prev, shape)
  {
    label = lab;
  }
  ~MyEvtHandler(void)
  {
  }
  void OnLeftClick(double x, double y, int keys = 0, int attachment = 0);
  void OnBeginDragRight(double x, double y, int keys = 0, int attachment = 0);
  void OnDragRight(bool draw, double x, double y, int keys = 0, int attachment = 0);
  void OnEndDragRight(double x, double y, int keys = 0, int attachment = 0);
  void OnEndSize(double x, double y);
};

/*
 * A diagram document, which contains a diagram.
 */
 
class DiagramDocument: public wxDocument
{
  DECLARE_DYNAMIC_CLASS(DiagramDocument)
 private:
 public:
  MyDiagram diagram;
  
  DiagramDocument(void);
  ~DiagramDocument(void);

#if wxUSE_STD_IOSTREAM
    virtual ostream& SaveObject(ostream& stream);
    virtual istream& LoadObject(istream& stream);
#else
    virtual wxOutputStream& SaveObject(wxOutputStream& stream);
    virtual wxInputStream& LoadObject(wxInputStream& stream);
#endif
  
  inline wxDiagram *GetDiagram() { return &diagram; }
  
  bool OnCloseDocument(void);
};

/*
 * Most user interface commands are routed through this, to give us the
 * Undo/Redo mechanism. If you add more commands, such as changing the shape colour,
 * you will need to add members to 'remember' what the user applied (for 'Do') and what the
 * previous state was (for 'Undo').
 * You can have one member for each property to be changed. Assume we also have
 * a pointer member wxShape *shape, which is set to the shape being changed.
 * Let's assume we're changing the shape colour. Our member for this is shapeColour.
 *
 * - In 'Do':
 *   o Set a temporary variable 'temp' to the current colour for 'shape'.
 *   o Change the colour to the new colour.
 *   o Set shapeColour to the _old_ colour, 'temp'.
 * - In 'Undo':
 *   o Set a temporary variable 'temp' to the current colour for 'shape'.
 *   o Change the colour to shapeColour (the old colour).
 *   o Set shapeColour to 'temp'.
 *
 * So, as long as we have a pointer to the shape being changed,
 * we only need one member variable for each property.
 *
 * PROBLEM: when an Add shape command is redone, the 'shape' pointer changes.
 * Assume, as here, that we keep a pointer to the old shape so we reuse it
 * when we recreate.
 */
 
class DiagramCommand: public wxCommand
{
 protected:
  DiagramDocument *doc;
  int cmd;
  wxShape *shape; // Pointer to the shape we're acting on
  wxShape *fromShape;
  wxShape *toShape;
  wxClassInfo *shapeInfo;
  double x;
  double y;
  bool selected;
  bool deleteShape;

  // Storage for property commands
  wxBrush *shapeBrush;
  wxPen *shapePen;
  wxString shapeLabel;
 public:
  // Multi-purpose constructor for creating, deleting shapes
  DiagramCommand(char *name, int cmd, DiagramDocument *ddoc, wxClassInfo *shapeInfo = NULL,
     double x = 0.0, double y = 0.0, bool sel = FALSE, wxShape *theShape = NULL, wxShape *fs = NULL, wxShape *ts = NULL);

  // Property-changing command constructors
  DiagramCommand(char *name, int cmd, DiagramDocument *ddoc, wxBrush *backgroundColour, wxShape *theShape);
  DiagramCommand(char *name, int cmd, DiagramDocument *ddoc, const wxString& lab, wxShape *theShape);

  ~DiagramCommand(void);

  bool Do(void);
  bool Undo(void);

  inline void SetShape(wxShape *s) { shape = s; }
  inline wxShape *GetShape(void) { return shape; }
  inline wxShape *GetFromShape(void) { return fromShape; }
  inline wxShape *GetToShape(void) { return toShape; }
  inline wxClassInfo *GetShapeInfo(void) { return shapeInfo; }
  inline bool GetSelected(void) { return selected; }

  void RemoveLines(wxShape *shape);
};

#endif
  // _OGLSAMPLE_DOC_H_
