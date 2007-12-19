///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/apptbase.h
// Purpose:     declaration of wxAppTraits for MSW
// Author:      Vadim Zeitlin
// Modified by:
// Created:     22.06.2003
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_APPTBASE_H_
#define _WX_MSW_APPTBASE_H_

// ----------------------------------------------------------------------------
// wxAppTraits: the MSW version adds extra hooks needed by MSW-only code
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxAppTraits : public wxAppTraitsBase
{
public:
    // wxExecute() support methods
    // ---------------------------

    // called before starting to wait for the child termination, may return
    // some opaque data which will be passed later to AfterChildWaitLoop()
    virtual void *BeforeChildWaitLoop() = 0;

    // process pending Windows messages, even in console app
    virtual void AlwaysYield() = 0;

    // called after starting to wait for the child termination, the parameter
    // is the return value of BeforeChildWaitLoop()
    virtual void AfterChildWaitLoop(void *data) = 0;


    // wxThread helpers
    // ----------------

    // process a message while waiting for a(nother) thread, should return
    // false if and only if we have to exit the application
    virtual bool DoMessageFromThreadWait() = 0;

    // wait for the handle to be signaled, return WAIT_OBJECT_0 if it is or, in
    // the GUI code, WAIT_OBJECT_0 + 1 if a Windows message arrived
    virtual WXDWORD WaitForThread(WXHANDLE hThread) = 0;


    // wxSocket support
    // ----------------

#if wxUSE_SOCKETS
    // this function is used by wxNet library to set the default socket manager
    // to use: doing it like this allows us to keep all socket-related code in
    // wxNet instead of having to pull it in wxBase itself as we'd have to do
    // if we really implemented GSocketManager here
    //
    // we don't take ownership of this pointer, it should have a lifetime
    // greater than that of any socket (e.g. be a pointer to a static object)
    static void SetDefaultSocketManager(GSocketManager *manager)
    {
        ms_manager = manager;
    }

    virtual GSocketManager *GetSocketManager() { return ms_manager; }
#endif // wxUSE_SOCKETS

protected:
    // implementation of WaitForThread() for the console applications which is
    // also used by the GUI code if it doesn't [yet|already} dispatch events
    WXDWORD DoSimpleWaitForThread(WXHANDLE hThread);

    static GSocketManager *ms_manager;
};

#endif // _WX_MSW_APPTBASE_H_

