/////////////////////////////////////////////////////////////////////////////
// Name:        shapes.h
// Purpose:     Shape classes
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:
/////////////////////////////////////////////////////////////////////////////

#ifndef _STUDIO_SHAPES_H_
#define _STUDIO_SHAPES_H_

#ifdef __GNUG__
// #pragma interface
#endif

#include <wx/docview.h>
#include <wx/string.h>
#include <wx/wxexpr.h>

#include "ogl.h"
#include "basicp.h"
#include "linesp.h"
#include "drawn.h"

class csDiagramDocument;

/*
 * Override a few members for this application
 */
 
class csDiagram: public wxDiagram
{
DECLARE_CLASS(csDiagram)
public:
    csDiagram(csDiagramDocument* doc) { m_doc = doc; }
    ~csDiagram();
    bool OnShapeSave(wxExprDatabase& db, wxShape& shape, wxExpr& expr);
    bool OnShapeLoad(wxExprDatabase& db, wxShape& shape, wxExpr& expr);

    inline csDiagramDocument* GetDocument() const { return m_doc; }
    virtual void Redraw(wxDC& dc);

protected:
    csDiagramDocument* m_doc;
};

class wxDiagramClipboard: public wxDiagram
{
DECLARE_DYNAMIC_CLASS(wxDiagramClipboard)
public:
    wxDiagramClipboard() {}
    ~wxDiagramClipboard() {}

    // Copy selection to clipboard
    bool Copy(wxDiagram* diagram);

    // Copy contents to the diagram, with new ids.
    // If dc is non-NULL, the pasted shapes will be selected.
    // The offsets are used to place the shapes at a different position
    // from the original (for example, for duplicating shapes).
    bool Paste(wxDiagram* diagram, wxDC* dc = NULL,
        int offsetX = 0, int offsetY = 0);

#ifdef __WXMSW__
    // Draw contents to a Windows metafile device context and bitmap, and then copy
    // to the Windows clipboard.
    bool CopyToClipboard(double scale);
#endif

// Overridables
    // Start/end copying
    virtual bool OnStartCopy(wxDiagram* diagramTo) { return TRUE; };
    virtual bool OnEndCopy(wxDiagram* diagramTo) { return TRUE; };

    // Override this to e.g. have the shape added through a Do/Undo command system.
    // By default, we'll just add it directly to the destination diagram, and
    // select the shape (if dc is non-NULL).
    virtual bool OnAddShape(wxDiagram* diagramTo, wxShape* newShape, wxDC* dc);

protected:
    bool DoCopy(wxDiagram* diagramFrom, wxDiagram* diagramTo, bool newIds,
                    wxDC* dc, int offsetX = 0, int offsetY = 0);

};

class csDiagramCommand;

class csDiagramClipboard: public wxDiagramClipboard
{
DECLARE_DYNAMIC_CLASS(csDiagramClipboard)
public:
    csDiagramClipboard() { m_currentCmd = NULL; }
    ~csDiagramClipboard() {}

    // Start/end copying
    bool OnStartCopy(wxDiagram* diagramTo);
    bool OnEndCopy(wxDiagram* diagramTo);

    bool OnAddShape(wxDiagram* diagramTo, wxShape* newShape, wxDC* dc);

protected:
    csDiagramCommand*   m_currentCmd;
};


/*
 * The Studio shapes
 * N.B. TODO: these should really all have another constructor
 * for the ready-initialised shape, with the default one not having any
 * data. Otherwise when copying a shape, you have to delete the old data
 * first -> slightly less efficient. The initialised shapes are only required
 * for the first creation of the shape in the palette, everything else is copied.
 */

class csThinRectangleShape: public wxDrawnShape
{
DECLARE_DYNAMIC_CLASS(csThinRectangleShape)
public:
    csThinRectangleShape();
};

class csWideRectangleShape: public wxDrawnShape
{
DECLARE_DYNAMIC_CLASS(csWideRectangleShape)
public:
    csWideRectangleShape();
};

class csTriangleShape: public wxDrawnShape
{
DECLARE_DYNAMIC_CLASS(csTriangleShape)
public:
    csTriangleShape();
};

class csSemiCircleShape: public wxDrawnShape
{
DECLARE_DYNAMIC_CLASS(csSemiCircleShape)
public:
    csSemiCircleShape();
};

