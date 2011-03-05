/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        static.cpp
// Purpose:     Part of the widgets sample showing various static controls
// Author:      Vadim Zeitlin
// Created:     11.04.01
// Id:          $Id$
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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

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

#include "widgets.h"
#include "icons/statbox.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    StaticPage_Reset = wxID_HIGHEST,
    StaticPage_BoxText,
    StaticPage_LabelText,
    StaticPage_LabelTextWithMarkup
};

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
    virtual ~StaticWidgetsPage(){};

    virtual wxControl *GetWidget() const { return m_statText; }
    virtual Widgets GetWidgets() const
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
    virtual void RecreateWidget() { CreateStatic(); }

    // lazy creation of the content
    virtual void CreateContent();

protected:
    // event handlers
    void OnCheckOrRadioBox(wxCommandEvent& event);

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
    DECLARE_EVENT_TABLE()
    DECLARE_WIDGETS_PAGE(StaticWidgetsPage)
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(StaticWidgetsPage, WidgetsPage)
    EVT_BUTTON(StaticPage_Reset, StaticWidgetsPage::OnButtonReset)
    EVT_BUTTON(StaticPage_LabelText, StaticWidgetsPage::OnButtonLabelText)
#if wxUSE_MARKUP
    EVT_BUTTON(StaticPage_LabelTextWithMarkup, StaticWidgetsPage::OnButtonLabelWithMarkupText)
#endif // wxUSE_MARKUP
    EVT_BUTTON(StaticPage_BoxText, StaticWidgetsPage::OnButtonBoxText)

    EVT_CHECKBOX(wxID_ANY, StaticWidgetsPage::OnCheckOrRadioBox)
    EVT_RADIOBOX(wxID_ANY, StaticWidgetsPage::OnCheckOrRadioBox)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_WIDGETS_PAGE(StaticWidgetsPage, wxT("Static"),
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
#if wxUSE_MARKUP
    m_chkGreen =
#endif // wxUSE_MARKUP
                NULL;

    m_radioHAlign =
    m_radioVAlign = (wxRadioBox *)NULL;

#if wxUSE_STATLINE
    m_statLine = (wxStaticLine *)NULL;
#endif // wxUSE_STATLINE
#if wxUSE_MARKUP
    m_statText = m_statMarkup = NULL;
#endif // wxUSE_MARKUP

    m_sizerStatBox = (wxStaticBoxSizer *)NULL;
    m_sizerStatic = (wxSizer *)NULL;

    m_textBox =
    m_textLabel =
#if wxUSE_MARKUP
    m_textLabelWithMarkup =
#endif // wxUSE_MARKUP
                            NULL;
}

