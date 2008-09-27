/////////////////////////////////////////////////////////////////////////////
// Name:        thread.h
// Purpose:     interface of wxCondition
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxCondition

    wxCondition variables correspond to pthread conditions or to Win32 event
    objects. They may be used in a multithreaded application to wait until the
    given condition becomes @true which happens when the condition becomes signaled.

    For example, if a worker thread is doing some long task and another thread has
    to wait until it is finished, the latter thread will wait on the condition
    object and the worker thread will signal it on exit (this example is not
    perfect because in this particular case it would be much better to just
    wxThread::Wait for the worker thread, but if there are several
    worker threads it already makes much more sense).

    Note that a call to wxCondition::Signal may happen before the
    other thread calls wxCondition::Wait and, just as with the
    pthread conditions, the signal is then lost and so if you want to be sure that
    you don't miss it you must keep the mutex associated with the condition
    initially locked and lock it again before calling
    wxCondition::Signal. Of course, this means that this call is
    going to block until wxCondition::Wait is called by another
    thread.

    @library{wxbase}
    @category{threading}

    @see wxThread, wxMutex
*/
class wxCondition
{
public:
    /**
        Default and only constructor. The @a mutex must be locked by the caller
        before calling Wait() function.
        Use IsOk() to check if the object was successfully
        initialized.
    */
    wxCondition(wxMutex& mutex);

    /**
        Destroys the wxCondition object. The destructor is not virtual so this class
        should not be used polymorphically.
    */
    ~wxCondition();

    /**
        Broadcasts to all waiting threads, waking all of them up. Note that this method
        may be called whether the mutex associated with this condition is locked or
        not.

        @see Signal()
    */
    void Broadcast();

    /**
        Returns @true if the object had been initialized successfully, @false
        if an error occurred.
    */
    bool IsOk() const;

    /**
        Signals the object waking up at most one thread. If several threads are waiting
        on the same condition, the exact thread which is woken up is undefined. If no
        threads are waiting, the signal is lost and the condition would have to be
        signalled again to wake up any thread which may start waiting on it later.
        Note that this method may be called whether the mutex associated with this
        condition is locked or not.

        @see Broadcast()
    */
    void Signal();

    /**
        Waits until the condition is signalled.
        This method atomically releases the lock on the mutex associated with this
        condition (this is why it must be locked prior to calling Wait) and puts the
        thread to sleep until Signal() or
        Broadcast() is called. It then locks the mutex
        again and returns.
        Note that even if Signal() had been called before
        Wait without waking up any thread, the thread would still wait for another one
        and so it is important to ensure that the condition will be signalled after
        Wait or the thread may sleep forever.

        @return Returns wxCOND_NO_ERROR on success, another value if an error
                 occurred.

        @see WaitTimeout()
    */
    wxCondError Wait();

    /**
        Waits until the condition is signalled or the timeout has elapsed.
        This method is identical to Wait() except that it
        returns, with the return code of @c wxCOND_TIMEOUT as soon as the given
        timeout expires.

        @param milliseconds
            Timeout in milliseconds
    */
    wxCondError WaitTimeout(unsigned long milliseconds);
};



/**
    @class wxCriticalSectionLocker

    This is a small helper class to be used with wxCriticalSection
    objects. A wxCriticalSectionLocker enters the critical section in the
    constructor and leaves it in the destructor making it much more difficult to
    forget to leave a critical section (which, in general, will lead to serious
    and difficult to debug problems).

    Example of using it:

    @code
    void Set Foo()
    {
        // gs_critSect is some (global) critical section guarding access to the
        // object "foo"
        wxCriticalSectionLocker locker(gs_critSect);

        if ( ... )
        {
            // do something
            ...

            return;
        }

        // do something else
        ...

        return;
    }
    @endcode

    Without wxCriticalSectionLocker, you would need to remember to manually leave
    the critical section before each @c return.

    @library{wxbase}
    @category{threading}

    @see wxCriticalSection, wxMutexLocker
*/
class wxCriticalSectionLocker
{
public:
    /**
        Constructs a wxCriticalSectionLocker object associated with given
        @a criticalsection and enters it.
    */
    wxCriticalSectionLocker(wxCriticalSection& criticalsection);

    /**
        Destructor leaves the critical section.
    */
    ~wxCriticalSectionLocker();
};



