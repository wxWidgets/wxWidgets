/////////////////////////////////////////////////////////////////////////////
// Name:        sckint.h
// Purpose:     Socket internal classes
// Author:      Guilhem Lavaux
// Modified by:
// Created:     April 1999
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#ifndef _WX_NETWORK_SOCKET_INT_H
#define _WX_NETWORK_SOCKET_INT_H

#ifdef __GNUG__
#pragma interface
#endif

#if wxUSE_SOCKETS

#include <wx/object.h>
#include <wx/list.h>
#include <wx/socket.h>
#include <wx/thread.h>

// Socket state
class SocketState
{
 public:
  // TRUE if the background notifyier is on.
  bool notify_state;
  // Specifies which events we want to be notified.
  wxSocketBase::wxRequestNotify evt_notify_state;
  // Socket flags.
  wxSocketBase::wxSockFlags socket_flags;
  // Pointer to the C callback function. 
  wxSocketBase::wxSockCbk c_callback;
  char *c_callback_data;
};

// Socket request
class SockRequest 
{
 public:
  // Buffer where to get/put data.
  char *buffer;
  // Size of the buffer.
  size_t size;
  // Number of bytes really read/written.
  size_t io_nbytes;
  // Error.
  unsigned int error;
  // Type of the request.
  wxSocketBase::wxRequestNotify type;
  // Timeout (in milliseconds).
  unsigned int timeout;
  // TRUE if the buffer has been processed.
  bool done;
  // TRUE if we must wait for the request completion, in the other case an
  // event will be sent to the main thread when the request is finished.
  bool wait;
};

class wxSocketInternal;
class SocketWaiter: public wxThread {
 public:
  SocketWaiter(wxSocketBase *socket, wxSocketInternal *internal);
  ~SocketWaiter();

  // Thread Entry point
  // ---
  virtual void *Entry();

 protected:
  void ProcessReadEvent();
  void ProcessWriteEvent();

 public:
  wxSocketBase *m_socket;
  wxSocketInternal *m_internal;
  int m_fd;
};

class SocketRequester: public wxThread {
 public:
  SocketRequester(wxSocketBase *socket, wxSocketInternal *internal);
  ~SocketRequester();

  void ProcessWaitEvent(SockRequest *req);
  void ProcessReadEvent(SockRequest *req);
  void ProcessWriteEvent(SockRequest *req);

  bool WaitFor(wxSocketBase::wxRequestNotify req, int millisec);

  // Thread Entry point
  // ---
  virtual void *Entry();

 public:
  wxSocketBase *m_socket;
  wxSocketInternal *m_internal;
  int m_fd;
};

class wxSocketInternal {
 public:
  wxSocketInternal(wxSocketBase *socket);
  ~wxSocketInternal();

  // wxSocket thread manager
  // -----------------------
  void AcquireData();
  void ReleaseData();
  void AcquireFD();
  void ReleaseFD();

  int GetFD() { return m_fd; }
  
  void ResumeWaiter();
  void StopWaiter();
  void ResumeRequester();
  void StopRequester();

  void QueueRequest(SockRequest *request, bool async);
  void WaitForEnd(SockRequest *request);

  SockRequest *WaitForReq();
  void EndRequest(SockRequest *req);
 public:
  wxMutex m_socket_locker, m_fd_locker, m_request_locker, m_end_requester;
  wxCondition m_socket_cond;
  wxSocketBase *m_socket;
  SocketWaiter *m_thread_waiter;
  SocketRequester *m_thread_requester;
  wxList m_requests;
  int m_fd;
  bool m_invalid_requester;
};

#endif
  // wxUSE_SOCKETS

#endif
  // _WX_NETWORK_SOCKET_INT_H
