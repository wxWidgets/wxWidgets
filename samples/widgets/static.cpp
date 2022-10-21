/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        static.cpp
// Purpose:     Part of the widgets sample showing various static controls
// Author:      Vadim Zeitlin
// Created:     11.04.01
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
    #include "wx/log.h"

    #include "wx/bitmap.h"
    #include "wx/button.h"
    #include "wx/checkbox.h"
    #include "wx/radiobox.h"
    #include "wx/statbox.h"
    #include "wx/stattext.h"
    #include "wx/textctrl.h"
#endif

#include "wx/sizer.h"

#include "wx/statline.h"
#include "wx/generic/stattextg.h"
#include "wx/wupdlock.h"

#include "widgets.h"
#include "icons/statbox.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// alignment radiobox values
enum
{
    StaticHAlign_Left,
    StaticHAlign_Centre,
    StaticHAlign_Right,
    StaticHAlign_Max
};

enum
{
    StaticVAlign_Top,
    StaticVAlign_Centre,
    StaticVAlign_Bottom,
    StaticVAlign_Max
};

enum
{
    StaticEllipsize_Start,
    StaticEllipsize_Middle,
    StaticEllipsize_End
};


// ----------------------------------------------------------------------------
// StaticWidgetsPage
// ----------------------------------------------------------------------------

class StaticWidgetsPage : public WidgetsPage
{
public:
    StaticWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist);

    virtual wxWindow *GetWidget() const override { return m_statText; }
    virtual Widgets GetWidgets() const override
    {
        Widgets widgets;
        widgets.push_back(m_sizerStatBox->GetStaticBox());
        widgets.push_back(m_statText);
#if wxUSE_MARKUP
        widgets.push_back(m_statMarkup);
#endif // wxUSE_MARKUP
#if wxUSE_STATLINE
        widgets.push_back(m_statLine);
#endif // wxUSE_STATLINE

        return widgets;
    }
    virtual void RecreateWidget() override { CreateStatic(); }

    // lazy creation of the content
    virtual void CreateContent() override;

protected:
    // event handlers
    void OnRecreate(wxCommandEvent& WXUNUSED(event))
    {
        CreateStatic();
    }

    void OnCheckEllipsize(wxCommandEvent& event);
#ifdef wxHAS_WINDOW_LABEL_IN_STATIC_BOX
    void OnBoxCheckBox(wxCommandEvent& event);
#endif // wxHAS_WINDOW_LABEL_IN_STATIC_BOX

    void OnButtonReset(wxCommandEvent& event);
    void OnButtonBoxText(wxCommandEvent& event);
    void OnButtonLabelText(wxCommandEvent& event);
#if wxUSE_MARKUP
    void OnButtonLabelWithMarkupText(wxCommandEvent& event);
#endif // wxUSE_MARKUP
    void OnMouseEvent(wxMouseEvent& event);

    // reset all parameters
    void Reset();

    // (re)create all controls
    void CreateStatic();

    // the controls
    // ------------

    // the check/radio boxes for styles
    wxCheckBox *m_chkVert,
               *m_chkGeneric,
#ifdef wxHAS_WINDOW_LABEL_IN_STATIC_BOX
               *m_chkBoxWithCheck,
#endif // wxHAS_WINDOW_LABEL_IN_STATIC_BOX
               *m_chkAutoResize,
               *m_chkEllipsize;

#if wxUSE_MARKUP
    wxCheckBox *m_chkMarkup,
               *m_chkGreen;
#endif // wxUSE_MARKUP

    wxRadioBox *m_radioHAlign,
               *m_radioVAlign,
               *m_radioEllipsize;

    // the controls and the sizer containing them
    wxStaticBoxSizer *m_sizerStatBox;
    wxStaticTextBase *m_statText;

#if wxUSE_MARKUP
    wxStaticTextBase *m_statMarkup;
#endif // wxUSE_MARKUP

#if wxUSE_STATLINE
    wxStaticLine *m_statLine;
#endif // wxUSE_STATLINE
    wxSizer *m_sizerStatic;

    // the text entries for command parameters
    wxTextCtrl *m_textBox,
               *m_textLabel;

#if wxUSE_MARKUP
    wxTextCtrl *m_textLabelWithMarkup;
#endif // wxUSE_MARKUP

