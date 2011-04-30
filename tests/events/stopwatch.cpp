///////////////////////////////////////////////////////////////////////////////
// Name:        tests/events/stopwatch.cpp
// Purpose:     Test wxStopWatch class
// Author:      Francesco Montorsi (extracted from console sample)
// Created:     2010-05-16
// RCS-ID:      $Id$
// Copyright:   (c) 2010 wxWidgets team
///////////////////////////////////////////////////////////////////////////////


// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include <time.h>

#include "wx/stopwatch.h"
#include "wx/utils.h"

// --------------------------------------------------------------------------
// test class
// --------------------------------------------------------------------------

class StopWatchTestCase : public CppUnit::TestCase
{
public:
    StopWatchTestCase() {}

private:
    CPPUNIT_TEST_SUITE( StopWatchTestCase );
        CPPUNIT_TEST( Misc );
        CPPUNIT_TEST( BackwardsClockBug );
    CPPUNIT_TEST_SUITE_END();

    void Misc();
    void BackwardsClockBug();

    DECLARE_NO_COPY_CLASS(StopWatchTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( StopWatchTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( StopWatchTestCase, "StopWatchTestCase" );

void StopWatchTestCase::Misc()
{
    static const long tolerance = 100;  // in ms

    wxStopWatch sw;
    long t;

    sw.Pause();         // pause it immediately

    wxSleep(2);
    t = sw.Time();

    // check that the stop watch doesn't advance while paused
    WX_ASSERT_MESSAGE
    (
        ("Actual time value is %ld", t),
        t >= 0 && t < tolerance
    );

    sw.Resume();
    wxSleep(3);
    t = sw.Time();
    // check that it did advance now by ~3s
    WX_ASSERT_MESSAGE
    (
        ("Actual time value is %ld", t),
        t > 3000 - tolerance && t < 3000 + tolerance
    );

    sw.Pause();
    sw.Resume();

    wxSleep(2);
    t = sw.Time();

    // and it should advance again
    WX_ASSERT_MESSAGE
    (
        ("Actual time value is %ld", t),
        t > 5000 - tolerance && t < 5000 + tolerance
    );
}

void StopWatchTestCase::BackwardsClockBug()
{
    wxStopWatch sw;
    wxStopWatch sw2;

    for ( size_t n = 0; n < 10; n++ )
    {
        sw2.Start();

        for ( size_t m = 0; m < 10000; m++ )
        {
            CPPUNIT_ASSERT ( sw.Time() >= 0 && sw2.Time() >= 0 );
        }
    }
}
