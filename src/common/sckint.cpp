///////////////////////////////////////////////////////////////////////////////
// Name:       socket.cpp
// Purpose:    Socket handler classes
// Authors:    Guilhem Lavaux (completely rewritten from a basic API of Andrew
//             Davidson(1995) in wxWeb)
// Created:    April 1997
// Updated:    April 1999
// Copyright:  (C) 1999, 1998, 1997, Guilhem Lavaux
// RCS_ID:     $Id$
// License:    see wxWindows license
///////////////////////////////////////////////////////////////////////////////
#ifdef __GNUG__
#pragma implementation "sckint.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_SOCKETS

#define WXSOCK_INTERNAL
#include <wx/object.h>
#include <wx/list.h>
#include <wx/socket.h>
#include <wx/thread.h>
#include <wx/sckint.h>

#ifndef __WXSTUBS__

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// -----------------------
// System specific headers
// -----------------------

#ifdef __WXMAC__
// in order to avoid problems with our c library and double definitions
#define close closesocket
#define ioctl ioctlsocket

#include <wx/mac/macsock.h>
#endif

#if defined(__WINDOWS__)
#include <winsock.h>
#endif // __WINDOWS__

#if defined(__UNIX__)

#ifdef VMS
#include <socket.h>
#else
#include <sys/socket.h>
#endif
#include <sys/ioctl.h>

#include <sys/time.h>
#include <unistd.h>

#ifdef sun
#include <sys/filio.h>
#endif

#endif // __UNIX__

#include <signal.h>
#include <errno.h>

#ifdef __VISUALC__
#include <io.h>
#endif

// Constants
#define READ_MASK wxSocketBase::REQ_READ | wxSocketBase::REQ_ACCEPT | wxSocketBase::REQ_LOST
#define WRITE_MASK wxSocketBase::REQ_WRITE | wxSocketBase::REQ_CONNECT

// --------------------------------------------------------------
// --------- SocketWaiter ---------------------------------------
// --------------------------------------------------------------

SocketWaiter::SocketWaiter(wxSocketBase *socket,
				 wxSocketInternal *internal)
  : m_socket(socket), m_internal(internal), m_fd(internal->GetFD())
{
}

SocketWaiter::~SocketWaiter()
{
}

void SocketWaiter::ProcessReadEvent()
{
  int ret;
  char c;

  ret = recv(m_fd, &c, 1, MSG_PEEK);
  
  // We are a server => emit a EVT_ACCEPT event.
  if (ret == -1 && m_socket->GetType() == wxSocketBase::SOCK_SERVER) {
    m_socket->OnRequest(wxSocketBase::EVT_ACCEPT);
    return;
  }
  
  // Else, no error => there is something to be read else
  // we've lost the connection.
  if (ret > 0)
    m_socket->OnRequest(wxSocketBase::EVT_READ);
  else {
    m_socket->OnRequest(wxSocketBase::EVT_LOST);
    Exit(NULL);
  }
}

void SocketWaiter::ProcessWriteEvent()
{
  if (m_socket->IsConnected())
    m_socket->OnRequest(wxSocketBase::EVT_CONNECT);
  else
    m_socket->OnRequest(wxSocketBase::EVT_WRITE);
}

void *SocketWaiter::Entry()
{
  struct timeval tv;
  fd_set sockrd_set, sockwr_set;
  wxSocketEvent event;
  int ret;

  while (1) {
    // We won't wait.
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    FD_ZERO(&sockrd_set);
    FD_ZERO(&sockwr_set);
    
    if ((m_socket->NeededReq() & READ_MASK) != 0)
      FD_SET(m_fd, &sockrd_set);
    if ((m_socket->NeededReq() & WRITE_MASK) != 0)
      FD_SET(m_fd, &sockwr_set);

    m_internal->AcquireFD();
    ret = select(FD_SETSIZE, &sockrd_set, &sockwr_set, NULL, &tv);
    m_internal->ReleaseFD();

    if (FD_ISSET(m_fd, &sockrd_set))
      ProcessReadEvent();

    if (FD_ISSET(m_fd, &sockwr_set))
      ProcessWriteEvent();

    if (ret == 0)
      // If nothing happened, we wait for 100 ms.
      wxThread::Sleep(10);
    else
      wxThread::Yield();

    // Check whether we should exit.
    if (TestDestroy())
      return NULL;
 }
  return NULL;
}

// --------------------------------------------------------------
// --------- SocketRequester ------------------------------------
// --------------------------------------------------------------

