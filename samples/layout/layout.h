/////////////////////////////////////////////////////////////////////////////
// Name:        layout.h
// Purpose:     Layout sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Define a new application
class MyApp: public wxApp
{
public:
    MyApp();
    bool OnInit();
};

// Define a new frame
class MyTextWindow;
class MyWindow;

class MyFrame: public wxFrame
{
public:
    wxPanel *panel;
    MyTextWindow *text_window;
    MyWindow *canvas;
    MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h);
    void OnSize(wxSizeEvent& event);
    void Draw(wxDC& dc, bool draw_bitmaps = TRUE);

    void LoadFile(wxCommandEvent& event);
    void Quit(wxCommandEvent& event);
    void TestSizers(wxCommandEvent& event);
    void TestNotebookSizers(wxCommandEvent& event);
    void About(wxCommandEvent& event);

private:
    DECLARE_EVENT_TABLE()
};

// Define a new text subwindow that can respond to drag-and-drop
class MyTextWindow: public wxTextCtrl
{
public:
    MyTextWindow(wxFrame *frame, int x=-1, int y=-1, int width=-1, int height=-1,
               long style=wxTE_MULTILINE):
    wxTextCtrl(frame, -1, "", wxPoint(x, y), wxSize(width, height), style)
    {
    }

};

// Define a new canvas which can receive some events
class MyWindow: public wxWindow
{
public:
    MyWindow(wxFrame *frame, int x, int y, int w, int h, long style = wxRETAINED);
    ~MyWindow();
    void OnPaint(wxPaintEvent& event);

private:    
    DECLARE_EVENT_TABLE()
};

class MySizerFrame: public wxFrame
{
public:
    wxPanel *panel;
    MySizerFrame(wxFrame *frame, char *title, int x, int y );
};

#define LAYOUT_QUIT       100
#define LAYOUT_TEST       101
#define LAYOUT_ABOUT      102
#define LAYOUT_LOAD_FILE  103
#define LAYOUT_TEST_SIZER 104
#define LAYOUT_TEST_NB    105
