/*
 * Program: canvas
 *
 * Author: Robert Roebling
 *
 * Copyright: (C) 1998, Robert Roebling
 *
 */
// For compilers that support precompilation, includes "wx/wx.h".

#ifndef __test_H__
#define __test_H__

#ifdef __GNUG__
    #pragma interface "test.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <wx/image.h>
#include <wx/file.h>
#include <wx/timer.h>
#include <wx/log.h>
#include <wx/splitter.h>

#include "wx/canvas/canvas.h"
#include "wx/canvas/polygon.h"

class MyCanvas;

// derived classes
class MywxCanvasObjectRef: public wxCanvasObjectRef
{
    DECLARE_DYNAMIC_CLASS(MywxCanvasObjectRef)

public:
    MywxCanvasObjectRef();
    MywxCanvasObjectRef(double x, double y, wxCanvasObjectGroup* group);
    ~MywxCanvasObjectRef();

    void OnMouseEvent(wxMouseEvent &event);

private:
    DECLARE_EVENT_TABLE()
};


class MyEventHandler: public wxEvtHandler
{
public:
    MyEventHandler();

    void OnMouseEvent(wxMouseEvent &event);

private:

    DECLARE_EVENT_TABLE()
};


class MySplitterWindow : public wxSplitterWindow
{
public:
  MySplitterWindow(wxFrame *parent, wxWindowID id)
    : wxSplitterWindow(parent, id, wxDefaultPosition, wxDefaultSize, wxSP_3D | wxSP_LIVE_UPDATE)
  {
    m_frame = parent;
  }

  virtual bool OnSashPositionChange(int newSashPosition)
  {
    if ( !wxSplitterWindow::OnSashPositionChange(newSashPosition) )
      return FALSE;

    wxString str;
    str.Printf( _T("Sash position = %d"), newSashPosition);
    m_frame->SetStatusText(str);

    return TRUE;
  }

  void SetStatusText(const wxString& str ){m_frame->SetStatusText(str);}

private:
  wxFrame *m_frame;
};


// MyFrame
const int ID_ABOUT = 109;

// MyFrame
class MyFrame: public wxFrame
{
public:
    MyFrame(wxFrame* frame, const wxString& title, const wxPoint& pos, const wxSize& size);
    virtual ~MyFrame();

    // Menu commands
    void SplitHorizontal(wxCommandEvent& event);
    void SplitVertical(wxCommandEvent& event);
    void Unsplit(wxCommandEvent& event);
    void SetMinSize(wxCommandEvent& event);
    void Quit(wxCommandEvent& event);

    // Menu command update functions
    void UpdateUIHorizontal(wxUpdateUIEvent& event);
    void UpdateUIVertical(wxUpdateUIEvent& event);
    void UpdateUIUnsplit(wxUpdateUIEvent& event);

    void OnAbout( wxCommandEvent &event );
    void OnNewFrame( wxCommandEvent &event );
    void OnQuit( wxCommandEvent &event );
    void OnTimer( wxTimerEvent &event );

    wxCanvasObject   *m_sm1;
    wxCanvasObject   *m_sm2;
    wxCanvasObject   *m_sm3;
    wxCanvasObject   *m_sm4;

    MywxCanvasObjectRef *m_ref;
    MywxCanvasObjectRef *m_ref2;

    wxTimer          *m_timer;
    wxTextCtrl       *m_log;

    wxBitmap gs_bmp36_mono;

private:

    void UpdatePosition();

    wxMenu*    fileMenu;
    wxMenuBar*  menuBar;
    MyCanvas*  m_canvas1;
    MyCanvas*  m_canvas2;
    MySplitterWindow* m_splitter;

    wxCanvasAdmin m_canvasadmin;

    wxCanvasObjectGroup* m_datatree;

    DECLARE_EVENT_TABLE()

    MyEventHandler* m_eventhandler;
};

// ID for the menu commands
enum
{
  SPLIT_QUIT,
  SPLIT_HORIZONTAL,
  SPLIT_VERTICAL,
  SPLIT_UNSPLIT,
  SPLIT_SETMINSIZE
};

// Window ids
#define SPLITTER_WINDOW     100
#define SPLITTER_FRAME      101
#define CANVAS1             102
#define CANVAS2             103

class MyCanvas: public wxCanvas
{
public:
    MyCanvas( wxCanvasAdmin* admin, MySplitterWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxScrolledWindowStyle );

    void OnMouseEvent(wxMouseEvent& event);

private:

    //mouse position used for displaying it in the statusbar
    int          m_mouse_x;
    //mouse position used for displaying it in the statusbar
    int          m_mouse_y;
    //mouse position used for displaying it in the statusbar
    double       m_mouse_worldx;
    //mouse position used for displaying it in the statusbar
    double       m_mouse_worldy;

    //mouse position used for zooming
    double       m_zoom_x1;
    //mouse position used for zooming
    double       m_zoom_y1;
    //mouse position used for zooming
    double       m_zoom_x2;
    //mouse position used for zooming
    double       m_zoom_y2;

    //declare events
    DECLARE_EVENT_TABLE()

    //parent of the canvas
    MySplitterWindow *m_parent;
};

// MyApp

class MyApp: public wxApp
{
public:
    virtual bool OnInit();

    const wxString& GetFontPath() const { return m_fontpath; }

private:
    wxString m_fontpath;
};

#endif
