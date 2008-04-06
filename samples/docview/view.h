/////////////////////////////////////////////////////////////////////////////
// Name:        view.h
// Purpose:     View classes
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __VIEW_H__
#define __VIEW_H__

#include "wx/docview.h"

class DrawingView;
class MyCanvas : public wxScrolledWindow
{
public:
    DrawingView* m_view;

    MyCanvas(DrawingView*, wxFrame*, const wxPoint& pos, const wxSize& size, const long style);
    virtual void OnDraw(wxDC& dc);

protected:
    void OnMouseEvent(wxMouseEvent& event);
    DECLARE_EVENT_TABLE()
};

class MyTextWindow: public wxTextCtrl
{
public:
    wxView* m_view;

    MyTextWindow(wxView *v, wxFrame *frame, const wxPoint& pos, const wxSize& size, const long style);
};

class DrawingView : public wxView
{
public:
    wxFrame*  m_frame;
    MyCanvas* m_canvas;

    DrawingView() { m_canvas = NULL; m_frame = NULL; };
    virtual ~DrawingView() {};

    virtual bool OnCreate(wxDocument *doc, long flags);
    virtual void OnDraw(wxDC *dc);
    virtual void OnUpdate(wxView *sender, wxObject *hint = NULL);
    virtual bool OnClose(bool deleteWindow = true);

    DrawingDocument* GetDocument();

protected:
    void OnCut(wxCommandEvent& event);

private:
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(DrawingView)
};

class TextEditView : public wxView
{
public:
    wxFrame*      m_frame;
    MyTextWindow* m_textsw;

    TextEditView(): wxView() { m_frame = NULL; m_textsw = NULL; }
    virtual ~TextEditView() {}

    virtual bool OnCreate(wxDocument *doc, long flags);
    virtual void OnDraw(wxDC *dc);
    virtual void OnUpdate(wxView *sender, wxObject *hint = NULL);
    virtual bool OnClose(bool deleteWindow = true);
    virtual bool ProcessEvent(wxEvent&);

private:
    DECLARE_DYNAMIC_CLASS(TextEditView)
};

#endif
