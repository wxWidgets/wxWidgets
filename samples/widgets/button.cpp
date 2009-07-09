/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
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
    #include "wx/app.h"
    #include "wx/log.h"

    #include "wx/bmpbuttn.h"
    #include "wx/button.h"
    #include "wx/checkbox.h"
    #include "wx/radiobox.h"
    #include "wx/statbox.h"
    #include "wx/textctrl.h"
#endif

#include "wx/artprov.h"
#include "wx/sizer.h"
#include "wx/dcmemory.h"

#include "widgets.h"

#include "icons/button.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    ButtonPage_Reset = wxID_HIGHEST,
    ButtonPage_ChangeLabel,
    ButtonPage_Button
};

// radio boxes
enum
{
    ButtonImagePos_Left,
    ButtonImagePos_Right,
    ButtonImagePos_Top,
    ButtonImagePos_Bottom
};

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
    ButtonWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist);
    virtual ~ButtonWidgetsPage(){};

    virtual wxControl *GetWidget() const { return m_button; }
    virtual void RecreateWidget() { CreateButton(); }

    // lazy creation of the content
    virtual void CreateContent();

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

    // add m_button to m_sizerButton using current value of m_chkFit
    void AddButtonToSizer();

    // helper function: create a bitmap for wxBitmapButton
    wxBitmap CreateBitmap(const wxString& label);


    // the controls
    // ------------

    // the check/radio boxes for styles
    wxCheckBox *m_chkBitmapOnly,
               *m_chkTextAndBitmap,
               *m_chkFit,
               *m_chkDefault;

    // more checkboxes for wxBitmapButton only
    wxCheckBox *m_chkUsePressed,
               *m_chkUseFocused,
               *m_chkUseCurrent,
               *m_chkUseDisabled;

    // and an image position choice used if m_chkTextAndBitmap is on
    wxRadioBox *m_radioImagePos;

    wxRadioBox *m_radioHAlign,
               *m_radioVAlign;

    // the button itself and the sizer it is in
    wxButton *m_button;
    wxSizer *m_sizerButton;

    // the text entries for command parameters
    wxTextCtrl *m_textLabel;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_WIDGETS_PAGE(ButtonWidgetsPage)
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ButtonWidgetsPage, WidgetsPage)
    EVT_BUTTON(ButtonPage_Button, ButtonWidgetsPage::OnButton)

    EVT_BUTTON(ButtonPage_Reset, ButtonWidgetsPage::OnButtonReset)
    EVT_BUTTON(ButtonPage_ChangeLabel, ButtonWidgetsPage::OnButtonChangeLabel)

    EVT_CHECKBOX(wxID_ANY, ButtonWidgetsPage::OnCheckOrRadioBox)
    EVT_RADIOBOX(wxID_ANY, ButtonWidgetsPage::OnCheckOrRadioBox)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

#if defined(__WXUNIVERSAL__)
    #define FAMILY_CTRLS UNIVERSAL_CTRLS
#else
    #define FAMILY_CTRLS NATIVE_CTRLS
#endif

IMPLEMENT_WIDGETS_PAGE(ButtonWidgetsPage, _T("Button"), FAMILY_CTRLS );

ButtonWidgetsPage::ButtonWidgetsPage(WidgetsBookCtrl *book,
                                     wxImageList *imaglist)
                  : WidgetsPage(book, imaglist, button_xpm)
{
    // init everything
    m_chkBitmapOnly =
    m_chkTextAndBitmap =
    m_chkFit =
    m_chkDefault =
    m_chkUsePressed =
    m_chkUseFocused =
    m_chkUseCurrent =
    m_chkUseDisabled = (wxCheckBox *)NULL;

    m_radioImagePos =
    m_radioHAlign =
    m_radioVAlign = (wxRadioBox *)NULL;

    m_textLabel = (wxTextCtrl *)NULL;

    m_button = (wxButton *)NULL;
    m_sizerButton = (wxSizer *)NULL;
}

