/////////////////////////////////////////////////////////////////////////////
// Name:        wx/evtloop.h
// Purpose:     wxEventLoop and related classes
// Author:      Vadim Zeitlin
// Copyright:   (C) 2008 Vadim Zeitlin
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxEventLoopBase

    Base class for all event loop implementations.

    An object of this class is created by wxAppTraits::CreateEventLoop() and
    used by wxApp to run the main application event loop.

    @library{wxbase}
    @category{appmanagement}

    @see wxApp
*/
class wxEventLoopBase
{
public:
    /**
        Return the currently active (running) event loop.

        May return @NULL if there is no active event loop (e.g. during
        application startup or shutdown).
     */
    static wxEventLoopBase *GetActive();

    /**
        Set currently active (running) event loop.

        Called by wxEventLoopActivator, use an instance of this class instead
        of calling this method directly to ensure that the previously active
        event loop is restored.
     */
    static void SetActive(wxEventLoopBase* loop);


    /**
        Use this to check whether the event loop was successfully created
        before using it
     */
    virtual bool IsOk() const;

    /**
        Start the event loop, return the exit code when it is finished.

        Logically, this method calls Dispatch() in a loop until it returns
        @false and also takes care of generating idle events during each loop
        iteration. However not all implementations of this class really
        implement it like this (e.g. wxGTK does not) so you shouldn't rely on
        Dispatch() being called from inside this function.

        @return The argument passed to Exit() which terminated this event loop.
     */
    virtual int Run() = 0;

    /**
        Exit from the loop with the given exit code.
     */
    virtual void Exit(int rc = 0) = 0;

    /**
        Return true if any events are available.

        If this method returns @true, calling Dispatch() will not block.
     */
    virtual bool Pending() const = 0;

    /**
        Dispatch a single event.

        If there are currently no events in the queue, blocks until an event
        becomes available.

        @return @false only if the event loop should terminate.
     */
    virtual bool Dispatch() = 0;

    /**
        Return true if this event loop is currently running.

        Notice that even if this event loop hasn't terminated yet but has just
        spawned a nested (e.g. modal) event loop, this method would return
        @false.
     */
    bool IsRunning() const;

    /**
        Called by wxWidgets to wake up the event loop even if it is currently
        blocked inside Dispatch().
     */
    virtual void WakeUp() = 0;

protected:
    /**
        This function is called before the event loop terminates, whether this
        happens normally (because of Exit() call) or abnormally (because of an
        exception thrown from inside the loop).

        Default version does nothing.
     */
    virtual void OnExit();
};

/**
    @class wxEventLoopActivator

    Makes an event loop temporarily active.

    This class is used to make the event loop active during its life-time,
    e.g.:
    @code
        class MyEventLoop : public wxEventLoopBase { ... };

        void RunMyLoop()
        {
            MyEventLoop loop;
            wxEventLoopActivator activate(&loop);

            ...
        } // the previously active event loop restored here
    @endcode

    @library{wxbase}
    @category{appmanagement}

    @see wxEventLoopBase
*/
class wxEventLoopActivator
{
public:
    /**
        Makes the loop passed as the parameter currently active.

        This saves the current return value of wxEventLoopBase::GetActive() and
        then calls wxEventLoopBase::SetActive() with the given @a loop.
     */
    wxEventLoopActivator(wxEventLoopBase *loop);

    /**
        Restores the previously active event loop stored by the constructor.
     */
    ~wxEventLoopActivator();
};
