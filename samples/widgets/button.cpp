/////////////////////////////////////////////////////////////////////////////
// Program:     wxWindows Widgets Sample
// Name:        button.cpp
// Purpose:     Part of the widgets sample showing wxButton
// Author:      Vadim Zeitlin
// Created:     10.04.01
// Id:          $Id$
// Copyright:   (c) 2001 Vadim Zeitlin
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

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/checkbox.h"
    #include "wx/radiobox.h"

    #include "wx/button.h"
#endif

#include "wx/sizer.h"

#include "widgets.h"

#include "icons/button.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    ButtonPage_Reset = 100,
    ButtonPage_ChangeLabel,
    ButtonPage_Button
};

// radio boxes
enum
{
    ButtonHAlign_Left,
    ButtonHAlign_Centre,
    ButtonHAlign_Right
};

enum
{
    ButtonVAlign_Top,
    ButtonVAlign_Centre,
    ButtonVAlign_Bottom
};

// ----------------------------------------------------------------------------
// ButtonWidgetsPage
// ----------------------------------------------------------------------------

class ButtonWidgetsPage : public WidgetsPage
{
public:
    ButtonWidgetsPage(wxNotebook *notebook, wxImageList *imaglist);
    virtual ~ButtonWidgetsPage();

protected:
    // event handlers
    void OnCheckOrRadioBox(wxCommandEvent& event);

    void OnButton(wxCommandEvent& event);
    void OnButtonReset(wxCommandEvent& event);
    void OnButtonChangeLabel(wxCommandEvent& event);

    // reset the wxButton parameters
    void Reset();

    // (re)create the wxButton
    void CreateButton();

    // the controls
    // ------------

    // the check/radio boxes for styles
    wxCheckBox *m_chkImage,
               *m_chkFit,
               *m_chkDefault;

    wxRadioBox *m_radioHAlign,
               *m_radioVAlign;

    // the gauge itself and the sizer it is in
    wxButton *m_button;
    wxSizer *m_sizerButton;

    // the text entries for command parameters
    wxTextCtrl *m_textLabel;

private:
    DECLARE_EVENT_TABLE();
    DECLARE_WIDGETS_PAGE(ButtonWidgetsPage);
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ButtonWidgetsPage, WidgetsPage)
    EVT_BUTTON(ButtonPage_Button, ButtonWidgetsPage::OnButton)

    EVT_BUTTON(ButtonPage_Reset, ButtonWidgetsPage::OnButtonReset)
    EVT_BUTTON(ButtonPage_ChangeLabel, ButtonWidgetsPage::OnButtonChangeLabel)

    EVT_CHECKBOX(-1, ButtonWidgetsPage::OnCheckOrRadioBox)
    EVT_RADIOBOX(-1, ButtonWidgetsPage::OnCheckOrRadioBox)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_WIDGETS_PAGE(ButtonWidgetsPage, _T("Button"));

ButtonWidgetsPage::ButtonWidgetsPage(wxNotebook *notebook,
                                       wxImageList *imaglist)
                  : WidgetsPage(notebook)
{
    imaglist->Add(wxBitmap(button_xpm));

    // init everything
    m_chkImage =
    m_chkFit =
    m_chkDefault = (wxCheckBox *)NULL;

    m_radioHAlign =
    m_radioVAlign = (wxRadioBox *)NULL;

    m_textLabel = (wxTextCtrl *)NULL;

    m_button = (wxButton *)NULL;
    m_sizerButton = (wxSizer *)NULL;

    wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    // left pane
    wxStaticBox *box = new wxStaticBox(this, -1, _T("&Set style"));

    wxSizer *sizerLeft = new wxStaticBoxSizer(box, wxVERTICAL);

    m_chkImage = CreateCheckBoxAndAddToSizer(sizerLeft, _T("With &image"));
    m_chkFit = CreateCheckBoxAndAddToSizer(sizerLeft, _T("&Fit exactly"));
    m_chkDefault = CreateCheckBoxAndAddToSizer(sizerLeft, _T("&Default"));

    sizerLeft->Add(5, 5, 0, wxGROW | wxALL, 5); // spacer

    // should be in sync with enums Button[HV]Align!
    static const wxString halign[] =
    {
        _T("left"),
        _T("centre"),
        _T("right"),
    };

    static const wxString valign[] =
    {
        _T("top"),
        _T("centre"),
        _T("bottom"),
    };

    m_radioHAlign = new wxRadioBox(this, -1, _T("&Horz alignment"),
                                   wxDefaultPosition, wxDefaultSize,
                                   WXSIZEOF(halign), halign);
    m_radioVAlign = new wxRadioBox(this, -1, _T("&Vert alignment"),
                                   wxDefaultPosition, wxDefaultSize,
                                   WXSIZEOF(valign), valign);

    sizerLeft->Add(m_radioHAlign, 0, wxGROW | wxALL, 5);
    sizerLeft->Add(m_radioVAlign, 0, wxGROW | wxALL, 5);

    sizerLeft->Add(5, 5, 0, wxGROW | wxALL, 5); // spacer

    wxButton *btn = new wxButton(this, ButtonPage_Reset, _T("&Reset"));
    sizerLeft->Add(btn, 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 15);

    // middle pane
    wxStaticBox *box2 = new wxStaticBox(this, -1, _T("&Operations"));
    wxSizer *sizerMiddle = new wxStaticBoxSizer(box2, wxVERTICAL);

    wxSizer *sizerRow = CreateSizerWithTextAndButton(ButtonPage_ChangeLabel,
                                                     _T("Change label"),
                                                     -1,
                                                     &m_textLabel);

    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    // right pane
    wxSizer *sizerRight = new wxBoxSizer(wxHORIZONTAL);
    m_button = new wxButton(this, ButtonPage_Button, _T("&Press me!"));
    sizerRight->Add(0, 0, 1, wxCENTRE);
    sizerRight->Add(m_button, 1, wxCENTRE);
    sizerRight->Add(0, 0, 1, wxCENTRE);
    sizerRight->SetMinSize(250, 0);
    m_sizerButton = sizerRight; // save it to modify it later

    // the 3 panes panes compose the window
    sizerTop->Add(sizerLeft, 0, wxGROW | (wxALL & ~wxLEFT), 10);
    sizerTop->Add(sizerMiddle, 1, wxGROW | wxALL, 10);
    sizerTop->Add(sizerRight, 1, wxGROW | (wxALL & ~wxRIGHT), 10);

    // final initializations
    Reset();

    SetAutoLayout(TRUE);
    SetSizer(sizerTop);

    sizerTop->Fit(this);
}

