/////////////////////////////////////////////////////////////////////////////
// Program:     wxWindows Widgets Sample
// Name:        radiobox.cpp
// Purpose:     Part of the widgets sample showing wxRadioBox
// Author:      Vadim Zeitlin
// Created:     15.04.01
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
#endif

#include "wx/sizer.h"

#include "widgets.h"

#include "icons/radiobox.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    RadioPage_Reset = 100,
    RadioPage_Update,
    RadioPage_Selection,
    RadioPage_Label,
    RadioPage_LabelBtn,
    RadioPage_Radio
};

// ----------------------------------------------------------------------------
// RadioWidgetsPage
// ----------------------------------------------------------------------------

class RadioWidgetsPage : public WidgetsPage
{
public:
    RadioWidgetsPage(wxNotebook *notebook, wxImageList *imaglist);
    virtual ~RadioWidgetsPage();

protected:
    // event handlers
    void OnCheckOrRadioBox(wxCommandEvent& event);
    void OnRadioBox(wxCommandEvent& event);

    void OnButtonReset(wxCommandEvent& event);
    void OnButtonRecreate(wxCommandEvent& event);

    void OnButtonSelection(wxCommandEvent& event);
    void OnButtonSetLabel(wxCommandEvent& event);

    void OnUpdateUIUpdate(wxUpdateUIEvent& event);
    void OnUpdateUISelection(wxUpdateUIEvent& event);

    // reset the wxRadioBox parameters
    void Reset();

    // (re)create the wxRadioBox
    void CreateRadio();

    // the controls
    // ------------

    // the checkboxes for styles
    wxCheckBox *m_chkVert;

    // the gauge itself and the sizer it is in
    wxRadioBox *m_radio;
    wxSizer *m_sizerRadio;

    // the text entries for command parameters
    wxTextCtrl *m_textNumBtns,
               *m_textMajorDim,
               *m_textCurSel,
               *m_textSel,
               *m_textLabel,
               *m_textLabelBtns;

private:
    DECLARE_EVENT_TABLE();
    DECLARE_WIDGETS_PAGE(RadioWidgetsPage);
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(RadioWidgetsPage, WidgetsPage)
    EVT_BUTTON(RadioPage_Reset, RadioWidgetsPage::OnButtonReset)

    EVT_BUTTON(RadioPage_Update, RadioWidgetsPage::OnButtonRecreate)
    EVT_BUTTON(RadioPage_LabelBtn, RadioWidgetsPage::OnButtonRecreate)

    EVT_BUTTON(RadioPage_Selection, RadioWidgetsPage::OnButtonSelection)
    EVT_BUTTON(RadioPage_Label, RadioWidgetsPage::OnButtonSetLabel)

    EVT_UPDATE_UI(RadioPage_Update, RadioWidgetsPage::OnUpdateUIUpdate)
    EVT_UPDATE_UI(RadioPage_Selection, RadioWidgetsPage::OnUpdateUISelection)

    EVT_RADIOBOX(RadioPage_Radio, RadioWidgetsPage::OnRadioBox)

