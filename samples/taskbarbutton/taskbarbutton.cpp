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
    VisibilityRadio,
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
    void OnVisibilityChange(wxCommandEvent& WXUNUSED(event));

    wxSlider *m_slider;
    wxRadioBox *m_visibilityRadioBox;
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

    // Show/Hide in Taskbar section.
    const wxString labels[] =
    {
        "&Show in Taskbar",
        "&Hide in Taskbar"
    };
    m_visibilityRadioBox = new wxRadioBox(panel, VisibilityRadio, "Visibility:",
                                          wxDefaultPosition, wxDefaultSize,
                                          WXSIZEOF(labels), labels,
                                          1, wxRA_SPECIFY_ROWS);

    gs->Add(spvSizer, 0, wxEXPAND);
    gs->Add(m_visibilityRadioBox, 0, wxEXPAND);

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
    EVT_RADIOBOX(VisibilityRadio, MyFrame::OnVisibilityChange)
wxEND_EVENT_TABLE()

void MyFrame::OnSetProgressValue(wxScrollEvent& WXUNUSED(event))
{
    MSWGetTaskBarButton()->SetProgressValue(m_slider->GetValue());
}

void MyFrame::OnVisibilityChange(wxCommandEvent& WXUNUSED(event))
{
    if ( m_visibilityRadioBox->GetSelection() == 0 )
        MSWGetTaskBarButton()->ShowInTaskbar();
    else
        MSWGetTaskBarButton()->HideInTaskbar();
}