ButtonWidgetsPage::~ButtonWidgetsPage()
{
}

// ----------------------------------------------------------------------------
// operations
// ----------------------------------------------------------------------------

void ButtonWidgetsPage::Reset()
{
    m_chkFit->SetValue(TRUE);
    m_chkImage->SetValue(FALSE);
    m_chkDefault->SetValue(FALSE);

    m_radioHAlign->SetSelection(ButtonHAlign_Centre);
    m_radioVAlign->SetSelection(ButtonVAlign_Centre);
}

void ButtonWidgetsPage::CreateButton()
{
    wxString label;
    if ( m_button )
    {
        label = m_button->GetLabel();

        size_t count = m_sizerButton->GetChildren().GetCount();
        for ( size_t n = 0; n < count; n++ )
        {
            m_sizerButton->Remove(0);
        }

        delete m_button;
    }
    else
    {
        label = _T("&Press me!");
    }

    int flags = 0;
    switch ( m_radioHAlign->GetSelection() )
    {
        case ButtonHAlign_Left:
            flags |= wxALIGN_LEFT;
            break;

        default:
            wxFAIL_MSG(_T("unexpected radiobox selection"));
            // fall through

        case ButtonHAlign_Centre:
            flags |= wxALIGN_CENTRE_HORIZONTAL;
            break;

        case ButtonHAlign_Right:
            flags |= wxALIGN_RIGHT;
            break;
    }

    switch ( m_radioVAlign->GetSelection() )
    {
        case ButtonVAlign_Top:
            flags |= wxALIGN_TOP;
            break;

        default:
            wxFAIL_MSG(_T("unexpected radiobox selection"));
            // fall through

        case ButtonVAlign_Centre:
            flags |= wxALIGN_CENTRE_VERTICAL;
            break;

        case ButtonVAlign_Bottom:
            flags |= wxALIGN_BOTTOM;
            break;
    }

    m_button = new wxButton(this, ButtonPage_Button, label,
                            wxDefaultPosition, wxDefaultSize,
                            flags);

    if ( m_chkImage->GetValue() )
    {
        m_button->SetImageLabel(wxTheApp->GetStdIcon(wxICON_INFORMATION));
    }

    if ( m_chkDefault->GetValue() )
    {
        m_button->SetDefault();
    }

    if ( m_chkFit->GetValue() )
    {
        m_sizerButton->Add(0, 0, 1, wxCENTRE);
        m_sizerButton->Add(m_button, 1, wxCENTRE);
        m_sizerButton->Add(0, 0, 1, wxCENTRE);
    }
    else
    {
        m_sizerButton->Add(m_button, 1, wxGROW | wxALL, 5);
    }

    m_sizerButton->Layout();
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void ButtonWidgetsPage::OnButtonReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();

    CreateButton();
}

void ButtonWidgetsPage::OnCheckOrRadioBox(wxCommandEvent& event)
{
    CreateButton();
}

void ButtonWidgetsPage::OnButtonChangeLabel(wxCommandEvent& WXUNUSED(event))
{
    m_button->SetLabel(m_textLabel->GetValue());
}

void ButtonWidgetsPage::OnButton(wxCommandEvent& event)
{
    wxLogMessage(_T("Test button clicked."));
}

