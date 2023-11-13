/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        button.cpp
// Purpose:     Part of the widgets sample showing wxButton
// Author:      Vadim Zeitlin
// Created:     10.04.01
// Copyright:   (c) 2001 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


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
#include "wx/commandlinkbutton.h"
#include "wx/valnum.h"

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
    ButtonPage_ChangeNote,
    ButtonPage_ChangeImageMargins,
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

    virtual wxWindow *GetWidget() const override { return m_button; }
    virtual void RecreateWidget() override { CreateButton(); }

    // lazy creation of the content
    virtual void CreateContent() override;

protected:
    // event handlers
    void OnCheckOrRadioBox(wxCommandEvent& event);

    void OnButton(wxCommandEvent& event);
    void OnButtonReset(wxCommandEvent& event);
    void OnButtonChangeLabel(wxCommandEvent& event);
    void OnButtonChangeNote(wxCommandEvent& event);
    void OnButtonChangeImageMargins(wxCommandEvent& event);

    // reset the wxButton parameters
    void Reset();

    // (re)create the wxButton
    void CreateButton();

    // helper function: create a bitmap bundle for wxBitmapButton
    wxBitmapBundle CreateBitmap(const wxString& label, const wxArtID& type);


    // the controls
    // ------------

    // the check/radio boxes for styles
    wxCheckBox *m_chkBitmapOnly,
               *m_chkTextAndBitmap,
               *m_chkFit,
               *m_chkAuthNeeded,
#if wxUSE_COMMANDLINKBUTTON
               *m_chkCommandLink,
#endif // wxUSE_COMMANDLINKBUTTON
#if wxUSE_MARKUP
               *m_chkUseMarkup,
#endif // wxUSE_MARKUP
               *m_chkDefault,
               *m_chkUseBitmapClass,
               *m_chkDisable;

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

#if wxUSE_COMMANDLINKBUTTON
    // same as m_button or nullptr if not showing a command link button currently
    wxCommandLinkButton *m_cmdLnkButton;
#endif // wxUSE_COMMANDLINKBUTTON

    wxSizer *m_sizerButton;

    // the text entries for command parameters
    wxTextCtrl *m_textLabel;

#if wxUSE_COMMANDLINKBUTTON
    wxTextCtrl *m_textNote;

    // used to hide or show button for changing note
    wxSizer *m_sizerNote;
#endif // wxUSE_COMMANDLINKBUTTON

    // the text entries for image margins
    wxTextCtrl* m_textImageMarginH;
    wxTextCtrl* m_textImageMarginV;

    int m_imageMarginH;
    int m_imageMarginV;

private:
    wxDECLARE_EVENT_TABLE();
    DECLARE_WIDGETS_PAGE(ButtonWidgetsPage)
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(ButtonWidgetsPage, WidgetsPage)
    EVT_BUTTON(ButtonPage_Button, ButtonWidgetsPage::OnButton)

    EVT_BUTTON(ButtonPage_Reset, ButtonWidgetsPage::OnButtonReset)
    EVT_BUTTON(ButtonPage_ChangeLabel, ButtonWidgetsPage::OnButtonChangeLabel)
    EVT_BUTTON(ButtonPage_ChangeNote, ButtonWidgetsPage::OnButtonChangeNote)
    EVT_BUTTON(ButtonPage_ChangeImageMargins, ButtonWidgetsPage::OnButtonChangeImageMargins)

    EVT_CHECKBOX(wxID_ANY, ButtonWidgetsPage::OnCheckOrRadioBox)
    EVT_RADIOBOX(wxID_ANY, ButtonWidgetsPage::OnCheckOrRadioBox)
wxEND_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

#if defined(__WXUNIVERSAL__)
    #define FAMILY_CTRLS UNIVERSAL_CTRLS
#else
    #define FAMILY_CTRLS NATIVE_CTRLS
#endif

IMPLEMENT_WIDGETS_PAGE(ButtonWidgetsPage, "Button", FAMILY_CTRLS );