private:
    DECLARE_WIDGETS_PAGE(StaticWidgetsPage)
};

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_WIDGETS_PAGE(StaticWidgetsPage, "Static",
                       (int)wxPlatform(GENERIC_CTRLS).If(wxOS_WINDOWS,NATIVE_CTRLS)
                       );

StaticWidgetsPage::StaticWidgetsPage(WidgetsBookCtrl *book,
                                     wxImageList *imaglist)
                  : WidgetsPage(book, imaglist, statbox_xpm)
{
    // init everything
    m_chkVert =
    m_chkAutoResize =
    m_chkGeneric =
#ifdef wxHAS_WINDOW_LABEL_IN_STATIC_BOX
    m_chkBoxWithCheck =
#endif // wxHAS_WINDOW_LABEL_IN_STATIC_BOX
#if wxUSE_MARKUP
    m_chkGreen =
#endif // wxUSE_MARKUP
                nullptr;

    m_radioHAlign =
    m_radioVAlign = nullptr;

    m_statText = nullptr;
#if wxUSE_STATLINE
    m_statLine = nullptr;
#endif // wxUSE_STATLINE
#if wxUSE_MARKUP
    m_statMarkup = nullptr;
#endif // wxUSE_MARKUP

    m_sizerStatBox = nullptr;
    m_sizerStatic = nullptr;

    m_textBox =
    m_textLabel =
#if wxUSE_MARKUP
    m_textLabelWithMarkup =
#endif // wxUSE_MARKUP
                            nullptr;
}

void StaticWidgetsPage::CreateContent()
{
    wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    // left pane
    wxSizer *sizerLeft = new wxStaticBoxSizer(wxVERTICAL, this, "&Set style");

    m_chkGeneric = CreateCheckBoxAndAddToSizer(sizerLeft,
                                               "&Generic wxStaticText");
    m_chkGeneric->Bind(wxEVT_CHECKBOX, &StaticWidgetsPage::OnRecreate, this);

#ifdef wxHAS_WINDOW_LABEL_IN_STATIC_BOX
    m_chkBoxWithCheck = CreateCheckBoxAndAddToSizer(sizerLeft, "Checkable &box");
    m_chkBoxWithCheck->Bind(wxEVT_CHECKBOX, &StaticWidgetsPage::OnRecreate, this);
#endif // wxHAS_WINDOW_LABEL_IN_STATIC_BOX

    m_chkVert = CreateCheckBoxAndAddToSizer(sizerLeft, "&Vertical line");
    m_chkVert->Bind(wxEVT_CHECKBOX, &StaticWidgetsPage::OnRecreate, this);

    m_chkAutoResize = CreateCheckBoxAndAddToSizer(sizerLeft, "&Fit to text");
    m_chkAutoResize->Bind(wxEVT_CHECKBOX, &StaticWidgetsPage::OnRecreate, this);

    sizerLeft->Add(5, 5, 0, wxGROW | wxALL, 5); // spacer

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

    m_radioHAlign = new wxRadioBox(this, wxID_ANY, "&Horz alignment",
                                   wxDefaultPosition, wxDefaultSize,
                                   WXSIZEOF(halign), halign, 3);
    m_radioHAlign->Bind(wxEVT_RADIOBOX, &StaticWidgetsPage::OnRecreate, this);

    m_radioVAlign = new wxRadioBox(this, wxID_ANY, "&Vert alignment",
                                   wxDefaultPosition, wxDefaultSize,
                                   WXSIZEOF(valign), valign, 3);
    m_radioVAlign->SetToolTip("Relevant for Generic wxStaticText only");
    m_radioVAlign->Bind(wxEVT_RADIOBOX, &StaticWidgetsPage::OnRecreate, this);

    sizerLeft->Add(m_radioHAlign, 0, wxGROW | wxALL, 5);
    sizerLeft->Add(m_radioVAlign, 0, wxGROW | wxALL, 5);


    sizerLeft->Add(5, 5, 0, wxGROW | wxALL, 5); // spacer

    m_chkEllipsize = CreateCheckBoxAndAddToSizer(sizerLeft, "&Ellipsize");
    m_chkEllipsize->Bind(wxEVT_CHECKBOX,
                         &StaticWidgetsPage::OnCheckEllipsize, this);

    static const wxString ellipsizeMode[] =
    {
        "&start",
        "&middle",
        "&end",
    };

    m_radioEllipsize = new wxRadioBox(this, wxID_ANY, "&Ellipsize mode",
                                      wxDefaultPosition, wxDefaultSize,
                                      WXSIZEOF(ellipsizeMode), ellipsizeMode,
                                      3);
    m_radioEllipsize->Bind(wxEVT_RADIOBOX, &StaticWidgetsPage::OnRecreate, this);

    sizerLeft->Add(m_radioEllipsize, 0, wxGROW | wxALL, 5);

    wxButton *b0 = new wxButton(this, wxID_ANY, "&Reset");
    b0->Bind(wxEVT_BUTTON, &StaticWidgetsPage::OnButtonReset, this);
    sizerLeft->Add(b0, 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 15);

    // middle pane
    wxSizer *sizerMiddle = new wxStaticBoxSizer(wxVERTICAL, this,
                                                "&Change labels");

    m_textBox = new wxTextCtrl(this, wxID_ANY, wxEmptyString);
    wxButton *b1 = new wxButton(this, wxID_ANY, "Change &box label");
    b1->Bind(wxEVT_BUTTON, &StaticWidgetsPage::OnButtonBoxText, this);
    sizerMiddle->Add(m_textBox, 0, wxEXPAND|wxALL, 5);
    sizerMiddle->Add(b1, 0, wxLEFT|wxBOTTOM, 5);

    m_textLabel = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                                 wxDefaultPosition, wxDefaultSize,
                                 wxTE_MULTILINE|wxHSCROLL);
    wxButton *b2 = new wxButton(this, wxID_ANY, "Change &text label");
    b2->Bind(wxEVT_BUTTON, &StaticWidgetsPage::OnButtonLabelText, this);
    sizerMiddle->Add(m_textLabel, 0, wxEXPAND|wxALL, 5);
    sizerMiddle->Add(b2, 0, wxLEFT|wxBOTTOM, 5);

