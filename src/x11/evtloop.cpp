///////////////////////////////////////////////////////////////////////////////
// Name:        x11/evtloop.cpp
// Purpose:     implements wxEventLoop for X11
// Author:      Julian Smart
// Modified by:
// Created:     01.06.01
// RCS-ID:      $Id$
// Copyright:   (c) 2002 Julian Smart
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "evtloop.h"
#endif

#include "wx/window.h"
#include "wx/app.h"
#include "wx/evtloop.h"
#include "wx/tooltip.h"
#if wxUSE_THREADS
#include "wx/thread.h"
#endif
#include "wx/timer.h"
#include "wx/hash.h"
#include "wx/module.h"
#include "wx/x11/private.h"
#include "X11/Xlib.h"

#include <sys/time.h>
#include <unistd.h>

#if wxUSE_SOCKETS
// ----------------------------------------------------------------------------
// wxSocketTable
// ----------------------------------------------------------------------------

typedef void (*wxSocketCallback) (int fd, void* data);

class wxSocketTableEntry: public wxObject
{
  public:
    wxSocketTableEntry()
    {
        m_fdInput = -1; m_fdOutput = -1;
        m_callbackInput = NULL; m_callbackOutput = NULL;
        m_dataInput = NULL; m_dataOutput = NULL;
    }

    int m_fdInput;
    int m_fdOutput;
    wxSocketCallback m_callbackInput;
    wxSocketCallback m_callbackOutput;
    void* m_dataInput;
    void* m_dataOutput;
};

typedef enum
{ wxSocketTableInput, wxSocketTableOutput } wxSocketTableType ;

class wxSocketTable: public wxHashTable
{
  public:
    wxSocketTable(): wxHashTable(wxKEY_INTEGER)
    {
    }
    ~wxSocketTable()
    {
        WX_CLEAR_HASH_TABLE(*this)
    }

    wxSocketTableEntry* FindEntry(int fd);

    void RegisterCallback(int fd, wxSocketTableType socketType, wxSocketCallback callback, void* data);

    void UnregisterCallback(int fd, wxSocketTableType socketType);

    bool CallCallback(int fd, wxSocketTableType socketType);

    void FillSets(fd_set* readset, fd_set* writeset, int* highest);

    void ProcessEvents(fd_set* readset, fd_set* writeset);
};

wxSocketTableEntry* wxSocketTable::FindEntry(int fd)
{
    wxSocketTableEntry* entry = (wxSocketTableEntry*) Get(fd);
    return entry;
}

void wxSocketTable::RegisterCallback(int fd, wxSocketTableType socketType, wxSocketCallback callback, void* data)
{
    wxSocketTableEntry* entry = FindEntry(fd);
    if (!entry)
    {
        entry = new wxSocketTableEntry();
        Put(fd, entry);
    }

    if (socketType == wxSocketTableInput)
    {
        entry->m_fdInput = fd;
        entry->m_dataInput = data;
        entry->m_callbackInput = callback;
    }
    else
    {
        entry->m_fdOutput = fd;
        entry->m_dataOutput = data;
        entry->m_callbackOutput = callback;
    }
}

void wxSocketTable::UnregisterCallback(int fd, wxSocketTableType socketType)
{
    wxSocketTableEntry* entry = FindEntry(fd);
    if (entry)
    {
        if (socketType == wxSocketTableInput)
        {
            entry->m_fdInput = -1;
            entry->m_dataInput = NULL;
            entry->m_callbackInput = NULL;
        }
        else
        {
            entry->m_fdOutput = -1;
            entry->m_dataOutput = NULL;
            entry->m_callbackOutput = NULL;
        }
        if (entry->m_fdInput == -1 && entry->m_fdOutput == -1)
        {
            Delete(fd);
            delete entry;
        }
    }
}

bool wxSocketTable::CallCallback(int fd, wxSocketTableType socketType)
{
    wxSocketTableEntry* entry = FindEntry(fd);
    if (entry)
    {
        if (socketType == wxSocketTableInput)
        {
            if (entry->m_fdInput != -1 && entry->m_callbackInput)
            {
                (entry->m_callbackInput) (entry->m_fdInput, entry->m_dataInput);
            }
        }
        else
        {
            if (entry->m_fdOutput != -1 && entry->m_callbackOutput)
            {
                (entry->m_callbackOutput) (entry->m_fdOutput, entry->m_dataOutput);
            }
        }
        return TRUE;
    }
    else
        return FALSE;
}

