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

TEST_CASE("StopWatch::Misc", "[stopwatch]")
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
    wxINFO_FMT("Elapsed time was %" wxLongLongFmtSpec "dus", usec);
    CHECK( usec < tolerance*1000 );

    wxSleep(1);
    t = sw.Time();

    // check that the stop watch doesn't advance while paused
    wxINFO_FMT("Actual time value is %ld", t);
    CHECK( t >= 0 );
    CHECK( t < tolerance );

    sw.Resume();
    wxMilliSleep(sleepTime);
    t = sw.Time();
    // check that it did advance now by ~1.5s
    wxINFO_FMT("Actual time value is %ld", t);
    CHECK( t > sleepTime - tolerance );
    CHECK( t < 2*sleepTime );

    sw.Pause();

    // check that this sleep won't be taken into account below
    wxMilliSleep(sleepTime);
    sw.Resume();

    wxMilliSleep(sleepTime);
    t = sw.Time();

    // and it should advance again
    wxINFO_FMT("Actual time value is %ld", t);
    CHECK( t > 2*sleepTime - tolerance );
    CHECK( t < 3*sleepTime );
}

TEST_CASE("StopWatch::BackwardsClockBug", "[stopwatch]")
{
    wxStopWatch sw;
    wxStopWatch sw2;

    for ( size_t n = 0; n < 10; n++ )
    {
        sw2.Start();

        for ( size_t m = 0; m < 10000; m++ )
        {
            CHECK( sw.Time() >= 0 );
            CHECK( sw2.Time() >= 0 );
        }
    }
}

TEST_CASE("StopWatch::RestartBug", "[stopwatch]")
{
    wxStopWatch sw;
    sw.Pause();

    // Calling Start() should resume the stopwatch if it was paused.
    static const int offset = 5000;
    sw.Start(offset);
    wxMilliSleep(sleepTime);

    long t = sw.Time();
    wxINFO_FMT("Actual time value is %ld", t);
    CHECK( t >= offset + sleepTime - tolerance );

    // As above, this is not actually due to the fact of the test being
    // automatic but just because buildbot machines are usually pretty slow, so
    // this test often fails there simply because of the high load on them.
    if ( !IsAutomaticTest() )
    {
        wxINFO_FMT("Actual time value is %ld", t);

        CHECK( t < offset + sleepTime + tolerance );
    }
}
