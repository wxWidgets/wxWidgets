/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        spinbtn.cpp
// Purpose:     Part of the widgets sample showing wxSpinButton
// Author:      Vadim Zeitlin
// Created:     16.04.01
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


#if wxUSE_SPINBTN

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

#include "wx/spinbutt.h"
#include "wx/spinctrl.h"

#include "wx/sizer.h"
#include "wx/stattext.h"
#include "widgets.h"

#include "icons/spinbtn.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    SpinBtnPage_Reset = wxID_HIGHEST,
    SpinBtnPage_Clear,
    SpinBtnPage_SetValue,
    SpinBtnPage_SetMinAndMax,
    SpinBtnPage_SetBase,
    SpinBtnPage_SetIncrement,
    SpinBtnPage_CurValueText,
    SpinBtnPage_ValueText,
    SpinBtnPage_MinText,
    SpinBtnPage_MaxText,
    SpinBtnPage_BaseText,
    SpinBtnPage_SetIncrementText,
    SpinBtnPage_SpinBtn,
    SpinBtnPage_SpinCtrl,
    SpinBtnPage_SpinCtrlDouble
};

// alignment radiobox values
enum
{
    Align_Left,
    Align_Centre,
    Align_Right
};

// ----------------------------------------------------------------------------
// SpinBtnWidgetsPage
// ----------------------------------------------------------------------------

class SpinBtnWidgetsPage : public WidgetsPage
{
public:
    SpinBtnWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist);

    virtual wxWindow *GetWidget() const wxOVERRIDE { return m_spinbtn; }
    virtual Widgets GetWidgets() const wxOVERRIDE
    {
        Widgets widgets(WidgetsPage::GetWidgets());
        widgets.push_back(m_spinctrl);
        widgets.push_back(m_spinctrldbl);
        return widgets;
    }

    virtual void RecreateWidget() wxOVERRIDE { CreateSpin(); }

    // lazy creation of the content
    virtual void CreateContent() wxOVERRIDE;

protected:
    // event handlers
    void OnButtonReset(wxCommandEvent& event);
    void OnButtonClear(wxCommandEvent& event);
    void OnButtonSetValue(wxCommandEvent& event);
    void OnButtonSetMinAndMax(wxCommandEvent& event);
    void OnButtonSetBase(wxCommandEvent& event);
    void OnButtonSetIncrement(wxCommandEvent &event);
    void OnCheckOrRadioBox(wxCommandEvent& event);

    void OnSpinBtn(wxSpinEvent& event);
    void OnSpinBtnUp(wxSpinEvent& event);
    void OnSpinBtnDown(wxSpinEvent& event);
    void OnSpinCtrl(wxSpinEvent& event);
    void OnSpinCtrlDouble(wxSpinDoubleEvent& event);
    void OnSpinText(wxCommandEvent& event);
    void OnSpinTextEnter(wxCommandEvent& event);

    void OnUpdateUIValueButton(wxUpdateUIEvent& event);
    void OnUpdateUIMinMaxButton(wxUpdateUIEvent& event);
    void OnUpdateUIBaseButton(wxUpdateUIEvent& event);

    void OnUpdateUIResetButton(wxUpdateUIEvent& event);

    void OnUpdateUICurValueText(wxUpdateUIEvent& event);

    // reset the spinbtn parameters
    void Reset();

    // (re)create the spinbtn
    void CreateSpin();

    // is this spinbtn value in range?
    bool IsValidValue(int val) const
        { return (val >= m_min) && (val <= m_max); }

    // the spinbtn range
    int m_min, m_max;

    // and numeric base
    int m_base;

    // the increment
    int m_increment;

    // the controls
    // ------------

    // the check/radio boxes for styles
    wxCheckBox *m_chkVert,
               *m_chkArrowKeys,
               *m_chkWrap,
               *m_chkProcessEnter;
    wxRadioBox *m_radioAlign;

    // the spinbtn and the spinctrl and the sizer containing them
    wxSpinButton *m_spinbtn;
    wxSpinCtrl *m_spinctrl;
    wxSpinCtrlDouble *m_spinctrldbl;

    wxSizer *m_sizerSpin;

    // the text entries for set value/range
    wxTextCtrl *m_textValue,
               *m_textMin,
               *m_textMax,
               *m_textBase,
               *m_textIncrement;

