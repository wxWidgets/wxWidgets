/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        radiobox.cpp
// Purpose:     Part of the widgets sample showing wxRadioBox
// Author:      Vadim Zeitlin
// Created:     15.04.01
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


#if wxUSE_RADIOBOX

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/log.h"

    #include "wx/bitmap.h"
    #include "wx/button.h"
    #include "wx/checkbox.h"
    #include "wx/radiobox.h"
    #include "wx/statbox.h"
    #include "wx/textctrl.h"
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
    RadioPage_Reset = wxID_HIGHEST,
    RadioPage_Update,
    RadioPage_Selection,
    RadioPage_Label,
    RadioPage_LabelBtn,
    RadioPage_EnableItem,
    RadioPage_ShowItem,
    RadioPage_Radio
};

// default values for the number of radiobox items
static const unsigned int DEFAULT_NUM_ENTRIES = 12;
static const unsigned int DEFAULT_MAJOR_DIM = 3;

// this item is enabled/disabled shown/hidden by the test checkboxes
static const int TEST_BUTTON = 1;

// ----------------------------------------------------------------------------
// RadioWidgetsPage
// ----------------------------------------------------------------------------

class RadioWidgetsPage : public WidgetsPage
{
public:
    RadioWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist);

    virtual wxWindow *GetWidget() const wxOVERRIDE { return m_radio; }
    virtual void RecreateWidget() wxOVERRIDE { CreateRadio(); }

    // lazy creation of the content
    virtual void CreateContent() wxOVERRIDE;

protected:
    // event handlers
    void OnCheckOrRadioBox(wxCommandEvent& event);
    void OnRadioBox(wxCommandEvent& event);

    void OnButtonReset(wxCommandEvent& event);
    void OnButtonRecreate(wxCommandEvent& event);

    void OnButtonSelection(wxCommandEvent& event);
    void OnButtonSetLabel(wxCommandEvent& event);

    void OnEnableItem(wxCommandEvent& event);
    void OnShowItem(wxCommandEvent& event);

    void OnUpdateUIReset(wxUpdateUIEvent& event);
    void OnUpdateUIUpdate(wxUpdateUIEvent& event);
    void OnUpdateUISelection(wxUpdateUIEvent& event);
    void OnUpdateUIEnableItem(wxUpdateUIEvent& event);
    void OnUpdateUIShowItem(wxUpdateUIEvent& event);

    // reset the wxRadioBox parameters
    void Reset();

    // (re)create the wxRadioBox
    void CreateRadio();

    // the controls
    // ------------

    // the check/radio boxes for styles
    wxCheckBox *m_chkSpecifyRows;
    wxCheckBox *m_chkEnableItem;
    wxCheckBox *m_chkShowItem;

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
    wxDECLARE_EVENT_TABLE();
    DECLARE_WIDGETS_PAGE(RadioWidgetsPage)
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(RadioWidgetsPage, WidgetsPage)
    EVT_BUTTON(RadioPage_Reset, RadioWidgetsPage::OnButtonReset)

    EVT_BUTTON(RadioPage_Update, RadioWidgetsPage::OnButtonRecreate)
    EVT_BUTTON(RadioPage_LabelBtn, RadioWidgetsPage::OnButtonRecreate)

    EVT_BUTTON(RadioPage_Selection, RadioWidgetsPage::OnButtonSelection)
    EVT_BUTTON(RadioPage_Label, RadioWidgetsPage::OnButtonSetLabel)

    EVT_UPDATE_UI(RadioPage_Reset, RadioWidgetsPage::OnUpdateUIReset)
    EVT_UPDATE_UI(RadioPage_Update, RadioWidgetsPage::OnUpdateUIUpdate)
    EVT_UPDATE_UI(RadioPage_Selection, RadioWidgetsPage::OnUpdateUISelection)

    EVT_RADIOBOX(RadioPage_Radio, RadioWidgetsPage::OnRadioBox)

    EVT_CHECKBOX(RadioPage_EnableItem, RadioWidgetsPage::OnEnableItem)
    EVT_CHECKBOX(RadioPage_ShowItem, RadioWidgetsPage::OnShowItem)

    EVT_UPDATE_UI(RadioPage_EnableItem, RadioWidgetsPage::OnUpdateUIEnableItem)
    EVT_UPDATE_UI(RadioPage_ShowItem, RadioWidgetsPage::OnUpdateUIShowItem)

    EVT_CHECKBOX(wxID_ANY, RadioWidgetsPage::OnCheckOrRadioBox)
    EVT_RADIOBOX(wxID_ANY, RadioWidgetsPage::OnCheckOrRadioBox)
wxEND_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

#if defined(__WXUNIVERSAL__)
    #define FAMILY_CTRLS UNIVERSAL_CTRLS
#else
    #define FAMILY_CTRLS NATIVE_CTRLS
#endif

