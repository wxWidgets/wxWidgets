/////////////////////////////////////////////////////////////////////////////
// Name:        view.h
// Purpose:     View-related classes
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _STUDIO_VIEW_H_
#define _STUDIO_VIEW_H_

#ifdef __GNUG__
// #pragma interface "view.h"
#endif

#include "doc.h"
#include <wx/ogl/ogl.h>

class csDiagramView;
class csCanvas: public wxShapeCanvas
{
DECLARE_CLASS(csCanvas)
 public:

  csCanvas(csDiagramView *view, wxWindow *parent = NULL, wxWindowID id = -1,
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
            long style = wxRETAINED);
  ~csCanvas(void);

  void DrawOutline(wxDC& dc, double x1, double y1, double x2, double y2);

  void OnMouseEvent(wxMouseEvent& event);
  void OnPaint(wxPaintEvent& event);

  virtual void OnLeftClick(double x, double y, int keys = 0);
  virtual void OnRightClick(double x, double y, int keys = 0);

  virtual void OnDragLeft(bool draw, double x, double y, int keys=0); // Erase if draw false
  virtual void OnBeginDragLeft(double x, double y, int keys=0);
  virtual void OnEndDragLeft(double x, double y, int keys=0);

  virtual void OnDragRight(bool draw, double x, double y, int keys=0); // Erase if draw false
  virtual void OnBeginDragRight(double x, double y, int keys=0);
  virtual void OnEndDragRight(double x, double y, int keys=0);

  inline csDiagramView*  GetView() const { return m_view; }
  inline void SetView(csDiagramView* view) { m_view = view; }

 protected:
  csDiagramView*    m_view;

DECLARE_EVENT_TABLE()
};

class csDiagramView: public wxView
{
  DECLARE_DYNAMIC_CLASS(csDiagramView)
 public:
  csDiagramView(void) { canvas = NULL; frame = NULL; };
  ~csDiagramView(void);

  bool OnCreate(wxDocument *doc, long flags);
  void OnDraw(wxDC *dc);
  void OnUpdate(wxView *sender, wxObject *hint = NULL);
  bool OnClose(bool deleteWindow = TRUE);
  void OnSelectAll(wxCommandEvent& event);

  wxShape *FindFirstSelectedShape(void);

  // Scans the canvas for selections (doesn't use m_selections)
  void FindSelectedShapes(wxList& selections, wxClassInfo* toFind = NULL);

  // The selections in the order in which they were selected
  inline wxList& GetSelectionList() const { return (wxList&) m_selections; }

  // Adds or removes shape from m_selections
  void SelectShape(wxShape* shape, bool select);

  // Apply point size to current shapes
  void ApplyPointSize(int pointSize);

  // Make the point size combobox reflect this
  void ReflectPointSize(int pointSize);

  // Make the arrow toggle button reflect the state of the line
  void ReflectArrowState(wxLineShape* lineShape);

  // Do a cut operation for the given list of shapes
  void DoCut(wxList& shapes);

  // Do a general command
  void DoCmd(wxList& shapes, wxList& oldShapes, int cmd, const wxString& op);

  // Select or deselect all
  void SelectAll(bool select = TRUE);

// Event handlers
  void OnCut(wxCommandEvent& event);
  void OnCopy(wxCommandEvent& event);
  void OnPaste(wxCommandEvent& event);
  void OnDuplicate(wxCommandEvent& event);
  void OnClear(wxCommandEvent& event);
  void OnChangeBackgroundColour(wxCommandEvent& event);
  void OnEditProperties(wxCommandEvent& event);
  void OnPointSizeComboSel(wxCommandEvent& event);
  void OnPointSizeComboText(wxCommandEvent& event);
  void OnToggleArrowTool(wxCommandEvent& event);
  void OnZoomSel(wxCommandEvent& event);
  void OnAlign(wxCommandEvent& event);
  void OnNewLinePoint(wxCommandEvent& event);
  void OnCutLinePoint(wxCommandEvent& event);
  void OnStraightenLines(wxCommandEvent& event);

// UI update handles
  void OnToggleArrowToolUpdate(wxUpdateUIEvent& event);
  void OnEditPropertiesUpdate(wxUpdateUIEvent& event);
  void OnCutUpdate(wxUpdateUIEvent& event);
  void OnClearUpdate(wxUpdateUIEvent& event);
  void OnCopyUpdate(wxUpdateUIEvent& event);
  void OnPasteUpdate(wxUpdateUIEvent& event);
  void OnDuplicateUpdate(wxUpdateUIEvent& event);
  void OnAlignUpdate(wxUpdateUIEvent& event);
  void OnNewLinePointUpdate(wxUpdateUIEvent& event);
  void OnCutLinePointUpdate(wxUpdateUIEvent& event);
  void OnStraightenLinesUpdate(wxUpdateUIEvent& event);
  void OnUndoUpdate(wxUpdateUIEvent& event);
  void OnRedoUpdate(wxUpdateUIEvent& event);

DECLARE_EVENT_TABLE()

public:
  wxFrame*      frame;
  csCanvas*     canvas;
  wxList        m_selections;
};

#endif
    // _STUDIO_VIEW_H_
