/////////////////////////////////////////////////////////////////////////////
// Name:        anitest.cpp
// Purpose:     anitest sample
// Author:      Julian Smart
// Modified by:
// Created:     02/07/2001
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/animate/animate.h"

// Define a new application
class MyApp : public wxApp
{
public:
    bool OnInit();
};

class MyCanvas : public wxScrolledWindow
{
public:
    MyCanvas(wxWindow *parent, const wxPoint& pos, const wxSize& size);
    void OnPaint(wxPaintEvent& event);

private:

    DECLARE_EVENT_TABLE()
};

// Define a new frame
class MyFrame : public wxFrame
{
public:

    MyFrame(wxWindow *parent, const wxWindowID id, const wxString& title,
            const wxPoint& pos, const wxSize& size, const long style);
    ~MyFrame();

    void OnAbout(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);

    void OnOpen(wxCommandEvent& event);

    MyCanvas* GetCanvas() const { return m_canvas; }
    wxGIFAnimationCtrl* GetAnimationCtrl() const { return m_animationCtrl; }

#if 0    
    wxAnimationPlayer& GetPlayer() { return m_player; }
    wxAnimationBase& GetAnimation() { return m_animation; }
#endif

    DECLARE_EVENT_TABLE()

protected:
    MyCanvas*           m_canvas;
    wxGIFAnimationCtrl* m_animationCtrl;
#if 0
    wxAnimationPlayer   m_player;
    wxGIFAnimation      m_animation;
#endif
};

// menu items ids
enum
{
    ANITEST_QUIT = 100,
    ANITEST_OPEN,
    ANITEST_ABOUT
};