#if wxUSE_MARKUP
    m_textLabelWithMarkup = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                                           wxDefaultPosition, wxDefaultSize,
                                           wxTE_MULTILINE|wxHSCROLL);

    wxButton *b3 = new wxButton(this, wxID_ANY, "Change decorated text label");
    b3->Bind(wxEVT_BUTTON, &StaticWidgetsPage::OnButtonLabelWithMarkupText, this);
    sizerMiddle->Add(m_textLabelWithMarkup, 0, wxEXPAND|wxALL, 5);
    sizerMiddle->Add(b3, 0, wxLEFT|wxBOTTOM, 5);

    m_chkGreen = CreateCheckBoxAndAddToSizer(sizerMiddle,
                                             "Decorated label on g&reen");
    m_chkGreen->Bind(wxEVT_CHECKBOX, &StaticWidgetsPage::OnRecreate, this);
#endif // wxUSE_MARKUP

    // final initializations
    // NB: must be done _before_ calling CreateStatic()
    Reset();

    m_textBox->SetValue("This is a &box");
    m_textLabel->SetValue("And this is a\n\tlabel inside the box with a &mnemonic.\n"
                          "Only this text is affected by the ellipsize settings.");
#if wxUSE_MARKUP
    m_textLabelWithMarkup->SetValue("Another label, this time <b>decorated</b> "
                                    "with <u>markup</u>; here you need entities "
                                    "for the symbols: &lt; &gt; &amp;&amp; &apos; &quot; "
                                    " but you can still use &mnemonics too");
#endif // wxUSE_MARKUP

    // right pane
    wxSizer *sizerRight = new wxBoxSizer(wxHORIZONTAL);
    sizerRight->SetMinSize(150, 0);
    m_sizerStatic = sizerRight;

    CreateStatic();

    // the 3 panes panes compose the window
    sizerTop->Add(sizerLeft, 0, wxGROW | (wxALL & ~wxLEFT), 10);
    sizerTop->Add(sizerMiddle, 0, wxGROW | wxALL, 10);
    sizerTop->Add(sizerRight, 1, wxGROW | (wxALL & ~wxRIGHT), 10);

    SetSizer(sizerTop);
}

// ----------------------------------------------------------------------------
// operations
// ----------------------------------------------------------------------------