void ButtonWidgetsPage::CreateContent()
{
    wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    // left pane
    wxStaticBox *box = new wxStaticBox(this, wxID_ANY, _T("&Set style"));

    wxSizer *sizerLeft = new wxStaticBoxSizer(box, wxVERTICAL);

    m_chkBitmapOnly = CreateCheckBoxAndAddToSizer(sizerLeft, "&Bitmap only");
    m_chkTextAndBitmap = CreateCheckBoxAndAddToSizer(sizerLeft, "Text &and &bitmap");
    m_chkFit = CreateCheckBoxAndAddToSizer(sizerLeft, _T("&Fit exactly"));
    m_chkDefault = CreateCheckBoxAndAddToSizer(sizerLeft, _T("&Default"));

    sizerLeft->AddSpacer(5);

    wxSizer *sizerUseLabels =
        new wxStaticBoxSizer(wxVERTICAL, this,
                "&Use the following bitmaps in addition to the normal one?");
    m_chkUsePressed = CreateCheckBoxAndAddToSizer(sizerUseLabels,
        "&Pressed (small help icon)");
    m_chkUseFocused = CreateCheckBoxAndAddToSizer(sizerUseLabels,
        "&Focused (small error icon)");
    m_chkUseCurrent = CreateCheckBoxAndAddToSizer(sizerUseLabels,
        "&Current (small warning icon)");
    m_chkUseDisabled = CreateCheckBoxAndAddToSizer(sizerUseLabels,
        "&Disabled (broken image icon)");
    sizerLeft->Add(sizerUseLabels, wxSizerFlags().Expand().Border());

    sizerLeft->AddSpacer(10);

    static const wxString dirs[] =
    {
        "left", "right", "top", "bottom",
    };
    m_radioImagePos = new wxRadioBox(this, wxID_ANY, "Image &position",
                                     wxDefaultPosition, wxDefaultSize,
                                     WXSIZEOF(dirs), dirs);
    sizerLeft->Add(m_radioImagePos, 0, wxGROW | wxALL, 5);
    sizerLeft->AddSpacer(15);

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

    m_radioHAlign = new wxRadioBox(this, wxID_ANY, _T("&Horz alignment"),
                                   wxDefaultPosition, wxDefaultSize,
                                   WXSIZEOF(halign), halign);
    m_radioVAlign = new wxRadioBox(this, wxID_ANY, _T("&Vert alignment"),
                                   wxDefaultPosition, wxDefaultSize,
                                   WXSIZEOF(valign), valign);

    sizerLeft->Add(m_radioHAlign, 0, wxGROW | wxALL, 5);
    sizerLeft->Add(m_radioVAlign, 0, wxGROW | wxALL, 5);

    sizerLeft->Add(5, 5, 0, wxGROW | wxALL, 5); // spacer

    wxButton *btn = new wxButton(this, ButtonPage_Reset, _T("&Reset"));
    sizerLeft->Add(btn, 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 15);

    // middle pane
    wxStaticBox *box2 = new wxStaticBox(this, wxID_ANY, _T("&Operations"));
    wxSizer *sizerMiddle = new wxStaticBoxSizer(box2, wxVERTICAL);

    wxSizer *sizerRow = CreateSizerWithTextAndButton(ButtonPage_ChangeLabel,
                                                     _T("Change label"),
                                                     wxID_ANY,
                                                     &m_textLabel);
    m_textLabel->SetValue(_T("&Press me!"));

    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    // right pane
    m_sizerButton = new wxBoxSizer(wxHORIZONTAL);
    m_sizerButton->SetMinSize(150, 0);

    // the 3 panes panes compose the window
    sizerTop->Add(sizerLeft, 0, wxGROW | (wxALL & ~wxLEFT), 10);
    sizerTop->Add(sizerMiddle, 1, wxGROW | wxALL, 10);
    sizerTop->Add(m_sizerButton, 1, wxGROW | (wxALL & ~wxRIGHT), 10);

    // do create the main control
    Reset();
    CreateButton();

    SetSizer(sizerTop);
}

// ----------------------------------------------------------------------------
// operations
// ----------------------------------------------------------------------------

