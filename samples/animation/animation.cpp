/////////////////////////////////////////////////////////////////////////////
// Name:        animation.cpp
// Purpose:     wxUIAnimation sample
// Author:      Chiciu Bogdan Gabriel
// Created:     2011-07-02
// RCS-ID:      $Id$
// Copyright:   (c) 2011 wxWidgets Team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/frame.h"
    #include "wx/button.h"
    #include "wx/panel.h"
    #include "wx/button.h"
    #include "wx/sizer.h"
#endif

#include "wx/uianimation.h"
#include "wx/uianimationstoryboard.h"

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

class MyPanel : public wxPanel
{
public:
    MyPanel(wxWindow *parent);
private:
    // Event handlers for our buttons
    void OnChangeBackgroundColour(wxCommandEvent& event);

    void OnMoveWithKeyframeAndHide(wxCommandEvent& event);

    void OnChangeBackgroundColourAndMove(wxCommandEvent& event);

    void OnChangeSize(wxCommandEvent& event);
    // Event handler for when a storyboard finishes playing.
    // The object will be released with this event handler.
    void OnStoryboardFinished(wxUIAnimationStoryboardEvent& event);

    wxSizer *m_sizer;
    wxButton* m_animatedButton1;
    wxButton* m_animatedButton2;
    wxButton* m_animatedButton3;
    wxButton* m_animatedButton4;

    DECLARE_EVENT_TABLE()
};