ButtonWidgetsPage::ButtonWidgetsPage(WidgetsBookCtrl *book,
                                     wxImageList *imaglist)
                  : WidgetsPage(book, imaglist, button_xpm)
{
    // init everything
    m_chkBitmapOnly =
    m_chkTextAndBitmap =
    m_chkFit =
    m_chkAuthNeeded =
#if wxUSE_COMMANDLINKBUTTON
    m_chkCommandLink =
#endif // wxUSE_COMMANDLINKBUTTON
#if wxUSE_MARKUP
    m_chkUseMarkup =
#endif // wxUSE_MARKUP
    m_chkDefault =
    m_chkUseBitmapClass =
    m_chkDisable =
    m_chkUsePressed =
    m_chkUseFocused =
    m_chkUseCurrent =
    m_chkUseDisabled = nullptr;

    m_radioImagePos =
    m_radioHAlign =
    m_radioVAlign = nullptr;

    m_textLabel = nullptr;

    m_textImageMarginH = nullptr;
    m_textImageMarginV = nullptr;

    m_button = nullptr;
    m_sizerButton = nullptr;

    m_imageMarginH = 0;
    m_imageMarginV = 0;
}

void ButtonWidgetsPage::CreateContent()
{
    wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    // left pane
    wxStaticBoxSizer *sizerLeft = new wxStaticBoxSizer(wxVERTICAL, this, "&Set style");
    wxStaticBox* const sizerLeftBox = sizerLeft->GetStaticBox();

    m_chkBitmapOnly = CreateCheckBoxAndAddToSizer(sizerLeft, "&Bitmap only", wxID_ANY, sizerLeftBox);
    m_chkTextAndBitmap = CreateCheckBoxAndAddToSizer(sizerLeft, "Text &and bitmap", wxID_ANY, sizerLeftBox);
    m_chkFit = CreateCheckBoxAndAddToSizer(sizerLeft, "&Fit exactly", wxID_ANY, sizerLeftBox);
    m_chkAuthNeeded = CreateCheckBoxAndAddToSizer(sizerLeft, "Require a&uth", wxID_ANY, sizerLeftBox);
#if wxUSE_COMMANDLINKBUTTON
    m_chkCommandLink = CreateCheckBoxAndAddToSizer(sizerLeft, "Use command &link button", wxID_ANY, sizerLeftBox);
#endif
#if wxUSE_MARKUP
    m_chkUseMarkup = CreateCheckBoxAndAddToSizer(sizerLeft, "Interpret &markup", wxID_ANY, sizerLeftBox);
#endif // wxUSE_MARKUP
    m_chkDefault = CreateCheckBoxAndAddToSizer(sizerLeft, "&Default", wxID_ANY, sizerLeftBox);

    m_chkUseBitmapClass = CreateCheckBoxAndAddToSizer(sizerLeft,
        "Use wxBitmapButton", wxID_ANY, sizerLeftBox);
    m_chkUseBitmapClass->SetValue(true);

    m_chkDisable = CreateCheckBoxAndAddToSizer(sizerLeft, "Disable", wxID_ANY, sizerLeftBox);

    sizerLeft->AddSpacer(5);

    wxStaticBoxSizer *sizerUseLabels =
        new wxStaticBoxSizer(wxVERTICAL, sizerLeftBox,
                "&Use the following bitmaps in addition to the normal one?");
    wxStaticBox* const sizerUseLabelsBox = sizerUseLabels->GetStaticBox();

    m_chkUsePressed = CreateCheckBoxAndAddToSizer(sizerUseLabels,
        "&Pressed (small help icon)", wxID_ANY, sizerUseLabelsBox);
    m_chkUseFocused = CreateCheckBoxAndAddToSizer(sizerUseLabels,
        "&Focused (small error icon)", wxID_ANY, sizerUseLabelsBox);
    m_chkUseCurrent = CreateCheckBoxAndAddToSizer(sizerUseLabels,
        "&Current (small warning icon)", wxID_ANY, sizerUseLabelsBox);
    m_chkUseDisabled = CreateCheckBoxAndAddToSizer(sizerUseLabels,
        "&Disabled (broken image icon)", wxID_ANY, sizerUseLabelsBox);
    sizerLeft->Add(sizerUseLabels, wxSizerFlags().Expand().Border());

    sizerLeft->AddSpacer(10);

    static const wxString dirs[] =
    {
        "left", "right", "top", "bottom",
    };
    m_radioImagePos = new wxRadioBox(sizerLeftBox, wxID_ANY, "Image &position",
                                     wxDefaultPosition, wxDefaultSize,
                                     WXSIZEOF(dirs), dirs);
    sizerLeft->Add(m_radioImagePos, wxSizerFlags().Expand().Border());

    wxStaticBoxSizer* sizerImageMargins = new wxStaticBoxSizer(wxVERTICAL, sizerLeftBox, "Image margins");
    wxStaticBox* const sizerImageMarginsBox = sizerImageMargins->GetStaticBox();
    wxSizer* sizerImageMarginsRow = CreateSizerWithTextAndButton(ButtonPage_ChangeImageMargins,
                                           "Horizontal and vertical", wxID_ANY, &m_textImageMarginH,
                                            sizerImageMarginsBox);
    wxIntegerValidator<int> validatorMargH;
    validatorMargH.SetRange(0, 100);
    m_textImageMarginH->SetValidator(validatorMargH);

    wxIntegerValidator<int> validatorMargV;
    validatorMargV.SetRange(0, 100);
    m_textImageMarginV = new wxTextCtrl(sizerImageMarginsBox, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, validatorMargV);
    sizerImageMarginsRow->Add(m_textImageMarginV, wxSizerFlags(1).CentreVertical().Border(wxLEFT));

    m_textImageMarginH->SetValue(wxString::Format("%d", m_imageMarginH));
    m_textImageMarginV->SetValue(wxString::Format("%d", m_imageMarginV));

    sizerImageMargins->Add(sizerImageMarginsRow, wxSizerFlags().Border().Centre());
    sizerLeft->Add(sizerImageMargins, wxSizerFlags().Expand().Border());

    sizerLeft->AddSpacer(15);

    // should be in sync with enums Button[HV]Align!
    static const wxString halign[] =
    {
        "left",
        "centre",
        "right",
    };

    static const wxString valign[] =
    {
        "top",
        "centre",
        "bottom",
    };

    m_radioHAlign = new wxRadioBox(sizerLeftBox, wxID_ANY, "&Horz alignment",
                                   wxDefaultPosition, wxDefaultSize,
                                   WXSIZEOF(halign), halign);
    m_radioVAlign = new wxRadioBox(sizerLeftBox, wxID_ANY, "&Vert alignment",
                                   wxDefaultPosition, wxDefaultSize,
                                   WXSIZEOF(valign), valign);

    sizerLeft->Add(m_radioHAlign, wxSizerFlags().Expand().Border());
    sizerLeft->Add(m_radioVAlign, wxSizerFlags().Expand().Border());

    sizerLeft->AddSpacer(5);

    wxButton *btn = new wxButton(sizerLeftBox, ButtonPage_Reset, "&Reset");
    sizerLeft->Add(btn, wxSizerFlags().CentreHorizontal().TripleBorder(wxALL));

    // middle pane
    wxStaticBoxSizer *sizerMiddle = new wxStaticBoxSizer(wxVERTICAL, this, "&Operations");
    wxStaticBox* const sizerMiddleBox = sizerMiddle->GetStaticBox();

    wxSizer *sizerRow = CreateSizerWithTextAndButton(ButtonPage_ChangeLabel,
                                                     "Change label",
                                                     wxID_ANY,
                                                     &m_textLabel,
                                                     sizerMiddleBox);
    m_textLabel->SetValue("&Press me!");
    sizerMiddle->Add(sizerRow, wxSizerFlags().Expand().Border());

#if wxUSE_COMMANDLINKBUTTON
    m_sizerNote = CreateSizerWithTextAndButton(ButtonPage_ChangeNote,
                                               "Change note",
                                               wxID_ANY,
                                               &m_textNote,
                                               sizerMiddleBox);
    m_textNote->SetValue("Writes down button clicks in the log.");

    sizerMiddle->Add(m_sizerNote, wxSizerFlags().Expand().Border());
#endif

    // right pane
    m_sizerButton = new wxBoxSizer(wxHORIZONTAL);
    m_sizerButton->SetMinSize(FromDIP(150), 0);

    // the 3 panes panes compose the window
    sizerTop->Add(sizerLeft,
                  wxSizerFlags(0).Expand().DoubleBorder(wxALL & ~wxLEFT));
    sizerTop->Add(sizerMiddle,
                  wxSizerFlags(1).Expand().DoubleBorder(wxALL));
    sizerTop->Add(m_sizerButton,
                  wxSizerFlags(1).Expand().DoubleBorder((wxALL & ~wxRIGHT)));

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
    m_chkFit->SetValue(false);
    m_chkAuthNeeded->SetValue(false);
    m_chkTextAndBitmap->SetValue(false);
    m_chkDefault->SetValue(false);
#if wxUSE_COMMANDLINKBUTTON
    m_chkCommandLink->SetValue(false);
#endif
#if wxUSE_MARKUP
    m_chkUseMarkup->SetValue(false);
#endif // wxUSE_MARKUP
    m_chkUseBitmapClass->SetValue(true);
    m_chkDisable->SetValue(false);

    m_chkUsePressed->SetValue(true);
    m_chkUseFocused->SetValue(true);
    m_chkUseCurrent->SetValue(true);
    m_chkUseDisabled->SetValue(true);

    m_radioImagePos->SetSelection(ButtonImagePos_Left);
    m_radioHAlign->SetSelection(ButtonHAlign_Centre);
    m_radioVAlign->SetSelection(ButtonVAlign_Centre);

    m_imageMarginH = 0;
    m_imageMarginV = 0;
    m_textImageMarginH->SetValue(wxString::Format("%d", m_imageMarginH));
    m_textImageMarginV->SetValue(wxString::Format("%d", m_imageMarginV));
}