void ButtonWidgetsPage::Reset()
{
    m_chkBitmapOnly->SetValue(false);
    m_chkFit->SetValue(true);
    m_chkTextAndBitmap->SetValue(false);
    m_chkDefault->SetValue(false);

    m_chkUsePressed->SetValue(true);
    m_chkUseFocused->SetValue(true);
    m_chkUseCurrent->SetValue(true);
    m_chkUseDisabled->SetValue(true);

    m_radioImagePos->SetSelection(ButtonImagePos_Left);
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
            m_sizerButton->Remove( 0 );
        }

        delete m_button;
    }

    if ( label.empty() )
    {
        // creating for the first time or recreating a button after bitmap
        // button
        label = m_textLabel->GetValue();
    }

    int flags = ms_defaultFlags;
    switch ( m_radioHAlign->GetSelection() )
    {
        case ButtonHAlign_Left:
            flags |= wxBU_LEFT;
            break;

        default:
            wxFAIL_MSG(_T("unexpected radiobox selection"));
            // fall through

        case ButtonHAlign_Centre:
            break;

        case ButtonHAlign_Right:
            flags |= wxBU_RIGHT;
            break;
    }

    switch ( m_radioVAlign->GetSelection() )
    {
        case ButtonVAlign_Top:
            flags |= wxBU_TOP;
            break;

        default:
            wxFAIL_MSG(_T("unexpected radiobox selection"));
            // fall through

        case ButtonVAlign_Centre:
            // centre vertical alignment is the default (no style)
            break;

        case ButtonVAlign_Bottom:
            flags |= wxBU_BOTTOM;
            break;
    }

    bool showsBitmap = false;
    if ( m_chkBitmapOnly->GetValue() )
    {
        showsBitmap = true;

        wxBitmapButton *bbtn = new wxBitmapButton(this, ButtonPage_Button,
                                                  CreateBitmap(_T("normal")));
        if ( m_chkUsePressed->GetValue() )
            bbtn->SetBitmapPressed(CreateBitmap(_T("pushed")));
        if ( m_chkUseFocused->GetValue() )
            bbtn->SetBitmapFocus(CreateBitmap(_T("focused")));
        if ( m_chkUseCurrent->GetValue() )
            bbtn->SetBitmapCurrent(CreateBitmap(_T("hover")));
        if ( m_chkUseDisabled->GetValue() )
            bbtn->SetBitmapDisabled(CreateBitmap(_T("disabled")));
        m_button = bbtn;
    }
    else // normal button
    {
        m_button = new wxButton(this, ButtonPage_Button, label,
                                wxDefaultPosition, wxDefaultSize,
                                flags);
    }

    if ( !showsBitmap && m_chkTextAndBitmap->GetValue() )
    {
        showsBitmap = true;

        static const wxDirection positions[] =
        {
            wxLEFT, wxRIGHT, wxTOP, wxBOTTOM
        };

        m_button->SetBitmap(wxArtProvider::GetIcon(wxART_INFORMATION),
                            positions[m_radioImagePos->GetSelection()]);

        if ( m_chkUsePressed->GetValue() )
            m_button->SetBitmapPressed(wxArtProvider::GetIcon(wxART_HELP));
        if ( m_chkUseFocused->GetValue() )
            m_button->SetBitmapFocus(wxArtProvider::GetIcon(wxART_ERROR));
        if ( m_chkUseCurrent->GetValue() )
            m_button->SetBitmapCurrent(wxArtProvider::GetIcon(wxART_WARNING));
        if ( m_chkUseDisabled->GetValue() )
            m_button->SetBitmapDisabled(wxArtProvider::GetIcon(wxART_MISSING_IMAGE));
    }

    m_chkUsePressed->Enable(showsBitmap);
    m_chkUseFocused->Enable(showsBitmap);
    m_chkUseCurrent->Enable(showsBitmap);
    m_chkUseDisabled->Enable(showsBitmap);

    if ( m_chkDefault->GetValue() )
    {
        m_button->SetDefault();
    }

    AddButtonToSizer();

    m_sizerButton->Layout();
}

void ButtonWidgetsPage::AddButtonToSizer()
{
    if ( m_chkFit->GetValue() )
    {
        m_sizerButton->AddStretchSpacer(1);
        m_sizerButton->Add(m_button, wxSizerFlags(0).Centre().Border());
        m_sizerButton->AddStretchSpacer(1);
    }
    else // take up the entire space
    {
        m_sizerButton->Add(m_button, wxSizerFlags(1).Expand().Border());
    }
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void ButtonWidgetsPage::OnButtonReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();

    CreateButton();
}

void ButtonWidgetsPage::OnCheckOrRadioBox(wxCommandEvent& WXUNUSED(event))
{
    CreateButton();
}

void ButtonWidgetsPage::OnButtonChangeLabel(wxCommandEvent& WXUNUSED(event))
{
    m_button->SetLabel(m_textLabel->GetValue());

    m_sizerButton->Layout();
}

void ButtonWidgetsPage::OnButton(wxCommandEvent& WXUNUSED(event))
{
    wxLogMessage(_T("Test button clicked."));
}

// ----------------------------------------------------------------------------
// bitmap button stuff
// ----------------------------------------------------------------------------

wxBitmap ButtonWidgetsPage::CreateBitmap(const wxString& label)
{
    wxBitmap bmp(180, 70); // shouldn't hardcode but it's simpler like this
    wxMemoryDC dc;
    dc.SelectObject(bmp);
    dc.SetBackground(wxBrush(*wxCYAN));
    dc.Clear();
    dc.SetTextForeground(*wxBLACK);
    dc.DrawLabel(wxStripMenuCodes(m_textLabel->GetValue()) + _T("\n")
                    _T("(") + label + _T(" state)"),
                 wxArtProvider::GetBitmap(wxART_INFORMATION),
                 wxRect(10, 10, bmp.GetWidth() - 20, bmp.GetHeight() - 20),
                 wxALIGN_CENTRE);

    return bmp;
}