/**
    @class wxThreadHelper

    The wxThreadHelper class is a mix-in class that manages a single background
    thread.  By deriving from wxThreadHelper, a class can implement the thread
    code in its own wxThreadHelper::Entry method
    and easily share data and synchronization objects between the main thread
    and the worker thread.  Doing this prevents the awkward passing of pointers
    that is needed when the original object in the main thread needs to
    synchronize with its worker thread in its own wxThread derived object.

    For example, wxFrame may need to make some calculations
    in a background thread and then display the results of those calculations in
    the main window.

    Ordinarily, a wxThread derived object would be created
    with the calculation code implemented in
    wxThread::Entry.  To access the inputs to the
    calculation, the frame object would often to pass a pointer to itself to the
    thread object.  Similarly, the frame object would hold a pointer to the
    thread object.  Shared data and synchronization objects could be stored in
    either object though the object without the data would have to access the
    data through a pointer.

    However, with wxThreadHelper, the frame object and the thread object are
    treated as the same object.  Shared data and synchronization variables are
    stored in the single object, eliminating a layer of indirection and the
    associated pointers.

    @library{wxbase}
    @category{threading}

    @see wxThread
*/
class wxThreadHelper
{
public:
    /**
        This constructor simply initializes a member variable.
    */
    wxThreadHelper();

    /**
        The destructor frees the resources associated with the thread.
    */
    virtual ~wxThreadHelper();

    /**
        Creates a new thread. The thread object is created in the suspended state, and
        you
        should call @ref wxThread::Run GetThread()-Run to start running
        it.  You may optionally specify the stack size to be allocated to it (Ignored on
        platforms that don't support setting it explicitly, eg. Unix).

        @return One of:
    */
    wxThreadError Create(unsigned int stackSize = 0);

    /**
        This is the entry point of the thread. This function is pure virtual and must
        be implemented by any derived class. The thread execution will start here.
        The returned value is the thread exit code which is only useful for
        joinable threads and is the value returned by
        @ref wxThread::Wait GetThread()-Wait.
        This function is called by wxWidgets itself and should never be called
        directly.
    */
    virtual ExitCode Entry();

    /**
        This is a public function that returns the wxThread object
        associated with the thread.
    */
    wxThread* GetThread() const;

    /**
        wxThread * m_thread
        the actual wxThread object.
    */
};

/**
   Possible critical section types
*/

enum wxCriticalSectionType
{
    wxCRITSEC_DEFAULT,
      /** Recursive critical section under both Windows and Unix */

    wxCRITSEC_NON_RECURSIVE  
      /** Non-recursive critical section under Unix, recursive under Windows */
};

/**
    @class wxCriticalSection

    A critical section object is used for exactly the same purpose as
    a wxMutex. The only difference is that under Windows platform
    critical sections are only visible inside one process, while mutexes may be
    shared among processes, so using critical sections is slightly more
    efficient. The terminology is also slightly different: mutex may be locked
    (or acquired) and unlocked (or released) while critical section is entered
    and left by the program.

    Finally, you should try to use wxCriticalSectionLocker class whenever
    possible instead of directly using wxCriticalSection for the same reasons
    wxMutexLocker is preferrable to wxMutex - please see wxMutex for an example.

    @library{wxbase}
    @category{threading}

    @see wxThread, wxCondition, wxCriticalSectionLocker
*/
class wxCriticalSection
{
public:
    /**
        Default constructor initializes critical section object. By default
        critical sections are recursive under Unix and Windows.
    */
    wxCriticalSection( wxCriticalSectionType critSecType = wxCRITSEC_DEFAULT );

    /**
        Destructor frees the resources.
    */
    ~wxCriticalSection();

    /**
        Enter the critical section (same as locking a mutex). There is no error return
        for this function. After entering the critical section protecting some global
        data the thread running in critical section may safely use/modify it.
    */
    void Enter();

    /**
        Leave the critical section allowing other threads use the global data protected
        by it. There is no error return for this function.
    */
    void Leave();
};

/**
  The possible thread kinds.
*/
enum wxThreadKind
{
    /** Detached thread */
    wxTHREAD_DETACHED,  
    
    /** Joinable thread */
    wxTHREAD_JOINABLE   
};

/**
  The possible thread errors.
*/
enum wxThreadError
{
    /** No error */
    wxTHREAD_NO_ERROR = 0,      
    
    /** No resource left to create a new thread. */
    wxTHREAD_NO_RESOURCE,       
    
