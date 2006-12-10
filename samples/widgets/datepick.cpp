/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        datepick.cpp
// Purpose:     Part of the widgets sample showing date picker
// Author:      Dimitri Schoolwerth, Vadim Zeitlin
// Created:     27 Sep 2003
// Id:          $Id$
// Copyright:   (c) 2003 wxWindows team
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

#include "widgets.h"

#include "icons/datepick.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    DatePickerPage_Reset = wxID_HIGHEST,
    DatePickerPage_Day,
    DatePickerPage_Month,
    DatePickerPage_Year,
    DatePickerPage_Set,
    DatePickerPage_Picker
};

// ----------------------------------------------------------------------------
// CheckBoxWidgetsPage
// ----------------------------------------------------------------------------

class DatePickerWidgetsPage : public WidgetsPage
{
public:
    DatePickerWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist);
    virtual ~DatePickerWidgetsPage(){};

    virtual wxControl *GetWidget() const { return m_datePicker; }
    virtual void RecreateWidget() { CreateDatePicker(); }

    // lazy creation of the content
    virtual void CreateContent();

protected:
    // event handlers
    void OnButtonSet(wxCommandEvent& event);

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

    wxTextCtrl *m_day;
    wxTextCtrl *m_month;
    wxTextCtrl *m_year;

    // the text entries for command parameters
    wxTextCtrl *m_textLabel;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_WIDGETS_PAGE(DatePickerWidgetsPage)
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(DatePickerWidgetsPage, WidgetsPage)
    EVT_BUTTON(DatePickerPage_Reset, DatePickerWidgetsPage::OnButtonReset)
    EVT_BUTTON(DatePickerPage_Set, DatePickerWidgetsPage::OnButtonSet)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

#if defined(__WXMSW__)
    #define FAMILY_CTRLS NATIVE_CTRLS
#else
    #define FAMILY_CTRLS GENERIC_CTRLS
#endif

IMPLEMENT_WIDGETS_PAGE(DatePickerWidgetsPage, wxT("DatePicker"),
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

    // left pane
    wxStaticBox *box = new wxStaticBox(this, wxID_ANY, wxT("Date details"));

    wxSizer *sizerLeft = new wxStaticBoxSizer(box, wxVERTICAL);

    sizerLeft->Add( CreateSizerWithTextAndLabel( wxT("&Day:"), DatePickerPage_Day , &m_day ),
                    0, wxALL | wxALIGN_RIGHT , 5 );

    sizerLeft->Add( CreateSizerWithTextAndLabel( wxT("&Month:"), DatePickerPage_Month , &m_month ),
                    0, wxALL | wxALIGN_RIGHT , 5 );

    sizerLeft->Add( CreateSizerWithTextAndLabel( wxT("&Year:"), DatePickerPage_Year , &m_year ),
                    0, wxALL | wxALIGN_RIGHT , 5 );

    sizerLeft->Add( new wxButton( this, DatePickerPage_Set, wxT("&Set date") ),
                    0, wxALL , 5 );

    // right pane
    wxSizer *sizerRight = new wxBoxSizer(wxHORIZONTAL);

    m_datePicker = new wxDatePickerCtrl(this, DatePickerPage_Picker);

    sizerRight->Add(0, 0, 1, wxCENTRE);
    sizerRight->Add(m_datePicker, 1, wxCENTRE);
    sizerRight->Add(0, 0, 1, wxCENTRE);
    sizerRight->SetMinSize(150, 0);
    m_sizerDatePicker = sizerRight; // save it to modify it later

    // the 3 panes panes compose the window
    sizerTop->Add(sizerLeft, 0, (wxALL & ~wxLEFT), 10);
    sizerTop->Add(sizerRight, 1, wxGROW | (wxALL & ~wxRIGHT), 10);

    // final initializations
    Reset();

    SetSizer(sizerTop);
}

void DatePickerWidgetsPage::Reset()
{
    const wxDateTime today = wxDateTime::Today();

    m_datePicker->SetValue(today);
    m_day->SetValue(wxString::Format(_T("%d"), today.GetDay()));
    m_month->SetValue(wxString::Format(_T("%d"), today.GetMonth()));
    m_year->SetValue(wxString::Format(_T("%d"), today.GetYear()));
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

    m_datePicker = new wxDatePickerCtrl(this, DatePickerPage_Picker, value);

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

void DatePickerWidgetsPage::OnButtonSet(wxCommandEvent& WXUNUSED(event))
{
    long day = 0, month = 0, year = 0;
    if ( m_day->GetValue().ToLong(&day) &&
         m_month->GetValue().ToLong(&month) &&
         m_year->GetValue().ToLong(&year) )
    {
        const wxDateTime someDay(wxDateTime::wxDateTime_t(day),
                                 wxDateTime::Month(month),
                                 year);
        if(someDay.IsValid())
        {
            m_datePicker->SetValue(someDay);
        }
        else
        {
            wxLogError(_T("Date is invalid"));
        }
    }
    else
    {
        wxLogError(_T("One of inputs is not number"));
    }
}

#endif // wxUSE_DATEPICKCTRL
