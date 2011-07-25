///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/datepickerctrltest.cpp
// Purpose:     wxDatePickerCtrl unit test
// Author:      Vadim Zeitlin
// Created:     2011-06-18
// RCS-ID:      $Id$
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_DATEPICKCTRL

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/datectrl.h"

#include "testableframe.h"
#include "testdate.h"

class DatePickerCtrlTestCase : public CppUnit::TestCase
{
public:
    DatePickerCtrlTestCase() { }

    void setUp();
    void tearDown();

private:
    CPPUNIT_TEST_SUITE( DatePickerCtrlTestCase );
        CPPUNIT_TEST( Value );
        CPPUNIT_TEST( Range );
    CPPUNIT_TEST_SUITE_END();

    void Value();
    void Range();

    wxDatePickerCtrl* m_datepicker;

    DECLARE_NO_COPY_CLASS(DatePickerCtrlTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( DatePickerCtrlTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( DatePickerCtrlTestCase, "DatePickerCtrlTestCase" );

void DatePickerCtrlTestCase::setUp()
{
    m_datepicker = new wxDatePickerCtrl(wxTheApp->GetTopWindow(), wxID_ANY);
}

void DatePickerCtrlTestCase::tearDown()
{
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
    m_datepicker->GetRange(&dtRangeStart, &dtRangeEnd);
    CPPUNIT_ASSERT( !dtRangeEnd.IsValid() );

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
}

#endif // wxUSE_DATEPICKCTRL