    /** The thread is already running. */
    wxTHREAD_RUNNING,          
    
    /** The thread isn't running. */ 
    wxTHREAD_NOT_RUNNING,       
    
    /** Thread we waited for had to be killed. */
    wxTHREAD_KILLED,            
    
    /** Some other error */
    wxTHREAD_MISC_ERROR         
};

/**
   Defines the interval of priority
*/
enum
{
    WXTHREAD_MIN_PRIORITY      = 0u,
    WXTHREAD_DEFAULT_PRIORITY  = 50u,
    WXTHREAD_MAX_PRIORITY      = 100u
};


/**
    @class wxThread

    A thread is basically a path of execution through a program. Threads are
    sometimes called @e light-weight processes, but the fundamental difference
    between threads and processes is that memory spaces of different processes are
    separated while all threads share the same address space.

    While it makes it much easier to share common data between several threads, it
    also makes it much easier to shoot oneself in the foot, so careful use of
    synchronization objects such as mutexes() or @ref wxCriticalSection
    "critical sections" is recommended. In addition, don't create global thread
    objects because they allocate memory in their constructor, which will cause
    problems for the memory checking system.

    @section overview_typeswxthread Types of wxThreads
        There are two types of threads in wxWidgets: @e detached and @e joinable,
        modeled after the the POSIX thread API. This is different from the Win32 API
        where all threads are joinable.
        
        By default wxThreads in wxWidgets use the detached behavior. Detached threads
        delete themselves once they have completed, either by themselves when they
        complete processing or through a call to Delete(), and thus
        must be created on the heap (through the new operator, for example).
        Conversely, joinable threads do not delete themselves when they are done 
        processing and as such are safe to create on the stack. Joinable threads
        also provide the ability for one to get value it returned from Entry()
        through Wait().
        
        You shouldn't hurry to create all the threads joinable, however, because this
        has a disadvantage as well: you @b must Wait() for a joinable thread or the
        system resources used by it will never be freed, and you also must delete the
        corresponding wxThread object yourself if you did not create it on the stack.
        In contrast, detached threads are of the "fire-and-forget" kind: you only have to
        start a detached thread and it will terminate and destroy itself.
        
    @section overview_deletionwxthread wxThread Deletion
        Regardless of whether it has terminated or not, you should call
        Wait() on a joinable thread to release its memory, as outlined in 
        @ref overview_typeswxthread "Types of wxThreads". If you created
        a joinable thread on the heap, remember to delete it manually with the delete
        operator or similar means as only detached threads handle this type of memory
        management.
        
        Since detached threads delete themselves when they are finished processing,
        you should take care when calling a routine on one. If you are certain the
        thread is still running and would like to end it, you may call Delete() 
        to gracefully end it (which implies that the thread will be deleted after 
        that call to Delete()). It should be implied that you should never attempt
        to delete a detached thread with the delete operator or similar means.
        As mentioned, Wait() or Delete() attempts to gracefully terminate a 
        joinable and detached thread, respectively. It does this by waiting until
        the thread in question calls TestDestroy() or ends processing (returns 
        from wxThread::Entry).
        
        Obviously, if the thread does call TestDestroy() and does not end the calling
        thread will come to halt. This is why it is important to call TestDestroy() in
        the Entry() routine of your threads as often as possible.
        As a last resort you can end the thread immediately through Kill(). It is
        strongly recommended that you do not do this, however, as it does not free
        the resources associated with the object (although the wxThread object of
        detached threads will still be deleted) and could leave the C runtime 
        library in an undefined state.
        
    @section overview_secondarythreads wxWidgets Calls in Secondary Threads
        All threads other than the "main application thread" (the one
        wxApp::OnInit or your main function runs in, for example) are considered
        "secondary threads". These include all threads created by Create() or the
        corresponding constructors.
        
        GUI calls, such as those to a wxWindow or wxBitmap are explicitly not safe
        at all in secondary threads and could end your application prematurely.
        This is due to several reasons, including the underlying native API and 
        the fact that wxThread does not run a GUI event loop similar to other APIs
        as MFC.
        
        A workaround for some wxWidgets ports is calling wxMutexGUIEnter()
        before any GUI calls and then calling wxMutexGUILeave() afterwords. However,
        the recommended way is to simply process the GUI calls in the main thread
        through an event that is posted by either wxQueueEvent().
        This does not imply that calls to these classes are thread-safe, however,
        as most wxWidgets classes are not thread-safe, including wxString.
        
    @section overview_pollwxThread Don't Poll a wxThread
        A common problem users experience with wxThread is that in their main thread
        they will check the thread every now and then to see if it has ended through
        IsRunning(), only to find that their application has run into problems 
        because the thread is using the default behavior and has already deleted 
        itself. Naturally, they instead attempt to use joinable threads in place 
        of the previous behavior. However, polling a wxThread for when it has ended
        is in general a bad idea - in fact calling a routine on any running wxThread 
        should be avoided if possible. Instead, find a way to notify yourself when
        the thread has ended.
        
        Usually you only need to notify the main thread, in which case you can
        post an event to it via wxPostEvent() or wxEvtHandler::AddPendingEvent.
        In the case of secondary threads you can call a routine of another class
        when the thread is about to complete processing and/or set the value of 
        a variable, possibly using mutexes() and/or other synchronization means
        if necessary.

    @library{wxbase}
    @category{threading}
    @see wxMutex, wxCondition, wxCriticalSection
*/
class wxThread
{
public:
    /**
        This constructor creates a new detached (default) or joinable C++
        thread object. It does not create or start execution of the real thread --
        for this you should use the Create() and Run() methods.
        
        The possible values for @a kind parameters are:
          - @b wxTHREAD_DETACHED - Creates a detached thread.
          - @b wxTHREAD_JOINABLE - Creates a joinable thread.
    */
    wxThread(wxThreadKind kind = wxTHREAD_DETACHED);

