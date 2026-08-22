///////////////////////////////////////////////////////////////////////////////
// Name:        tests/events/timertest.cpp
// Purpose:     Test wxTimer events
// Author:      Vadim Zeitlin
// Created:     2008-10-22
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include <time.h>

#include "wx/evtloop.h"
#include "wx/timer.h"

// --------------------------------------------------------------------------
// helper class counting the number of timer events
// --------------------------------------------------------------------------

class TimerCounterHandler : public wxEvtHandler
{
public:
    TimerCounterHandler()
    {
        m_events = 0;

        Connect(wxEVT_TIMER, wxTimerEventHandler(TimerCounterHandler::OnTimer));
    }

    int GetNumEvents() const { return m_events; }

private:
    void OnTimer(wxTimerEvent& WXUNUSED(event))
    {
        m_events++;

        Tick();
    }

    virtual void Tick() { /* nothing to do in the base class */ }

    int m_events;

    wxDECLARE_NO_COPY_CLASS(TimerCounterHandler);
};

// --------------------------------------------------------------------------
// test class
// --------------------------------------------------------------------------

TEST_CASE("TimerEvent::OneShot", "[timer]")
{
    class ExitOnTimerHandler : public TimerCounterHandler
    {
    public:
        ExitOnTimerHandler(wxEventLoopBase& loop)
            : TimerCounterHandler(),
              m_loop(loop)
        {
        }

    private:
        virtual void Tick() override { m_loop.Exit(); }

        wxEventLoopBase& m_loop;

        // don't use wxDECLARE_NO_COPY_CLASS() to avoid upsetting MSVC
    };

    wxEventLoop loop;

    ExitOnTimerHandler handler(loop);
    wxTimer timer(&handler);
    timer.Start(200, true);

    loop.Run();

    CHECK( handler.GetNumEvents() == 1 );
}

TEST_CASE("TimerEvent::Multiple", "[timer]")
{
    wxEventLoop loop;

    TimerCounterHandler handler;
    wxTimer timer(&handler);
    timer.Start(100);

    // run the loop for 2 seconds
    time_t t;
    time(&t);
    const time_t tEnd = t + 2;
    while ( time(&t) < tEnd )
    {
        loop.Dispatch();
    }

    // we can't count on getting exactly 20 ticks but we shouldn't get more
    // than this
    const int numTicks = handler.GetNumEvents();
    CHECK( numTicks <= 20 );

    // and we should get a decent number of them but if the system is very
    // loaded (as happens with build bot slaves running a couple of builds in
    // parallel actually) it may be much less than 20 so just check that we get
    // more than one
    CHECK( numTicks > 1 );
}
