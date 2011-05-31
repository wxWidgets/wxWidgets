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

#ifndef _WXPOEM_H_
#define _WXPOEM_H_


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
    MyCanvas(wxFrame *frame);
    virtual ~MyCanvas();

    void OnPaint(wxPaintEvent& event);
    void OnMouseEvent(wxMouseEvent& event);
    void OnChar(wxKeyEvent& event);

private:
    wxMenu *m_popupMenu;

    DECLARE_EVENT_TABLE()
};

// Define a new frame
class MainWindow: public wxFrame
{
public:
    MyCanvas *canvas;
    MainWindow(wxFrame *frame, wxWindowID id, const wxString& title,
               const wxPoint& pos, const wxSize& size, long style);
    virtual ~MainWindow();

    void OnCloseWindow(wxCloseEvent& event);
    void OnChar(wxKeyEvent& event);
    void OnPopup(wxCommandEvent& event);

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

private:

    wxString m_searchString;
    wxString m_title;

    // Preferences
    void WritePreferences();
    void ReadPreferences();

    // Fonts
    void CreateFonts();
    wxFont *m_normalFont;
    wxFont *m_boldFont;
    wxFont *m_italicFont;

    // Icons
    wxIcon *m_corners[4];

    DECLARE_EVENT_TABLE()
};

// Menu items
enum
{
    POEM_ABOUT         = wxID_ABOUT,
    POEM_EXIT          = wxID_EXIT,
    POEM_PREVIOUS      = wxID_BACKWARD,
    POEM_COPY          = wxID_COPY,
    POEM_NEXT          = wxID_FORWARD,
    POEM_NEXT_MATCH    = wxID_MORE,
    POEM_BIGGER_TEXT   = wxID_ZOOM_IN,
    POEM_SMALLER_TEXT  = wxID_ZOOM_OUT,
    POEM_SEARCH        = wxID_FIND,
    POEM_MINIMIZE      = wxID_ICONIZE_FRAME
};

#endif      // _WXPOEM_H_
