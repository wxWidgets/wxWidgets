/////////////////////////////////////////////////////////////////////////////
// Name:        mgl/timer.cpp
// Purpose:     wxTimer implementation
// Author:      Vaclav Slavik
// Id:          $Id$
// Copyright:   (c) 2001 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "timer.h"
#endif

#include "wx/timer.h"

#if wxUSE_TIMER

#include "wx/log.h"
#include "wx/mgl/private.h"

extern "C" ulong _EVT_getTicks();

// ----------------------------------------------------------------------------
// helper structures and wxTimerScheduler
// ----------------------------------------------------------------------------

class wxTimerDesc
{
public:
    wxTimerDesc(wxTimer *t) : timer(t), running(FALSE), next(NULL), prev(NULL) {}

    wxTimer         *timer;
    bool             running;
    wxTimerDesc     *next, *prev;
    unsigned long    shotTime;
};

class wxTimerScheduler
{
public:
    wxTimerScheduler() : m_timers(NULL) {}

    void QueueTimer(wxTimerDesc *desc, unsigned long when = 0);
    void RemoveTimer(wxTimerDesc *desc);
    void NotifyTimers();
   
private:
    wxTimerDesc *m_timers;
};

void wxTimerScheduler::QueueTimer(wxTimerDesc *desc, unsigned long when)
{
    if ( when == 0 )
        when = _EVT_getTicks() + desc->timer->GetInterval();
    desc->shotTime = when;
    desc->running = TRUE;

    wxLogTrace("mgl_timer", "queued timer %p at tick %i", 
               desc->timer, when);

    if ( m_timers )
    {
        wxTimerDesc *d = m_timers;
        while ( d->next && d->next->shotTime < when ) d = d->next;
        desc->next = d->next;
        desc->prev = d;
        if ( d->next )
            d->next->prev = desc;
        d->next = desc;
    }
    else
    {
        m_timers = desc;
        desc->prev = desc->next = NULL;
    }
}

void wxTimerScheduler::RemoveTimer(wxTimerDesc *desc)
{
    desc->running = FALSE;
    if ( desc == m_timers )
        m_timers = desc->next;
    if ( desc->prev )
        desc->prev->next = desc->next;
    if ( desc->next )
        desc->next->prev = desc->prev;
    desc->prev = desc->next = NULL;
}

void wxTimerScheduler::NotifyTimers()
{
    if ( m_timers )
    {
        unsigned long now = _EVT_getTicks();
        wxTimerDesc *desc;

        wxLogTrace("mgl_timer", "notifying timers, time is %i", now);
        
        while ( m_timers && m_timers->shotTime <= now )
        {
            desc = m_timers;
            bool oneShot = desc->timer->IsOneShot();
            RemoveTimer(desc);

            desc->timer->Notify();
            wxLogTrace("mgl_timer", "notified timer %p sheduled for %i", 
                       desc->timer, desc->shotTime);

            if ( !oneShot )
            {
                QueueTimer(desc, now + desc->timer->GetInterval());
            }
        }
    }
}



// ----------------------------------------------------------------------------
// wxTimer
// ----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxTimer,wxObject)

wxTimerScheduler *wxTimer::ms_scheduler = NULL;
size_t wxTimer::ms_timersCnt = 0;

void wxTimer::Init()
{
    if ( ms_timersCnt++ == 0 )
        ms_scheduler = new wxTimerScheduler;
    m_desc = new wxTimerDesc(this);
    wxLogTrace("mgl_timer", "--added timer (count=%i)", ms_timersCnt);
}

wxTimer::~wxTimer()
{
    if ( IsRunning() )
        Stop();

    if ( --ms_timersCnt == 0 )
    {
        delete ms_scheduler;
        ms_scheduler = NULL;
    }
    delete m_desc;
    wxLogTrace("mgl_timer", "--removed timer (count=%i)", ms_timersCnt);
}

bool wxTimer::IsRunning() const
{
    return m_desc->running;
}

bool wxTimer::Start(int millisecs, bool oneShot)
{
    wxLogTrace("mgl_timer", "started timer %p: %i ms, oneshot=%i", 
               this, millisecs, oneShot);

    if ( !wxTimerBase::Start(millisecs, oneShot) )
        return FALSE;
    
    ms_scheduler->QueueTimer(m_desc);
    return TRUE;
}

void wxTimer::Stop()
{
    if ( !m_desc->running ) return;
    
    ms_scheduler->RemoveTimer(m_desc);
}

/*static*/ void wxTimer::NotifyTimers()
{
    ms_scheduler->NotifyTimers();
}

#endif //wxUSE_TIMER
