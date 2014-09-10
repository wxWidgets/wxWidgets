/////////////////////////////////////////////////////////////////////////////
// Name:        taskbarbutton.cpp
// Purpose:     wxTaskbarButton sample
// Author:      Chaobin Zhang <zhchbin@gmail.com>
// Created:     2014-04-30
// Copyright:   (c) 2014 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/taskbarbutton.h"

enum
{
    ProgressValueSlider = wxID_HIGHEST,
};

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title);

private:
    wxDECLARE_EVENT_TABLE();

    void OnSetProgressValue(wxScrollEvent& WXUNUSED(event));

    wxSlider *m_slider;
};

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    MyFrame *frame = new MyFrame("wxTaskbarButton App");
    frame->Show(true);

    return true;
}

MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title)
{
    wxPanel *panel = new wxPanel(this);
    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
    wxFlexGridSizer *gs = new wxFlexGridSizer(4, 2, 10, 10);

    // SetProgressValue section.
    wxStaticBoxSizer *spvSizer =
        new wxStaticBoxSizer(wxVERTICAL, panel, wxT("SetProgressValue"));
    int flags = wxSL_MIN_MAX_LABELS | wxSL_VALUE_LABEL | wxSL_AUTOTICKS;
    m_slider = new wxSlider(spvSizer->GetStaticBox(), ProgressValueSlider,
                            0, 0, 100,
                            wxDefaultPosition, wxSize(250, -1),
                            flags);
    m_slider->SetTickFreq(10);
    spvSizer->Add(m_slider);

    gs->Add(spvSizer, 0, wxEXPAND);

    wxStaticText *text = new wxStaticText(
        panel, wxID_ANY, wxT("Welcome to wxTaskbarButton sample"));
    mainSizer->Add(text, 0, wxALIGN_CENTRE_HORIZONTAL);
    mainSizer->Add(gs);

    panel->SetSizer(mainSizer);

    SetIcon(wxICON(sample));
    SetSize(537, 420);
    Centre();
}

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_COMMAND_SCROLL_CHANGED(ProgressValueSlider, MyFrame::OnSetProgressValue)
wxEND_EVENT_TABLE()

void MyFrame::OnSetProgressValue(wxScrollEvent& WXUNUSED(event))
{
    MSWGetTaskBarButton()->SetProgressValue(m_slider->GetValue());
}
