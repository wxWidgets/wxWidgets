/////////////////////////////////////////////////////////////////////////////
// Program:     wxWindows Widgets Sample
// Name:        gauge.cpp
// Purpose:     Part of the widgets sample showing wxGauge
// Author:      Vadim Zeitlin
// Created:     27.03.01
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
    #include "wx/gauge.h"
    #include "wx/timer.h"
#endif

#include "wx/sizer.h"

#include "widgets.h"

#include "icons/gauge.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    GaugePage_Reset = 100,
    GaugePage_Progress,
    GaugePage_Clear,
    GaugePage_SetValue,
    GaugePage_SetRange,
    GaugePage_CurValueText,
    GaugePage_ValueText,
    GaugePage_RangeText,
    GaugePage_Timer,
    GaugePage_Gauge
};

// ----------------------------------------------------------------------------
// GaugeWidgetsPage
// ----------------------------------------------------------------------------

class GaugeWidgetsPage : public WidgetsPage
{
public:
    GaugeWidgetsPage(wxNotebook *notebook, wxImageList *imaglist);
    virtual ~GaugeWidgetsPage();

protected:
    // event handlers
    void OnButtonReset(wxCommandEvent& event);
    void OnButtonProgress(wxCommandEvent& event);
    void OnButtonClear(wxCommandEvent& event);
    void OnButtonSetValue(wxCommandEvent& event);
    void OnButtonSetRange(wxCommandEvent& event);

    void OnCheckOrRadioBox(wxCommandEvent& event);

    void OnUpdateUIValueButton(wxUpdateUIEvent& event);
    void OnUpdateUIRangeButton(wxUpdateUIEvent& event);
    void OnUpdateUIResetButton(wxUpdateUIEvent& event);

    void OnUpdateUICurValueText(wxUpdateUIEvent& event);

    void OnProgressTimer(wxTimerEvent& event);

    // reset the gauge parameters
    void Reset();

    // (re)create the gauge
    void CreateGauge();

    // stop the progress timer
    void StopTimer();

    // the gauge range
    unsigned long m_range;

    // the controls
    // ------------

    // the checkboxes for styles
    wxCheckBox *m_chkVert,
               *m_chkSmooth;

    // the gauge itself and the sizer it is in
    wxGauge *m_gauge;
    wxSizer *m_sizerGauge;

    // the text entries for set value/range
    wxTextCtrl *m_textValue,
               *m_textRange;

    // the timer for simulating gauge progress
    wxTimer *m_timer;

private:
    DECLARE_EVENT_TABLE();
    DECLARE_WIDGETS_PAGE(GaugeWidgetsPage);
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(GaugeWidgetsPage, WidgetsPage)
    EVT_BUTTON(GaugePage_Reset, GaugeWidgetsPage::OnButtonReset)
    EVT_BUTTON(GaugePage_Progress, GaugeWidgetsPage::OnButtonProgress)
    EVT_BUTTON(GaugePage_Clear, GaugeWidgetsPage::OnButtonClear)
    EVT_BUTTON(GaugePage_SetValue, GaugeWidgetsPage::OnButtonSetValue)
    EVT_BUTTON(GaugePage_SetRange, GaugeWidgetsPage::OnButtonSetRange)

    EVT_UPDATE_UI(GaugePage_SetValue, GaugeWidgetsPage::OnUpdateUIValueButton)
    EVT_UPDATE_UI(GaugePage_SetRange, GaugeWidgetsPage::OnUpdateUIRangeButton)
    EVT_UPDATE_UI(GaugePage_Reset, GaugeWidgetsPage::OnUpdateUIResetButton)

    EVT_UPDATE_UI(GaugePage_CurValueText, GaugeWidgetsPage::OnUpdateUICurValueText)

    EVT_CHECKBOX(-1, GaugeWidgetsPage::OnCheckOrRadioBox)
    EVT_RADIOBOX(-1, GaugeWidgetsPage::OnCheckOrRadioBox)

    EVT_TIMER(GaugePage_Timer, GaugeWidgetsPage::OnProgressTimer)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_WIDGETS_PAGE(GaugeWidgetsPage, _T("Gauge"));

GaugeWidgetsPage::GaugeWidgetsPage(wxNotebook *notebook,
                                       wxImageList *imaglist)
                  : WidgetsPage(notebook)
{
    imaglist->Add(wxBitmap(gauge_xpm));

    // init everything
    m_range = 100;

    m_timer = (wxTimer *)NULL;

    m_chkVert =
    m_chkSmooth = (wxCheckBox *)NULL;

    m_gauge = (wxGauge *)NULL;
    m_sizerGauge = (wxSizer *)NULL;

    wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    // left pane
    wxStaticBox *box = new wxStaticBox(this, -1, _T("&Set style"));

    wxSizer *sizerLeft = new wxStaticBoxSizer(box, wxVERTICAL);

    m_chkVert = CreateCheckBoxAndAddToSizer(sizerLeft, _T("&Vertical"));
    m_chkSmooth = CreateCheckBoxAndAddToSizer(sizerLeft, _T("&Smooth"));

    sizerLeft->Add(5, 5, 0, wxGROW | wxALL, 5); // spacer

    wxButton *btn = new wxButton(this, GaugePage_Reset, _T("&Reset"));
    sizerLeft->Add(btn, 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 15);

    // middle pane
    wxStaticBox *box2 = new wxStaticBox(this, -1, _T("&Change gauge value"));
    wxSizer *sizerMiddle = new wxStaticBoxSizer(box2, wxVERTICAL);

    wxTextCtrl *text;
    wxSizer *sizerRow = CreateSizerWithTextAndLabel(_T("Current value"),
                                                    GaugePage_CurValueText,
                                                    &text);
    text->SetEditable(FALSE);

    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton(GaugePage_SetValue,
                                            _T("Set &value"),
                                            GaugePage_ValueText,
                                            &m_textValue);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton(GaugePage_SetRange,
                                            _T("Set &range"),
                                            GaugePage_RangeText,
                                            &m_textRange);
    m_textRange->SetValue(wxString::Format(_T("%lu"), m_range));
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, GaugePage_Progress, _T("Simulate &progress"));
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, GaugePage_Clear, _T("&Clear"));
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    // right pane
    wxSizer *sizerRight = new wxBoxSizer(wxHORIZONTAL);
    m_gauge = new wxGauge(this, GaugePage_Gauge, m_range);
    sizerRight->Add(m_gauge, 1, wxCENTRE | wxALL, 5);
    sizerRight->SetMinSize(250, 0);
    m_sizerGauge = sizerRight; // save it to modify it later

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