void StaticWidgetsPage::CreateContent()
{
    wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    // left pane
    wxSizer *sizerLeft = new wxStaticBoxSizer(wxVERTICAL, this, "&Set style");

    m_chkGeneric = CreateCheckBoxAndAddToSizer(sizerLeft,
                                               "&Generic wxStaticText");
    m_chkVert = CreateCheckBoxAndAddToSizer(sizerLeft, "&Vertical line");
    m_chkAutoResize = CreateCheckBoxAndAddToSizer(sizerLeft, "&Fit to text");
    sizerLeft->Add(5, 5, 0, wxGROW | wxALL, 5); // spacer

    static const wxString halign[] =
    {
        wxT("left"),
        wxT("centre"),
        wxT("right"),
    };

    static const wxString valign[] =
    {
        wxT("top"),
        wxT("centre"),
        wxT("bottom"),
    };

    m_radioHAlign = new wxRadioBox(this, wxID_ANY, wxT("&Horz alignment"),
                                   wxDefaultPosition, wxDefaultSize,
                                   WXSIZEOF(halign), halign, 3);
    m_radioVAlign = new wxRadioBox(this, wxID_ANY, wxT("&Vert alignment"),
                                   wxDefaultPosition, wxDefaultSize,
                                   WXSIZEOF(valign), valign, 3);

    sizerLeft->Add(m_radioHAlign, 0, wxGROW | wxALL, 5);
    sizerLeft->Add(m_radioVAlign, 0, wxGROW | wxALL, 5);


    sizerLeft->Add(5, 5, 0, wxGROW | wxALL, 5); // spacer

    m_chkEllipsize = CreateCheckBoxAndAddToSizer(sizerLeft, wxT("&Ellipsize"));

    static const wxString ellipsizeMode[] =
    {
        wxT("&start"),
        wxT("&middle"),
        wxT("&end"),
    };

    m_radioEllipsize = new wxRadioBox(this, wxID_ANY, wxT("&Ellipsize mode"),
                                      wxDefaultPosition, wxDefaultSize,
                                      WXSIZEOF(ellipsizeMode), ellipsizeMode,
                                      3);

    sizerLeft->Add(m_radioEllipsize, 0, wxGROW | wxALL, 5);

    wxButton *btn = new wxButton(this, StaticPage_Reset, wxT("&Reset"));
    sizerLeft->Add(btn, 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 15);

    // middle pane
    wxSizer *sizerMiddle = new wxStaticBoxSizer(wxVERTICAL, this,
                                                "&Change labels");

    m_textBox = new wxTextCtrl(this, wxID_ANY, wxEmptyString);
    wxButton *b1 = new wxButton(this, wxID_ANY, "Change &box label");
    b1->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                wxCommandEventHandler(StaticWidgetsPage::OnButtonBoxText),
                NULL, this);
    sizerMiddle->Add(m_textBox, 0, wxEXPAND|wxALL, 5);
    sizerMiddle->Add(b1, 0, wxLEFT|wxBOTTOM, 5);

    m_textLabel = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                                 wxDefaultPosition, wxDefaultSize,
                                 wxTE_MULTILINE|wxHSCROLL);
    wxButton *b2 = new wxButton(this, wxID_ANY, "Change &text label");
    b2->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                wxCommandEventHandler(StaticWidgetsPage::OnButtonLabelText),
                NULL, this);
    sizerMiddle->Add(m_textLabel, 0, wxEXPAND|wxALL, 5);
    sizerMiddle->Add(b2, 0, wxLEFT|wxBOTTOM, 5);

#if wxUSE_MARKUP
    m_textLabelWithMarkup = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                                           wxDefaultPosition, wxDefaultSize,
                                           wxTE_MULTILINE|wxHSCROLL);

    wxButton *b3 = new wxButton(this, wxID_ANY, "Change decorated text label");
    b3->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                wxCommandEventHandler(StaticWidgetsPage::OnButtonLabelWithMarkupText),
                NULL, this);
    sizerMiddle->Add(m_textLabelWithMarkup, 0, wxEXPAND|wxALL, 5);
    sizerMiddle->Add(b3, 0, wxLEFT|wxBOTTOM, 5);

    m_chkGreen = CreateCheckBoxAndAddToSizer(sizerLeft,
                                             "Decorated label on g&reen");

    sizerMiddle->Add(m_chkGreen, 0, wxALL, 5);
#endif // wxUSE_MARKUP

    // final initializations
    // NB: must be done _before_ calling CreateStatic()
    Reset();

    m_textBox->SetValue(wxT("This is a &box"));
    m_textLabel->SetValue(wxT("And this is a\n\tlabel inside the box with a &mnemonic.\n")
                          wxT("Only this text is affected by the ellipsize settings."));
#if wxUSE_MARKUP
    m_textLabelWithMarkup->SetValue(wxT("Another label, this time <b>decorated</b> ")
                                    wxT("with <u>markup</u>; here you need entities ")
                                    wxT("for the symbols: &lt; &gt; &amp; &apos; &quot; ")
                                    wxT(" but you can still place &mnemonics..."));
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
    m_chkVert->SetValue(false);
    m_chkAutoResize->SetValue(true);
    m_chkEllipsize->SetValue(true);

    m_radioHAlign->SetSelection(StaticHAlign_Left);
    m_radioVAlign->SetSelection(StaticVAlign_Top);
}