private:
    wxDECLARE_EVENT_TABLE();
    DECLARE_WIDGETS_PAGE(SpinBtnWidgetsPage)
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(SpinBtnWidgetsPage, WidgetsPage)
    EVT_BUTTON(SpinBtnPage_Reset, SpinBtnWidgetsPage::OnButtonReset)
    EVT_BUTTON(SpinBtnPage_SetValue, SpinBtnWidgetsPage::OnButtonSetValue)
    EVT_BUTTON(SpinBtnPage_SetMinAndMax, SpinBtnWidgetsPage::OnButtonSetMinAndMax)
    EVT_BUTTON(SpinBtnPage_SetBase, SpinBtnWidgetsPage::OnButtonSetBase)
    EVT_BUTTON(SpinBtnPage_SetIncrement, SpinBtnWidgetsPage::OnButtonSetIncrement)

    EVT_UPDATE_UI(SpinBtnPage_SetValue, SpinBtnWidgetsPage::OnUpdateUIValueButton)
    EVT_UPDATE_UI(SpinBtnPage_SetMinAndMax, SpinBtnWidgetsPage::OnUpdateUIMinMaxButton)
    EVT_UPDATE_UI(SpinBtnPage_SetBase, SpinBtnWidgetsPage::OnUpdateUIBaseButton)

    EVT_UPDATE_UI(SpinBtnPage_Reset, SpinBtnWidgetsPage::OnUpdateUIResetButton)

    EVT_UPDATE_UI(SpinBtnPage_CurValueText, SpinBtnWidgetsPage::OnUpdateUICurValueText)

    EVT_SPIN(SpinBtnPage_SpinBtn, SpinBtnWidgetsPage::OnSpinBtn)
    EVT_SPIN_UP(SpinBtnPage_SpinBtn, SpinBtnWidgetsPage::OnSpinBtnUp)
    EVT_SPIN_DOWN(SpinBtnPage_SpinBtn, SpinBtnWidgetsPage::OnSpinBtnDown)
    EVT_SPINCTRL(SpinBtnPage_SpinCtrl, SpinBtnWidgetsPage::OnSpinCtrl)
    EVT_SPINCTRLDOUBLE(SpinBtnPage_SpinCtrlDouble, SpinBtnWidgetsPage::OnSpinCtrlDouble)
    EVT_TEXT(SpinBtnPage_SpinCtrl, SpinBtnWidgetsPage::OnSpinText)
    EVT_TEXT_ENTER(SpinBtnPage_SpinCtrl, SpinBtnWidgetsPage::OnSpinTextEnter)
    EVT_TEXT(SpinBtnPage_SpinCtrlDouble, SpinBtnWidgetsPage::OnSpinText)
    EVT_TEXT_ENTER(SpinBtnPage_SpinCtrlDouble, SpinBtnWidgetsPage::OnSpinTextEnter)

    EVT_CHECKBOX(wxID_ANY, SpinBtnWidgetsPage::OnCheckOrRadioBox)
    EVT_RADIOBOX(wxID_ANY, SpinBtnWidgetsPage::OnCheckOrRadioBox)
wxEND_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

#if defined(__WXUNIVERSAL__)
    #define FAMILY_CTRLS UNIVERSAL_CTRLS
#else
    #define FAMILY_CTRLS NATIVE_CTRLS
#endif

IMPLEMENT_WIDGETS_PAGE(SpinBtnWidgetsPage, "Spin",
                       FAMILY_CTRLS | EDITABLE_CTRLS
                       );

SpinBtnWidgetsPage::SpinBtnWidgetsPage(WidgetsBookCtrl *book,
                                       wxImageList *imaglist)
                  : WidgetsPage(book, imaglist, spinbtn_xpm)
{
    m_chkVert = NULL;
    m_chkArrowKeys = NULL;
    m_chkWrap = NULL;
    m_chkProcessEnter = NULL;
    m_radioAlign = NULL;
    m_spinbtn = NULL;
    m_spinctrl = NULL;
    m_spinctrldbl = NULL;
    m_textValue =
    m_textMin =
    m_textMax =
    m_textBase =
    m_textIncrement = NULL;

    m_min = 0;
    m_max = 10;

    m_base = 10;
    m_increment = 1;

    m_sizerSpin = NULL;
}

