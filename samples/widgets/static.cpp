/////////////////////////////////////////////////////////////////////////////
// Program:     wxWindows Widgets Sample
// Name:        static.cpp
// Purpose:     Part of the widgets sample showing various static controls
// Author:      Vadim Zeitlin
// Created:     11.04.01
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

    #include "wx/statbox.h"
    #include "wx/stattext.h"
#endif

#include "wx/sizer.h"

#include "wx/statline.h"

#include "widgets.h"

#include "icons/statbox.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    StaticPage_Reset = 100,
    StaticPage_BoxText,
    StaticPage_LabelText
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

// ----------------------------------------------------------------------------
// StaticWidgetsPage
// ----------------------------------------------------------------------------

class StaticWidgetsPage : public WidgetsPage
{
public:
    StaticWidgetsPage(wxNotebook *notebook, wxImageList *imaglist);
    virtual ~StaticWidgetsPage();

protected:
    // event handlers
    void OnCheckOrRadioBox(wxCommandEvent& event);

    void OnButtonReset(wxCommandEvent& event);
    void OnButtonBoxText(wxCommandEvent& event);
    void OnButtonLabelText(wxCommandEvent& event);

    // reset all parameters
    void Reset();

    // (re)create all controls
    void CreateStatic();

    // the controls
    // ------------

    // the check/radio boxes for styles
    wxCheckBox *m_chkVert,
               *m_chkAutoResize;

    wxRadioBox *m_radioHAlign,
               *m_radioVAlign;

    // the controls and the sizer containing them
    wxStaticBoxSizer *m_sizerStatBox;
    wxStaticText *m_statText;
    wxStaticLine *m_statLine;
    wxSizer *m_sizerStatic;

    // the text entries for command parameters
    wxTextCtrl *m_textBox,
               *m_textLabel;

private:
    DECLARE_EVENT_TABLE();
    DECLARE_WIDGETS_PAGE(StaticWidgetsPage);
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(StaticWidgetsPage, WidgetsPage)
    EVT_BUTTON(StaticPage_Reset, StaticWidgetsPage::OnButtonReset)
    EVT_BUTTON(StaticPage_LabelText, StaticWidgetsPage::OnButtonLabelText)
    EVT_BUTTON(StaticPage_BoxText, StaticWidgetsPage::OnButtonBoxText)

