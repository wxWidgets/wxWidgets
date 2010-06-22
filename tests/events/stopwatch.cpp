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

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( StopWatchTestCase, "StopWatchTestCase" );

void StopWatchTestCase::Misc()
{
    wxStopWatch sw;
    long tmp;

    sw.Pause();         // pause it immediately

    wxSleep(2);
    tmp = sw.Time();
    CPPUNIT_ASSERT(tmp >= 0 && tmp < 100);
            // should not have counted while paused!

    sw.Resume();
    wxSleep(3);
    tmp = sw.Time();
    CPPUNIT_ASSERT(tmp >= 3000 && tmp < 4000);

    sw.Pause();
    sw.Resume();

    wxSleep(2);
    tmp = sw.Time();
    CPPUNIT_ASSERT(tmp >= 5000 && tmp < 6000);
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