void wxSocketTable::FillSets(fd_set* readset, fd_set* writeset, int* highest)
{
    BeginFind();
    wxHashTable::compatibility_iterator node = Next();
    while (node)
    {
        wxSocketTableEntry* entry = (wxSocketTableEntry*) node->GetData();

        if (entry->m_fdInput != -1)
        {
            FD_SET(entry->m_fdInput, readset);
            if (entry->m_fdInput > *highest)
                * highest = entry->m_fdInput;
        }

        if (entry->m_fdOutput != -1)
        {
            FD_SET(entry->m_fdOutput, writeset);
            if (entry->m_fdOutput > *highest)
                * highest = entry->m_fdOutput;
        }

        node = Next();
    }
}

void wxSocketTable::ProcessEvents(fd_set* readset, fd_set* writeset)
{
    BeginFind();
    wxHashTable::compatibility_iterator node = Next();
    while (node)
    {
        wxSocketTableEntry* entry = (wxSocketTableEntry*) node->GetData();

        if (entry->m_fdInput != -1 && FD_ISSET(entry->m_fdInput, readset))
        {
            (entry->m_callbackInput) (entry->m_fdInput, entry->m_dataInput);
        }

        if (entry->m_fdOutput != -1 && FD_ISSET(entry->m_fdOutput, writeset))
        {
            (entry->m_callbackOutput) (entry->m_fdOutput, entry->m_dataOutput);
        }

        node = Next();
    }
}

wxSocketTable* wxTheSocketTable = NULL;

class wxSocketTableModule: public wxModule
{
DECLARE_DYNAMIC_CLASS(wxSocketTableModule)
public:
    wxSocketTableModule() {}
    bool OnInit() { wxTheSocketTable = new wxSocketTable; return TRUE; };
    void OnExit() { delete wxTheSocketTable; wxTheSocketTable = NULL; };
};

IMPLEMENT_DYNAMIC_CLASS(wxSocketTableModule, wxModule)

// Implement registration functions as C functions so they
// can be called from gsock11.c

extern "C" void wxRegisterSocketCallback(int fd, wxSocketTableType socketType, wxSocketCallback callback, void* data)
{
    if (wxTheSocketTable)
    {
        wxTheSocketTable->RegisterCallback(fd, socketType, callback, data);
    }
}

extern "C" void wxUnregisterSocketCallback(int fd, wxSocketTableType socketType)
{
    if (wxTheSocketTable)
    {
        wxTheSocketTable->UnregisterCallback(fd, socketType);
    }
}
#endif

// ----------------------------------------------------------------------------
// wxEventLoopImpl
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxEventLoopImpl
{
public:
    // ctor
    wxEventLoopImpl() { SetExitCode(0); m_keepGoing = FALSE; }

    // process an XEvent, return TRUE if it was processed
    bool ProcessEvent(XEvent* event);

    // generate an idle message, return TRUE if more idle time requested
    bool SendIdleEvent();

    // set/get the exit code
    void SetExitCode(int exitcode) { m_exitcode = exitcode; }
    int GetExitCode() const { return m_exitcode; }

public:
    // preprocess an event, return TRUE if processed (i.e. no further
    // dispatching required)
    bool PreProcessEvent(XEvent* event);

    // the exit code of the event loop
    int m_exitcode;

    bool m_keepGoing;
};

// ============================================================================
// wxEventLoopImpl implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxEventLoopImpl message processing
// ----------------------------------------------------------------------------

bool wxEventLoopImpl::ProcessEvent(XEvent *event)
{
    // give us the chance to preprocess the message first
    if ( PreProcessEvent(event) )
        return TRUE;

    // if it wasn't done, dispatch it to the corresponding window
    if (wxTheApp)
        return wxTheApp->ProcessXEvent((WXEvent*) event);

    return FALSE;
}

bool wxEventLoopImpl::PreProcessEvent(XEvent *event)
{
    // TODO
#if 0
    HWND hWnd = msg->hwnd;
    wxWindow *wndThis = wxGetWindowFromHWND((WXHWND)hWnd);


    // try translations first; find the youngest window with a translation
    // table.
    wxWindow *wnd;
    for ( wnd = wndThis; wnd; wnd = wnd->GetParent() )
    {
        if ( wnd->MSWTranslateMessage((WXMSG *)msg) )
            return TRUE;
    }

    // Anyone for a non-translation message? Try youngest descendants first.
    for ( wnd = wndThis; wnd; wnd = wnd->GetParent() )
    {
        if ( wnd->MSWProcessMessage((WXMSG *)msg) )
            return TRUE;
    }
#endif

    return FALSE;
}

