/////////////////////////////////////////////////////////////////////////////
// Name:        src/unix/sockunix.cpp
// Purpose:     wxSocketImpl implementation for Unix systems
// Authors:     Guilhem Lavaux, Guillermo Rodriguez Garcia, David Elliott,
//              Vadim Zeitlin
// Created:     April 1997
// RCS-ID:      $Id$
// Copyright:   (c) 1997 Guilhem Lavaux
//              (c) 2008 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#include "wx/wxprec.h"

#if wxUSE_SOCKETS

#include "wx/private/fd.h"
#include "wx/private/socket.h"
#include "wx/unix/private/sockunix.h"

#include <errno.h>

#if defined(__WATCOMC__)
    #include <nerrno.h>
#endif

#include <sys/types.h>

#ifdef HAVE_SYS_SELECT_H
#   include <sys/select.h>
#endif

#ifdef __EMX__
    #include <sys/select.h>
#endif

#ifndef WX_SOCKLEN_T

#ifdef VMS
#  define WX_SOCKLEN_T unsigned int
#else
#  ifdef __GLIBC__
#    if __GLIBC__ == 2
#      define WX_SOCKLEN_T socklen_t
#    endif
#  elif defined(__WXMAC__)
#    define WX_SOCKLEN_T socklen_t
#  else
#    define WX_SOCKLEN_T int
#  endif
#endif

#endif /* SOCKLEN_T */

#ifndef SOCKOPTLEN_T
    #define SOCKOPTLEN_T WX_SOCKLEN_T
#endif

// UnixWare reportedly needs this for FIONBIO definition
#ifdef __UNIXWARE__
    #include <sys/filio.h>
#endif

// ============================================================================
// wxSocketImpl implementation
// ============================================================================

/* static */
wxSocketImpl *wxSocketImpl::Create(wxSocketBase& wxsocket)
{
    return new wxSocketImplUnix(wxsocket);
}


wxSocketError wxSocketImplUnix::GetLastError() const
{
    switch ( errno )
    {
        case 0:
            return wxSOCKET_NOERROR;

        case ENOTSOCK:
            return wxSOCKET_INVSOCK;

        // unfortunately EAGAIN only has the "would block" meaning for read(),
        // not for connect() for which it means something rather different but
        // we can't distinguish between these two situations currently...
        //
        // also notice that EWOULDBLOCK can be different from EAGAIN on some
        // systems (HP-UX being the only known example) while it's defined as
        // EAGAIN on most others (e.g. Linux)
        case EAGAIN:
#ifdef EWOULDBLOCK
    #if EWOULDBLOCK != EAGAIN
        case EWOULDBLOCK:
    #endif
#endif // EWOULDBLOCK
        case EINPROGRESS:
            return wxSOCKET_WOULDBLOCK;

        default:
            return wxSOCKET_IOERR;
    }
}

void wxSocketImplUnix::DoEnableEvents(bool flag)
{
    wxSocketManager * const manager = wxSocketManager::Get();
    if ( flag )
    {
        manager->Install_Callback(this, wxSOCKET_INPUT);
        manager->Install_Callback(this, wxSOCKET_OUTPUT);
    }
    else // off
    {
        manager->Uninstall_Callback(this, wxSOCKET_INPUT);
        manager->Uninstall_Callback(this, wxSOCKET_OUTPUT);
    }
}


void wxSocketImplUnix::OnStateChange(wxSocketNotify event)
{
    NotifyOnStateChange(event);

    if ( event == wxSOCKET_LOST )
        Shutdown();
}

void wxSocketImplUnix::OnReadWaiting()
{
  char c;

  if (m_fd == INVALID_SOCKET)
  {
    return;
  }

  int num =  recv(m_fd, &c, 1, MSG_PEEK);

  if (num > 0)
  {
    OnStateChange(wxSOCKET_INPUT);
  }
  else
  {
    if (m_server && m_stream)
    {
      OnStateChange(wxSOCKET_CONNECTION);
    }
    else if (num == 0)
    {
      if (m_stream)
      {
        /* graceful shutdown */
        OnStateChange(wxSOCKET_LOST);
      }
      else
      {
        /* Empty datagram received */
        OnStateChange(wxSOCKET_INPUT);
      }
    }
    else
    {
      /* Do not throw a lost event in cases where the socket isn't really lost */
      if ((errno == EWOULDBLOCK) || (errno == EAGAIN) || (errno == EINTR))
      {
        OnStateChange(wxSOCKET_INPUT);
      }
      else
      {
        OnStateChange(wxSOCKET_LOST);
      }
    }
  }
}

void wxSocketImplUnix::OnWriteWaiting()
{
  if (m_establishing && !m_server)
  {
    int error;
    SOCKOPTLEN_T len = sizeof(error);

    m_establishing = false;

    getsockopt(m_fd, SOL_SOCKET, SO_ERROR, (char*)&error, &len);

    if (error)
    {
      OnStateChange(wxSOCKET_LOST);
    }
    else
    {
      OnStateChange(wxSOCKET_CONNECTION);
      /* We have to fire this event by hand because CONNECTION (for clients)
       * and OUTPUT are internally the same and we just disabled CONNECTION
       * events with the above macro.
       */
      OnStateChange(wxSOCKET_OUTPUT);
    }
  }
  else
  {
    OnStateChange(wxSOCKET_OUTPUT);
  }
}

void wxSocketImplUnix::OnExceptionWaiting()
{
    wxFAIL_MSG( "not supposed to be called" );
}

#endif  /* wxUSE_SOCKETS */
