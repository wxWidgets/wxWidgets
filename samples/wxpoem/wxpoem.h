/////////////////////////////////////////////////////////////////////////////
// Name:        wxpoem.h
// Purpose:     A small C++ program which displays a random poem on
//              execution. It also allows search for poems containing a
//              string.
//              It requires winpoem.dat and creates winpoem.idx.
//              Original version (WinPoem) written in 1994.
//              This has not been rewritten in a long time so
//              beware, inelegant code!
// Author:      Julian Smart
// Created:     12/12/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma interface "wxpoem.h"
#endif

// Define a new application
class MyApp: public wxApp
{
  public:
    bool OnInit();
    int OnExit();
};

DECLARE_APP(MyApp)

// Define a new canvas which can receive some events
class MyCanvas: public wxWindow
{
  public:
    MyCanvas(wxFrame *frame, wxWindowID id, const wxPoint& pos, const wxSize& size);

    void OnPaint(wxPaintEvent& event);
    void OnMouseEvent(wxMouseEvent& event);
    void OnChar(wxKeyEvent& event);

    DECLARE_EVENT_TABLE()
};

// Define a new frame
class MainWindow: public wxFrame
{
  public:
    MyCanvas *canvas;
    MainWindow(wxFrame *frame, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style);
    ~MainWindow();

    void OnCloseWindow(wxCloseEvent& event);
    void OnChar(wxKeyEvent& event);

    // Display next page or poem
    void NextPage(void);

    // Display previous page
    void PreviousPage(void);

    // User search
    void Search(bool);

    // Look in file for string
    long DoSearch(void);

    // Do the actual drawing of text (or just calculate size needed)
    void ScanBuffer(wxDC *dc, bool DrawIt, int *max_x, int *max_y);

    // Load the poem
    void GetIndexLoadPoem(void);
    void Resize(void);

DECLARE_EVENT_TABLE()
};

// Menu items
#define         POEM_NEXT       100
#define         POEM_PREVIOUS   101
#define         POEM_COPY       102
#define         POEM_SEARCH     103
#define         POEM_NEXT_MATCH 104
#define         POEM_ABOUT      105
#define         POEM_EXIT       106
#define         POEM_COMPILE    107
#define         POEM_HELP_CONTENTS 108
#define         POEM_BIGGER_TEXT 109
#define         POEM_SMALLER_TEXT 110
#define         POEM_MINIMIZE   111