void ButtonWidgetsPage::CreateButton()
{
    wxString label;
    if ( m_button )
    {
#if wxUSE_COMMANDLINKBUTTON
        if ( m_cmdLnkButton )
            label = m_cmdLnkButton->GetMainLabel();
        else
#endif
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

    int flags = GetAttrs().m_defaultFlags;
    switch ( m_radioHAlign->GetSelection() )
    {
        case ButtonHAlign_Left:
            flags |= wxBU_LEFT;
            break;

        default:
            wxFAIL_MSG("unexpected radiobox selection");
            wxFALLTHROUGH;

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
            wxFAIL_MSG("unexpected radiobox selection");
            wxFALLTHROUGH;

        case ButtonVAlign_Centre:
            // centre vertical alignment is the default (no style)
            break;

        case ButtonVAlign_Bottom:
            flags |= wxBU_BOTTOM;
            break;
    }

    if ( m_chkFit->GetValue() )
    {
        flags |= wxBU_EXACTFIT;
    }

    bool showsBitmap = false;
    if ( m_chkBitmapOnly->GetValue() )
    {
#if wxUSE_COMMANDLINKBUTTON
        m_chkCommandLink->SetValue(false); // wxCommandLinkButton cannot be "Bitmap only"
#endif

        showsBitmap = true;

        wxButton *bbtn;
        if ( m_chkUseBitmapClass->GetValue() )
        {
          bbtn = new wxBitmapButton(this, ButtonPage_Button,
                                    CreateBitmap("normal", wxART_INFORMATION),
                                    wxDefaultPosition, wxDefaultSize, flags);
        }
        else
        {
          bbtn = new wxButton(this, ButtonPage_Button);
          bbtn->SetBitmapLabel(CreateBitmap("normal", wxART_INFORMATION));
        }
        bbtn->SetBitmapMargins((wxCoord)m_imageMarginH, (wxCoord)m_imageMarginV);

        if ( m_chkUsePressed->GetValue() )
            bbtn->SetBitmapPressed(CreateBitmap("pushed", wxART_HELP));
        if ( m_chkUseFocused->GetValue() )
            bbtn->SetBitmapFocus(CreateBitmap("focused", wxART_ERROR));
        if ( m_chkUseCurrent->GetValue() )
            bbtn->SetBitmapCurrent(CreateBitmap("hover", wxART_WARNING));
        if ( m_chkUseDisabled->GetValue() )
            bbtn->SetBitmapDisabled(CreateBitmap("disabled", wxART_MISSING_IMAGE));
        m_button = bbtn;
#if wxUSE_COMMANDLINKBUTTON
        m_cmdLnkButton = nullptr;
#endif
    }
    else // normal button
    {
#if wxUSE_COMMANDLINKBUTTON
        m_cmdLnkButton = nullptr;

        if ( m_chkCommandLink->GetValue() )
        {
            m_cmdLnkButton = new wxCommandLinkButton(this, ButtonPage_Button,
                                                     label,
                                                     m_textNote->GetValue(),
                                                     wxDefaultPosition,
                                                     wxDefaultSize,
                                                     flags);
            m_button = m_cmdLnkButton;
        }
        else
#endif // wxUSE_COMMANDLINKBUTTON
        {
            m_button = new wxButton(this, ButtonPage_Button, label,
                                    wxDefaultPosition, wxDefaultSize,
                                    flags);
        }
    }

#if wxUSE_COMMANDLINKBUTTON
    m_sizerNote->Show(m_chkCommandLink->GetValue());
#endif

    if ( !showsBitmap && m_chkTextAndBitmap->GetValue() )
    {
        showsBitmap = true;

        static const wxDirection positions[] =
        {
            wxLEFT, wxRIGHT, wxTOP, wxBOTTOM
        };

        m_button->SetBitmap(wxArtProvider::GetIcon(wxART_INFORMATION, wxART_BUTTON),
                            positions[m_radioImagePos->GetSelection()]);

        m_button->SetBitmapMargins((wxCoord)m_imageMarginH, (wxCoord)m_imageMarginV);

        if ( m_chkUsePressed->GetValue() )
            m_button->SetBitmapPressed(wxArtProvider::GetIcon(wxART_HELP, wxART_BUTTON));
        if ( m_chkUseFocused->GetValue() )
            m_button->SetBitmapFocus(wxArtProvider::GetIcon(wxART_ERROR, wxART_BUTTON));
        if ( m_chkUseCurrent->GetValue() )
            m_button->SetBitmapCurrent(wxArtProvider::GetIcon(wxART_WARNING, wxART_BUTTON));
        if ( m_chkUseDisabled->GetValue() )
            m_button->SetBitmapDisabled(wxArtProvider::GetIcon(wxART_MISSING_IMAGE, wxART_BUTTON));
    }

    m_chkTextAndBitmap->Enable(!m_chkBitmapOnly->IsChecked());
    m_chkBitmapOnly->Enable(!m_chkTextAndBitmap->IsChecked());
#if wxUSE_COMMANDLINKBUTTON
    m_chkCommandLink->Enable(!m_chkBitmapOnly->IsChecked());
#endif
    m_chkUseBitmapClass->Enable(showsBitmap);

    m_chkUsePressed->Enable(showsBitmap);
    m_chkUseFocused->Enable(showsBitmap);
    m_chkUseCurrent->Enable(showsBitmap);
    m_chkUseDisabled->Enable(showsBitmap);
    m_radioImagePos->Enable(m_chkTextAndBitmap->IsChecked());
    m_textImageMarginH->Enable(showsBitmap);
    m_textImageMarginV->Enable(showsBitmap);
    wxWindow::FindWindowById(ButtonPage_ChangeImageMargins)->Enable(showsBitmap);

    if ( m_chkAuthNeeded->GetValue() )
        m_button->SetAuthNeeded();

    if ( m_chkDefault->GetValue() )
        m_button->SetDefault();

    m_button->Enable(!m_chkDisable->IsChecked());

    m_sizerButton->AddStretchSpacer();
    m_sizerButton->Add(m_button, wxSizerFlags().Centre().Border());
    m_sizerButton->AddStretchSpacer();

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

void ButtonWidgetsPage::OnCheckOrRadioBox(wxCommandEvent& WXUNUSED(event))
{
    CreateButton();
    Layout(); // make sure the text field for changing note displays correctly.
}

void ButtonWidgetsPage::OnButtonChangeLabel(wxCommandEvent& WXUNUSED(event))
{
    const wxString labelText = m_textLabel->GetValue();

#if wxUSE_COMMANDLINKBUTTON
    if ( m_cmdLnkButton )
        m_cmdLnkButton->SetMainLabel(labelText);
    else
#endif // wxUSE_COMMANDLINKBUTTON
    {
#if wxUSE_MARKUP
        if ( m_chkUseMarkup->GetValue() )
            m_button->SetLabelMarkup(labelText);
        else
#endif // wxUSE_MARKUP
            m_button->SetLabel(labelText);
    }

    if ( m_chkBitmapOnly->IsChecked() )
        CreateButton();

    m_sizerButton->Layout();
}

void ButtonWidgetsPage::OnButtonChangeNote(wxCommandEvent& WXUNUSED(event))
{
#if wxUSE_COMMANDLINKBUTTON
    m_cmdLnkButton->SetNote(m_textNote->GetValue());

    m_sizerButton->Layout();
#endif // wxUSE_COMMANDLINKBUTTON
}

void ButtonWidgetsPage::OnButtonChangeImageMargins(wxCommandEvent& WXUNUSED(event))
{
    long margH = 0;
    long margV = 0;
    if ( !m_textImageMarginH->GetValue().ToLong(&margH) ||
         !m_textImageMarginV->GetValue().ToLong(&margV) ||
         margH < 0 || margV < 0 )
    {
        wxLogWarning("Invalid margin values for bitmap.");
        return;
    }

    m_imageMarginH = (int)margH;
    m_imageMarginV = (int)margV;

    m_button->SetBitmapMargins((wxCoord)m_imageMarginH, (wxCoord)m_imageMarginV);
    m_button->Refresh();
    m_sizerButton->Layout();
}

void ButtonWidgetsPage::OnButton(wxCommandEvent& WXUNUSED(event))
{
    wxLogMessage("Test button clicked.");
}

// ----------------------------------------------------------------------------
// bitmap button stuff
// ----------------------------------------------------------------------------

wxBitmapBundle
ButtonWidgetsPage::CreateBitmap(const wxString& label, const wxArtID& type)
{
    wxBitmap bmp(FromDIP(wxSize(180, 70))); // shouldn't hardcode but it's simpler like this
    wxMemoryDC dc;
    dc.SelectObject(bmp);
    dc.SetFont(GetFont());
    dc.SetBackground(*wxCYAN_BRUSH);
    dc.Clear();
    dc.SetTextForeground(*wxBLACK);
    dc.DrawLabel(wxStripMenuCodes(m_textLabel->GetValue()) + "\n"
                    "(" + label + " state)",
                 wxArtProvider::GetBitmap(type),
                 wxRect(10, 10, bmp.GetWidth() - 20, bmp.GetHeight() - 20),
                 wxALIGN_CENTRE);

    return bmp;
}