void StaticWidgetsPage::CreateStatic()
{
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
        flagsText = ms_defaultFlags,
        flagsDummyText = ms_defaultFlags;

    if ( !m_chkAutoResize->GetValue() )
    {
        flagsText |= wxST_NO_AUTORESIZE;
        flagsDummyText |= wxST_NO_AUTORESIZE;
    }

    int align = 0;
    switch ( m_radioHAlign->GetSelection() )
    {
        default:
            wxFAIL_MSG(wxT("unexpected radiobox selection"));
            // fall through

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
            wxFAIL_MSG(wxT("unexpected radiobox selection"));
            // fall through

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
                wxFAIL_MSG(wxT("unexpected radiobox selection"));
                // fall through

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

    wxStaticBox *staticBox = new wxStaticBox(this, wxID_ANY,
                                             m_textBox->GetValue(),
                                             wxDefaultPosition, wxDefaultSize,
                                             flagsBox);
    m_sizerStatBox = new wxStaticBoxSizer(staticBox, isVert ? wxHORIZONTAL
                                                            : wxVERTICAL);

    if ( m_chkGeneric->GetValue() )
    {
        m_statText = new wxGenericStaticText(this, wxID_ANY,
                                             m_textLabel->GetValue(),
                                             wxDefaultPosition, wxDefaultSize,
                                             flagsDummyText);
#if wxUSE_MARKUP
        m_statMarkup = new wxGenericStaticText(this, wxID_ANY,
                                             wxString(),
                                             wxDefaultPosition, wxDefaultSize,
                                             flagsText);
#endif // wxUSE_MARKUP
    }
    else // use native versions
    {
        m_statText = new wxStaticText(this, wxID_ANY,
                                      m_textLabel->GetValue(),
                                      wxDefaultPosition, wxDefaultSize,
                                      flagsDummyText);
#if wxUSE_MARKUP
        m_statMarkup = new wxStaticText(this, wxID_ANY,
                                        wxString(),
                                        wxDefaultPosition, wxDefaultSize,
                                        flagsText);
#endif // wxUSE_MARKUP
    }

#if wxUSE_MARKUP
    m_statMarkup->SetLabelMarkup(m_textLabelWithMarkup->GetValue());

    if ( m_chkGreen->GetValue() )
        m_statMarkup->SetBackgroundColour(*wxGREEN);
#endif // wxUSE_MARKUP

#if wxUSE_STATLINE
    m_statLine = new wxStaticLine(this, wxID_ANY,
                                  wxDefaultPosition, wxDefaultSize,
                                  isVert ? wxLI_VERTICAL : wxLI_HORIZONTAL);
#endif // wxUSE_STATLINE

    m_sizerStatBox->Add(m_statText, 1, wxGROW | wxALL, 5);
#if wxUSE_STATLINE
    m_sizerStatBox->Add(m_statLine, 0, wxGROW | wxALL, 5);
#endif // wxUSE_STATLINE
#if wxUSE_MARKUP
    m_sizerStatBox->Add(m_statMarkup, 1, wxGROW | wxALL, 5);
#endif // wxUSE_MARKUP

    m_sizerStatic->Add(m_sizerStatBox, 1, wxGROW);

    m_sizerStatic->Layout();

    m_statText->Connect(wxEVT_LEFT_UP,
                        wxMouseEventHandler(StaticWidgetsPage::OnMouseEvent),
                        NULL, this);
    staticBox->Connect(wxEVT_LEFT_UP,
                       wxMouseEventHandler(StaticWidgetsPage::OnMouseEvent),
                       NULL, this);
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void StaticWidgetsPage::OnButtonReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();

    CreateStatic();
}

void StaticWidgetsPage::OnCheckOrRadioBox(wxCommandEvent& event)
{
    if (event.GetEventObject() == static_cast<wxObject*>(m_chkEllipsize))
    {
        m_radioEllipsize->Enable(event.IsChecked());
    }

    CreateStatic();
}

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
    wxLogMessage(wxT("The original label should be '%s'"),
                 m_statText->GetLabel());
    wxLogMessage(wxT("The label text is '%s'"),
                 m_statText->GetLabelText());
}

#if wxUSE_MARKUP
void StaticWidgetsPage::OnButtonLabelWithMarkupText(wxCommandEvent& WXUNUSED(event))
{
    m_statMarkup->SetLabelMarkup(m_textLabelWithMarkup->GetValue());

    // test GetLabel() and GetLabelText(); the first should return the
    // label as it is written in the relative text control; the second should
    // return the label as it's shown in the wxStaticText
    wxLogMessage(wxT("The original label should be '%s'"),
                 m_statMarkup->GetLabel());
    wxLogMessage(wxT("The label text is '%s'"),
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