IMPLEMENT_WIDGETS_PAGE(RadioWidgetsPage, "Radio",
                       FAMILY_CTRLS | WITH_ITEMS_CTRLS
                       );

RadioWidgetsPage::RadioWidgetsPage(WidgetsBookCtrl *book,
                                   wxImageList *imaglist)
                  : WidgetsPage(book, imaglist, radio_xpm)
{
    // init everything
    m_chkSpecifyRows = (wxCheckBox *)NULL;
    m_chkEnableItem = (wxCheckBox *)NULL;
    m_chkShowItem = (wxCheckBox *)NULL;

    m_textNumBtns =
    m_textLabelBtns =
    m_textLabel = (wxTextCtrl *)NULL;

    m_radio = (wxRadioBox *)NULL;
    m_sizerRadio = (wxSizer *)NULL;
}

void RadioWidgetsPage::CreateContent()
{
    wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    // left pane
    wxStaticBox *box = new wxStaticBox(this, wxID_ANY, "&Set style");

    wxSizer *sizerLeft = new wxStaticBoxSizer(box, wxVERTICAL);

    m_chkSpecifyRows = CreateCheckBoxAndAddToSizer
                       (
                        sizerLeft,
                        "Major specifies &rows count"
                       );

    wxSizer *sizerRow;
    sizerRow = CreateSizerWithTextAndLabel("&Major dimension:",
                                           wxID_ANY,
                                           &m_textMajorDim);
    sizerLeft->Add(sizerRow, wxSizerFlags().Expand().Border());

    sizerRow = CreateSizerWithTextAndLabel("&Number of buttons:",
                                           wxID_ANY,
                                           &m_textNumBtns);
    sizerLeft->Add(sizerRow, wxSizerFlags().Expand().Border());

    wxButton *btn;
    btn = new wxButton(this, RadioPage_Update, "&Update");
    sizerLeft->Add(btn, wxSizerFlags().CentreHorizontal().Border());

    sizerLeft->AddSpacer(5);

    btn = new wxButton(this, RadioPage_Reset, "&Reset");
    sizerLeft->Add(btn, wxSizerFlags().CentreHorizontal().Border(wxALL, 15));

    // middle pane
    wxStaticBox *box2 = new wxStaticBox(this, wxID_ANY, "&Change parameters");
    wxSizer *sizerMiddle = new wxStaticBoxSizer(box2, wxVERTICAL);

    sizerRow = CreateSizerWithTextAndLabel("Current selection:",
                                           wxID_ANY,
                                           &m_textCurSel);
    sizerMiddle->Add(sizerRow, wxSizerFlags().Expand().Border());

    sizerRow = CreateSizerWithTextAndButton(RadioPage_Selection,
                                            "&Change selection:",
                                           wxID_ANY,
                                           &m_textSel);
    sizerMiddle->Add(sizerRow, wxSizerFlags().Expand().Border());

    sizerRow = CreateSizerWithTextAndButton(RadioPage_Label,
                                            "&Label for box:",
                                            wxID_ANY,
                                            &m_textLabel);
    sizerMiddle->Add(sizerRow, wxSizerFlags().Expand().Border());

    sizerRow = CreateSizerWithTextAndButton(RadioPage_LabelBtn,
                                            "&Label for buttons:",
                                            wxID_ANY,
                                            &m_textLabelBtns);
    sizerMiddle->Add(sizerRow, wxSizerFlags().Expand().Border());

    m_chkEnableItem = CreateCheckBoxAndAddToSizer(sizerMiddle,
                                                  "Disable &2nd item",
                                                  RadioPage_EnableItem);
    m_chkShowItem = CreateCheckBoxAndAddToSizer(sizerMiddle,
                                                "Hide 2nd &item",
                                                RadioPage_ShowItem);

    // right pane
    wxSizer *sizerRight = new wxBoxSizer(wxHORIZONTAL);
    m_sizerRadio = sizerRight; // save it to modify it later

    Reset();
    CreateRadio();

    // the 3 panes panes compose the window
    sizerTop->Add(sizerLeft,
                  wxSizerFlags(0).Expand().Border((wxALL & ~wxLEFT), 10));
    sizerTop->Add(sizerMiddle,
                  wxSizerFlags(1).Expand().Border(wxALL, 10));
    sizerTop->Add(sizerRight,
                  wxSizerFlags(0).Expand().Border((wxALL & ~wxRIGHT), 10));

    // final initializations
    SetSizer(sizerTop);
}

// ----------------------------------------------------------------------------
// operations
// ----------------------------------------------------------------------------

void RadioWidgetsPage::Reset()
{
    m_textMajorDim->SetValue(wxString::Format("%u", DEFAULT_MAJOR_DIM));
    m_textNumBtns->SetValue(wxString::Format("%u", DEFAULT_NUM_ENTRIES));
    m_textLabel->SetValue("I'm a radiobox");
    m_textLabelBtns->SetValue("item");

    m_chkSpecifyRows->SetValue(false);
    m_chkEnableItem->SetValue(true);
    m_chkShowItem->SetValue(true);
}

