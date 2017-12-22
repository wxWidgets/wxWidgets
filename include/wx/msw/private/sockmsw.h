/////////////////////////////////////////////////////////////////////////////
// Name:       wx/msw/private/gsockmsw.h
// Purpose:    MSW-specific socket implementation
// Authors:    Guilhem Lavaux, Guillermo Rodriguez Garcia, Vadim Zeitlin
// Created:    April 1997
// Copyright:  (C) 1999-1997, Guilhem Lavaux
//             (C) 1999-2000, Guillermo Rodriguez Garcia
//             (C) 2008 Vadim Zeitlin
// Licence:    wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef _WX_MSW_GSOCKMSW_H_
#define _WX_MSW_GSOCKMSW_H_

#include "wx/msw/wrapwin.h"

#if defined(__CYGWIN__)
    //CYGWIN gives annoying warning about runtime stuff if we don't do this
#   define USE_SYS_TYPES_FD_SET
#   include <sys/types.h>
#endif

#if defined(__CYGWIN__)
    #include <winsock.h>
#endif

// ----------------------------------------------------------------------------
// MSW-specific socket implementation
// ----------------------------------------------------------------------------

class wxSocketImplMSW : public wxSocketImpl
{
public:
    wxSocketImplMSW(wxSocketBase& wxsocket);

    virtual ~wxSocketImplMSW();

    virtual wxSocketError GetLastError() const wxOVERRIDE;

    virtual void ReenableEvents(wxSocketEventFlags WXUNUSED(flags)) wxOVERRIDE
    {
        // notifications are never disabled in this implementation, there is no
        // need for this as WSAAsyncSelect() only sends notification once when
        // the new data becomes available anyhow, so there is no need to do
        // anything here
    }

private:
    virtual void DoClose() wxOVERRIDE;

    virtual void UnblockAndRegisterWithEventLoop() wxOVERRIDE
    {
        if ( GetSocketFlags() & wxSOCKET_BLOCK )
        {
            // Counter-intuitively, we make the socket non-blocking even in
            // this case as it is necessary e.g. for Read() to return
            // immediately if there is no data available. However we must not
            // install a callback for it as blocking sockets don't use any
            // events and generating them would actually be harmful (and not
            // just useless) as they would be dispatched by the main thread
            // while this blocking socket can be used from a worker one, so it
            // would result in data races and other unpleasantness.
            unsigned long trueArg = 1;
            ioctlsocket(m_fd, FIONBIO, &trueArg);
        }
        else
        {
            // No need to make the socket non-blocking, Install_Callback() will
            // do it as a side effect of calling WSAAsyncSelect().
            wxSocketManager::Get()->Install_Callback(this);
        }
    }

    int m_msgnumber;

    friend class wxSocketMSWManager;

    wxDECLARE_NO_COPY_CLASS(wxSocketImplMSW);
};

#endif  /* _WX_MSW_GSOCKMSW_H_ */
