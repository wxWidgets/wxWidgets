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

#ifndef _OGLSAMPLE_VIEW_H_
#define _OGLSAMPLE_VIEW_H_

#ifdef __GNUG__
// #pragma interface "view.h"
#endif

#include "doc.h"
#include <wx/ogl/ogl.h>

class MyCanvas: public wxShapeCanvas
{
// DECLARE_DYNAMIC_CLASS(wxShapeCanvas)
 protected:
 public:
  wxView *view;

  MyCanvas(wxView *view, wxWindow *parent = NULL, wxWindowID id = -1,
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
            long style = wxRETAINED);
  ~MyCanvas(void);

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

DECLARE_EVENT_TABLE()
};

class DiagramView: public wxView
{
  DECLARE_DYNAMIC_CLASS(DiagramView)
 private:
 public:
  wxFrame *frame;
  MyCanvas *canvas;
  
  DiagramView(void) { canvas = NULL; frame = NULL; };
  ~DiagramView(void) {};

  bool OnCreate(wxDocument *doc, long flags);
  void OnDraw(wxDC *dc);
  void OnUpdate(wxView *sender, wxObject *hint = NULL);
  bool OnClose(bool deleteWindow = TRUE);

  wxShape *FindSelectedShape(void);

//  void OnMenuCommand(int cmd);

  void OnCut(wxCommandEvent& event);
  void OnChangeBackgroundColour(wxCommandEvent& event);
  void OnEditLabel(wxCommandEvent& event);

DECLARE_EVENT_TABLE()
};

#endif
    // _OGLSAMPLE_VIEW_H_