    /**
        The destructor frees the resources associated with the thread. Notice that you
        should never delete a detached thread -- you may only call
        Delete() on it or wait until it terminates (and auto
        destructs) itself. Because the detached threads delete themselves, they can
        only be allocated on the heap.
        Joinable threads should be deleted explicitly. The Delete() and Kill() functions
        will not delete the C++ thread object. It is also safe to allocate them on
        stack.
    */
    virtual ~wxThread();

    /**
        Creates a new thread. The thread object is created in the suspended state,
        and you should call Run() to start running it.  You may optionally
        specify the stack size to be allocated to it (Ignored on platforms that don't
        support setting it explicitly, eg. Unix system without
        @c pthread_attr_setstacksize). If you do not specify the stack size,
        the system's default value is used.
        @b Warning: It is a good idea to explicitly specify a value as systems'
        default values vary from just a couple of KB on some systems (BSD and
        OS/2 systems) to one or several MB (Windows, Solaris, Linux). So, if you
        have a thread that requires more than just a few KB of memory, you will
        have mysterious problems on some platforms but not on the common ones. On the
        other hand, just indicating a large stack size by default will give you
        performance issues on those systems with small default stack since those
        typically use fully committed memory for the stack. On the contrary, if
        use a lot of threads (say several hundred), virtual adress space can get tight
        unless you explicitly specify a smaller amount of thread stack space for each
        thread.

        @return One of:
          - @b wxTHREAD_NO_ERROR - No error.
          - @b wxTHREAD_NO_RESOURCE - There were insufficient resources to create the thread.
          - @b wxTHREAD_NO_RUNNING - The thread is already running
    */
    wxThreadError Create(unsigned int stackSize = 0);

    /**
        Calling Delete() gracefully terminates a
        detached thread, either when the thread calls TestDestroy() or finished
        processing.
        (Note that while this could work on a joinable thread you simply should not
        call this routine on one as afterwards you may not be able to call
        Wait() to free the memory of that thread).
        See @ref overview_deletionwxthread "wxThread deletion" for a broader
        explanation of this routine.
    */
    wxThreadError Delete();

    /**
        This is the entry point of the thread. This function is pure virtual and must
        be implemented by any derived class. The thread execution will start here.
        The returned value is the thread exit code which is only useful for
        joinable threads and is the value returned by Wait().
        This function is called by wxWidgets itself and should never be called
        directly.
    */
    virtual ExitCode Entry();

    /**
        This is a protected function of the wxThread class and thus can only be called
        from a derived class. It also can only be called in the context of this
        thread, i.e. a thread can only exit from itself, not from another thread.
        This function will terminate the OS thread (i.e. stop the associated path of
        execution) and also delete the associated C++ object for detached threads.
        OnExit() will be called just before exiting.
    */
    void Exit(ExitCode exitcode = 0);

    /**
        Returns the number of system CPUs or -1 if the value is unknown.

        @see SetConcurrency()
    */
    static int GetCPUCount();

