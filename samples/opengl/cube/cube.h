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

// the rendering context used by all GL canvases
class TestGLContext : public wxGLContext
{
public:
    TestGLContext(wxGLCanvas *canvas);

    // render the cube showing it at given angles
    void DrawRotatedCube(float xangle, float yangle);

private:
    // textures for the cube faces
    GLuint m_textures[6];
};

// Define a new application type
class MyApp: public wxApp
{
public:
    MyApp() { m_glContext = NULL; }

    // get the context we use creating it on demand (and set it as current)
    TestGLContext& GetContext(wxGLCanvas *canvas);

    // virtual wxApp methods
    virtual bool OnInit();
    virtual int OnExit();

private:
    // the GL context we use for all our windows
    TestGLContext *m_glContext;
};

// Define a new frame type
class MyFrame: public wxFrame
{
public:
    MyFrame();

private:
    void OnClose(wxCommandEvent& event);
    void OnNewWindow(wxCommandEvent& event);
    void OnDefRotateLeftKey(wxCommandEvent& event);
    void OnDefRotateRightKey(wxCommandEvent& event);

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

    // angles of rotation around x- and y- axis
    float m_xangle,
          m_yangle;

    DECLARE_EVENT_TABLE()
};

#endif // _WX_CUBE_H_

