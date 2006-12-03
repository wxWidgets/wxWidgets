/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        spinbtn.cpp
// Purpose:     Part of the widgets sample showing wxSpinButton
// Author:      Vadim Zeitlin
// Created:     16.04.01
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
    SpinBtnPage_CurValueText,
    SpinBtnPage_ValueText,
    SpinBtnPage_MinText,
    SpinBtnPage_MaxText,
    SpinBtnPage_SpinBtn,
    SpinBtnPage_SpinCtrl
};

// ----------------------------------------------------------------------------
// SpinBtnWidgetsPage
// ----------------------------------------------------------------------------

class SpinBtnWidgetsPage : public WidgetsPage
{
public:
    SpinBtnWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist);
    virtual ~SpinBtnWidgetsPage(){};

    virtual wxControl *GetWidget() const { return m_spinbtn; }
    virtual wxControl *GetWidget2() const { return m_spinctrl; }
    virtual void RecreateWidget() { CreateSpin(); }

    // lazy creation of the content
    virtual void CreateContent();

protected:
    // event handlers
    void OnButtonReset(wxCommandEvent& event);
    void OnButtonClear(wxCommandEvent& event);
    void OnButtonSetValue(wxCommandEvent& event);
    void OnButtonSetMinAndMax(wxCommandEvent& event);

    void OnCheckOrRadioBox(wxCommandEvent& event);

    void OnSpinBtn(wxSpinEvent& event);
    void OnSpinBtnUp(wxSpinEvent& event);
    void OnSpinBtnDown(wxSpinEvent& event);
    void OnSpinCtrl(wxSpinEvent& event);

    void OnUpdateUIValueButton(wxUpdateUIEvent& event);
    void OnUpdateUIMinMaxButton(wxUpdateUIEvent& event);

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

    // the controls
    // ------------

    // the check/radio boxes for styles
    wxCheckBox *m_chkVert,
               *m_chkWrap;

    // the spinbtn and the spinctrl and the sizer containing them
    wxSpinButton *m_spinbtn;
    wxSpinCtrl *m_spinctrl;

    wxSizer *m_sizerSpin;

    // the text entries for set value/range
    wxTextCtrl *m_textValue,
               *m_textMin,
               *m_textMax;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_WIDGETS_PAGE(SpinBtnWidgetsPage)
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(SpinBtnWidgetsPage, WidgetsPage)
    EVT_BUTTON(SpinBtnPage_Reset, SpinBtnWidgetsPage::OnButtonReset)
    EVT_BUTTON(SpinBtnPage_SetValue, SpinBtnWidgetsPage::OnButtonSetValue)
    EVT_BUTTON(SpinBtnPage_SetMinAndMax, SpinBtnWidgetsPage::OnButtonSetMinAndMax)

    EVT_UPDATE_UI(SpinBtnPage_SetValue, SpinBtnWidgetsPage::OnUpdateUIValueButton)
    EVT_UPDATE_UI(SpinBtnPage_SetMinAndMax, SpinBtnWidgetsPage::OnUpdateUIMinMaxButton)

    EVT_UPDATE_UI(SpinBtnPage_Reset, SpinBtnWidgetsPage::OnUpdateUIResetButton)

    EVT_UPDATE_UI(SpinBtnPage_CurValueText, SpinBtnWidgetsPage::OnUpdateUICurValueText)

    EVT_SPIN(SpinBtnPage_SpinBtn, SpinBtnWidgetsPage::OnSpinBtn)
    EVT_SPIN_UP(SpinBtnPage_SpinBtn, SpinBtnWidgetsPage::OnSpinBtnUp)
    EVT_SPIN_DOWN(SpinBtnPage_SpinBtn, SpinBtnWidgetsPage::OnSpinBtnDown)
    EVT_SPINCTRL(SpinBtnPage_SpinCtrl, SpinBtnWidgetsPage::OnSpinCtrl)

    EVT_CHECKBOX(wxID_ANY, SpinBtnWidgetsPage::OnCheckOrRadioBox)
    EVT_RADIOBOX(wxID_ANY, SpinBtnWidgetsPage::OnCheckOrRadioBox)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