// ----------------------------------------------------------------------------
// wxEventLoopImpl idle event processing
// ----------------------------------------------------------------------------

bool wxEventLoopImpl::SendIdleEvent()
{
    return wxTheApp->ProcessIdle();
}

// ============================================================================
// wxEventLoop implementation
// ============================================================================

wxEventLoop *wxEventLoopBase::ms_activeLoop = NULL;

// ----------------------------------------------------------------------------
// wxEventLoop running and exiting
// ----------------------------------------------------------------------------

wxEventLoop::~wxEventLoop()
{
    wxASSERT_MSG( !m_impl, _T("should have been deleted in Run()") );
}

int wxEventLoop::Run()
{
    // event loops are not recursive, you need to create another loop!
    wxCHECK_MSG( !IsRunning(), -1, _T("can't reenter a message loop") );

    m_impl = new wxEventLoopImpl;

    wxEventLoop *oldLoop = ms_activeLoop;
    ms_activeLoop = this;

    m_impl->m_keepGoing = TRUE;
    while ( m_impl->m_keepGoing )
    {
#if 0 // wxUSE_THREADS
        wxMutexGuiLeaveOrEnter();
#endif // wxUSE_THREADS

        // generate and process idle events for as long as we don't have
        // anything else to do
        while ( ! Pending() )
        {
#if wxUSE_TIMER
            wxTimer::NotifyTimers(); // TODO: is this the correct place for it?
#endif
            if (!m_impl->SendIdleEvent())
            {
#if 0 // wxUSE_THREADS
                // leave the main loop to give other threads a chance to
                // perform their GUI work
                wxMutexGuiLeave();
                wxUsleep(20);
                wxMutexGuiEnter();
#endif
                // Break out of while loop
                break;
            }
        }

        // a message came or no more idle processing to do, sit in Dispatch()
        // waiting for the next message
        if ( !Dispatch() )
        {
            break;
        }
    }

    int exitcode = m_impl->GetExitCode();
    delete m_impl;
    m_impl = NULL;

    ms_activeLoop = oldLoop;

    return exitcode;
}

void wxEventLoop::Exit(int rc)
{
    wxCHECK_RET( IsRunning(), _T("can't call Exit() if not running") );

    m_impl->SetExitCode(rc);
    m_impl->m_keepGoing = FALSE;
}

// ----------------------------------------------------------------------------
// wxEventLoop message processing dispatching
// ----------------------------------------------------------------------------

bool wxEventLoop::Pending() const
{
    XFlush( wxGlobalDisplay() );
    return (XPending( wxGlobalDisplay() ) > 0);
}

bool wxEventLoop::Dispatch()
{
    XEvent event;

    // TODO allowing for threads, as per e.g. wxMSW

    // This now waits until either an X event is received,
    // or the select times out. So we should now process
    // wxTimers in a reasonably timely fashion. However it
    // does also mean that idle processing will happen more
    // often, so we should probably limit idle processing to
    // not be repeated more than every N milliseconds.

    if (XPending( wxGlobalDisplay() ) == 0)
    {
#if wxUSE_NANOX
        GR_TIMEOUT timeout = 10; // Milliseconds
        // Wait for next event, or timeout
        GrGetNextEventTimeout(& event, timeout);

        // Fall through to ProcessEvent.
        // we'll assume that ProcessEvent will just ignore
        // the event if there was a timeout and no event.

#else
        struct timeval tv;
        tv.tv_sec=0;
        tv.tv_usec=10000; // TODO make this configurable
        int fd = ConnectionNumber( wxGlobalDisplay() );

        fd_set readset;
        fd_set writeset;
        int highest = fd;
        FD_ZERO(&readset);
        FD_ZERO(&writeset);

        FD_SET(fd, &readset);

#if wxUSE_SOCKETS
        if (wxTheSocketTable)
            wxTheSocketTable->FillSets( &readset, &writeset, &highest );
#endif

        if (select( highest+1, &readset, &writeset, NULL, &tv ) == 0)
        {
            // Timed out, so no event to process
            return TRUE;
        }
        else
        {
            // An X11 event was pending, so get it
            if (FD_ISSET( fd, &readset ))
                XNextEvent( wxGlobalDisplay(), &event );

#if wxUSE_SOCKETS
            // Check if any socket events were pending,
            // and if so, call their callbacks
            if (wxTheSocketTable)
                wxTheSocketTable->ProcessEvents( &readset, &writeset );
#endif
        }
#endif
    }
    else
    {
        XNextEvent( wxGlobalDisplay(), &event );
    }


    (void) m_impl->ProcessEvent( &event );
    return TRUE;
}

