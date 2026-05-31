/////////////////////////////////////////////////////////////////////////////
// Name:        samples/winui/winui.cpp
// Purpose:     wxWinUI sample
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/choice.h"
#include "wx/radiobut.h"
#include "wx/slider.h"
#include "wx/statline.h"

#include "wx/winui/winui.h"

#if !wxUSE_WINUI3
    #error "This sample requires wxUSE_WINUI3"
#endif

namespace
{

class WinUISampleFrame : public wxFrame
{
public:
    WinUISampleFrame()
        : wxFrame(nullptr, wxID_ANY, "wxWinUI sample", wxDefaultPosition, wxSize(720, 480))
    {
        wxPanel *panel = new wxPanel(this);
        wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

        m_status = new wxStaticText
        (
            panel,
            wxID_ANY,
            "wxStaticText rendered by WinUI TextBlock"
        );
        sizer->Add(m_status, 0, wxEXPAND | wxALL, FromDIP(12));

        sizer->Add(new wxStaticLine(panel), 0, wxEXPAND | wxLEFT | wxRIGHT, FromDIP(12));

        m_text = new wxTextCtrl
        (
            panel,
            wxID_ANY,
            "Edit me from a WinUI TextBox",
            wxDefaultPosition,
            wxDefaultSize,
            wxTE_PROCESS_ENTER
        );
        sizer->Add(m_text, 0, wxEXPAND | wxALL, FromDIP(12));

        const wxString choices[] =
        {
            "WinUI ComboBox item 1",
            "WinUI ComboBox item 2",
            "WinUI ComboBox item 3"
        };
        m_choice = new wxChoice
        (
            panel,
            wxID_ANY,
            wxDefaultPosition,
            wxDefaultSize,
            WXSIZEOF(choices),
            choices
        );
        m_choice->SetSelection(0);
        sizer->Add(m_choice, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(12));

        m_check = new wxCheckBox(panel, wxID_ANY, "WinUI CheckBox");
        m_check->SetValue(true);
        sizer->Add(m_check, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(12));

        m_radioA = new wxRadioButton(panel, wxID_ANY, "WinUI Radio A", wxDefaultPosition,
                                     wxDefaultSize, wxRB_GROUP);
        m_radioB = new wxRadioButton(panel, wxID_ANY, "WinUI Radio B");
        wxBoxSizer *radioSizer = new wxBoxSizer(wxHORIZONTAL);
        radioSizer->Add(m_radioA, 0, wxRIGHT, FromDIP(16));
        radioSizer->Add(m_radioB, 0);
        sizer->Add(radioSizer, 0, wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(12));

        m_slider = new wxSlider(panel, wxID_ANY, 40, 0, 100);
        sizer->Add(m_slider, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(12));

        wxButton *button = new wxButton(panel, wxID_ANY, "WinUI Button");
        sizer->Add(button, 0, wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(12));

        m_themeButton = new wxButton(panel, wxID_ANY, "Theme: System");
        sizer->Add(m_themeButton, 0, wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(12));

        panel->SetSizer(sizer);

        button->Bind(wxEVT_BUTTON, &WinUISampleFrame::OnButton, this);
        m_themeButton->Bind(wxEVT_BUTTON, &WinUISampleFrame::OnToggleTheme, this);
        m_text->Bind(wxEVT_TEXT, &WinUISampleFrame::OnText, this);
        m_choice->Bind(wxEVT_CHOICE, &WinUISampleFrame::OnChoice, this);
        m_check->Bind(wxEVT_CHECKBOX, &WinUISampleFrame::OnCheckBox, this);
        m_radioA->Bind(wxEVT_RADIOBUTTON, &WinUISampleFrame::OnRadio, this);
        m_radioB->Bind(wxEVT_RADIOBUTTON, &WinUISampleFrame::OnRadio, this);
        m_slider->Bind(wxEVT_SLIDER, &WinUISampleFrame::OnSlider, this);

        Centre();
    }

private:
    void OnToggleTheme(wxCommandEvent&)
    {
        // Cycle System -> Light -> Dark and route the change through the
        // standard wxApp::SetAppearance() so it works exactly like it would in
        // any other wxWidgets application.
        const char *label = "Theme: System";
        switch ( m_theme )
        {
            case wxWinUIAppTheme::System:
                m_theme = wxWinUIAppTheme::Light;
                label = "Theme: Light";
                break;
            case wxWinUIAppTheme::Light:
                m_theme = wxWinUIAppTheme::Dark;
                label = "Theme: Dark";
                break;
            case wxWinUIAppTheme::Dark:
                m_theme = wxWinUIAppTheme::System;
                label = "Theme: System";
                break;
        }

        wxApp::Appearance appearance = wxApp::Appearance::System;
        if ( m_theme == wxWinUIAppTheme::Light )
            appearance = wxApp::Appearance::Light;
        else if ( m_theme == wxWinUIAppTheme::Dark )
            appearance = wxApp::Appearance::Dark;

        wxTheApp->SetAppearance(appearance);
        m_themeButton->SetLabel(label);
    }

    void OnButton(wxCommandEvent&)
    {
        m_status->SetLabel("Button clicked; text is: " + m_text->GetValue());
    }

    void OnText(wxCommandEvent& event)
    {
        m_status->SetLabel("Text changed: " + event.GetString());
    }

    void OnChoice(wxCommandEvent& event)
    {
        m_status->SetLabel("Choice selected: " + event.GetString());
    }

    void OnCheckBox(wxCommandEvent&)
    {
        m_status->SetLabel(m_check->GetValue() ? "Checkbox checked" : "Checkbox unchecked");
    }

    void OnRadio(wxCommandEvent& event)
    {
        m_status->SetLabel(event.GetEventObject() == m_radioA ? "Radio A selected" : "Radio B selected");
    }

    void OnSlider(wxCommandEvent& event)
    {
        m_status->SetLabel(wxString::Format("Slider value: %d", event.GetInt()));
    }

    wxStaticText *m_status = nullptr;
    wxTextCtrl *m_text = nullptr;
    wxChoice *m_choice = nullptr;
    wxCheckBox *m_check = nullptr;
    wxRadioButton *m_radioA = nullptr;
    wxRadioButton *m_radioB = nullptr;
    wxSlider *m_slider = nullptr;
    wxButton *m_themeButton = nullptr;
    wxWinUIAppTheme m_theme = wxWinUIAppTheme::System;
};

class WinUISampleApp : public wxApp
{
public:
    bool OnInit() override
    {
        if ( !wxApp::OnInit() )
            return false;

        WinUISampleFrame *frame = new WinUISampleFrame;
        frame->Show();
        // Give the window a Mica backdrop and a theme-matching title bar.
        wxWinUIApplyWindowBackdrop(frame);
        return true;
    }
};

} // namespace

wxIMPLEMENT_APP(WinUISampleApp);
