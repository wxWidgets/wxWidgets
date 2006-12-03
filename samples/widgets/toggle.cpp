/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        toggle.cpp
// Purpose:     Part of the widgets sample showing toggle control
// Author:      Dimitri Schoolwerth, Vadim Zeitlin
// Created:     27 Sep 2003
// Id:          $Id$
// Copyright:   (c) 2006 Wlodzmierz Skiba
// License:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_TOGGLEBTN

#include "wx/tglbtn.h"

#include "widgets.h"

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/sizer.h"
    #include "wx/statbox.h"
    #include "wx/textctrl.h"
#endif

#include "icons/toggle.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    TogglePage_Reset = wxID_HIGHEST,
    TogglePage_ChangeLabel,
    TogglePage_Picker
};

// ----------------------------------------------------------------------------
// CheckBoxWidgetsPage
// ----------------------------------------------------------------------------

class ToggleWidgetsPage : public WidgetsPage
{
public:
    ToggleWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist);
    virtual ~ToggleWidgetsPage(){};

    virtual wxControl *GetWidget() const { return m_toggle; }
    virtual void RecreateWidget() { CreateToggle(); }

    // lazy creation of the content
    virtual void CreateContent();

protected:
    // event handlers
    void OnButtonReset(wxCommandEvent& event);
    void OnButtonChangeLabel(wxCommandEvent& event);

    // reset the toggle parameters
    void Reset();

    // (re)create the toggle
    void CreateToggle();

    // the controls
    // ------------

    // the checkbox itself and the sizer it is in
    wxToggleButton *m_toggle;
    wxSizer *m_sizerToggle;

    // the text entries for command parameters
    wxTextCtrl *m_textLabel;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_WIDGETS_PAGE(ToggleWidgetsPage)
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ToggleWidgetsPage, WidgetsPage)
    EVT_BUTTON(TogglePage_Reset, ToggleWidgetsPage::OnButtonReset)
    EVT_BUTTON(TogglePage_ChangeLabel, ToggleWidgetsPage::OnButtonChangeLabel)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

#if defined(__WXUNIVERSAL__)
    #define FAMILY_CTRLS UNIVERSAL_CTRLS
#else
    #define FAMILY_CTRLS NATIVE_CTRLS
#endif

IMPLEMENT_WIDGETS_PAGE(ToggleWidgetsPage, wxT("ToggleButton"),
                       FAMILY_CTRLS
                       );

ToggleWidgetsPage::ToggleWidgetsPage(WidgetsBookCtrl *book,
                                     wxImageList *imaglist)
                      :WidgetsPage(book, imaglist, toggle_xpm)
{
}

void ToggleWidgetsPage::CreateContent()
{
    wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    // left pane
//    wxStaticBox *box = new wxStaticBox(this, wxID_ANY, wxT("Styles"));

//    wxSizer *sizerLeft = new wxStaticBoxSizer(box, wxVERTICAL);

    // middle pane
    wxStaticBox *box2 = new wxStaticBox(this, wxID_ANY, _T("&Operations"));
    wxSizer *sizerMiddle = new wxStaticBoxSizer(box2, wxVERTICAL);

    wxSizer *sizerRow = CreateSizerWithTextAndButton(TogglePage_ChangeLabel,
                                                     _T("Change label"),
                                                     wxID_ANY,
                                                     &m_textLabel);
    m_textLabel->SetValue(_T("&Toggle me!"));

    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    // right pane
    wxSizer *sizerRight = new wxBoxSizer(wxHORIZONTAL);

    m_toggle = new wxToggleButton(this, TogglePage_Picker, wxT("Toggle Button"));

    sizerRight->Add(0, 0, 1, wxCENTRE);
    sizerRight->Add(m_toggle, 1, wxCENTRE);
    sizerRight->Add(0, 0, 1, wxCENTRE);
    sizerRight->SetMinSize(150, 0);
    m_sizerToggle = sizerRight; // save it to modify it later

    // the 3 panes panes compose the window
//    sizerTop->Add(sizerLeft, 0, (wxALL & ~wxLEFT), 10);
    sizerTop->Add(sizerMiddle, 1, wxGROW | wxALL, 10);
    sizerTop->Add(sizerRight, 1, wxGROW | (wxALL & ~wxRIGHT), 10);

    // final initializations
    Reset();

    SetSizer(sizerTop);
}

void ToggleWidgetsPage::Reset()
{
    m_toggle->SetValue(false);
}

void ToggleWidgetsPage::CreateToggle()
{
    const bool value = m_toggle->GetValue();

    size_t count = m_sizerToggle->GetChildren().GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        m_sizerToggle->Remove(0);
    }

    delete m_toggle;

    m_toggle = new wxToggleButton(this, TogglePage_Picker, wxT("Toggle Button"));

    m_toggle->SetValue(value);

    m_sizerToggle->Add(0, 0, 1, wxCENTRE);
    m_sizerToggle->Add(m_toggle, 1, wxCENTRE);
    m_sizerToggle->Add(0, 0, 1, wxCENTRE);
    m_sizerToggle->Layout();
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void ToggleWidgetsPage::OnButtonReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();

    CreateToggle();
}

void ToggleWidgetsPage::OnButtonChangeLabel(wxCommandEvent& WXUNUSED(event))
{
    m_toggle->SetLabel(m_textLabel->GetValue());
}

#endif // wxUSE_TOGGLEBTN
