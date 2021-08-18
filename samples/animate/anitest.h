/////////////////////////////////////////////////////////////////////////////
// Name:        anitest.cpp
// Purpose:     anitest sample
// Author:      Julian Smart
// Modified by:
// Created:     02/07/2001
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/animate.h"

// Define a new application
class MyApp : public wxApp
{
public:
    bool OnInit() wxOVERRIDE;
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

    wxAnimationCtrlBase* m_animationCtrl;

    wxDECLARE_EVENT_TABLE();
};
