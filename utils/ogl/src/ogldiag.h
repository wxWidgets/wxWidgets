/////////////////////////////////////////////////////////////////////////////
// Name:        ogldiag.h
// Purpose:     OGL - wxDiagram class
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _OGL_OGLDIAG_H_
#define _OGL_OGLDIAG_H_

#ifdef __GNUG__
#pragma interface "ogldiag.h"
#endif

#include "basic.h"

class wxDiagram: public wxObject
{
 DECLARE_DYNAMIC_CLASS(wxDiagram)

public:

  wxDiagram();
  virtual ~wxDiagram();

  void SetCanvas(wxShapeCanvas *can);

  inline wxShapeCanvas *GetCanvas() const { return m_diagramCanvas; }

  virtual void Redraw(wxDC& dc);
  virtual void Clear(wxDC& dc);
  virtual void DrawOutline(wxDC& dc, float x1, float y1, float x2, float y2);

  // Add object to end of object list (if addAfter is NULL)
  // or just after addAfter.
  virtual void AddShape(wxShape *object, wxShape *addAfter = NULL);

  // Add object to front of object list
  virtual void InsertShape(wxShape *object);

  void SetSnapToGrid(bool snap);
  void SetGridSpacing(float spacing);
  inline float GetGridSpacing() { return m_gridSpacing; }
  inline bool GetSnapToGrid() const { return m_snapToGrid; }
  void Snap(float *x, float *y);

  inline void SetQuickEditMode(bool qem) { m_quickEditMode = qem; }
  inline bool GetQuickEditMode() const { return m_quickEditMode; }

  virtual void RemoveShape(wxShape *object);
  virtual void RemoveAllShapes();
  virtual void DeleteAllShapes();
  virtual void ShowAll(bool show);

  inline void SetMouseTolerance(int tol) { m_mouseTolerance = tol; }
  inline int GetMouseTolerance() const { return m_mouseTolerance; }
  inline wxList *GetShapeList() const { return m_shapeList; }

  // Make sure all text that should be centred, is centred.
  void RecentreAll(wxDC& dc);

#ifdef PROLOGIO
  // Prolog database stuff
  virtual bool SaveFile(const wxString& filename);
  virtual bool LoadFile(const wxString& filename);

  virtual void ReadNodes(wxExprDatabase& database);
  virtual void ReadLines(wxExprDatabase& database);
  virtual void ReadContainerGeometry(wxExprDatabase& database);

  // Allow for modifying file
  virtual bool OnDatabaseLoad(wxExprDatabase& db);
  virtual bool OnDatabaseSave(wxExprDatabase& db);
  virtual bool OnShapeSave(wxExprDatabase& db, wxShape& shape, wxExpr& expr);
  virtual bool OnShapeLoad(wxExprDatabase& db, wxShape& shape, wxExpr& expr);
  virtual bool OnHeaderSave(wxExprDatabase& db, wxExpr& expr);
  virtual bool OnHeaderLoad(wxExprDatabase& db, wxExpr& expr);
#endif

protected:
  wxShapeCanvas*        m_diagramCanvas;
  bool                  m_quickEditMode;
  bool                  m_snapToGrid;
  float                 m_gridSpacing;
  int                   m_mouseTolerance;
  wxList*               m_shapeList;
};

#endif
 // _OGL_OGLDIAG_H_
