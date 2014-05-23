/////////////////////////////////////////////////////////////////////////////
// Name:        samples/docview/view.h
// Purpose:     View classes
// Author:      Julian Smart
// Modified by: Vadim Zeitlin: merge with the MDI version and general cleanup
// Created:     04/01/98
// Copyright:   (c) 1998 Julian Smart
//              (c) 2008 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SAMPLES_DOCVIEW_VIEW_H_
#define _WX_SAMPLES_DOCVIEW_VIEW_H_

#include "wx/docview.h"

// ----------------------------------------------------------------------------
// Drawing view classes
// ----------------------------------------------------------------------------

// The window showing the drawing itself
class MyCanvas : public wxScrolledWindow
{
public:
    // view may be NULL if we're not associated with one yet, but parent must
    // be a valid pointer
    MyCanvas(wxView *view, wxWindow *parent = NULL);
    virtual ~MyCanvas();

    virtual void OnDraw(wxDC& dc) wxOVERRIDE;

    // in a normal multiple document application a canvas is associated with
    // one view from the beginning until the end, but to support the single
    // document mode in which all documents reuse the same MyApp::GetCanvas()
    // we need to allow switching the canvas from one view to another one

    void SetView(wxView *view)
    {
        wxASSERT_MSG( !m_view, "shouldn't be already associated with a view" );

        m_view = view;
    }

    void ResetView()
    {
        wxASSERT_MSG( m_view, "should be associated with a view" );

        m_view = NULL;
    }

private:
    void OnMouseEvent(wxMouseEvent& event);

    wxView *m_view;

    // the segment being currently drawn or NULL if none
    DoodleSegment *m_currentSegment;

    // the last mouse press position
    wxPoint m_lastMousePos;

    wxDECLARE_EVENT_TABLE();
};

// The view using MyCanvas to show its contents
class DrawingView : public wxView
{
public:
    DrawingView() : wxView(), m_canvas(NULL) {}

    virtual bool OnCreate(wxDocument *doc, long flags) wxOVERRIDE;
    virtual void OnDraw(wxDC *dc) wxOVERRIDE;
    virtual void OnUpdate(wxView *sender, wxObject *hint = NULL) wxOVERRIDE;
    virtual bool OnClose(bool deleteWindow = true) wxOVERRIDE;

    DrawingDocument* GetDocument();

private:
    void OnCut(wxCommandEvent& event);

    MyCanvas *m_canvas;

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_DYNAMIC_CLASS(DrawingView);
};

// ----------------------------------------------------------------------------
// Text view classes
// ----------------------------------------------------------------------------

// The view using a standard wxTextCtrl to show its contents
class TextEditView : public wxView
{
public:
    TextEditView() : wxView(), m_text(NULL) {}

    virtual bool OnCreate(wxDocument *doc, long flags) wxOVERRIDE;
    virtual void OnDraw(wxDC *dc) wxOVERRIDE;
    virtual bool OnClose(bool deleteWindow = true) wxOVERRIDE;

    wxTextCtrl *GetText() const { return m_text; }

private:
    void OnCopy(wxCommandEvent& WXUNUSED(event)) { m_text->Copy(); }
    void OnPaste(wxCommandEvent& WXUNUSED(event)) { m_text->Paste(); }
    void OnSelectAll(wxCommandEvent& WXUNUSED(event)) { m_text->SelectAll(); }

    wxTextCtrl *m_text;

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_DYNAMIC_CLASS(TextEditView);
};

// ----------------------------------------------------------------------------
// ImageCanvas
// ----------------------------------------------------------------------------

class ImageCanvas : public wxScrolledWindow
{
public:
    ImageCanvas(wxView*);

    virtual void OnDraw(wxDC& dc) wxOVERRIDE;
private:
    wxView *m_view;
};

// ----------------------------------------------------------------------------
// ImageView
// ----------------------------------------------------------------------------

class ImageView : public wxView
{
public:
    ImageView() : wxView() {}

    virtual bool OnCreate(wxDocument*, long flags) wxOVERRIDE;
    virtual void OnDraw(wxDC*) wxOVERRIDE;
    virtual bool OnClose(bool deleteWindow = true) wxOVERRIDE;
    virtual void OnUpdate(wxView *sender, wxObject *hint = NULL) wxOVERRIDE;

    ImageDocument* GetDocument();

private:
    ImageCanvas* m_canvas;

    wxDECLARE_DYNAMIC_CLASS(ImageView);
};

// ----------------------------------------------------------------------------
// ImageDetailsView
// ----------------------------------------------------------------------------

class ImageDetailsView : public wxView
{
public:
    ImageDetailsView(ImageDetailsDocument *doc);

    virtual void OnDraw(wxDC *dc) wxOVERRIDE;
    virtual bool OnClose(bool deleteWindow) wxOVERRIDE;

private:
    wxFrame *m_frame;

    wxDECLARE_NO_COPY_CLASS(ImageDetailsView);
};

#endif // _WX_SAMPLES_DOCVIEW_VIEW_H_
