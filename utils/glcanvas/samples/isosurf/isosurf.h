/////////////////////////////////////////////////////////////////////////////
// Name:        isosurf.h
// Purpose:     wxGLCanvas demo program
// Author:      Brian Paul (original gltk version), Wolfram Gloger
// Modified by: Julian Smart
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_ISOSURF_H_
#define _WX_ISOSURF_H_

// Define a new application type
class MyApp: public wxApp
{ public:
    bool OnInit(void);
};

class TestGLCanvas: public wxGLCanvas
{
 public:
   TestGLCanvas(wxWindow *parent, const wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = "TestGLCanvas",
      int* gl_attrib = NULL);
   ~TestGLCanvas(void);

   void OnPaint(wxPaintEvent& event);
   void OnSize(wxSizeEvent& event);
   void OnEraseBackground(wxEraseEvent& event);
   void OnChar(wxKeyEvent& event);
   void OnMouseEvent(wxMouseEvent& event);

DECLARE_EVENT_TABLE()
};

class MyFrame: public wxFrame
{
public:
    MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& size,
        long style = wxDEFAULT_FRAME_STYLE);

    void OnExit(wxCommandEvent& event);
public:
    TestGLCanvas*    m_canvas;

DECLARE_EVENT_TABLE()
};

#endif

