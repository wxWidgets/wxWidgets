/////////////////////////////////////////////////////////////////////////////
// Name:        canvas.h
// Purpose:     wxShapeCanvas
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _OGL_CANVAS_H_
#define _OGL_CANVAS_H_


// Drag states
#define NoDragging             0
#define StartDraggingLeft      1
#define ContinueDraggingLeft   2
#define StartDraggingRight     3
#define ContinueDraggingRight  4

WXDLLIMPEXP_OGL extern const wxChar* wxShapeCanvasNameStr;

// When drag_count reaches 0, process drag message

class WXDLLIMPEXP_OGL wxDiagram;

class WXDLLIMPEXP_OGL wxShapeCanvas: public wxScrolledWindow
{
 DECLARE_DYNAMIC_CLASS(wxShapeCanvas)
 public:
  wxShapeCanvas(wxWindow *parent = NULL, wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxBORDER | wxRETAINED,
                const wxString& name = wxShapeCanvasNameStr);
  ~wxShapeCanvas();

  inline void SetDiagram(wxDiagram *diag) { m_shapeDiagram = diag; }
  inline wxDiagram *GetDiagram() const { return m_shapeDiagram; }

  virtual void OnLeftClick(double x, double y, int keys = 0);
  virtual void OnRightClick(double x, double y, int keys = 0);

  virtual void OnDragLeft(bool draw, double x, double y, int keys=0); // Erase if draw false
  virtual void OnBeginDragLeft(double x, double y, int keys=0);
  virtual void OnEndDragLeft(double x, double y, int keys=0);

  virtual void OnDragRight(bool draw, double x, double y, int keys=0); // Erase if draw false
  virtual void OnBeginDragRight(double x, double y, int keys=0);
  virtual void OnEndDragRight(double x, double y, int keys=0);

  // Find object for mouse click, of given wxClassInfo (NULL for any type).
  // If notImage is non-NULL, don't find an object that is equal to or a descendant of notImage
  virtual wxShape *FindShape(double x, double y, int *attachment, wxClassInfo *info = NULL, wxShape *notImage = NULL);
  wxShape *FindFirstSensitiveShape(double x, double y, int *new_attachment, int op);
  wxShape *FindFirstSensitiveShape1(wxShape *image, int op);

  // Redirect to wxDiagram object
  virtual void AddShape(wxShape *object, wxShape *addAfter = NULL);
  virtual void InsertShape(wxShape *object);
  virtual void RemoveShape(wxShape *object);
  virtual bool GetQuickEditMode();
  virtual void Redraw(wxDC& dc);
  void Snap(double *x, double *y);

  // Events
  void OnPaint(wxPaintEvent& event);
  void OnMouseEvent(wxMouseEvent& event);

 protected:
  wxDiagram*        m_shapeDiagram;
  int               m_dragState;
  double             m_oldDragX, m_oldDragY;     // Previous drag coordinates
  double             m_firstDragX, m_firstDragY; // INITIAL drag coordinates
  bool              m_checkTolerance;           // Whether to check drag tolerance
  wxShape*          m_draggedShape;
  int               m_draggedAttachment;

DECLARE_EVENT_TABLE()
};

#endif
 // _OGL_CANVAS_H_
