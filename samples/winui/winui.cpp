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

#include "wx/activityindicator.h"
#include "wx/artprov.h"
#include "wx/bmpcbox.h"
#include "wx/calctrl.h"
#include "wx/checklst.h"
#include "wx/clrpicker.h"
#include "wx/choice.h"
#include "wx/datectrl.h"
#include "wx/gauge.h"
#include "wx/hyperlink.h"
#include "wx/infobar.h"
#include "wx/listbox.h"
#include "wx/notebook.h"
#include "wx/radiobox.h"
#include "wx/radiobut.h"
#include "wx/scrolwin.h"
#include "wx/slider.h"
#include "wx/spinbutt.h"
#include "wx/spinctrl.h"
#include "wx/srchctrl.h"
#include "wx/statline.h"
#include "wx/tglbtn.h"
#include "wx/timectrl.h"

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
        wxNotebook *notebook = new wxNotebook(this, wxID_ANY);

        wxScrolledWindow *panel = new wxScrolledWindow(notebook, wxID_ANY);
        panel->SetScrollRate(0, FromDIP(10)); // vertical scrolling only
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

        const wxString listItems[] =
        {
            "WinUI ListBox item 1",
            "WinUI ListBox item 2",
            "WinUI ListBox item 3",
            "WinUI ListBox item 4"
        };
        m_list = new wxListBox(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                               WXSIZEOF(listItems), listItems);
        m_list->SetSelection(0);
        sizer->Add(m_list, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(12));

        const wxString checkItems[] =
        {
            "WinUI CheckListBox A",
            "WinUI CheckListBox B",
            "WinUI CheckListBox C"
        };
        m_checkList = new wxCheckListBox(panel, wxID_ANY, wxDefaultPosition,
                                         wxDefaultSize, WXSIZEOF(checkItems),
                                         checkItems);
        m_checkList->Check(0, true);
        sizer->Add(m_checkList, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(12));

        const wxString radioChoices[] =
        {
            "WinUI RadioBox 1", "WinUI RadioBox 2", "WinUI RadioBox 3"
        };
        wxRadioBox *radioBox = new wxRadioBox(panel, wxID_ANY, "WinUI RadioBox",
            wxDefaultPosition, wxDefaultSize, WXSIZEOF(radioChoices),
            radioChoices, 1, wxRA_SPECIFY_COLS);
        sizer->Add(radioBox, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(12));

        m_slider = new wxSlider(panel, wxID_ANY, 40, 0, 100);
        sizer->Add(m_slider, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(12));

        m_gauge = new wxGauge(panel, wxID_ANY, 100);
        m_gauge->SetValue(40);
        sizer->Add(m_gauge, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(12));

        wxButton *button = new wxButton(panel, wxID_ANY, "WinUI Button");
        sizer->Add(button, 0, wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(12));

        m_toggle = new wxToggleButton(panel, wxID_ANY, "WinUI ToggleButton");
        sizer->Add(m_toggle, 0, wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(12));

        wxSearchCtrl *search = new wxSearchCtrl(panel, wxID_ANY);
        search->ShowSearchButton(true);
        search->ShowCancelButton(true);
        search->SetDescriptiveText("Search the WinUI AutoSuggestBox");
        wxArrayString suggestions;
        suggestions.Add("apple");
        suggestions.Add("apricot");
        suggestions.Add("banana");
        suggestions.Add("cherry");
        suggestions.Add("grape");
        search->AutoComplete(suggestions);
        sizer->Add(search, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(12));

        wxSpinCtrl *spin = new wxSpinCtrl(panel, wxID_ANY, wxEmptyString,
            wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 42);
        sizer->Add(spin, 0, wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(12));

        wxSpinButton *spinButton = new wxSpinButton(panel, wxID_ANY);
        spinButton->SetRange(0, 100);
        spinButton->SetValue(20);
        sizer->Add(spinButton, 0, wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(12));

        wxSpinCtrlDouble *spinDouble = new wxSpinCtrlDouble(panel, wxID_ANY,
            wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS,
            0.0, 10.0, 2.5, 0.5);
        spinDouble->SetDigits(1);
        sizer->Add(spinDouble, 0, wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(12));

        wxColourPickerCtrl *colourPicker = new wxColourPickerCtrl(panel, wxID_ANY,
            *wxBLUE);
        sizer->Add(colourPicker, 0, wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(12));

        wxBitmapComboBox *bmpCombo = new wxBitmapComboBox(panel, wxID_ANY);
        bmpCombo->Append("Information",
            wxArtProvider::GetBitmapBundle(wxART_INFORMATION, wxART_OTHER, FromDIP(wxSize(16, 16))));
        bmpCombo->Append("Warning",
            wxArtProvider::GetBitmapBundle(wxART_WARNING, wxART_OTHER, FromDIP(wxSize(16, 16))));
        bmpCombo->Append("Error",
            wxArtProvider::GetBitmapBundle(wxART_ERROR, wxART_OTHER, FromDIP(wxSize(16, 16))));
        bmpCombo->SetSelection(0);
        sizer->Add(bmpCombo, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(12));

        wxDatePickerCtrl *datePicker = new wxDatePickerCtrl(panel, wxID_ANY);
        sizer->Add(datePicker, 0, wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(12));

        wxTimePickerCtrl *timePicker = new wxTimePickerCtrl(panel, wxID_ANY);
        sizer->Add(timePicker, 0, wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(12));

        wxHyperlinkCtrl *link = new wxHyperlinkCtrl(panel, wxID_ANY,
            "WinUI HyperlinkButton (wxWidgets.org)", "https://www.wxwidgets.org/");
        sizer->Add(link, 0, wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(12));

        wxActivityIndicator *activity = new wxActivityIndicator(panel, wxID_ANY);
        activity->Start();
        sizer->Add(activity, 0, wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(12));

        wxStaticBitmap *bitmap = new wxStaticBitmap(panel, wxID_ANY,
            wxArtProvider::GetBitmapBundle(wxART_INFORMATION, wxART_OTHER,
                                           FromDIP(wxSize(32, 32))));
        sizer->Add(bitmap, 0, wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(12));

        m_themeButton = new wxButton(panel, wxID_ANY, "Theme: System");
        sizer->Add(m_themeButton, 0, wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(12));

        panel->SetSizer(sizer);
        // Set the scrolled window's virtual size from the sizer so the vertical
        // scrollbar appears when the controls overflow the page area.
        sizer->FitInside(panel);

        // Second notebook page, to show the WinUI TabView switching pages.
        wxPanel *page2 = new wxPanel(notebook, wxID_ANY);
        wxBoxSizer *page2Sizer = new wxBoxSizer(wxVERTICAL);
        page2Sizer->Add(new wxStaticText(page2, wxID_ANY,
                            "This is a second WinUI TabView page."),
                        0, wxALL, FromDIP(16));
        wxButton *page2Button = new wxButton(page2, wxID_ANY, "A button on page 2");
        page2Sizer->Add(page2Button, 0, wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(16));

        wxInfoBar *infoBar = new wxInfoBar(page2);
        page2Sizer->Add(infoBar, 0, wxEXPAND | wxALL, FromDIP(16));
        infoBar->ShowMessage("This is a WinUI InfoBar.", wxICON_INFORMATION);

        wxCalendarCtrl *calendar = new wxCalendarCtrl(page2, wxID_ANY);
        page2Sizer->Add(calendar, 0, wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(16));

        page2->SetSizer(page2Sizer);

        notebook->AddPage(panel, "Controls", true);
        notebook->AddPage(page2, "More");

        button->Bind(wxEVT_BUTTON, &WinUISampleFrame::OnButton, this);
        m_toggle->Bind(wxEVT_TOGGLEBUTTON, &WinUISampleFrame::OnToggle, this);
        m_themeButton->Bind(wxEVT_BUTTON, &WinUISampleFrame::OnToggleTheme, this);
        m_text->Bind(wxEVT_TEXT, &WinUISampleFrame::OnText, this);
        m_choice->Bind(wxEVT_CHOICE, &WinUISampleFrame::OnChoice, this);
        m_check->Bind(wxEVT_CHECKBOX, &WinUISampleFrame::OnCheckBox, this);
        m_radioA->Bind(wxEVT_RADIOBUTTON, &WinUISampleFrame::OnRadio, this);
        m_radioB->Bind(wxEVT_RADIOBUTTON, &WinUISampleFrame::OnRadio, this);
        m_slider->Bind(wxEVT_SLIDER, &WinUISampleFrame::OnSlider, this);
        m_list->Bind(wxEVT_LISTBOX, &WinUISampleFrame::OnListBox, this);
        m_checkList->Bind(wxEVT_CHECKLISTBOX, &WinUISampleFrame::OnCheckList, this);

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
        // Escape mnemonics: a status label is a wxControl label, so a bare '&'
        // in user text would be treated as a mnemonic marker and swallowed.
        m_status->SetLabel("Button clicked; text is: " +
                           wxControl::EscapeMnemonics(m_text->GetValue()));
    }

    void OnToggle(wxCommandEvent& event)
    {
        m_status->SetLabel(event.IsChecked() ? "Toggle is ON" : "Toggle is OFF");
    }

    void OnText(wxCommandEvent& event)
    {
        m_status->SetLabel("Text changed: " +
                           wxControl::EscapeMnemonics(event.GetString()));
    }

    void OnChoice(wxCommandEvent& event)
    {
        m_status->SetLabel("Choice selected: " +
                           wxControl::EscapeMnemonics(event.GetString()));
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
        m_gauge->SetValue(event.GetInt());
    }

    void OnListBox(wxCommandEvent& event)
    {
        m_status->SetLabel("ListBox selected: " +
                           wxControl::EscapeMnemonics(event.GetString()));
    }

    void OnCheckList(wxCommandEvent& event)
    {
        const int n = event.GetInt();
        m_status->SetLabel(wxString::Format("CheckListBox item %d %s", n,
                           m_checkList->IsChecked(n) ? "checked" : "unchecked"));
    }

    wxStaticText *m_status = nullptr;
    wxTextCtrl *m_text = nullptr;
    wxChoice *m_choice = nullptr;
    wxCheckBox *m_check = nullptr;
    wxRadioButton *m_radioA = nullptr;
    wxRadioButton *m_radioB = nullptr;
    wxSlider *m_slider = nullptr;
    wxListBox *m_list = nullptr;
    wxCheckListBox *m_checkList = nullptr;
    wxGauge *m_gauge = nullptr;
    wxToggleButton *m_toggle = nullptr;
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
