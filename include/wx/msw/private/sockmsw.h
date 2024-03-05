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
    #ifdef __LP64__
        // We can't use long in this case because it is 64 bits with Cygwin, so
        // use their special type used for working around this instead.
        #define wxIoctlSocketArg_t __ms_u_long
    #endif
#endif

#ifndef wxIoctlSocketArg_t
    #define wxIoctlSocketArg_t u_long
#endif

#define wxCloseSocket closesocket

// ----------------------------------------------------------------------------
// MSW-specific socket implementation
// ----------------------------------------------------------------------------

class wxSocketImplMSW : public wxSocketImpl
{
public:
    wxSocketImplMSW(wxSocketBase& wxsocket);

    virtual ~wxSocketImplMSW();

    virtual wxSocketError GetLastError() const override;

    virtual void ReenableEvents(wxSocketEventFlags WXUNUSED(flags)) override
    {
        // notifications are never disabled in this implementation, there is no
        // need for this as WSAAsyncSelect() only sends notification once when
        // the new data becomes available anyhow, so there is no need to do
        // anything here
    }

    virtual void UpdateBlockingState() override
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
            wxIoctlSocketArg_t trueArg = 1;
            ioctlsocket(m_fd, FIONBIO, &trueArg);

            // Uninstall it in case it was installed before.
            wxSocketManager::Get()->Uninstall_Callback(this);
        }
        else
        {
            // No need to make the socket non-blocking, Install_Callback() will
            // do it as a side effect of calling WSAAsyncSelect().
            wxSocketManager::Get()->Install_Callback(this);
        }
    }

private:
    virtual void DoClose() override;

    int m_msgnumber;

    friend class wxSocketMSWManager;

    wxDECLARE_NO_COPY_CLASS(wxSocketImplMSW);
};

#endif  /* _WX_MSW_GSOCKMSW_H_ */
