///////////////////////////////////////////////////////////////////////////////
// Name:        tests/events/stopwatch.cpp
// Purpose:     Test wxStopWatch class
// Author:      Francesco Montorsi (extracted from console sample)
// Created:     2010-05-16
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

namespace
{

const long tolerance = 50;  // in ms
const int sleepTime = 500;

} // anonymous namespace

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
        CPPUNIT_TEST( RestartBug );
    CPPUNIT_TEST_SUITE_END();

    void Misc();
    void BackwardsClockBug();
    void RestartBug();

    DECLARE_NO_COPY_CLASS(StopWatchTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( StopWatchTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( StopWatchTestCase, "StopWatchTestCase" );

void StopWatchTestCase::Misc()
{
    // Buildbot machines are quite slow and sleep doesn't work reliably there,
    // i.e. it can sleep for much longer than requested. This is not really an
    // error, so just don't run this test there -- and if you get failures in
    // this test when running it interactively, this might also be normal if
    // the machine is under heavy load.
    if ( IsAutomaticTest() )
        return;

    wxStopWatch sw;
    long t;
    wxLongLong usec;

    sw.Pause();         // pause it immediately

    // verify that almost no time elapsed
    usec = sw.TimeInMicro();
    WX_ASSERT_MESSAGE
    (
        ("Elapsed time was %" wxLongLongFmtSpec "dus", usec),
        usec < tolerance*1000
    );

    wxSleep(1);
    t = sw.Time();

    // check that the stop watch doesn't advance while paused
    WX_ASSERT_MESSAGE
    (
        ("Actual time value is %ld", t),
        t >= 0 && t < tolerance
    );

    sw.Resume();
    wxMilliSleep(sleepTime);
    t = sw.Time();
    // check that it did advance now by ~1.5s
    WX_ASSERT_MESSAGE
    (
        ("Actual time value is %ld", t),
        t > sleepTime - tolerance && t < sleepTime + tolerance
    );

    sw.Pause();

    // check that this sleep won't be taken into account below
    wxMilliSleep(sleepTime);
    sw.Resume();

    wxMilliSleep(sleepTime);
    t = sw.Time();

    // and it should advance again
    WX_ASSERT_MESSAGE
    (
        ("Actual time value is %ld", t),
        t > 2*sleepTime - tolerance && t < 2*sleepTime + tolerance
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

void StopWatchTestCase::RestartBug()
{
    wxStopWatch sw;
    sw.Pause();

    // Calling Start() should resume the stopwatch if it was paused.
    static const int offset = 5000;
    sw.Start(offset);
    wxMilliSleep(sleepTime);

    long t = sw.Time();
    WX_ASSERT_MESSAGE
    (
        ("Actual time value is %ld", t),
        t > offset + sleepTime - tolerance &&
            t < offset + sleepTime + tolerance
    );
}
