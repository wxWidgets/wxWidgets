/////////////////////////////////////////////////////////////////////////////
// Name:        pyramid.h
// Purpose:     OpenGL version >= 3.2 sample
// Author:      Manuel Martin
// Created:     2015/11/05
// Copyright:   (c) 2015 Manuel Martin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef PYRSAMLE_H
#define PYRSAMLE_H

// Define a new application
class MyApp: public wxApp
{
public:
    MyApp(){}
    bool OnInit() override;
};


class MyGLCanvas;

// The main frame class
class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title);

    void OnAbout(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
#if wxUSE_LOGWINDOW
    void OnLogWindow(wxCommandEvent& event);
#endif // wxUSE_LOGWINDOW
    void SetOGLString(const wxString& ogls)
        { m_OGLString = ogls; }
    bool OGLAvailable();

private:
#if wxUSE_LOGWINDOW
    wxLogWindow* m_LogWin;
#endif // wxUSE_LOGWINDOW
    wxString     m_OGLString;
    MyGLCanvas*  m_mycanvas;

    wxDECLARE_EVENT_TABLE();
};


// The canvas window
class MyGLCanvas : public wxGLCanvas
{
public:
    MyGLCanvas(MyFrame* parent, const wxGLAttributes& canvasAttrs);
    ~MyGLCanvas();

    //Used just to know if we must end now because OGL 3.2 isn't available
    bool OglCtxAvailable()
        {return m_oglContext != nullptr;}

    //Init the OpenGL stuff
    bool oglInit();

    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnMouse(wxMouseEvent& event);

private:
    // Members
    MyFrame*      m_parent;
    wxGLContext*  m_oglContext;
    myOGLManager* m_oglManager;
    int           m_winHeight; // We use this var to know if we have been sized

    wxDECLARE_EVENT_TABLE();
};


// IDs for the controls and the menu commands
enum
{
    Pyramid_Quit = wxID_EXIT,
    Pyramid_About = wxID_ABOUT,
    Pyramid_LogW = wxID_HIGHEST + 10
};

#endif // PYRSAMLE_H