GaugeWidgetsPage::~GaugeWidgetsPage()
{
    delete m_timer;
}

// ----------------------------------------------------------------------------
// operations
// ----------------------------------------------------------------------------

void GaugeWidgetsPage::Reset()
{
    m_chkVert->SetValue(FALSE);
    m_chkSmooth->SetValue(FALSE);
}

void GaugeWidgetsPage::CreateGauge()
{
    int flags = 0;

    if ( m_chkVert->GetValue() )
        flags |= wxGA_VERTICAL;
    else
        flags |= wxGA_HORIZONTAL;

    if ( m_chkSmooth->GetValue() )
        flags |= wxGA_SMOOTH;

    int val = 0;
    if ( m_gauge )
    {
        val = m_gauge->GetValue();

        m_sizerGauge->Remove(m_gauge);
        delete m_gauge;
    }

    m_gauge = new wxGauge(this, GaugePage_Gauge, m_range,
                          wxDefaultPosition, wxDefaultSize,
                          flags);
    m_gauge->SetValue(val);

    if ( flags & wxGA_VERTICAL )
        m_sizerGauge->Add(m_gauge, 0, wxGROW | wxALL, 5);
    else
        m_sizerGauge->Add(m_gauge, 1, wxCENTRE | wxALL, 5);

    m_sizerGauge->Layout();
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void GaugeWidgetsPage::OnButtonReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();

    CreateGauge();
}

void GaugeWidgetsPage::OnButtonProgress(wxCommandEvent& event)
{
    if ( !m_timer )
    {
        static const int INTERVAL = 300;

        wxLogMessage(_T("Launched progress timer (interval = %d ms)"), INTERVAL);

        m_timer = new wxTimer(this, GaugePage_Timer);
        m_timer->Start(INTERVAL);

        wxButton *btn = (wxButton *)event.GetEventObject();
        btn->SetLabel(_T("&Stop timer"));
    }
    else // stop the running timer
    {
        StopTimer();

        wxLogMessage(_T("Stopped the timer."));
    }
}

void GaugeWidgetsPage::OnButtonClear(wxCommandEvent& WXUNUSED(event))
{
    m_gauge->SetValue(0);
}

void GaugeWidgetsPage::OnButtonSetRange(wxCommandEvent& WXUNUSED(event))
{
    unsigned long val;
    if ( !m_textRange->GetValue().ToULong(&val) )
        return;

    m_gauge->SetRange(val);
}

void GaugeWidgetsPage::OnButtonSetValue(wxCommandEvent& WXUNUSED(event))
{
    unsigned long val;
    if ( !m_textValue->GetValue().ToULong(&val) )
        return;

    m_gauge->SetValue(val);
}

void GaugeWidgetsPage::OnUpdateUIValueButton(wxUpdateUIEvent& event)
{
    unsigned long val;
    event.Enable( m_textValue->GetValue().ToULong(&val) && (val < m_range) );
}

void GaugeWidgetsPage::OnUpdateUIRangeButton(wxUpdateUIEvent& event)
{
    unsigned long val;
    event.Enable( m_textRange->GetValue().ToULong(&val) );
}

void GaugeWidgetsPage::OnUpdateUIResetButton(wxUpdateUIEvent& event)
{
    event.Enable( m_chkVert->GetValue() || m_chkSmooth->GetValue() );
}

void GaugeWidgetsPage::OnCheckOrRadioBox(wxCommandEvent& event)
{
    CreateGauge();
}

void GaugeWidgetsPage::OnProgressTimer(wxTimerEvent& WXUNUSED(event))
{
    int val = m_gauge->GetValue();
    if ( (unsigned)val < m_range )
    {
        m_gauge->SetValue(val + 1);
    }
    else // reached the end
    {
        StopTimer();
    }
}

void GaugeWidgetsPage::OnUpdateUICurValueText(wxUpdateUIEvent& event)
{
    event.SetText( wxString::Format(_T("%d"), m_gauge->GetValue()));
}

void GaugeWidgetsPage::StopTimer()
{
    wxCHECK_RET( m_timer, _T("shouldn't be called") );

    m_timer->Stop();
    delete m_timer;
    m_timer = NULL;

    wxButton *btn = (wxButton *)FindWindow(GaugePage_Progress);
    wxCHECK_RET( btn, _T("no progress button?") );

    btn->SetLabel(_T("Simulate &progress"));

    wxLogMessage(_T("Progress finished."));
}

