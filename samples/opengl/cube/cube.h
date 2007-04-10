/////////////////////////////////////////////////////////////////////////////
// Name:        cube.h
// Purpose:     wxGLCanvas demo program
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CUBE_H_
#define _WX_CUBE_H_

#include "wx/glcanvas.h"

// Define a new application type
class MyApp: public wxApp
{
public:
    MyApp() { m_glContext = NULL; }

    // set the specified canvas for current output
    void SetCurrent(wxGLCanvas *canvas);

    // virtual wxApp methods
    virtual bool OnInit();
    virtual int OnExit();

private:
    // the GL context we use for all our windows
    wxGLContext *m_glContext;
};

// Define a new frame type
class TestGLCanvas;

class MyFrame: public wxFrame
{
public:
    MyFrame();

    // update the image shown on the canvas (after the shared wxGLContext was
    // updated, presumably)
    void RefreshCanvas();

private:
    void OnClose(wxCommandEvent& event);
    void OnNewWindow(wxCommandEvent& event);
    void OnDefRotateLeftKey(wxCommandEvent& event);
    void OnDefRotateRightKey(wxCommandEvent& event);

    TestGLCanvas *m_canvas;

    DECLARE_EVENT_TABLE()
};

class TestGLCanvas : public wxGLCanvas
{
public:
    TestGLCanvas(wxWindow *parent);

private:
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnKeyDown(wxKeyEvent& event);

    // OpenGL calls can't be done until we're initialized
    bool IsInitialized() const { return m_gllist != 0; }

    // one-time OpenGL initialization, only does something if !IsInitialized()
    void InitGL();

    // render to window
    void Render();


    // the list of commands to draw the cube
    GLuint m_gllist;

    DECLARE_EVENT_TABLE()
};

#endif // _WX_CUBE_H_