void SpinBtnWidgetsPage::CreateContent()
{
    wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    // left pane
    wxStaticBox *box = new wxStaticBox(this, wxID_ANY, "&Set style");
    wxSizer *sizerLeft = new wxStaticBoxSizer(box, wxVERTICAL);

    m_chkVert = CreateCheckBoxAndAddToSizer(sizerLeft, "&Vertical");
    m_chkArrowKeys = CreateCheckBoxAndAddToSizer(sizerLeft, "&Arrow Keys");
    m_chkWrap = CreateCheckBoxAndAddToSizer(sizerLeft, "&Wrap");
    m_chkProcessEnter = CreateCheckBoxAndAddToSizer(sizerLeft,
                                                    "Process &Enter");

    sizerLeft->Add(5, 5, 0, wxGROW | wxALL, 5); // spacer

    static const wxString halign[] =
    {
        "left",
        "centre",
        "right",
    };

    m_radioAlign = new wxRadioBox(this, wxID_ANY, "&Text alignment",
                                   wxDefaultPosition, wxDefaultSize,
                                   WXSIZEOF(halign), halign, 1);

    sizerLeft->Add(m_radioAlign, 0, wxGROW | wxALL, 5);

    sizerLeft->Add(5, 5, 0, wxGROW | wxALL, 5); // spacer

    wxButton *btn = new wxButton(this, SpinBtnPage_Reset, "&Reset");
    sizerLeft->Add(btn, 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 15);

    // middle pane
    wxStaticBox *box2 = new wxStaticBox(this, wxID_ANY,
        "&Change spinbtn value");

    wxSizer *sizerMiddle = new wxStaticBoxSizer(box2, wxVERTICAL);

    wxTextCtrl *text;
    wxSizer *sizerRow = CreateSizerWithTextAndLabel("Current value",
                                                    SpinBtnPage_CurValueText,
                                                    &text);
    text->SetEditable(false);

    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton(SpinBtnPage_SetValue,
                                            "Set &value",
                                            SpinBtnPage_ValueText,
                                            &m_textValue);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton(SpinBtnPage_SetMinAndMax,
                                            "&Min and max",
                                            SpinBtnPage_MinText,
                                            &m_textMin);

    m_textMax = new wxTextCtrl(this, SpinBtnPage_MaxText, wxEmptyString);
    sizerRow->Add(m_textMax, 1, wxLEFT | wxALIGN_CENTRE_VERTICAL, 5);

    m_textMin->SetValue( wxString::Format("%d", m_min) );
    m_textMax->SetValue( wxString::Format("%d", m_max) );

    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton(SpinBtnPage_SetBase,
                                            "Set &base",
                                            SpinBtnPage_BaseText,
                                            &m_textBase);
    m_textBase->SetValue("10");
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton( SpinBtnPage_SetIncrement,
                                             "Set Increment",
                                             SpinBtnPage_SetIncrementText,
                                             &m_textIncrement );
    m_textIncrement->SetValue( "1" );
    sizerMiddle->Add( sizerRow, 0, wxALL | wxGROW, 5 );

    // right pane
    wxSizer *sizerRight = new wxBoxSizer(wxVERTICAL);
    sizerRight->SetMinSize(150, 0);
    m_sizerSpin = sizerRight; // save it to modify it later

    Reset();
    CreateSpin();

    // the 3 panes panes compose the window
    sizerTop->Add(sizerLeft, 0, wxGROW | (wxALL & ~wxLEFT), 10);
    sizerTop->Add(sizerMiddle, 0, wxGROW | wxALL, 10);
    sizerTop->Add(sizerRight, 1, wxGROW | (wxALL & ~wxRIGHT), 10);

    // final initializations
    SetSizer(sizerTop);
}

// ----------------------------------------------------------------------------
// operations
// ----------------------------------------------------------------------------

void SpinBtnWidgetsPage::Reset()
{
    m_chkVert->SetValue(true);
    m_chkArrowKeys->SetValue(true);
    m_chkWrap->SetValue(false);
    m_chkProcessEnter->SetValue(false);
    m_radioAlign->SetSelection(Align_Right);
}

