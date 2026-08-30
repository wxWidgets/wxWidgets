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
#include "wx/app.h"
#include "wx/evtloop.h"
#include "wx/thread.h"

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

        CHECK( loopInner.Run() == EXIT_CODE_INNER_LOOP );
    }

private:
    wxTimer& m_timerOuter;
    const int m_loopOuterDuration;
    const int m_loopInnerDuration;
};

TEST_CASE("EventLoop::TestExit", "[evtloop]")
{
    // Test that simply exiting the loop works.
    wxEventLoop loopOuter;
    ScheduleLoopExitTimer timerExit(loopOuter, EXIT_CODE_OUTER_LOOP);
    timerExit.StartOnce(1);
    CHECK( loopOuter.Run() == EXIT_CODE_OUTER_LOOP );

    // Test that exiting the outer loop before the inner loop (outer duration
    // parameter less than inner duration in the timer ctor below) works.
    ScheduleLoopExitTimer timerExitOuter(loopOuter, EXIT_CODE_OUTER_LOOP);
    RunNestedAndExitBothLoopsTimer timerRun(timerExitOuter, 5, 10);
    timerRun.StartOnce(1);
    CHECK( loopOuter.Run() == EXIT_CODE_OUTER_LOOP );

    // Test that exiting the inner loop before the outer one works too.
    ScheduleLoopExitTimer timerExitOuter2(loopOuter, EXIT_CODE_OUTER_LOOP);
    RunNestedAndExitBothLoopsTimer timerRun2(timerExitOuter, 10, 5);
    timerRun2.StartOnce(1);
    CHECK( loopOuter.Run() == EXIT_CODE_OUTER_LOOP );
}

#if wxUSE_THREADS

// Worker thread that, after giving the main thread time to enter and block in
// its event loop, schedules a callback on the main thread via CallAfter().
class ExitLoopFromThread : public wxThread
{
public:
    ExitLoopFromThread(wxEventLoop& loop, int rc)
        : wxThread(wxTHREAD_JOINABLE),
          m_loop(loop),
          m_rc(rc)
    {
    }

protected:
    virtual void *Entry() override
    {
        // Wait until the main thread is actually blocked waiting for events, so
        // the CallAfter() below arrives while the loop is idle -- which is when
        // it must still be able to wake the loop.
        wxMilliSleep(100);

        // wxEventLoop is not a wxEvtHandler, so route the deferred call through
        // wxTheApp (which lives on, and runs the call on, the main thread).
        wxEventLoop* const loop = &m_loop;
        const int rc = m_rc;
        wxTheApp->CallAfter([loop, rc] { loop->ScheduleExit(rc); });

        return nullptr;
    }

private:
    wxEventLoop& m_loop;
    const int m_rc;

    wxDECLARE_NO_COPY_CLASS(ExitLoopFromThread);
};

TEST_CASE("EventLoop::TestCrossThreadCallAfter", "[evtloop]")
{
    // A CallAfter() issued from another thread must wake the main event loop and
    // run there, even when the loop is otherwise idle. This is a regression test
    // for wxQt, where wxQtEventLoopBase::WakeUp() woke the loop without posting a
    // Qt event, so queued pending events (CallAfter) were never processed and the
    // Run() below would block forever. See src/qt/evtloop.cpp.
    wxEventLoop loop;

    ExitLoopFromThread thread(loop, EXIT_CODE_OUTER_LOOP);
    REQUIRE( thread.Run() == wxTHREAD_NO_ERROR );

    // If the cross-thread CallAfter() is delivered, the loop exits with the code
    // the worker passed to ScheduleExit(); otherwise this hangs (the bug).
    CHECK( loop.Run() == EXIT_CODE_OUTER_LOOP );

    thread.Wait();
}

#endif // wxUSE_THREADS