#if defined(__WXUNIVERSAL__)
    #define FAMILY_CTRLS UNIVERSAL_CTRLS
#else
    #define FAMILY_CTRLS NATIVE_CTRLS
#endif

IMPLEMENT_WIDGETS_PAGE(SpinBtnWidgetsPage, _T("Spin"),
                       FAMILY_CTRLS | EDITABLE_CTRLS
                       );

SpinBtnWidgetsPage::SpinBtnWidgetsPage(WidgetsBookCtrl *book,
                                       wxImageList *imaglist)
                  : WidgetsPage(book, imaglist, spinbtn_xpm)
{
    m_chkVert = NULL;
    m_chkWrap = NULL;
    m_spinbtn = NULL;
    m_spinctrl = NULL;
    m_textValue = NULL;
    m_textMin = NULL;
    m_textMax = NULL;

    // init everything
    m_min = 0;
    m_max = 10;

    m_chkVert =
    m_chkWrap = (wxCheckBox *)NULL;

    m_spinbtn = (wxSpinButton *)NULL;
    m_sizerSpin = (wxSizer *)NULL;
}

void SpinBtnWidgetsPage::CreateContent()
{
    wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    // left pane
    wxStaticBox *box = new wxStaticBox(this, wxID_ANY, _T("&Set style"));
    wxSizer *sizerLeft = new wxStaticBoxSizer(box, wxVERTICAL);

    m_chkVert = CreateCheckBoxAndAddToSizer(sizerLeft, _T("&Vertical"));
    m_chkWrap = CreateCheckBoxAndAddToSizer(sizerLeft, _T("&Wrap"));

    sizerLeft->Add(5, 5, 0, wxGROW | wxALL, 5); // spacer

    wxButton *btn = new wxButton(this, SpinBtnPage_Reset, _T("&Reset"));
    sizerLeft->Add(btn, 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 15);

    // middle pane
    wxStaticBox *box2 = new wxStaticBox(this, wxID_ANY,
        _T("&Change spinbtn value"));

    wxSizer *sizerMiddle = new wxStaticBoxSizer(box2, wxVERTICAL);

    wxTextCtrl *text;
    wxSizer *sizerRow = CreateSizerWithTextAndLabel(_T("Current value"),
                                                    SpinBtnPage_CurValueText,
                                                    &text);
    text->SetEditable(false);

    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton(SpinBtnPage_SetValue,
                                            _T("Set &value"),
                                            SpinBtnPage_ValueText,
                                            &m_textValue);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton(SpinBtnPage_SetMinAndMax,
                                            _T("&Min and max"),
                                            SpinBtnPage_MinText,
                                            &m_textMin);

    m_textMax = new wxTextCtrl(this, SpinBtnPage_MaxText, wxEmptyString);
    sizerRow->Add(m_textMax, 1, wxLEFT | wxALIGN_CENTRE_VERTICAL, 5);

    m_textMin->SetValue( wxString::Format(_T("%d"), m_min) );
    m_textMax->SetValue( wxString::Format(_T("%d"), m_max) );

    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

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
    m_chkWrap->SetValue(false);
}

