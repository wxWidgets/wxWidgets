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

#include "glcanvas.h"

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
    MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& size,
        long style = wxDEFAULT_FRAME_STYLE);

    void OnExit(wxCommandEvent& event);
    bool OnClose(void);
public:
    TestGLCanvas*    m_canvas;

DECLARE_EVENT_TABLE()
};

class TestGLCanvas: public wxGLCanvas
{
 public:
   TestGLCanvas(wxWindow *parent, const wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = "TestGLCanvas");
   ~TestGLCanvas(void);

   void OnPaint(wxPaintEvent& event);
   void OnSize(wxSizeEvent& event);
   void OnEraseBackground(wxEraseEvent& event);
   void InitGL(void);
   
 private:
 
   bool m_init;

DECLARE_EVENT_TABLE()
};

#endif