void SpinBtnWidgetsPage::CreateSpin()
{
    int flags = GetAttrs().m_defaultFlags;

    bool isVert = m_chkVert->GetValue();
    if ( isVert )
        flags |= wxSP_VERTICAL;
    else
        flags |= wxSP_HORIZONTAL;

    if ( m_chkArrowKeys->GetValue() )
        flags |= wxSP_ARROW_KEYS;

    if ( m_chkWrap->GetValue() )
        flags |= wxSP_WRAP;

    if ( m_chkProcessEnter->GetValue() )
        flags |= wxTE_PROCESS_ENTER;

    int textFlags = 0;
    switch ( m_radioAlign->GetSelection() )
    {
        default:
            wxFAIL_MSG("unexpected radiobox selection");
            wxFALLTHROUGH;

        case Align_Left:
            textFlags |= wxALIGN_LEFT;  // no-op
            break;

        case Align_Centre:
            textFlags |= wxALIGN_CENTRE_HORIZONTAL;
            break;

        case Align_Right:
            textFlags |= wxALIGN_RIGHT;
            break;
    }

    int val = m_min;
    if ( m_spinbtn )
    {
        int valOld = m_spinbtn->GetValue();
        if ( !IsValidValue(valOld) )
        {
            val = valOld;
        }

        m_sizerSpin->Clear(true /* delete windows */);
    }

    m_spinbtn = new wxSpinButton(this, SpinBtnPage_SpinBtn,
                                 wxDefaultPosition, wxDefaultSize,
                                 flags);

    m_spinbtn->SetValue(val);
    m_spinbtn->SetRange(m_min, m_max);

    m_spinctrl = new wxSpinCtrl(this, SpinBtnPage_SpinCtrl,
                                wxString::Format("%d", val),
                                wxDefaultPosition, wxDefaultSize,
                                flags | textFlags,
                                m_min, m_max, val);

    m_spinctrldbl = new wxSpinCtrlDouble(this, SpinBtnPage_SpinCtrlDouble,
                                         wxString::Format("%d", val),
                                         wxDefaultPosition, wxDefaultSize,
                                         flags | textFlags,
                                         m_min, m_max, val, 0.1);

    // Add spacers, labels and spin controls to the sizer.
    m_sizerSpin->Add(0, 0, 1);
    m_sizerSpin->Add(new wxStaticText(this, wxID_ANY, "wxSpinButton"),
                     0, wxALIGN_CENTRE | wxALL, 5);
    m_sizerSpin->Add(m_spinbtn, 0, wxALIGN_CENTRE | wxALL, 5);
    m_sizerSpin->Add(0, 0, 1);
    m_sizerSpin->Add(new wxStaticText(this, wxID_ANY, "wxSpinCtrl"),
                     0, wxALIGN_CENTRE | wxALL, 5);
    m_sizerSpin->Add(m_spinctrl, 0, wxALIGN_CENTRE | wxALL, 5);
    m_sizerSpin->Add(0, 0, 1);
    m_sizerSpin->Add(new wxStaticText(this, wxID_ANY, "wxSpinCtrlDouble"),
                     0, wxALIGN_CENTRE | wxALL, 5);
    m_sizerSpin->Add(m_spinctrldbl, 0, wxALIGN_CENTRE | wxALL, 5);
    m_sizerSpin->Add(0, 0, 1);

    m_sizerSpin->Layout();
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void SpinBtnWidgetsPage::OnButtonReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();

    CreateSpin();
}

void SpinBtnWidgetsPage::OnButtonSetMinAndMax(wxCommandEvent& WXUNUSED(event))
{
    long minNew,
         maxNew = 0; // init to suppress compiler warning
    if ( !m_textMin->GetValue().ToLong(&minNew) ||
         !m_textMax->GetValue().ToLong(&maxNew) ||
         minNew > maxNew )
    {
        wxLogWarning("Invalid min/max values for the spinbtn.");

        return;
    }

    m_min = minNew;
    m_max = maxNew;

    m_spinbtn->SetRange(minNew, maxNew);
    m_spinctrl->SetRange(minNew, maxNew);
    m_spinctrldbl->SetRange(minNew, maxNew);

    m_sizerSpin->Layout();
}

void SpinBtnWidgetsPage::OnButtonSetBase(wxCommandEvent& WXUNUSED(event))
{
    unsigned long base;
    if ( !m_textBase->GetValue().ToULong(&base) || !base )
    {
        wxLogWarning("Invalid base value.");
        return;
    }

    m_base = base;
    if ( !m_spinctrl->SetBase(m_base) )
    {
        wxLogWarning("Setting base %d failed.", m_base);
    }

    m_sizerSpin->Layout();
}

void SpinBtnWidgetsPage::OnButtonSetIncrement(wxCommandEvent& WXUNUSED (event))
{
    int increment = wxAtoi( m_textIncrement->GetValue() );
    if ( !increment )
    {
        wxLogWarning("Invalid increment value.");
        return;
    }

    m_increment = increment;
    m_spinctrl->SetIncrement(m_increment);
    wxLogWarning("Setting increment to %d.", m_increment);
}

