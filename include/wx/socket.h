/////////////////////////////////////////////////////////////////////////////
// Name:        socket.h
// Purpose:     Socket handling classes
// Author:      Guilhem Lavaux
// Modified by:
// Created:     April 1997
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_NETWORK_SOCKET_H
#define _WX_NETWORK_SOCKET_H

#ifdef __GNUG__
#pragma interface "socket.h"
#endif

#include "wx/defs.h"

#if wxUSE_SOCKETS

// ---------------------------------------------------------------------------
// Windows(tm) specific
// ---------------------------------------------------------------------------
#if defined(__WINDOWS__) && defined(WXSOCK_INTERNAL)
#include <winsock.h>
#include <wx/msw/private.h>
#endif // defined(__WINDOWS__) && defined(WXSOCK_INTERNAL)

// ---------------------------------------------------------------------------
// Unix specific
// ---------------------------------------------------------------------------
#if defined(__UNIX__) && defined(WXSOCK_INTERNAL)
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif // defined(__UNIX__) && defined(WXSOCK_INTERNAL)

// ---------------------------------------------------------------------------
// wxSocket headers (generic)
// ---------------------------------------------------------------------------
#ifdef WXPREC
#include <wx/wxprec.h>
#else
#include <wx/wx.h>
#endif
#include "wx/sckaddr.h"

class WXDLLEXPORT wxSocketEvent;
class WXDLLEXPORT wxSocketHandler;
class wxSocketInternal;
class WXDLLEXPORT wxSocketBase : public wxEvtHandler
{
  DECLARE_CLASS(wxSocketBase)
#ifdef __WXMAC__
  friend void wxMacSocketOnRequestProc(void *refcon , short event) ;
#endif
public:

  enum wxSockFlags { NONE=0, NOWAIT=1, WAITALL=2, SPEED=4 };
  // Type of request
  enum { REQ_READ=0x1, REQ_PEEK=0x2, REQ_WRITE=0x4, REQ_LOST=0x8,
         REQ_ACCEPT=0x10, REQ_CONNECT=0x20, REQ_WAIT=0x40};
  enum { EVT_READ=0, EVT_PEEK=1, EVT_WRITE=2, EVT_LOST=3, EVT_ACCEPT=4,
         EVT_CONNECT=5 };

  typedef int wxRequestNotify;
  typedef int wxRequestEvent;
  enum wxSockType { SOCK_CLIENT, SOCK_SERVER, SOCK_INTERNAL, SOCK_UNINIT }; 
  typedef void (*wxSockCbk)(wxSocketBase& sock,wxRequestEvent evt,char *cdata);

protected:
  wxSockFlags m_flags;
  wxSockType m_type;			// wxSocket type
  bool m_connected, m_connecting;	// State of the socket
  int m_fd;				// Socket file descriptors
  wxList m_states;			// States list
  int m_id;				// Socket id (for event handler)
  wxSocketHandler *m_handler;		// the current socket handler
  wxRequestNotify m_neededreq;		// Specify which requet signals we need
  unsigned long m_timeout;
  size_t m_lcount;			// Last IO request size
  int m_error;				// Last IO error
  wxSocketInternal *m_internal;
  char *m_unread;			// Pushback buffer
  size_t m_unrd_size;			// Pushback buffer size
  wxSockCbk m_cbk;
  char *m_cdata;
  bool m_notify_state;
  
public:
  wxSocketBase();
  virtual ~wxSocketBase();
  virtual bool Close();

  // Base IO
  wxSocketBase& Peek(char* buffer, size_t nbytes);
  wxSocketBase& Read(char* buffer, size_t nbytes);
  wxSocketBase& Write(const char *buffer, size_t nbytes);
  wxSocketBase& Unread(const char *buffer, size_t nbytes);
  wxSocketBase& ReadMsg(char *buffer, size_t nbytes);
  wxSocketBase& WriteMsg(const char *buffer, size_t nbytes);
  void Discard();

  // Try not to use this two methods (they sould be protected)
  void CreatePushbackAfter(const char *buffer, size_t size);
  void CreatePushbackBefore(const char *buffer, size_t size);

  // Status
  inline bool Ok() const { return (m_fd < 0 ? 0 : 1); };
  inline bool Error() const { return (m_error != 0); };
  inline bool IsConnected() const { return m_connected; };
  inline bool IsDisconnected() const { return !IsConnected(); };
  inline bool IsNoWait() const { return m_flags & NOWAIT; };
  bool IsData() const;
  inline size_t LastCount() const { return m_lcount; }
  inline int LastError() const { return m_error; }
  inline wxSockType GetType() const { return m_type; }
  
  void SetFlags(wxSockFlags _flags);
  wxSockFlags GetFlags() const;
  inline void SetTimeout(unsigned long sec) { m_timeout = sec; }

  // seconds = -1 means infinite wait
  // seconds = 0 means no wait
  // seconds > 0 means specified wait
  bool Wait(long seconds = -1, long microseconds = 0);
  bool WaitForRead(long seconds = -1, long microseconds = 0);
  bool WaitForWrite(long seconds = -1, long microseconds = 0);
  bool WaitForLost(long seconds = -1, long microseconds = 0);
  
