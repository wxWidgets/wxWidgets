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
    bool OnInit();
};

// Define a new frame type
class TestGLCanvas;

class MyFrame: public wxFrame
{
public:
    static MyFrame *Create(MyFrame *parentFrame, bool isCloneWindow = false);

    void OnExit(wxCommandEvent& event);
    void OnNewWindow(wxCommandEvent& event);
    void OnDefRotateLeftKey(wxCommandEvent& event);
    void OnDefRotateRightKey(wxCommandEvent& event);

private:

    MyFrame(wxWindow *parent, const wxString& title, const wxPoint& pos,
            const wxSize& size, long style = wxDEFAULT_FRAME_STYLE);


    TestGLCanvas *m_canvas;

    DECLARE_EVENT_TABLE()
};

#if wxUSE_GLCANVAS

class TestGLCanvas: public wxGLCanvas
{
    friend class MyFrame;
public:
    TestGLCanvas( wxWindow *parent, wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0, const wxString& name = _T("TestGLCanvas") );

    TestGLCanvas( wxWindow *parent, const TestGLCanvas *other,
        wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0,
        const wxString& name = _T("TestGLCanvas") );

    ~TestGLCanvas();

    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);
    void OnEnterWindow(wxMouseEvent& event);

    void Render();
    void InitGL();
    void Rotate(GLfloat deg);
    static GLfloat CalcRotateSpeed(unsigned long acceltime);
    static GLfloat CalcRotateAngle( unsigned long lasttime,
        unsigned long acceltime );
    void Action( long code, unsigned long lasttime,
        unsigned long acceltime );

private:
    bool   m_init;
    GLuint m_gllist;
    long   m_rleft;
    long   m_rright;

    static unsigned long  m_secbase;
    static int            m_TimeInitialized;
    static unsigned long  m_xsynct;
    static unsigned long  m_gsynct;

    long           m_Key;
    unsigned long  m_StartTime;
    unsigned long  m_LastTime;
    unsigned long  m_LastRedraw;

DECLARE_EVENT_TABLE()
};

#endif // #if wxUSE_GLCANVAS

#endif // #ifndef _WX_CUBE_H_

