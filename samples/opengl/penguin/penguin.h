/////////////////////////////////////////////////////////////////////////////
// Name:        penguin.h
// Purpose:     wxGLCanvas demo program
// Author:      Robert Roebling
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PENGUIN_H_
#define _WX_PENGUIN_H_


#include "wx/defs.h"
#include "wx/app.h"
#include "wx/menu.h"
#include "wx/dcclient.h"

#include "wx/glcanvas.h"

extern "C"
{
#include "lw.h"
#include "trackball.h"
}

/* information needed to display lightwave mesh */
typedef struct
{
//  gint do_init;         /* true if initgl not yet called */
    bool do_init;
    lwObject *lwobject;   /* lightwave object mesh */
    float beginx,beginy;  /* position of mouse */
    float quat[4];        /* orientation of object */
    float zoom;           /* field of view in degrees */
} mesh_info;


/* Define a new application type */
class MyApp: public wxApp
{
public:
    bool OnInit();
};

/* Define a new frame type */
class TestGLCanvas;

class MyFrame: public wxFrame
{
public:
    MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
        const wxSize& size, long style = wxDEFAULT_FRAME_STYLE);

    void OnExit(wxCommandEvent& event);

#if wxUSE_GLCANVAS
    void SetCanvas( TestGLCanvas *canvas ) { m_canvas = canvas; }
    TestGLCanvas *GetCanvas() { return m_canvas; }

private:
    TestGLCanvas *m_canvas;
#endif

    DECLARE_EVENT_TABLE()
};

#if wxUSE_GLCANVAS

class TestGLCanvas: public wxGLCanvas
{
public:
    TestGLCanvas(wxWindow *parent, wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0,
        const wxString& name = wxT("TestGLCanvas"));

    ~TestGLCanvas();

    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void LoadLWO( const wxString &filename);
    void OnMouse( wxMouseEvent& event );
    void InitGL();

    mesh_info  info;
    bool       block;

private:
    DECLARE_EVENT_TABLE()
};

#endif // #if wxUSE_GLCANVAS

#endif // #ifndef _WX_PENGUIN_H_