    EVT_CHECKBOX(-1, RadioWidgetsPage::OnCheckOrRadioBox)
    EVT_RADIOBOX(-1, RadioWidgetsPage::OnCheckOrRadioBox)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_WIDGETS_PAGE(RadioWidgetsPage, _T("Radio"));

RadioWidgetsPage::RadioWidgetsPage(wxNotebook *notebook,
                                       wxImageList *imaglist)
                  : WidgetsPage(notebook)
{
    imaglist->Add(wxBitmap(radio_xpm));

    // init everything
    m_chkVert = (wxCheckBox *)NULL;

    m_textNumBtns =
    m_textLabelBtns =
    m_textLabel = (wxTextCtrl *)NULL;

    m_radio = (wxRadioBox *)NULL;
    m_sizerRadio = (wxSizer *)NULL;

    wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    // left pane
    wxStaticBox *box = new wxStaticBox(this, -1, _T("&Set style"));

    wxSizer *sizerLeft = new wxStaticBoxSizer(box, wxVERTICAL);

    m_chkVert = CreateCheckBoxAndAddToSizer(sizerLeft, _T("&Vertical layout"));

    wxSizer *sizerRow;
    sizerRow = CreateSizerWithTextAndLabel(_T("&Major dimension"),
                                           -1,
                                           &m_textMajorDim);
    sizerLeft->Add(sizerRow, 0, wxGROW | wxALL, 5);

    sizerRow = CreateSizerWithTextAndLabel(_T("&Number of buttons"),
                                           -1,
                                           &m_textNumBtns);
    sizerLeft->Add(sizerRow, 0, wxGROW | wxALL, 5);

    wxButton *btn;
    btn = new wxButton(this, RadioPage_Update, _T("&Update"));
    sizerLeft->Add(btn, 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 5);

    sizerLeft->Add(5, 5, 0, wxGROW | wxALL, 5); // spacer

    btn = new wxButton(this, RadioPage_Reset, _T("&Reset"));
    sizerLeft->Add(btn, 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 15);

    // middle pane
    wxStaticBox *box2 = new wxStaticBox(this, -1, _T("&Change parameters"));
    wxSizer *sizerMiddle = new wxStaticBoxSizer(box2, wxVERTICAL);

    sizerRow = CreateSizerWithTextAndLabel(_T("Current selection"),
                                           -1,
                                           &m_textCurSel);
    sizerMiddle->Add(sizerRow, 0, wxGROW | wxALL, 5);

    sizerRow = CreateSizerWithTextAndButton(RadioPage_Selection,
                                            _T("&Change selection"),
                                           -1,
                                           &m_textSel);
    sizerMiddle->Add(sizerRow, 0, wxGROW | wxALL, 5);

    sizerRow = CreateSizerWithTextAndButton(RadioPage_Label,
                                            _T("&Label for box"),
                                            -1,
                                            &m_textLabel);
    sizerMiddle->Add(sizerRow, 0, wxGROW | wxALL, 5);

    sizerRow = CreateSizerWithTextAndButton(RadioPage_LabelBtn,
                                            _T("&Label for buttons"),
                                            -1,
                                            &m_textLabelBtns);
    sizerMiddle->Add(sizerRow, 0, wxGROW | wxALL, 5);

    // right pane
    wxSizer *sizerRight = new wxBoxSizer(wxHORIZONTAL);
    sizerRight->SetMinSize(250, 0);
    m_sizerRadio = sizerRight; // save it to modify it later

    Reset();
    CreateRadio();

    // the 3 panes panes compose the window
    sizerTop->Add(sizerLeft, 0, wxGROW | (wxALL & ~wxLEFT), 10);
    sizerTop->Add(sizerMiddle, 1, wxGROW | wxALL, 10);
    sizerTop->Add(sizerRight, 1, wxGROW | (wxALL & ~wxRIGHT), 10);

    // final initializations
    SetAutoLayout(TRUE);
    SetSizer(sizerTop);

    sizerTop->Fit(this);
}

RadioWidgetsPage::~RadioWidgetsPage()
{
}

// ----------------------------------------------------------------------------
// operations
// ----------------------------------------------------------------------------

void RadioWidgetsPage::Reset()
{
    m_textMajorDim->SetValue(_T("4"));
    m_textNumBtns->SetValue(_T("12"));
    m_textLabel->SetValue(_T("I'm a radiobox"));
    m_textLabelBtns->SetValue(_T("item"));

    m_chkVert->SetValue(FALSE);
}

void RadioWidgetsPage::CreateRadio()
{
    int sel;
    if ( m_radio )
    {
        sel = m_radio->GetSelection();

        m_sizerRadio->Remove(m_radio);

        delete m_radio;
    }
    else // first time creation, no old selection to preserve
    {
        sel = -1;
    }

    unsigned long count;
    if ( !m_textNumBtns->GetValue().ToULong(&count) )
    {
        wxLogWarning(_T("Should have a valid number for number of items."));

        // fall back to default
        count = 12;
    }

    unsigned long majorDim;
    if ( !m_textMajorDim->GetValue().ToULong(&majorDim) )
    {
        wxLogWarning(_T("Should have a valid major dimension number."));

        // fall back to default
        majorDim = 4;
    }

    wxString *items = new wxString[count];

    wxString labelBtn = m_textLabelBtns->GetValue();
    for ( size_t n = 0; n < count; n++ )
    {
        items[n] = wxString::Format(_T("%s %u"), labelBtn.c_str(), n + 1);
    }

    m_radio = new wxRadioBox(this, RadioPage_Radio,
                             m_textLabel->GetValue(),
                             wxDefaultPosition, wxDefaultSize,
                             count, items,
                             majorDim,
                             m_chkVert->GetValue() ? wxRA_VERTICAL
                                                   : wxRA_HORIZONTAL);

    delete [] items;

    if ( sel >= 0 && (size_t)sel < count )
    {
        m_radio->SetSelection(sel);
    }

    m_sizerRadio->Add(m_radio, 1, wxGROW);
    m_sizerRadio->Layout();
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void RadioWidgetsPage::OnButtonReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();

    CreateRadio();
}

void RadioWidgetsPage::OnCheckOrRadioBox(wxCommandEvent& event)
{
    CreateRadio();
}

void RadioWidgetsPage::OnRadioBox(wxCommandEvent& event)
{
    int sel = m_radio->GetSelection();

    wxLogMessage(_T("Radiobox selection changed, now %d"), sel);

    wxASSERT_MSG( sel == event.GetSelection(),
                  _T("selection should be the same in event and radiobox") );

    m_textCurSel->SetValue(wxString::Format(_T("%d"), sel));
}

void RadioWidgetsPage::OnButtonRecreate(wxCommandEvent& WXUNUSED(event))
{
    CreateRadio();
}

void RadioWidgetsPage::OnButtonSetLabel(wxCommandEvent& WXUNUSED(event))
{
    m_radio->wxControl::SetLabel(m_textLabel->GetValue());
}

void RadioWidgetsPage::OnButtonSelection(wxCommandEvent& WXUNUSED(event))
{
    unsigned long sel;
    if ( !m_textSel->GetValue().ToULong(&sel) ||
            (sel >= (size_t)m_radio->Number()) )
    {
        wxLogWarning(_T("Invalid number specified as new selection."));
    }
    else
    {
        m_radio->SetSelection(sel);
    }
}

void RadioWidgetsPage::OnUpdateUIUpdate(wxUpdateUIEvent& event)
{
    unsigned long n;
    event.Enable( m_textNumBtns->GetValue().ToULong(&n) &&
                  m_textMajorDim->GetValue().ToULong(&n) );
}

void RadioWidgetsPage::OnUpdateUISelection(wxUpdateUIEvent& event)
{
    unsigned long n;
    event.Enable( m_textSel->GetValue().ToULong(&n) &&
                   (n < (size_t)m_radio->Number()) );
}

