/////////////////////////////////////////////////////////////////////////////
// Name:        penguin.h
// Purpose:     wxGLCanvas demo program
// Author:      Robert Roebling
// Modified by:
// Created:     04/01/98
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PENGUIN_H_
#define _WX_PENGUIN_H_


#include "wx/defs.h"
#include "wx/app.h"
#include "wx/menu.h"
#include "wx/dcclient.h"
#include "wx/wfstream.h"
#if wxUSE_ZLIB
#include "wx/zstream.h"
#endif

#include "wx/glcanvas.h"

extern "C"
{
#include "trackball.h"
}

#include "dxfrenderer.h"


// OpenGL view data
struct GLData
{
    bool initialized;           // have OpenGL been initialized?
    float beginx, beginy;       // position of mouse
    float quat[4];              // orientation of object
    float zoom;                 // field of view in degrees
};


// Define a new application type
class MyApp : public wxApp
{
public:
    virtual bool OnInit() override;
};


// Define a new frame type
class TestGLCanvas;


class MyFrame : public wxFrame
{
public:
    MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
            const wxSize& size, long style = wxDEFAULT_FRAME_STYLE);

    void OnMenuFileOpen(wxCommandEvent& event);
    void OnMenuFileExit(wxCommandEvent& event);
    void OnMenuHelpAbout(wxCommandEvent& event);

    void SetCanvas(TestGLCanvas *canvas) { m_canvas = canvas; }
    TestGLCanvas *GetCanvas() { return m_canvas; }

private:
    TestGLCanvas *m_canvas;

    wxDECLARE_EVENT_TABLE();
};


class TestGLCanvas : public wxGLCanvas
{
public:
    TestGLCanvas(wxWindow *parent, wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0,
        const wxString& name = "TestGLCanvas");

    virtual ~TestGLCanvas();

    void LoadDXF(const wxString& filename);

protected:
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void OnMouse(wxMouseEvent& event);

private:
    void InitGL();
    void ResetProjectionMode();

    wxGLContext* m_glRC;
    GLData       m_gldata;
    DXFRenderer  m_renderer;

    wxDECLARE_NO_COPY_CLASS(TestGLCanvas);
    wxDECLARE_EVENT_TABLE();
};

#endif // #ifndef _WX_PENGUIN_H_
