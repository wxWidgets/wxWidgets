/////////////////////////////////////////////////////////////////////////////
// Name:        cube.h
// Purpose:     wxGLCanvas demo program
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CUBE_H_
#define _WX_CUBE_H_

#include <wx/glcanvas.h>

// Define a new application type
class MyApp: public wxApp
{
public:
    bool OnInit(void);
};

// Define a new frame type
class TestGLCanvas;
class MyFrame: public wxFrame
{
public:
    MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
            const wxSize& size, long style = wxDEFAULT_FRAME_STYLE);

    void OnExit(wxCommandEvent& event);
    void OnNewWindow();
    void OnDefRotateLeftKey();
    void OnDefRotateRightKey();
    
public:
    TestGLCanvas*    m_canvas;

DECLARE_EVENT_TABLE()
};

class TestGLCanvas: public wxGLCanvas
{
  friend class MyFrame;
public:
 TestGLCanvas(wxWindow *parent, const wxWindowID id = -1, 
    const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
    long style = 0, const wxString& name = "TestGLCanvas");
 TestGLCanvas(wxWindow *parent, const TestGLCanvas &other,
          const wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition,
          const wxSize& size = wxDefaultSize, long style = 0,
          const wxString& name = "TestGLCanvas" );
   
 ~TestGLCanvas(void);

 void OnPaint(wxPaintEvent& event);
 void OnSize(wxSizeEvent& event);
 void OnEraseBackground(wxEraseEvent& event);
 void OnKeyDown(wxKeyEvent& event);
 void OnKeyUp(wxKeyEvent& event);
 void OnEnterWindow( wxMouseEvent& event );
 
 void Render( void );
 void InitGL(void);
 void Rotate( GLfloat deg );
 static GLfloat CalcRotateSpeed( unsigned long acceltime );
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

#endif