    EVT_CHECKBOX(-1, StaticWidgetsPage::OnCheckOrRadioBox)
    EVT_RADIOBOX(-1, StaticWidgetsPage::OnCheckOrRadioBox)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_WIDGETS_PAGE(StaticWidgetsPage, _T("Static"));

StaticWidgetsPage::StaticWidgetsPage(wxNotebook *notebook,
                                       wxImageList *imaglist)
                  : WidgetsPage(notebook)
{
    imaglist->Add(wxBitmap(statbox_xpm));

    // init everything
    m_chkVert =
    m_chkAutoResize = (wxCheckBox *)NULL;

    m_radioHAlign =
    m_radioVAlign = (wxRadioBox *)NULL;

    m_statLine = (wxStaticLine *)NULL;
    m_statText = (wxStaticText *)NULL;

    m_sizerStatBox = (wxStaticBoxSizer *)NULL;
    m_sizerStatic = (wxSizer *)NULL;

    wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    // left pane
    wxStaticBox *box = new wxStaticBox(this, -1, _T("&Set style"));

    wxSizer *sizerLeft = new wxStaticBoxSizer(box, wxVERTICAL);

    m_chkVert = CreateCheckBoxAndAddToSizer(sizerLeft, _T("&Vertical line"));
    m_chkAutoResize = CreateCheckBoxAndAddToSizer(sizerLeft, _T("&Fit to text"));
    sizerLeft->Add(5, 5, 0, wxGROW | wxALL, 5); // spacer

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

    wxButton *btn = new wxButton(this, StaticPage_Reset, _T("&Reset"));
    sizerLeft->Add(btn, 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 15);

    // middle pane
    wxStaticBox *box2 = new wxStaticBox(this, -1, _T("&Change labels"));
    wxSizer *sizerMiddle = new wxStaticBoxSizer(box2, wxVERTICAL);

    wxSizer *sizerRow;

    sizerRow = CreateSizerWithTextAndButton(StaticPage_BoxText,
                                            _T("Change &box label"),
                                            -1, &m_textBox);
    sizerMiddle->Add(sizerRow, 0, wxGROW | wxALL, 5);

    sizerRow = CreateSizerWithTextAndButton(StaticPage_LabelText,
                                            _T("Change &text label"),
                                            -1, &m_textLabel);
    sizerMiddle->Add(sizerRow, 0, wxGROW | wxALL, 5);

    m_textBox->SetValue(_T("This is a box"));
    m_textLabel->SetValue(_T("And this is a label\ninside the box"));

    // right pane
    wxSizer *sizerRight = new wxBoxSizer(wxHORIZONTAL);
    sizerRight->SetMinSize(250, 0);
    m_sizerStatic = sizerRight;

    CreateStatic();

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

StaticWidgetsPage::~StaticWidgetsPage()
{
}

// ----------------------------------------------------------------------------
// operations
// ----------------------------------------------------------------------------

void StaticWidgetsPage::Reset()
{
    m_chkVert->SetValue(FALSE);
    m_chkAutoResize->SetValue(TRUE);

    m_radioHAlign->SetSelection(StaticHAlign_Left);
    m_radioVAlign->SetSelection(StaticVAlign_Top);
}

void StaticWidgetsPage::CreateStatic()
{
    bool isVert = m_chkVert->GetValue();

    if ( m_sizerStatBox )
    {
        m_sizerStatic->Remove(m_sizerStatBox);

        // delete m_sizerStatBox; -- deleted by Remove()
        delete m_statText;
        delete m_statLine;
    }

    int flagsBox = 0,
        flagsText = 0;

    if ( !m_chkAutoResize->GetValue() )
    {
        flagsText |= wxST_NO_AUTORESIZE;
    }

    int align = 0;
    switch ( m_radioHAlign->GetSelection() )
    {
        default:
            wxFAIL_MSG(_T("unexpected radiobox selection"));
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
            wxFAIL_MSG(_T("unexpected radiobox selection"));
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

    flagsText |= align;
    flagsBox |= align;

    wxStaticBox *box = new wxStaticBox(this, -1, m_textBox->GetValue(),
                                       wxDefaultPosition, wxDefaultSize,
                                       flagsBox);
    m_sizerStatBox = new wxStaticBoxSizer(box, isVert ? wxHORIZONTAL
                                                      : wxVERTICAL);

    m_statText = new wxStaticText(this, -1, m_textLabel->GetValue(),
                                  wxDefaultPosition, wxDefaultSize,
                                  flagsText);

    m_statLine = new wxStaticLine(this, -1,
                                  wxDefaultPosition, wxDefaultSize,
                                  isVert ? wxLI_VERTICAL : wxLI_HORIZONTAL);

    m_sizerStatBox->Add(m_statText, 1, wxGROW | wxALL, 5);
    m_sizerStatBox->Add(m_statLine, 0, wxGROW | wxALL, 5);
    m_sizerStatBox->Add(0, 0, 1);

    m_sizerStatic->Add(m_sizerStatBox, 1, wxGROW);

    m_sizerStatic->Layout();
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
    CreateStatic();
}

void StaticWidgetsPage::OnButtonBoxText(wxCommandEvent& event)
{
    m_sizerStatBox->GetStaticBox()->SetLabel(m_textBox->GetValue());
}

void StaticWidgetsPage::OnButtonLabelText(wxCommandEvent& event)
{
    m_statText->SetLabel(m_textLabel->GetValue());
}