class csCircleShape: public wxCircleShape
{
DECLARE_DYNAMIC_CLASS(csCircleShape)
public:
    csCircleShape();
};

class csCircleShadowShape: public wxCircleShape
{
DECLARE_DYNAMIC_CLASS(csCircleShadowShape)
public:
    csCircleShadowShape();
};

class csOctagonShape: public wxPolygonShape
{
DECLARE_DYNAMIC_CLASS(csOctagonShape)
public:
    csOctagonShape();

    // The attachments are as if it's a rectangle
    bool GetAttachmentPosition(int attachment, double *x, double *y,
                                     int nth = 0, int no_arcs = 1, wxLineShape *line = NULL)
    { return wxShape::GetAttachmentPosition(attachment, x, y, nth, no_arcs, line); }
    int GetNumberOfAttachments() const
    { return wxShape::GetNumberOfAttachments(); }
    bool AttachmentIsValid(int attachment) const
    { return wxShape::AttachmentIsValid(attachment); }
};

// This is a transparent shape for drawing around other shapes.
class csGroupShape: public wxRectangleShape
{
DECLARE_DYNAMIC_CLASS(csGroupShape)
public:
    csGroupShape();

    void OnDraw(wxDC& dc);
    // Must modify the hit-test so it doesn't obscure shapes that are inside.
    bool HitTest(double x, double y, int* attachment, double* distance);
};

class csTextBoxShape: public wxRectangleShape
{
DECLARE_DYNAMIC_CLASS(csTextBoxShape)
public:
    csTextBoxShape();
};

class csLineShape: public wxLineShape
{
DECLARE_DYNAMIC_CLASS(csLineShape)
public:
    csLineShape();

    virtual bool OnMoveMiddleControlPoint(wxDC& dc, wxLineControlPoint* lpt, const wxRealPoint& pt);
    wxLabelShape* OnCreateLabelShape(wxLineShape *parent = NULL, wxShapeRegion *region = NULL, double w = 0.0, double h = 0.0);
};

/*
 * Temporary arc label object
 */
 
class csLabelShape: public wxLabelShape
{
  DECLARE_DYNAMIC_CLASS(csLabelShape)

 public:
  csLabelShape(wxLineShape *parent = NULL, wxShapeRegion *region = NULL, double w = 0.0, double h = 0.0);

  void OnEndDragLeft(double x, double y, int keys=0, int attachment = 0);
};

/*
 * All shape event behaviour is routed through this handler, so we don't
 * have to derive from each shape class. We plug this in to each shape.
 */

class csEvtHandler: public wxShapeEvtHandler
{
 DECLARE_DYNAMIC_CLASS(csEvtHandler)
 public:
  csEvtHandler(wxShapeEvtHandler *prev = NULL, wxShape *shape = NULL, const wxString& lab = "");
  ~csEvtHandler();

  void OnLeftClick(double x, double y, int keys = 0, int attachment = 0);
  void OnRightClick(double x, double y, int keys = 0, int attachment = 0);
  void OnBeginDragRight(double x, double y, int keys = 0, int attachment = 0);
  void OnDragRight(bool draw, double x, double y, int keys = 0, int attachment = 0);
  void OnEndDragRight(double x, double y, int keys = 0, int attachment = 0);
  void OnEndSize(double x, double y);
  void OnDragLeft(bool draw, double x, double y, int keys = 0, int attachment = 0);
  void OnBeginDragLeft(double x, double y, int keys = 0, int attachment = 0);
  void OnEndDragLeft(double x, double y, int keys = 0, int attachment = 0);
  void OnSizingEndDragLeft(wxControlPoint* pt, double x, double y, int keys = 0, int attachment = 0);
  void OnChangeAttachment(int attachment, wxLineShape* line, wxList& ordering);

  void OnLeftDoubleClick(double x, double y, int keys = 0, int attachment = 0);

  // Copy any event handler data
  virtual void CopyData(wxShapeEvtHandler& copy);

  // Popup up a property dialog
  virtual bool EditProperties();

public:
  wxString m_label;
};

class ShapeEditMenu: public wxMenu
{
public:
    ShapeEditMenu() {}

    void OnCommand(wxCommandEvent& event);

DECLARE_EVENT_TABLE()
};

extern void studioShapeEditProc(wxMenu& menu, wxCommandEvent& event);

#endif
  // _STUDIO_SHAPES_H_
