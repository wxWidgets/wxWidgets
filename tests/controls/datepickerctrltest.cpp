///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/datepickerctrltest.cpp
// Purpose:     wxDatePickerCtrl unit test
// Author:      Vadim Zeitlin
// Created:     2011-06-18
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_DATEPICKCTRL

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/button.h"
#endif // WX_PRECOMP

#include "wx/datectrl.h"
#include "wx/uiaction.h"

#include "testableframe.h"
#include "testdate.h"

class DatePickerCtrlTestCase : public CppUnit::TestCase
{
public:
    DatePickerCtrlTestCase() { }

    void setUp() wxOVERRIDE;
    void tearDown() wxOVERRIDE;

private:
    CPPUNIT_TEST_SUITE( DatePickerCtrlTestCase );
        CPPUNIT_TEST( Value );
        CPPUNIT_TEST( Range );
        WXUISIM_TEST( Focus );
    CPPUNIT_TEST_SUITE_END();

    void Value();
    void Range();
    void Focus();

    wxDatePickerCtrl* m_datepicker;
    wxButton* m_button;

    wxDECLARE_NO_COPY_CLASS(DatePickerCtrlTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( DatePickerCtrlTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( DatePickerCtrlTestCase, "DatePickerCtrlTestCase" );

void DatePickerCtrlTestCase::setUp()
{
    m_datepicker = new wxDatePickerCtrl(wxTheApp->GetTopWindow(), wxID_ANY);
    m_button = NULL;
}

void DatePickerCtrlTestCase::tearDown()
{
    delete m_button;
    delete m_datepicker;
}

void DatePickerCtrlTestCase::Value()
{
    const wxDateTime dt(18, wxDateTime::Jul, 2011);
    m_datepicker->SetValue(dt);

    CPPUNIT_ASSERT_EQUAL( dt, m_datepicker->GetValue() );

    // We don't use wxDP_ALLOWNONE currently, hence a value is required.
    WX_ASSERT_FAILS_WITH_ASSERT( m_datepicker->SetValue(wxDateTime()) );
}

void DatePickerCtrlTestCase::Range()
{
    // Initially we have no valid range but MSW version still has (built in)
    // minimum as it doesn't support dates before 1601-01-01, hence don't rely
    // on GetRange() returning false.
    wxDateTime dtRangeStart, dtRangeEnd;

    // Default end date for QT is 31/12/7999 which is considered valid,
    // therefore we should omit this assertion for QT
#ifndef __WXQT__
    m_datepicker->GetRange(&dtRangeStart, &dtRangeEnd);
    CPPUNIT_ASSERT( !dtRangeEnd.IsValid() );
#endif

    // After we set it we should be able to get it back.
    const wxDateTime
        dtStart(15, wxDateTime::Feb, 1923),
        dtEnd(18, wxDateTime::Jun, 2011);

    m_datepicker->SetRange(dtStart, dtEnd);
    CPPUNIT_ASSERT( m_datepicker->GetRange(&dtRangeStart, &dtRangeEnd) );
    CPPUNIT_ASSERT_EQUAL( dtStart, dtRangeStart );
    CPPUNIT_ASSERT_EQUAL( dtEnd, dtRangeEnd );

    // Setting dates inside the range should work, including the range end
    // points.
    m_datepicker->SetValue(dtStart);
    CPPUNIT_ASSERT_EQUAL( dtStart, m_datepicker->GetValue() );

    m_datepicker->SetValue(dtEnd);
    CPPUNIT_ASSERT_EQUAL( dtEnd, m_datepicker->GetValue() );


    // Setting dates outside the range should not work.
    m_datepicker->SetValue(dtEnd + wxTimeSpan::Day());
    CPPUNIT_ASSERT_EQUAL( dtEnd, m_datepicker->GetValue() );

    m_datepicker->SetValue(dtStart - wxTimeSpan::Day());
    CPPUNIT_ASSERT_EQUAL( dtEnd, m_datepicker->GetValue() );


    // Changing the range should clamp the current value to it if necessary.
    const wxDateTime
        dtBeforeEnd = dtEnd - wxDateSpan::Day();
    m_datepicker->SetRange(dtStart, dtBeforeEnd);
    CPPUNIT_ASSERT_EQUAL( dtBeforeEnd, m_datepicker->GetValue() );
}

#if wxUSE_UIACTIONSIMULATOR

static wxPoint GetRectCenter(const wxRect& r)
{
    return (r.GetTopRight() + r.GetBottomLeft()) / 2;
}

void DatePickerCtrlTestCase::Focus()
{
    // Create another control just to give focus to it initially.
    m_button = new wxButton(wxTheApp->GetTopWindow(), wxID_OK);
    m_button->Move(0, m_datepicker->GetSize().y * 3);
    m_button->SetFocus();
    wxYield();

    CHECK( !m_datepicker->HasFocus() );

    EventCounter setFocus(m_datepicker, wxEVT_SET_FOCUS);
    EventCounter killFocus(m_datepicker, wxEVT_KILL_FOCUS);

    wxUIActionSimulator sim;

    sim.MouseMove(GetRectCenter(m_datepicker->GetScreenRect()));
    sim.MouseClick();
    wxYield();

    REQUIRE( m_datepicker->HasFocus() );
    CHECK( setFocus.GetCount() == 1 );
    CHECK( killFocus.GetCount() == 0 );

    sim.MouseMove(GetRectCenter(m_button->GetScreenRect()));
    sim.MouseClick();
    wxYield();

    CHECK( !m_datepicker->HasFocus() );
    CHECK( setFocus.GetCount() == 1 );
    CHECK( killFocus.GetCount() == 1 );
}

#endif // wxUSE_UIACTIONSIMULATOR

#endif // wxUSE_DATEPICKCTRL
