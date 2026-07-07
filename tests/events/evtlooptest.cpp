///////////////////////////////////////////////////////////////////////////////
// Name:        tests/events/evtloop.cpp
// Purpose:     Tests for the event loop classes
// Author:      Rob Bresalier
// Created:     2013-05-02
// Copyright:   (c) 2013 Rob Bresalier
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#include "wx/app.h"
#include "wx/apptrait.h"
#include "wx/evtloop.h"
#include "wx/timer.h"

#include <memory>

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// Use two arbitrary but different return codes for the two loops.
const int EXIT_CODE_OUTER_LOOP = 99;
const int EXIT_CODE_INNER_LOOP = 55;

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class EvtloopTestCase : public CppUnit::TestCase
{
public:
    EvtloopTestCase() { }

private:
    CPPUNIT_TEST_SUITE( EvtloopTestCase );
        CPPUNIT_TEST( TestExit );
    CPPUNIT_TEST_SUITE_END();

    void TestExit();

    wxDECLARE_NO_COPY_CLASS(EvtloopTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( EvtloopTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( EvtloopTestCase, "EvtloopTestCase" );


// Check that the application traits create an event loop of the appropriate
// kind: this notably would not be the case for console applications using a
// monolithic build of the library before the fix for #24909.
#if wxUSE_GUI || wxUSE_CONSOLE_EVENTLOOP

TEST_CASE("EventLoop::CreateFromTraits", "[evtloop]")
{
    wxAppTraits* const traits = wxTheApp->GetTraits();
    REQUIRE( traits );

    std::unique_ptr<wxEventLoopBase> loop(traits->CreateEventLoop());
    REQUIRE( loop );

#if wxUSE_GUI
    CHECK( dynamic_cast<wxGUIEventLoop*>(loop.get()) );
#else // console application
    CHECK( dynamic_cast<wxConsoleEventLoop*>(loop.get()) );
#endif
}

#endif // wxUSE_GUI || wxUSE_CONSOLE_EVENTLOOP

// Helper class to schedule exit of the given event loop after the specified
// delay.
class ScheduleLoopExitTimer : public wxTimer
{
public:
    ScheduleLoopExitTimer(wxEventLoop& loop, int rc)
        : m_loop(loop),
          m_rc(rc)
    {
    }

    virtual void Notify() override
    {
        m_loop.ScheduleExit(m_rc);
    }

private:
    wxEventLoop& m_loop;
    const int m_rc;
};

// Another helper which runs a nested loop and schedules exiting both the outer
// and the inner loop after the specified delays.
class RunNestedAndExitBothLoopsTimer : public wxTimer
{
public:
    RunNestedAndExitBothLoopsTimer(wxTimer& timerOuter,
                                   int loopOuterDuration,
                                   int loopInnerDuration)
        : m_timerOuter(timerOuter),
          m_loopOuterDuration(loopOuterDuration),
          m_loopInnerDuration(loopInnerDuration)
    {
    }

    virtual void Notify() override
    {
        wxEventLoop loopInner;
        ScheduleLoopExitTimer timerInner(loopInner, EXIT_CODE_INNER_LOOP);

        m_timerOuter.StartOnce(m_loopOuterDuration);
        timerInner.StartOnce(m_loopInnerDuration);

        CPPUNIT_ASSERT_EQUAL( EXIT_CODE_INNER_LOOP, loopInner.Run() );
    }

private:
    wxTimer& m_timerOuter;
    const int m_loopOuterDuration;
    const int m_loopInnerDuration;
};

void EvtloopTestCase::TestExit()
{
    // Test that simply exiting the loop works.
    wxEventLoop loopOuter;
    ScheduleLoopExitTimer timerExit(loopOuter, EXIT_CODE_OUTER_LOOP);
    timerExit.StartOnce(1);
    CPPUNIT_ASSERT_EQUAL( EXIT_CODE_OUTER_LOOP, loopOuter.Run() );

    // Test that exiting the outer loop before the inner loop (outer duration
    // parameter less than inner duration in the timer ctor below) works.
    ScheduleLoopExitTimer timerExitOuter(loopOuter, EXIT_CODE_OUTER_LOOP);
    RunNestedAndExitBothLoopsTimer timerRun(timerExitOuter, 5, 10);
    timerRun.StartOnce(1);
    CPPUNIT_ASSERT_EQUAL( EXIT_CODE_OUTER_LOOP, loopOuter.Run() );

    // Test that exiting the inner loop before the outer one works too.
    ScheduleLoopExitTimer timerExitOuter2(loopOuter, EXIT_CODE_OUTER_LOOP);
    RunNestedAndExitBothLoopsTimer timerRun2(timerExitOuter, 10, 5);
    timerRun2.StartOnce(1);
    CPPUNIT_ASSERT_EQUAL( EXIT_CODE_OUTER_LOOP, loopOuter.Run() );
}
