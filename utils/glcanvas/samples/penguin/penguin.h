/////////////////////////////////////////////////////////////////////////////
// Name:        penguin.h
// Purpose:     wxGLCanvas demo program
// Author:      Robert Roebling
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PENGUIN_H_
#define _WX_PENGUIN_H_


#include "wx/defs.h"
#include "wx/app.h"
#include "wx/menu.h"
#include "wx/dcclient.h"

#include "glcanvas.h"

extern "C" {
#include "lw.h"
#include "trackball.h"
}

/* information needed to display lightwave mesh */
typedef struct 
{
//  gint do_init;         /* true if initgl not yet called */
  int do_init;
  lwObject *lwobject;   /* lightwave object mesh */
  float beginx,beginy;  /* position of mouse */
  float quat[4];        /* orientation of object */
  float zoom;           /* field of view in degrees */
} mesh_info;


/* Define a new application type */
class MyApp: public wxApp
{
public:
    bool OnInit(void);
};

/* Define a new frame type */
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
   void LoadLWO( const wxString &filename);
   void OnMouse( wxMouseEvent& event );
   void InitGL(void);
   
   mesh_info  info;
   bool       block;

DECLARE_EVENT_TABLE()
};

#endif