void SpinBtnWidgetsPage::OnButtonSetValue(wxCommandEvent& WXUNUSED(event))
{
    if ( m_textValue->IsEmpty() )
    {
        m_spinctrl->SetValue( wxEmptyString );
        m_spinctrldbl->SetValue( wxEmptyString );

        return;
    }

    long val;
    if ( !m_textValue->GetValue().ToLong(&val) || !IsValidValue(val) )
    {
        wxLogWarning("Invalid spinbtn value.");

        return;
    }

    m_spinbtn->SetValue(val);
    m_spinctrl->SetValue(val);
    m_spinctrldbl->SetValue(val);
}

void SpinBtnWidgetsPage::OnUpdateUIValueButton(wxUpdateUIEvent& event)
{
    long val;
    event.Enable( m_textValue->IsEmpty() ||
                  ( m_textValue->GetValue().ToLong(&val) && IsValidValue(val) ) );
}

void SpinBtnWidgetsPage::OnUpdateUIMinMaxButton(wxUpdateUIEvent& event)
{
    long mn, mx;
    event.Enable( m_textMin->GetValue().ToLong(&mn) &&
                  m_textMax->GetValue().ToLong(&mx) &&
                  mn <= mx);
}

void SpinBtnWidgetsPage::OnUpdateUIBaseButton(wxUpdateUIEvent& event)
{
    unsigned long base;
    event.Enable( m_textBase->GetValue().ToULong(&base) && base );
}

void SpinBtnWidgetsPage::OnUpdateUIResetButton(wxUpdateUIEvent& event)
{
    event.Enable( !m_chkVert->GetValue() ||
                  m_chkWrap->GetValue() ||
                  m_chkProcessEnter->GetValue() );
}

void SpinBtnWidgetsPage::OnCheckOrRadioBox(wxCommandEvent& WXUNUSED(event))
{
    CreateSpin();
}

void SpinBtnWidgetsPage::OnUpdateUICurValueText(wxUpdateUIEvent& event)
{
    event.SetText( wxString::Format("%d", m_spinbtn->GetValue()));
}

void SpinBtnWidgetsPage::OnSpinBtn(wxSpinEvent& event)
{
    int value = event.GetInt();

    wxASSERT_MSG( value == m_spinbtn->GetValue(),
                  "spinbtn value should be the same" );

    wxLogMessage("Spin button value changed, now %d", value);
}

void SpinBtnWidgetsPage::OnSpinBtnUp(wxSpinEvent& event)
{
    // Demonstrate that these events can be vetoed to prevent the control value
    // from changing.
    if ( event.GetInt() == 11 )
    {
        wxLogMessage("Spin button prevented from going up to 11 (still %d)",
                     m_spinbtn->GetValue());
        event.Veto();
        return;
    }

    wxLogMessage( "Spin button value incremented, will be %d (was %d)",
                  event.GetInt(), m_spinbtn->GetValue() );
}

void SpinBtnWidgetsPage::OnSpinBtnDown(wxSpinEvent& event)
{
    // Also demonstrate that vetoing the event but then skipping the handler
    // doesn't actually apply the veto.
    if ( event.GetInt() == 0 )
    {
        wxLogMessage("Spin button change not effectively vetoed, will become 0");
        event.Veto();
        event.Skip();
    }

    wxLogMessage( "Spin button value decremented, will be %d (was %d)",
                  event.GetInt(), m_spinbtn->GetValue() );
}

void SpinBtnWidgetsPage::OnSpinCtrl(wxSpinEvent& event)
{
    int value = event.GetInt();

    wxASSERT_MSG( value == m_spinctrl->GetValue(),
                  "spinctrl value should be the same" );

    wxLogMessage("Spin control value changed, now %d", value);
}

void SpinBtnWidgetsPage::OnSpinCtrlDouble(wxSpinDoubleEvent& event)
{
    double value = event.GetValue();

    wxLogMessage("Spin control value changed, now %g", value);
}

void SpinBtnWidgetsPage::OnSpinText(wxCommandEvent& event)
{
    wxLogMessage("Text changed in spin control, now \"%s\"",
                 event.GetString());
}

void SpinBtnWidgetsPage::OnSpinTextEnter(wxCommandEvent& event)
{
    wxLogMessage("\"Enter\" pressed in spin control, text is \"%s\"",
                 event.GetString());
}

#endif // wxUSE_SPINBTN