    /**
        Returns the platform specific thread ID of the current thread as a
        long.  This can be used to uniquely identify threads, even if they are
        not wxThreads.
    */
    static unsigned long GetCurrentId();

    /**
        Gets the thread identifier: this is a platform dependent number that uniquely
        identifies the
        thread throughout the system during its existence (i.e. the thread identifiers
        may be reused).
    */
    unsigned long GetId() const;

    /**
        Gets the priority of the thread, between zero and 100.
        
        The following priorities are defined:
          - @b WXTHREAD_MIN_PRIORITY: 0
          - @b WXTHREAD_DEFAULT_PRIORITY: 50
          - @b WXTHREAD_MAX_PRIORITY: 100
    */
    int GetPriority() const;

    /**
        Returns @true if the thread is alive (i.e. started and not terminating).
        Note that this function can only safely be used with joinable threads, not
        detached ones as the latter delete themselves and so when the real thread is
        no longer alive, it is not possible to call this function because
        the wxThread object no longer exists.
    */
    bool IsAlive() const;

    /**
        Returns @true if the thread is of the detached kind, @false if it is a
        joinable
        one.
    */
    bool IsDetached() const;

    /**
        Returns @true if the calling thread is the main application thread.
    */
    static bool IsMain();

    /**
        Returns @true if the thread is paused.
    */
    bool IsPaused() const;

    /**
        Returns @true if the thread is running.
        This method may only be safely used for joinable threads, see the remark in
        IsAlive().
    */
    bool IsRunning() const;

    /**
        Immediately terminates the target thread. @b This function is dangerous and
        should
        be used with extreme care (and not used at all whenever possible)! The resources
        allocated to the thread will not be freed and the state of the C runtime library
        may become inconsistent. Use Delete() for detached
        threads or Wait() for joinable threads instead.
        For detached threads Kill() will also delete the associated C++ object.
        However this will not happen for joinable threads and this means that you will
        still have to delete the wxThread object yourself to avoid memory leaks.
        In neither case OnExit() of the dying thread will be
        called, so no thread-specific cleanup will be performed.
        This function can only be called from another thread context, i.e. a thread
        cannot kill itself.
        It is also an error to call this function for a thread which is not running or
        paused (in the latter case, the thread will be resumed first) -- if you do it,
        a @b wxTHREAD_NOT_RUNNING error will be returned.
    */
    wxThreadError Kill();

    /**
        Called when the thread exits. This function is called in the context of the
        thread associated with the wxThread object, not in the context of the main
        thread. This function will not be called if the thread was
        @ref Kill() killed.
        This function should never be called directly.
    */
    virtual void OnExit();

    /**
        Suspends the thread. Under some implementations (Win32), the thread is
        suspended immediately, under others it will only be suspended when it calls
        TestDestroy() for the next time (hence, if the
        thread doesn't call it at all, it won't be suspended).
        This function can only be called from another thread context.
    */
    wxThreadError Pause();

    /**
        Resumes a thread suspended by the call to Pause().
        This function can only be called from another thread context.
    */
    wxThreadError Resume();

    /**
        Starts the thread execution. Should be called after
        Create().
        This function can only be called from another thread context.
    */
    wxThreadError Run();

    /**
        Sets the thread concurrency level for this process. This is, roughly, the
        number of threads that the system tries to schedule to run in parallel.
        The value of 0 for @a level may be used to set the default one.
        Returns @true on success or @false otherwise (for example, if this function is
        not implemented for this platform -- currently everything except Solaris).
    */
    static bool SetConcurrency(size_t level);

    /**
        Sets the priority of the thread, between 0 and 100. It can only be set
        after calling Create() but before calling
        Run().

        The following priorities are defined:
          - @b WXTHREAD_MIN_PRIORITY: 0
          - @b WXTHREAD_DEFAULT_PRIORITY: 50
          - @b WXTHREAD_MAX_PRIORITY: 100
    */
    void SetPriority(int priority);

    /**
        Pauses the thread execution for the given amount of time.

        This is the same as wxMilliSleep().
    */
    static void Sleep(unsigned long milliseconds);

    /**
        This function should be called periodically by the thread to ensure that
        calls to Pause() and Delete() will work. If it returns @true, the thread
        should exit as soon as possible. Notice that under some platforms (POSIX),
        implementation of Pause() also relies on this function being called, so
        not calling it would prevent both stopping and suspending thread from working.
    */
    virtual bool TestDestroy();

