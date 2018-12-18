/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        datepick.cpp
// Purpose:     Part of the widgets sample showing date picker
// Author:      Dimitri Schoolwerth, Vadim Zeitlin
// Created:     27 Sep 2003
// Copyright:   (c) 2003 wxWindows team
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

#if wxUSE_DATEPICKCTRL

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/log.h"

    #include "wx/bitmap.h"
    #include "wx/button.h"
    #include "wx/checkbox.h"
    #include "wx/radiobox.h"
    #include "wx/statbox.h"
    #include "wx/textctrl.h"

    #include "wx/sizer.h"
#endif

#include "wx/datectrl.h"
#include "wx/dateevt.h"

#include "widgets.h"

#include "icons/datepick.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    DatePickerPage_Reset = wxID_HIGHEST,
    DatePickerPage_Set,
    DatePickerPage_SetRange,
    DatePickerPage_Picker
};

// ----------------------------------------------------------------------------
// CheckBoxWidgetsPage
// ----------------------------------------------------------------------------

class DatePickerWidgetsPage : public WidgetsPage
{
public:
    DatePickerWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist);

    virtual wxWindow *GetWidget() const wxOVERRIDE { return m_datePicker; }
    virtual void RecreateWidget() wxOVERRIDE { CreateDatePicker(); }

    // lazy creation of the content
    virtual void CreateContent() wxOVERRIDE;

protected:
    // event handlers
    void OnDateChanged(wxDateEvent& event);

    void OnButtonSet(wxCommandEvent& event);
    void OnButtonSetRange(wxCommandEvent& event);
    void OnButtonReset(wxCommandEvent& event);

    // reset the date picker parameters
    void Reset();

    // (re)create the date picker
    void CreateDatePicker();

    // the controls
    // ------------

    // the checkbox itself and the sizer it is in
    wxDatePickerCtrl *m_datePicker;
    wxSizer *m_sizerDatePicker;

    wxTextCtrl *m_textCur;
    wxTextCtrl *m_textMin;
    wxTextCtrl *m_textMax;

    wxRadioBox* m_radioKind;
    wxCheckBox* m_chkStyleCentury;
    wxCheckBox* m_chkStyleAllowNone;

    // the text entries for command parameters
    wxTextCtrl *m_textLabel;

private:
    wxDECLARE_EVENT_TABLE();
    DECLARE_WIDGETS_PAGE(DatePickerWidgetsPage)
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(DatePickerWidgetsPage, WidgetsPage)
    EVT_BUTTON(DatePickerPage_Reset, DatePickerWidgetsPage::OnButtonReset)
    EVT_BUTTON(DatePickerPage_Set, DatePickerWidgetsPage::OnButtonSet)
    EVT_BUTTON(DatePickerPage_SetRange, DatePickerWidgetsPage::OnButtonSetRange)

    EVT_DATE_CHANGED(wxID_ANY, DatePickerWidgetsPage::OnDateChanged)
wxEND_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

#if defined(__WXMSW__)
    #define FAMILY_CTRLS NATIVE_CTRLS
#else
    #define FAMILY_CTRLS GENERIC_CTRLS
#endif

IMPLEMENT_WIDGETS_PAGE(DatePickerWidgetsPage, "DatePicker",
                       FAMILY_CTRLS | PICKER_CTRLS
                       );

DatePickerWidgetsPage::DatePickerWidgetsPage(WidgetsBookCtrl *book,
                                         wxImageList *imaglist)
                      :WidgetsPage(book, imaglist, datepick_xpm)
{
}

