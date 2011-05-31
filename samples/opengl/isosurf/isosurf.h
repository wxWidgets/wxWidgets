/////////////////////////////////////////////////////////////////////////////
// Name:        isosurf.h
// Purpose:     wxGLCanvas demo program
// Author:      Brian Paul (original gltk version), Wolfram Gloger
// Modified by: Julian Smart
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_ISOSURF_H_
#define _WX_ISOSURF_H_

// we need OpenGL headers for GLfloat/GLint types used below
#if defined(__WXMAC__) || defined(__WXCOCOA__)
#   ifdef __DARWIN__
#       include <OpenGL/gl.h>
#       include <OpenGL/glu.h>
#   else
#       include <gl.h>
#       include <glu.h>
#   endif
#else
#   include <GL/gl.h>
#   include <GL/glu.h>
#endif

// the maximum number of vertex in the loaded .dat file
#define MAXVERTS     10000


// Define a new application type
class MyApp : public wxApp
{
public:
    virtual bool OnInit();

    virtual void OnInitCmdLine(wxCmdLineParser& parser);
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
};


// The OpenGL-enabled canvas
class TestGLCanvas : public wxGLCanvas
{
public:
    TestGLCanvas(wxWindow *parent,
                 wxWindowID id = wxID_ANY,
                 int *gl_attrib = NULL);

    virtual ~TestGLCanvas();

    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnChar(wxKeyEvent& event);
    void OnMouseEvent(wxMouseEvent& event);

    void LoadSurface(const wxString& filename);
    void InitMaterials();
    void InitGL();

private:
    wxGLContext* m_glRC;

    GLfloat m_verts[MAXVERTS][3];
    GLfloat m_norms[MAXVERTS][3];
    GLint m_numverts;

    GLfloat m_xrot;
    GLfloat m_yrot;

    wxDECLARE_NO_COPY_CLASS(TestGLCanvas);
    DECLARE_EVENT_TABLE()
};


// The frame containing the GL canvas
class MyFrame : public wxFrame
{
public:
    MyFrame(wxFrame *frame,
            const wxString& title,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxDEFAULT_FRAME_STYLE);

    virtual ~MyFrame();

    TestGLCanvas *m_canvas;

private :
    void OnExit(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};


#endif // _WX_ISOSURF_H_