    /**
        Return the thread object for the calling thread. @NULL is returned if
        the calling thread is the main (GUI) thread, but IsMain() should be used
        to test whether the thread is really the main one because @NULL may also
        be returned for the thread not created with wxThread class. Generally
        speaking, the return value for such a thread is undefined.
    */
    static wxThread* This();

    /**
        Waits for a joinable thread to terminate and returns the value the thread
        returned from Entry() or @c (ExitCode)-1 on error. Notice that, unlike
        Delete() doesn't cancel the thread in any way so the caller waits for as
        long as it takes to the thread to exit.
        You can only Wait() for joinable (not detached) threads.
        This function can only be called from another thread context.
        See @ref overview_deletionwxthread "wxThread deletion" for a broader
        explanation of this routine.
    */
    ExitCode Wait() const;

    /**
        Give the rest of the thread time slice to the system allowing the other
        threads to run.
        Note that using this function is @b strongly discouraged, since in
        many cases it indicates a design weakness of your threading model (as
        does using Sleep functions).
        Threads should use the CPU in an efficient manner, i.e. they should
        do their current work efficiently, then as soon as the work is done block
        on a wakeup event (wxCondition, wxMutex, select(), poll(), ...)
        which will get signalled e.g. by other threads or a user device once further
        thread work is available. Using Yield or Sleep
        indicates polling-type behaviour, since we're fuzzily giving up our timeslice
        and wait until sometime later we'll get reactivated, at which time we
        realize that there isn't really much to do and Yield again...
        The most critical characteristic of Yield is that it's operating system
        specific: there may be scheduler changes which cause your thread to not
        wake up relatively soon again, but instead many seconds later,
        causing huge performance issues for your application. @b with a
        well-behaving, CPU-efficient thread the operating system is likely to properly
        care for its reactivation the moment it needs it, whereas with
        non-deterministic, Yield-using threads all bets are off and the system
        scheduler is free to penalize drastically, and this effect gets worse
        with increasing system load due to less free CPU resources available.
        You may refer to various Linux kernel sched_yield discussions for more
        information.
        See also Sleep().
    */
    static void Yield();
};

/**
    @class wxSemaphore

    wxSemaphore is a counter limiting the number of threads concurrently accessing
    a shared resource. This counter is always between 0 and the maximum value
    specified during the semaphore creation. When the counter is strictly greater
    than 0, a call to wxSemaphore::Wait returns immediately and
    decrements the counter. As soon as it reaches 0, any subsequent calls to
    wxSemaphore::Wait block and only return when the semaphore
    counter becomes strictly positive again as the result of calling
    wxSemaphore::Post which increments the counter.

    In general, semaphores are useful to restrict access to a shared resource
    which can only be accessed by some fixed number of clients at the same time. For
    example, when modeling a hotel reservation system a semaphore with the counter
    equal to the total number of available rooms could be created. Each time a room
    is reserved, the semaphore should be acquired by calling
    wxSemaphore::Wait and each time a room is freed it should be
    released by calling wxSemaphore::Post.

    @library{wxbase}
    @category{threading}
*/
class wxSemaphore
{
public:
    /**
        Specifying a @a maxcount of 0 actually makes wxSemaphore behave as if
        there is no upper limit. If maxcount is 1, the semaphore behaves almost as a
        mutex (but unlike a mutex it can be released by a thread different from the one
        which acquired it).
        @a initialcount is the initial value of the semaphore which must be between
        0 and @a maxcount (if it is not set to 0).
    */
    wxSemaphore(int initialcount = 0, int maxcount = 0);

    /**
        Destructor is not virtual, don't use this class polymorphically.
    */
    ~wxSemaphore();

    /**
        Increments the semaphore count and signals one of the waiting
        threads in an atomic way. Returns wxSEMA_OVERFLOW if the count
        would increase the counter past the maximum.

        @return One of:
    */
    wxSemaError Post();

    /**
        Same as Wait(), but returns immediately.

        @return One of:
    */
    wxSemaError TryWait();

    /**
        Wait indefinitely until the semaphore count becomes strictly positive
        and then decrement it and return.

        @return One of:
    */
    wxSemaError Wait();
};



