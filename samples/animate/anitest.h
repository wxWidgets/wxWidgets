/////////////////////////////////////////////////////////////////////////////
// Name:        anitest.cpp
// Purpose:     anitest sample
// Author:      Julian Smart
// Created:     02/07/2001
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/animate.h"

// Define a new application
class MyApp : public wxApp
{
public:
    bool OnInit() override;
};

// Define a new frame
class MyFrame : public wxFrame
{
public:

    MyFrame(wxWindow *parent, const wxWindowID id, const wxString& title);

    void OnAbout(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);

    void OnPlay(wxCommandEvent& event);
    void OnSetNullAnimation(wxCommandEvent& event);
    void OnSetInactiveBitmap(wxCommandEvent& event);
    void OnSetNoAutoResize(wxCommandEvent& event);
    void OnSetBgColor(wxCommandEvent& event);
    void OnStop(wxCommandEvent& event);

#ifdef wxHAS_NATIVE_ANIMATIONCTRL
    void OnUseGeneric(wxCommandEvent& event);
#endif // wxHAS_NATIVE_ANIMATIONCTRL

    void OnUpdateUI(wxUpdateUIEvent& event);

#if wxUSE_FILEDLG
    void OnOpen(wxCommandEvent& event);
#endif // wxUSE_FILEDLG

private:
    void RecreateAnimation(long style);

    // Update the frame controls after the animation changes.
    void UpdateFrameControls();

    // Update the controls showing the currently selected frame.
    void UpdateFrameInfo();

    void OnFrameSpin(wxSpinEvent& event);

    wxAnimationCtrlBase* m_animationCtrl;

    // Controls used to show the individual animation frames.
    wxStaticBox* m_frameBox;
    wxSpinCtrl* m_frameSpin;
    wxStaticText* m_frameCount;
    wxStaticText* m_frameDelay;
    wxStaticBitmap* m_frameBitmap;

    wxDECLARE_EVENT_TABLE();
};