void StaticWidgetsPage::Reset()
{
    m_chkGeneric->SetValue(false);
#ifdef wxHAS_WINDOW_LABEL_IN_STATIC_BOX
    m_chkBoxWithCheck->SetValue(false);
#endif // wxHAS_WINDOW_LABEL_IN_STATIC_BOX
    m_chkVert->SetValue(false);
    m_chkAutoResize->SetValue(true);
    m_chkEllipsize->SetValue(true);

    m_radioHAlign->SetSelection(StaticHAlign_Left);
    m_radioVAlign->SetSelection(StaticVAlign_Top);
}

void StaticWidgetsPage::CreateStatic()
{
    wxWindowUpdateLocker lock(this);

    bool isVert = m_chkVert->GetValue();

    if ( m_sizerStatBox )
    {
        // delete m_sizerStatBox; -- deleted by Remove()
        m_sizerStatic->Remove(m_sizerStatBox);
        delete m_statText;
#if wxUSE_MARKUP
        delete m_statMarkup;
#endif // wxUSE_MARKUP
#if wxUSE_STATLINE
        delete m_statLine;
#endif // wxUSE_STATLINE
    }

    int flagsBox = 0,
        flagsText = GetAttrs().m_defaultFlags,
        flagsDummyText = GetAttrs().m_defaultFlags;

    if ( !m_chkAutoResize->GetValue() )
    {
        flagsText |= wxST_NO_AUTORESIZE;
        flagsDummyText |= wxST_NO_AUTORESIZE;
    }

    int align = 0;
    switch ( m_radioHAlign->GetSelection() )
    {
        default:
            wxFAIL_MSG("unexpected radiobox selection");
            wxFALLTHROUGH;

        case StaticHAlign_Left:
            align |= wxALIGN_LEFT;
            break;

        case StaticHAlign_Centre:
            align |= wxALIGN_CENTRE_HORIZONTAL;
            break;

        case StaticHAlign_Right:
            align |= wxALIGN_RIGHT;
            break;
    }

    switch ( m_radioVAlign->GetSelection() )
    {
        default:
            wxFAIL_MSG("unexpected radiobox selection");
            wxFALLTHROUGH;

        case StaticVAlign_Top:
            align |= wxALIGN_TOP;
            break;

        case StaticVAlign_Centre:
            align |= wxALIGN_CENTRE_VERTICAL;
            break;

        case StaticVAlign_Bottom:
            align |= wxALIGN_BOTTOM;
            break;
    }

    if ( m_chkEllipsize->GetValue() )
    {
        switch ( m_radioEllipsize->GetSelection() )
        {
            default:
                wxFAIL_MSG("unexpected radiobox selection");
                wxFALLTHROUGH;

            case StaticEllipsize_Start:
                flagsDummyText |= wxST_ELLIPSIZE_START;
                break;

            case StaticEllipsize_Middle:
                flagsDummyText |= wxST_ELLIPSIZE_MIDDLE;
                break;

            case StaticEllipsize_End:
                flagsDummyText |= wxST_ELLIPSIZE_END;
                break;
        }
    }

    flagsDummyText |= align;
    flagsText |= align;
    flagsBox |= align;

    wxStaticBox *staticBox;
#ifdef wxHAS_WINDOW_LABEL_IN_STATIC_BOX
    if ( m_chkBoxWithCheck->GetValue() )
    {
        wxCheckBox* const label = new wxCheckBox(this, wxID_ANY,
                                                 m_textBox->GetValue());
        label->Bind(wxEVT_CHECKBOX, &StaticWidgetsPage::OnBoxCheckBox, this);

        staticBox = new wxStaticBox(this, wxID_ANY,
                                    label,
                                    wxDefaultPosition, wxDefaultSize,
                                    flagsBox);
    }
    else // normal static box
#endif // wxHAS_WINDOW_LABEL_IN_STATIC_BOX
    {
        staticBox = new wxStaticBox(this, wxID_ANY,
                                    m_textBox->GetValue(),
                                    wxDefaultPosition, wxDefaultSize,
                                    flagsBox);
    }

    m_sizerStatBox = new wxStaticBoxSizer(staticBox, isVert ? wxHORIZONTAL
                                                            : wxVERTICAL);

    if ( m_chkGeneric->GetValue() )
    {
        m_statText = new wxGenericStaticText(staticBox, wxID_ANY,
                                             m_textLabel->GetValue(),
                                             wxDefaultPosition, wxDefaultSize,
                                             flagsDummyText);
#if wxUSE_MARKUP
        m_statMarkup = new wxGenericStaticText(staticBox, wxID_ANY,
                                             wxString(),
                                             wxDefaultPosition, wxDefaultSize,
                                             flagsText);
#endif // wxUSE_MARKUP
    }
    else // use native versions
    {
        m_statText = new wxStaticText(staticBox, wxID_ANY,
                                      m_textLabel->GetValue(),
                                      wxDefaultPosition, wxDefaultSize,
                                      flagsDummyText);
#if wxUSE_MARKUP
        m_statMarkup = new wxStaticText(staticBox, wxID_ANY,
                                        wxString(),
                                        wxDefaultPosition, wxDefaultSize,
                                        flagsText);
#endif // wxUSE_MARKUP
    }

    m_statText->SetToolTip("Tooltip for a label inside the box");

#if wxUSE_MARKUP
    m_statMarkup->SetLabelMarkup(m_textLabelWithMarkup->GetValue());

    if ( m_chkGreen->GetValue() )
        m_statMarkup->SetBackgroundColour(*wxGREEN);
#endif // wxUSE_MARKUP

#if wxUSE_STATLINE
    m_statLine = new wxStaticLine(staticBox, wxID_ANY,
                                  wxDefaultPosition, wxDefaultSize,
                                  isVert ? wxLI_VERTICAL : wxLI_HORIZONTAL);
#endif // wxUSE_STATLINE

    m_sizerStatBox->Add(m_statText, 0, wxGROW);
#if wxUSE_STATLINE
    m_sizerStatBox->Add(m_statLine, 0, wxGROW | wxTOP | wxBOTTOM, 10);
#endif // wxUSE_STATLINE
#if wxUSE_MARKUP
    m_sizerStatBox->Add(m_statMarkup);
#endif // wxUSE_MARKUP

    m_sizerStatic->Add(m_sizerStatBox, 0, wxGROW);

    m_sizerStatic->Layout();

    m_statText->Bind(wxEVT_LEFT_UP, &StaticWidgetsPage::OnMouseEvent, this);
    staticBox->Bind(wxEVT_LEFT_UP, &StaticWidgetsPage::OnMouseEvent, this);

    SetUpWidget();
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void StaticWidgetsPage::OnButtonReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();

    CreateStatic();
}