/**
    @class wxMutexLocker

    This is a small helper class to be used with wxMutex
    objects. A wxMutexLocker acquires a mutex lock in the constructor and releases
    (or unlocks) the mutex in the destructor making it much more difficult to
    forget to release a mutex (which, in general, will promptly lead to serious
    problems). See wxMutex for an example of wxMutexLocker
    usage.

    @library{wxbase}
    @category{threading}

    @see wxMutex, wxCriticalSectionLocker
*/
class wxMutexLocker
{
public:
    /**
        Constructs a wxMutexLocker object associated with mutex and locks it.
        Call IsOk() to check if the mutex was successfully locked.
    */
    wxMutexLocker(wxMutex& mutex);

    /**
        Destructor releases the mutex if it was successfully acquired in the ctor.
    */
    ~wxMutexLocker();

    /**
        Returns @true if mutex was acquired in the constructor, @false otherwise.
    */
    bool IsOk() const;
};


/**
    The possible wxMutex kinds.
*/
enum wxMutexType
{
    /** Normal non-recursive mutex: try to always use this one. */
    wxMUTEX_DEFAULT,   

    /** Recursive mutex: don't use these ones with wxCondition. */
    wxMUTEX_RECURSIVE  
};


/**
    The possible wxMutex errors.
*/
enum wxMutexError
{
    /** The operation completed successfully. */
    wxMUTEX_NO_ERROR = 0,   
    
    /** The mutex hasn't been initialized. */
    wxMUTEX_INVALID,       
    
     /** The mutex is already locked by the calling thread. */ 
    wxMUTEX_DEAD_LOCK,     
    
    /** The mutex is already locked by another thread. */
    wxMUTEX_BUSY,           
    
    /** An attempt to unlock a mutex which is not locked. */
    wxMUTEX_UNLOCKED,       
    
    /** wxMutex::LockTimeout() has timed out. */
    wxMUTEX_TIMEOUT,        
    
    /** Any other error */
    wxMUTEX_MISC_ERROR      
};



/**
    @class wxMutex

    A mutex object is a synchronization object whose state is set to signaled when
    it is not owned by any thread, and nonsignaled when it is owned. Its name comes
    from its usefulness in coordinating mutually-exclusive access to a shared
    resource as only one thread at a time can own a mutex object.

    Mutexes may be recursive in the sense that a thread can lock a mutex which it
    had already locked before (instead of dead locking the entire process in this
    situation by starting to wait on a mutex which will never be released while the
    thread is waiting) but using them is not recommended under Unix and they are
    @b not recursive by default. The reason for this is that recursive
    mutexes are not supported by all Unix flavours and, worse, they cannot be used
    with wxCondition.

    For example, when several threads use the data stored in the linked list,
    modifications to the list should only be allowed to one thread at a time
    because during a new node addition the list integrity is temporarily broken
    (this is also called @e program invariant).

    @code
    // this variable has an "s_" prefix because it is static: seeing an "s_" in
    // a multithreaded program is in general a good sign that you should use a
    // mutex (or a critical section)
    static wxMutex *s_mutexProtectingTheGlobalData;

    // we store some numbers in this global array which is presumably used by
    // several threads simultaneously
    wxArrayInt s_data;

    void MyThread::AddNewNode(int num)
    {
        // ensure that no other thread accesses the list
        s_mutexProtectingTheGlobalList->Lock();

        s_data.Add(num);

        s_mutexProtectingTheGlobalList->Unlock();
    }

    // return true if the given number is greater than all array elements
    bool MyThread::IsGreater(int num)
    {
        // before using the list we must acquire the mutex
        wxMutexLocker lock(s_mutexProtectingTheGlobalData);

        size_t count = s_data.Count();
        for ( size_t n = 0; n < count; n++ )
        {
            if ( s_data[n] > num )
                return false;
        }

        return true;
    }
    @endcode

    Notice how wxMutexLocker was used in the second function to ensure that the
    mutex is unlocked in any case: whether the function returns true or false
    (because the destructor of the local object lock is always called). Using
    this class instead of directly using wxMutex is, in general safer and is
    even more so if your program uses C++ exceptions.

    @library{wxbase}
    @category{threading}

    @see wxThread, wxCondition, wxMutexLocker, wxCriticalSection
*/
class wxMutex
{
public:
    /**
        Default constructor.
    */
    wxMutex(wxMutexType type = wxMUTEX_DEFAULT);

    /**
        Destroys the wxMutex object.
    */
    ~wxMutex();