  // Save the current state of Socket
  void SaveState();
  void RestoreState();

  // Setup external callback
  wxSockCbk Callback(wxSockCbk cbk_);
  char *CallbackData(char *data);

  // Setup event handler
  void SetEventHandler(wxEvtHandler& evt_hdlr, int id = -1);
  
  // Method called when it happens something on the socket
  void SetNotify(wxRequestNotify flags);
  virtual void OnRequest(wxRequestEvent req_evt);

  // Public internal callback
  virtual void OldOnNotify(wxRequestEvent WXUNUSED(evt));

  // Some info on the socket...
  virtual bool GetPeer(wxSockAddress& addr_man) const;
  virtual bool GetLocal(wxSockAddress& addr_man) const;

  // Install or uninstall callbacks
  void Notify(bool notify);

  // So you can know what the socket driver is looking for ...
  inline wxRequestNotify NeededReq() const { return m_neededreq; }

  static wxRequestNotify EventToNotify(wxRequestEvent evt);

protected:
  friend class wxSocketServer;
  friend class wxSocketHandler;
  friend class wxSocketInternal;

#ifdef __SALFORDC__
public:
#endif

  wxSocketBase(wxSockFlags flags, wxSockType type);

#ifdef __SALFORDC__
protected:
#endif
  
  bool _Wait(long seconds, long microseconds, int type);
  
  // Set "my" handler
  inline virtual void SetHandler(wxSocketHandler *handler)
         { m_handler = handler; }

  // Pushback library
  size_t GetPushback(char *buffer, size_t size, bool peek);

  // To prevent many read or write on the same socket at the same time
  //   ==> cause strange things :-)
  void WantSpeedBuffer(char *buffer, size_t size, wxRequestEvent req);
  void WantBuffer(char *buffer, size_t size, wxRequestEvent req);
};

////////////////////////////////////////////////////////////////////////

class WXDLLEXPORT wxSocketServer : public wxSocketBase
{
  DECLARE_CLASS(wxSocketServer)
public:

  // 'service' can be a name or a port-number

  wxSocketServer(wxSockAddress& addr_man, wxSockFlags flags = wxSocketBase::NONE);

  wxSocketBase* Accept();
  bool AcceptWith(wxSocketBase& sock);
};

////////////////////////////////////////////////////////////////////////

class WXDLLEXPORT wxSocketClient : public wxSocketBase
{
  DECLARE_CLASS(wxSocketClient)
public:

  wxSocketClient(wxSockFlags flags = wxSocketBase::NONE);
  virtual ~wxSocketClient();

  virtual bool Connect(wxSockAddress& addr_man, bool wait = TRUE);

  bool WaitOnConnect(long seconds = -1, long microseconds = 0);

  virtual void OnRequest(wxRequestEvent flags);
};

////////////////////////////////////////////////////////////////////////

class WXDLLEXPORT wxSocketHandler : public wxObject
{
  DECLARE_CLASS(wxSocketHandler)
protected:
  wxList *socks;

public:
  enum SockStatus { SOCK_NONE, SOCK_DATA, SOCK_CONNECT, SOCK_DISCONNECT,
		    SOCK_ERROR };
  static wxSocketHandler *master;

  wxSocketHandler();
  virtual ~wxSocketHandler();
  
  void Register(wxSocketBase* sock);
  void UnRegister(wxSocketBase* sock);
  
  unsigned long Count() const;
  
  // seconds = -1 means indefinite wait
  // seconds = 0 means no wait
  // seconds > 0 means specified wait
  
  int Wait(long seconds = -1, long microseconds = 0);
  void YieldSock();
  
  wxSocketServer *CreateServer
                       (wxSockAddress& addr,
			wxSocketBase::wxSockFlags flags = wxSocketBase::NONE);
  wxSocketClient *CreateClient
                       (wxSocketBase::wxSockFlags flags = wxSocketBase::NONE);

  // Create or reuse a socket handler
  static wxSocketHandler& Master() { return *master; }
};

class WXDLLEXPORT wxSocketEvent : public wxEvent {
  DECLARE_DYNAMIC_CLASS(wxSocketEvent)
public:
  wxSocketEvent(int id = 0);

  wxSocketBase::wxRequestEvent SocketEvent() const { return m_skevt; }
  wxSocketBase *Socket() const { return m_socket; }

  void CopyObject(wxObject& obj_d) const;

public:
  wxSocketBase::wxRequestEvent m_skevt;
  wxSocketBase *m_socket;
};

typedef void (wxEvtHandler::*wxSocketEventFunction)(wxSocketEvent&);

#define EVT_SOCKET(id, func) { wxEVT_SOCKET, id, -1, \
  (wxObjectEventFunction) (wxEventFunction) (wxSocketEventFunction) & func, \
  (wxObject *) NULL  },

#endif
  // wxUSE_SOCKETS

#endif
  // _WX_NETWORK_SOCKET_H