void DatePickerWidgetsPage::CreateContent()
{
    wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    // left pane: style
    wxSizer* const sizerLeft = new wxBoxSizer(wxVERTICAL);

    static const wxString kinds[] = { "&Default", "&Spin", "Drop do&wn" };
    m_radioKind = new wxRadioBox(this, wxID_ANY, "&Kind",
                                 wxDefaultPosition, wxDefaultSize,
                                 WXSIZEOF(kinds), kinds,
                                 1, wxRA_SPECIFY_COLS);
    sizerLeft->Add(m_radioKind, wxSizerFlags().Expand().Border());

    wxSizer* const sizerStyle = new wxStaticBoxSizer(wxVERTICAL, this, "&Style");
    m_chkStyleCentury = CreateCheckBoxAndAddToSizer(sizerStyle, "Show &century");
    m_chkStyleAllowNone = CreateCheckBoxAndAddToSizer(sizerStyle, "Allow &no value");

    sizerLeft->Add(sizerStyle, wxSizerFlags().Expand().Border());

    sizerLeft->Add(new wxButton(this, DatePickerPage_Reset, "&Recreate"),
                   wxSizerFlags().Centre().Border());


    // middle pane: operations
    wxSizer* const sizerMiddle = new wxBoxSizer(wxVERTICAL);
    sizerMiddle->Add(CreateSizerWithTextAndButton
                     (
                        DatePickerPage_Set,
                        "&Set date",
                        wxID_ANY,
                        &m_textCur
                     ),
                     wxSizerFlags().Expand().Border());

    m_textCur->SetMinSize(wxSize(GetTextExtent("  9999-99-99  ").x, -1));

    sizerMiddle->AddSpacer(10);

    sizerMiddle->Add(CreateSizerWithTextAndLabel
                     (
                        "&Min date",
                        wxID_ANY,
                        &m_textMin
                     ),
                     wxSizerFlags().Expand().Border());
    sizerMiddle->Add(CreateSizerWithTextAndLabel
                     (
                        "Ma&x date",
                        wxID_ANY,
                        &m_textMax
                     ),
                     wxSizerFlags().Expand().Border());
    sizerMiddle->Add(new wxButton(this, DatePickerPage_SetRange, "Set &range"),
                     wxSizerFlags().Centre().Border());


    // right pane: control itself
    wxSizer *sizerRight = new wxBoxSizer(wxHORIZONTAL);

    m_datePicker = new wxDatePickerCtrl(this, DatePickerPage_Picker);

    sizerRight->Add(0, 0, 1, wxCENTRE);
    sizerRight->Add(m_datePicker, 1, wxCENTRE);
    sizerRight->Add(0, 0, 1, wxCENTRE);
    m_sizerDatePicker = sizerRight; // save it to modify it later

    // the 3 panes panes compose the window
    sizerTop->Add(sizerLeft, 0, (wxALL & ~wxLEFT), 10);
    sizerTop->Add(sizerMiddle, 0, (wxTOP | wxBOTTOM), 10);
    sizerTop->Add(sizerRight, 1, wxGROW | (wxALL & ~wxRIGHT), 10);

    // final initializations
    m_chkStyleCentury->SetValue(true);
    Reset();

    SetSizer(sizerTop);
}

void DatePickerWidgetsPage::Reset()
{
    const wxDateTime today = wxDateTime::Today();

    m_datePicker->SetValue(today);
    m_textCur->SetValue(today.FormatISODate());
}

void DatePickerWidgetsPage::CreateDatePicker()
{
    const wxDateTime value = m_datePicker->GetValue();

    size_t count = m_sizerDatePicker->GetChildren().GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        m_sizerDatePicker->Remove(0);
    }

    delete m_datePicker;

    long style = GetAttrs().m_defaultFlags;
    switch ( m_radioKind->GetSelection() )
    {
        case 0:
            style = wxDP_DEFAULT;
            break;

        case 1:
            style = wxDP_SPIN;
            break;

        case 2:
            style = wxDP_DROPDOWN;
            break;
    }

    if ( m_chkStyleCentury->GetValue() )
        style |= wxDP_SHOWCENTURY;
    if ( m_chkStyleAllowNone->GetValue() )
        style |= wxDP_ALLOWNONE;

    m_datePicker = new wxDatePickerCtrl(this, DatePickerPage_Picker, value,
                                        wxDefaultPosition, wxDefaultSize,
                                        style);

    m_sizerDatePicker->Add(0, 0, 1, wxCENTRE);
    m_sizerDatePicker->Add(m_datePicker, 1, wxCENTRE);
    m_sizerDatePicker->Add(0, 0, 1, wxCENTRE);
    m_sizerDatePicker->Layout();
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void DatePickerWidgetsPage::OnButtonReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();

    CreateDatePicker();
}

static bool GetDateFromTextControl(wxDateTime& dt, const wxTextCtrl* text)
{
    const wxString& value = text->GetValue();
    if ( !value.empty() )
    {
        wxString::const_iterator end;
        if ( !dt.ParseDate(value, &end) || end != value.end() )
        {
            wxLogError("Invalid date \"%s\"");
            return false;
        }
    }

    return true;
}

void DatePickerWidgetsPage::OnButtonSet(wxCommandEvent& WXUNUSED(event))
{
    wxDateTime dt;
    if ( GetDateFromTextControl(dt, m_textCur) )
        m_datePicker->SetValue(dt);
}

void DatePickerWidgetsPage::OnButtonSetRange(wxCommandEvent& WXUNUSED(event))
{
    wxDateTime dt1, dt2;
    if ( !GetDateFromTextControl(dt1, m_textMin) ||
           !GetDateFromTextControl(dt2, m_textMax) )
        return;

    m_datePicker->SetRange(dt1, dt2);

    if ( !m_datePicker->GetRange(&dt1, &dt2) )
    {
        wxLogMessage("No range set");
    }
    else
    {
        m_textMin->SetValue(dt1.IsValid() ? dt1.FormatISODate() : wxString());
        m_textMax->SetValue(dt2.IsValid() ? dt2.FormatISODate() : wxString());

        wxLogMessage("Date picker range updated");
    }
}

void DatePickerWidgetsPage::OnDateChanged(wxDateEvent& event)
{
    wxLogMessage("Date changed, now is %s (control value is %s).",
                 event.GetDate().FormatISOCombined(),
                 m_datePicker->GetValue().FormatISOCombined());
}

#endif // wxUSE_DATEPICKCTRL