    /**
        Locks the mutex object. This is equivalent to
        LockTimeout() with infinite timeout.

        @return One of: @c wxMUTEX_NO_ERROR, @c wxMUTEX_DEAD_LOCK.
    */
    wxMutexError Lock();

    /**
        Try to lock the mutex object during the specified time interval.

        @return One of: @c wxMUTEX_NO_ERROR, @c wxMUTEX_DEAD_LOCK, @c wxMUTEX_TIMEOUT.
    */
    wxMutexError LockTimeout(unsigned long msec);

    /**
        Tries to lock the mutex object. If it can't, returns immediately with an error.

        @return One of: @c wxMUTEX_NO_ERROR, @c wxMUTEX_BUSY.
    */
    wxMutexError TryLock();

    /**
        Unlocks the mutex object.

        @return One of: @c wxMUTEX_NO_ERROR, @c wxMUTEX_UNLOCKED.
    */
    wxMutexError Unlock();
};



// ============================================================================
// Global functions/macros
// ============================================================================

/** @ingroup group_funcmacro_thread */
//@{

/**
    This macro declares a (static) critical section object named @a cs if
    @c wxUSE_THREADS is 1 and does nothing if it is 0.

    @header{wx/thread.h}
*/
#define wxCRIT_SECT_DECLARE(cs)

/**
    This macro declares a critical section object named @a cs if
    @c wxUSE_THREADS is 1 and does nothing if it is 0. As it doesn't include
    the @c static keyword (unlike wxCRIT_SECT_DECLARE()), it can be used to
    declare a class or struct member which explains its name.

    @header{wx/thread.h}
*/
#define wxCRIT_SECT_DECLARE_MEMBER(cs)

/**
    This macro creates a wxCriticalSectionLocker named @a name and associated
    with the critical section @a cs if @c wxUSE_THREADS is 1 and does nothing
    if it is 0.

    @header{wx/thread.h}
*/
#define wxCRIT_SECT_LOCKER(name, cs)

/**
    This macro combines wxCRIT_SECT_DECLARE() and wxCRIT_SECT_LOCKER(): it
    creates a static critical section object and also the lock object
    associated with it. Because of this, it can be only used inside a function,
    not at global scope. For example:

    @code
    int IncCount()
    {
        static int s_counter = 0;

        wxCRITICAL_SECTION(counter);

        return ++s_counter;
    }
    @endcode

    Note that this example assumes that the function is called the first time
    from the main thread so that the critical section object is initialized
    correctly by the time other threads start calling it, if this is not the
    case this approach can @b not be used and the critical section must be made
    a global instead.

    @header{wx/thread.h}
*/
#define wxCRITICAL_SECTION(name)

/**
    This macro is equivalent to
    @ref wxCriticalSection::Leave "critical_section.Leave()" if
    @c wxUSE_THREADS is 1 and does nothing if it is 0.

    @header{wx/thread.h}
*/
#define wxLEAVE_CRIT_SECT(critical_section)

/**
    This macro is equivalent to
    @ref wxCriticalSection::Enter "critical_section.Enter()" if
    @c wxUSE_THREADS is 1 and does nothing if it is 0.

    @header{wx/thread.h}
*/
#define wxENTER_CRIT_SECT(critical_section)

/**
    Returns @true if this thread is the main one. Always returns @true if
    @c wxUSE_THREADS is 0.

    @header{wx/thread.h}
*/
bool wxIsMainThread();

/**
    This function must be called when any thread other than the main GUI thread
    wants to get access to the GUI library. This function will block the
    execution of the calling thread until the main thread (or any other thread
    holding the main GUI lock) leaves the GUI library and no other thread will
    enter the GUI library until the calling thread calls wxMutexGuiLeave().

    Typically, these functions are used like this:

    @code
    void MyThread::Foo(void)
    {
        // before doing any GUI calls we must ensure that
        // this thread is the only one doing it!

        wxMutexGuiEnter();

        // Call GUI here:
        my_window-DrawSomething();

        wxMutexGuiLeave();
    }
    @endcode

    This function is only defined on platforms which support preemptive
    threads.

    @note Under GTK, no creation of top-level windows is allowed in any thread
          but the main one.

    @header{wx/thread.h}
*/
void wxMutexGuiEnter();

/**
    This function is only defined on platforms which support preemptive
    threads.

    @see wxMutexGuiEnter()

    @header{wx/thread.h}
*/
void wxMutexGuiLeave();

//@}