void SpinBtnWidgetsPage::CreateSpin()
{
    int flags = ms_defaultFlags;

    bool isVert = m_chkVert->GetValue();
    if ( isVert )
        flags |= wxSP_VERTICAL;
    else
        flags |= wxSP_HORIZONTAL;

    if ( m_chkWrap->GetValue() )
        flags |= wxSP_WRAP;

    int val = m_min;
    if ( m_spinbtn )
    {
        int valOld = m_spinbtn->GetValue();
        if ( !IsValidValue(valOld) )
        {
            val = valOld;
        }

        m_sizerSpin->Detach( m_spinbtn );
        m_sizerSpin->Detach( m_spinctrl );

        // there are 3 spacers left
        m_sizerSpin->Remove( 0 );
        m_sizerSpin->Remove( 0 );
        m_sizerSpin->Remove( 0 );

        delete m_spinbtn;
        delete m_spinctrl;
    }

    m_spinbtn = new wxSpinButton(this, SpinBtnPage_SpinBtn,
                                 wxDefaultPosition, wxDefaultSize,
                                 flags);

    m_spinbtn->SetValue(val);
    m_spinbtn->SetRange(m_min, m_max);

    m_spinctrl = new wxSpinCtrl(this, SpinBtnPage_SpinCtrl,
                                wxString::Format(_T("%d"), val),
                                wxDefaultPosition, wxDefaultSize,
                                flags,
                                m_min, m_max, val);

    m_sizerSpin->Add(0, 0, 1);
    m_sizerSpin->Add(m_spinbtn, 0, wxALIGN_CENTRE | wxALL, 5);
    m_sizerSpin->Add(0, 0, 1);
    m_sizerSpin->Add(m_spinctrl, 0, wxALIGN_CENTRE | wxALL, 5);
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
        wxLogWarning(_T("Invalid min/max values for the spinbtn."));

        return;
    }

    m_min = minNew;
    m_max = maxNew;

    m_spinbtn->SetRange(minNew, maxNew);
    m_spinctrl->SetRange(minNew, maxNew);
}

void SpinBtnWidgetsPage::OnButtonSetValue(wxCommandEvent& WXUNUSED(event))
{
    long val;
    if ( !m_textValue->GetValue().ToLong(&val) || !IsValidValue(val) )
    {
        wxLogWarning(_T("Invalid spinbtn value."));

        return;
    }

    m_spinbtn->SetValue(val);
    m_spinctrl->SetValue(val);
}

void SpinBtnWidgetsPage::OnUpdateUIValueButton(wxUpdateUIEvent& event)
{
    long val;
    event.Enable( m_textValue->GetValue().ToLong(&val) && IsValidValue(val) );
}

void SpinBtnWidgetsPage::OnUpdateUIMinMaxButton(wxUpdateUIEvent& event)
{
    long mn, mx;
    event.Enable( m_textMin->GetValue().ToLong(&mn) &&
                  m_textMax->GetValue().ToLong(&mx) &&
                  mn <= mx);
}

void SpinBtnWidgetsPage::OnUpdateUIResetButton(wxUpdateUIEvent& event)
{
    event.Enable( !m_chkVert->GetValue() || m_chkWrap->GetValue() );
}

void SpinBtnWidgetsPage::OnCheckOrRadioBox(wxCommandEvent& WXUNUSED(event))
{
    CreateSpin();
}

void SpinBtnWidgetsPage::OnUpdateUICurValueText(wxUpdateUIEvent& event)
{
    event.SetText( wxString::Format(_T("%d"), m_spinbtn->GetValue()));
}

void SpinBtnWidgetsPage::OnSpinBtn(wxSpinEvent& event)
{
    int value = event.GetInt();

    wxASSERT_MSG( value == m_spinbtn->GetValue(),
                  _T("spinbtn value should be the same") );

    wxLogMessage(_T("Spin button value changed, now %d"), value);
}

void SpinBtnWidgetsPage::OnSpinBtnUp(wxSpinEvent& event)
{
    wxLogMessage( _T("Spin button value incremented, will be %d (was %d)"),
                  event.GetInt(), m_spinbtn->GetValue() );
}

void SpinBtnWidgetsPage::OnSpinBtnDown(wxSpinEvent& event)
{
    wxLogMessage( _T("Spin button value decremented, will be %d (was %d)"),
                  event.GetInt(), m_spinbtn->GetValue() );
}

void SpinBtnWidgetsPage::OnSpinCtrl(wxSpinEvent& event)
{
    if (!m_spinctrl)
        return;
    int value = event.GetInt();

    wxASSERT_MSG( value == m_spinctrl->GetValue(),
                  _T("spinctrl value should be the same") );

    wxLogMessage(_T("Spin control value changed, now %d"), value);
}
#endif
    // wxUSE_SPINBTN