void RadioWidgetsPage::CreateRadio()
{
    int sel;
    if ( m_radio )
    {
        sel = m_radio->GetSelection();

        m_sizerRadio->Detach( m_radio );

        delete m_radio;
    }
    else // first time creation, no old selection to preserve
    {
        sel = -1;
    }

    unsigned long count;
    if ( !m_textNumBtns->GetValue().ToULong(&count) )
    {
        wxLogWarning("Should have a valid number for number of items.");

        // fall back to default
        count = DEFAULT_NUM_ENTRIES;
    }

    unsigned long majorDim;
    if ( !m_textMajorDim->GetValue().ToULong(&majorDim) )
    {
        wxLogWarning("Should have a valid major dimension number.");

        // fall back to default
        majorDim = DEFAULT_MAJOR_DIM;
    }

    wxArrayString items;

    wxString labelBtn = m_textLabelBtns->GetValue();
    for ( size_t n = 0; n < count; n++ )
    {
        items.push_back(wxString::Format("%s %lu",
                                         labelBtn, (unsigned long)n + 1));
    }

    int flags = m_chkSpecifyRows->GetValue() ? wxRA_SPECIFY_ROWS
                                             : wxRA_SPECIFY_COLS;

    flags |= GetAttrs().m_defaultFlags;

    m_radio = new wxRadioBox(this, RadioPage_Radio,
                             m_textLabel->GetValue(),
                             wxDefaultPosition, wxDefaultSize,
                             items,
                             majorDim,
                             flags);

    if ( sel >= 0 && (size_t)sel < count )
    {
        m_radio->SetSelection(sel);
    }

    if ( count > TEST_BUTTON )
    {
        m_radio->Enable(TEST_BUTTON, m_chkEnableItem->IsChecked());
        m_radio->Show(TEST_BUTTON, m_chkShowItem->IsChecked());
    }

    m_sizerRadio->Add(m_radio, wxSizerFlags(1).Expand());
    Layout();

    m_chkEnableItem->Enable(count > TEST_BUTTON);
    m_chkShowItem->Enable(count > TEST_BUTTON);
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void RadioWidgetsPage::OnButtonReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();

    CreateRadio();
}

void RadioWidgetsPage::OnCheckOrRadioBox(wxCommandEvent& WXUNUSED(event))
{
    CreateRadio();
}

void RadioWidgetsPage::OnRadioBox(wxCommandEvent& event)
{
    int sel = m_radio->GetSelection();
    int event_sel = event.GetSelection();
    wxUnusedVar(event_sel);

    wxLogMessage("Radiobox selection changed, now %d", sel);

    wxASSERT_MSG( sel == event_sel,
                  "selection should be the same in event and radiobox" );

    m_textCurSel->SetValue(wxString::Format("%d", sel));
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
            (sel >= (size_t)m_radio->GetCount()) )
    {
        wxLogWarning("Invalid number specified as new selection.");
    }
    else
    {
        m_radio->SetSelection(sel);
    }
}

void RadioWidgetsPage::OnEnableItem(wxCommandEvent& event)
{
    m_radio->Enable(TEST_BUTTON, event.IsChecked());
}

void RadioWidgetsPage::OnShowItem(wxCommandEvent& event)
{
    m_radio->Show(TEST_BUTTON, event.IsChecked());
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
                   (n < (size_t)m_radio->GetCount()) );
}

void RadioWidgetsPage::OnUpdateUIReset(wxUpdateUIEvent& event)
{
    // only enable it if something is not set to default
    bool enable = m_chkSpecifyRows->GetValue();

    if ( !enable )
    {
        unsigned long numEntries;

        enable = !m_textNumBtns->GetValue().ToULong(&numEntries) ||
                    numEntries != DEFAULT_NUM_ENTRIES;

        if ( !enable )
        {
            unsigned long majorDim;

            enable = !m_textMajorDim->GetValue().ToULong(&majorDim) ||
                        majorDim != DEFAULT_MAJOR_DIM;
        }
    }

    event.Enable(enable);
}

void RadioWidgetsPage::OnUpdateUIEnableItem(wxUpdateUIEvent& event)
{
    if ( m_radio->GetCount() <= TEST_BUTTON )
        return;

    event.SetText(m_radio->IsItemEnabled(TEST_BUTTON) ? "Disable &2nd item"
                                                      : "Enable &2nd item");
}

void RadioWidgetsPage::OnUpdateUIShowItem(wxUpdateUIEvent& event)
{
    if ( m_radio->GetCount() <= TEST_BUTTON )
        return;

    event.SetText(m_radio->IsItemShown(TEST_BUTTON) ? "Hide 2nd &item"
                                                    : "Show 2nd &item");
}

#endif // wxUSE_RADIOBOX