void StaticWidgetsPage::OnCheckEllipsize(wxCommandEvent& event)
{
    m_radioEllipsize->Enable(event.IsChecked());

    CreateStatic();
}

#ifdef wxHAS_WINDOW_LABEL_IN_STATIC_BOX
void StaticWidgetsPage::OnBoxCheckBox(wxCommandEvent& event)
{
    wxLogMessage("Box check box has been %schecked",
                 event.IsChecked() ? "": "un");
}
#endif // wxHAS_WINDOW_LABEL_IN_STATIC_BOX

void StaticWidgetsPage::OnButtonBoxText(wxCommandEvent& WXUNUSED(event))
{
    m_sizerStatBox->GetStaticBox()->SetLabel(m_textBox->GetValue());
}

void StaticWidgetsPage::OnButtonLabelText(wxCommandEvent& WXUNUSED(event))
{
    m_statText->SetLabel(m_textLabel->GetValue());

    // test GetLabel() and GetLabelText(); the first should return the
    // label as it is written in the relative text control; the second should
    // return the label as it's shown in the wxStaticText
    wxLogMessage("The original label should be '%s'",
                 m_statText->GetLabel());
    wxLogMessage("The label text is '%s'",
                 m_statText->GetLabelText());
}

#if wxUSE_MARKUP
void StaticWidgetsPage::OnButtonLabelWithMarkupText(wxCommandEvent& WXUNUSED(event))
{
    m_statMarkup->SetLabelMarkup(m_textLabelWithMarkup->GetValue());

    // test GetLabel() and GetLabelText(); the first should return the
    // label as it is written in the relative text control; the second should
    // return the label as it's shown in the wxStaticText
    wxLogMessage("The original label should be '%s'",
                 m_statMarkup->GetLabel());
    wxLogMessage("The label text is '%s'",
                 m_statMarkup->GetLabelText());
}
#endif // wxUSE_MARKUP

void StaticWidgetsPage::OnMouseEvent(wxMouseEvent& event)
{
    if ( event.GetEventObject() == m_statText )
    {
        wxLogMessage("Clicked on static text");
    }
    else
    {
        wxLogMessage("Clicked on static box");
    }
}