SocketRequester::SocketRequester(wxSocketBase *socket,
				 wxSocketInternal *internal)
  : m_socket(socket), m_internal(internal), m_fd(internal->GetFD())
{
}

SocketRequester::~SocketRequester()
{
}

bool SocketRequester::WaitFor(wxSocketBase::wxRequestNotify req, int millisec)
{
  int ret;
  struct timeval tv;
  fd_set sockrd_set, sockwr_set;

  // We won't wait.
  tv.tv_sec = millisec / 1000;
  tv.tv_usec = (millisec % 1000) * 1000;

  if ((req & READ_MASK) != 0)  
    FD_ZERO(&sockrd_set);
  FD_ZERO(&sockwr_set);

  FD_SET(m_fd, &sockrd_set);
  FD_SET(m_fd, &sockwr_set);
  
  m_internal->AcquireFD();
  ret = select(FD_SETSIZE, &sockrd_set, &sockwr_set, NULL, &tv);
  m_internal->ReleaseFD();

  return (ret != 0);
}

void SocketRequester::ProcessReadEvent(SockRequest *req)
{
  int ret;
  size_t len;

  // We'll wait for the first byte, in case a "timeout event" occurs it returns   // immediately
  if (!WaitFor(wxSocketBase::REQ_READ, req->timeout)) {
    m_internal->EndRequest(req);
    return;
  }

  m_internal->AcquireFD();
  ret = recv(m_fd, req->buffer, req->size,
	     (req->type == wxSocketBase::REQ_PEEK) ? MSG_PEEK : 0);
  m_internal->ReleaseFD();

  // An error occured, we exit.
  if (ret < 0) {
    req->error = errno;
    m_internal->EndRequest(req);
    return;
  }
  len = ret;

  // If the buffer isn't full (and we want it to be full), we don't unqueue it.
  if ((len < req->size) && (m_socket->GetFlags() & wxSocketBase::WAITALL)) {
    req->size -= len;
    req->io_nbytes += len;
    req->buffer += len;
    return;
  }
  // The End.
  req->io_nbytes += len;
  m_internal->EndRequest(req);
}

void SocketRequester::ProcessWriteEvent(SockRequest *req)
{
  int ret;
  size_t len;

  m_internal->AcquireFD();
  ret = send(m_fd, req->buffer, req->size, 0);
  m_internal->ReleaseFD();
  if (ret < 0) {
    req->error = errno;
    m_internal->EndRequest(req);
    return;
  }
  len = ret;
  if ((len < req->size) && ((m_socket->GetFlags() & wxSocketBase::WAITALL) != 0)) {
    req->size -= len;
    req->io_nbytes += len;
    req->buffer += len;
    return;
  }
  req->io_nbytes += len;
  m_internal->EndRequest(req);
}

void SocketRequester::ProcessWaitEvent(SockRequest *req)
{
  if (WaitFor(req->type, req->timeout))
    req->io_nbytes = 1; // We put 1 in the counter to tell the requester
                        // there is no timeout.
  else
    req->io_nbytes = 0;

  m_internal->EndRequest(req);
}

void *SocketRequester::Entry()
{
  SockRequest *req;

  while (1) {
    // Wait for a new request or a destroy message.
    req = m_internal->WaitForReq();
    if (TestDestroy() || req == NULL)
      return NULL;

    if ((req->type & wxSocketBase::REQ_WAIT) != 0) {
      ProcessWaitEvent(req);
      continue;
    }

    switch (req->type) {
    case wxSocketBase::REQ_READ:
    case wxSocketBase::REQ_PEEK:
      ProcessReadEvent(req);
      break;
    case wxSocketBase::REQ_WRITE:
      ProcessWriteEvent(req);
      break;
    }
  }
  return NULL;
}

// --------------------------------------------------------------
// --------- wxSocketInternal -----------------------------------
// --------------------------------------------------------------

wxSocketInternal::wxSocketInternal(wxSocketBase *socket)
{
  m_socket = socket;
  m_thread_waiter = new SocketWaiter(socket, this);
  m_thread_requester = new SocketRequester(socket, this);
  m_request_locker.Lock();
}

wxSocketInternal::~wxSocketInternal()
{
  wxASSERT(!m_finalized);
  m_request_locker.Unlock();
  delete m_thread_waiter;
  delete m_thread_requester;
}