// Define a main frame class with 3 buttons
class MyFrame : public wxFrame
{
public:
    // ctor
    MyFrame(const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
private:
    MyPanel* m_panel;
    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
};

// IDs for the buttons
enum
{
    Animation_ChangeBackgroundColor = 200,
    Animation_MoveWithKeyframes,
    Animation_ChangeBackgroundColorAndMove,
    Animation_ChangeSize
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them.
BEGIN_EVENT_TABLE(MyPanel, wxPanel)
    EVT_BUTTON(Animation_ChangeBackgroundColor, MyPanel::OnChangeBackgroundColour)
    EVT_BUTTON(Animation_MoveWithKeyframes, MyPanel::OnMoveWithKeyframeAndHide)
    EVT_BUTTON(Animation_ChangeBackgroundColorAndMove, MyPanel::OnChangeBackgroundColourAndMove)
    EVT_BUTTON(Animation_ChangeSize, MyPanel::OnChangeSize)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;
    MyFrame *frame = new MyFrame("wxUIAnimation sample", wxPoint(50, 50), wxSize(1280, 720) );
    frame->Show(true);

    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, wxID_ANY, title, pos, size)
{
    m_panel = new MyPanel(this);
}

// ----------------------------------------------------------------------------
// panel frame
// ----------------------------------------------------------------------------
MyPanel::MyPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
{
    SetBackgroundColour(wxColour("WHITE"));
    wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    
    m_animatedButton1 = new wxButton(this, Animation_ChangeBackgroundColor, "Animate background color");
    m_animatedButton2 = new wxButton(this, Animation_MoveWithKeyframes, "Animate positon");
    m_animatedButton3 = new wxButton(this, Animation_ChangeBackgroundColorAndMove, "Animate position and background color");
    m_animatedButton4 = new wxButton(this, Animation_ChangeSize, "Animate size");

    sizer->Add(m_animatedButton1);
    sizer->Add(m_animatedButton2);
    sizer->Add(m_animatedButton3);
    sizer->Add(m_animatedButton4);

    SetSizer(sizer);
}

// Animates the background color of m_AnimatedButton1.
void MyPanel::OnChangeBackgroundColour(wxCommandEvent& event)
{
    wxUIAnimationStoryboard* storyboard = new wxUIAnimationStoryboard();
    storyboard->Connect(wxEVT_STORYBOARD, wxStoryboardEventHandler(MyPanel::OnStoryboardFinished));
    wxUIAnimation<wxANIMATION_TARGET_PROPERTY_BACKGROUND_COLOR> backgroundColourAnimation(*wxRED,
        *wxBLUE,
        0.4);

    storyboard->SetAnimationTarget(m_animatedButton1);
    storyboard->SetRepeatCount(4);
    storyboard->AddAnimation(backgroundColourAnimation);
    storyboard->Start();
}

// Animates the position of m_AnimatedButton2 using key frames then hides it when the
// end of the animation animation is reached.
void MyPanel::OnMoveWithKeyframeAndHide(wxCommandEvent& event)
{
    wxUIAnimationStoryboard* storyboard = new wxUIAnimationStoryboard();
    storyboard->Connect(wxEVT_STORYBOARD, wxStoryboardEventHandler(MyPanel::OnStoryboardFinished));
    wxUIKeyframeAnimation<wxANIMATION_TARGET_PROPERTY_POSITION> positionAnimation;
    wxUIAnimation<wxANIMATION_TARGET_PROPERTY_HIDDEN> hideAnimation(false, true, 3.2);

    wxUIAnimationKeyframe<wxPoint> keyframe1(wxPoint(100,100), 0.5);
    wxUIAnimationKeyframe<wxPoint> keyframe2(wxPoint(300,300), 1);
    wxUIAnimationKeyframe<wxPoint> keyframe3(wxPoint(600,600), 0.5);
    wxUIAnimationKeyframe<wxPoint> keyframe4(wxPoint(100,100), 0.7);
    wxUIAnimationKeyframe<wxPoint> keyframe5(wxPoint(10,500), 0.5);

    positionAnimation.AddKeyframe(keyframe1);
    positionAnimation.AddKeyframe(keyframe2);
    positionAnimation.AddKeyframe(keyframe3);
    positionAnimation.AddKeyframe(keyframe4);
    positionAnimation.AddKeyframe(keyframe5);

    storyboard->SetAnimationTarget(m_animatedButton2);
    storyboard->AddAnimation(positionAnimation);
    storyboard->AddAnimation(hideAnimation);
    storyboard->Start();
}

// Animates the position and background color of m_animatedButton3 using simple animations.
void MyPanel::OnChangeBackgroundColourAndMove(wxCommandEvent& event)
{
    wxUIAnimationStoryboard* storyboard = new wxUIAnimationStoryboard();
    storyboard->Connect(wxEVT_STORYBOARD, wxStoryboardEventHandler(MyPanel::OnStoryboardFinished));
    wxUIAnimation<wxANIMATION_TARGET_PROPERTY_POSITION> positionAnimation(m_animatedButton3->GetPosition(),
        wxPoint(500,500), 0.4);
    wxUIAnimation<wxANIMATION_TARGET_PROPERTY_BACKGROUND_COLOR> backgroundColourAnimation(*wxRED, *wxBLUE, 0.7);

    storyboard->SetAnimationTarget(m_animatedButton3);

    storyboard->AddAnimation(positionAnimation);
    storyboard->AddAnimation(backgroundColourAnimation);
    storyboard->Start();
}

// Animates the size of m_animatedButton4 using a simple animations that repeats forever.
void MyPanel::OnChangeSize(wxCommandEvent& event)
{
    // NOTE: this will inevitably leak since OnStoryboardFinished never fires.
    // TODO: fix
    wxUIAnimationStoryboard* storyboard = new wxUIAnimationStoryboard();
    storyboard->Connect(wxEVT_STORYBOARD, wxStoryboardEventHandler(MyPanel::OnStoryboardFinished));
    wxUIAnimation<wxANIMATION_TARGET_PROPERTY_SIZE> sizeAnimation(m_animatedButton3->GetSize(),
        wxSize(200,250), 0.5);
    
    storyboard->SetAnimationTarget(m_animatedButton4);

    storyboard->SetRepeatCount(wxSTORYBOARD_REPEAT_FOREVER);
    storyboard->AddAnimation(sizeAnimation);
    storyboard->Start();
}

void MyPanel::OnStoryboardFinished(wxUIAnimationStoryboardEvent& event)
{
    if(event.GetStoryboardStatus() == wxSTORYBOARD_STATUS_FINISHED)
    {
        // The storyboard that fired this event has finished playing. It is now safe to delete it.
        delete event.GetSender();
    }
}