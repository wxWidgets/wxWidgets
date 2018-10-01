/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        timepick.cpp
// Purpose:     Part of the widgets sample showing time picker
// Author:      Vadim Zeitlin
// Created:     2011-12-20
// Copyright:   (c) 2011 wxWindows team
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

#if wxUSE_TIMEPICKCTRL

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/crt.h"
    #include "wx/app.h"
    #include "wx/log.h"

    #include "wx/button.h"
    #include "wx/textctrl.h"

    #include "wx/sizer.h"
#endif

#include "wx/timectrl.h"
#include "wx/dateevt.h"

#include "widgets.h"

#include "icons/timepick.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    TimePickerPage_Reset = wxID_HIGHEST,
    TimePickerPage_Set,
    TimePickerPage_Picker
};

// ----------------------------------------------------------------------------
// CheckBoxWidgetsPage
// ----------------------------------------------------------------------------

class TimePickerWidgetsPage : public WidgetsPage
{
public:
    TimePickerWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist);

    virtual wxWindow *GetWidget() const wxOVERRIDE { return m_timePicker; }
    virtual void RecreateWidget() wxOVERRIDE { CreateTimePicker(); }

    // lazy creation of the content
    virtual void CreateContent() wxOVERRIDE;

protected:
    // event handlers
    void OnTimeChanged(wxDateEvent& event);

    void OnButtonSet(wxCommandEvent& event);
    void OnButtonReset(wxCommandEvent& event);

    // reset the time picker parameters
    void Reset();

    // (re)create the time picker
    void CreateTimePicker();

    // the controls
    // ------------

    // the checkbox itself and the sizer it is in
    wxTimePickerCtrl *m_timePicker;
    wxSizer *m_sizerTimePicker;

    wxTextCtrl *m_textCur;

private:
    wxDECLARE_EVENT_TABLE();
    DECLARE_WIDGETS_PAGE(TimePickerWidgetsPage)
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(TimePickerWidgetsPage, WidgetsPage)
    EVT_BUTTON(TimePickerPage_Reset, TimePickerWidgetsPage::OnButtonReset)
    EVT_BUTTON(TimePickerPage_Set, TimePickerWidgetsPage::OnButtonSet)

    EVT_TIME_CHANGED(wxID_ANY, TimePickerWidgetsPage::OnTimeChanged)
wxEND_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

#if defined(__WXMSW__)
    #define FAMILY_CTRLS NATIVE_CTRLS
#else
    #define FAMILY_CTRLS GENERIC_CTRLS
#endif

IMPLEMENT_WIDGETS_PAGE(TimePickerWidgetsPage, "TimePicker",
                       FAMILY_CTRLS | PICKER_CTRLS
                       );

TimePickerWidgetsPage::TimePickerWidgetsPage(WidgetsBookCtrl *book,
                                         wxImageList *imaglist)
                     : WidgetsPage(book, imaglist, timepick_xpm)
{
}

void TimePickerWidgetsPage::CreateContent()
{
    wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    // left pane
    wxSizer* const sizerLeft = new wxBoxSizer(wxVERTICAL);

    sizerLeft->Add(new wxButton(this, TimePickerPage_Reset, "&Reset"),
                   wxSizerFlags().Centre().Border());


    // middle pane: operations
    wxSizer* const sizerMiddle = new wxBoxSizer(wxVERTICAL);
    sizerMiddle->Add(CreateSizerWithTextAndButton
                     (
                        TimePickerPage_Set,
                        "&Set time",
                        wxID_ANY,
                        &m_textCur
                     ),
                     wxSizerFlags().Expand().Border());

    m_textCur->SetMinSize(wxSize(GetTextExtent("  99:99:99  ").x, -1));


    // right pane: control itself
    wxSizer *sizerRight = new wxBoxSizer(wxHORIZONTAL);

    m_timePicker = new wxTimePickerCtrl(this, TimePickerPage_Picker);

    sizerRight->Add(0, 0, 1, wxCENTRE);
    sizerRight->Add(m_timePicker, 1, wxCENTRE);
    sizerRight->Add(0, 0, 1, wxCENTRE);
    m_sizerTimePicker = sizerRight; // save it to modify it later

    // the 3 panes panes compose the window
    sizerTop->Add(sizerLeft, 0, (wxALL & ~wxLEFT), 10);
    sizerTop->Add(sizerMiddle, 0, (wxTOP | wxBOTTOM), 10);
    sizerTop->Add(sizerRight, 1, wxGROW | (wxALL & ~wxRIGHT), 10);

    // final initializations
    Reset();

    SetSizer(sizerTop);
}

void TimePickerWidgetsPage::Reset()
{
    const wxDateTime today = wxDateTime::Today();

    m_timePicker->SetValue(today);
    m_textCur->SetValue(today.FormatISOTime());
}

void TimePickerWidgetsPage::CreateTimePicker()
{
    const wxDateTime value = m_timePicker->GetValue();

    size_t count = m_sizerTimePicker->GetChildren().GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        m_sizerTimePicker->Remove(0);
    }

    delete m_timePicker;

    long style = GetAttrs().m_defaultFlags;

    m_timePicker = new wxTimePickerCtrl(this, TimePickerPage_Picker, value,
                                        wxDefaultPosition, wxDefaultSize,
                                        style);

    m_sizerTimePicker->Add(0, 0, 1, wxCENTRE);
    m_sizerTimePicker->Add(m_timePicker, 1, wxCENTRE);
    m_sizerTimePicker->Add(0, 0, 1, wxCENTRE);
    m_sizerTimePicker->Layout();
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void TimePickerWidgetsPage::OnButtonReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();

    CreateTimePicker();
}

void TimePickerWidgetsPage::OnButtonSet(wxCommandEvent& WXUNUSED(event))
{
    int h, m, s;
    if ( wxSscanf(m_textCur->GetValue(), "%d:%d:%d", &h, &m, &s) != 3 )
    {
        wxLogError("Invalid time, please use HH:MM:SS format.");
        return;
    }

    m_timePicker->SetTime(h, m, s);
}

void TimePickerWidgetsPage::OnTimeChanged(wxDateEvent& event)
{
    int h, m, s;
    m_timePicker->GetTime(&h, &m, &s);

    wxLogMessage("Time changed, now is %s (control value is %02d:%02d:%02d).",
                 event.GetDate().FormatISOTime(), h, m, s);
}

#endif // wxUSE_TIMEPICKCTRL