// ----------------------------------------------------------------------
// WaitForReq: it is called by SocketRequester and should return the next
// socket request if available
// ----------------------------------------------------------------------
SockRequest *wxSocketInternal::WaitForReq()
{
  wxNode *node;

  node = m_requests.First();
  if (node == NULL) {
    m_socket_cond.Wait(m_request_locker);

    node = m_requests.First();
    if (node == NULL)
      return NULL;
  }

  return (SockRequest *)node->Data();
}

// ----------------------------------------------------------------------
// EndRequest: Should be called to finalize a request
// ----------------------------------------------------------------------
void wxSocketInternal::EndRequest(SockRequest *req)
{
  wxNode *node = NULL;

  req->done = TRUE;

  node = m_requests.Member((wxObject *)req);
  if (node != NULL)
    delete node;
}

void wxSocketInternal::AcquireFD()
{
  m_fd_locker.Lock();
}

void wxSocketInternal::ReleaseFD()
{
  m_fd_locker.Unlock();
}

// ----------------------------------------------------------------------
// InitializeSocket: called by wxSocketBase to initialize the daemons with
// a new file descriptor and to create them
// ----------------------------------------------------------------------
void wxSocketInternal::InitializeSocket()
{
  wxASSERT( ((m_thread_waiter->IsAlive() && !m_thread_waiter->IsPaused()) ||
	     (m_thread_requester->IsAlive() && !m_thread_requester->IsPaused())));

  m_thread_waiter->m_fd = m_socket->m_fd;
  m_thread_requester->m_fd = m_socket->m_fd;

  if (m_thread_waiter->IsPaused())
    ResumeSocket();
  else {

    if (m_thread_waiter->Create() != wxTHREAD_NO_ERROR) {
      // Something should be done here.
      return;
    }
    
    if (m_thread_requester->Create() != wxTHREAD_NO_ERROR) {
      // Something should be done here.
      return;
    }
 
    m_thread_waiter->Run();
    m_thread_requester->Run();
  }
}


// ----------------------------------------------------------------------
// InitializeSocket: called by wxSocketBase to destroy daemons
// ----------------------------------------------------------------------
void wxSocketInternal::FinalizeSocket()
{
  wxASSERT( (!m_thread_waiter->IsAlive() && !m_thread_requester->IsAlive()) );

  ResumeSocket();

  m_thread_waiter->Delete();
  m_socket_locker.Lock();
  if (m_requests.Number() == 0)
    m_socket_cond.Signal();
  m_socket_locker.Unlock();

  m_thread_requester->Delete();
}

void wxSocketInternal::PauseSocket()
{
  if (m_thread_waiter != NULL && !m_thread_waiter->IsPaused())
    m_thread_waiter->Pause();
}

void wxSocketInternal::ResumeSocket()
{
  if (m_thread_waiter != NULL && m_thread_waiter->IsPaused())
    m_thread_waiter->Resume();
}

void wxSocketInternal::EnableWaiter()
{
  if (m_thread_waiter != NULL && m_thread_waiter->IsPaused())
    m_thread_waiter->Resume();
}

void wxSocketInternal::DisableWaiter()
{
  if (m_thread_waiter != NULL && !m_thread_waiter->IsPaused())
    m_thread_waiter->Pause();
}

// ----------------------------------------------------------------------
// QueueRequest:
// ----------------------------------------------------------------------
void wxSocketInternal::QueueRequest(SockRequest *request, bool async)
{
  if (async) {
    m_request_locker.Lock();
    request->done = FALSE;
    m_requests.Append((wxObject *)request);
    m_request_locker.Unlock();
    
    // Wake up
    m_socket_cond.Signal();
    
    if (request->wait) {
      if (wxThread::IsMain())
        while (!request->done) {
	  wxYield();
        }
      else
        while (!request->done) {
          wxThread::Yield();
        }
    }

  } else {
    m_request_locker.Lock();

    if ((request->type & wxSocketBase::REQ_WAIT) != 0) {
      m_thread_requester->ProcessWaitEvent(request);
    } else {

      request->done = FALSE;

      switch (request->type) {
      case wxSocketBase::REQ_PEEK:
      case wxSocketBase::REQ_READ:
        m_thread_requester->ProcessReadEvent(request);
        break;
      case wxSocketBase::REQ_WRITE:
        m_thread_requester->ProcessWriteEvent(request);
        break;
      }
    }
    request->done = TRUE;
    m_request_locker.Unlock();
  }
}

void wxSocketInternal::WaitForEnd(SockRequest *request)
{
  // TODOTODO
}

#endif
  // __WXSTUBS__

#endif
  // wxUSE_SOCKETS
