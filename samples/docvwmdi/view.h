/////////////////////////////////////////////////////////////////////////////
// Name:        view.h
// Purpose:     View classes
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __VIEWSAMPLEH__
#define __VIEWSAMPLEH__

#include "wx/docview.h"

class MyCanvas: public wxScrolledWindow
{
public:
    wxView *view;
    
    MyCanvas(wxView *v, wxFrame *frame, const wxPoint& pos, const wxSize& size, long style);
    virtual void OnDraw(wxDC& dc);
    void OnMouseEvent(wxMouseEvent& event);

private:
    DECLARE_EVENT_TABLE()
};

class MyTextWindow: public wxTextCtrl
{
public:
    wxView *view;
    
    MyTextWindow(wxView *v, wxFrame *frame, const wxPoint& pos, const wxSize& size, long style);
};

class DrawingView: public wxView
{
public:
    wxFrame *frame;
    MyCanvas *canvas;
  
    DrawingView() { canvas = (MyCanvas *) NULL; frame = (wxFrame *) NULL; }
    ~DrawingView() {}

    bool OnCreate(wxDocument *doc, long flags);
    void OnDraw(wxDC *dc);
    void OnUpdate(wxView *sender, wxObject *hint = (wxObject *) NULL);
    bool OnClose(bool deleteWindow = TRUE);

    void OnCut(wxCommandEvent& event);

private:
    DECLARE_DYNAMIC_CLASS(DrawingView)
    DECLARE_EVENT_TABLE()
};

class TextEditView: public wxView
{
public:
    wxFrame *frame;
    MyTextWindow *textsw;
  
    TextEditView(): wxView() { frame = (wxFrame *) NULL; textsw = (MyTextWindow *) NULL; }
    ~TextEditView() {}

    bool OnCreate(wxDocument *doc, long flags);
    void OnDraw(wxDC *dc);
    void OnUpdate(wxView *sender, wxObject *hint = (wxObject *) NULL);
    bool OnClose(bool deleteWindow = TRUE);
    
private:
  DECLARE_DYNAMIC_CLASS(TextEditView)
};

#endif
